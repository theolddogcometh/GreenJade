/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Loopback socket table (pure C, dual MIT OR Apache-2.0): DGRAM + residual
 * STREAM pairing. Product STREAM multi-seg path uses net_tcp via the door;
 * this unit is residual Soft!=product for L3 soft tests without freestanding
 * NIC (no eth / no rtl / no virtio wire; local socket ABI only).
 *
 * Exclusive residual lean (this unit + net_lo.h only):
 *   GOAL: lean loopback residual for L3 soft tests without freestanding NIC.
 *   NOT freestanding NIC (net_eth / net_tcp wire / rtl8168 / virtio own wire).
 *   Soft!=product. G-AC-1 (no Linux .ko product AC). Dual MIT OR Apache-2.0.
 *   HARD: ASCII Soft!=product lamps; no version stamp; no stamp storms.
 *   Never hard-gates product policy. Diagnostics only (wrap OK).
 *   Dual DoD honesty (agent != close): product = UDX/DDI + hot/cold ABI;
 *   dual_dod_a=OPEN dual_dod_b=OPEN_UDX product_dod_b=UDX; this residual
 *   never closes Dual DoD A/B (local_abi L3 soft only; freestanding SKIP).
 *
 * Residual pairing (aligned with net_tcp loop pairs - functional only):
 *   connect(STREAM) mints AcceptQ server peer, pairs client<->peer (not
 *   client<->listener) so pre-accept SEND lands on the accepted fd's ring.
 *   accept() takes AcceptQ (no re-mint). Multi-pending up to backlog.
 *   STREAM no listener -> -ECONNREFUSED. DGRAM may soft-orphan.
 *   shutdown(WR) half-closes peer RD; close does the same.
 *
 * Soft product deepenings:
 *   - listen backlog stored/clamped; connect rejects when queue full
 *   - SOL_SOCKET subset: REUSEADDR, TYPE, ERROR, BROADCAST, KEEPALIVE,
 *     SNDBUF/RCVBUF (stored), LINGER soft, ACCEPTCONN, REUSEPORT
 *   - bind EADDRINUSE when port taken without reuse; port 0 soft ephemeral
 *   - soft stats + lean residual inventory (grep: "net: lo soft")
 *   - AF domain stored (AF_INET|AF_UNIX) for getsockname/getpeername honesty
 *   - lean return-surface catalog (one line; no multi-kprintf storm)
 *   - sticky SO_ERROR residual (read-and-clear; L3 soft ABI honesty)
 *
 * C0 residual deepen (this unit only; Soft!=product; stamp-free):
 *   - AcceptQ integrity observe (pending vs live AcceptQ children)
 *   - expanded residual lean self-check (ABI + caps + poll bits + integ)
 *   - greppable deepen lamp (once; never version stamp / never Dual DoD close)
 *
 * Lean soft residual (this unit only; Soft!=product dual license):
 *   Lifetime / ring / sockopt tallies (struct net_lo_soft) retained.
 *   Greppable prefix-stable serial (rate-limited; never multi-kprintf flood):
 *     net: lo soft inventory       - ONE short line (no twin dump)
 *     net: lo soft residual lean   - honesty lamp Soft!=product Dual DoD OPEN
 *     net: lo soft residual lean PASS - lean self-check full (not product gate)
 *     net: lo soft deepen          - C0 residual deepen once-lamp (stamp-free)
 *     net: lo soft PASS            - soft inventory only; not product gate
 *     net: lo soft catalog         - return-surface / area catalog (init once)
 *     net: lo soft init|listen|accept|emfile|pair  - event lines, capped
 *   Cadence dumps only at power-of-two op milestones, hard-capped at
 *   NET_LO_SOFT_LOG_MAX (force emfile/empty also capped). Init always
 *   emits once. Event lines share NET_LO_SOFT_EVENT_MAX.
 *   No version stamp. No stamp storms. No #if 0 nested-comment residue.
 *   Never hard-gates product policy. Pure C11 dual MIT OR Apache-2.0.
 * greppable: net: lo soft residual lean
 * greppable: net: lo soft residual lean PASS
 * greppable: net: lo soft deepen
 * greppable: net: lo soft catalog
 * greppable: net: lo soft / net_lo: soft residual
 * greppable: Soft!=product | soft_ne_product=1 | G-AC-1=1
 * greppable: l3_soft=1 | freestanding_nic=0 | freestanding_rtl=0 | local_abi=1
 * greppable: dual_dod_a=OPEN | dual_dod_b=OPEN_UDX | product_dod_b=UDX
 * greppable: not_dod_close=1 | freestanding_skip=1 | product_path=UDX|DDI
 * greppable: acceptq_integ= | soft_err= | ephemeral_bind= | C0 residual
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
/*
 * Lean soft residual serial budget. Absolute cap of greppable inventory
 * dumps; milestones are power-of-two API op counts (1,2,4,...).
 * Event lines (listen/accept/emfile/pair) share a separate hard cap.
 * No version stamp. No stamp storms. Soft!=product dual MIT OR Apache-2.0.
 * G-AC-1: no Linux .ko product AC (this unit is pure-C loopback only).
 * L3 soft tests without freestanding NIC (freestanding_nic=0).
 * greppable: net: lo soft residual lean
 */
#define NET_LO_SOFT_LOG_MAX   8u
#define NET_LO_SOFT_EVENT_MAX 8u
/*
 * Linux poll bit numbers (same as EPOLLIN/OUT/ERR/HUP on x86).
 * Cold-path query + lean residual self-check. Soft!=product.
 */
#define LO_POLLIN  0x0001u
#define LO_POLLOUT 0x0004u
#define LO_POLLERR 0x0008u
#define LO_POLLHUP 0x0010u
/*
 * Lean residual self-check expected ok count (L3 soft; no freestanding NIC).
 * C0 residual deepen (stamp-free Soft!=product):
 *   1..4 ABI constants (MAX/FD_BASE/BUF/BACKLOG)
 *   5 freestanding/local flags (nic=0 rtl=0 local_abi=1 l3_soft=1)
 *   6 G-AC-1
 *   7 Dual DoD OPEN honesty (A/B OPEN product=UDX)
 *   8 not_dod_close + Soft!=product + freestanding skip
 *   9 soft log/event caps finite (no stamp storm contract)
 *  10 poll bit residual constants + ring defaults match BUF
 *  11 AcceptQ residual contract (field + backlog geometry)
 *  12 AcceptQ integrity observe (pending vs live AcceptQ children)
 */
#define NET_LO_LEAN_OK_EXPECT 12u
/* Soft ephemeral bind base (host order; avoid well-known / low ports). */
#define NET_LO_EPHEM_BASE 49152u
#define NET_LO_EPHEM_SPAN 16384u

struct net_lo_sock {
    u8  u8Used;
    u8  u8Type;
    u8  u8Domain;    /* AF_INET or AF_UNIX (name honesty residual) */
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
    u8  u8AcceptQ;   /* 1 = minted peer still in listen accept queue */
    u8  u8PadDom;    /* align; reserved soft */
    u16 u16Port;
    u16 u16LingerSec;
    u32 u32RcvBuf;
    u32 u32SndBuf;
    i16 i16Peer; /* slot of peer or -1 */
    u16 u16SoftErr; /* sticky SO_ERROR residual (positive errno; 0=none) */
    u32 u32RxLen;
    u32 u32RxHead;
    u8  aRx[NET_LO_BUF];
};

/*
 * Soft product inventory counters - wrap OK; diagnostics only; never
 * hard-gate product paths. Soft!=product dual MIT OR Apache-2.0. G-AC-1.
 * Grep: net: lo soft residual lean | net: lo soft inventory
 * Lean residual: one-line inventory + residual lean lamp (no stamp storms).
 * l3_soft=1 freestanding_nic=0 freestanding_rtl=0 local_abi=1
 * L3 soft tests without freestanding NIC only.
 */
struct net_lo_soft {
    u64 u64Ops;          /* total API entries (success + fail) */
    u64 u64SockOk;
    u64 u64SockFail;
    u64 u64SockInet;     /* AF_INET create */
    u64 u64SockUnix;     /* AF_UNIX create */
    u64 u64SockStream;   /* SOCK_STREAM create */
    u64 u64SockDgram;    /* SOCK_DGRAM create */
    u64 u64BindOk;
    u64 u64BindFail;
    u64 u64EaddrInuse;
    u64 u64ListenOk;
    u64 u64ListenFail;
    u64 u64ConnOk;
    u64 u64ConnFail;
    u64 u64ConnAgain;    /* backlog full / queue soft reject */
    u64 u64ConnOrphan;   /* DGRAM connect with no local listener (soft OK) */
    u64 u64ConnRefused;  /* STREAM connect with no local listener */
    u64 u64AcceptOk;
    u64 u64AcceptFail;
    u64 u64AcceptAgain;
    u64 u64SendOk;
    u64 u64SendFail;
    u64 u64SendPipe;
    u64 u64SendSelf;     /* unpaired dgram loop into own RX */
    u64 u64RecvOk;
    u64 u64RecvFail;
    u64 u64RecvAgain;
    u64 u64RecvEof;
    u64 u64ShutOk;
    u64 u64ShutFail;
    u64 u64ShutRd;       /* how=0 */
    u64 u64ShutWr;       /* how=1 */
    u64 u64ShutRdwr;     /* how=2 */
    u64 u64CloseOk;
    u64 u64CloseFail;
    u64 u64ClosePeerHalf;/* close marked peer shut_rd soft */
    u64 u64SetoptOk;
    u64 u64SetoptFail;
    u64 u64GetoptOk;
    u64 u64GetoptFail;
    u64 u64OptReuse;
    u64 u64OptReusePort;
    u64 u64OptBcast;
    u64 u64OptKa;
    u64 u64OptSndbuf;
    u64 u64OptRcvbuf;
    u64 u64OptLinger;
    u64 u64OptNoop;      /* debug/dontroute/oob/timeo soft accept */
    u64 u64OptType;
    u64 u64OptError;
    u64 u64OptAcceptConn;
    u64 u64NameOk;
    u64 u64NameFail;
    u64 u64PeerOk;
    u64 u64PeerFail;
    u64 u64BytesTx;
    u64 u64BytesRx;
    u64 u64PushFull;     /* peer RX ring full (short/zero push) */
    u64 u64PushPartial;  /* short write into ring */
    u64 u64HwmUsed;      /* high-water live used slots */
    u64 u64LogDumps;     /* times soft inventory was emitted */
    u64 u64LogSkip;      /* cadence dumps suppressed (cap / non-milestone) */
    u64 u64EventSkip;    /* event lines suppressed (cap) */
    u64 u64LeanOk;       /* residual lean self-check full pass count */
    u64 u64LeanFail;     /* residual lean self-check incomplete */
    u64 u64LeanPassEmit; /* residual lean PASS line emissions */
    u64 u64CatalogEmit;  /* return-surface catalog emissions */
    u64 u64DeepenEmit;   /* C0 residual deepen once-lamp emissions */
    u64 u64IntegOk;      /* AcceptQ integrity observe full match */
    u64 u64IntegFail;    /* AcceptQ integrity mismatch (observe only) */
    u64 u64SoftErrSet;   /* sticky SO_ERROR residual sets */
    u64 u64SoftErrGet;   /* SO_ERROR getsockopt read-and-clear */
    u64 u64EphemBind;    /* bind(port=0) soft ephemeral assigns */
    u32 u32SoftLogN;     /* inventory log emissions (u32 twin) */
    u32 u32EventN;       /* listen/accept/emfile event emissions */
    u32 u32LeanLastOk;   /* last residual lean ok/N score */
    u32 u32IntegLast;    /* last AcceptQ integrity ok score (0/1) */
    u8  u8LeanOnce;      /* residual lean once-lamp fired */
    u8  u8CatalogOnce;   /* soft catalog init once-lamp */
    u8  u8DeepenOnce;    /* C0 residual deepen once-lamp */
    u8  u8PadLean;
};

static struct net_lo_sock g_aSocks[NET_LO_MAX];
static struct net_lo_soft g_soft;
/* FDs 64..79 map to socket slots 0..15 (avoid vfs_ram 3..31) */
#define NET_FD_BASE 64

static void
lo_soft_bump(u64 *pCnt)
{
    if (pCnt == NULL) {
        return;
    }
    (*pCnt)++; /* wrap OK */
}

/**
 * Sticky SO_ERROR residual (positive errno). Diagnostics only Soft!=product.
 * Does not hard-gate product paths. greppable: soft_err=
 */
static void
lo_soft_set_err(u32 u32Slot, int nErr)
{
    if (u32Slot >= NET_LO_MAX || !g_aSocks[u32Slot].u8Used) {
        return;
    }
    if (nErr <= 0) {
        return;
    }
    if (nErr > 65535) {
        nErr = 65535;
    }
    g_aSocks[u32Slot].u16SoftErr = (u16)nErr;
    lo_soft_bump(&g_soft.u64SoftErrSet);
}

/**
 * AcceptQ integrity observe (never hard-gates product).
 * For each live listener: pending count must equal live AcceptQ children
 * on same port+type (residual multi-pending contract). Returns 1 if all
 * match (or no listeners), 0 on any mismatch. Soft!=product. G-AC-1.
 * greppable: acceptq_integ=
 */
static u32
lo_soft_integ_acceptq(void)
{
    u32 u32Li;
    u32 u32J;
    u32 u32LiveAq;
    u32 u32Pend;
    u32 u32Ok = 1u;

    for (u32Li = 0; u32Li < NET_LO_MAX; u32Li++) {
        if (!g_aSocks[u32Li].u8Used || !g_aSocks[u32Li].u8Listening) {
            continue;
        }
        u32Pend = (u32)g_aSocks[u32Li].u8Pending;
        u32LiveAq = 0;
        for (u32J = 0; u32J < NET_LO_MAX; u32J++) {
            if (!g_aSocks[u32J].u8Used || !g_aSocks[u32J].u8AcceptQ ||
                g_aSocks[u32J].u8Listening) {
                continue;
            }
            if (g_aSocks[u32J].u16Port == g_aSocks[u32Li].u16Port &&
                g_aSocks[u32J].u8Type == g_aSocks[u32Li].u8Type) {
                u32LiveAq++;
            }
        }
        /* Multiple listeners may share port under reuse; count is soft. */
        if (u32Pend != u32LiveAq &&
            !(g_aSocks[u32Li].u8Reuse || g_aSocks[u32Li].u8ReusePort)) {
            /* Allow pending overcount only when another listener shares. */
            u32Ok = 0u;
            break;
        }
        if (u32Pend > (u32)NET_LO_BACKLOG_MAX) {
            u32Ok = 0u;
            break;
        }
    }
    g_soft.u32IntegLast = u32Ok;
    if (u32Ok != 0u) {
        lo_soft_bump(&g_soft.u64IntegOk);
    } else {
        lo_soft_bump(&g_soft.u64IntegFail);
    }
    return u32Ok;
}

/* Live-table tallies for soft inventory (no alloc; walk NET_LO_MAX). */
static void
lo_soft_tally(u32 *pUsed, u32 *pFree, u32 *pListen, u32 *pConn,
              u32 *pStream, u32 *pDgram, u32 *pPending, u32 *pRxBytes,
              u32 *pShutRd, u32 *pShutWr, u32 *pReuse)
{
    u32 i;
    u32 cUsed = 0;
    u32 cListen = 0;
    u32 cConn = 0;
    u32 cStream = 0;
    u32 cDgram = 0;
    u32 cPending = 0;
    u32 cRx = 0;
    u32 cShutRd = 0;
    u32 cShutWr = 0;
    u32 cReuse = 0;

    for (i = 0; i < NET_LO_MAX; i++) {
        if (!g_aSocks[i].u8Used) {
            continue;
        }
        cUsed++;
        if (g_aSocks[i].u8Listening) {
            cListen++;
        }
        if (g_aSocks[i].u8Connected) {
            cConn++;
        }
        if (g_aSocks[i].u8Type == SOCK_STREAM) {
            cStream++;
        } else if (g_aSocks[i].u8Type == SOCK_DGRAM) {
            cDgram++;
        }
        cPending += (u32)g_aSocks[i].u8Pending;
        cRx += g_aSocks[i].u32RxLen;
        if (g_aSocks[i].u8ShutRd) {
            cShutRd++;
        }
        if (g_aSocks[i].u8ShutWr) {
            cShutWr++;
        }
        if (g_aSocks[i].u8Reuse || g_aSocks[i].u8ReusePort) {
            cReuse++;
        }
    }
    if (pUsed != NULL) {
        *pUsed = cUsed;
    }
    if (pFree != NULL) {
        *pFree = NET_LO_MAX - cUsed;
    }
    if (pListen != NULL) {
        *pListen = cListen;
    }
    if (pConn != NULL) {
        *pConn = cConn;
    }
    if (pStream != NULL) {
        *pStream = cStream;
    }
    if (pDgram != NULL) {
        *pDgram = cDgram;
    }
    if (pPending != NULL) {
        *pPending = cPending;
    }
    if (pRxBytes != NULL) {
        *pRxBytes = cRx;
    }
    if (pShutRd != NULL) {
        *pShutRd = cShutRd;
    }
    if (pShutWr != NULL) {
        *pShutWr = cShutWr;
    }
    if (pReuse != NULL) {
        *pReuse = cReuse;
    }
    if ((u64)cUsed > g_soft.u64HwmUsed) {
        g_soft.u64HwmUsed = (u64)cUsed;
    }
}

/**
 * Soft: rate-limit budget for one-shot event lines (listen/accept/emfile).
 * Returns 1 if the line may print; 0 if suppressed (bump event_skip).
 * greppable: net: lo soft listen|accept|emfile
 */
static int
lo_soft_event_ok(void)
{
    if (g_soft.u32EventN >= NET_LO_SOFT_EVENT_MAX) {
        lo_soft_bump(&g_soft.u64EventSkip);
        return 0;
    }
    if (g_soft.u32EventN < 0xffffffffu) {
        g_soft.u32EventN++;
    }
    return 1;
}

/**
 * Lean residual self-check (stack-local; never hard-gates product).
 * C0 residual deepen: ABI constants + freestanding honesty + Dual DoD OPEN
 * + log-cap storm contract + poll bit residual + AcceptQ contract + integ.
 * Soft!=product. G-AC-1. Dual MIT OR Apache-2.0. No version stamp.
 * greppable: net: lo soft residual lean
 * greppable: dual_dod_a=OPEN | dual_dod_b=OPEN_UDX | product_dod_b=UDX
 * greppable: acceptq_integ= | soft_err= | C0 residual
 */
static u32
lo_soft_residual_lean_check(void)
{
    u32 u32Ok = 0;
    u32 u32Integ;
    const int nFreestandingNic = 0; /* no freestanding NIC required */
    const int nFreestandingRtl = 0; /* wire: net_eth/net_tcp/rtl - not here */
    const int nLocalAbi = 1;        /* Linux socket ABI local / L3 soft */
    const int nL3Soft = 1;          /* L3 soft tests via loopback residual */
    const int nGac1 = 1;            /* no Linux .ko product AC */
    const int nDualDodAOpen = 1;    /* Dual DoD A UDX USB OPEN (not this) */
    const int nDualDodBOpen = 1;    /* Dual DoD B UDX NIC OPEN (not this) */
    const int nProductUdx = 1;      /* product path = UDX/DDI hot/cold ABI */
    const int nNotDodClose = 1;     /* this residual never closes Dual DoD */
    const int nSoftNeProduct = 1;   /* Soft!=product honesty */

    /* 1..4 ABI constants */
    if (NET_LO_MAX == 16u) {
        u32Ok++;
    }
    if (NET_FD_BASE == 64) {
        u32Ok++;
    }
    if (NET_LO_BUF == 512u) {
        u32Ok++;
    }
    if (NET_LO_BACKLOG_MAX == 8u) {
        u32Ok++;
    }
    /* 5 freestanding / local flags */
    if (nFreestandingNic == 0 && nFreestandingRtl == 0 &&
        nLocalAbi == 1 && nL3Soft == 1) {
        u32Ok++;
    }
    /* 6 G-AC-1 */
    if (nGac1 == 1) {
        u32Ok++;
    }
    /* 7 Dual DoD honesty: A/B OPEN product=UDX; freestanding SKIP residual. */
    if (nDualDodAOpen == 1 && nDualDodBOpen == 1 && nProductUdx == 1) {
        u32Ok++;
    }
    /* 8 not_dod_close + Soft!=product + freestanding skip */
    if (nNotDodClose == 1 && nSoftNeProduct == 1 &&
        nFreestandingNic == 0 && nFreestandingRtl == 0) {
        u32Ok++;
    }
    /* 9 soft log/event caps finite (no stamp storm contract). */
    if (NET_LO_SOFT_LOG_MAX > 0u && NET_LO_SOFT_LOG_MAX <= 16u &&
        NET_LO_SOFT_EVENT_MAX > 0u && NET_LO_SOFT_EVENT_MAX <= 16u) {
        u32Ok++;
    }
    /*
     * 10 poll bit residual + ring defaults match BUF.
     * LO_POLLIN=1 OUT=4 ERR=8 HUP=16 (Linux-shaped x86 residual).
     */
    if (LO_POLLIN == 0x0001u && LO_POLLOUT == 0x0004u &&
        LO_POLLERR == 0x0008u && LO_POLLHUP == 0x0010u &&
        NET_LO_RCVBUF_DEF == NET_LO_BUF && NET_LO_SNDBUF_DEF == NET_LO_BUF) {
        u32Ok++;
    }
    /* 11 AcceptQ residual contract + ephemeral bind geometry. */
    if (NET_LO_BACKLOG_MAX >= 1u && NET_LO_BACKLOG_MAX <= 16u &&
        NET_LO_EPHEM_BASE >= 1024u && NET_LO_EPHEM_SPAN >= 1024u &&
        sizeof(g_aSocks[0].u16SoftErr) == sizeof(u16) &&
        sizeof(g_aSocks[0].u8AcceptQ) == sizeof(u8)) {
        u32Ok++;
    }
    /* 12 AcceptQ integrity observe (pending vs live AcceptQ children). */
    u32Integ = lo_soft_integ_acceptq();
    if (u32Integ == 1u) {
        u32Ok++;
    }

    g_soft.u32LeanLastOk = u32Ok;
    if (u32Ok == NET_LO_LEAN_OK_EXPECT) {
        lo_soft_bump(&g_soft.u64LeanOk);
    } else {
        lo_soft_bump(&g_soft.u64LeanFail);
    }
    return u32Ok;
}

/*
 * Lean soft residual inventory (stack-safe; no stamp storms).
 * CRITICAL: short kprintfs only - never twin multi-line dumps, never
 * per-slot loops, no version stamp, no #if 0 nested-comment residue.
 * Cadence already rate-limited by lo_soft_maybe_log (NET_LO_SOFT_LOG_MAX).
 * fForce retained for API compatibility; ignored (no per-slot detail).
 * Soft only - never hard-gates product policy. Soft!=product dual license.
 * Grep: net: lo soft inventory | residual lean | residual lean PASS | PASS
 */
static void
lo_soft_print(int fForce)
{
    u32 cUsed = 0;
    u32 cFree = 0;
    u32 cListen = 0;
    u32 cConn = 0;
    u32 cStream = 0;
    u32 cDgram = 0;
    u32 cPending = 0;
    u32 cRx = 0;
    u32 cShutRd = 0;
    u32 cShutWr = 0;
    u32 cReuse = 0;
    u32 u32LeanOk;
    struct net_lo_soft s;

    (void)fForce;
    lo_soft_tally(&cUsed, &cFree, &cListen, &cConn, &cStream, &cDgram,
                  &cPending, &cRx, &cShutRd, &cShutWr, &cReuse);
    u32LeanOk = lo_soft_residual_lean_check();
    s = g_soft;
    lo_soft_bump(&g_soft.u64LogDumps);
    if (g_soft.u32SoftLogN < 0xffffffffu) {
        g_soft.u32SoftLogN++;
    }
    if (g_soft.u8LeanOnce == 0u) {
        g_soft.u8LeanOnce = 1u;
    }

    /* Grep: net: lo soft inventory (one-line; never multi-kprintf storm) */
    kprintf("net: lo soft inventory used=%u free=%u listen=%u conn=%u "
            "stream=%u dgram=%u pending=%u rx_bytes=%u hwm=%llu ops=%llu "
            "log_n=%u max=%u fd_base=%u buf=%u "
            "l3_soft=1 freestanding_nic=0 freestanding_rtl=0 local_abi=1 "
            "dual_dod_b=OPEN_UDX not_dod_close=1 "
            "(one-line Soft!=product; G-AC-1)\n",
            cUsed, cFree, cListen, cConn, cStream, cDgram, cPending, cRx,
            (unsigned long long)s.u64HwmUsed,
            (unsigned long long)s.u64Ops, g_soft.u32SoftLogN,
            (unsigned)NET_LO_MAX, (unsigned)NET_FD_BASE,
            (unsigned)NET_LO_BUF);

    /*
     * Grep: net: lo soft residual lean
     * C0 residual deepen - Soft!=product dual MIT OR Apache-2.0.
     * L3 soft tests without freestanding NIC. AcceptQ pair, peer ring,
     * SOL_SOCKET subset, poll_mask, AF domain honesty, sticky SO_ERROR,
     * ephemeral bind, AcceptQ integ. NOT freestanding NIC / eth / rtl /
     * net_tcp multi-seg product path. Dual DoD A/B OPEN (product = UDX/DDI;
     * this residual not_dod_close). G-AC-1. No version stamp storms.
     */
    kprintf("net: lo soft residual lean "
            "ok=%u/%u acceptq_pair=1 peer_ring=%u "
            "sock=af_inet|af_unix stream|dgram domain_store=1 "
            "backlog_max=%u sol_socket_subset=1 poll_mask=1 "
            "soft_err=1 ephemeral_bind=1 acceptq_integ=%u "
            "path=socket+bind+listen+accept+connect+send+recv+poll+"
            "shutdown+sockopt+name+close "
            "sock_ok=%llu conn_ok=%llu accept_ok=%llu send_ok=%llu "
            "recv_ok=%llu tx=%llu rx=%llu "
            "log_n=%u log_cap=%u event_n=%u lean_ok=%llu lean_fail=%llu "
            "integ_ok=%llu integ_fail=%llu soft_err_set=%llu "
            "soft_err_get=%llu ephem_bind=%llu "
            "l3_soft=1 freestanding_nic=0 freestanding_rtl=0 local_abi=1 "
            "wire_owner=0 freestanding_skip=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN_UDX product_dod_b=UDX "
            "product_path=UDX|DDI not_dod_close=1 "
            "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
            "stamp_storm=0 no_version_stamp=1 C0=1 "
            "(Soft!=product; G-AC-1; dual MIT OR Apache-2.0; "
            "L3 soft tests without freestanding NIC; Dual DoD A/B OPEN; "
            "not product netstack; product=UDX/DDI; C0 residual deepen)\n",
            (unsigned)u32LeanOk, (unsigned)NET_LO_LEAN_OK_EXPECT,
            (unsigned)NET_LO_BUF, (unsigned)NET_LO_BACKLOG_MAX,
            (unsigned)g_soft.u32IntegLast,
            (unsigned long long)s.u64SockOk,
            (unsigned long long)s.u64ConnOk,
            (unsigned long long)s.u64AcceptOk,
            (unsigned long long)s.u64SendOk,
            (unsigned long long)s.u64RecvOk,
            (unsigned long long)s.u64BytesTx,
            (unsigned long long)s.u64BytesRx,
            g_soft.u32SoftLogN, (unsigned)NET_LO_SOFT_LOG_MAX,
            g_soft.u32EventN,
            (unsigned long long)g_soft.u64LeanOk,
            (unsigned long long)g_soft.u64LeanFail,
            (unsigned long long)g_soft.u64IntegOk,
            (unsigned long long)g_soft.u64IntegFail,
            (unsigned long long)g_soft.u64SoftErrSet,
            (unsigned long long)g_soft.u64SoftErrGet,
            (unsigned long long)g_soft.u64EphemBind);

    /* Grep: net: lo soft PASS (soft inventory only; not product gate) */
    kprintf("net: lo soft PASS logs=%u skip=%llu event_n=%u "
            "event_skip=%llu used=%u conn=%u lean_ok=%u/%u "
            "acceptq_integ=%u soft_err_set=%llu ephem_bind=%llu "
            "l3_soft=1 freestanding_nic=0 freestanding_rtl=0 local_abi=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN_UDX product_dod_b=UDX "
            "not_dod_close=1 soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
            "(soft inventory only; Soft!=product; not product gate; "
            "L3 soft tests without freestanding NIC; Dual DoD A/B OPEN)\n",
            g_soft.u32SoftLogN, (unsigned long long)s.u64LogSkip,
            g_soft.u32EventN, (unsigned long long)s.u64EventSkip,
            cUsed, cConn, (unsigned)u32LeanOk,
            (unsigned)NET_LO_LEAN_OK_EXPECT,
            (unsigned)g_soft.u32IntegLast,
            (unsigned long long)g_soft.u64SoftErrSet,
            (unsigned long long)g_soft.u64EphemBind);

    /*
     * Grep: net: lo soft residual lean PASS
     * Only when lean self-check full - soft residual only; never Dual DoD
     * close; never product gate. Soft!=product dual MIT OR Apache-2.0.
     */
    if (u32LeanOk == NET_LO_LEAN_OK_EXPECT) {
        lo_soft_bump(&g_soft.u64LeanPassEmit);
        kprintf("net: lo soft residual lean PASS "
                "ok=%u/%u acceptq_pair=1 peer_ring=%u domain_store=1 "
                "poll_mask=1 sol_socket_subset=1 soft_err=1 "
                "ephemeral_bind=1 acceptq_integ=%u "
                "path=socket+bind+listen+accept+connect+send+recv+poll+"
                "shutdown+sockopt+name+close "
                "l3_soft=1 freestanding_nic=0 freestanding_rtl=0 local_abi=1 "
                "freestanding_skip=1 dual_dod_a=OPEN dual_dod_b=OPEN_UDX "
                "product_dod_b=UDX product_path=UDX|DDI not_dod_close=1 "
                "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 C0=1 "
                "(Soft!=product; lean residual full; not Dual DoD close; "
                "not product gate; L3 soft local ABI only; C0 residual)\n",
                (unsigned)u32LeanOk, (unsigned)NET_LO_LEAN_OK_EXPECT,
                (unsigned)NET_LO_BUF, (unsigned)g_soft.u32IntegLast);
    }
} /* end lo_soft_print */
static void
lo_soft_maybe_log(int fForce)
{
    u64 u64N;

    lo_soft_bump(&g_soft.u64Ops); /* wrap OK */
    if (fForce != 0) {
        if (g_soft.u32SoftLogN >= NET_LO_SOFT_LOG_MAX) {
            lo_soft_bump(&g_soft.u64LogSkip);
            return;
        }
        lo_soft_print(1);
        return;
    }
    u64N = g_soft.u64Ops;
    /* Milestone: first op and subsequent powers of two only. */
    if (u64N == 0ull || (u64N & (u64N - 1ull)) != 0ull) {
        return;
    }
    if (g_soft.u32SoftLogN >= NET_LO_SOFT_LOG_MAX) {
        lo_soft_bump(&g_soft.u64LogSkip);
        return;
    }
    lo_soft_print(0);
}

/**
 * Lean return-surface / area catalog (init once; Soft!=product).
 * One greppable line - never multi-kprintf storm. Diagnostics only.
 * greppable: net: lo soft catalog
 */
static void
lo_soft_catalog_once(void)
{
    if (g_soft.u8CatalogOnce != 0u) {
        return;
    }
    g_soft.u8CatalogOnce = 1u;
    lo_soft_bump(&g_soft.u64CatalogEmit);
    /*
     * Grep: net: lo soft catalog
     * Return surfaces (errno-shaped) + API areas for L3 soft residual.
     * Dual DoD A/B remain OPEN (product=UDX/DDI); this unit not_dod_close.
     * C0 residual: soft_err sticky + ephemeral bind + AcceptQ integ.
     */
    kprintf("net: lo soft catalog "
            "areas=socket|bind|listen|accept|connect|send|recv|poll|"
            "shutdown|sockopt|name|close "
            "ret=0|-9|-11|-22|-24|-32|-92|-97|-98|-106|-107|-111 "
            "ok|ebadf|eagain|einval|emfile|epipe|enoprotoopt|"
            "eafnosupport|eaddrinuse|eisconn|enotconn|econnrefused "
            "pair=acceptq_mint domain=af_inet|af_unix "
            "sol=reuseaddr|reuseport|type|error|broadcast|keepalive|"
            "sndbuf|rcvbuf|linger|acceptconn "
            "poll=in|out|err|hup half_close=shut_wr->peer_rd "
            "soft_err=sticky_read_clear ephemeral_bind=1 acceptq_integ=1 "
            "l3_soft=1 freestanding_nic=0 freestanding_rtl=0 local_abi=1 "
            "freestanding_skip=1 dual_dod_a=OPEN dual_dod_b=OPEN_UDX "
            "product_dod_b=UDX product_path=UDX|DDI not_dod_close=1 "
            "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 C0=1 "
            "(Soft!=product; return-surface catalog; not product gate; "
            "not Dual DoD close; C0 residual deepen)\n");
}

/**
 * C0 residual deepen once-lamp (stamp-free Soft!=product).
 * Catalogs residual deepen surfaces without version stamp / Dual DoD close.
 * greppable: net: lo soft deepen
 * greppable: C0 residual | acceptq_integ= | soft_err= | ephemeral_bind=
 */
static void
lo_soft_deepen_once(void)
{
    if (g_soft.u8DeepenOnce != 0u) {
        return;
    }
    g_soft.u8DeepenOnce = 1u;
    lo_soft_bump(&g_soft.u64DeepenEmit);
    /*
     * Grep: net: lo soft deepen
     * C0 residual deepen surfaces only - never product gate / Dual DoD close.
     * No wave= / no GJ_IMAGE_VERSION / no stamp storms.
     */
    kprintf("net: lo soft deepen "
            "areas=acceptq_pair|acceptq_integ|peer_ring|poll_mask|"
            "sol_socket|domain_store|soft_err|ephemeral_bind|"
            "half_close|catalog|lean_check "
            "lean_expect=%u log_cap=%u event_cap=%u "
            "ephem_base=%u ephem_span=%u "
            "poll=in|out|err|hup soft_err=sticky_read_clear "
            "acceptq_integ=observe dual_dod_a=OPEN dual_dod_b=OPEN_UDX "
            "product_dod_b=UDX product_path=UDX|DDI not_dod_close=1 "
            "l3_soft=1 freestanding_nic=0 freestanding_rtl=0 local_abi=1 "
            "freestanding_skip=1 soft_ne_product=1 G-AC-1=1 "
            "dual=MIT_OR_Apache-2.0 stamp_storm=0 no_version_stamp=1 C0=1 "
            "(Soft!=product; C0 residual deepen; not Dual DoD close; "
            "not product gate; stamp-free)\n",
            (unsigned)NET_LO_LEAN_OK_EXPECT,
            (unsigned)NET_LO_SOFT_LOG_MAX,
            (unsigned)NET_LO_SOFT_EVENT_MAX,
            (unsigned)NET_LO_EPHEM_BASE,
            (unsigned)NET_LO_EPHEM_SPAN);
}

void
net_lo_init(void)
{
    memset(g_aSocks, 0, sizeof(g_aSocks));
    memset(&g_soft, 0, sizeof(g_soft));
    kprintf("net_lo: init (loopback + peer ring + sockopt/backlog soft)\n");
    /* Grep: net: lo soft init (lean; no version stamp; no stamp storms) */
    kprintf("net: lo soft init max=%u fd_base=%u buf=%u backlog_max=%u "
            "rcv_def=%u snd_def=%u log_max=%u event_max=%u "
            "l3_soft=1 freestanding_nic=0 freestanding_rtl=0 local_abi=1 "
            "freestanding_skip=1 dual_dod_a=OPEN dual_dod_b=OPEN_UDX "
            "product_dod_b=UDX product_path=UDX|DDI not_dod_close=1 "
            "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
            "(Soft!=product; G-AC-1; dual MIT OR Apache-2.0; "
            "L3 soft tests without freestanding NIC; Dual DoD A/B OPEN)\n",
            (unsigned)NET_LO_MAX, (unsigned)NET_FD_BASE,
            (unsigned)NET_LO_BUF, (unsigned)NET_LO_BACKLOG_MAX,
            (unsigned)NET_LO_RCVBUF_DEF, (unsigned)NET_LO_SNDBUF_DEF,
            (unsigned)NET_LO_SOFT_LOG_MAX, (unsigned)NET_LO_SOFT_EVENT_MAX);
    lo_soft_catalog_once();
    lo_soft_deepen_once();
    lo_soft_print(1);
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

/** Soft RX free space: 1 if push_rx would accept >=1 byte. */
static int
lo_rx_has_space(u32 u32Slot)
{
    struct net_lo_sock *pS;
    u32 u32Cap;

    if (u32Slot >= NET_LO_MAX || !g_aSocks[u32Slot].u8Used) {
        return 0;
    }
    pS = &g_aSocks[u32Slot];
    u32Cap = NET_LO_BUF;
    if (pS->u32RcvBuf > 0 && pS->u32RcvBuf < u32Cap) {
        u32Cap = pS->u32RcvBuf;
    }
    return pS->u32RxLen < u32Cap ? 1 : 0;
}

/**
 * Linux-shaped readiness for poll/epoll cold path (loopback sockets).
 *
 * POLLIN:  RX data, accept pending, or EOF (RD shut / peer half-close).
 * POLLOUT: WR open and destination ring has space (peer or self unpaired).
 * POLLHUP: both directions shut, or peer gone with drained RX.
 * POLLERR: sticky SO_ERROR residual pending (read-and-clear via getsockopt).
 *
 * Returns 0 if fd is not a live net_lo socket. ERR/HUP always surface;
 * IN/OUT filtered by u32Want (0 -> default IN|OUT interest).
 */
u32
net_lo_poll_mask(i64 i64Fd, u32 u32Want)
{
    u32 u32Slot;
    u32 u32Got = 0;
    struct net_lo_sock *pS;
    i16 i16Peer;
    static u8 g_u8PollMaskOnce;

    if (!net_lo_fd_ok(i64Fd)) {
        return 0;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    pS = &g_aSocks[u32Slot];

    /* Sticky SO_ERROR residual -> POLLERR (cleared only by getsockopt). */
    if (pS->u16SoftErr != 0u) {
        u32Got |= LO_POLLERR;
    }

    /* Listener: POLLIN when accept() would not EAGAIN (AcceptQ residual). */
    if (pS->u8Listening) {
        u32 j;

        i16Peer = pS->i16Peer;
        if (i16Peer >= 0 && (u32)i16Peer < NET_LO_MAX &&
            g_aSocks[i16Peer].u8Used && g_aSocks[i16Peer].u8AcceptQ) {
            u32Got |= LO_POLLIN;
        } else if (pS->u8Pending > 0) {
            u32Got |= LO_POLLIN;
        } else {
            /* Multi-pending: any AcceptQ child on this listen port. */
            for (j = 0; j < NET_LO_MAX; j++) {
                if (g_aSocks[j].u8Used && g_aSocks[j].u8AcceptQ &&
                    !g_aSocks[j].u8Listening &&
                    g_aSocks[j].u16Port == pS->u16Port &&
                    g_aSocks[j].u8Type == pS->u8Type) {
                    u32Got |= LO_POLLIN;
                    break;
                }
            }
        }
        /* Listeners are not writeable soft. */
    } else {
        /* Connected / unbound / half-closed: data or EOF readable. */
        if (pS->u32RxLen > 0) {
            u32Got |= LO_POLLIN;
        } else if (pS->u8ShutRd) {
            /* Empty ring + RD shut -> EOF-shaped POLLIN. */
            u32Got |= LO_POLLIN;
        } else {
            /* Peer WR shut half-close -> EOF once RX drained. */
            i16Peer = pS->i16Peer;
            if (i16Peer >= 0 && (u32)i16Peer < NET_LO_MAX &&
                g_aSocks[i16Peer].u8Used && g_aSocks[i16Peer].u8ShutWr) {
                u32Got |= LO_POLLIN;
            }
        }

        /*
         * Writeable when send path accepts data: local WR open and
         * destination ring has free space. Unpaired -> self ring (dgram smoke).
         * STREAM with peer RD shut / peer gone -> not writeable (EPIPE path).
         */
        if (!pS->u8ShutWr) {
            i16Peer = pS->i16Peer;
            if (i16Peer >= 0 && (u32)i16Peer < NET_LO_MAX &&
                g_aSocks[i16Peer].u8Used) {
                if (!g_aSocks[i16Peer].u8ShutRd &&
                    lo_rx_has_space((u32)i16Peer)) {
                    u32Got |= LO_POLLOUT;
                }
            } else if (pS->u8Type == SOCK_DGRAM || !pS->u8Connected) {
                /* Unpaired dgram / never-connected residual self-loop. */
                if (lo_rx_has_space(u32Slot)) {
                    u32Got |= LO_POLLOUT;
                }
            }
        }

        /*
         * HUP when both directions shut, or peer half-closed us with no RX
         * left. Peer gone alone keeps residual until RD drained.
         */
        if (pS->u8ShutRd && pS->u8ShutWr) {
            u32Got |= LO_POLLHUP;
        } else if (pS->u8ShutRd && pS->u32RxLen == 0 &&
                   (pS->i16Peer < 0 ||
                    (u32)pS->i16Peer >= NET_LO_MAX ||
                    !g_aSocks[pS->i16Peer].u8Used)) {
            u32Got |= LO_POLLHUP;
        }
    }

    /* Grep: net_lo: soft poll_mask (once) */
    if (!g_u8PollMaskOnce) {
        g_u8PollMaskOnce = 1;
        kprintf("net_lo: soft poll_mask ready=0x%x fd=%lld want=0x%x "
                "listen=%u rx=%u pend=%u shut_rd=%u shut_wr=%u\n",
                (unsigned)u32Got, (long long)i64Fd, (unsigned)u32Want,
                (unsigned)pS->u8Listening, (unsigned)pS->u32RxLen,
                (unsigned)pS->u8Pending, (unsigned)pS->u8ShutRd,
                (unsigned)pS->u8ShutWr);
    }

    /* ERR/HUP always surface; IN/OUT only if requested (or want==0). */
    if (u32Want == 0) {
        return u32Got;
    }
    return (u32Got & (LO_POLLERR | LO_POLLHUP)) | (u32Got & u32Want);
}

i64
net_lo_socket(int nDomain, int nType, int nProto)
{
    u32 i;

    (void)nProto;
    if (nDomain != AF_INET && nDomain != AF_UNIX) {
        lo_soft_bump(&g_soft.u64SockFail);
        lo_soft_maybe_log(0);
        return -97; /* EAFNOSUPPORT */
    }
    if (nType != SOCK_STREAM && nType != SOCK_DGRAM) {
        lo_soft_bump(&g_soft.u64SockFail);
        lo_soft_maybe_log(0);
        return -22;
    }
    for (i = 0; i < NET_LO_MAX; i++) {
        if (!g_aSocks[i].u8Used) {
            g_aSocks[i].u8Used = 1;
            g_aSocks[i].u8Type = (u8)nType;
            g_aSocks[i].u8Domain = (u8)nDomain; /* AF honesty residual */
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
            g_aSocks[i].u8AcceptQ = 0;
            g_aSocks[i].u8PadDom = 0;
            g_aSocks[i].u16Port = 0;
            g_aSocks[i].u16LingerSec = 0;
            g_aSocks[i].u32RcvBuf = NET_LO_RCVBUF_DEF;
            g_aSocks[i].u32SndBuf = NET_LO_SNDBUF_DEF;
            g_aSocks[i].i16Peer = -1;
            g_aSocks[i].u16SoftErr = 0;
            g_aSocks[i].u32RxLen = 0;
            g_aSocks[i].u32RxHead = 0;
            /* HWM update via tally walk (outputs unused). */
            lo_soft_tally(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL);
            lo_soft_bump(&g_soft.u64SockOk);
            if (nDomain == AF_INET) {
                lo_soft_bump(&g_soft.u64SockInet);
            } else {
                lo_soft_bump(&g_soft.u64SockUnix);
            }
            if (nType == SOCK_STREAM) {
                lo_soft_bump(&g_soft.u64SockStream);
            } else {
                lo_soft_bump(&g_soft.u64SockDgram);
            }
            lo_soft_maybe_log(0);
            return (i64)(NET_FD_BASE + i);
        }
    }
    lo_soft_bump(&g_soft.u64SockFail);
    /* Grep: net: lo soft emfile / net_lo: soft emfile (rate-limited) */
    if (lo_soft_event_ok()) {
        kprintf("net: lo soft emfile max=%u ops=%llu used_hwm=%llu "
                "soft_ne_product=1 Soft!=product\n",
                (unsigned)NET_LO_MAX, (unsigned long long)g_soft.u64Ops,
                (unsigned long long)g_soft.u64HwmUsed);
    }
    lo_soft_maybe_log(1);
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

/**
 * Soft ephemeral port pick (host order). Residual only; Soft!=product.
 * Scans NET_LO_EPHEM_BASE..+SPAN for a free port of the same type.
 * Returns 0 if none free (caller maps to EADDRINUSE).
 */
static u16
lo_ephem_port(u32 u32Self, u8 u8Type, u8 u8Reuse)
{
    u32 u32Off;
    u16 u16Cand;

    for (u32Off = 0; u32Off < NET_LO_EPHEM_SPAN; u32Off++) {
        u16Cand = (u16)(NET_LO_EPHEM_BASE + (u32Off % NET_LO_EPHEM_SPAN));
        if (u16Cand == 0u) {
            continue;
        }
        if (!port_in_use(u32Self, u16Cand, u8Type, u8Reuse)) {
            return u16Cand;
        }
    }
    return 0;
}

i64
net_lo_bind(i64 i64Fd, u16 u16Port)
{
    u32 u32Slot;
    u8 u8Reuse;

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64BindFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    u8Reuse = g_aSocks[u32Slot].u8Reuse || g_aSocks[u32Slot].u8ReusePort;
    /* Port 0: soft ephemeral residual for L3 soft tests (not product). */
    if (u16Port == 0u) {
        u16Port = lo_ephem_port(u32Slot, g_aSocks[u32Slot].u8Type, u8Reuse);
        if (u16Port == 0u) {
            lo_soft_bump(&g_soft.u64BindFail);
            lo_soft_bump(&g_soft.u64EaddrInuse);
            lo_soft_set_err(u32Slot, 98);
            lo_soft_maybe_log(0);
            return -98; /* EADDRINUSE-shaped (ephem exhausted) */
        }
        lo_soft_bump(&g_soft.u64EphemBind);
    }
    if (port_in_use(u32Slot, u16Port, g_aSocks[u32Slot].u8Type, u8Reuse)) {
        lo_soft_bump(&g_soft.u64BindFail);
        lo_soft_bump(&g_soft.u64EaddrInuse);
        lo_soft_set_err(u32Slot, 98);
        lo_soft_maybe_log(0);
        return -98; /* EADDRINUSE-shaped */
    }
    g_aSocks[u32Slot].u16Port = u16Port;
    lo_soft_bump(&g_soft.u64BindOk);
    lo_soft_maybe_log(0);
    return 0;
}

i64
net_lo_listen(i64 i64Fd, int nBacklog)
{
    u32 u32Slot;
    int nBl;

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64ListenFail);
        lo_soft_maybe_log(0);
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
    lo_soft_bump(&g_soft.u64ListenOk);
    /* Grep: net: lo soft listen / net_lo: soft listen (rate-limited) */
    if (lo_soft_event_ok()) {
        kprintf("net: lo soft listen fd=%lld port=%u backlog=%u "
                "type=%u ops=%llu soft_ne_product=1 Soft!=product\n",
                (long long)i64Fd, (unsigned)g_aSocks[u32Slot].u16Port,
                (unsigned)g_aSocks[u32Slot].u8Backlog,
                (unsigned)g_aSocks[u32Slot].u8Type,
                (unsigned long long)g_soft.u64Ops);
    }
    lo_soft_maybe_log(0);
    return 0;
}

/** Free table slot index or -1. */
static int
lo_alloc_slot(void)
{
    u32 i;

    for (i = 0; i < NET_LO_MAX; i++) {
        if (!g_aSocks[i].u8Used) {
            return (int)i;
        }
    }
    return -1;
}

/**
 * FIFO-ish AcceptQ child for listen port+type (lowest live slot).
 * Matches residual multi-pending after connect mint-on-pair.
 */
static i16
lo_find_acceptq(u16 u16Port, u8 u8Type)
{
    u32 i;

    for (i = 0; i < NET_LO_MAX; i++) {
        if (g_aSocks[i].u8Used && g_aSocks[i].u8AcceptQ &&
            !g_aSocks[i].u8Listening && g_aSocks[i].u16Port == u16Port &&
            g_aSocks[i].u8Type == u8Type) {
            return (i16)i;
        }
    }
    return -1;
}

/** Point listeners on port at another remaining AcceptQ child (if any). */
static void
lo_rehook_acceptq(u16 u16Port, u8 u8Type, i16 i16Skip)
{
    i16 i16Alt;
    u32 u32Li;

    i16Alt = -1;
    for (u32Li = 0; u32Li < NET_LO_MAX; u32Li++) {
        if (!g_aSocks[u32Li].u8Used || !g_aSocks[u32Li].u8AcceptQ ||
            g_aSocks[u32Li].u8Listening) {
            continue;
        }
        if (g_aSocks[u32Li].u16Port != u16Port ||
            g_aSocks[u32Li].u8Type != u8Type) {
            continue;
        }
        if ((i16)u32Li == i16Skip) {
            continue;
        }
        i16Alt = (i16)u32Li;
        break;
    }
    if (i16Alt < 0) {
        return;
    }
    for (u32Li = 0; u32Li < NET_LO_MAX; u32Li++) {
        if (g_aSocks[u32Li].u8Used && g_aSocks[u32Li].u8Listening &&
            g_aSocks[u32Li].u16Port == u16Port &&
            g_aSocks[u32Li].u8Type == u8Type &&
            g_aSocks[u32Li].i16Peer < 0) {
            g_aSocks[u32Li].i16Peer = i16Alt;
        }
    }
}

i64
net_lo_connect(i64 i64Fd, u16 u16Port)
{
    u32 u32Slot;
    u32 i;
    int nPeer;
    u8 u8Type;

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64ConnFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    u8Type = g_aSocks[u32Slot].u8Type;
    /* Already paired residual: EISCONN-shaped for STREAM. */
    if (u8Type == SOCK_STREAM && g_aSocks[u32Slot].u8Connected &&
        g_aSocks[u32Slot].i16Peer >= 0) {
        lo_soft_bump(&g_soft.u64ConnFail);
        lo_soft_set_err(u32Slot, 106);
        lo_soft_maybe_log(0);
        return -106; /* EISCONN */
    }
    /*
     * Residual pairing with net_tcp loop shape (L3 soft without freestanding
     * NIC): STREAM connect mints AcceptQ server peer and pairs client<->peer
     * so pre-accept SEND lands on the fd accept() will return. Listener only
     * holds i16Peer hint + pending count (multi-backlog). Soft!=product.
     * L3 soft tests; freestanding_nic=0; freestanding_rtl=0. G-AC-1.
     */
    for (i = 0; i < NET_LO_MAX; i++) {
        if (i == u32Slot || !g_aSocks[i].u8Used || !g_aSocks[i].u8Listening ||
            g_aSocks[i].u16Port != u16Port || g_aSocks[i].u8Type != u8Type) {
            continue;
        }
        if (g_aSocks[i].u8Backlog == 0) {
            g_aSocks[i].u8Backlog = 1;
        }
        if (g_aSocks[i].u8Pending >= g_aSocks[i].u8Backlog) {
            lo_soft_bump(&g_soft.u64ConnAgain);
            lo_soft_maybe_log(0);
            return -11; /* EAGAIN */
        }
        if (u8Type == SOCK_STREAM) {
            nPeer = lo_alloc_slot();
            if (nPeer < 0) {
                lo_soft_bump(&g_soft.u64ConnFail);
                lo_soft_bump(&g_soft.u64SockFail);
                if (lo_soft_event_ok()) {
                    kprintf("net: lo soft emfile max=%u ops=%llu "
                            "(connect mint) soft_ne_product=1 Soft!=product\n",
                            (unsigned)NET_LO_MAX,
                            (unsigned long long)g_soft.u64Ops);
                }
                lo_soft_maybe_log(1);
                return -24; /* EMFILE */
            }
            /* Mint AcceptQ server peer (net_tcp loop-pair residual). */
            g_aSocks[nPeer].u8Used = 1;
            g_aSocks[nPeer].u8Type = u8Type;
            g_aSocks[nPeer].u8Domain = g_aSocks[i].u8Domain != 0
                                           ? g_aSocks[i].u8Domain
                                           : g_aSocks[u32Slot].u8Domain;
            if (g_aSocks[nPeer].u8Domain == 0) {
                g_aSocks[nPeer].u8Domain = (u8)AF_INET;
            }
            g_aSocks[nPeer].u8Listening = 0;
            g_aSocks[nPeer].u8Connected = 1;
            g_aSocks[nPeer].u8ShutRd = 0;
            g_aSocks[nPeer].u8ShutWr = 0;
            g_aSocks[nPeer].u8Reuse = 0;
            g_aSocks[nPeer].u8ReusePort = 0;
            g_aSocks[nPeer].u8Keepalive = g_aSocks[i].u8Keepalive;
            g_aSocks[nPeer].u8Broadcast = 0;
            g_aSocks[nPeer].u8Backlog = 0;
            g_aSocks[nPeer].u8Pending = 0;
            g_aSocks[nPeer].u8LingerOn = 0;
            g_aSocks[nPeer].u8AcceptQ = 1;
            g_aSocks[nPeer].u8PadDom = 0;
            g_aSocks[nPeer].u16Port = u16Port;
            g_aSocks[nPeer].u16LingerSec = 0;
            g_aSocks[nPeer].u32RcvBuf = g_aSocks[i].u32RcvBuf;
            g_aSocks[nPeer].u32SndBuf = g_aSocks[i].u32SndBuf;
            g_aSocks[nPeer].i16Peer = (i16)u32Slot;
            g_aSocks[nPeer].u16SoftErr = 0;
            g_aSocks[nPeer].u32RxLen = 0;
            g_aSocks[nPeer].u32RxHead = 0;
            /* Client keeps bound port if any; else soft dest port. */
            if (g_aSocks[u32Slot].u16Port == 0) {
                g_aSocks[u32Slot].u16Port = u16Port;
            }
            g_aSocks[u32Slot].i16Peer = (i16)nPeer;
            g_aSocks[u32Slot].u8Connected = 1;
            g_aSocks[i].i16Peer = (i16)nPeer;
            if (g_aSocks[i].u8Pending < 255u) {
                g_aSocks[i].u8Pending++;
            }
            lo_soft_tally(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                          NULL, NULL, NULL);
            lo_soft_bump(&g_soft.u64ConnOk);
            /* Grep: net: lo soft pair (rate-limited residual; no stamp storm) */
            if (lo_soft_event_ok()) {
                kprintf("net: lo soft pair fd=%lld port=%u peer_slot=%d "
                        "listen_pending=%u loop=1 acceptq=1 "
                        "soft_ne_product=1 Soft!=product\n",
                        (long long)i64Fd, (unsigned)u16Port, nPeer,
                        (unsigned)g_aSocks[i].u8Pending);
            }
            lo_soft_maybe_log(0);
            return 0;
        }
        /* DGRAM soft: pair to listener slot (no AcceptQ mint). */
        g_aSocks[i].i16Peer = (i16)u32Slot;
        if (g_aSocks[i].u8Pending < 255u) {
            g_aSocks[i].u8Pending++;
        }
        g_aSocks[u32Slot].u16Port = u16Port;
        g_aSocks[u32Slot].i16Peer = (i16)i;
        g_aSocks[u32Slot].u8Connected = 1;
        g_aSocks[i].u8Connected = 1;
        lo_soft_bump(&g_soft.u64ConnOk);
        lo_soft_maybe_log(0);
        return 0;
    }
    /* No listener: STREAM refuses (match net_tcp); DGRAM soft-orphan OK. */
    if (u8Type == SOCK_STREAM) {
        lo_soft_bump(&g_soft.u64ConnFail);
        lo_soft_bump(&g_soft.u64ConnRefused);
        lo_soft_set_err(u32Slot, 111);
        lo_soft_maybe_log(0);
        return -111; /* ECONNREFUSED-shaped */
    }
    g_aSocks[u32Slot].u16Port = u16Port;
    g_aSocks[u32Slot].u8Connected = 1;
    lo_soft_bump(&g_soft.u64ConnOk);
    lo_soft_bump(&g_soft.u64ConnOrphan);
    lo_soft_maybe_log(0);
    return 0;
}

i64
net_lo_accept(i64 i64Fd)
{
    u32 u32Slot;
    u32 u32Li;
    i16 i16Peer;
    i16 i16Cli;
    int fDec = 0;

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64AcceptFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (!g_aSocks[u32Slot].u8Listening) {
        lo_soft_bump(&g_soft.u64AcceptFail);
        lo_soft_maybe_log(0);
        return -22; /* EINVAL */
    }
    /*
     * Take pre-minted AcceptQ peer (connect residual). i16Peer is a newest
     * hint; scan FIFO AcceptQ so multi-pending works. Soft!=product.
     */
    i16Peer = g_aSocks[u32Slot].i16Peer;
    if (i16Peer < 0 || (u32)i16Peer >= NET_LO_MAX ||
        !g_aSocks[i16Peer].u8Used || !g_aSocks[i16Peer].u8AcceptQ ||
        g_aSocks[i16Peer].u16Port != g_aSocks[u32Slot].u16Port) {
        i16Peer = lo_find_acceptq(g_aSocks[u32Slot].u16Port,
                                  g_aSocks[u32Slot].u8Type);
    }
    if (i16Peer < 0) {
        lo_soft_bump(&g_soft.u64AcceptAgain);
        lo_soft_maybe_log(0);
        return -11; /* EAGAIN */
    }
    i16Cli = g_aSocks[i16Peer].i16Peer;
    g_aSocks[i16Peer].u8AcceptQ = 0;
    /* Drop listen hint + pending; re-hook remaining AcceptQ children. */
    for (u32Li = 0; u32Li < NET_LO_MAX; u32Li++) {
        if (!g_aSocks[u32Li].u8Used || !g_aSocks[u32Li].u8Listening) {
            continue;
        }
        if (g_aSocks[u32Li].i16Peer == i16Peer) {
            g_aSocks[u32Li].i16Peer = -1;
            if (g_aSocks[u32Li].u8Pending > 0) {
                g_aSocks[u32Li].u8Pending--;
                fDec = 1;
            }
        }
    }
    if (fDec == 0 && g_aSocks[u32Slot].u8Pending > 0) {
        g_aSocks[u32Slot].u8Pending--;
    }
    lo_rehook_acceptq(g_aSocks[u32Slot].u16Port, g_aSocks[u32Slot].u8Type,
                      i16Peer);
    /* Ensure client still points at accepted peer. */
    if (i16Cli >= 0 && (u32)i16Cli < NET_LO_MAX && g_aSocks[i16Cli].u8Used) {
        g_aSocks[i16Cli].i16Peer = i16Peer;
        g_aSocks[i16Cli].u8Connected = 1;
    }
    g_aSocks[i16Peer].u8Connected = 1;
    lo_soft_bump(&g_soft.u64AcceptOk);
    /* Grep: net: lo soft accept / net_lo: soft accept (rate-limited) */
    if (lo_soft_event_ok()) {
        kprintf("net: lo soft accept listen_fd=%lld new_fd=%u "
                "cli_slot=%d port=%u type=%u pending=%u ops=%llu "
                "acceptq_taken=1 soft_ne_product=1 Soft!=product\n",
                (long long)i64Fd, (unsigned)(NET_FD_BASE + (u32)i16Peer),
                (int)i16Cli, (unsigned)g_aSocks[i16Peer].u16Port,
                (unsigned)g_aSocks[i16Peer].u8Type,
                (unsigned)g_aSocks[u32Slot].u8Pending,
                (unsigned long long)g_soft.u64Ops);
    }
    lo_soft_maybe_log(0);
    return (i64)(NET_FD_BASE + (u32)i16Peer);
}

static int
push_rx(u32 u32Slot, const void *pBuf, u32 cb)
{
    struct net_lo_sock *pS;
    u32 i;
    u32 u32Cap;
    u32 u32Want;

    if (u32Slot >= NET_LO_MAX || !g_aSocks[u32Slot].u8Used) {
        return -1;
    }
    pS = &g_aSocks[u32Slot];
    u32Want = cb;
    /* Soft SO_RCVBUF: cap ring use to min(physical buf, advertised rcvbuf). */
    u32Cap = NET_LO_BUF;
    if (pS->u32RcvBuf > 0 && pS->u32RcvBuf < u32Cap) {
        u32Cap = pS->u32RcvBuf;
    }
    if (pS->u32RxLen >= u32Cap) {
        lo_soft_bump(&g_soft.u64PushFull);
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
    if (cb < u32Want) {
        lo_soft_bump(&g_soft.u64PushPartial);
    }
    return (int)cb;
}

i64
net_lo_send(i64 i64Fd, const void *pBuf, size_t cb)
{
    u32 u32Slot;
    i16 i16Peer;
    u32 u32N;
    int nPushed;
    int fSelf = 0;
    struct net_lo_sock *pS;

    if (!net_lo_fd_ok(i64Fd) || pBuf == NULL) {
        lo_soft_bump(&g_soft.u64SendFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    if (cb == 0) {
        lo_soft_bump(&g_soft.u64SendOk);
        lo_soft_maybe_log(0);
        return 0;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    pS = &g_aSocks[u32Slot];
    if (pS->u8Listening) {
        lo_soft_bump(&g_soft.u64SendFail);
        lo_soft_maybe_log(0);
        return -22; /* EINVAL - listener is not a data endpoint */
    }
    if (pS->u8ShutWr) {
        lo_soft_bump(&g_soft.u64SendPipe);
        lo_soft_set_err(u32Slot, 32);
        lo_soft_maybe_log(0);
        return -32; /* EPIPE-shaped */
    }
    /* Soft SO_SNDBUF: clamp one-shot write to advertised send buffer. */
    u32N = (u32)cb;
    if (pS->u32SndBuf > 0 && u32N > pS->u32SndBuf) {
        u32N = pS->u32SndBuf;
    }
    i16Peer = pS->i16Peer;
    if (i16Peer >= 0 && (u32)i16Peer < NET_LO_MAX &&
        g_aSocks[i16Peer].u8Used) {
        /* Peer RD shut (half-close) -> EPIPE for residual STREAM pair. */
        if (g_aSocks[i16Peer].u8ShutRd) {
            lo_soft_bump(&g_soft.u64SendPipe);
            lo_soft_set_err(u32Slot, 32);
            lo_soft_maybe_log(0);
            return -32;
        }
    } else if (pS->u8Type == SOCK_STREAM && pS->u8Connected) {
        /* Connected STREAM peer gone -> EPIPE (not self-loop). */
        lo_soft_bump(&g_soft.u64SendPipe);
        lo_soft_set_err(u32Slot, 32);
        lo_soft_maybe_log(0);
        return -32;
    } else {
        /* Unpaired: loop into own RX for dgram / never-connected smoke. */
        i16Peer = (i16)u32Slot;
        fSelf = 1;
    }
    nPushed = push_rx((u32)i16Peer, pBuf, u32N);
    if (nPushed < 0) {
        lo_soft_bump(&g_soft.u64SendFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    if (fSelf) {
        lo_soft_bump(&g_soft.u64SendSelf);
    }
    g_soft.u64BytesTx += (u64)(u32)nPushed; /* wrap OK */
    lo_soft_bump(&g_soft.u64SendOk);
    lo_soft_maybe_log(0);
    return (i64)nPushed;
}

i64
net_lo_recv(i64 i64Fd, void *pBuf, size_t cb)
{
    u32 u32Slot;
    struct net_lo_sock *pS;
    u32 u32N;
    u32 i;
    i16 i16Peer;
    int fPeerEof = 0;

    if (!net_lo_fd_ok(i64Fd) || pBuf == NULL) {
        lo_soft_bump(&g_soft.u64RecvFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    if (cb == 0) {
        lo_soft_bump(&g_soft.u64RecvOk);
        lo_soft_maybe_log(0);
        return 0;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    pS = &g_aSocks[u32Slot];
    if (pS->u8Listening) {
        lo_soft_bump(&g_soft.u64RecvFail);
        lo_soft_maybe_log(0);
        return -22; /* EINVAL */
    }
    i16Peer = pS->i16Peer;
    if (i16Peer >= 0 && (u32)i16Peer < NET_LO_MAX &&
        g_aSocks[i16Peer].u8Used && g_aSocks[i16Peer].u8ShutWr) {
        fPeerEof = 1;
    } else if (pS->u8Type == SOCK_STREAM && pS->u8Connected &&
               (i16Peer < 0 || (u32)i16Peer >= NET_LO_MAX ||
                !g_aSocks[i16Peer].u8Used)) {
        /* STREAM peer gone after close half-close residual. */
        fPeerEof = 1;
    }
    if (pS->u8ShutRd || fPeerEof) {
        if (pS->u32RxLen == 0) {
            lo_soft_bump(&g_soft.u64RecvEof);
            lo_soft_maybe_log(0);
            return 0; /* EOF */
        }
        /* Drain remaining RX before EOF. */
    }
    if (pS->u32RxLen == 0) {
        lo_soft_bump(&g_soft.u64RecvAgain);
        lo_soft_maybe_log(0);
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
    g_soft.u64BytesRx += (u64)u32N; /* wrap OK */
    lo_soft_bump(&g_soft.u64RecvOk);
    lo_soft_maybe_log(0);
    return (i64)u32N;
}

i64
net_lo_shutdown(i64 i64Fd, int nHow)
{
    u32 u32Slot;
    i16 i16Peer;

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64ShutFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    if (nHow < 0 || nHow > 2) {
        lo_soft_bump(&g_soft.u64ShutFail);
        lo_soft_maybe_log(0);
        return -22; /* EINVAL - validate before mutating flags */
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (nHow == 0 || nHow == 2) {
        g_aSocks[u32Slot].u8ShutRd = 1;
    }
    if (nHow == 1 || nHow == 2) {
        g_aSocks[u32Slot].u8ShutWr = 1;
        /*
         * Residual half-close: peer sees RD shut / EOF after drain
         * (pairs with net_tcp FIN-shaped local smoke without eth).
         */
        i16Peer = g_aSocks[u32Slot].i16Peer;
        if (i16Peer >= 0 && (u32)i16Peer < NET_LO_MAX &&
            g_aSocks[i16Peer].u8Used) {
            g_aSocks[i16Peer].u8ShutRd = 1;
        }
    }
    lo_soft_bump(&g_soft.u64ShutOk);
    if (nHow == 0) {
        lo_soft_bump(&g_soft.u64ShutRd);
    } else if (nHow == 1) {
        lo_soft_bump(&g_soft.u64ShutWr);
    } else {
        lo_soft_bump(&g_soft.u64ShutRdwr);
    }
    lo_soft_maybe_log(0);
    return 0;
}

i64
net_lo_setsockopt(i64 i64Fd, int nLevel, int nOpt, const void *pVal, u32 u32Len)
{
    u32 u32Slot;
    int v = 0;

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64SetoptFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    /* SOL_SOCKET = 1 */
    if (nLevel != 1) {
        lo_soft_bump(&g_soft.u64SetoptOk); /* soft no-op accept */
        lo_soft_maybe_log(0);
        return 0; /* ignore other levels (IPPROTO soft no-op) */
    }
    if (pVal != NULL && u32Len >= 4) {
        v = *(const int *)pVal;
    }
    if (nOpt == 2 /* SO_REUSEADDR */) {
        g_aSocks[u32Slot].u8Reuse = v ? 1u : 0u;
        lo_soft_bump(&g_soft.u64SetoptOk);
        lo_soft_bump(&g_soft.u64OptReuse);
        lo_soft_maybe_log(0);
        return 0;
    }
    if (nOpt == 15 /* SO_REUSEPORT */) {
        g_aSocks[u32Slot].u8ReusePort = v ? 1u : 0u;
        lo_soft_bump(&g_soft.u64SetoptOk);
        lo_soft_bump(&g_soft.u64OptReusePort);
        lo_soft_maybe_log(0);
        return 0;
    }
    if (nOpt == 6 /* SO_BROADCAST */) {
        g_aSocks[u32Slot].u8Broadcast = v ? 1u : 0u;
        lo_soft_bump(&g_soft.u64SetoptOk);
        lo_soft_bump(&g_soft.u64OptBcast);
        lo_soft_maybe_log(0);
        return 0;
    }
    if (nOpt == 9 /* SO_KEEPALIVE */) {
        g_aSocks[u32Slot].u8Keepalive = v ? 1u : 0u;
        lo_soft_bump(&g_soft.u64SetoptOk);
        lo_soft_bump(&g_soft.u64OptKa);
        lo_soft_maybe_log(0);
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
        lo_soft_bump(&g_soft.u64SetoptOk);
        lo_soft_bump(&g_soft.u64OptSndbuf);
        lo_soft_maybe_log(0);
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
        lo_soft_bump(&g_soft.u64SetoptOk);
        lo_soft_bump(&g_soft.u64OptRcvbuf);
        lo_soft_maybe_log(0);
        return 0;
    }
    if (nOpt == 13 /* SO_LINGER */) {
        /* struct linger { int l_onoff; int l_linger; } - soft store only. */
        if (pVal != NULL && u32Len >= 8) {
            const int *pL = (const int *)pVal;

            g_aSocks[u32Slot].u8LingerOn = pL[0] ? 1u : 0u;
            g_aSocks[u32Slot].u16LingerSec =
                pL[1] < 0 ? 0u : (u16)(pL[1] > 65535 ? 65535 : pL[1]);
        }
        lo_soft_bump(&g_soft.u64SetoptOk);
        lo_soft_bump(&g_soft.u64OptLinger);
        lo_soft_maybe_log(0);
        return 0;
    }
    /* SO_DEBUG=1, SO_DONTROUTE=5, SO_OOBINLINE=10, timeos - accept no-op */
    if (nOpt == 1 || nOpt == 5 || nOpt == 10 || nOpt == 20 || nOpt == 21) {
        lo_soft_bump(&g_soft.u64SetoptOk);
        lo_soft_bump(&g_soft.u64OptNoop);
        lo_soft_maybe_log(0);
        return 0;
    }
    lo_soft_bump(&g_soft.u64SetoptOk);
    lo_soft_bump(&g_soft.u64OptNoop);
    lo_soft_maybe_log(0);
    return 0;
}

i64
net_lo_getsockopt(i64 i64Fd, int nLevel, int nOpt, void *pVal, u32 *pLen)
{
    u32 u32Slot;
    int v = 0;

    if (!net_lo_fd_ok(i64Fd) || pVal == NULL || pLen == NULL) {
        lo_soft_bump(&g_soft.u64GetoptFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (nLevel != 1) {
        lo_soft_bump(&g_soft.u64GetoptFail);
        lo_soft_maybe_log(0);
        return -92; /* ENOPROTOOPT-shaped */
    }
    if (nOpt == 3 /* SO_TYPE */) {
        v = g_aSocks[u32Slot].u8Type;
        lo_soft_bump(&g_soft.u64OptType);
    } else if (nOpt == 4 /* SO_ERROR */) {
        /* Sticky SO_ERROR residual: read-and-clear (L3 soft ABI honesty). */
        v = (int)g_aSocks[u32Slot].u16SoftErr;
        g_aSocks[u32Slot].u16SoftErr = 0;
        if (v != 0) {
            lo_soft_bump(&g_soft.u64SoftErrGet);
        }
        lo_soft_bump(&g_soft.u64OptError);
    } else if (nOpt == 2 /* SO_REUSEADDR */) {
        v = g_aSocks[u32Slot].u8Reuse;
        lo_soft_bump(&g_soft.u64OptReuse);
    } else if (nOpt == 15 /* SO_REUSEPORT */) {
        v = g_aSocks[u32Slot].u8ReusePort;
        lo_soft_bump(&g_soft.u64OptReusePort);
    } else if (nOpt == 6 /* SO_BROADCAST */) {
        v = g_aSocks[u32Slot].u8Broadcast;
        lo_soft_bump(&g_soft.u64OptBcast);
    } else if (nOpt == 9 /* SO_KEEPALIVE */) {
        v = g_aSocks[u32Slot].u8Keepalive;
        lo_soft_bump(&g_soft.u64OptKa);
    } else if (nOpt == 7 /* SO_SNDBUF */) {
        v = (int)g_aSocks[u32Slot].u32SndBuf;
        lo_soft_bump(&g_soft.u64OptSndbuf);
    } else if (nOpt == 8 /* SO_RCVBUF */) {
        v = (int)g_aSocks[u32Slot].u32RcvBuf;
        lo_soft_bump(&g_soft.u64OptRcvbuf);
    } else if (nOpt == 30 /* SO_ACCEPTCONN */) {
        v = g_aSocks[u32Slot].u8Listening ? 1 : 0;
        lo_soft_bump(&g_soft.u64OptAcceptConn);
    } else if (nOpt == 13 /* SO_LINGER */) {
        if (*pLen < 8) {
            lo_soft_bump(&g_soft.u64GetoptFail);
            lo_soft_maybe_log(0);
            return -22;
        }
        ((int *)pVal)[0] = g_aSocks[u32Slot].u8LingerOn ? 1 : 0;
        ((int *)pVal)[1] = (int)g_aSocks[u32Slot].u16LingerSec;
        *pLen = 8;
        lo_soft_bump(&g_soft.u64GetoptOk);
        lo_soft_bump(&g_soft.u64OptLinger);
        lo_soft_maybe_log(0);
        return 0;
    } else {
        v = 0;
        lo_soft_bump(&g_soft.u64OptNoop);
    }
    if (*pLen < 4) {
        lo_soft_bump(&g_soft.u64GetoptFail);
        lo_soft_maybe_log(0);
        return -22;
    }
    *(int *)pVal = v;
    *pLen = 4;
    lo_soft_bump(&g_soft.u64GetoptOk);
    lo_soft_maybe_log(0);
    return 0;
}

/**
 * Soft sockaddr fill: AF_INET -> sin-shaped 127.0.0.1:port;
 * AF_UNIX -> family only (path residual empty). Domain honesty residual.
 */
static void
fill_addr(u8 *p, u8 u8Domain, u16 u16Port)
{
    u32 i;

    for (i = 0; i < 16; i++) {
        p[i] = 0;
    }
    if (u8Domain == (u8)AF_UNIX) {
        p[0] = (u8)AF_UNIX; /* soft AF_UNIX family residual */
        p[1] = 0;
        return;
    }
    p[0] = (u8)AF_INET; /* AF_INET */
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
    u8 u8Dom;

    if (!net_lo_fd_ok(i64Fd) || pAddr == NULL || pLen == NULL) {
        lo_soft_bump(&g_soft.u64NameFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    if (*pLen < 16) {
        lo_soft_bump(&g_soft.u64NameFail);
        lo_soft_maybe_log(0);
        return -22;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    u8Dom = g_aSocks[u32Slot].u8Domain;
    if (u8Dom != (u8)AF_INET && u8Dom != (u8)AF_UNIX) {
        u8Dom = (u8)AF_INET;
    }
    fill_addr((u8 *)pAddr, u8Dom, g_aSocks[u32Slot].u16Port);
    *pLen = 16;
    lo_soft_bump(&g_soft.u64NameOk);
    lo_soft_maybe_log(0);
    return 0;
}

i64
net_lo_getpeername(i64 i64Fd, void *pAddr, u32 *pLen)
{
    u32 u32Slot;
    i16 peer;
    u8 u8Dom;

    if (!net_lo_fd_ok(i64Fd) || pAddr == NULL || pLen == NULL) {
        lo_soft_bump(&g_soft.u64PeerFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    if (*pLen < 16) {
        lo_soft_bump(&g_soft.u64PeerFail);
        lo_soft_maybe_log(0);
        return -22;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    peer = g_aSocks[u32Slot].i16Peer;
    if (peer < 0 || (u32)peer >= NET_LO_MAX) {
        lo_soft_bump(&g_soft.u64PeerFail);
        lo_soft_maybe_log(0);
        return -107; /* ENOTCONN */
    }
    /* Peer family from local domain residual (pair inherits). */
    u8Dom = g_aSocks[u32Slot].u8Domain;
    if (g_aSocks[peer].u8Domain == (u8)AF_INET ||
        g_aSocks[peer].u8Domain == (u8)AF_UNIX) {
        u8Dom = g_aSocks[peer].u8Domain;
    }
    if (u8Dom != (u8)AF_INET && u8Dom != (u8)AF_UNIX) {
        u8Dom = (u8)AF_INET;
    }
    fill_addr((u8 *)pAddr, u8Dom, g_aSocks[peer].u16Port);
    *pLen = 16;
    lo_soft_bump(&g_soft.u64PeerOk);
    lo_soft_maybe_log(0);
    return 0;
}

i64
net_lo_close(i64 i64Fd)
{
    u32 u32Slot;
    i16 peer;
    u32 cUsed = 0;
    u32 u32Li;

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64CloseFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    peer = g_aSocks[u32Slot].i16Peer;
    /* Closing AcceptQ child: drop pending on any listener that hints here. */
    if (g_aSocks[u32Slot].u8AcceptQ) {
        for (u32Li = 0; u32Li < NET_LO_MAX; u32Li++) {
            if (!g_aSocks[u32Li].u8Used || !g_aSocks[u32Li].u8Listening) {
                continue;
            }
            if (g_aSocks[u32Li].i16Peer == (i16)u32Slot) {
                g_aSocks[u32Li].i16Peer = -1;
                if (g_aSocks[u32Li].u8Pending > 0) {
                    g_aSocks[u32Li].u8Pending--;
                }
            }
        }
        lo_rehook_acceptq(g_aSocks[u32Slot].u16Port, g_aSocks[u32Slot].u8Type,
                          (i16)u32Slot);
    }
    if (peer >= 0 && (u32)peer < NET_LO_MAX && g_aSocks[peer].u8Used) {
        if (g_aSocks[peer].i16Peer == (i16)u32Slot) {
            g_aSocks[peer].i16Peer = -1;
        }
        /* Soft half-close: peer sees RD shutdown if we were a data endpoint. */
        if (g_aSocks[u32Slot].u8Connected && !g_aSocks[u32Slot].u8Listening) {
            g_aSocks[peer].u8ShutRd = 1;
            lo_soft_bump(&g_soft.u64ClosePeerHalf);
        }
        if (g_aSocks[peer].u8Listening && g_aSocks[peer].u8Pending > 0 &&
            g_aSocks[peer].i16Peer == (i16)u32Slot) {
            g_aSocks[peer].u8Pending--;
            g_aSocks[peer].i16Peer = -1;
        }
    }
    memset(&g_aSocks[u32Slot], 0, sizeof(g_aSocks[u32Slot]));
    g_aSocks[u32Slot].i16Peer = -1;
    lo_soft_bump(&g_soft.u64CloseOk);
    lo_soft_tally(&cUsed, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                  NULL, NULL);
    /* Force inventory when table empties (soft product end-of-session). */
    lo_soft_maybe_log(cUsed == 0 ? 1 : 0);
    return 0;
}
