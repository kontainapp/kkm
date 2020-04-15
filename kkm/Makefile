# Copyright © 2020-2019 Kontain Inc. All rights reserved.
#
# Kontain Inc CONFIDENTIAL
#
#  This file includes unpublished proprietary source code of Kontain Inc. The
#  copyright notice above does not evidence any actual or intended publication of
#  such source code. Disclosure of this source code or any related proprietary
#  information is strictly prohibited without the express written permission of
#  Kontain Inc.

TOP := $(shell git rev-parse --show-toplevel)
CURRENT_DIR := ${TOP}/kkm
KERNEL_VERSION := $(shell uname -r)
KDIR ?= /lib/modules/${KERNEL_VERSION}/build

default:
	$(MAKE) -C $(KDIR) M=$(CURRENT_DIR)
	objdump -d kkm.ko > kkm.dis
	objdump -S kkm.ko > kkm.s.dis

clean:
	$(MAKE) -C $(KDIR) M=$(CURRENT_DIR) clean
	rm -f kkm.dis
	rm -f kkm.s.dis
