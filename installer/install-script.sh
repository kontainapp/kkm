#!/bin/bash -ex
#
# Copyright © 2021 Kontain Inc. All rights reserved.
#
# Install and loads KKM module. Expects to be run under sudo
#
location=$(realpath $(dirname "${BASH_SOURCE[0]}"))

# some of the installs have incorrect /boot links, update them
depmod -a `uname -r`

# files required for kkm module in /etc/
cp $location/etc/modprobe.d/kkm.conf /etc/modprobe.d/kkm.conf
cp $location/etc/modules-load.d/kkm.conf /etc/modules-load.d/kkm.conf

modprobe kkm

