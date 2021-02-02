#!/usr/bin/sh

# make sure kitchen sink is available
sudo -u root /usr/bin/bash << INSTALL_END

dnf install -y kmod patch bash tar git-core
dnf install -y bzip2 xz findutils gzip m4 perl-interpreter perl-Carp perl-devel perl-generators make diffutils gawk
dnf install -y gcc binutils redhat-rpm-config hmaccalc bison flex
dnf install -y net-tools hostname bc elfutils-devel
dnf install -y dwarves
dnf install -y python3-devel
dnf install -y rsync
dnf install -y xmlto asciidoc python3-sphinx
dnf install -y sparse
dnf install -y zlib-devel binutils-devel newt-devel bison flex xz-devel
dnf install -y gettext ncurses-devel
dnf install -y pciutils-devel
dnf install -y zlib-devel binutils-devel
dnf install -y clang llvm
dnf install -y numactl-devel
dnf install -y libcap-devel libcap-ng-devel rsync
dnf install -y rpm-build elfutils
dnf install -y kabi-dw
dnf install -y openssl openssl-devel
dnf install -y nss-tools
dnf install -y xmlto
dnf install -y asciidoc

INSTALL_END

pwd
ls -l

echo "running kkm build script"
make -C kkm

echo "installing kkm.ko"
sudo make -C kkm modules_install

# Fedora
sudo cp installer/fedora/modprobe.d/kkm.conf /etc/modprobe.d/kkm.conf
sudo cp installer/fedora/modules-load.d/kkm.conf /etc/modules-load.d/kkm.conf

echo "done"
