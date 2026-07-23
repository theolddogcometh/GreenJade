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
 *
 * Ownership: token 0 means kernel interim owns policy; non-zero means
 * netstackd claimed the door. Claim is re-entrant for the same token
 * (idempotent reclaim soft), BUSY for a different token. RELEASE when
 * free is soft 0. Queue / ring ops allowed without claim for bring-up
 * smokes (owned path preferred by product netstackd).
 *
 * Ring soft path (netstackd / UDX):
 *   EXPORT/MAP/KICK → NODEV when virtio-net is absent (client soft-skips).
 *   RING_STATE always succeeds: free=0 pushes=0 without device.
 *   MAP records last user VA for diagnostics; re-MAP of the same VA is a
 *   soft reclaim of the map (re-install PTEs, re-export).
 *
 * User pointers: prefer user_range_ok + copy_{to,from}_user. The !user
 * branch is for early kernel smokes that pass HHDM/static buffers.
 *
 * Product PASS markers (main.c) depend on CLAIM/RELEASE, QUEUE_INFO,
 * EXPORT/MAP, AVAIL_PUSH, RING_STATE, and USER_AVAIL wire semantics —
 * keep those ABI-stable.
 */
#include <gj/config.h>
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
static u32 g_u32OwnerToken; /* 0 = kernel interim owns */
static u32 g_u32VqCalls;
static u32 g_u32Claims;     /* successful first claims */
static u32 g_u32Reclaims;   /* idempotent same-token CLAIM soft path */
static u32 g_u32RingCalls;  /* EXPORT/MAP/KICK/RING_STATE soft ops */
static u64 g_u64RingMapVa;  /* last successful MAP_RING base (0 = none) */

void
net_door_init(void)
{
    g_fInit = 1;
    g_u32Calls = 0;
    g_u32OwnerToken = 0;
    g_u32VqCalls = 0;
    g_u32Claims = 0;
    g_u32Reclaims = 0;
    g_u32RingCalls = 0;
    g_u64RingMapVa = 0;
    kprintf("net_door: init (claim+ring soft)\n");
}

int
net_door_owned(void)
{
    return g_u32OwnerToken != 0;
}

u32
net_door_owner_token(void)
{
    return g_u32OwnerToken;
}

u64
net_door_ring_map_va(void)
{
    return g_u64RingMapVa;
}

u32
net_door_ring_calls(void)
{
    return g_u32RingCalls;
}

u32
net_door_claim_count(void)
{
    /* Soft diagnostics: first claims + idempotent reclaims. */
    return g_u32Claims + g_u32Reclaims;
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
        /* arg1 = non-zero ownership token (low 32 bits only). */
        if (u64Arg1 == 0 || (u64Arg1 >> 32) != 0) {
            return GJ_ERR_INVAL;
        }
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            return GJ_ERR_BUSY; /* another netstackd */
        }
        /* Soft reclaim: same token re-CLAIM is idempotent (no re-log). */
        if (g_u32OwnerToken == (u32)u64Arg1) {
            g_u32Reclaims++;
            return 0;
        }
        g_u32OwnerToken = (u32)u64Arg1;
        g_u32Claims++;
        kprintf("net_door: CLAIM token=0x%x (userspace owns net)\n",
                g_u32OwnerToken);
        return 0;
    case GJ_NET_OP_RELEASE:
        /* Soft free path: already unowned → 0 (no token match required). */
        if (g_u32OwnerToken == 0) {
            return 0;
        }
        if ((u64Arg1 >> 32) != 0 || (u32)u64Arg1 != g_u32OwnerToken) {
            return GJ_ERR_INVAL;
        }
        kprintf("net_door: RELEASE token=0x%x\n", g_u32OwnerToken);
        g_u32OwnerToken = 0;
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

        g_u32RingCalls++;
        if (u64Arg2 == 0) {
            return GJ_ERR_INVAL;
        }
        /* Soft-skip surface: no virtio-net → NODEV (netstackd soft-logs). */
        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
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
        int fRemap;

        g_u32RingCalls++;
        if (u64Arg2 == 0) {
            return GJ_ERR_INVAL;
        }
        /* VA base must be page-aligned for ring map into user AS. */
        if ((u64Arg2 & (GJ_PAGE_SIZE - 1ull)) != 0) {
            return GJ_ERR_INVAL;
        }
        /* Soft-skip surface: no virtio-net → NODEV (distinct from map FAULT). */
        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
        }
        /*
         * Soft re-MAP of the same VA: re-install PTEs + re-export (idempotent
         * hand-off for netstackd / UDX reclaim of the map window).
         */
        fRemap = (g_u64RingMapVa != 0 && g_u64RingMapVa == u64Arg2) ? 1 : 0;
        if (virtio_net_map_q_user((u16)u64Arg1, u64Arg2, &ex) != 0) {
            return GJ_ERR_FAULT;
        }
        g_u64RingMapVa = u64Arg2;
        g_u32VqCalls++;
        /* fRemap: soft re-MAP of same VA (PTE re-install); ring_calls covers it. */
        (void)fRemap;
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
        g_u32RingCalls++;
        /* Soft-skip when virtio-net absent; kick is best-effort notify. */
        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
        }
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
        /* Soft: always fills state; free/pushes = 0 without virtio-net. */
        u32 aS[4];

        g_u32RingCalls++;
        if (u64Arg1 == 0) {
            return GJ_ERR_INVAL;
        }
        if (virtio_net_ready()) {
            aS[0] = virtio_net_q_free(1);
            aS[1] = virtio_net_q_free(0);
            /* high 16: user_ring_pushes, low 16: total avail pushes */
            aS[2] = (virtio_net_user_ring_pushes() << 16) |
                    (virtio_net_avail_pushes() & 0xffffu);
        } else {
            aS[0] = 0;
            aS[1] = 0;
            aS[2] = 0;
        }
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
        if ((u64Arg1 & (GJ_PAGE_SIZE - 1ull)) != 0) {
            return GJ_ERR_INVAL;
        }
        if (!virtio_net_ready()) {
            return GJ_ERR_NODEV;
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
