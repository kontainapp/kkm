# Copyright © 2020-2019 Kontain Inc. All rights reserved.
#
# Kontain Inc CONFIDENTIAL
#
#  This file includes unpublished proprietary source code of Kontain Inc. The
#  copyright notice above does not evidence any actual or intended publication of
#  such source code. Disclosure of this source code or any related proprietary
#  information is strictly prohibited without the express written permission of
#  Kontain Inc.
#
# Build and install Kontain Kernel Monitor kernel module.
#
# Use 'make help' fo info about targets
#


# top of the git repo or unpacked archive, without git dependency
TOP=$(realpath $(CURDIR)/..)

KERNEL_VERSION := $(shell uname -r)
KDIR ?= /lib/modules/${KERNEL_VERSION}/build

KKM_RUN ?= kkm.run

build: ## Just build, no package and no install. Mainly used from kkm.run
	@echo "Building KKM module"
	$(MAKE) -C $(KDIR) M=$(CURDIR)
	objdump -d kkm.ko > kkm.dis
	objdump -S kkm.ko > kkm.s.dis

modules_install: build ## Install KKM using Linux kernel module support. Mainly used from kkm.run
	@echo "Installing KKM module"
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules_install

package: ${KKM_RUN} ## Package KKM makeself-archive

install: package ## Install KKM from makeself-archive
	sudo ${KKM_RUN}

INSTALLER := ./installer/build-script.sh
KKM_SOURCES := *.c *.h *.S Makefile

${KKM_RUN}: $(KKM_SOURCES) ../$(INSTALLER)
	@if ! type makeself >& /dev/null ; then echo Please install \"makeself\" first; false; fi
	rm -f $(KKM_RUN)
	makeself -q ${TOP} $(KKM_RUN) "beta-release" $(INSTALLER)

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
	rm -f kkm.dis kkm.s.dis $(KKM_RUN)

# 'Help' target - based on '##' comments in targets
#
# This target ("help") scans Makefile for '##' in targets and prints a summary
# Note - used awk to print (instead of echo) so escaping/coloring is platform independed
help:  ## Prints help on 'make' targets
	@awk 'BEGIN {FS = ":.*##"; printf "\nUsage:\n make $(CYAN)<target>$(NOCOLOR)\n" } \
	/^[.a-zA-Z0-9_ -]+[ \t]*:.*?##/ { printf " $(CYAN)%-15s$(NOCOLOR) %s\n", $$1, $$2 } \
	/^##@/ { printf "\n\033[1m%s$(NOCOLOR)\n", substr($$0, 5) } ' \
	$(MAKEFILE_LIST)
	@echo 'For specific help in folders, try "(cd <dir>; make help)"'
	@echo ""

# allows to do 'make print-varname'
print-% : ; @echo $*=\"$($*)\"