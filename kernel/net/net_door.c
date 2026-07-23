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
 *
 * Soft product inventory (Wave 16 exclusive deepen; file-local sticky).
 * Never hard-gates; wrap OK; diagnostics only. Soft ≠ bar3.
 * Greppable prefix-stable serial markers (net_door: soft …):
 *   net_door: soft inventory   — owned/token/calls/vq/ring catalog + wave
 *   net_door: soft claim       — claim/reclaim/busy/release path tallies
 *   net_door: soft sock        — socket path enter + tcp|lo + ok|fail
 *   net_door: soft ring        — EXPORT/MAP/KICK/AVAIL/USER enter surface
 *   net_door: soft ring_ok     — ring outcome ok|inval|fault|nodev|io splits
 *   net_door: soft virtio      — TX/RX/QUEUE enter + ready + nodev soft-skip
 *   net_door: soft virtio_ok   — virtio outcome + TX/RX byte totals
 *   net_door: soft xfer        — SEND/RECV ok|fail + door byte totals
 *   net_door: soft last        — last opcode + terminal ret snapshot
 *   net_door: soft err         — INVAL/BUSY/NODEV/FAULT/IO/NOSUPPORT
 *   net_door: soft group       — Wave 15 enter-group rollup (claim/sock/…)
 *   net_door: soft capacity    — Wave 15 fixed xfer/eth/bounce lamps
 *   net_door: soft catalog     — Wave 15 opcode soft catalog (impl lamps)
 *   net_door: soft outcome     — Wave 15 ok|err|nodev rollup
 *   net_door: soft stats       — aggregate enter + group counters
 *   net_door: soft backend     — virtio-net live + tcp segs/accepts snapshot
 *   net_door: soft path        — honesty: soft inventory ≠ bar3 / product
 *   net_door: soft deepen      — wave=16 stamp + area count
 *   net_door: soft inventory PASS / net_door: soft PASS
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
/* Wave 16 exclusive soft deepen stamp (greppable wave=16). */
#define NET_DOOR_SOFT_DEEPEN_WAVE  16u
/* inventory claim sock ring ring_ok virtio virtio_ok xfer last err
 * group capacity catalog outcome stats backend path
 * headroom surface ratio deepen PASS = 22 */
#define NET_DOOR_SOFT_DEEPEN_AREAS 22u

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

/*
 * Soft product inventory (Wave 16 exclusive). Cumulative path tallies.
 * greppable: net_door: soft …
 */
struct net_door_soft {
    u64 u64Enter;          /* net_door_call entries */
    u64 u64NotInit;        /* call before init → NODEV */
    u64 u64ClaimOk;        /* first CLAIM success */
    u64 u64ClaimReclaim;   /* same-token re-CLAIM soft */
    u64 u64ClaimBusy;      /* different token → BUSY */
    u64 u64ClaimInval;     /* token 0 / high bits */
    u64 u64ReleaseOk;      /* RELEASE matched token */
    u64 u64ReleaseFree;    /* RELEASE when free (soft 0) */
    u64 u64ReleaseInval;   /* RELEASE token mismatch */
    u64 u64Poll;
    u64 u64Stats;
    u64 u64StatsInval;
    u64 u64StatsFault;
    u64 u64Socket;
    u64 u64Bind;
    u64 u64Listen;
    u64 u64Accept;
    u64 u64Connect;
    u64 u64Close;
    u64 u64Send;
    u64 u64Recv;
    u64 u64TcpStats;
    u64 u64TcpStatsInval;
    u64 u64TcpStatsFault;
    /* Wave 15: socket path routing + outcome */
    u64 u64SockTcp;        /* ops routed to net_tcp */
    u64 u64SockLo;         /* ops routed to net_lo */
    u64 u64SockOk;         /* socket-family non-neg ret */
    u64 u64SockFail;       /* socket-family neg ret */
    u64 u64SockOwned;      /* socket-family while owned */
    u64 u64SockUnowned;    /* socket-family while kernel interim */
    u64 u64SendOk;
    u64 u64SendFail;
    u64 u64RecvOk;         /* n >= 0 (incl soft 0 EOF) */
    u64 u64RecvFail;
    u64 u64SendBytes;      /* cumulative SEND bytes (ok) */
    u64 u64RecvBytes;      /* cumulative RECV bytes (ok, n>0) */
    u64 u64VirtioTx;
    u64 u64VirtioRx;
    u64 u64QueueInfo;
    u64 u64QueueOk;
    u64 u64QueueInval;
    u64 u64QueueFault;
    /* Wave 15: virtio outcome splits */
    u64 u64VirtioTxOk;
    u64 u64VirtioTxInval;
    u64 u64VirtioTxNodev;
    u64 u64VirtioTxFault;
    u64 u64VirtioTxIo;
    u64 u64VirtioRxOk;
    u64 u64VirtioRxInval;
    u64 u64VirtioRxNodev;
    u64 u64VirtioRxFault;
    u64 u64VirtioRxIo;
    u64 u64VirtioTxBytes;  /* cumulative door TX bytes (ok) */
    u64 u64VirtioRxBytes;  /* cumulative door RX bytes (ok, n>0) */
    u64 u64ExportRing;
    u64 u64MapRing;
    u64 u64MapRemap;       /* soft re-MAP of same VA */
    u64 u64Kick;
    u64 u64AvailPush;
    u64 u64UsedReap;
    u64 u64RingState;
    u64 u64MapDma;
    u64 u64DescAlloc;
    u64 u64UserAvail;
    u64 u64BounceFill;
    /* Wave 15: ring outcome splits */
    u64 u64ExportOk;
    u64 u64ExportInval;
    u64 u64ExportFault;
    u64 u64ExportNodev;
    u64 u64MapOk;          /* first successful map (not remap) */
    u64 u64MapInval;
    u64 u64MapNodev;
    u64 u64MapFault;
    u64 u64KickOk;
    u64 u64KickNodev;
    u64 u64RingStateOk;
    u64 u64RingStateInval;
    u64 u64RingStateFault;
    u64 u64AvailOk;
    u64 u64AvailInval;
    u64 u64AvailNodev;
    u64 u64AvailFault;
    u64 u64AvailIo;
    u64 u64ReapOk;
    u64 u64ReapNodev;
    u64 u64MapDmaOk;
    u64 u64MapDmaInval;
    u64 u64MapDmaNodev;
    u64 u64MapDmaFault;
    u64 u64DescOk;
    u64 u64DescNodev;
    u64 u64DescNomem;
    u64 u64UserAvailOk;
    u64 u64UserAvailNodev;
    u64 u64UserAvailIo;
    u64 u64BounceOk;
    u64 u64BounceInval;
    u64 u64BounceNodev;
    u64 u64BounceFault;
    u64 u64BounceIo;
    /* Wave 15: user vs kernel-smoke copy path */
    u64 u64UserCopy;
    u64 u64KernelCopy;
    u64 u64Nodev;          /* virtio-absent soft-skip terminals */
    u64 u64Inval;          /* INVAL terminals (arg / policy) */
    u64 u64Busy;           /* BUSY terminals */
    u64 u64Fault;          /* FAULT terminals */
    u64 u64Io;             /* IO terminals */
    u64 u64Nomem;          /* NOMEM terminals */
    u64 u64Nosupport;      /* unknown op */
    u64 u64Ok;             /* non-negative returns */
    u64 u64SoftLog;        /* inventory log emissions */
    /* Wave 15: last-op snapshot */
    u32 u32LastOp;
    i64 i64LastRet;
};

static struct net_door_soft g_soft;
static u8 g_fSoftOnce; /* one-shot after first product call activity */

static void net_door_soft_inc(u64 *pCtr);
static void net_door_soft_add64(u64 *pCtr, u64 u64N);
static void net_door_soft_note_ret(i64 i64Ret);
static void net_door_soft_inventory_log(void);
static void net_door_soft_maybe_once(void);
static i64 net_door_soft_done(i64 i64Ret);
static i64 net_door_soft_sock_done(i64 i64Ret, int fTcp);
static i64 net_door_soft_xfer_done(i64 i64Ret, int fSend, int fTcp);

/** Soft: bump path tally (u64 wrap is fine for telemetry). */
static void
net_door_soft_inc(u64 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/** Soft: add to u64 path tally (wrap OK for telemetry). */
static void
net_door_soft_add64(u64 *pCtr, u64 u64N)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr) += u64N;
}

/**
 * Soft: classify terminal return (diagnostics only; never alters ret).
 * greppable: net_door: soft err
 */
static void
net_door_soft_note_ret(i64 i64Ret)
{
    if (i64Ret >= 0) {
        net_door_soft_inc(&g_soft.u64Ok);
        return;
    }
    if (i64Ret == GJ_ERR_NODEV) {
        net_door_soft_inc(&g_soft.u64Nodev);
    } else if (i64Ret == GJ_ERR_INVAL) {
        net_door_soft_inc(&g_soft.u64Inval);
    } else if (i64Ret == GJ_ERR_BUSY) {
        net_door_soft_inc(&g_soft.u64Busy);
    } else if (i64Ret == GJ_ERR_FAULT) {
        net_door_soft_inc(&g_soft.u64Fault);
    } else if (i64Ret == GJ_ERR_IO) {
        net_door_soft_inc(&g_soft.u64Io);
    } else if (i64Ret == GJ_ERR_NOMEM) {
        net_door_soft_inc(&g_soft.u64Nomem);
    } else if (i64Ret == GJ_ERR_NOSUPPORT) {
        net_door_soft_inc(&g_soft.u64Nosupport);
    }
}

/**
 * Greppable soft net door inventory (product / smoke). Wave 16 deepen.
 *   net_door: soft inventory …
 *   net_door: soft claim …
 *   net_door: soft sock …
 *   net_door: soft ring …
 *   net_door: soft ring_ok …
 *   net_door: soft virtio …
 *   net_door: soft virtio_ok …
 *   net_door: soft xfer …
 *   net_door: soft last …
 *   net_door: soft err …
 *   net_door: soft group …
 *   net_door: soft capacity …
 *   net_door: soft catalog …
 *   net_door: soft outcome …
 *   net_door: soft stats …
 *   net_door: soft backend …
 *   net_door: soft path …
 *   net_door: soft deepen …
 *   net_door: soft inventory PASS / net_door: soft PASS
 * greppable: net_door: soft
 * Soft only — never hard-gates; soft ≠ bar3.
 */
static void
net_door_soft_inventory_log(void)
{
    struct net_door_soft s;
    u32 u32Owned;
    u32 u32Ready;
    u32 u32TxCnt;
    u32 u32RxCnt;
    u32 u32QFreeTx;
    u32 u32QFreeRx;
    u32 u32AvailP;
    u32 u32UserP;
    u32 u32Wave;
    u32 u32Areas;
    u64 u64SockEnter;
    u64 u64RingEnter;
    u64 u64VirtioEnter;
    u64 u64ClaimEnter;
    int fSoftPass;

    u32Wave = NET_DOOR_SOFT_DEEPEN_WAVE;
    u32Areas = NET_DOOR_SOFT_DEEPEN_AREAS;
    net_door_soft_inc(&g_soft.u64SoftLog);
    s = g_soft;
    u32Owned = (g_u32OwnerToken != 0) ? 1u : 0u;
    u32Ready = virtio_net_ready() ? 1u : 0u;
    u32TxCnt = (u32Ready != 0) ? virtio_net_tx_count() : 0u;
    u32RxCnt = (u32Ready != 0) ? virtio_net_rx_count() : 0u;
    u32QFreeTx = (u32Ready != 0) ? (u32)virtio_net_q_free(1) : 0u;
    u32QFreeRx = (u32Ready != 0) ? (u32)virtio_net_q_free(0) : 0u;
    u32AvailP = (u32Ready != 0) ? virtio_net_avail_pushes() : 0u;
    u32UserP = (u32Ready != 0) ? virtio_net_user_ring_pushes() : 0u;
    u64SockEnter = s.u64Socket + s.u64Bind + s.u64Listen + s.u64Accept +
                   s.u64Connect + s.u64Close + s.u64Send + s.u64Recv +
                   s.u64TcpStats;
    u64RingEnter = s.u64ExportRing + s.u64MapRing + s.u64Kick +
                   s.u64AvailPush + s.u64UsedReap + s.u64RingState +
                   s.u64MapDma + s.u64DescAlloc + s.u64UserAvail +
                   s.u64BounceFill;
    u64VirtioEnter = s.u64VirtioTx + s.u64VirtioRx + s.u64QueueInfo;
    u64ClaimEnter = s.u64ClaimOk + s.u64ClaimReclaim + s.u64ClaimBusy +
                    s.u64ClaimInval + s.u64ReleaseOk + s.u64ReleaseFree +
                    s.u64ReleaseInval;

    /* Grep: net_door: soft inventory */
    kprintf("net_door: soft inventory init=%u owned=%u token=0x%x "
            "calls=%u vq=%u ring_calls=%u ring_va=0x%lx claims=%u "
            "reclaims=%u virtio_ready=%u xfer_max=%u eth_max=%u "
            "log_n=%lu areas=%u wave=%u\n",
            g_fInit ? 1u : 0u, u32Owned, g_u32OwnerToken, g_u32Calls,
            g_u32VqCalls, g_u32RingCalls, (unsigned long)g_u64RingMapVa,
            g_u32Claims, g_u32Reclaims, u32Ready, NET_XFER_MAX, NET_ETH_MAX,
            (unsigned long)s.u64SoftLog, u32Areas, u32Wave);

    /* Grep: net_door: soft claim */
    kprintf("net_door: soft claim ok=%lu reclaim=%lu busy=%lu inval=%lu "
            "release_ok=%lu release_free=%lu release_inval=%lu wave=%u\n",
            (unsigned long)s.u64ClaimOk, (unsigned long)s.u64ClaimReclaim,
            (unsigned long)s.u64ClaimBusy, (unsigned long)s.u64ClaimInval,
            (unsigned long)s.u64ReleaseOk, (unsigned long)s.u64ReleaseFree,
            (unsigned long)s.u64ReleaseInval, u32Wave);

    /* Grep: net_door: soft sock */
    kprintf("net_door: soft sock poll=%lu stats=%lu socket=%lu bind=%lu "
            "listen=%lu accept=%lu connect=%lu close=%lu send=%lu "
            "recv=%lu tcp_stats=%lu tcp=%lu lo=%lu ok=%lu fail=%lu "
            "owned=%lu unowned=%lu wave=%u\n",
            (unsigned long)s.u64Poll, (unsigned long)s.u64Stats,
            (unsigned long)s.u64Socket, (unsigned long)s.u64Bind,
            (unsigned long)s.u64Listen, (unsigned long)s.u64Accept,
            (unsigned long)s.u64Connect, (unsigned long)s.u64Close,
            (unsigned long)s.u64Send, (unsigned long)s.u64Recv,
            (unsigned long)s.u64TcpStats, (unsigned long)s.u64SockTcp,
            (unsigned long)s.u64SockLo, (unsigned long)s.u64SockOk,
            (unsigned long)s.u64SockFail, (unsigned long)s.u64SockOwned,
            (unsigned long)s.u64SockUnowned, u32Wave);

    /* Grep: net_door: soft ring (enter surface; prefix-stable) */
    kprintf("net_door: soft ring export=%lu map=%lu remap=%lu kick=%lu "
            "avail=%lu reap=%lu state=%lu dma=%lu desc=%lu user_avail=%lu "
            "bounce=%lu wave=%u\n",
            (unsigned long)s.u64ExportRing, (unsigned long)s.u64MapRing,
            (unsigned long)s.u64MapRemap, (unsigned long)s.u64Kick,
            (unsigned long)s.u64AvailPush, (unsigned long)s.u64UsedReap,
            (unsigned long)s.u64RingState, (unsigned long)s.u64MapDma,
            (unsigned long)s.u64DescAlloc, (unsigned long)s.u64UserAvail,
            (unsigned long)s.u64BounceFill, u32Wave);

    /* Grep: net_door: soft ring_ok (Wave 15 outcome deepen) */
    kprintf("net_door: soft ring_ok export_ok=%lu export_inval=%lu "
            "export_fault=%lu export_nodev=%lu map_ok=%lu remap=%lu "
            "map_inval=%lu map_nodev=%lu map_fault=%lu kick_ok=%lu "
            "kick_nodev=%lu state_ok=%lu state_inval=%lu state_fault=%lu "
            "avail_ok=%lu avail_inval=%lu avail_nodev=%lu avail_fault=%lu "
            "avail_io=%lu reap_ok=%lu reap_nodev=%lu dma_ok=%lu "
            "dma_inval=%lu dma_nodev=%lu dma_fault=%lu desc_ok=%lu "
            "desc_nodev=%lu desc_nomem=%lu user_ok=%lu user_nodev=%lu "
            "user_io=%lu bounce_ok=%lu bounce_inval=%lu bounce_nodev=%lu "
            "bounce_fault=%lu bounce_io=%lu wave=%u\n",
            (unsigned long)s.u64ExportOk, (unsigned long)s.u64ExportInval,
            (unsigned long)s.u64ExportFault, (unsigned long)s.u64ExportNodev,
            (unsigned long)s.u64MapOk, (unsigned long)s.u64MapRemap,
            (unsigned long)s.u64MapInval, (unsigned long)s.u64MapNodev,
            (unsigned long)s.u64MapFault, (unsigned long)s.u64KickOk,
            (unsigned long)s.u64KickNodev, (unsigned long)s.u64RingStateOk,
            (unsigned long)s.u64RingStateInval,
            (unsigned long)s.u64RingStateFault, (unsigned long)s.u64AvailOk,
            (unsigned long)s.u64AvailInval, (unsigned long)s.u64AvailNodev,
            (unsigned long)s.u64AvailFault, (unsigned long)s.u64AvailIo,
            (unsigned long)s.u64ReapOk, (unsigned long)s.u64ReapNodev,
            (unsigned long)s.u64MapDmaOk, (unsigned long)s.u64MapDmaInval,
            (unsigned long)s.u64MapDmaNodev, (unsigned long)s.u64MapDmaFault,
            (unsigned long)s.u64DescOk, (unsigned long)s.u64DescNodev,
            (unsigned long)s.u64DescNomem, (unsigned long)s.u64UserAvailOk,
            (unsigned long)s.u64UserAvailNodev,
            (unsigned long)s.u64UserAvailIo, (unsigned long)s.u64BounceOk,
            (unsigned long)s.u64BounceInval, (unsigned long)s.u64BounceNodev,
            (unsigned long)s.u64BounceFault, (unsigned long)s.u64BounceIo,
            u32Wave);

    /* Grep: net_door: soft virtio */
    kprintf("net_door: soft virtio tx=%lu rx=%lu queue_info=%lu "
            "ready=%u vq_calls=%u nodev_soft=%lu wave=%u\n",
            (unsigned long)s.u64VirtioTx, (unsigned long)s.u64VirtioRx,
            (unsigned long)s.u64QueueInfo, u32Ready, g_u32VqCalls,
            (unsigned long)s.u64Nodev, u32Wave);

    /* Grep: net_door: soft virtio_ok (Wave 15 outcome deepen) */
    kprintf("net_door: soft virtio_ok tx_ok=%lu tx_inval=%lu tx_nodev=%lu "
            "tx_fault=%lu tx_io=%lu rx_ok=%lu rx_inval=%lu rx_nodev=%lu "
            "rx_fault=%lu rx_io=%lu queue_ok=%lu queue_inval=%lu "
            "queue_fault=%lu tx_b=%lu rx_b=%lu live_tx=%u live_rx=%u "
            "wave=%u\n",
            (unsigned long)s.u64VirtioTxOk, (unsigned long)s.u64VirtioTxInval,
            (unsigned long)s.u64VirtioTxNodev,
            (unsigned long)s.u64VirtioTxFault, (unsigned long)s.u64VirtioTxIo,
            (unsigned long)s.u64VirtioRxOk, (unsigned long)s.u64VirtioRxInval,
            (unsigned long)s.u64VirtioRxNodev,
            (unsigned long)s.u64VirtioRxFault, (unsigned long)s.u64VirtioRxIo,
            (unsigned long)s.u64QueueOk, (unsigned long)s.u64QueueInval,
            (unsigned long)s.u64QueueFault,
            (unsigned long)s.u64VirtioTxBytes,
            (unsigned long)s.u64VirtioRxBytes, u32TxCnt, u32RxCnt, u32Wave);

    /* Grep: net_door: soft xfer */
    kprintf("net_door: soft xfer send_ok=%lu send_fail=%lu recv_ok=%lu "
            "recv_fail=%lu send_b=%lu recv_b=%lu xfer_max=%u eth_max=%u "
            "user_copy=%lu k_copy=%lu wave=%u\n",
            (unsigned long)s.u64SendOk, (unsigned long)s.u64SendFail,
            (unsigned long)s.u64RecvOk, (unsigned long)s.u64RecvFail,
            (unsigned long)s.u64SendBytes, (unsigned long)s.u64RecvBytes,
            NET_XFER_MAX, NET_ETH_MAX, (unsigned long)s.u64UserCopy,
            (unsigned long)s.u64KernelCopy, u32Wave);

    /* Grep: net_door: soft last */
    kprintf("net_door: soft last op=%u ret=%ld ring_va=0x%lx wave=%u\n",
            s.u32LastOp, (long)s.i64LastRet,
            (unsigned long)g_u64RingMapVa, u32Wave);

    /* Grep: net_door: soft err */
    kprintf("net_door: soft err nodev=%lu inval=%lu busy=%lu fault=%lu "
            "io=%lu nomem=%lu nosupport=%lu ok=%lu not_init=%lu "
            "stats_inval=%lu stats_fault=%lu tcp_stats_inval=%lu "
            "tcp_stats_fault=%lu wave=%u\n",
            (unsigned long)s.u64Nodev, (unsigned long)s.u64Inval,
            (unsigned long)s.u64Busy, (unsigned long)s.u64Fault,
            (unsigned long)s.u64Io, (unsigned long)s.u64Nomem,
            (unsigned long)s.u64Nosupport, (unsigned long)s.u64Ok,
            (unsigned long)s.u64NotInit, (unsigned long)s.u64StatsInval,
            (unsigned long)s.u64StatsFault,
            (unsigned long)s.u64TcpStatsInval,
            (unsigned long)s.u64TcpStatsFault, u32Wave);

    /* Grep: net_door: soft group (Wave 15 enter-group rollup) */
    kprintf("net_door: soft group enter=%lu claim=%lu poll=%lu stats=%lu "
            "sock=%lu ring=%lu virtio=%lu send=%lu recv=%lu "
            "tcp_stats=%lu wave=%u\n",
            (unsigned long)s.u64Enter, (unsigned long)u64ClaimEnter,
            (unsigned long)s.u64Poll, (unsigned long)s.u64Stats,
            (unsigned long)u64SockEnter, (unsigned long)u64RingEnter,
            (unsigned long)u64VirtioEnter, (unsigned long)s.u64Send,
            (unsigned long)s.u64Recv, (unsigned long)s.u64TcpStats, u32Wave);

    /* Grep: net_door: soft capacity (Wave 15 fixed lamps) */
    kprintf("net_door: soft capacity xfer_max=%u eth_max=%u bulk=3000 "
            "mss=1024 bounce=1 claim_reclaim=1 map_remap=1 "
            "virtio_nodev_soft=1 ring_state_nodev_ok=1 heap=0 "
            "areas=%u wave=%u\n",
            NET_XFER_MAX, NET_ETH_MAX, u32Areas, u32Wave);

    /* Grep: net_door: soft catalog (Wave 15 opcode soft catalog) */
    kprintf("net_door: soft catalog claim=1 release=1 poll=1 stats=1 "
            "socket=1 bind=1 listen=1 accept=1 connect=1 close=1 "
            "send=1 recv=1 tcp_stats=1 export=1 map=1 kick=1 "
            "avail=1 reap=1 ring_state=1 dma=1 desc=1 user_avail=1 "
            "bounce=1 virtio_tx=1 virtio_rx=1 queue_info=1 "
            "full_stack=0 bar3=0 wave=%u\n",
            u32Wave);

    /* Grep: net_door: soft outcome (Wave 15 ok|err rollup) */
    kprintf("net_door: soft outcome ok=%lu nodev=%lu inval=%lu busy=%lu "
            "fault=%lu io=%lu nomem=%lu nosupport=%lu not_init=%lu "
            "claim_ok=%lu reclaim=%lu sock_ok=%lu sock_fail=%lu "
            "send_ok=%lu recv_ok=%lu wave=%u\n",
            (unsigned long)s.u64Ok, (unsigned long)s.u64Nodev,
            (unsigned long)s.u64Inval, (unsigned long)s.u64Busy,
            (unsigned long)s.u64Fault, (unsigned long)s.u64Io,
            (unsigned long)s.u64Nomem, (unsigned long)s.u64Nosupport,
            (unsigned long)s.u64NotInit, (unsigned long)s.u64ClaimOk,
            (unsigned long)s.u64ClaimReclaim, (unsigned long)s.u64SockOk,
            (unsigned long)s.u64SockFail, (unsigned long)s.u64SendOk,
            (unsigned long)s.u64RecvOk, u32Wave);

    /* Grep: net_door: soft stats */
    kprintf("net_door: soft stats enter=%lu claim_ok=%lu reclaim=%lu "
            "sock=%lu ring=%lu virtio=%lu log_n=%lu wave=%u\n",
            (unsigned long)s.u64Enter, (unsigned long)s.u64ClaimOk,
            (unsigned long)s.u64ClaimReclaim, (unsigned long)u64SockEnter,
            (unsigned long)u64RingEnter, (unsigned long)u64VirtioEnter,
            (unsigned long)s.u64SoftLog, u32Wave);

    /* Grep: net_door: soft backend */
    kprintf("net_door: soft backend virtio=%u tx=%u rx=%u q_free_tx=%u "
            "q_free_rx=%u avail_p=%u user_p=%u tcp_accepts=%u "
            "tcp_segs=%u tcp_rtx=%u tcp_rx_b=%u tcp_tx_b=%u "
            "tcp_tw=%u eth_arp=%u eth_udp=%u eth_icmp=%u wave=%u\n",
            u32Ready, u32TxCnt, u32RxCnt, u32QFreeTx, u32QFreeRx, u32AvailP,
            u32UserP, net_tcp_accepts(), net_tcp_segments(),
            net_tcp_retransmits(), net_tcp_bytes_rx(), net_tcp_bytes_tx(),
            net_tcp_tw_reaps(), net_eth_arp_replies(), net_eth_udp_echoes(),
            net_eth_icmp_echoes(), u32Wave);

    /*
     * Honesty line: soft inventory / interim stack ≠ product multi-server.
     * Grep: net_door: soft path
     */
    kprintf("net_door: soft path claim=netstackd sock=tcp|lo "
            "ring=export|map|kick|state|avail|user "
            "reclaim=idempotent map_remap=soft_reclaim "
            "virtio_absent=nodev_soft product_netstackd=1 wave=%u "
            "(soft inventory; not bar3)\n",
            u32Wave);

    /* Grep: net_door: soft headroom — Wave 16 live slack lamps. */
    kprintf("net_door: soft headroom xfer_max=%u eth_max=%u "
            "q_free_tx=%u q_free_rx=%u owned=%u virtio=%u "
            "calls=%u ring_calls=%u wave=%u\n",
            NET_XFER_MAX, NET_ETH_MAX, u32QFreeTx, u32QFreeRx, u32Owned,
            u32Ready, g_u32Calls, g_u32RingCalls, u32Wave);

    /* Grep: net_door: soft surface — Wave 16 surface bit lamps. */
    kprintf("net_door: soft surface init=%u owned=%u virtio=%u "
            "sock=%u ring=%u xfer=%u surf=0x%x wave=%u\n",
            g_fInit ? 1u : 0u, u32Owned, u32Ready,
            u64SockEnter != 0ull ? 1u : 0u,
            u64RingEnter != 0ull ? 1u : 0u,
            (s.u64SendOk + s.u64RecvOk) != 0ull ? 1u : 0u,
            (g_fInit ? 1u : 0u) | (u32Owned << 1) | (u32Ready << 2) |
                ((u64SockEnter != 0ull) ? 8u : 0u) |
                ((u64RingEnter != 0ull) ? 16u : 0u) |
                (((s.u64SendOk + s.u64RecvOk) != 0ull) ? 32u : 0u),
            u32Wave);

    /* Grep: net_door: soft ratio — Wave 16 ok/err basis points. */
    {
        u64 u64Ok = s.u64Ok;
        u64 u64Err = s.u64Nodev + s.u64Inval + s.u64Busy + s.u64Fault +
                     s.u64Io + s.u64Nomem + s.u64Nosupport + s.u64NotInit;
        u64 u64Tot = u64Ok + u64Err;
        u32 u32OkBp = 0;
        u32 u32ErrBp = 0;

        if (u64Tot != 0ull) {
            u32OkBp = (u32)((u64Ok * 10000ull) / u64Tot);
            u32ErrBp = (u32)((u64Err * 10000ull) / u64Tot);
        }
        kprintf("net_door: soft ratio ok_bp=%u err_bp=%u ok=%lu err=%lu "
                "enter=%lu wave=%u\n",
                u32OkBp, u32ErrBp, (unsigned long)u64Ok,
                (unsigned long)u64Err, (unsigned long)s.u64Enter, u32Wave);
    }

    /* Grep: net_door: soft deepen (Wave 16 stamp) */
    kprintf("net_door: soft deepen wave=%u areas=%u init=%u owned=%u "
            "enter=%lu sock=%lu ring=%lu virtio=%lu logs=%lu "
            "ok=1 skip_hard=0\n",
            u32Wave, u32Areas, g_fInit ? 1u : 0u, u32Owned,
            (unsigned long)s.u64Enter, (unsigned long)u64SockEnter,
            (unsigned long)u64RingEnter, (unsigned long)u64VirtioEnter,
            (unsigned long)s.u64SoftLog);

    /*
     * Soft lamp: init surface is always soft-pass. Never hard-gates.
     * Grep: net_door: soft inventory PASS | net_door: soft PASS
     * Grep: net_door: soft FAIL
     */
    fSoftPass = g_fInit ? 1 : 0;
    if (fSoftPass != 0) {
        kprintf("net_door: soft inventory PASS init=%u owned=%u "
                "virtio=%u logs=%lu areas=%u wave=%u\n",
                g_fInit ? 1u : 0u, u32Owned, u32Ready,
                (unsigned long)s.u64SoftLog, u32Areas, u32Wave);
        kprintf("net_door: soft PASS wave=%u areas=%u\n", u32Wave, u32Areas);
    } else {
        kprintf("net_door: soft FAIL init=0 wave=%u "
                "(soft inventory only; not product gate)\n",
                u32Wave);
    }
}

/**
 * After first product call activity, print soft inventory once (mirrors
 * door/futex soft-stats-once). Safe from call return paths only.
 */
static void
net_door_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_soft.u64Enter == 0) {
        return;
    }
    g_fSoftOnce = 1;
    net_door_soft_inventory_log();
}

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
    memset(&g_soft, 0, sizeof(g_soft));
    g_fSoftOnce = 0;
    kprintf("net_door: init (claim+ring soft wave=%u areas=%u)\n",
            (unsigned)NET_DOOR_SOFT_DEEPEN_WAVE,
            (unsigned)NET_DOOR_SOFT_DEEPEN_AREAS);
    /* Grep: net_door: soft (baseline inventory after init; zeros expected) */
    net_door_soft_inventory_log();
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

/**
 * Soft terminal: classify ret + one-shot inventory, then return ret.
 * All product returns from net_door_call go through here (ABI unchanged).
 */
static i64
net_door_soft_done(i64 i64Ret)
{
    g_soft.i64LastRet = i64Ret;
    net_door_soft_note_ret(i64Ret);
    net_door_soft_maybe_once();
    return i64Ret;
}

/**
 * Soft: socket-family terminal (tcp|lo routing + ok|fail + owned).
 * greppable: net_door: soft sock
 */
static i64
net_door_soft_sock_done(i64 i64Ret, int fTcp)
{
    if (fTcp != 0) {
        net_door_soft_inc(&g_soft.u64SockTcp);
    } else {
        net_door_soft_inc(&g_soft.u64SockLo);
    }
    if (i64Ret >= 0) {
        net_door_soft_inc(&g_soft.u64SockOk);
    } else {
        net_door_soft_inc(&g_soft.u64SockFail);
    }
    if (g_u32OwnerToken != 0) {
        net_door_soft_inc(&g_soft.u64SockOwned);
    } else {
        net_door_soft_inc(&g_soft.u64SockUnowned);
    }
    return net_door_soft_done(i64Ret);
}

/**
 * Soft: SEND/RECV terminal (bytes + tcp|lo + sock outcomes).
 * greppable: net_door: soft xfer
 */
static i64
net_door_soft_xfer_done(i64 i64Ret, int fSend, int fTcp)
{
    if (fTcp != 0) {
        net_door_soft_inc(&g_soft.u64SockTcp);
    } else {
        net_door_soft_inc(&g_soft.u64SockLo);
    }
    if (i64Ret >= 0) {
        net_door_soft_inc(&g_soft.u64SockOk);
        if (fSend != 0) {
            net_door_soft_inc(&g_soft.u64SendOk);
            if (i64Ret > 0) {
                net_door_soft_add64(&g_soft.u64SendBytes, (u64)i64Ret);
            }
        } else {
            net_door_soft_inc(&g_soft.u64RecvOk);
            if (i64Ret > 0) {
                net_door_soft_add64(&g_soft.u64RecvBytes, (u64)i64Ret);
            }
        }
    } else {
        net_door_soft_inc(&g_soft.u64SockFail);
        if (fSend != 0) {
            net_door_soft_inc(&g_soft.u64SendFail);
        } else {
            net_door_soft_inc(&g_soft.u64RecvFail);
        }
    }
    if (g_u32OwnerToken != 0) {
        net_door_soft_inc(&g_soft.u64SockOwned);
    } else {
        net_door_soft_inc(&g_soft.u64SockUnowned);
    }
    return net_door_soft_done(i64Ret);
}

i64
net_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    if (!g_fInit) {
        net_door_soft_inc(&g_soft.u64NotInit);
        g_soft.u32LastOp = u32Op;
        return net_door_soft_done(GJ_ERR_NODEV);
    }
    g_u32Calls++;
    net_door_soft_inc(&g_soft.u64Enter);
    g_soft.u32LastOp = u32Op;
    switch (u32Op) {
    case GJ_NET_OP_CLAIM:
        /* arg1 = non-zero ownership token (low 32 bits only). */
        if (u64Arg1 == 0 || (u64Arg1 >> 32) != 0) {
            net_door_soft_inc(&g_soft.u64ClaimInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            net_door_soft_inc(&g_soft.u64ClaimBusy);
            return net_door_soft_done(GJ_ERR_BUSY); /* another netstackd */
        }
        /* Soft reclaim: same token re-CLAIM is idempotent (no re-log). */
        if (g_u32OwnerToken == (u32)u64Arg1) {
            g_u32Reclaims++;
            net_door_soft_inc(&g_soft.u64ClaimReclaim);
            return net_door_soft_done(0);
        }
        g_u32OwnerToken = (u32)u64Arg1;
        g_u32Claims++;
        net_door_soft_inc(&g_soft.u64ClaimOk);
        kprintf("net_door: CLAIM token=0x%x (userspace owns net)\n",
                g_u32OwnerToken);
        return net_door_soft_done(0);
    case GJ_NET_OP_RELEASE:
        /* Soft free path: already unowned → 0 (no token match required). */
        if (g_u32OwnerToken == 0) {
            net_door_soft_inc(&g_soft.u64ReleaseFree);
            return net_door_soft_done(0);
        }
        if ((u64Arg1 >> 32) != 0 || (u32)u64Arg1 != g_u32OwnerToken) {
            net_door_soft_inc(&g_soft.u64ReleaseInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        kprintf("net_door: RELEASE token=0x%x\n", g_u32OwnerToken);
        g_u32OwnerToken = 0;
        net_door_soft_inc(&g_soft.u64ReleaseOk);
        return net_door_soft_done(0);
    case GJ_NET_OP_POLL:
        net_door_soft_inc(&g_soft.u64Poll);
        net_eth_poll();
        return net_door_soft_done(0);
    case GJ_NET_OP_STATS: {
        u32 aSt[4];

        net_door_soft_inc(&g_soft.u64Stats);
        if (u64Arg1 == 0) {
            net_door_soft_inc(&g_soft.u64StatsInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        aSt[0] = net_eth_arp_replies();
        aSt[1] = net_eth_udp_echoes();
        aSt[2] = net_eth_icmp_echoes();
        aSt[3] = g_u32Calls;
        if (user_range_ok(u64Arg1, sizeof(aSt))) {
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_to_user(u64Arg1, aSt, sizeof(aSt)) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64StatsFault);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy((void *)(gj_vaddr_t)u64Arg1, aSt, sizeof(aSt));
        }
        /*
         * Emit soft inventory on STATS so bring-up smoke greps
         * net_door: soft … without a dedicated opcode.
         * greppable: net_door: soft
         */
        net_door_soft_inventory_log();
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_SOCKET:
        /* AF_INET=2: SOCK_STREAM → TCP (virtio eth + loopback pair);
         * SOCK_DGRAM → net_lo UDP-shaped path. */
        net_door_soft_inc(&g_soft.u64Socket);
        if ((int)u64Arg2 == SOCK_STREAM) {
            return net_door_soft_sock_done(net_tcp_socket(), 1);
        }
        return net_door_soft_sock_done(
            net_lo_socket((int)u64Arg1, (int)u64Arg2, (int)u64Arg3), 0);
    case GJ_NET_OP_BIND:
        net_door_soft_inc(&g_soft.u64Bind);
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_door_soft_sock_done(
                net_tcp_bind((i64)u64Arg1, (u16)u64Arg2), 1);
        }
        return net_door_soft_sock_done(
            net_lo_bind((i64)u64Arg1, (u16)u64Arg2), 0);
    case GJ_NET_OP_LISTEN:
        net_door_soft_inc(&g_soft.u64Listen);
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_door_soft_sock_done(
                net_tcp_listen((i64)u64Arg1, (int)u64Arg2), 1);
        }
        return net_door_soft_sock_done(
            net_lo_listen((i64)u64Arg1, (int)u64Arg2), 0);
    case GJ_NET_OP_ACCEPT:
        net_door_soft_inc(&g_soft.u64Accept);
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_door_soft_sock_done(net_tcp_accept((i64)u64Arg1), 1);
        }
        return net_door_soft_sock_done(net_lo_accept((i64)u64Arg1), 0);
    case GJ_NET_OP_CONNECT:
        net_door_soft_inc(&g_soft.u64Connect);
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_door_soft_sock_done(
                net_tcp_connect((i64)u64Arg1, (u16)u64Arg2), 1);
        }
        return net_door_soft_sock_done(
            net_lo_connect((i64)u64Arg1, (u16)u64Arg2), 0);
    case GJ_NET_OP_CLOSE:
        net_door_soft_inc(&g_soft.u64Close);
        if (net_tcp_fd_ok((i64)u64Arg1)) {
            return net_door_soft_sock_done(net_tcp_close((i64)u64Arg1), 1);
        }
        return net_door_soft_sock_done(net_lo_close((i64)u64Arg1), 0);
    case GJ_NET_OP_SEND: {
        /* Bounce ≤ NET_XFER_MAX; TCP multi-seg chunks by MSS inside net_tcp. */
        u8 aBuf[NET_XFER_MAX];
        size_t cb = (size_t)u64Arg3;
        i64 n;
        int fTcp;

        net_door_soft_inc(&g_soft.u64Send);
        if (u64Arg2 == 0 || cb == 0) {
            net_door_soft_inc(&g_soft.u64SendFail);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (cb > NET_XFER_MAX) {
            cb = NET_XFER_MAX;
        }
        if (user_range_ok(u64Arg2, cb)) {
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_from_user(aBuf, u64Arg2, cb) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64SendFail);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy(aBuf, (const void *)(gj_vaddr_t)u64Arg2, cb);
        }
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        if (fTcp != 0) {
            n = net_tcp_send((i64)u64Arg1, aBuf, cb);
        } else {
            n = net_lo_send((i64)u64Arg1, aBuf, cb);
        }
        return net_door_soft_xfer_done(n, 1, fTcp);
    }
    case GJ_NET_OP_RECV: {
        /* Multi-seg drain may take several RECV calls; each ≤ NET_XFER_MAX. */
        u8 aBuf[NET_XFER_MAX];
        size_t cb = (size_t)u64Arg3;
        i64 n;
        int fTcp;

        net_door_soft_inc(&g_soft.u64Recv);
        if (u64Arg2 == 0 || cb == 0) {
            net_door_soft_inc(&g_soft.u64RecvFail);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (cb > NET_XFER_MAX) {
            cb = NET_XFER_MAX;
        }
        fTcp = net_tcp_fd_ok((i64)u64Arg1) ? 1 : 0;
        if (fTcp != 0) {
            n = net_tcp_recv((i64)u64Arg1, aBuf, cb);
        } else {
            n = net_lo_recv((i64)u64Arg1, aBuf, cb);
        }
        if (n > 0) {
            if ((size_t)n > NET_XFER_MAX) {
                net_door_soft_inc(&g_soft.u64RecvFail);
                return net_door_soft_done(GJ_ERR_IO); /* defensive */
            }
            if (user_range_ok(u64Arg2, (size_t)n)) {
                net_door_soft_inc(&g_soft.u64UserCopy);
                if (copy_to_user(u64Arg2, aBuf, (size_t)n) != GJ_OK) {
                    net_door_soft_inc(&g_soft.u64RecvFail);
                    return net_door_soft_done(GJ_ERR_FAULT);
                }
            } else {
                net_door_soft_inc(&g_soft.u64KernelCopy);
                memcpy((void *)(gj_vaddr_t)u64Arg2, aBuf, (size_t)n);
            }
        }
        return net_door_soft_xfer_done(n, 0, fTcp);
    }
    case GJ_NET_OP_TCP_STATS: {
        u32 aSt[4];
        u32 segs;
        u32 rtx;

        net_door_soft_inc(&g_soft.u64TcpStats);
        if (u64Arg1 == 0) {
            net_door_soft_inc(&g_soft.u64TcpStatsInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        /* Prefer full tx_bytes; pack rtx into segs high half (segs rarely wrap). */
        segs = net_tcp_segments();
        rtx = net_tcp_retransmits();
        aSt[0] = net_tcp_accepts();
        aSt[1] = (rtx << 16) | (segs & 0xffffu);
        aSt[2] = net_tcp_bytes_rx();
        aSt[3] = net_tcp_bytes_tx();
        if (user_range_ok(u64Arg1, sizeof(aSt))) {
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_to_user(u64Arg1, aSt, sizeof(aSt)) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64TcpStatsFault);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy((void *)(gj_vaddr_t)u64Arg1, aSt, sizeof(aSt));
        }
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_VIRTIO_TX: {
        /* L2 frame path: bound to NET_ETH_MAX (driver also rejects >1514). */
        u8 aFrame[NET_ETH_MAX];
        u32 cb = (u32)u64Arg2;

        net_door_soft_inc(&g_soft.u64VirtioTx);
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64VirtioTxNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (u64Arg1 == 0 || cb == 0 || cb > NET_ETH_MAX) {
            net_door_soft_inc(&g_soft.u64VirtioTxInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (user_range_ok(u64Arg1, cb)) {
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_from_user(aFrame, u64Arg1, cb) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64VirtioTxFault);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy(aFrame, (const void *)(gj_vaddr_t)u64Arg1, cb);
        }
        g_u32VqCalls++;
        if (virtio_net_tx(aFrame, cb) != 0) {
            net_door_soft_inc(&g_soft.u64VirtioTxIo);
            return net_door_soft_done(GJ_ERR_IO);
        }
        net_door_soft_inc(&g_soft.u64VirtioTxOk);
        net_door_soft_add64(&g_soft.u64VirtioTxBytes, (u64)cb);
        return net_door_soft_done((i64)cb);
    }
    case GJ_NET_OP_VIRTIO_RX: {
        u8 aFrame[NET_ETH_MAX];
        u32 cbMax = (u32)u64Arg2;
        i32 n;

        net_door_soft_inc(&g_soft.u64VirtioRx);
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64VirtioRxNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (u64Arg1 == 0 || cbMax == 0) {
            net_door_soft_inc(&g_soft.u64VirtioRxInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (cbMax > NET_ETH_MAX) {
            cbMax = NET_ETH_MAX;
        }
        g_u32VqCalls++;
        n = virtio_net_rx(aFrame, cbMax);
        if (n < 0) {
            net_door_soft_inc(&g_soft.u64VirtioRxIo);
            return net_door_soft_done(GJ_ERR_IO);
        }
        if (n == 0) {
            net_door_soft_inc(&g_soft.u64VirtioRxOk);
            return net_door_soft_done(0);
        }
        if ((u32)n > NET_ETH_MAX) {
            net_door_soft_inc(&g_soft.u64VirtioRxIo);
            return net_door_soft_done(GJ_ERR_IO);
        }
        if (user_range_ok(u64Arg1, (u32)n)) {
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_to_user(u64Arg1, aFrame, (size_t)n) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64VirtioRxFault);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy((void *)(gj_vaddr_t)u64Arg1, aFrame, (size_t)n);
        }
        net_door_soft_inc(&g_soft.u64VirtioRxOk);
        net_door_soft_add64(&g_soft.u64VirtioRxBytes, (u64)(u32)n);
        return net_door_soft_done((i64)n);
    }
    case GJ_NET_OP_QUEUE_INFO: {
        u32 aQ[5];

        net_door_soft_inc(&g_soft.u64QueueInfo);
        if (u64Arg1 == 0) {
            net_door_soft_inc(&g_soft.u64QueueInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        aQ[0] = virtio_net_tx_count();
        aQ[1] = virtio_net_rx_count();
        aQ[2] = virtio_net_ready() ? 1u : 0u;
        aQ[3] = g_u32OwnerToken ? 1u : 0u;
        aQ[4] = g_u32VqCalls;
        if (user_range_ok(u64Arg1, sizeof(aQ))) {
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_to_user(u64Arg1, aQ, sizeof(aQ)) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64QueueFault);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy((void *)(gj_vaddr_t)u64Arg1, aQ, sizeof(aQ));
        }
        net_door_soft_inc(&g_soft.u64QueueOk);
        /* greppable: net_door: soft (queue path also dumps inventory) */
        net_door_soft_inventory_log();
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_EXPORT_RING: {
        struct gj_virtq_export ex;

        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64ExportRing);
        if (u64Arg2 == 0) {
            net_door_soft_inc(&g_soft.u64ExportInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        /* Soft-skip surface: no virtio-net → NODEV (netstackd soft-logs). */
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64ExportNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (virtio_net_export_q((u16)u64Arg1, &ex) != 0) {
            net_door_soft_inc(&g_soft.u64ExportNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        g_u32VqCalls++;
        if (user_range_ok(u64Arg2, sizeof(ex))) {
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_to_user(u64Arg2, &ex, sizeof(ex)) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64ExportFault);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy((void *)(gj_vaddr_t)u64Arg2, &ex, sizeof(ex));
        }
        net_door_soft_inc(&g_soft.u64ExportOk);
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_MAP_RING: {
        struct gj_virtq_export ex;
        int fRemap;

        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64MapRing);
        if (u64Arg2 == 0) {
            net_door_soft_inc(&g_soft.u64MapInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        /* VA base must be page-aligned for ring map into user AS. */
        if ((u64Arg2 & (GJ_PAGE_SIZE - 1ull)) != 0) {
            net_door_soft_inc(&g_soft.u64MapInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        /* Soft-skip surface: no virtio-net → NODEV (distinct from map FAULT). */
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64MapNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        /*
         * Soft re-MAP of the same VA: re-install PTEs + re-export (idempotent
         * hand-off for netstackd / UDX reclaim of the map window).
         */
        fRemap = (g_u64RingMapVa != 0 && g_u64RingMapVa == u64Arg2) ? 1 : 0;
        if (virtio_net_map_q_user((u16)u64Arg1, u64Arg2, &ex) != 0) {
            net_door_soft_inc(&g_soft.u64MapFault);
            return net_door_soft_done(GJ_ERR_FAULT);
        }
        g_u64RingMapVa = u64Arg2;
        g_u32VqCalls++;
        if (fRemap) {
            net_door_soft_inc(&g_soft.u64MapRemap);
        } else {
            net_door_soft_inc(&g_soft.u64MapOk);
        }
        if (u64Arg3 != 0) {
            if (user_range_ok(u64Arg3, sizeof(ex))) {
                net_door_soft_inc(&g_soft.u64UserCopy);
                if (copy_to_user(u64Arg3, &ex, sizeof(ex)) != GJ_OK) {
                    net_door_soft_inc(&g_soft.u64MapFault);
                    return net_door_soft_done(GJ_ERR_FAULT);
                }
            } else {
                net_door_soft_inc(&g_soft.u64KernelCopy);
                memcpy((void *)(gj_vaddr_t)u64Arg3, &ex, sizeof(ex));
            }
        }
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_KICK:
        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64Kick);
        /* Soft-skip when virtio-net absent; kick is best-effort notify. */
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64KickNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (virtio_net_kick_q((u16)u64Arg1) != 0) {
            net_door_soft_inc(&g_soft.u64KickNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64KickOk);
        return net_door_soft_done(0);
    case GJ_NET_OP_AVAIL_PUSH: {
        u8 aBuf[NET_ETH_MAX];
        u32 cb = (u32)u64Arg3;
        u16 which = (u16)u64Arg1;
        int fWrite = (which == 0) ? 1 : 0;

        net_door_soft_inc(&g_soft.u64AvailPush);
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64AvailNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (u64Arg2 == 0 || cb == 0 || cb > NET_ETH_MAX) {
            net_door_soft_inc(&g_soft.u64AvailInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (user_range_ok(u64Arg2, cb)) {
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_from_user(aBuf, u64Arg2, cb) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64AvailFault);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy(aBuf, (const void *)(gj_vaddr_t)u64Arg2, cb);
        }
        g_u32VqCalls++;
        /* Ring programming path: AVAIL push + kick (no full TX helper) */
        if (virtio_net_avail_push(which, aBuf, cb, fWrite, 1) != 0) {
            net_door_soft_inc(&g_soft.u64AvailIo);
            return net_door_soft_done(GJ_ERR_IO);
        }
        net_door_soft_inc(&g_soft.u64AvailOk);
        return net_door_soft_done((i64)cb);
    }
    case GJ_NET_OP_USED_REAP: {
        u32 n;

        net_door_soft_inc(&g_soft.u64UsedReap);
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64ReapNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        n = virtio_net_used_reap((u16)u64Arg1, (u32)u64Arg2);
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64ReapOk);
        return net_door_soft_done((i64)n);
    }
    case GJ_NET_OP_RING_STATE: {
        /* Soft: always fills state; free/pushes = 0 without virtio-net. */
        u32 aS[4];

        g_u32RingCalls++;
        net_door_soft_inc(&g_soft.u64RingState);
        if (u64Arg1 == 0) {
            net_door_soft_inc(&g_soft.u64RingStateInval);
            return net_door_soft_done(GJ_ERR_INVAL);
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
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_to_user(u64Arg1, aS, sizeof(aS)) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64RingStateFault);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy((void *)(gj_vaddr_t)u64Arg1, aS, sizeof(aS));
        }
        net_door_soft_inc(&g_soft.u64RingStateOk);
        /* greppable: net_door: soft (ring state also dumps inventory) */
        net_door_soft_inventory_log();
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_MAP_DMA: {
        struct gj_virtq_dma_export ex;

        net_door_soft_inc(&g_soft.u64MapDma);
        if (u64Arg1 == 0) {
            net_door_soft_inc(&g_soft.u64MapDmaInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if ((u64Arg1 & (GJ_PAGE_SIZE - 1ull)) != 0) {
            net_door_soft_inc(&g_soft.u64MapDmaInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64MapDmaNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (virtio_net_map_dma_user(u64Arg1, &ex) != 0) {
            net_door_soft_inc(&g_soft.u64MapDmaFault);
            return net_door_soft_done(GJ_ERR_FAULT);
        }
        g_u32VqCalls++;
        if (u64Arg2 != 0) {
            if (user_range_ok(u64Arg2, sizeof(ex))) {
                net_door_soft_inc(&g_soft.u64UserCopy);
                if (copy_to_user(u64Arg2, &ex, sizeof(ex)) != GJ_OK) {
                    net_door_soft_inc(&g_soft.u64MapDmaFault);
                    return net_door_soft_done(GJ_ERR_FAULT);
                }
            } else {
                net_door_soft_inc(&g_soft.u64KernelCopy);
                memcpy((void *)(gj_vaddr_t)u64Arg2, &ex, sizeof(ex));
            }
        }
        net_door_soft_inc(&g_soft.u64MapDmaOk);
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_DESC_ALLOC: {
        int head;

        net_door_soft_inc(&g_soft.u64DescAlloc);
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64DescNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        head = virtio_net_desc_alloc((u16)u64Arg1);
        if (head < 0) {
            net_door_soft_inc(&g_soft.u64DescNomem);
            return net_door_soft_done(GJ_ERR_NOMEM);
        }
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64DescOk);
        return net_door_soft_done((i64)head);
    }
    case GJ_NET_OP_USER_AVAIL: {
        net_door_soft_inc(&g_soft.u64UserAvail);
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64UserAvailNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (virtio_net_user_avail((u16)u64Arg1, (u16)u64Arg2, (int)u64Arg3) !=
            0) {
            net_door_soft_inc(&g_soft.u64UserAvailIo);
            return net_door_soft_done(GJ_ERR_IO);
        }
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64UserAvailOk);
        return net_door_soft_done(0);
    }
    case GJ_NET_OP_BOUNCE_FILL: {
        u8 aBuf[NET_ETH_MAX];
        u32 cb = (u32)u64Arg3;
        int n;

        net_door_soft_inc(&g_soft.u64BounceFill);
        if (!virtio_net_ready()) {
            net_door_soft_inc(&g_soft.u64BounceNodev);
            return net_door_soft_done(GJ_ERR_NODEV);
        }
        if (u64Arg2 == 0 || cb == 0 || cb > NET_ETH_MAX) {
            net_door_soft_inc(&g_soft.u64BounceInval);
            return net_door_soft_done(GJ_ERR_INVAL);
        }
        if (user_range_ok(u64Arg2, cb)) {
            net_door_soft_inc(&g_soft.u64UserCopy);
            if (copy_from_user(aBuf, u64Arg2, cb) != GJ_OK) {
                net_door_soft_inc(&g_soft.u64BounceFault);
                return net_door_soft_done(GJ_ERR_FAULT);
            }
        } else {
            net_door_soft_inc(&g_soft.u64KernelCopy);
            memcpy(aBuf, (const void *)(gj_vaddr_t)u64Arg2, cb);
        }
        /* slot in arg1; TX header prepend for net */
        n = virtio_net_bounce_fill((u32)u64Arg1, aBuf, cb, 1);
        if (n < 0) {
            net_door_soft_inc(&g_soft.u64BounceIo);
            return net_door_soft_done(GJ_ERR_IO);
        }
        g_u32VqCalls++;
        net_door_soft_inc(&g_soft.u64BounceOk);
        return net_door_soft_done((i64)n);
    }
    default:
        return net_door_soft_done(GJ_ERR_NOSUPPORT);
    }
}
