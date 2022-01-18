#!/usr/bin/sh
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

force_install=false
for arg in "$@"
do
	shift
	case "$arg" in
		"--force-install") force_install="true";;
		*);;
	esac
done

MAJOR_VERSION=$(uname -r | cut -d'.' -f1-1)
#MINOR_VERSION=$(uname -r | cut -d'.' -f2-2)

# support only kernel 5+
if [ "$MAJOR_VERSION" -lt 5 ]; then
	echo "KKM is only supported above 5.x kernel"
	exit 1
fi

# check if necessary features are available
check_script="installer/kkm-ok.bash"
if [ ! -x "$check_script" ]; then
	echo "Cannot find $check_script bailing out"
	exit 1
fi

eval $check_script
if [ $? -ne 0 ]; then
	echo "KKM required kernel/cpu features are not available on this instance."
	if [ "$force_install" = "true" ]; then
		echo "Force install flag provided continuing with installation."
		echo "KKM may not work on this instance."
	else
		echo "Stopping installation."
		echo "Provide --force-install option to install on this instance."
		exit 1
	fi
fi

# if we have dnf or apt or yum we can use it to install other wise bailout.
if [ -f /usr/bin/dnf ]; then
	PACKAGE_LIST=" kmod patch bash tar git-core bzip2 xz findutils gzip m4 perl-interpreter perl-Carp perl-devel perl-generators make diffutils gawk gcc binutils redhat-rpm-config hmaccalc bison flex net-tools hostname bc elfutils-devel dwarves python3-devel rsync xmlto asciidoc python3-sphinx sparse zlib-devel binutils-devel newt-devel bison flex xz-devel gettext ncurses-devel pciutils-devel zlib-devel binutils-devel clang llvm numactl-devel libcap-devel libcap-ng-devel rsync rpm-build elfutils kabi-dw openssl openssl-devel nss-tools xmlto asciidoc dkms "
	INSTALLER_CMD="/usr/bin/dnf install -y ${PACKAGE_LIST}"
elif [ -f /usr/bin/apt ]; then
	PACKAGE_LIST=" make gcc dkms build-essential "
	INSTALLER_CMD="/usr/bin/apt-get -q -y update; /usr/bin/apt-get -q -y install -y ${PACKAGE_LIST}"
elif [ -f /usr/bin/yum ]; then
	PACKAGE_LIST=" make gcc dkms build-essential "
	INSTALLER_CMD="/usr/bin/yum -q -y update; /usr/bin/yum -q -y install -y ${PACKAGE_LIST}"
else
	echo "Cannot find dnf or apt exiting"
	exit 1
fi

# make sure kitchen sink is available on fedora
sudo -u root /usr/bin/bash << INSTALL_END

${INSTALLER_CMD}

INSTALL_END

# script is running in root directory of extracted files

# setup kkm

# remove
sudo dkms remove -q -m kkm -v 0.9 --all
sudo rm -fr /usr/src/kkm-0.9
rm -f /var/lib/dkms/kkm

umask 022
sudo cp installer/etc/modprobe.d/kkm.conf /etc/modprobe.d/kkm.conf
sudo cp installer/etc/modules-load.d/kkm.conf /etc/modules-load.d/kkm.conf

# add
sudo mkdir -p /usr/src/kkm-0.9
sudo cp -r kkm /usr/src/kkm-0.9
sudo cp -r licenses /usr/src/kkm-0.9
sudo cp installer/dkms.conf /usr/src/kkm-0.9
sudo dkms add -c /usr/src/kkm-0.9/dkms.conf -m kkm -v 0.9
sudo dkms build -m kkm -v 0.9
sudo dkms install -m kkm -v 0.9

sudo modprobe kkm

echo "done"
