/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Loopback socket table (pure C, dual-licensed): DGRAM + non-TCP STREAM
 * fallback. Product STREAM multi-seg path uses net_tcp via the door.
 *
 * Soft product deepenings:
 *   - listen backlog stored/clamped; connect rejects when queue full
 *   - SOL_SOCKET subset: REUSEADDR, TYPE, ERROR, BROADCAST, KEEPALIVE,
 *     SNDBUF/RCVBUF (stored), LINGER soft, ACCEPTCONN, REUSEPORT
 *   - bind EADDRINUSE when port taken without reuse
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
/* Soft SOMAXCONN-shaped listen ceiling (slots leave room for peers). */
#define NET_LO_BACKLOG_MAX 8
#define NET_LO_RCVBUF_DEF  NET_LO_BUF
#define NET_LO_SNDBUF_DEF  NET_LO_BUF

struct net_lo_sock {
    u8  u8Used;
    u8  u8Type;
    u8  u8Listening;
    u8  u8Connected;
    u8  u8ShutRd;
    u8  u8ShutWr;
    u8  u8Reuse;
    u8  u8ReusePort;
    u8  u8Keepalive;
    u8  u8Broadcast;
    u8  u8Backlog;   /* listen queue depth soft (1..NET_LO_BACKLOG_MAX) */
    u8  u8Pending;   /* soft pending count against backlog */
    u8  u8LingerOn;
    u8  u8Pad;
    u16 u16Port;
    u16 u16LingerSec;
    u32 u32RcvBuf;
    u32 u32SndBuf;
    i16 i16Peer; /* slot of peer or -1 */
    u16 u16Pad2;
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
    kprintf("net_lo: init (loopback + peer ring + sockopt/backlog soft)\n");
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
            g_aSocks[i].u8ReusePort = 0;
            g_aSocks[i].u8Keepalive = 0;
            g_aSocks[i].u8Broadcast = 0;
            g_aSocks[i].u8Backlog = 0;
            g_aSocks[i].u8Pending = 0;
            g_aSocks[i].u8LingerOn = 0;
            g_aSocks[i].u16Port = 0;
            g_aSocks[i].u16LingerSec = 0;
            g_aSocks[i].u32RcvBuf = NET_LO_RCVBUF_DEF;
            g_aSocks[i].u32SndBuf = NET_LO_SNDBUF_DEF;
            g_aSocks[i].i16Peer = -1;
            g_aSocks[i].u32RxLen = 0;
            g_aSocks[i].u32RxHead = 0;
            return (i64)(NET_FD_BASE + i);
        }
    }
    return -24; /* EMFILE */
}

/*
 * Soft EADDRINUSE: same port + same type listener/bound socket without
 * SO_REUSEADDR (or REUSEPORT) on both sides.
 */
static int
port_in_use(u32 u32Self, u16 u16Port, u8 u8Type, u8 u8Reuse)
{
    u32 i;

    if (u16Port == 0) {
        return 0;
    }
    for (i = 0; i < NET_LO_MAX; i++) {
        if (i == u32Self || !g_aSocks[i].u8Used) {
            continue;
        }
        if (g_aSocks[i].u16Port != u16Port || g_aSocks[i].u8Type != u8Type) {
            continue;
        }
        if (u8Reuse && (g_aSocks[i].u8Reuse || g_aSocks[i].u8ReusePort)) {
            continue;
        }
        return 1;
    }
    return 0;
}

i64
net_lo_bind(i64 i64Fd, u16 u16Port)
{
    u32 u32Slot;

    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (port_in_use(u32Slot, u16Port, g_aSocks[u32Slot].u8Type,
                    g_aSocks[u32Slot].u8Reuse ||
                        g_aSocks[u32Slot].u8ReusePort)) {
        return -98; /* EADDRINUSE-shaped */
    }
    g_aSocks[u32Slot].u16Port = u16Port;
    return 0;
}

i64
net_lo_listen(i64 i64Fd, int nBacklog)
{
    u32 u32Slot;
    int nBl;

    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    /* Soft backlog: clamp; 0 means one pending (Linux-shaped). */
    nBl = nBacklog;
    if (nBl < 1) {
        nBl = 1;
    }
    if (nBl > NET_LO_BACKLOG_MAX) {
        nBl = NET_LO_BACKLOG_MAX;
    }
    g_aSocks[u32Slot].u8Backlog = (u8)nBl;
    g_aSocks[u32Slot].u8Pending = 0;
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
     * Soft backlog: reject when listener already has a pending peer and
     * pending count would exceed u8Backlog (single-slot queue today).
     */
    for (i = 0; i < NET_LO_MAX; i++) {
        if (i != u32Slot && g_aSocks[i].u8Used && g_aSocks[i].u8Listening &&
            g_aSocks[i].u16Port == u16Port) {
            if (g_aSocks[i].i16Peer >= 0 &&
                g_aSocks[i].i16Peer != (i16)u32Slot) {
                /* Queue full (one soft pending slot). */
                return -11; /* EAGAIN */
            }
            if (g_aSocks[i].u8Backlog == 0) {
                g_aSocks[i].u8Backlog = 1;
            }
            if (g_aSocks[i].u8Pending >= g_aSocks[i].u8Backlog &&
                g_aSocks[i].i16Peer >= 0) {
                return -11;
            }
            g_aSocks[i].i16Peer = (i16)u32Slot;
            if (g_aSocks[i].u8Pending < 255u) {
                g_aSocks[i].u8Pending++;
            }
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
            g_aSocks[i].u8ReusePort = 0;
            g_aSocks[i].u8Keepalive = g_aSocks[u32Slot].u8Keepalive;
            g_aSocks[i].u8Broadcast = 0;
            g_aSocks[i].u8Backlog = 0;
            g_aSocks[i].u8Pending = 0;
            g_aSocks[i].u8LingerOn = 0;
            g_aSocks[i].u16Port = g_aSocks[u32Slot].u16Port;
            g_aSocks[i].u16LingerSec = 0;
            g_aSocks[i].u32RcvBuf = g_aSocks[u32Slot].u32RcvBuf;
            g_aSocks[i].u32SndBuf = g_aSocks[u32Slot].u32SndBuf;
            g_aSocks[i].i16Peer = i16Cli;
            g_aSocks[i].u32RxLen = 0;
            g_aSocks[i].u32RxHead = 0;
            g_aSocks[i16Cli].i16Peer = (i16)i;
            g_aSocks[u32Slot].i16Peer = -1;
            if (g_aSocks[u32Slot].u8Pending > 0) {
                g_aSocks[u32Slot].u8Pending--;
            }
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
    u32 u32Cap;

    if (u32Slot >= NET_LO_MAX || !g_aSocks[u32Slot].u8Used) {
        return -1;
    }
    pS = &g_aSocks[u32Slot];
    /* Soft SO_RCVBUF: cap ring use to min(physical buf, advertised rcvbuf). */
    u32Cap = NET_LO_BUF;
    if (pS->u32RcvBuf > 0 && pS->u32RcvBuf < u32Cap) {
        u32Cap = pS->u32RcvBuf;
    }
    if (pS->u32RxLen >= u32Cap) {
        return 0;
    }
    if (cb > u32Cap - pS->u32RxLen) {
        cb = u32Cap - pS->u32RxLen;
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
    u32 u32N;

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
    /* Soft SO_SNDBUF: clamp one-shot write to advertised send buffer. */
    u32N = (u32)cb;
    if (g_aSocks[u32Slot].u32SndBuf > 0 && u32N > g_aSocks[u32Slot].u32SndBuf) {
        u32N = g_aSocks[u32Slot].u32SndBuf;
    }
    i16Peer = g_aSocks[u32Slot].i16Peer;
    if (i16Peer < 0 || (u32)i16Peer >= NET_LO_MAX) {
        /* Unpaired: loop into own RX for dgram smoke */
        i16Peer = (i16)u32Slot;
    }
    return (i64)push_rx((u32)i16Peer, pBuf, u32N);
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
        return 0; /* ignore other levels (IPPROTO soft no-op) */
    }
    if (pVal != NULL && u32Len >= 4) {
        v = *(const int *)pVal;
    }
    if (nOpt == 2 /* SO_REUSEADDR */) {
        g_aSocks[u32Slot].u8Reuse = v ? 1u : 0u;
        return 0;
    }
    if (nOpt == 15 /* SO_REUSEPORT */) {
        g_aSocks[u32Slot].u8ReusePort = v ? 1u : 0u;
        return 0;
    }
    if (nOpt == 6 /* SO_BROADCAST */) {
        g_aSocks[u32Slot].u8Broadcast = v ? 1u : 0u;
        return 0;
    }
    if (nOpt == 9 /* SO_KEEPALIVE */) {
        g_aSocks[u32Slot].u8Keepalive = v ? 1u : 0u;
        return 0;
    }
    if (nOpt == 7 /* SO_SNDBUF */) {
        if (v < 256) {
            v = 256;
        }
        if (v > 65536) {
            v = 65536;
        }
        g_aSocks[u32Slot].u32SndBuf = (u32)v;
        return 0;
    }
    if (nOpt == 8 /* SO_RCVBUF */) {
        if (v < 256) {
            v = 256;
        }
        if (v > 65536) {
            v = 65536;
        }
        g_aSocks[u32Slot].u32RcvBuf = (u32)v;
        return 0;
    }
    if (nOpt == 13 /* SO_LINGER */) {
        /* struct linger { int l_onoff; int l_linger; } — soft store only. */
        if (pVal != NULL && u32Len >= 8) {
            const int *pL = (const int *)pVal;

            g_aSocks[u32Slot].u8LingerOn = pL[0] ? 1u : 0u;
            g_aSocks[u32Slot].u16LingerSec =
                pL[1] < 0 ? 0u : (u16)(pL[1] > 65535 ? 65535 : pL[1]);
        }
        return 0;
    }
    /* SO_DEBUG=1, SO_DONTROUTE=5, SO_OOBINLINE=10, timeos — accept no-op */
    if (nOpt == 1 || nOpt == 5 || nOpt == 10 || nOpt == 20 || nOpt == 21) {
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
    } else if (nOpt == 15 /* SO_REUSEPORT */) {
        v = g_aSocks[u32Slot].u8ReusePort;
    } else if (nOpt == 6 /* SO_BROADCAST */) {
        v = g_aSocks[u32Slot].u8Broadcast;
    } else if (nOpt == 9 /* SO_KEEPALIVE */) {
        v = g_aSocks[u32Slot].u8Keepalive;
    } else if (nOpt == 7 /* SO_SNDBUF */) {
        v = (int)g_aSocks[u32Slot].u32SndBuf;
    } else if (nOpt == 8 /* SO_RCVBUF */) {
        v = (int)g_aSocks[u32Slot].u32RcvBuf;
    } else if (nOpt == 30 /* SO_ACCEPTCONN */) {
        v = g_aSocks[u32Slot].u8Listening ? 1 : 0;
    } else if (nOpt == 13 /* SO_LINGER */) {
        if (*pLen < 8) {
            return -22;
        }
        ((int *)pVal)[0] = g_aSocks[u32Slot].u8LingerOn ? 1 : 0;
        ((int *)pVal)[1] = (int)g_aSocks[u32Slot].u16LingerSec;
        *pLen = 8;
        return 0;
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
    i16 peer;

    if (!net_lo_fd_ok(i64Fd)) {
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    peer = g_aSocks[u32Slot].i16Peer;
    if (peer >= 0 && (u32)peer < NET_LO_MAX) {
        if (g_aSocks[peer].i16Peer == (i16)u32Slot) {
            g_aSocks[peer].i16Peer = -1;
        }
        /* Soft half-close: peer sees RD shutdown if we were connected. */
        if (g_aSocks[u32Slot].u8Connected && !g_aSocks[u32Slot].u8Listening) {
            g_aSocks[peer].u8ShutRd = 1;
        }
        if (g_aSocks[peer].u8Listening && g_aSocks[peer].u8Pending > 0) {
            g_aSocks[peer].u8Pending--;
        }
    }
    memset(&g_aSocks[u32Slot], 0, sizeof(g_aSocks[u32Slot]));
    g_aSocks[u32Slot].i16Peer = -1;
    return 0;
}
