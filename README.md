# kkm
kontain kernel module

KKM is a software virtualization driver intended to support the Kontain Runtime in environments where
hardware based virtualization (KVM) isn't available, for example in cloud providers like AWS. KKM is not
a general purpose virtual machine driver, it only supports the facilities required by the Kontain Runtime.

Currently, KKM supports only X86_64 processors with certain features enabled. To see whether a system supports
KKM, run:
```bash
$ curl -s https://raw.githubusercontent.com/kontainapp/kkm/master/installer/kkm-ok.bash | bash
```

# Installation

Typically, KKM is installed as part of a larger install of the Kontain Runtime.

To build from source and install KKM standalone on a local Fedora or Ubuntu system, run:

```bash
$ git clone https://github.com/kontainapp/kkm.git
cd kkm
./installer/build-script.sh

```

The script will fail if the local system is incapable of running KKM.

# License
Check licenses directory and individual file for applicable licenses.
