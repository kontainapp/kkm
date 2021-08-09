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

test_kkm : test_kkm.c ../kkm/kkm_ioctl.h
	gcc -Wall -g -I../kkm -o $@ $<

clean :
	rm -f test_kkm
