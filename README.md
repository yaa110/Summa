Summa
=====
[![Build Status](https://travis-ci.org/yaa110/Summa.svg?branch=master)](https://travis-ci.org/yaa110/Summa)

Practical examples of C programming language to save the day

## Examples

- [**benchmark-server.c**](https://github.com/yaa110/Summa/blob/master/src/benchmark-server.c): Creates a socket to perform download and upload benchmark tests.
- [**netlink-listener.c**](https://github.com/yaa110/Summa/blob/master/src/netlink-listener.c): Creates a netlink socket to listen for `RTMGRP_LINK`, `RTMGRP_IPV4_IFADDR`, `RTMGRP_IPV6_IFADDR` and `RTMGRP_NEIGH` multicast groups.

## Contribution
Please feel free to open an issue to report a bug, or open a pull request to improve or add more practical examples. For opening a pull request:

- Make sure that the source code has the same structure as [template file](https://github.com/yaa110/Summa/blob/master/template.c).
- Make sure that the source code is formatted based on [.editorconfig file](https://github.com/yaa110/Summa/blob/master/.editorconfig).
- Place the source code in [src folder](https://github.com/yaa110/Summa/tree/master/src).
- Add filename and description of source code to [*Examples*](https://github.com/yaa110/Summa#examples) section of [README file](https://github.com/yaa110/Summa/blob/master/README.md) (the list of filenames should be sorted alphabetically).
- Add a new target to [Makefile](https://github.com/yaa110/Summa/blob/master/Makefile) to build the source code.
- Add source code filename (without extension) to [gitignore file](https://github.com/yaa110/Summa/blob/master/.gitignore).
