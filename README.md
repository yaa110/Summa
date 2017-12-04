Summa
=====
Practical examples of C programming language to save the day

## Examples

### [benchmark-server.c](https://github.com/yaa110/Summa/blob/master/src/benchmark-server.c)
Creates a socket to perform download and upload benchmark tests.

**Compile** `make benchmark-server`

#### Test
- **Download Server** `./benchmark-server -d <port>`
- **Download Client** `curl -s -m 10 -w "Speed: %{speed_download} bytes/sec\n" http://<server-ip>:<port> -o /dev/null`
- **Upload Server** `./benchmark-server -u <port>`
- **Upload Client** `curl -s -m 10 -w "Speed: %{speed_upload} bytes/sec" -d @"/path/to/file" http://<server-ip>:<port>`

---

### [netlink-listener.c](https://github.com/yaa110/Summa/blob/master/src/netlink-listener.c)
Creates a netlink socket to listen for `RTMGRP_LINK`, `RTMGRP_IPV4_IFADDR`, `RTMGRP_IPV6_IFADDR` and `RTMGRP_NEIGH` multicast groups.

**Compile** `make netlink-listener`

#### Test
- **Run listener** `./netlink-listener`
