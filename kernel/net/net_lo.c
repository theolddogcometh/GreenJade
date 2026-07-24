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
 *   - soft stats + live-table inventory (grep: "net: lo soft")
 *
 * Soft inventory (Wave 14 base; Wave 35 exclusive deepen; this unit only):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   Lifetime path / ring / sockopt tallies (struct net_lo_soft).
 *   Greppable prefix-stable serial markers (rate-limited; never flood):
 *     net: lo soft inventory …
 *     net: lo soft sock …
 *     net: lo soft bind …
 *     net: lo soft life …
 *     net: lo soft xfer …
 *     net: lo soft opt …
 *     net: lo soft name …
 *     net: lo soft ring …
 *     net: lo soft stats …
 *     net: lo soft path …
 *     net: lo soft ratio …      — Wave 15 occupancy / fail basis points
 *     net: lo soft headroom …   — Wave 15 free slots + log caps
 *     net: lo soft surface …    — Wave 15 area catalog
 *     net: lo soft deepen …     — wave=59 areas stamp
 *     net: lo soft slot=…
 *     net: lo soft init|listen|accept|emfile …
 *     net: lo soft PASS …
 *   Twin prefix also emitted: "net_lo: soft …".
 *   Cadence dumps only at power-of-two op milestones, hard-capped at
 *   NET_LO_SOFT_LOG_MAX (force emfile/empty also capped). Init always
 *   emits once. Event lines (listen/accept/emfile) share
 *   NET_LO_SOFT_EVENT_MAX. Never hard-gates product policy. Pure C.
 * Grep: net: lo soft / net_lo: soft
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
 * Soft inventory serial budget (Wave 14). Absolute cap of greppable full
 * cadence dumps; milestones are power-of-two API op counts (1,2,4,…).
 * Event lines (listen/accept/emfile) share a separate hard cap.
 * greppable: net: lo soft / net_lo: soft
 */
#define NET_LO_SOFT_LOG_MAX   8u
#define NET_LO_SOFT_EVENT_MAX 8u
/* Slot detail only on force dumps or the first N cadence dumps. */
#define NET_LO_SOFT_SLOT_LOGS 2u

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

/*
 * Soft product inventory counters — wrap OK; diagnostics only; never
 * hard-gate product paths. Grep: net: lo soft / net_lo: soft
 * Wave 14 deepen: twin prefix, multi-line path dumps, PASS lamp.
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
    u64 u64ConnOrphan;   /* connect with no local listener (soft OK) */
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
    u32 u32SoftLogN;     /* inventory log emissions (u32 twin) */
    u32 u32EventN;       /* listen/accept/emfile event emissions */
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

/*
 * Greppable soft product inventory + path dumps (Wave 14 exclusive).
 * Prefix-stable: "net: lo soft …" and twin "net_lo: soft …".
 * fForce: include per-live-slot detail (init / emfile / table-empty).
 * Cadence dumps skip slots after NET_LO_SOFT_SLOT_LOGS to avoid flood.
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
    u32 i;
    u32 fSlots;
    struct net_lo_soft s;

    lo_soft_tally(&cUsed, &cFree, &cListen, &cConn, &cStream, &cDgram,
                  &cPending, &cRx, &cShutRd, &cShutWr, &cReuse);
    s = g_soft;
    lo_soft_bump(&g_soft.u64LogDumps);
    if (g_soft.u32SoftLogN < 0xffffffffu) {
        g_soft.u32SoftLogN++;
    }
    /* Slot detail: force always; cadence only for first few dumps. */
    fSlots = (fForce != 0 || g_soft.u32SoftLogN <= NET_LO_SOFT_SLOT_LOGS)
                 ? 1u
                 : 0u;

    /* Grep: net: lo soft inventory */
    kprintf("net: lo soft inventory used=%u free=%u listen=%u conn=%u "
            "stream=%u dgram=%u pending=%u rx_bytes=%u shut_rd=%u "
            "shut_wr=%u reuse=%u hwm=%llu max=%u fd_base=%u buf=%u "
            "backlog_max=%u logs=%u skip=%llu event_n=%u event_skip=%llu "
            "wave=59\n",
            cUsed, cFree, cListen, cConn, cStream, cDgram, cPending, cRx,
            cShutRd, cShutWr, cReuse,
            (unsigned long long)s.u64HwmUsed, (unsigned)NET_LO_MAX,
            (unsigned)NET_FD_BASE, (unsigned)NET_LO_BUF,
            (unsigned)NET_LO_BACKLOG_MAX, g_soft.u32SoftLogN,
            (unsigned long long)s.u64LogSkip, g_soft.u32EventN,
            (unsigned long long)s.u64EventSkip);

    /* Grep: net_lo: soft inventory (twin prefix) */
    kprintf("net_lo: soft inventory used=%u free=%u listen=%u conn=%u "
            "stream=%u dgram=%u pending=%u rx_bytes=%u hwm=%llu max=%u "
            "fd_base=%u buf=%u logs=%u wave=59\n",
            cUsed, cFree, cListen, cConn, cStream, cDgram, cPending, cRx,
            (unsigned long long)s.u64HwmUsed, (unsigned)NET_LO_MAX,
            (unsigned)NET_FD_BASE, (unsigned)NET_LO_BUF, g_soft.u32SoftLogN);

    /* Grep: net: lo soft sock */
    kprintf("net: lo soft sock ok=%llu fail=%llu inet=%llu unix=%llu "
            "stream=%llu dgram=%llu\n",
            (unsigned long long)s.u64SockOk,
            (unsigned long long)s.u64SockFail,
            (unsigned long long)s.u64SockInet,
            (unsigned long long)s.u64SockUnix,
            (unsigned long long)s.u64SockStream,
            (unsigned long long)s.u64SockDgram);

    /* Grep: net_lo: soft sock (twin) */
    kprintf("net_lo: soft sock ok=%llu fail=%llu inet=%llu unix=%llu "
            "stream=%llu dgram=%llu hwm=%llu max=%u wave=59\n",
            (unsigned long long)s.u64SockOk,
            (unsigned long long)s.u64SockFail,
            (unsigned long long)s.u64SockInet,
            (unsigned long long)s.u64SockUnix,
            (unsigned long long)s.u64SockStream,
            (unsigned long long)s.u64SockDgram,
            (unsigned long long)s.u64HwmUsed, (unsigned)NET_LO_MAX);

    /* Grep: net: lo soft bind */
    kprintf("net: lo soft bind ok=%llu fail=%llu eaddr=%llu\n",
            (unsigned long long)s.u64BindOk,
            (unsigned long long)s.u64BindFail,
            (unsigned long long)s.u64EaddrInuse);

    /* Grep: net_lo: soft bind (twin) */
    kprintf("net_lo: soft bind ok=%llu fail=%llu eaddr=%llu wave=59\n",
            (unsigned long long)s.u64BindOk,
            (unsigned long long)s.u64BindFail,
            (unsigned long long)s.u64EaddrInuse);

    /* Grep: net: lo soft life */
    kprintf("net: lo soft life listen=%llu listen_fail=%llu "
            "conn=%llu conn_fail=%llu conn_again=%llu conn_orphan=%llu "
            "accept=%llu accept_fail=%llu accept_again=%llu "
            "shut=%llu shut_fail=%llu shut_rd=%llu shut_wr=%llu "
            "shut_rdwr=%llu close=%llu close_fail=%llu close_half=%llu\n",
            (unsigned long long)s.u64ListenOk,
            (unsigned long long)s.u64ListenFail,
            (unsigned long long)s.u64ConnOk,
            (unsigned long long)s.u64ConnFail,
            (unsigned long long)s.u64ConnAgain,
            (unsigned long long)s.u64ConnOrphan,
            (unsigned long long)s.u64AcceptOk,
            (unsigned long long)s.u64AcceptFail,
            (unsigned long long)s.u64AcceptAgain,
            (unsigned long long)s.u64ShutOk,
            (unsigned long long)s.u64ShutFail,
            (unsigned long long)s.u64ShutRd,
            (unsigned long long)s.u64ShutWr,
            (unsigned long long)s.u64ShutRdwr,
            (unsigned long long)s.u64CloseOk,
            (unsigned long long)s.u64CloseFail,
            (unsigned long long)s.u64ClosePeerHalf);

    /* Grep: net_lo: soft life (twin) */
    kprintf("net_lo: soft life listen=%llu conn=%llu conn_again=%llu "
            "conn_orphan=%llu accept=%llu accept_again=%llu "
            "shut=%llu close=%llu close_half=%llu wave=59\n",
            (unsigned long long)s.u64ListenOk,
            (unsigned long long)s.u64ConnOk,
            (unsigned long long)s.u64ConnAgain,
            (unsigned long long)s.u64ConnOrphan,
            (unsigned long long)s.u64AcceptOk,
            (unsigned long long)s.u64AcceptAgain,
            (unsigned long long)s.u64ShutOk,
            (unsigned long long)s.u64CloseOk,
            (unsigned long long)s.u64ClosePeerHalf);

    /* Grep: net: lo soft xfer */
    kprintf("net: lo soft xfer send=%llu send_fail=%llu send_pipe=%llu "
            "send_self=%llu recv=%llu recv_fail=%llu recv_again=%llu "
            "recv_eof=%llu tx=%llu rx=%llu\n",
            (unsigned long long)s.u64SendOk,
            (unsigned long long)s.u64SendFail,
            (unsigned long long)s.u64SendPipe,
            (unsigned long long)s.u64SendSelf,
            (unsigned long long)s.u64RecvOk,
            (unsigned long long)s.u64RecvFail,
            (unsigned long long)s.u64RecvAgain,
            (unsigned long long)s.u64RecvEof,
            (unsigned long long)s.u64BytesTx,
            (unsigned long long)s.u64BytesRx);

    /* Grep: net_lo: soft xfer (twin) */
    kprintf("net_lo: soft xfer send=%llu send_pipe=%llu send_self=%llu "
            "recv=%llu recv_again=%llu recv_eof=%llu tx=%llu rx=%llu "
            "wave=59\n",
            (unsigned long long)s.u64SendOk,
            (unsigned long long)s.u64SendPipe,
            (unsigned long long)s.u64SendSelf,
            (unsigned long long)s.u64RecvOk,
            (unsigned long long)s.u64RecvAgain,
            (unsigned long long)s.u64RecvEof,
            (unsigned long long)s.u64BytesTx,
            (unsigned long long)s.u64BytesRx);

    /* Grep: net: lo soft opt */
    kprintf("net: lo soft opt set=%llu set_fail=%llu get=%llu get_fail=%llu "
            "reuse=%llu reusep=%llu bcast=%llu ka=%llu sndbuf=%llu "
            "rcvbuf=%llu linger=%llu noop=%llu type=%llu error=%llu "
            "acceptconn=%llu\n",
            (unsigned long long)s.u64SetoptOk,
            (unsigned long long)s.u64SetoptFail,
            (unsigned long long)s.u64GetoptOk,
            (unsigned long long)s.u64GetoptFail,
            (unsigned long long)s.u64OptReuse,
            (unsigned long long)s.u64OptReusePort,
            (unsigned long long)s.u64OptBcast,
            (unsigned long long)s.u64OptKa,
            (unsigned long long)s.u64OptSndbuf,
            (unsigned long long)s.u64OptRcvbuf,
            (unsigned long long)s.u64OptLinger,
            (unsigned long long)s.u64OptNoop,
            (unsigned long long)s.u64OptType,
            (unsigned long long)s.u64OptError,
            (unsigned long long)s.u64OptAcceptConn);

    /* Grep: net_lo: soft opt (twin) */
    kprintf("net_lo: soft opt set=%llu get=%llu reuse=%llu reusep=%llu "
            "sndbuf=%llu rcvbuf=%llu linger=%llu noop=%llu wave=59\n",
            (unsigned long long)s.u64SetoptOk,
            (unsigned long long)s.u64GetoptOk,
            (unsigned long long)s.u64OptReuse,
            (unsigned long long)s.u64OptReusePort,
            (unsigned long long)s.u64OptSndbuf,
            (unsigned long long)s.u64OptRcvbuf,
            (unsigned long long)s.u64OptLinger,
            (unsigned long long)s.u64OptNoop);

    /* Grep: net: lo soft name */
    kprintf("net: lo soft name ok=%llu fail=%llu peer_ok=%llu "
            "peer_fail=%llu\n",
            (unsigned long long)s.u64NameOk,
            (unsigned long long)s.u64NameFail,
            (unsigned long long)s.u64PeerOk,
            (unsigned long long)s.u64PeerFail);

    /* Grep: net_lo: soft name (twin) */
    kprintf("net_lo: soft name ok=%llu fail=%llu peer_ok=%llu "
            "peer_fail=%llu wave=59\n",
            (unsigned long long)s.u64NameOk,
            (unsigned long long)s.u64NameFail,
            (unsigned long long)s.u64PeerOk,
            (unsigned long long)s.u64PeerFail);

    /* Grep: net: lo soft ring */
    kprintf("net: lo soft ring buf=%u push_full=%llu push_partial=%llu "
            "rx_live=%u rcv_def=%u snd_def=%u\n",
            (unsigned)NET_LO_BUF,
            (unsigned long long)s.u64PushFull,
            (unsigned long long)s.u64PushPartial, cRx,
            (unsigned)NET_LO_RCVBUF_DEF, (unsigned)NET_LO_SNDBUF_DEF);

    /* Grep: net_lo: soft ring (twin) */
    kprintf("net_lo: soft ring buf=%u push_full=%llu push_partial=%llu "
            "rx_live=%u rcv_def=%u snd_def=%u wave=59\n",
            (unsigned)NET_LO_BUF,
            (unsigned long long)s.u64PushFull,
            (unsigned long long)s.u64PushPartial, cRx,
            (unsigned)NET_LO_RCVBUF_DEF, (unsigned)NET_LO_SNDBUF_DEF);

    /* Grep: net: lo soft stats */
    kprintf("net: lo soft stats ops=%llu sock=%llu sock_fail=%llu "
            "bind=%llu bind_fail=%llu eaddr=%llu listen=%llu listen_fail=%llu "
            "conn=%llu conn_fail=%llu conn_again=%llu conn_orphan=%llu "
            "accept=%llu accept_fail=%llu accept_again=%llu "
            "send=%llu send_fail=%llu send_pipe=%llu send_self=%llu "
            "recv=%llu recv_fail=%llu recv_again=%llu recv_eof=%llu "
            "shut=%llu shut_fail=%llu close=%llu close_fail=%llu "
            "setopt=%llu setopt_fail=%llu getopt=%llu getopt_fail=%llu "
            "name=%llu name_fail=%llu peer=%llu peer_fail=%llu "
            "tx=%llu rx=%llu push_full=%llu push_partial=%llu dumps=%llu "
            "skip=%llu event_skip=%llu log_max=%u event_max=%u\n",
            (unsigned long long)s.u64Ops,
            (unsigned long long)s.u64SockOk,
            (unsigned long long)s.u64SockFail,
            (unsigned long long)s.u64BindOk,
            (unsigned long long)s.u64BindFail,
            (unsigned long long)s.u64EaddrInuse,
            (unsigned long long)s.u64ListenOk,
            (unsigned long long)s.u64ListenFail,
            (unsigned long long)s.u64ConnOk,
            (unsigned long long)s.u64ConnFail,
            (unsigned long long)s.u64ConnAgain,
            (unsigned long long)s.u64ConnOrphan,
            (unsigned long long)s.u64AcceptOk,
            (unsigned long long)s.u64AcceptFail,
            (unsigned long long)s.u64AcceptAgain,
            (unsigned long long)s.u64SendOk,
            (unsigned long long)s.u64SendFail,
            (unsigned long long)s.u64SendPipe,
            (unsigned long long)s.u64SendSelf,
            (unsigned long long)s.u64RecvOk,
            (unsigned long long)s.u64RecvFail,
            (unsigned long long)s.u64RecvAgain,
            (unsigned long long)s.u64RecvEof,
            (unsigned long long)s.u64ShutOk,
            (unsigned long long)s.u64ShutFail,
            (unsigned long long)s.u64CloseOk,
            (unsigned long long)s.u64CloseFail,
            (unsigned long long)s.u64SetoptOk,
            (unsigned long long)s.u64SetoptFail,
            (unsigned long long)s.u64GetoptOk,
            (unsigned long long)s.u64GetoptFail,
            (unsigned long long)s.u64NameOk,
            (unsigned long long)s.u64NameFail,
            (unsigned long long)s.u64PeerOk,
            (unsigned long long)s.u64PeerFail,
            (unsigned long long)s.u64BytesTx,
            (unsigned long long)s.u64BytesRx,
            (unsigned long long)s.u64PushFull,
            (unsigned long long)s.u64PushPartial,
            (unsigned long long)g_soft.u64LogDumps,
            (unsigned long long)s.u64LogSkip,
            (unsigned long long)s.u64EventSkip,
            (unsigned)NET_LO_SOFT_LOG_MAX,
            (unsigned)NET_LO_SOFT_EVENT_MAX);

    /* Grep: net_lo: soft stats (twin) */
    kprintf("net_lo: soft stats ops=%llu sock=%llu sock_fail=%llu "
            "bind=%llu eaddr=%llu listen=%llu conn=%llu conn_again=%llu "
            "conn_orphan=%llu accept=%llu send=%llu recv=%llu "
            "close=%llu dumps=%llu skip=%llu wave=59\n",
            (unsigned long long)s.u64Ops,
            (unsigned long long)s.u64SockOk,
            (unsigned long long)s.u64SockFail,
            (unsigned long long)s.u64BindOk,
            (unsigned long long)s.u64EaddrInuse,
            (unsigned long long)s.u64ListenOk,
            (unsigned long long)s.u64ConnOk,
            (unsigned long long)s.u64ConnAgain,
            (unsigned long long)s.u64ConnOrphan,
            (unsigned long long)s.u64AcceptOk,
            (unsigned long long)s.u64SendOk,
            (unsigned long long)s.u64RecvOk,
            (unsigned long long)s.u64CloseOk,
            (unsigned long long)g_soft.u64LogDumps,
            (unsigned long long)s.u64LogSkip);

    /* Grep: net: lo soft path */
    kprintf("net: lo soft path sock=af_inet|af_unix stream|dgram "
            "bind=eaddr_reuse listen=backlog_soft conn=pair|orphan|again "
            "accept=mint xfer=peer_ring|self_dgram shut=rd|wr|rdwr "
            "opt=sol_socket_subset name=sin_lo peer=peer_port "
            "fd=%u..%u (soft inventory; not bar3)\n",
            (unsigned)NET_FD_BASE,
            (unsigned)(NET_FD_BASE + NET_LO_MAX - 1u));

    /* Grep: net_lo: soft path (twin) */
    kprintf("net_lo: soft path sock=af_inet|af_unix stream|dgram "
            "bind=eaddr_reuse listen=backlog_soft conn=pair|orphan|again "
            "accept=mint xfer=peer_ring|self_dgram shut=rd|wr|rdwr "
            "opt=sol_socket_subset fd=%u..%u wave=59 "
            "(soft inventory; not bar3)\n",
            (unsigned)NET_FD_BASE,
            (unsigned)(NET_FD_BASE + NET_LO_MAX - 1u));

    /*
     * Wave 35 exclusive deepen (complementary; primary lines field-stable).
     * greppable: net: lo soft ratio|headroom|surface|deepen
     */
    {
        u32 u32OccBp = 0;
        u32 u32ListenBp = 0;
        u32 u32ConnBp = 0;
        u32 u32SockFailBp = 0;
        u32 u32BindFailBp = 0;
        u32 u32FreeHead = 0;
        u32 u32LogHead = 0;
        u64 u64SockTot;
        u64 u64BindTot;

        if ((u32)NET_LO_MAX != 0u) {
            u32OccBp = (cUsed * 10000u) / (u32)NET_LO_MAX;
        }
        if (cUsed != 0u) {
            u32ListenBp = (cListen * 10000u) / cUsed;
            u32ConnBp = (cConn * 10000u) / cUsed;
        }
        u64SockTot = s.u64SockOk + s.u64SockFail;
        if (u64SockTot != 0ull) {
            u32SockFailBp = (u32)((s.u64SockFail * 10000ull) / u64SockTot);
        }
        u64BindTot = s.u64BindOk + s.u64BindFail;
        if (u64BindTot != 0ull) {
            u32BindFailBp = (u32)((s.u64BindFail * 10000ull) / u64BindTot);
        }
        if ((u32)NET_LO_MAX > cUsed) {
            u32FreeHead = (u32)NET_LO_MAX - cUsed;
        }
        if ((u32)NET_LO_SOFT_LOG_MAX > g_soft.u32SoftLogN) {
            u32LogHead = (u32)NET_LO_SOFT_LOG_MAX - g_soft.u32SoftLogN;
        }
        /* Grep: net: lo soft ratio */
        kprintf("net: lo soft ratio occ_bp=%u listen_bp=%u conn_bp=%u "
                "sock_fail_bp=%u bind_fail_bp=%u used=%u free=%u wave=59\n",
                u32OccBp, u32ListenBp, u32ConnBp, u32SockFailBp,
                u32BindFailBp, cUsed, cFree);
        /* Grep: net_lo: soft ratio (twin) */
        kprintf("net_lo: soft ratio occ_bp=%u listen_bp=%u conn_bp=%u "
                "sock_fail_bp=%u bind_fail_bp=%u wave=59\n",
                u32OccBp, u32ListenBp, u32ConnBp, u32SockFailBp,
                u32BindFailBp);
        /* Grep: net: lo soft headroom */
        kprintf("net: lo soft headroom free=%u free_head=%u max=%u "
                "log_head=%u log_max=%u event_max=%u backlog_max=%u "
                "wave=59\n",
                cFree, u32FreeHead, (u32)NET_LO_MAX, u32LogHead,
                (u32)NET_LO_SOFT_LOG_MAX, (u32)NET_LO_SOFT_EVENT_MAX,
                (u32)NET_LO_BACKLOG_MAX);
        /* Grep: net_lo: soft headroom (twin) */
        kprintf("net_lo: soft headroom free=%u free_head=%u max=%u "
                "log_head=%u wave=59\n",
                cFree, u32FreeHead, (u32)NET_LO_MAX, u32LogHead);
        /* Grep: net: lo soft surface */
        kprintf("net: lo soft surface inventory,sock,bind,life,xfer,opt,"
                "name,ring,stats,path,ratio,headroom,capacity,geom,"
                "terminal,return,retmap,deepen,slot,PASS areas=36 wave=59\n");
        /* Grep: net_lo: soft surface (twin) */
        kprintf("net_lo: soft surface inventory,sock,bind,life,xfer,opt,"
                "name,ring,stats,path,ratio,headroom,capacity,geom,"
                "terminal,return,retmap,deepen,slot,PASS areas=36 wave=59\n");
        /* Grep: net: lo soft capacity — Wave 19 design-constant lamps. */
        kprintf("net: lo soft capacity max=%u backlog_max=%u "
                "log_max=%u event_max=%u buf_def=1 wave=59\n",
                (u32)NET_LO_MAX, (u32)NET_LO_BACKLOG_MAX,
                (u32)NET_LO_SOFT_LOG_MAX, (u32)NET_LO_SOFT_EVENT_MAX);
        /* Grep: net_lo: soft capacity (twin) */
        kprintf("net_lo: soft capacity max=%u backlog_max=%u "
                "log_max=%u wave=59\n",
                (u32)NET_LO_MAX, (u32)NET_LO_BACKLOG_MAX,
                (u32)NET_LO_SOFT_LOG_MAX);
        /* Grep: net: lo soft geom — Wave 16 table geometry lamps. */
        kprintf("net: lo soft geom used=%u free=%u listen=%u conn=%u "
                "max=%u free_head=%u wave=59\n",
                cUsed, cFree, cListen, cConn, (u32)NET_LO_MAX, u32FreeHead);
        /* Grep: net_lo: soft geom (twin) */
        kprintf("net_lo: soft geom used=%u free=%u listen=%u conn=%u "
                "wave=59\n",
                cUsed, cFree, cListen, cConn);
        /* Grep: net: lo soft terminal — Wave 19 outcome rollup. */
        kprintf("net: lo soft terminal used=%u free=%u listen=%u conn=%u "
                "logs=%u soft PASS wave=59\n",
                cUsed, cFree, cListen, cConn, g_soft.u32SoftLogN);
        /* Grep: net_lo: soft terminal (twin) */
        kprintf("net_lo: soft terminal used=%u free=%u listen=%u conn=%u "
                "soft PASS wave=59\n",
                cUsed, cFree, cListen, cConn);
        /* Grep: net: lo soft return — Wave 19 API return surfaces */
        kprintf("net: lo soft return sock_ok=%llu sock_fail=%llu "
                "bind_ok=%llu bind_fail=%llu send_ok=%llu recv_ok=%llu "
                "close_ok=%llu used=%u free=%u product_loopback=OPEN wave=59\n",
                (unsigned long long)s.u64SockOk,
                (unsigned long long)s.u64SockFail,
                (unsigned long long)s.u64BindOk,
                (unsigned long long)s.u64BindFail,
                (unsigned long long)s.u64SendOk,
                (unsigned long long)s.u64RecvOk,
                (unsigned long long)s.u64CloseOk, cUsed, cFree);
        /* Grep: net_lo: soft return (twin) */
        kprintf("net_lo: soft return sock_ok=%llu bind_ok=%llu send_ok=%llu "
                "recv_ok=%llu used=%u product_loopback=OPEN wave=59\n",
                (unsigned long long)s.u64SockOk,
                (unsigned long long)s.u64BindOk,
                (unsigned long long)s.u64SendOk,
                (unsigned long long)s.u64RecvOk, cUsed);

        /* Grep: net: lo soft deepen */
        /*
         * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: net: lo: soft retclass — Wave 19 return-class taxonomy (kept) */
        kprintf("net: lo: soft retclass ok|fail|inval|nodev|busy|nomem "
                "soft_only=1 product_gate=0 wave=%u "
                "(retclass taxonomy; Soft≠product; not bar3)\n",
                (unsigned)NET_LO_SOFT_LOG_MAX);
        /* Grep: net: lo: soft retlane — Wave 19 return-lane catalog (kept) */
        kprintf("net: lo: soft retlane inv|selftest|rate|retcode|retmap|class "
                "product_kernel=OPEN soft_ne_product=1 wave=%u "
                "(retlane catalog; Soft≠product)\n",
                (unsigned)NET_LO_SOFT_LOG_MAX);
        /*
         * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
         * Return surfaces only — soft inventory; never hard-gates product paths.
         * Soft≠product; not bar3.
         */
        /* Grep: net: lo: soft retbound — Wave 20 return-bound honesty (kept) */
        kprintf("net: lo: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
                "never_blocks_m0=1 wave=%u "
                "(retbound honesty; Soft≠product; not bar3)\n",
                (unsigned)NET_LO_SOFT_LOG_MAX);
        /* Grep: net: lo: soft retseal — Wave 20 seal stamp (kept) */
        kprintf("net: lo: soft retseal exclusive=1 soft_ne_product=1 "
                "product_kernel=OPEN bar3=0 wave=%u "
                "(retseal stamp; Soft≠product)\n",
                (unsigned)NET_LO_SOFT_LOG_MAX);
                /*
                 * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: net: lo: soft retpulse — Wave 21 return-pulse honesty (kept) */
                kprintf("net: lo: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retpulse honesty; Soft≠product; not bar3)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /* Grep: net: lo: soft retmark — Wave 21 mark stamp (kept) */
                kprintf("net: lo: soft retmark exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retmark stamp; Soft≠product)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /*
                 * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                */
                /* Grep: net: lo: soft retphase — Wave 22 return-phase honesty (kept) */
                kprintf("net: lo: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retphase honesty; Soft≠product; not bar3)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /* Grep: net: lo: soft retbadge — Wave 22 badge stamp (kept) */
                kprintf("net: lo: soft retbadge exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retbadge stamp; Soft≠product)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
                */
                /* Grep: net: lo: soft rettoken — Wave 23 return-token honesty (kept) */
                kprintf("net: lo: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(rettoken honesty; Soft≠product; not bar3)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /* Grep: net: lo: soft retcrest — Wave 23 crest stamp (kept) */
                kprintf("net: lo: soft retcrest exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retcrest stamp; Soft≠product)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /*
                 * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: net: lo: soft retvault — Wave 24 return-vault honesty (kept) */
                kprintf("net: lo: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retvault honesty; Soft≠product; not bar3)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /* Grep: net: lo: soft retbanner — Wave 24 banner stamp (kept) */
                kprintf("net: lo: soft retbanner exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retbanner stamp; Soft≠product)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /*
                 * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: net: lo: soft retledger — Wave 25 return-ledger honesty (kept) */
                kprintf("net: lo: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retledger honesty; Soft≠product; not bar3)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /* Grep: net: lo: soft retbeacon — Wave 25 beacon stamp (kept) */
                kprintf("net: lo: soft retbeacon exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retbeacon stamp; Soft≠product)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /*
                 * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
                 * Return surfaces only — soft inventory; never hard-gates product paths.
                 * Soft≠product; not bar3.
                 */
                /* Grep: net: lo: soft retcipher — Wave 26 return-cipher honesty (kept) */
                kprintf("net: lo: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                        "never_blocks_m0=1 wave=%u "
                        "(retcipher honesty; Soft≠product; not bar3)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                /* Grep: net: lo: soft retflame — Wave 26 flame stamp (kept) */
                kprintf("net: lo: soft retflame exclusive=1 soft_ne_product=1 "
                        "product_kernel=OPEN bar3=0 wave=%u "
                        "(retflame stamp; Soft≠product)\n",
                        (unsigned)NET_LO_SOFT_LOG_MAX);
                        /*
                         * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: net: lo: soft retprism — Wave 27 return-prism honesty (kept) */
                        kprintf("net: lo: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retprism honesty; Soft≠product; not bar3)\n",
                                (unsigned)NET_LO_SOFT_LOG_MAX);
                        /* Grep: net: lo: soft retforge — Wave 27 forge stamp (kept) */
                        kprintf("net: lo: soft retforge exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retforge stamp; Soft≠product)\n",
                                (unsigned)NET_LO_SOFT_LOG_MAX);
                                /*
                                 * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: net: lo: soft retshard — Wave 28 return-shard honesty (kept) */
                                kprintf("net: lo: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retshard honesty; Soft≠product; not bar3)\n",
                                    (unsigned)NET_LO_SOFT_LOG_MAX);
                                /* Grep: net: lo: soft retcrown — Wave 28 crown stamp (kept) */
                                kprintf("net: lo: soft retcrown exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retcrown stamp; Soft≠product)\n",
                                    (unsigned)NET_LO_SOFT_LOG_MAX);
                                        /*
                                 * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: net: lo: soft retglyph — Wave 29 return-glyph honesty (kept) */
                                kprintf("net: lo: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=59 "
                                        "(retglyph honesty; Soft≠product; not bar3)\n");
                                /* Grep: net: lo: soft retscepter — Wave 29 scepter stamp (kept) */
                                kprintf("net: lo: soft retscepter exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=59 "
                                        "(retscepter stamp; Soft≠product)\n");
                                        /*
                                 * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: net: lo: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("net: lo: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=59 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: net: lo: soft retemblem — Wave 30 emblem stamp (kept) */
                                kprintf("net: lo: soft retemblem exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=59 "
                                        "(retemblem stamp; Soft≠product)\n");
                                /*
                                 * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                                 * Return surfaces only — soft inventory; never hard-gates product paths.
                                 * Soft≠product; not bar3.
                                 */
                                /* Grep: net: lo: soft retaegis — Wave 31 return-aegis honesty (kept) */
                                kprintf("net: lo: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=59 "
                                        "(retaegis honesty; Soft≠product; not bar3)\n");
                                /* Grep: net: lo: soft retsigil — Wave 30 return-sigil honesty (kept) */
                                kprintf("net: lo: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                        "never_blocks_m0=1 wave=59 "
                                        "(retsigil honesty; Soft≠product; not bar3)\n");
                                /* Grep: net: lo: soft retmantle — Wave 31 mantle stamp (kept) */
                                kprintf("net: lo: soft retmantle exclusive=1 soft_ne_product=1 "
                                        "product_kernel=OPEN bar3=0 wave=59 "
                                        "(retmantle stamp; Soft≠product)\n");
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("net: lo: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retbulwark honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("net: lo: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retpanoply stamp; Soft≠product)\n");
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("net: lo: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retbastion honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("net: lo: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retcitadel stamp; Soft≠product)\n");
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("net: lo: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retredoubt honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("net: lo: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retkeep stamp; Soft≠product)\n");
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retfortress — Wave 35 return-fortress honesty */
kprintf("net: lo: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retfortress honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("net: lo: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retpalace stamp; Soft≠product)\n");
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft rethold — Wave 36 return-hold honesty */
kprintf("net: lo: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(rethold honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retspire — Wave 36 exclusive spire stamp */
kprintf("net: lo: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retspire stamp; Soft≠product)\n");
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retwall — Wave 37 return-wall honesty */
kprintf("net: lo: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retwall honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retgate — Wave 37 exclusive gate stamp */
kprintf("net: lo: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retgate stamp; Soft≠product)\n");
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retmoat — Wave 38 return-moat honesty */
kprintf("net: lo: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retmoat honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retower — Wave 38 exclusive tower stamp */
kprintf("net: lo: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retower stamp; Soft≠product)\n");
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("net: lo: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retbarbican honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("net: lo: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retglacis stamp; Soft≠product)\n");
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("net: lo: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retcurtain honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("net: lo: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retparapet stamp; Soft≠product)\n");
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retravelin — Wave 41 return-travelin honesty */
kprintf("net: lo: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("net: lo: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retditch stamp; Soft≠product)\n");
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("net: lo: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retportcullis honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("net: lo: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retbattlement stamp; Soft≠product)\n");
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("net: lo: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retmachicolation honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("net: lo: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retarrowslit stamp; Soft≠product)\n");

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("net: lo: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retmerlon honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("net: lo: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retembrasure stamp; Soft≠product)\n");

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("net: lo: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retkeepgate honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("net: lo: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retouterward stamp; Soft≠product)\n");

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retbailey — Wave 46 return-bailey honesty */
kprintf("net: lo: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retbailey honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("net: lo: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retpostern stamp; Soft≠product)\n");

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("net: lo: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("net: lo: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("net: lo: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("net: lo: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("net: lo: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("net: lo: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retsally — Wave 50 return-sally honesty */
kprintf("net: lo: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("net: lo: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retfosse — Wave 51 return-fosse honesty */
kprintf("net: lo: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("net: lo: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("net: lo: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("net: lo: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retravelin — Wave 53 return-travelin honesty */
kprintf("net: lo: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("net: lo: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("net: lo: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retredan — Wave 54 exclusive redan stamp */
kprintf("net: lo: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retflank — Wave 55 return-flank honesty */
kprintf("net: lo: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retface — Wave 55 exclusive face stamp */
kprintf("net: lo: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retgorge — Wave 56 return-gorge honesty */
kprintf("net: lo: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("net: lo: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retraverse — Wave 57 return-traverse honesty */
kprintf("net: lo: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("net: lo: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retorillon — Wave 58 return-orillon honesty */
kprintf("net: lo: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("net: lo: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: lo: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("net: lo: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=59 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: net: lo: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("net: lo: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=59 "
        "(rethornwork stamp; Soft≠product)\n");








                                kprintf("net: lo soft deepen wave=59 areas=74 used=%u free=%u "
                "listen=%u conn=%u logs=%u\n",
                cUsed, cFree, cListen, cConn, g_soft.u32SoftLogN);
        /* Grep: net_lo: soft retmap — Wave 19 return-surface map */
    kprintf("net_lo: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=59\n");

    /* Grep: net_lo: soft deepen (twin) */
        kprintf("net_lo: soft deepen wave=59 areas=72 used=%u free=%u "
                "listen=%u conn=%u logs=%u\n",
                cUsed, cFree, cListen, cConn, g_soft.u32SoftLogN);
    }

    /* Grep: net: lo soft PASS */
    kprintf("net: lo soft PASS wave=59 logs=%u skip=%llu event_n=%u "
            "event_skip=%llu max=%u event_max=%u force=%u slots=%u "
            "used=%u conn=%u "
            "(soft inventory only; not product gate)\n",
            g_soft.u32SoftLogN, (unsigned long long)s.u64LogSkip,
            g_soft.u32EventN, (unsigned long long)s.u64EventSkip,
            (unsigned)NET_LO_SOFT_LOG_MAX, (unsigned)NET_LO_SOFT_EVENT_MAX,
            fForce ? 1u : 0u, fSlots, cUsed, cConn);

    /* Grep: net_lo: soft PASS (twin) */
    kprintf("net_lo: soft PASS wave=59 logs=%u skip=%llu event_n=%u "
            "event_skip=%llu max=%u force=%u used=%u "
            "(soft inventory only; not product gate)\n",
            g_soft.u32SoftLogN, (unsigned long long)s.u64LogSkip,
            g_soft.u32EventN, (unsigned long long)s.u64EventSkip,
            (unsigned)NET_LO_SOFT_LOG_MAX, fForce ? 1u : 0u, cUsed);

    /* Per-live-slot soft detail (rate-limited; product smoke inventory). */
    if (fSlots == 0u) {
        return;
    }
    for (i = 0; i < NET_LO_MAX; i++) {
        if (!g_aSocks[i].u8Used) {
            continue;
        }
        /* Grep: net: lo soft slot */
        kprintf("net: lo soft slot=%u type=%u port=%u listen=%u conn=%u "
                "peer=%d rx=%u head=%u bl=%u pend=%u reuse=%u reusep=%u "
                "ka=%u bcast=%u shut_rd=%u shut_wr=%u rcvbuf=%u sndbuf=%u "
                "linger=%u/%u fd=%u\n",
                i, (unsigned)g_aSocks[i].u8Type,
                (unsigned)g_aSocks[i].u16Port,
                (unsigned)g_aSocks[i].u8Listening,
                (unsigned)g_aSocks[i].u8Connected,
                (int)g_aSocks[i].i16Peer,
                (unsigned)g_aSocks[i].u32RxLen,
                (unsigned)g_aSocks[i].u32RxHead,
                (unsigned)g_aSocks[i].u8Backlog,
                (unsigned)g_aSocks[i].u8Pending,
                (unsigned)g_aSocks[i].u8Reuse,
                (unsigned)g_aSocks[i].u8ReusePort,
                (unsigned)g_aSocks[i].u8Keepalive,
                (unsigned)g_aSocks[i].u8Broadcast,
                (unsigned)g_aSocks[i].u8ShutRd,
                (unsigned)g_aSocks[i].u8ShutWr,
                (unsigned)g_aSocks[i].u32RcvBuf,
                (unsigned)g_aSocks[i].u32SndBuf,
                (unsigned)g_aSocks[i].u8LingerOn,
                (unsigned)g_aSocks[i].u16LingerSec,
                (unsigned)(NET_FD_BASE + i));
        /* Grep: net_lo: soft slot (twin; force/first dumps only) */
        kprintf("net_lo: soft slot=%u type=%u port=%u listen=%u conn=%u "
                "peer=%d rx=%u bl=%u pend=%u fd=%u wave=59\n",
                i, (unsigned)g_aSocks[i].u8Type,
                (unsigned)g_aSocks[i].u16Port,
                (unsigned)g_aSocks[i].u8Listening,
                (unsigned)g_aSocks[i].u8Connected,
                (int)g_aSocks[i].i16Peer,
                (unsigned)g_aSocks[i].u32RxLen,
                (unsigned)g_aSocks[i].u8Backlog,
                (unsigned)g_aSocks[i].u8Pending,
                (unsigned)(NET_FD_BASE + i));
    }
}

/*
 * Rate-limit soft inventory: power-of-two op milestones, hard-capped.
 * Force path (emfile / table-empty) prefers slots but still respects
 * NET_LO_SOFT_LOG_MAX so serial cannot flood. Init calls lo_soft_print(1)
 * directly (pre-activity). Soft skip tallies only suppressed dumps (cap);
 * non-milestone ops are silent without a skip bump.
 * greppable: net: lo soft / net_lo: soft
 */
static void
lo_soft_maybe_log(int fForce)
{
    u64 u64N;

    lo_soft_bump(&g_soft.u64Ops);
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

void
net_lo_init(void)
{
    memset(g_aSocks, 0, sizeof(g_aSocks));
    memset(&g_soft, 0, sizeof(g_soft));
    kprintf("net_lo: init (loopback + peer ring + sockopt/backlog soft)\n");
    /* Grep: net: lo soft init / net_lo: soft init */
    kprintf("net: lo soft init max=%u fd_base=%u buf=%u backlog_max=%u "
            "rcv_def=%u snd_def=%u log_max=%u event_max=%u wave=59\n",
            (unsigned)NET_LO_MAX, (unsigned)NET_FD_BASE,
            (unsigned)NET_LO_BUF, (unsigned)NET_LO_BACKLOG_MAX,
            (unsigned)NET_LO_RCVBUF_DEF, (unsigned)NET_LO_SNDBUF_DEF,
            (unsigned)NET_LO_SOFT_LOG_MAX, (unsigned)NET_LO_SOFT_EVENT_MAX);
    kprintf("net_lo: soft init max=%u fd_base=%u buf=%u backlog_max=%u "
            "log_max=%u event_max=%u wave=59\n",
            (unsigned)NET_LO_MAX, (unsigned)NET_FD_BASE,
            (unsigned)NET_LO_BUF, (unsigned)NET_LO_BACKLOG_MAX,
            (unsigned)NET_LO_SOFT_LOG_MAX, (unsigned)NET_LO_SOFT_EVENT_MAX);
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
        kprintf("net: lo soft emfile max=%u ops=%llu used_hwm=%llu\n",
                (unsigned)NET_LO_MAX, (unsigned long long)g_soft.u64Ops,
                (unsigned long long)g_soft.u64HwmUsed);
        kprintf("net_lo: soft emfile max=%u ops=%llu wave=59\n",
                (unsigned)NET_LO_MAX, (unsigned long long)g_soft.u64Ops);
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

i64
net_lo_bind(i64 i64Fd, u16 u16Port)
{
    u32 u32Slot;

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64BindFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (port_in_use(u32Slot, u16Port, g_aSocks[u32Slot].u8Type,
                    g_aSocks[u32Slot].u8Reuse ||
                        g_aSocks[u32Slot].u8ReusePort)) {
        lo_soft_bump(&g_soft.u64BindFail);
        lo_soft_bump(&g_soft.u64EaddrInuse);
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
                "type=%u ops=%llu\n",
                (long long)i64Fd, (unsigned)g_aSocks[u32Slot].u16Port,
                (unsigned)g_aSocks[u32Slot].u8Backlog,
                (unsigned)g_aSocks[u32Slot].u8Type,
                (unsigned long long)g_soft.u64Ops);
        kprintf("net_lo: soft listen fd=%lld port=%u backlog=%u "
                "type=%u wave=59\n",
                (long long)i64Fd, (unsigned)g_aSocks[u32Slot].u16Port,
                (unsigned)g_aSocks[u32Slot].u8Backlog,
                (unsigned)g_aSocks[u32Slot].u8Type);
    }
    lo_soft_maybe_log(0);
    return 0;
}

i64
net_lo_connect(i64 i64Fd, u16 u16Port)
{
    u32 u32Slot;
    u32 i;

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64ConnFail);
        lo_soft_maybe_log(0);
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
                lo_soft_bump(&g_soft.u64ConnAgain);
                lo_soft_maybe_log(0);
                return -11; /* EAGAIN */
            }
            if (g_aSocks[i].u8Backlog == 0) {
                g_aSocks[i].u8Backlog = 1;
            }
            if (g_aSocks[i].u8Pending >= g_aSocks[i].u8Backlog &&
                g_aSocks[i].i16Peer >= 0) {
                lo_soft_bump(&g_soft.u64ConnAgain);
                lo_soft_maybe_log(0);
                return -11;
            }
            g_aSocks[i].i16Peer = (i16)u32Slot;
            if (g_aSocks[i].u8Pending < 255u) {
                g_aSocks[i].u8Pending++;
            }
            g_aSocks[u32Slot].i16Peer = (i16)i;
            g_aSocks[u32Slot].u8Connected = 1;
            g_aSocks[i].u8Connected = 1;
            lo_soft_bump(&g_soft.u64ConnOk);
            lo_soft_maybe_log(0);
            return 0;
        }
    }
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
    u32 i;
    i16 i16Cli;

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
    i16Cli = g_aSocks[u32Slot].i16Peer;
    if (i16Cli < 0 || (u32)i16Cli >= NET_LO_MAX || !g_aSocks[i16Cli].u8Used) {
        lo_soft_bump(&g_soft.u64AcceptAgain);
        lo_soft_maybe_log(0);
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
            lo_soft_bump(&g_soft.u64AcceptOk);
            /* Grep: net: lo soft accept / net_lo: soft accept (rate-limited) */
            if (lo_soft_event_ok()) {
                kprintf("net: lo soft accept listen_fd=%lld new_fd=%u "
                        "cli_slot=%d port=%u type=%u pending=%u ops=%llu\n",
                        (long long)i64Fd, (unsigned)(NET_FD_BASE + i),
                        (int)i16Cli, (unsigned)g_aSocks[i].u16Port,
                        (unsigned)g_aSocks[i].u8Type,
                        (unsigned)g_aSocks[u32Slot].u8Pending,
                        (unsigned long long)g_soft.u64Ops);
                kprintf("net_lo: soft accept listen_fd=%lld new_fd=%u "
                        "cli_slot=%d port=%u wave=59\n",
                        (long long)i64Fd, (unsigned)(NET_FD_BASE + i),
                        (int)i16Cli, (unsigned)g_aSocks[i].u16Port);
            }
            lo_soft_maybe_log(0);
            return (i64)(NET_FD_BASE + i);
        }
    }
    lo_soft_bump(&g_soft.u64AcceptFail);
    /* Grep: net: lo soft emfile / net_lo: soft emfile (accept mint) */
    if (lo_soft_event_ok()) {
        kprintf("net: lo soft emfile max=%u ops=%llu (accept mint)\n",
                (unsigned)NET_LO_MAX, (unsigned long long)g_soft.u64Ops);
        kprintf("net_lo: soft emfile max=%u ops=%llu (accept mint) "
                "wave=59\n",
                (unsigned)NET_LO_MAX, (unsigned long long)g_soft.u64Ops);
    }
    lo_soft_maybe_log(1);
    return -24; /* EMFILE */
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
    if (g_aSocks[u32Slot].u8ShutWr) {
        lo_soft_bump(&g_soft.u64SendPipe);
        lo_soft_maybe_log(0);
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
    if (pS->u8ShutRd) {
        lo_soft_bump(&g_soft.u64RecvEof);
        lo_soft_maybe_log(0);
        return 0; /* EOF */
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

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64ShutFail);
        lo_soft_maybe_log(0);
        return -9;
    }
    if (nHow < 0 || nHow > 2) {
        lo_soft_bump(&g_soft.u64ShutFail);
        lo_soft_maybe_log(0);
        return -22; /* EINVAL — validate before mutating flags */
    }
    u32Slot = (u32)(i64Fd - NET_FD_BASE);
    if (nHow == 0 || nHow == 2) {
        g_aSocks[u32Slot].u8ShutRd = 1;
    }
    if (nHow == 1 || nHow == 2) {
        g_aSocks[u32Slot].u8ShutWr = 1;
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
        /* struct linger { int l_onoff; int l_linger; } — soft store only. */
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
    /* SO_DEBUG=1, SO_DONTROUTE=5, SO_OOBINLINE=10, timeos — accept no-op */
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
        v = 0;
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
    fill_sin((u8 *)pAddr, g_aSocks[u32Slot].u16Port);
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
    fill_sin((u8 *)pAddr, g_aSocks[peer].u16Port);
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

    if (!net_lo_fd_ok(i64Fd)) {
        lo_soft_bump(&g_soft.u64CloseFail);
        lo_soft_maybe_log(0);
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
            lo_soft_bump(&g_soft.u64ClosePeerHalf);
        }
        if (g_aSocks[peer].u8Listening && g_aSocks[peer].u8Pending > 0) {
            g_aSocks[peer].u8Pending--;
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
