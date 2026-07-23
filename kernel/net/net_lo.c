/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Loopback socket table (pure C, dual-licensed): DGRAM + non-TCP STREAM
 * fallback. Product STREAM multi-seg path uses net_tcp via the door.
 */
#include <gj/klog.h>
#include <gj/net_lo.h>
#include <gj/string.h>

#define NET_LO_MAX 16
#define AF_INET    2
#define AF_UNIX    1
#define SOCK_STREAM 1
#define SOCK_DGRAM  2
#define NET_LO_BUF  512

struct net_lo_sock {
    u8  u8Used;
    u8  u8Type;
    u8  u8Listening;
    u8  u8Connected;
    u8  u8ShutRd;
    u8  u8ShutWr;
    u8  u8Reuse;
    u8  u8Pad;
    u16 u16Port;
    i16 i16Peer; /* slot of peer or -1 */
    u32 u32RxLen;
    u32 u32RxHead;
    u8  aRx[NET_LO_BUF];
};

static struct net_lo_sock g_aSocks[NET_LO_MAX];
/* FDs 64..79 map to socket slots 0..15 (avoid vfs_ram 3..31) */
#define NET_FD_BASE 64

void
net_lo_init(void)
{
    memset(g_aSocks, 0, sizeof(g_aSocks));
    kprintf("net_lo: init (loopback + peer ring)\n");
}

int
net_lo_fd_ok(i64 i64Fd)
{
    u32 u32Slot;

    if (i64Fd < NET_FD_BASE || i64Fd >= NET_FD_BASE + NET_LO_MAX) {
        return 0;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    return g_aSocks[u32Slot].u8Used;
}

i64
net_lo_socket(int nDomain, int nType, int nProto)
{
    u32 i;

    (void)nProto;
    if (nDomain != AF_INET && nDomain != AF_UNIX) {
        return -97; /* EAFNOSUPPORT */
    }
    if (nType != SOCK_STREAM && nType != SOCK_DGRAM) {
        return -22;
    }
    for (i = 0; i < NET_LO_MAX; i++) {
        if (!g_aSocks[i].u8Used) {
            g_aSocks[i].u8Used = 1;
            g_aSocks[i].u8Type = (u8)nType;
            g_aSocks[i].u8Listening = 0;
            g_aSocks[i].u8Connected = 0;
            g_aSocks[i].u8ShutRd = 0;
            g_aSocks[i].u8ShutWr = 0;
            g_aSocks[i].u8Reuse = 0;
            g_aSocks[i].u16Port = 0;
            g_aSocks[i].i16Peer = -1;
            g_aSocks[i].u32RxLen = 0;
            g_aSocks[i].u32RxHead = 0;
            return (i64)(NET_FD_BASE + i);
        }
    }
    return -24; /* EMFILE */
}

i64
net_lo_bind(i64 i64Fd, u16 u16Port)
{
    u32 u32Slot;

    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    g_aSocks[u32Slot].u16Port = u16Port;
    return 0;
}

i64
net_lo_listen(i64 i64Fd, int nBacklog)
{
    u32 u32Slot;

    (void)nBacklog;
    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    g_aSocks[u32Slot].u8Listening = 1;
    return 0;
}

i64
net_lo_connect(i64 i64Fd, u16 u16Port)
{
    u32 u32Slot;
    u32 i;

    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    g_aSocks[u32Slot].u16Port = u16Port;
    /*
     * Pair client ↔ listener so send/recv works without accept (legacy path).
     * accept() later mints a dedicated server peer and re-pairs.
     */
    for (i = 0; i < NET_LO_MAX; i++) {
        if (i != u32Slot && g_aSocks[i].u8Used && g_aSocks[i].u8Listening &&
            g_aSocks[i].u16Port == u16Port) {
            if (g_aSocks[i].i16Peer >= 0 &&
                g_aSocks[i].i16Peer != (i16)u32Slot) {
                return -11; /* EAGAIN — one pending only */
            }
            g_aSocks[i].i16Peer = (i16)u32Slot;
            g_aSocks[u32Slot].i16Peer = (i16)i;
            g_aSocks[u32Slot].u8Connected = 1;
            g_aSocks[i].u8Connected = 1;
            return 0;
        }
    }
    g_aSocks[u32Slot].u8Connected = 1;
    return 0;
}

i64
net_lo_accept(i64 i64Fd)
{
    u32 u32Slot;
    u32 i;
    i16 i16Cli;

    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (!g_aSocks[u32Slot].u8Listening) {
        return -22; /* EINVAL */
    }
    i16Cli = g_aSocks[u32Slot].i16Peer;
    if (i16Cli < 0 || (u32)i16Cli >= NET_LO_MAX || !g_aSocks[i16Cli].u8Used) {
        return -11; /* EAGAIN */
    }
    /* Mint accepted socket; pair with client; free listener pending slot */
    for (i = 0; i < NET_LO_MAX; i++) {
        if (!g_aSocks[i].u8Used) {
            g_aSocks[i].u8Used = 1;
            g_aSocks[i].u8Type = g_aSocks[u32Slot].u8Type;
            g_aSocks[i].u8Listening = 0;
            g_aSocks[i].u8Connected = 1;
            g_aSocks[i].u8ShutRd = 0;
            g_aSocks[i].u8ShutWr = 0;
            g_aSocks[i].u8Reuse = 0;
            g_aSocks[i].u16Port = g_aSocks[u32Slot].u16Port;
            g_aSocks[i].i16Peer = i16Cli;
            g_aSocks[i].u32RxLen = 0;
            g_aSocks[i].u32RxHead = 0;
            g_aSocks[i16Cli].i16Peer = (i16)i;
            g_aSocks[u32Slot].i16Peer = -1;
            return (i64)(NET_FD_BASE + i);
        }
    }
    return -24; /* EMFILE */
}

static int
push_rx(u32 u32Slot, const void *pBuf, u32 cb)
{
    struct net_lo_sock *pS;
    u32 i;

    if (u32Slot >= NET_LO_MAX || !g_aSocks[u32Slot].u8Used) {
        return -1;
    }
    pS = &g_aSocks[u32Slot];
    if (cb > NET_LO_BUF - pS->u32RxLen) {
        cb = NET_LO_BUF - pS->u32RxLen;
    }
    for (i = 0; i < cb; i++) {
        u32 pos = (pS->u32RxHead + pS->u32RxLen) % NET_LO_BUF;

        pS->aRx[pos] = ((const u8 *)pBuf)[i];
        pS->u32RxLen++;
    }
    return (int)cb;
}

i64
net_lo_send(i64 i64Fd, const void *pBuf, size_t cb)
{
    u32 u32Slot;
    i16 i16Peer;

    if (!net_lo_fd_ok(i64Fd) || pBuf == NULL) {
        return -9;
    }
    if (cb == 0) {
        return 0;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (g_aSocks[u32Slot].u8ShutWr) {
        return -32; /* EPIPE-shaped */
    }
    i16Peer = g_aSocks[u32Slot].i16Peer;
    if (i16Peer < 0 || (u32)i16Peer >= NET_LO_MAX) {
        /* Unpaired: loop into own RX for dgram smoke */
        i16Peer = (i16)u32Slot;
    }
    return (i64)push_rx((u32)i16Peer, pBuf, (u32)cb);
}

i64
net_lo_recv(i64 i64Fd, void *pBuf, size_t cb)
{
    u32 u32Slot;
    struct net_lo_sock *pS;
    u32 u32N;
    u32 i;

    if (!net_lo_fd_ok(i64Fd) || pBuf == NULL) {
        return -9;
    }
    if (cb == 0) {
        return 0;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    pS = &g_aSocks[u32Slot];
    if (pS->u8ShutRd) {
        return 0; /* EOF */
    }
    if (pS->u32RxLen == 0) {
        return -11; /* EAGAIN */
    }
    u32N = (u32)cb;
    if (u32N > pS->u32RxLen) {
        u32N = pS->u32RxLen;
    }
    for (i = 0; i < u32N; i++) {
        ((u8 *)pBuf)[i] = pS->aRx[pS->u32RxHead];
        pS->u32RxHead = (pS->u32RxHead + 1) % NET_LO_BUF;
        pS->u32RxLen--;
    }
    return (i64)u32N;
}

i64
net_lo_shutdown(i64 i64Fd, int nHow)
{
    u32 u32Slot;

    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    if (nHow < 0 || nHow > 2) {
        return -22; /* EINVAL — validate before mutating flags */
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (nHow == 0 || nHow == 2) {
        g_aSocks[u32Slot].u8ShutRd = 1;
    }
    if (nHow == 1 || nHow == 2) {
        g_aSocks[u32Slot].u8ShutWr = 1;
    }
    return 0;
}

i64
net_lo_setsockopt(i64 i64Fd, int nLevel, int nOpt, const void *pVal, u32 u32Len)
{
    u32 u32Slot;
    int v = 0;

    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    /* SOL_SOCKET = 1 */
    if (nLevel != 1) {
        return 0; /* ignore other levels */
    }
    if (pVal != NULL && u32Len >= 4) {
        v = *(const int *)pVal;
    }
    if (nOpt == 2 /* SO_REUSEADDR */) {
        g_aSocks[u32Slot].u8Reuse = v ? 1u : 0u;
        return 0;
    }
    /* SO_KEEPALIVE=9, SO_RCVBUF=8, SO_SNDBUF=7 — accept no-op */
    if (nOpt == 7 || nOpt == 8 || nOpt == 9 || nOpt == 1 /* SO_DEBUG */) {
        return 0;
    }
    return 0;
}

i64
net_lo_getsockopt(i64 i64Fd, int nLevel, int nOpt, void *pVal, u32 *pLen)
{
    u32 u32Slot;
    int v = 0;

    if (!net_lo_fd_ok(i64Fd) || pVal == NULL || pLen == NULL) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (nLevel != 1) {
        return -92; /* ENOPROTOOPT-shaped */
    }
    if (nOpt == 3 /* SO_TYPE */) {
        v = g_aSocks[u32Slot].u8Type;
    } else if (nOpt == 4 /* SO_ERROR */) {
        v = 0;
    } else if (nOpt == 2 /* SO_REUSEADDR */) {
        v = g_aSocks[u32Slot].u8Reuse;
    } else {
        v = 0;
    }
    if (*pLen < 4) {
        return -22;
    }
    *(int *)pVal = v;
    *pLen = 4;
    return 0;
}

static void
fill_sin(u8 *p, u16 u16Port)
{
    u32 i;

    for (i = 0; i < 16; i++) {
        p[i] = 0;
    }
    p[0] = 2; /* AF_INET */
    p[1] = 0;
    p[2] = (u8)(u16Port >> 8);
    p[3] = (u8)(u16Port & 0xffu);
    p[4] = 127;
    p[5] = 0;
    p[6] = 0;
    p[7] = 1; /* 127.0.0.1 */
}

i64
net_lo_getsockname(i64 i64Fd, void *pAddr, u32 *pLen)
{
    u32 u32Slot;

    if (!net_lo_fd_ok(i64Fd) || pAddr == NULL || pLen == NULL) {
        return -9;
    }
    if (*pLen < 16) {
        return -22;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    fill_sin((u8 *)pAddr, g_aSocks[u32Slot].u16Port);
    *pLen = 16;
    return 0;
}

i64
net_lo_getpeername(i64 i64Fd, void *pAddr, u32 *pLen)
{
    u32 u32Slot;
    i16 peer;

    if (!net_lo_fd_ok(i64Fd) || pAddr == NULL || pLen == NULL) {
        return -9;
    }
    if (*pLen < 16) {
        return -22;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    peer = g_aSocks[u32Slot].i16Peer;
    if (peer < 0 || (u32)peer >= NET_LO_MAX) {
        return -107; /* ENOTCONN */
    }
    fill_sin((u8 *)pAddr, g_aSocks[peer].u16Port);
    *pLen = 16;
    return 0;
}

i64
net_lo_close(i64 i64Fd)
{
    u32 u32Slot;

    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (g_aSocks[u32Slot].i16Peer >= 0 &&
        (u32)g_aSocks[u32Slot].i16Peer < NET_LO_MAX) {
        g_aSocks[g_aSocks[u32Slot].i16Peer].i16Peer = -1;
    }
    memset(&g_aSocks[u32Slot], 0, sizeof(g_aSocks[u32Slot]));
    g_aSocks[u32Slot].i16Peer = -1;
    return 0;
}
