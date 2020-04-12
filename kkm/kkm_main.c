/*
 * Copyright © 2020-2020 Kontain Inc. All rights reserved.
 *
 * Kontain Inc CONFIDENTIAL
 *
 * This file includes unpublished proprietary source code of Kontain Inc. The
 * copyright notice above does not evidence any actual or intended publication
 * of such source code. Disclosure of this source code or any related
 * proprietary information is strictly prohibited without the express written
 * permission of Kontain Inc.
 */

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/anon_inodes.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <asm/cpu_entry_area.h>
#include <asm/desc.h>

#include "kkm.h"
#include "kkm_run.h"
#include "kkm_kontainer.h"
#include "kkm_kontext.h"
#include "kkm_mm.h"
#include "kkm_idt_cache.h"

uint32_t kkm_version = 12;

void kkm_destroy_app(struct kkm *kkm)
{
	kfree(kkm);
}

void kkm_reference_count_init(struct kkm *kkm)
{
	refcount_set(&kkm->reference_count, 1);
}

void kkm_reference_count_up(struct kkm *kkm)
{
	refcount_inc(&kkm->reference_count);
}

void kkm_reference_count_down(struct kkm *kkm)
{
	if (refcount_dec_and_test(&kkm->reference_count)) {
		kkm_destroy_app(kkm);
	}
}

static int kkm_execution_kontext_release(struct inode *inode_p,
					 struct file *file_p)
{
	struct kkm_kontext *kkm_kontext = file_p->private_data;
	int i = 0;
	struct kkm_kontext_mmap_area *kkma = NULL;

	kkm_kontext_cleanup(kkm_kontext);

	for (i = 0; i < KKM_CONTEXT_MAP_PAGE_COUNT; i++) {
		kkma = &kkm_kontext->mmap_area[i];
		if (kkma->kvaddr != 0) {
			free_page(kkma->kvaddr);
			kkma->kvaddr = 0;
		}
	}

	kkm_reference_count_down(kkm_kontext->kkm);
	return 0;
}

struct kkm *kkm_global = NULL;

static long kkm_run(struct kkm_kontext *kkm_kontext)
{
	int ret_val = 0;

	kkm_global = kkm_kontext->kkm;

	ret_val = kkm_kontext_switch_kernel(kkm_kontext);

	return ret_val;
}

static long kkm_to_user(void *dest, void *src, size_t size)
{
	long ret_val = 0;
	if (copy_to_user(dest, src, size)) {
		ret_val = -EFAULT;
	}
	return ret_val;
}

static long kkm_from_user(void *dest, void *src, size_t size)
{
	long ret_val = 0;
	if (copy_from_user(dest, src, size)) {
		ret_val = -EFAULT;
	}
	return ret_val;
}

/*
 * ioctls on execution context anon fd
 * all the copies go directly to/from guest private area
 */
static long kkm_execution_kontext_ioctl(struct file *file_p,
					unsigned int ioctl_type,
					unsigned long arg)
{
	int ret_val = 0;
	struct kkm_kontext *kkm_kontext =
		(struct kkm_kontext *)file_p->private_data;
	struct kkm_guest_area *ga =
		(struct kkm_guest_area *)kkm_kontext->guest_area;

	switch (ioctl_type) {
	case KKM_RUN:
		/* switch to guest payload */
		ret_val = kkm_run(kkm_kontext);
		break;
	case KKM_GET_REGS:
		/* get guest state */
		ret_val = kkm_to_user((void *)arg, &ga->regs,
				      sizeof(struct kkm_regs));
		break;
	case KKM_SET_REGS:
		/* set guest state */
		ret_val = kkm_from_user(&ga->regs, (void *)arg,
					sizeof(struct kkm_regs));
		break;
	case KKM_GET_SREGS:
		/* get guest system registers and segment registers */
		ret_val = kkm_to_user((void *)arg, &ga->sregs,
				      sizeof(struct kkm_sregs));
		break;
	case KKM_SET_SREGS:
		/* set guest system registers and segment registers */
		ret_val = kkm_from_user(&ga->sregs, (void *)arg,
					sizeof(struct kkm_sregs));
		break;
	case KKM_SET_MSRS:
		/* return success */
		break;
	case KKM_GET_FPU:
		/* get guest fpu state */
		ret_val = kkm_to_user((void *)arg, &ga->fpu,
				      sizeof(struct kkm_fpu));
		break;
	case KKM_SET_FPU:
		/* set guest fpu state */
		ret_val = kkm_from_user(&ga->fpu, (void *)arg,
					sizeof(struct kkm_fpu));
		break;
	case KKM_SET_CPUID:
		/* return success */
		break;
	case KKM_SET_DEBUG:
		/* set guest debug state */
		ret_val = kkm_from_user(&ga->debug, (void *)arg,
					sizeof(struct kkm_debug));
		break;
	case KKM_GET_EVENTS:
	default:
		printk(KERN_NOTICE
		       "kkm_execution_kontext_ioctl: unsupported ioctl_type(%x)\n",
		       ioctl_type);
		ret_val = -EOPNOTSUPP;
		break;
	}
	return ret_val;
}

static vm_fault_t kkm_execution_kontext_fault(struct vm_fault *vmf)
{
	struct kkm_kontext *kkm_kontext =
		(struct kkm_kontext *)vmf->vma->vm_file->private_data;

	if (vmf->pgoff >= KKM_CONTEXT_MAP_PAGE_COUNT) {
		printk(KERN_NOTICE
		       "kkm_execution_kontext_fault: out of range vaddr %lx offset %ld %p\n",
		       vmf->address, vmf->pgoff, vmf->page);

		return VM_FAULT_SIGBUS;
	}

	vmf->page = virt_to_page(kkm_kontext->mmap_area[vmf->pgoff].kvaddr);
	get_page(vmf->page);
	return 0;
}

static const struct vm_operations_struct kkm_execution_kontext_vm_ops = {
	.fault = kkm_execution_kontext_fault
};

static int kkm_execution_kontext_mmap(struct file *file_p,
				      struct vm_area_struct *vma)
{
	vma->vm_ops = &kkm_execution_kontext_vm_ops;
	return 0;
}

struct file_operations kkm_execution_kontext_fops = {
	.release = kkm_execution_kontext_release,
	.unlocked_ioctl = kkm_execution_kontext_ioctl,
	.mmap = kkm_execution_kontext_mmap,
	.llseek = noop_llseek,
};

/*
 * create execution context one per vcpu
 */
int kkm_add_execution_kontext(struct kkm *kkm)
{
	int ret_val = 0;
	int i = 0;
	struct kkm_kontext *kkm_kontext = NULL;
	char buffer[32];
	struct kkm_kontext_mmap_area *kkma = NULL;

	mutex_lock(&kkm->kontext_lock);
	if (kkm->mm != current->mm) {
		ret_val = -EINVAL;
		goto error;
	}
	if (kkm->kontext_count >= KKM_MAX_CONTEXTS) {
		ret_val = -EINVAL;
		goto error;
	}

	for (i = 0; i < KKM_MAX_CONTEXTS; i++) {
		if (kkm->kontext[i].used == false) {
			break;
		}
	}

	kkm_kontext = &kkm->kontext[i];

	kkm_kontext->used = true;
	kkm_kontext->task = current;
	kkm_kontext->kkm = kkm;

	/*
	 * create anon fd for execution context
	 */
	snprintf(buffer, sizeof(buffer), "kkm-kontext:%d", i);
	kkm_kontext->kontext_fd =
		anon_inode_getfd(buffer, &kkm_execution_kontext_fops,
				 kkm_kontext, O_CLOEXEC | O_RDWR);
	if (kkm_kontext->kontext_fd >= 0) {
		ret_val = kkm_kontext->kontext_fd;
	}

	for (i = 0; i < KKM_CONTEXT_MAP_PAGE_COUNT; i++) {
		kkma = &kkm_kontext->mmap_area[i];
		kkma->offset = i;
		kkma->page = alloc_page(GFP_KERNEL | __GFP_ZERO);
		if (kkma->page == NULL) {
			ret_val = -ENOMEM;
			goto error;
		}
		kkma->kvaddr = (unsigned long)page_address(kkma->page);
	}

	kkm_kontext_init(kkm_kontext);

	kkm_reference_count_up(kkm);

error:
	mutex_unlock(&kkm->kontext_lock);
	return ret_val;
}

/*
 * add physical memory
 */
int kkm_set_kontainer_memory(struct kkm *kkm, unsigned long arg)
{
	struct kkm_memory_region mr;
	int ret_val = 0;
	int i = 0;

	if (copy_from_user(&mr, (void *)arg,
			   sizeof(struct kkm_memory_region))) {
		return -EFAULT;
	}

	if (mr.slot > KKM_MAX_MEMORY_SLOTS) {
		return -EINVAL;
	}
	if ((mr.guest_phys_addr & (PAGE_SIZE - 1)) ||
	    (mr.memory_size & (PAGE_SIZE - 1)) ||
	    (mr.userspace_addr & (PAGE_SIZE - 1))) {
		return -EINVAL;
	}

	mutex_lock(&kkm->mem_lock);
	if (mr.memory_size == 0 && kkm->mem_slot[mr.slot].used == false) {
		ret_val = -EINVAL;
		goto error;
	}
	if (mr.memory_size != 0 && kkm->mem_slot[mr.slot].used == true) {
		ret_val = -EINVAL;
		goto error;
	}
	if (mr.memory_size == 0) {
		memset(&kkm->mem_slot[mr.slot], 0, sizeof(struct kkm_mem_slot));
		kkm->mem_slot_count--;
	} else {
		// fail if there are overlaps
		for (i = 0; i < KKM_MAX_MEMORY_SLOTS; i++) {
			struct kkm_mem_slot *current_slot = &kkm->mem_slot[i];
			if (current_slot->used == true) {
				continue;
			}

			// no overlaps allowed in monitor space or guest physical space
			if ((current_slot->mr.userspace_addr <=
			     mr.userspace_addr) &&
			    (mr.userspace_addr <
			     current_slot->mr.userspace_addr +
				     current_slot->mr.memory_size)) {
				ret_val = -EINVAL;
				break;
			}
			if ((current_slot->mr.guest_phys_addr <=
			     mr.guest_phys_addr) &&
			    (mr.guest_phys_addr <
			     current_slot->mr.guest_phys_addr +
				     current_slot->mr.memory_size)) {
				ret_val = -EINVAL;
				break;
			}
		}
		kkm->mem_slot[mr.slot].used = true;
		kkm->mem_slot[mr.slot].npages = mr.memory_size >> PAGE_SHIFT;
		kkm->mem_slot[mr.slot].user_pfn =
			mr.userspace_addr >> PAGE_SHIFT;
		memcpy(&kkm->mem_slot[mr.slot].mr, &mr,
		       sizeof(struct kkm_memory_region));
		kkm->mem_slot_count++;
	}

	kkm_mmu_sync((uint64_t)kkm->mm->pgd, kkm->guest_kernel_va,
		     kkm->guest_payload_va);
error:
	mutex_unlock(&kkm->mem_lock);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_set_kontainer_memory: ret_val %d slot %d\n",
		       ret_val, mr.slot);
	}
	return ret_val;
}

int kkm_set_id_map_addr(struct kkm *kkm, unsigned long arg)
{
	uint64_t addr;
	int ret_val = 0;

	if (copy_from_user(&addr, (void *)arg, sizeof(uint64_t))) {
		ret_val = -EFAULT;
		goto error;
	}

	kkm->id_map_addr = addr;

error:
	return ret_val;
}

static int kkm_kontainer_release(struct inode *inode_p, struct file *file_p)
{
	struct kkm *kkm = file_p->private_data;

	kkm_kontainer_cleanup(kkm);

	kkm_reference_count_down(kkm);

	return 0;
}

/*
 * ioctls for kontainer one instance per guest
 */
static long kkm_kontainer_ioctl(struct file *file_p, unsigned int ioctl_type,
				unsigned long arg)
{
	long ret_val = 0;
	struct kkm *kkm = file_p->private_data;

	switch (ioctl_type) {
	case KKM_ADD_EXECUTION_CONTEXT:
		/* add one execution context */
		ret_val = kkm_add_execution_kontext(kkm);
		break;
	case KKM_MEMORY:
		/* modify memory state */
		ret_val = kkm_set_kontainer_memory(kkm, arg);
		break;
	case KKM_SET_ID_MAP_ADDR:
		/* set id map area */
		ret_val = kkm_set_id_map_addr(kkm, arg);
		break;
	default:
		printk(KERN_NOTICE
		       "kkm_kontainer_ioctl: unsupported ioctl_type(%x)\n",
		       ioctl_type);
		ret_val = -EOPNOTSUPP;
		break;
	}
	return ret_val;
}

static struct file_operations kkm_kontainer_ops = {
	.release = kkm_kontainer_release,
	.unlocked_ioctl = kkm_kontainer_ioctl,
	.llseek = noop_llseek,
};

/*
 * one call per guest
 */
int kkm_create_kontainer(unsigned long arg)
{
	int ret_val = 0;
	struct kkm *kkm = NULL;

	// create
	kkm = kzalloc(sizeof(struct kkm), GFP_KERNEL);
	if (kkm == NULL) {
		ret_val = -ENOMEM;
		goto error;
	}

	/*
	 * create anon fd and return it back to user
	 * all further ioctls for this guest are on this fd
	 * and kontext anon fd's
	 */
	kkm->kontainer_fd = anon_inode_getfd(
		"kkm-kontainer", &kkm_kontainer_ops, kkm, O_CLOEXEC | O_RDWR);
	if (kkm->kontainer_fd < 0) {
		ret_val = kkm->kontainer_fd;
		goto error;
	}

	kkm->mm = current->mm;

	ret_val = kkm_kontainer_init(kkm);
	if (ret_val != 0) {
		goto error;
	}

	/*
	 * setup pml4 for guest kernel and guest payload
	 */
	ret_val = kkm_mmu_copy_kernel_pgd((uint64_t)kkm->mm->pgd,
					  kkm->guest_kernel_va,
					  kkm->guest_payload_va);
	if (ret_val != 0) {
		printk(KERN_NOTICE
		       "kkm_create_kontainer: Copy kernel pgd entry failed error(%d)\n",
		       ret_val);
		goto error;
	}

	kkm_reference_count_init(kkm);

	return kkm->kontainer_fd;

error:
	return ret_val;
}

/*
 * support cpuid functions needed by km
 * cpuid functions used by monitor are in cpuid_functions array.
 * execute on native cpu and return results to monitor
 */
static int kkm_get_native_cpuid(unsigned long arg)
{
	static uint32_t cpuid_functions[] = { 0x0,	  0x80000001,
					      0x80000002, 0x80000003,
					      0x80000004, 0x80000008 };
	struct kkm_cpuid kcpuid;
	struct kkm_ec_entry *ec = NULL;
	int ret_val = 0;
	int i = 0;
	int ec_entries_size = 0;

	if (copy_from_user(&kcpuid, (void *)arg, sizeof(struct kkm_cpuid))) {
		ret_val = -EFAULT;
		goto error;
	}

	if (kcpuid.entry_count == 0 ||
	    kcpuid.entry_count < KKM_CONTEXT_INFO_ENTRY_COUNT) {
		ret_val = -EINVAL;
		goto error;
	}

	ec_entries_size =
		sizeof(struct kkm_ec_entry) * KKM_CONTEXT_INFO_ENTRY_COUNT;
	ec = vzalloc(ec_entries_size);
	if (ec == NULL) {
		ret_val = -ENOMEM;
		goto error;
	}

	kcpuid.entry_count = KKM_CONTEXT_INFO_ENTRY_COUNT;
	for (i = 0; i < sizeof(cpuid_functions) / sizeof(uint32_t); i++) {
		cpuid(cpuid_functions[i], &ec[i].eax, &ec[i].ebx, &ec[i].ecx,
		      &ec[i].edx);
		ec[i].function = cpuid_functions[i];
	}

	if (copy_to_user(&(((struct kkm_cpuid *)arg)->entries[0]), ec,
			 ec_entries_size)) {
		ret_val = -EFAULT;
	}

	kcpuid.reserved = 0;
	if (copy_to_user((void *)arg, &kcpuid, sizeof(struct kkm_cpuid))) {
		ret_val = -EFAULT;
	}

	vfree(ec);

error:
	return ret_val;
}

/*
 * ioctl handler for /dev/kkm
 */
static long kkm_device_ioctl(struct file *file_p, unsigned int ioctl_type,
			     unsigned long arg)
{
	long ret_val = 0;

	switch (ioctl_type) {
	case KKM_GET_VERSION:
		ret_val = kkm_version;
		break;
	case KKM_CREATE_KONTAINER:
		ret_val = kkm_create_kontainer(arg);
		break;
	case KKM_GET_CONTEXT_MAP_SIZE:
		ret_val = KKM_CONTEXT_MAP_SIZE;
		break;
	case KKM_GET_SUPPORTED_CONTEXT_INFO:
		ret_val = kkm_get_native_cpuid(arg);
		break;
	default:
		printk(KERN_NOTICE
		       "kkm_device_ioctl: unsupported ioctl_type(%x)\n",
		       ioctl_type);
		ret_val = -EOPNOTSUPP;
		break;
	}

	return ret_val;
}

static struct file_operations kkm_chardev_ops = {
	.unlocked_ioctl = kkm_device_ioctl,
	.llseek = noop_llseek,
};

static struct miscdevice kkm_device = { MISC_DYNAMIC_MINOR, KKM_DEVICE_NAME,
					&kkm_chardev_ops };

/*
 * called during insmod
 * allocate required global data structures
 */
static int __init kkm_init(void)
{
	int ret_val = 0;

	/*
	 * register /dev/kkm
	 */
	ret_val = misc_register(&kkm_device);
	if (ret_val != 0) {
		printk(KERN_ERR "kkm_init: Cannot register kkm.\n");
		return ret_val;
	}

	/*
	 * intialize mmu, allocate kkm private area data structures
	 */
	ret_val = kkm_mmu_init();
	if (ret_val != 0) {
		printk(KERN_ERR "kkm_init: Cannot initialize mmu tables.\n");
		return ret_val;
	}

	/*
	 * initialize idt cache
	 */
	ret_val = kkm_idt_cache_init();
	if (ret_val != 0) {
		printk(KERN_ERR "kkm_init: Cannot initialize idt cache.\n");
		return ret_val;
	}

	printk(KERN_INFO "kkm_init: Registered kkm.\n");

	return 0;
}

module_init(kkm_init);

/*
 * called during module unload
 * cleanup everything
 */
static void __exit kkm_exit(void)
{
	kkm_idt_cache_cleanup();
	kkm_mmu_cleanup();
	misc_deregister(&kkm_device);
	printk(KERN_INFO "kkm_exit: De-Registered kkm.\n");
}
module_exit(kkm_exit);

MODULE_AUTHOR("Kontain");
MODULE_DESCRIPTION("Kontain Kernel Monitor");
MODULE_LICENSE("GPL");
