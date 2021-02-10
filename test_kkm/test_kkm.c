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

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kkm_ioctl.h"

#define MAX_DEVICE_NAME_LEN (32)

typedef struct {
	char device_name[MAX_DEVICE_NAME_LEN];
	int root_device_fd;
	bool cpu_supported;
	uint32_t driver_version;
	int driver_identity;
	int kontain_device_fd;
	int context_map_size;
	int context_device_fd;
} kkm_t;

kkm_t kkm;

int init(kkm_t *kkm);
void cleanup(kkm_t *kkm);

int init(kkm_t *kkm)
{
	strcpy(kkm->device_name, "/dev/");
	strcat(kkm->device_name, KKM_DEVICE_NAME);

	kkm->root_device_fd = open(kkm->device_name, O_RDONLY);
	if (kkm->root_device_fd < 0) {
		perror("open:");
		goto error;
	}

	int ret_val = ioctl(kkm->root_device_fd, KKM_CPU_SUPPORTED, NULL);
	if (ret_val < 0) {
		perror("ioctl:");
		goto error;
	}
	kkm->cpu_supported = ret_val;
	printf("cpu supported : %d\n", kkm->cpu_supported);

	ret_val = ioctl(kkm->root_device_fd, KKM_GET_VERSION, NULL);
	if (ret_val < 0) {
		perror("ioctl:");
		goto error;
	}
	kkm->driver_version = ret_val;
	printf("driver version : %d\n", kkm->driver_version);

	ret_val = ioctl(kkm->root_device_fd, KKM_GET_IDENTITY, NULL);
	if (ret_val < 0) {
		perror("ioctl:");
		goto error;
	}
	if (KKM_DEVICE_IDENTITY != ret_val) {
		printf("ioctl: KKM_DEVICE_IDENTITY expecting (%d) found(%x)\n",
		       KKM_DEVICE_IDENTITY, ret_val);
		goto error;
	}
	kkm->driver_identity = ret_val;
	printf("driver identity : %x\n", kkm->driver_identity);

	return 0;

error:
	cleanup(kkm);
	return 1;
}

void cleanup(kkm_t *kkm)
{
	if (kkm->root_device_fd >= 0) {
		close(kkm->root_device_fd);
	}
}

void create_kontainer(kkm_t *kkm)
{
	int ret_val = ioctl(kkm->root_device_fd, KKM_CREATE_KONTAINER, NULL);
	if (ret_val < 0) {
		perror("ioctl:");
		goto error;
	}
	kkm->kontain_device_fd = ret_val;
	printf("kontain fd : %d\n", kkm->kontain_device_fd);
error:
	return;
}

void get_mapsize(kkm_t *kkm)
{
	int ret_val =
		ioctl(kkm->root_device_fd, KKM_GET_CONTEXT_MAP_SIZE, NULL);
	if (ret_val < 0) {
		perror("ioctl:");
		goto error;
	}
	kkm->context_map_size = ret_val;
	printf("context map size : %d\n", kkm->context_map_size);
error:
	return;
}

void get_cpuid(kkm_t *kkm)
{
	struct kkm_cpuid *cpuid;
	int alloc_size =
		sizeof(struct kkm_cpuid) +
		sizeof(struct kkm_ec_entry) * KKM_CONTEXT_INFO_ENTRY_COUNT;
	cpuid = malloc(alloc_size);
	memset(cpuid, 0xA5, alloc_size);

	cpuid->entry_count = KKM_CONTEXT_INFO_ENTRY_COUNT;
	int ret_val = ioctl(kkm->root_device_fd, KKM_GET_SUPPORTED_CONTEXT_INFO,
			    cpuid);
	if (ret_val < 0) {
		perror("ioctl:");
		goto error;
	}
	printf("cpuid entry count : %d\n", cpuid->entry_count);
	printf("function    index    flags      eax      ebc     ecx       edx\n");

	for (int i = 0; i < cpuid->entry_count; i++) {
		printf("%8x %8x %8x %8x %8x %8x %8x\n",
		       cpuid->entries[i].function, cpuid->entries[i].index,
		       cpuid->entries[i].flags, cpuid->entries[i].eax,
		       cpuid->entries[i].ebx, cpuid->entries[i].ecx,
		       cpuid->entries[i].edx);
	}
error:
	return;
}

void add_memory(kkm_t *kkm)
{
	struct kkm_memory_region mm;
	mm.slot = 0;
	mm.flags = 0;
	mm.guest_phys_addr = 0x1000;
	mm.memory_size = 63 * 0x1000;
	mm.userspace_addr = 0x100000001000ull;
	int ret_val = ioctl(kkm->kontain_device_fd, KKM_MEMORY, &mm);
	if (ret_val < 0) {
		perror("ioctl:");
		goto error;
	}
	printf("added %lx bytes of memory at userspace %lx guest phys %lx\n",
	       mm.memory_size, mm.userspace_addr, mm.guest_phys_addr);
error:
	return;
}

void create_context(kkm_t *kkm)
{
	int ret_val =
		ioctl(kkm->kontain_device_fd, KKM_ADD_EXECUTION_CONTEXT, NULL);
	if (ret_val < 0) {
		perror("ioctl:");
		goto error;
	}
	kkm->context_device_fd = ret_val;
	printf("context fd : %d\n", kkm->context_device_fd);
error:
	return;
}

int main()
{
	init(&kkm);
	create_kontainer(&kkm);
	get_mapsize(&kkm);
	get_cpuid(&kkm);
	add_memory(&kkm);
	create_context(&kkm);
	cleanup(&kkm);
	return 0;
}
