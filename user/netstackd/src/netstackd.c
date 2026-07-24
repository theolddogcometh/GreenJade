/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * netstackd — userspace network stack host (A1 host smoke).
 * Product: own virtio-net RX/TX via UDX + socket door. Here we exercise
 * a software loopback table (UDP echo) until kernel door is handed off.
 * Host POSIX smoke only (not the freestanding live daemon). Dual SPDX; no GPL.
 * Greppable success: "netstackd: PASS"
 *
 *   make netstackd && ./build/netstackd
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define GJ_NET_MAX_SOCK 8u
#define GJ_NET_MTU      1500u

struct net_sock {
    int used;
    uint16_t port;
    uint8_t aRx[GJ_NET_MTU];
    uint32_t u32RxLen;
};

static struct net_sock g_aSock[GJ_NET_MAX_SOCK];
static uint32_t g_u32Tx;
static uint32_t g_u32Rx;
static uint32_t g_u32Echo;

static int
net_bind(uint16_t port)
{
    uint32_t i;

    for (i = 0; i < GJ_NET_MAX_SOCK; i++) {
        if (!g_aSock[i].used) {
            g_aSock[i].used = 1;
            g_aSock[i].port = port;
            g_aSock[i].u32RxLen = 0;
            return (int)i;
        }
    }
    return -1;
}

static int
net_sendto(int fd, uint16_t dstPort, const void *p, uint32_t len)
{
    uint32_t i;

    if (fd < 0 || (uint32_t)fd >= GJ_NET_MAX_SOCK || !g_aSock[fd].used ||
        p == NULL || len == 0 || len > GJ_NET_MTU) {
        return -1;
    }
    g_u32Tx++;
    /* Loopback deliver to bound port */
    for (i = 0; i < GJ_NET_MAX_SOCK; i++) {
        if (g_aSock[i].used && g_aSock[i].port == dstPort) {
            memcpy(g_aSock[i].aRx, p, len);
            g_aSock[i].u32RxLen = len;
            g_u32Rx++;
            return (int)len;
        }
    }
    return (int)len; /* sent into void */
}

static int
net_recv(int fd, void *p, uint32_t cap)
{
    uint32_t n;

    if (fd < 0 || (uint32_t)fd >= GJ_NET_MAX_SOCK || !g_aSock[fd].used ||
        p == NULL) {
        return -1;
    }
    n = g_aSock[fd].u32RxLen;
    if (n == 0) {
        return 0;
    }
    if (n > cap) {
        n = cap;
    }
    memcpy(p, g_aSock[fd].aRx, n);
    g_aSock[fd].u32RxLen = 0;
    return (int)n;
}

/** Echo server tick: if sock has data, reply to peer port. */
static void
net_echo_tick(int srvFd, uint16_t peerPort)
{
    uint8_t buf[GJ_NET_MTU];
    int n = net_recv(srvFd, buf, sizeof(buf));

    if (n > 0) {
        (void)net_sendto(srvFd, peerPort, buf, (uint32_t)n);
        g_u32Echo++;
    }
}

int
main(void)
{
    int srv;
    int cli;
    const char *msg = "greenjade-net";
    char rbuf[64];
    int n;

    printf("netstackd: start (software lo until virtio door)\n");
    memset(g_aSock, 0, sizeof(g_aSock));

    srv = net_bind(7); /* echo */
    cli = net_bind(40000);
    if (srv < 0 || cli < 0) {
        printf("netstackd: bind FAIL\n");
        return 1;
    }
    if (net_sendto(cli, 7, msg, (uint32_t)strlen(msg) + 1) < 0) {
        printf("netstackd: send FAIL\n");
        return 1;
    }
    net_echo_tick(srv, 40000);
    n = net_recv(cli, rbuf, sizeof(rbuf));
    if (n <= 0 || strcmp(rbuf, msg) != 0) {
        printf("netstackd: echo FAIL n=%d\n", n);
        return 1;
    }
    printf("netstackd: udp echo ok tx=%u rx=%u echo=%u\n", g_u32Tx, g_u32Rx,
           g_u32Echo);
    /* Second socket pair (door SOCKET/BIND/SEND/RECV shape) */
    {
        int s2 = net_bind(9);
        int c2 = net_bind(40001);
        const char *m2 = "gj-door";

        if (s2 < 0 || c2 < 0) {
            printf("netstackd: second bind FAIL\n");
            return 1;
        }
        if (net_sendto(c2, 9, m2, (uint32_t)strlen(m2) + 1) < 0) {
            printf("netstackd: second send FAIL\n");
            return 1;
        }
        net_echo_tick(s2, 40001);
        n = net_recv(c2, rbuf, sizeof(rbuf));
        if (n <= 0 || strcmp(rbuf, m2) != 0) {
            printf("netstackd: second echo FAIL\n");
            return 1;
        }
        printf("netstackd: door-shaped sockets ok\n");
    }

    /* Grep: netstackd: soft inventory (Wave 35 exclusive deepen; host smoke) */
    printf("netstackd: soft inventory tx=%u rx=%u echo=%u wave=35 "
           "multi_server=0 confine=0\n",
           g_u32Tx, g_u32Rx, g_u32Echo);
    printf("netstackd: soft deepen product_kernel=OPEN wave=35 areas=1 multi_server=0 "
           "confine=0 bar3=0\n");
    printf("netstackd: soft honesty multi_server=0 confine=0 bar3=0 "
           "exclusive=1 soft=1 product_kernel=OPEN wave=35\n");

    printf("netstackd: PASS\n");
    return 0;
}
