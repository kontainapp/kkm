# Copyright © 2020-2019 Kontain Inc. All rights reserved.
#
# Kontain Inc CONFIDENTIAL
#
#  This file includes unpublished proprietary source code of Kontain Inc. The
#  copyright notice above does not evidence any actual or intended publication of
#  such source code. Disclosure of this source code or any related proprietary
#  information is strictly prohibited without the express written permission of
#  Kontain Inc.

test_kkm : test_kkm.c ../kkm/kkm_ioctl.h
	gcc -g -I../kkm -o $@ $<

clean :
	rm -f test_kkm
