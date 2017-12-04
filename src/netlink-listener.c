/*
 * =====================================================================================
 *
 *       Filename:  netlink-listener.c
 *
 *    Description:  Creates a netlink socket to listen for RTMGRP_LINK, RTMGRP_NEIG,
 *                  HRTMGRP_IPV4_IFADDR and RTMGRP_IPV6_IFADDR multicast groups.
 *
 *        Version:  1.0
 *        Created:  2017/04/12 17:30:13
 *       Compiler:  gcc
 *
 *           Test:
 *                  - Download Server: `./benchmark-server -d <port>`
 *                  - Download Client: `curl -s -m 10 -w "Speed: %{speed_download} bytes/sec\n" http://<server-ip>:<port> -o /dev/null`
 *                  - Upload Server: `./benchmark-server -u <port>`
 *                  - Upload Client: `curl -s -m 10 -w "Speed: %{speed_upload} bytes/sec" -d @"/path/to/file" http://<server-ip>:<port>`
 *
 *        Authors:  Navid (@yaa110), 
 *        License:  MIT  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <unistd.h>
#include <asm/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <net/if.h>

#define BUFFER_SIZE 8192

void nl_listen(int);
void parse_payload(struct nlmsghdr *);
char *get_neigh_state(__u16);

char *get_neigh_state(__u16 state) {
    if (state & NUD_INCOMPLETE) return "NUD_INCOMPLETE";
    if (state & NUD_REACHABLE) return "NUD_REACHABLE";
    if (state & NUD_STALE) return "NUD_STALE";
    if (state & NUD_DELAY) return "NUD_DELAY";
    if (state & NUD_PROBE) return "NUD_PROBE";
    if (state & NUD_FAILED) return "NUD_FAILED";
    if (state & NUD_NOARP) return "NUD_NOARP";
    if (state & NUD_PERMANENT) return "NUD_PERMANENT";
    return "UNKNOWN";
}

void parse_payload(struct nlmsghdr *nmh) {
    struct ifinfomsg *ifinfo;
    struct ifaddrmsg *ifaddr;
    struct ndmsg *nd;

    char ifname[IF_NAMESIZE];

    switch (nmh->nlmsg_type) {
        case RTM_NEWLINK:
        case RTM_DELLINK:
        case RTM_GETLINK:
            ifinfo = (struct ifinfomsg *)NLMSG_DATA(nmh);
            printf("Link message type: %d, ifname: %s, state: %s\n", 
                    nmh->nlmsg_type,
                    if_indextoname(ifinfo->ifi_index, ifname),
                    (ifinfo->ifi_flags & IFF_UP) ? "UP" : "DOWN");
            break;

        case RTM_NEWADDR:
        case RTM_DELADDR:
        case RTM_GETADDR:
            ifaddr = (struct ifaddrmsg *)NLMSG_DATA(nmh);
            printf("Address message type: %d, ifname: %s\n",
                    nmh->nlmsg_type,
                    if_indextoname(ifaddr->ifa_index, ifname));
            break;

        case RTM_NEWNEIGH:
        case RTM_DELNEIGH:
        case RTM_GETNEIGH:
            nd = (struct ndmsg *)NLMSG_DATA(nmh);
            printf("Neighbour message type: %d, ifname: %s, state: %s\n",
                    nmh->nlmsg_type,
                    if_indextoname(nd->ndm_ifindex, ifname),
                    get_neigh_state(nd->ndm_state));
            break;

        default:
            printf("Other message type: %d\n", nmh->nlmsg_type);
    }
}

void nl_listen(int soc) {
    int len;
    char buf[BUFFER_SIZE];
    struct iovec iov = {
        buf,
        sizeof(buf)
    };
    struct sockaddr_nl addr;
    struct nlmsghdr *nmh;
    struct msghdr mh = {
        &addr,
        sizeof(addr),
        &iov,
        1,
        NULL,
        0,
        0
    };

    len = recvmsg(soc, &mh, 0);

    if (len < 0) {
        fprintf(stderr, "unable to receive message: %s\n", strerror(errno));
        return;
    }

    for (nmh = (struct nlmsghdr *)buf; NLMSG_OK(nmh, len); nmh = NLMSG_NEXT(nmh, len)) {
        switch (nmh->nlmsg_type) {
            case NLMSG_DONE:
                return;

            case NLMSG_ERROR:
            case NLMSG_NOOP:
                break;

            default:
                parse_payload(nmh);
        }
    }
}

int main() {
    int soc;
    struct sockaddr_nl addr;
    
    soc = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (soc < 0) {
        fprintf(stderr, "unable to create socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_nl));
    addr.nl_family = AF_NETLINK;
    addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV6_IFADDR | RTMGRP_NEIGH;

    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
        fprintf(stderr, "unable to assign address to socket: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

    while (true)
        nl_listen(soc);
    
    close(soc);
    return EXIT_SUCCESS;
}

