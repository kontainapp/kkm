# kkm
kontain kernel module

KKM is a software virtualization driver intended to support the Kontain Runtime in enviroments where
hardware based virtualzation (KVM) isn't availble, for example in cloud provders like AWS. KKM is not
a general purpose virtual machine driver, it only supports the factities required by the Kontain Runtime.

Currently, KKM supports only X86_64 processors with certain features enabled. To see whether a system supports
KKM, run:
```
$ curl -s https://raw.githubusercontent.com/kontainapp/kkm/master/installer/kkm-ok.sh | bash
```

# Installation

Typically, KKM is installed as part of a larger install of the Kontain Runtime.

To build from source and install KKM standalone on a local Fedora or Ubuntu system, run:

```
$ git clone https://github.com/kontainapp/kkm.git
cd kkm
./installer/build-script.sh

```

The script will fail if the local system is incapable of runnign KKM.

# License
Check licenses directory and individual file for applicable licenses.
