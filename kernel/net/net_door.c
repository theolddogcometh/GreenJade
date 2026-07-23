/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Net door: eth poll/stats + socket ops for netstackd hand-off.
 * Pure C11, dual-licensed (MIT OR Apache-2.0).
 *
 * STREAM (SOCK_STREAM) → net_tcp (virtio + local pair); DGRAM → net_lo.
 * TCP_STATS packing (u32[4]):
 *   [0] accepts
 *   [1] low16=segments, high16=retransmits
 *   [2] rx_bytes
 *   [3] tx_bytes (full 32-bit)
 */
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/net_door.h>
#include <gj/net_eth.h>
#include <gj/net_lo.h>
#include <gj/net_tcp.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/virtio_net.h>

#define SOCK_STREAM 1
#define SOCK_DGRAM  2

/*
 * Door bounce buffer for SEND/RECV (and virtio frame copy helpers).
 *
 * Multi-seg TCP contract (keep high enough — do not shrink below bulk):
 *   netstackd freestanding smoke sends 3000 B in one GJ_NET_OP_SEND
 *   (MSS = 1024 → ceil(3000/1024) = 3 payload segments inside net_tcp_send).
 *   NET_XFER_MAX must be ≥ 3000; 4096 matches TCP_RX_MAX / TCP_TX_MAX.
 * Eth/virtio L2 frames still reject >1514 inside virtio_net_tx.
 */
#define NET_XFER_MAX 4096u
#define NET_ETH_MAX  1514u

/* Keep multi-seg room: bounce ≥ bulk smoke and > one MSS. */
typedef char net_xfer_ge_bulk[(NET_XFER_MAX >= 3000u) ? 1 : -1];
typedef char net_xfer_ge_mss[(NET_XFER_MAX > 1024u) ? 1 : -1];

static int g_fInit;
static u32 g_u32Calls;
static u32 g_u32OwnerToken;
static u32 g_u32VqCalls;

void
net_door_init(void)
{
    g_fInit = 1;
    g_u32Calls = 0;
    g_u32OwnerToken = 0;
    g_u32VqCalls = 0;
    kprintf("net_door: init\n");
}

int
net_door_owned(void)
{
    return g_u32OwnerToken != 0;
}

i64
net_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    if (!g_fInit) {
        return GJ_ERR_NODEV;
    }
    g_u32Calls++;
    switch (u32Op) {
    case GJ_NET_OP_CLAIM:
        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            return GJ_ERR_BUSY;
        }
        g_u32OwnerToken = (u32)u64Arg1;
        kprintf("net_door: CLAIM token=0x%x\n", g_u32OwnerToken);
        return 0;
    case GJ_NET_OP_RELEASE:
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            return GJ_ERR_INVAL;
        }
        g_u32OwnerToken = 0;
        kprintf("net_door: RELEASE\n");
        return 0;
    case GJ_NET_OP_POLL:
        net_eth_poll();
        return 0;
    case GJ_NET_OP_STATS: {
        u32 aSt[4];

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        aSt[0] = net_eth_arp_replies();
        aSt[1] = net_eth_udp_echoes();
        aSt[2] = net_eth_icmp_echoes();
        aSt[3] = g_u32Calls;
        if (user_range_ok(u64Arg1, sizeof(aSt))) {
            if (copy_to_user(u64Arg1, aSt, sizeof(aSt)) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aSt, sizeof(aSt));
        }
        return 0;
    }
    case GJ_NET_OP_SOCKET:
        /* AF_INET=2: SOCK_STREAM → TCP (virtio eth + loopback pair);
         * SOCK_DGRAM → net_lo UDP-shaped path. */
        if ((int)u64Arg2 == SOCK_STREAM) {
            return net_tcp_socket();
        }
        return net_lo_socket((int)u64Arg1, (int)u64Arg2, (int)u64Arg3);
    case GJ_NET_OP_BIND:
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_tcp_bind((i64)u64Arg1, (u16)u64Arg2);
        }
        return net_lo_bind((i64)u64Arg1, (u16)u64Arg2);
    case GJ_NET_OP_LISTEN:
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_tcp_listen((i64)u64Arg1, (int)u64Arg2);
        }
        return net_lo_listen((i64)u64Arg1, (int)u64Arg2);
    case GJ_NET_OP_ACCEPT:
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_tcp_accept((i64)u64Arg1);
        }
        return net_lo_accept((i64)u64Arg1);
    case GJ_NET_OP_CONNECT:
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_tcp_connect((i64)u64Arg1, (u16)u64Arg2);
        }
        return net_lo_connect((i64)u64Arg1, (u16)u64Arg2);
    case GJ_NET_OP_CLOSE:
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_tcp_close((i64)u64Arg1);
        }
        return net_lo_close((i64)u64Arg1);
    case GJ_NET_OP_SEND: {
        /* Bounce ≤ NET_XFER_MAX; TCP multi-seg chunks by MSS inside net_tcp. */
        u8 aBuf[NET_XFER_MAX];
        size_t cb = (size_t)u64Arg3;
        i64 n;

        if (u64Arg2 == 0 || cb == 0) {
            return GJ_ERR_INVAL;
        }
        if (cb > NET_XFER_MAX) {
            cb = NET_XFER_MAX;
        }
        if (user_range_ok(u64Arg2, cb)) {
            if (copy_from_user(aBuf, u64Arg2, cb) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy(aBuf, (const void *)(gj_vaddr_t)u64Arg2, cb);
        }
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            n = net_tcp_send((i64)u64Arg1, aBuf, cb);
        } else {
            n = net_lo_send((i64)u64Arg1, aBuf, cb);
        }
        return n;
    }
    case GJ_NET_OP_RECV: {
        /* Multi-seg drain may take several RECV calls; each ≤ NET_XFER_MAX. */
        u8 aBuf[NET_XFER_MAX];
        size_t cb = (size_t)u64Arg3;
        i64 n;

        if (u64Arg2 == 0 || cb == 0) {
            return GJ_ERR_INVAL;
        }
        if (cb > NET_XFER_MAX) {
            cb = NET_XFER_MAX;
        }
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            n = net_tcp_recv((i64)u64Arg1, aBuf, cb);
        } else {
            n = net_lo_recv((i64)u64Arg1, aBuf, cb);
        }
        if (n > 0) {
            if ((size_t)n > NET_XFER_MAX) {
                return GJ_ERR_IO; /* defensive; recv never exceeds bounce */
            }
            if (user_range_ok(u64Arg2, (size_t)n)) {
                if (copy_to_user(u64Arg2, aBuf, (size_t)n) != GJ_OK) {
                    return GJ_ERR_FAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)u64Arg2, aBuf, (size_t)n);
            }
        }
        return n;
    }
    case GJ_NET_OP_TCP_STATS: {
        u32 aSt[4];
        u32 segs;
        u32 rtx;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        /* Prefer full tx_bytes; pack rtx into segs high half (segs rarely wrap). */
        segs = net_tcp_segments();
        rtx = net_tcp_retransmits();
        aSt[0] = net_tcp_accepts();
        aSt[1] = (rtx << 16) | (segs & 0xffffu);
        aSt[2] = net_tcp_bytes_rx();
        aSt[3] = net_tcp_bytes_tx();
        if (user_range_ok(u64Arg1, sizeof(aSt))) {
            if (copy_to_user(u64Arg1, aSt, sizeof(aSt)) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aSt, sizeof(aSt));
        }
        return 0;
    }
    case GJ_NET_OP_VIRTIO_TX: {
        /* L2 frame path: bound to NET_ETH_MAX (driver also rejects >1514). */
        u8 aFrame[NET_ETH_MAX];
        u32 cb = (u32)u64Arg2;

        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
        }
        if (u64Arg1 == 0 || cb == 0 || cb > NET_ETH_MAX) {
            return GJ_ERR_INVAL;
        }
        if (user_range_ok(u64Arg1, cb)) {
            if (copy_from_user(aFrame, u64Arg1, cb) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy(aFrame, (const void *)(gj_vaddr_t)u64Arg1, cb);
        }
        g_u32VqCalls++;
        if (virtio_net_tx(aFrame, cb) != 0) {
            return GJ_ERR_IO;
        }
        return (i64)cb;
    }
    case GJ_NET_OP_VIRTIO_RX: {
        u8 aFrame[NET_ETH_MAX];
        u32 cbMax = (u32)u64Arg2;
        i32 n;

        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
        }
        if (u64Arg1 == 0 || cbMax == 0) {
            return GJ_ERR_INVAL;
        }
        if (cbMax > NET_ETH_MAX) {
            cbMax = NET_ETH_MAX;
        }
        g_u32VqCalls++;
        n = virtio_net_rx(aFrame, cbMax);
        if (n < 0) {
            return GJ_ERR_IO;
        }
        if (n == 0) {
            return 0;
        }
        if ((u32)n > NET_ETH_MAX) {
            return GJ_ERR_IO;
        }
        if (user_range_ok(u64Arg1, (u32)n)) {
            if (copy_to_user(u64Arg1, aFrame, (size_t)n) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aFrame, (size_t)n);
        }
        return (i64)n;
    }
    case GJ_NET_OP_QUEUE_INFO: {
        u32 aQ[5];

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        aQ[0] = virtio_net_tx_count();
        aQ[1] = virtio_net_rx_count();
        aQ[2] = virtio_net_ready() ? 1u : 0u;
        aQ[3] = g_u32OwnerToken ? 1u : 0u;
        aQ[4] = g_u32VqCalls;
        if (user_range_ok(u64Arg1, sizeof(aQ))) {
            if (copy_to_user(u64Arg1, aQ, sizeof(aQ)) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aQ, sizeof(aQ));
        }
        return 0;
    }
    case GJ_NET_OP_EXPORT_RING: {
        struct gj_virtq_export ex;

        if (u64Arg2 == 0) {
            return GJ_ERR_INVAL;
        }
        if (virtio_net_export_q((u16)u64Arg1, &ex) != 0) {
            return GJ_ERR_NODEV;
        }
        g_u32VqCalls++;
        if (user_range_ok(u64Arg2, sizeof(ex))) {
            if (copy_to_user(u64Arg2, &ex, sizeof(ex)) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg2, &ex, sizeof(ex));
        }
        return 0;
    }
    case GJ_NET_OP_MAP_RING: {
        struct gj_virtq_export ex;

        if (virtio_net_map_q_user((u16)u64Arg1, u64Arg2, &ex) != 0) {
            return GJ_ERR_FAULT;
        }
        g_u32VqCalls++;
        if (u64Arg3 != 0) {
            if (user_range_ok(u64Arg3, sizeof(ex))) {
                if (copy_to_user(u64Arg3, &ex, sizeof(ex)) != GJ_OK) {
                    return GJ_ERR_FAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)u64Arg3, &ex, sizeof(ex));
            }
        }
        return 0;
    }
    case GJ_NET_OP_KICK:
        if (virtio_net_kick_q((u16)u64Arg1) != 0) {
            return GJ_ERR_NODEV;
        }
        g_u32VqCalls++;
        return 0;
    case GJ_NET_OP_AVAIL_PUSH: {
        u8 aBuf[NET_ETH_MAX];
        u32 cb = (u32)u64Arg3;
        u16 which = (u16)u64Arg1;
        int fWrite = (which == 0) ? 1 : 0;

        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
        }
        if (u64Arg2 == 0 || cb == 0 || cb > NET_ETH_MAX) {
            return GJ_ERR_INVAL;
        }
        if (user_range_ok(u64Arg2, cb)) {
            if (copy_from_user(aBuf, u64Arg2, cb) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy(aBuf, (const void *)(gj_vaddr_t)u64Arg2, cb);
        }
        g_u32VqCalls++;
        /* Ring programming path: AVAIL push + kick (no full TX helper) */
        if (virtio_net_avail_push(which, aBuf, cb, fWrite, 1) != 0) {
            return GJ_ERR_IO;
        }
        return (i64)cb;
    }
    case GJ_NET_OP_USED_REAP: {
        u32 n;

        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
        }
        n = virtio_net_used_reap((u16)u64Arg1, (u32)u64Arg2);
        g_u32VqCalls++;
        return (i64)n;
    }
    case GJ_NET_OP_RING_STATE: {
        u32 aS[4];

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        aS[0] = virtio_net_q_free(1);
        aS[1] = virtio_net_q_free(0);
        /* high 16: user_ring_pushes, low 16: total avail pushes */
        aS[2] = (virtio_net_user_ring_pushes() << 16) |
                (virtio_net_avail_pushes() & 0xffffu);
        aS[3] = g_u32VqCalls;
        if (user_range_ok(u64Arg1, sizeof(aS))) {
            if (copy_to_user(u64Arg1, aS, sizeof(aS)) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy((void *)(gj_vaddr_t)u64Arg1, aS, sizeof(aS));
        }
        return 0;
    }
    case GJ_NET_OP_MAP_DMA: {
        struct gj_virtq_dma_export ex;

        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        if (virtio_net_map_dma_user(u64Arg1, &ex) != 0) {
            return GJ_ERR_FAULT;
        }
        g_u32VqCalls++;
        if (u64Arg2 != 0) {
            if (user_range_ok(u64Arg2, sizeof(ex))) {
                if (copy_to_user(u64Arg2, &ex, sizeof(ex)) != GJ_OK) {
                    return GJ_ERR_FAULT;
                }
            } else {
                memcpy((void *)(gj_vaddr_t)u64Arg2, &ex, sizeof(ex));
            }
        }
        return 0;
    }
    case GJ_NET_OP_DESC_ALLOC: {
        int head;

        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
        }
        head = virtio_net_desc_alloc((u16)u64Arg1);
        if (head < 0) {
            return GJ_ERR_NOMEM;
        }
        g_u32VqCalls++;
        return (i64)head;
    }
    case GJ_NET_OP_USER_AVAIL: {
        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
        }
        if (virtio_net_user_avail((u16)u64Arg1, (u16)u64Arg2, (int)u64Arg3) !=
            0) {
            return GJ_ERR_IO;
        }
        g_u32VqCalls++;
        return 0;
    }
    case GJ_NET_OP_BOUNCE_FILL: {
        u8 aBuf[NET_ETH_MAX];
        u32 cb = (u32)u64Arg3;
        int n;

        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
        }
        if (u64Arg2 == 0 || cb == 0 || cb > NET_ETH_MAX) {
            return GJ_ERR_INVAL;
        }
        if (user_range_ok(u64Arg2, cb)) {
            if (copy_from_user(aBuf, u64Arg2, cb) != GJ_OK) {
                return GJ_ERR_FAULT;
            }
        } else {
            memcpy(aBuf, (const void *)(gj_vaddr_t)u64Arg2, cb);
        }
        /* slot in arg1; TX header prepend for net */
        n = virtio_net_bounce_fill((u32)u64Arg1, aBuf, cb, 1);
        if (n < 0) {
            return GJ_ERR_IO;
        }
        g_u32VqCalls++;
        return (i64)n;
    }
    default:
        return GJ_ERR_NOSUPPORT;
    }
}
