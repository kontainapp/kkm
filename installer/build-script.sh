#!/usr/bin/sh

MAJOR_VERSION=`uname -r | cut -d'.' -f1-1`
MINOR_VERSION=`uname -r | cut -d'.' -f2-2`

# support only kernel 5+
if [ $MAJOR_VERSION -lt 5 ]; then
	echo "KKM is only supported above 5.x kernel"
	exit 1
fi

# if we have dnf or apt we can use it to install other wise bailout.
if [ -f /usr/bin/dnf ]; then
	PACKAGE_LIST=" kmod patch bash tar git-core bzip2 xz findutils gzip m4 perl-interpreter perl-Carp perl-devel perl-generators make diffutils gawk gcc binutils redhat-rpm-config hmaccalc bison flex net-tools hostname bc elfutils-devel dwarves python3-devel rsync xmlto asciidoc python3-sphinx sparse zlib-devel binutils-devel newt-devel bison flex xz-devel gettext ncurses-devel pciutils-devel zlib-devel binutils-devel clang llvm numactl-devel libcap-devel libcap-ng-devel rsync rpm-build elfutils kabi-dw openssl openssl-devel nss-tools xmlto asciidoc"
	INSTALLER_CMD="/usr/bin/dnf install -y ${PACKAGE_LIST}"
elif [ -f /usr/bin/apt ]; then
	PACKAGE_LIST=" make gcc "
	INSTALLER_CMD="/usr/bin/apt update; /usr/bin/apt install -y ${PACKAGE_LIST}"
else
	echo "Cannot find dnf or apt-get exiting"
	exit 1
fi

# make sure kitchen sink is available on fedora
sudo -u root /usr/bin/bash << INSTALL_END

${INSTALLER_CMD}

INSTALL_END

# script is running in root directory of extracted files

echo "Building kkm.ko"
make -C kkm

echo "Installing kkm.ko"
sudo make -C kkm modules_install

# some of the installs have incorrect /boot links, run depmod for running kernel
sudo depmod -a `uname -r`

# files required for kkm module in /etc/
echo "Installing /etc file for kkm"
sudo cp installer/etc/modprobe.d/kkm.conf /etc/modprobe.d/kkm.conf
sudo cp installer/etc/modules-load.d/kkm.conf /etc/modules-load.d/kkm.conf

echo "Doing modprobe kkm"
modprobe kkm

echo "done"
