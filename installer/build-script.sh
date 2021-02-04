#!/bin/bash
#
# Copyright © 2021 Kontain Inc. All rights reserved.
#
# Builds and installs KKM modules. Also install pre-requisites
#
set -e ; [ "$TRACE" ] && set -x

MAJOR_VERSION=`uname -r | cut -d'.' -f1-1`
MINOR_VERSION=`uname -r | cut -d'.' -f2-2`

# support only kernel 5+
if [ $MAJOR_VERSION -lt 5 ]; then
	echo "KKM only supports Linux kernel 5.0 and above"
	exit 1
fi

# if we have dnf or apt we can use it to install other wise bailout.
if [ -f /usr/bin/dnf ]; then
	PACKAGE_LIST="kmod patch bash tar git-core bzip2 xz findutils gzip m4 perl-interpreter perl-Carp perl-devel perl-generators make diffutils gawk gcc binutils redhat-rpm-config hmaccalc bison flex net-tools hostname bc elfutils-devel dwarves python3-devel rsync xmlto asciidoc python3-sphinx sparse zlib-devel binutils-devel newt-devel bison flex xz-devel gettext ncurses-devel pciutils-devel zlib-devel binutils-devel clang llvm numactl-devel libcap-devel libcap-ng-devel rsync rpm-build elfutils kabi-dw openssl openssl-devel nss-tools xmlto asciidoc"
	sudo dnf install -y ${PACKAGE_LIST}
elif [ -f /usr/bin/apt ]; then
   PACKAGE_LIST="make gcc"
   sudo apt update -y -q
	sudo apt install -y -q -o Dpkg::Progress-Fancy=0 ${PACKAGE_LIST}
else
	echo "Cannot find 'dnf' nor 'apt'. Exiting."
	exit 1
fi

# script is running in root directory of extracted files
echo "Building kkm.ko"
make -C kkm

echo "Installing kkm.ko"
sudo make -C kkm modules_install

echo "Installing /etc files for kkm and loading KKM module"
sudo "$(realpath $(dirname "${BASH_SOURCE[0]}"))/install-script.sh"

echo "Kontain Kernel Monitor installed and loaded"
