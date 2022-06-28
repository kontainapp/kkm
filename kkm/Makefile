# SPDX-License-Identifier: GPL-2.0
#
# Kontain Kernel Module
#
# This module enables Kontain unikernel in absence of
# hardware support for virtualization
#
# Copyright (C) 2020-2021 Kontain Inc.
#
# Authors:
#  Srinivasa Vetsa <svetsa@kontain.app>
#
#

TOP := $(shell git rev-parse --show-toplevel)
ifeq ($(TOP),)
	CURRENT_DIR := $(shell pwd)
else
	CURRENT_DIR := ${TOP}/kkm
endif
KERNEL_VERSION := $(shell uname -r)
KDIR ?= /lib/modules/${KERNEL_VERSION}/build

ifeq (valgrind, $(filter valgrind,$(MAKECMDGOALS)))
	KKM_EXTRA_FLAGS := ccflags-y="-DKM_GPA_AT_16T=1"
else
	KKM_EXTRA_FLAGS :=
endif

default:
	@echo "Building KKM module"
	$(MAKE) -C $(KDIR) M=$(CURRENT_DIR) $(KKM_EXTRA_FLAGS)
	objdump -d kkm.ko > kkm.dis
	objdump -S kkm.ko > kkm.s.dis

valgrind: default

modules_install: default
	@echo "Installing KKM module"
	$(MAKE) -C $(KDIR) M=$(CURRENT_DIR) modules_install

install: modules_install


clean:
	$(MAKE) -C $(KDIR) M=$(CURRENT_DIR) clean
	rm -f kkm.dis
	rm -f kkm.s.dis
