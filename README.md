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

To install KKM in Fedora or Ubuntu systems, run:

```
$ curl -s https://raw.githubusercontent.com/kontainapp/kkm/master/installer/build_script.sh | bash
```

# License
Check licenses directory and individual file for applicable licenses.
