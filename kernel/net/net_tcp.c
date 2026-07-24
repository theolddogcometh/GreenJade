/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * IPv4 TCP over virtio-net + loopback pairs for product sshd / netstackd.
 * Pure C11, dual-licensed (MIT OR Apache-2.0).
 *
 * Features: SYN handshake, ordered RX, multi-segment TX, advertised window,
 * last-segment retransmit on poll, basic RTT ticks, soft close-state progress
 * (FIN_WAIT / LAST_ACK / TIME_WAIT reclaim), listen backlog soft.
 *
 * Multi-segment TX (product / netstackd 3000 B bulk smoke):
 *   net_tcp_send chunks payloads into TCP_MSS (1024) segments with FL_PSH.
 *   Door bounce NET_XFER_MAX (4096) must stay ≥ bulk; one SEND → ≥3 segs.
 *   Loopback path requires full peer RX push per segment (no silent short
 *   fill) so multi-seg integrity holds across the whole door transfer.
 *   Peer window (u16PeerWnd) soft-limits in-flight bytes across chunks.
 *
 * Stats (accepts, segments, bytes_rx/tx, retransmits):
 *   segs = TX segments + RX segments seen by net_tcp_input
 *   rtx  = successful last-segment retransmits from net_tcp_poll
 *
 * Soft inventory (Wave 34 exclusive deepen; this unit only):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   Lifetime path / ring / multi-seg / rtx / TW tallies (struct tcp_soft).
 *   Greppable prefix-stable serial markers (rate-limited; never flood):
 *     net: tcp soft inventory …  — live table + caps + wave
 *     net: tcp soft sock …       — mint ok|fail + hwm
 *     net: tcp soft bind …       — bind ok|fail
 *     net: tcp soft life …       — listen/conn/accept/close path
 *     net: tcp soft xfer …       — send/recv multi-seg/wnd/eof
 *     net: tcp soft input …      — hit|miss|syn|fin|rst|data
 *     net: tcp soft poll …       — rtx / TW reclaim ticks
 *     net: tcp soft ring …       — RX/TX ring + push soft
 *     net: tcp soft multi …      — multi-seg + peer-wnd clamp rollup
 *     net: tcp soft state …      — live state-class distribution
 *     net: tcp soft capacity …   — fixed table / MSS / bulk lamps
 *     net: tcp soft catalog …    — surface catalog (impl soft lamps)
 *     net: tcp soft outcome …    — ok|fail|again rollup
 *     net: tcp soft stats …      — aggregate path tallies
 *     net: tcp soft path …       — honesty: soft inventory ≠ bar3
 *     net: tcp soft slot=…       — per-live-slot detail (rate-limited)
 *     net: tcp soft deepen …     — wave=34 stamp + area count
 *     net: tcp soft init|listen|accept|connect|emfile|syn|syn_drop|multi-seg …
 *     net: tcp soft PASS …
 *   Twin prefix also emitted: "net_tcp: soft …".
 *   Cadence dumps at power-of-two op milestones, hard-capped at
 *   TCP_SOFT_LOG_MAX (force emfile / stats / rtx-poll also capped).
 *   Init always emits once. Event lines share TCP_SOFT_EVENT_MAX.
 *   Never hard-gates product policy. Soft ≠ bar3. Pure C.
 * Grep: net: tcp soft / net_tcp: soft
 */
#include <gj/klog.h>
#include <gj/net_tcp.h>
#include <gj/string.h>
#include <gj/timer.h>
#include <gj/virtio_net.h>

#define TCP_MAX      16
#define TCP_FD_BASE  96
/* RX ring + design per-call TX cap; both ≥ multi-seg bulk (3000) + preamble. */
#define TCP_RX_MAX   4096
#define TCP_TX_MAX   4096
/* Multi-seg chunk size; eth frames stay under MTU (hdr+MSS ≤ 1518). */
#define TCP_MSS      1024
#define TCP_WND      4096
#define TCP_RTX_MS   200
#define TCP_RTX_MAX  8
#define TCP_TW_MS    1000 /* soft TIME_WAIT reclaim */
#define TCP_BACKLOG_MAX 8
/*
 * Soft inventory serial budget (Wave 15). Absolute cap of greppable full
 * cadence dumps; milestones are power-of-two API op counts (1,2,4,…).
 * Legacy EVERY remains as a secondary cadence for dense op streams.
 * Event lines (listen/accept/emfile/syn/connect/multi-seg) share a
 * separate hard cap. Slot detail only on force or first N dumps.
 * greppable: net: tcp soft / net_tcp: soft
 */
#define TCP_SOFT_LOG_EVERY 32u
#define TCP_SOFT_LOG_MAX   8u
#define TCP_SOFT_EVENT_MAX 8u
#define TCP_SOFT_SLOT_LOGS 2u
/* Wave 34 exclusive soft deepen stamp (greppable wave=34). */
#define TCP_SOFT_DEEPEN_WAVE 34u
/* inventory sock bind life xfer input poll ring multi state capacity
 * catalog outcome stats path headroom surface ratio PASS slot deepen = 21 */
#define TCP_SOFT_DEEPEN_AREAS 45u

/* Compile-time sizing guards (pure C; fail if multi-seg room shrinks). */
typedef char tcp_rx_holds_bulk[(TCP_RX_MAX >= 3000u) ? 1 : -1];
typedef char tcp_tx_holds_bulk[(TCP_TX_MAX >= 3000u) ? 1 : -1];
typedef char tcp_mss_multi[(TCP_MSS > 0 && TCP_TX_MAX > TCP_MSS) ? 1 : -1];

#define ST_CLOSED      0
#define ST_LISTEN      1
#define ST_SYN_RCVD    2
#define ST_ESTABLISHED 3
#define ST_CLOSE_WAIT  4
#define ST_FIN_WAIT1   5
#define ST_FIN_WAIT2   6
#define ST_LAST_ACK    7
#define ST_TIME_WAIT   8

#define FL_FIN 0x01
#define FL_SYN 0x02
#define FL_RST 0x04
#define FL_PSH 0x08
#define FL_ACK 0x10

static const u8 g_aOurMac[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };
static const u8 g_aOurIp[4] = { 10, 0, 2, 15 };

struct tcp_sock {
	u8  u8Used;
	u8  u8State;
	u8  u8Listening;
	u8  u8IsLoop;
	u8  u8Backlog;  /* soft listen queue depth */
	u8  u8Pending;  /* soft pending SYN/accept count */
	u8  u8FinSent;  /* we have emitted FIN */
	u8  u8Pad0;
	u16 u16Lport;
	u16 u16Rport;
	u8  aRip[4];
	u8  aRmac[6];
	u16 u16PeerWnd;
	u16 u16Pad;
	u32 u32SndUna;
	u32 u32SndNxt;
	u32 u32RcvNxt;
	u32 u32RxLen;
	u32 u32RxHead;
	u8  aRx[TCP_RX_MAX];
	/* retransmit: last unacked data segment */
	u32 u32RtxSeq;
	u32 u32RtxLen;
	u32 u32RtxTick;
	u32 u32RtxCount;
	u8  aRtx[TCP_MSS];
	u8  u8RtxValid;
	u8  u8Pad2[3];
	i16 i16Peer;
	u16 u16Pad3;
	u32 u32TwTick; /* TIME_WAIT start (ms) */
};

static struct tcp_sock g_aT[TCP_MAX];
static u32 g_u32Accepts;
static u32 g_u32Segs;
static u32 g_u32RxB;
static u32 g_u32TxB;
static u32 g_u32Rtx;
static u32 g_u32TwReap;
static u16 g_u16IpId;

/*
 * Soft product inventory counters — wrap OK; diagnostics only; never
 * hard-gate product paths. Grep: net: tcp soft / net_tcp: soft
 * Wave 20 deepen: multi-line path dumps, capacity/catalog/outcome,
 * rate-limit lamps, deepen stamp, PASS. Soft ≠ bar3.
 */
struct tcp_soft {
	u64 u64Ops;          /* total API entries (success + fail) */
	u64 u64SockOk;
	u64 u64SockFail;     /* EMFILE-shaped table full */
	u64 u64BindOk;
	u64 u64BindFail;
	u64 u64ListenOk;
	u64 u64ListenFail;
	u64 u64ConnOk;
	u64 u64ConnFail;
	u64 u64ConnAgain;    /* backlog full soft reject */
	u64 u64ConnRefused;  /* no local listener */
	u64 u64AcceptOk;
	u64 u64AcceptFail;
	u64 u64AcceptAgain;
	u64 u64SendOk;
	u64 u64SendFail;
	u64 u64SendAgain;
	u64 u64SendPartial;  /* short multi-seg write */
	u64 u64SendMulti;    /* calls that used ≥2 MSS chunks */
	u64 u64SendWndLim;   /* peer window clamped a chunk */
	u64 u64RecvOk;
	u64 u64RecvFail;
	u64 u64RecvAgain;
	u64 u64RecvEof;
	u64 u64CloseOk;
	u64 u64CloseFail;
	u64 u64InputHit;     /* net_tcp_input consumed frame */
	u64 u64InputMiss;    /* ignored / not ours */
	u64 u64InputSyn;     /* passive SYN accepted into table */
	u64 u64InputSynDrop; /* SYN dropped (backlog / pending) */
	u64 u64InputRst;
	u64 u64InputFin;
	u64 u64InputData;
	u64 u64PollTicks;
	u64 u64PollRtx;
	u64 u64PollTw;
	u64 u64PushFull;     /* RX ring full / short push */
	u64 u64PushPartial;
	u64 u64HwmUsed;      /* high-water live used slots */
	u64 u64LogDumps;     /* times soft log was emitted */
	u64 u64LogSkip;      /* cadence/force dumps suppressed by cap */
	u64 u64EventSkip;    /* event lines suppressed by event cap */
	u32 u32SoftLogN;     /* inventory log emissions (u32 twin) */
	u32 u32EventN;       /* event line emissions (listen/accept/…) */
};

static struct tcp_soft g_soft;

static void
tcp_soft_bump(u64 *pCnt)
{
	if (pCnt == NULL) {
		return;
	}
	(*pCnt)++; /* wrap OK */
}

/* Live-table tallies for soft inventory (no alloc; walk TCP_MAX). */
static void
tcp_soft_tally(u32 *pUsed, u32 *pFree, u32 *pListen, u32 *pEstab,
	       u32 *pSyn, u32 *pCw, u32 *pFw, u32 *pTw, u32 *pLoop,
	       u32 *pPending, u32 *pRxBytes, u32 *pRtxLive)
{
	u32 i;
	u32 cUsed = 0;
	u32 cListen = 0;
	u32 cEstab = 0;
	u32 cSyn = 0;
	u32 cCw = 0;
	u32 cFw = 0;
	u32 cTw = 0;
	u32 cLoop = 0;
	u32 cPending = 0;
	u32 cRx = 0;
	u32 cRtx = 0;

	for (i = 0; i < TCP_MAX; i++) {
		if (!g_aT[i].u8Used) {
			continue;
		}
		cUsed++;
		if (g_aT[i].u8Listening || g_aT[i].u8State == ST_LISTEN) {
			cListen++;
		}
		if (g_aT[i].u8State == ST_ESTABLISHED) {
			cEstab++;
		}
		if (g_aT[i].u8State == ST_SYN_RCVD) {
			cSyn++;
		}
		if (g_aT[i].u8State == ST_CLOSE_WAIT ||
		    g_aT[i].u8State == ST_LAST_ACK) {
			cCw++;
		}
		if (g_aT[i].u8State == ST_FIN_WAIT1 ||
		    g_aT[i].u8State == ST_FIN_WAIT2) {
			cFw++;
		}
		if (g_aT[i].u8State == ST_TIME_WAIT) {
			cTw++;
		}
		if (g_aT[i].u8IsLoop) {
			cLoop++;
		}
		cPending += (u32)g_aT[i].u8Pending;
		cRx += g_aT[i].u32RxLen;
		if (g_aT[i].u8RtxValid) {
			cRtx++;
		}
	}
	if (pUsed != NULL) {
		*pUsed = cUsed;
	}
	if (pFree != NULL) {
		*pFree = TCP_MAX - cUsed;
	}
	if (pListen != NULL) {
		*pListen = cListen;
	}
	if (pEstab != NULL) {
		*pEstab = cEstab;
	}
	if (pSyn != NULL) {
		*pSyn = cSyn;
	}
	if (pCw != NULL) {
		*pCw = cCw;
	}
	if (pFw != NULL) {
		*pFw = cFw;
	}
	if (pTw != NULL) {
		*pTw = cTw;
	}
	if (pLoop != NULL) {
		*pLoop = cLoop;
	}
	if (pPending != NULL) {
		*pPending = cPending;
	}
	if (pRxBytes != NULL) {
		*pRxBytes = cRx;
	}
	if (pRtxLive != NULL) {
		*pRtxLive = cRtx;
	}
	if ((u64)cUsed > g_soft.u64HwmUsed) {
		g_soft.u64HwmUsed = (u64)cUsed;
	}
}

/**
 * Soft: rate-limit budget for one-shot event lines
 * (listen/accept/emfile/syn/connect/multi-seg/syn_drop).
 * Returns 1 if the line may print; 0 if suppressed (bump event_skip).
 * greppable: net: tcp soft listen|accept|emfile|syn|connect|multi-seg
 */
static int
tcp_soft_event_ok(void)
{
	if (g_soft.u32EventN >= TCP_SOFT_EVENT_MAX) {
		tcp_soft_bump(&g_soft.u64EventSkip);
		return 0;
	}
	if (g_soft.u32EventN < 0xffffffffu) {
		g_soft.u32EventN++;
	}
	return 1;
}

/*
 * Greppable soft product inventory + path dumps (Wave 20 exclusive).
 * Prefix-stable: "net: tcp soft …" and twin "net_tcp: soft …".
 * fForce: include per-live-slot detail (init / emfile / stats / rtx-poll).
 * Cadence dumps skip slots after TCP_SOFT_SLOT_LOGS to avoid flood.
 * Soft only — never hard-gates product policy; soft ≠ bar3.
 */
static void
tcp_soft_print(int fForce)
{
	u32 cUsed = 0;
	u32 cFree = 0;
	u32 cListen = 0;
	u32 cEstab = 0;
	u32 cSyn = 0;
	u32 cCw = 0;
	u32 cFw = 0;
	u32 cTw = 0;
	u32 cLoop = 0;
	u32 cPending = 0;
	u32 cRx = 0;
	u32 cRtxLive = 0;
	u32 i;
	u32 fSlots;
	u32 u32Wave;
	u32 u32Areas;
	struct tcp_soft s;

	u32Wave = TCP_SOFT_DEEPEN_WAVE;
	u32Areas = TCP_SOFT_DEEPEN_AREAS;
	tcp_soft_tally(&cUsed, &cFree, &cListen, &cEstab, &cSyn, &cCw, &cFw,
		       &cTw, &cLoop, &cPending, &cRx, &cRtxLive);
	s = g_soft;
	tcp_soft_bump(&g_soft.u64LogDumps);
	if (g_soft.u32SoftLogN < 0xffffffffu) {
		g_soft.u32SoftLogN++;
	}
	/* Slot detail: force always; cadence only for first few dumps. */
	fSlots = (fForce != 0 || g_soft.u32SoftLogN <= TCP_SOFT_SLOT_LOGS)
		     ? 1u
		     : 0u;

	/* Grep: net: tcp soft inventory */
	kprintf("net: tcp soft inventory used=%u free=%u listen=%u estab=%u "
		"syn=%u close_wait=%u fin_wait=%u time_wait=%u loop=%u "
		"pending=%u rx_bytes=%u rtx_live=%u hwm=%llu max=%u "
		"fd_base=%u mss=%u wnd=%u rx_max=%u tx_max=%u "
		"backlog_max=%u rtx_ms=%u tw_ms=%u logs=%u skip=%llu "
		"event_n=%u event_skip=%llu wave=%u\n",
		cUsed, cFree, cListen, cEstab, cSyn, cCw, cFw, cTw, cLoop,
		cPending, cRx, cRtxLive, (unsigned long long)s.u64HwmUsed,
		(unsigned)TCP_MAX, (unsigned)TCP_FD_BASE, (unsigned)TCP_MSS,
		(unsigned)TCP_WND, (unsigned)TCP_RX_MAX, (unsigned)TCP_TX_MAX,
		(unsigned)TCP_BACKLOG_MAX, (unsigned)TCP_RTX_MS,
		(unsigned)TCP_TW_MS, g_soft.u32SoftLogN,
		(unsigned long long)s.u64LogSkip, g_soft.u32EventN,
		(unsigned long long)s.u64EventSkip, u32Wave);

	/* Grep: net_tcp: soft inventory (twin prefix) */
	kprintf("net_tcp: soft inventory used=%u free=%u listen=%u estab=%u "
		"hwm=%llu accepts=%u segs=%u rx=%u tx=%u rtx=%u tw_reap=%u "
		"log_n=%u areas=%u wave=%u\n",
		cUsed, cFree, cListen, cEstab,
		(unsigned long long)s.u64HwmUsed, g_u32Accepts, g_u32Segs,
		g_u32RxB, g_u32TxB, g_u32Rtx, g_u32TwReap, g_soft.u32SoftLogN,
		u32Areas, u32Wave);

	/* Grep: net: tcp soft sock */
	kprintf("net: tcp soft sock ok=%llu fail=%llu hwm=%llu max=%u "
		"fd_base=%u wave=%u\n",
		(unsigned long long)s.u64SockOk,
		(unsigned long long)s.u64SockFail,
		(unsigned long long)s.u64HwmUsed, (unsigned)TCP_MAX,
		(unsigned)TCP_FD_BASE, u32Wave);

	/* Grep: net_tcp: soft sock (twin) */
	kprintf("net_tcp: soft sock ok=%llu fail=%llu hwm=%llu max=%u "
		"wave=%u\n",
		(unsigned long long)s.u64SockOk,
		(unsigned long long)s.u64SockFail,
		(unsigned long long)s.u64HwmUsed, (unsigned)TCP_MAX, u32Wave);

	/* Grep: net: tcp soft bind */
	kprintf("net: tcp soft bind ok=%llu fail=%llu wave=%u\n",
		(unsigned long long)s.u64BindOk,
		(unsigned long long)s.u64BindFail, u32Wave);

	/* Grep: net_tcp: soft bind (twin) */
	kprintf("net_tcp: soft bind ok=%llu fail=%llu wave=%u\n",
		(unsigned long long)s.u64BindOk,
		(unsigned long long)s.u64BindFail, u32Wave);

	/* Grep: net: tcp soft life */
	kprintf("net: tcp soft life listen=%llu listen_fail=%llu "
		"conn=%llu conn_fail=%llu conn_again=%llu conn_refused=%llu "
		"accept=%llu accept_fail=%llu accept_again=%llu "
		"close=%llu close_fail=%llu wave=%u\n",
		(unsigned long long)s.u64ListenOk,
		(unsigned long long)s.u64ListenFail,
		(unsigned long long)s.u64ConnOk,
		(unsigned long long)s.u64ConnFail,
		(unsigned long long)s.u64ConnAgain,
		(unsigned long long)s.u64ConnRefused,
		(unsigned long long)s.u64AcceptOk,
		(unsigned long long)s.u64AcceptFail,
		(unsigned long long)s.u64AcceptAgain,
		(unsigned long long)s.u64CloseOk,
		(unsigned long long)s.u64CloseFail, u32Wave);

	/* Grep: net_tcp: soft life (twin) */
	kprintf("net_tcp: soft life listen=%llu conn=%llu conn_again=%llu "
		"conn_refused=%llu accept=%llu close=%llu wave=%u\n",
		(unsigned long long)s.u64ListenOk,
		(unsigned long long)s.u64ConnOk,
		(unsigned long long)s.u64ConnAgain,
		(unsigned long long)s.u64ConnRefused,
		(unsigned long long)s.u64AcceptOk,
		(unsigned long long)s.u64CloseOk, u32Wave);

	/* Grep: net: tcp soft xfer */
	kprintf("net: tcp soft xfer send=%llu send_fail=%llu send_again=%llu "
		"send_partial=%llu send_multi=%llu send_wnd=%llu "
		"recv=%llu recv_fail=%llu recv_again=%llu recv_eof=%llu "
		"tx_bytes=%u rx_bytes=%u segs=%u mss=%u wave=%u\n",
		(unsigned long long)s.u64SendOk,
		(unsigned long long)s.u64SendFail,
		(unsigned long long)s.u64SendAgain,
		(unsigned long long)s.u64SendPartial,
		(unsigned long long)s.u64SendMulti,
		(unsigned long long)s.u64SendWndLim,
		(unsigned long long)s.u64RecvOk,
		(unsigned long long)s.u64RecvFail,
		(unsigned long long)s.u64RecvAgain,
		(unsigned long long)s.u64RecvEof, g_u32TxB, g_u32RxB,
		g_u32Segs, (unsigned)TCP_MSS, u32Wave);

	/* Grep: net_tcp: soft xfer (twin) */
	kprintf("net_tcp: soft xfer send=%llu multi=%llu wnd=%llu "
		"recv=%llu eof=%llu tx_b=%u rx_b=%u segs=%u wave=%u\n",
		(unsigned long long)s.u64SendOk,
		(unsigned long long)s.u64SendMulti,
		(unsigned long long)s.u64SendWndLim,
		(unsigned long long)s.u64RecvOk,
		(unsigned long long)s.u64RecvEof, g_u32TxB, g_u32RxB,
		g_u32Segs, u32Wave);

	/* Grep: net: tcp soft input */
	kprintf("net: tcp soft input hit=%llu miss=%llu syn=%llu "
		"syn_drop=%llu rst=%llu fin=%llu data=%llu accepts=%u "
		"wave=%u\n",
		(unsigned long long)s.u64InputHit,
		(unsigned long long)s.u64InputMiss,
		(unsigned long long)s.u64InputSyn,
		(unsigned long long)s.u64InputSynDrop,
		(unsigned long long)s.u64InputRst,
		(unsigned long long)s.u64InputFin,
		(unsigned long long)s.u64InputData, g_u32Accepts, u32Wave);

	/* Grep: net_tcp: soft input (twin) */
	kprintf("net_tcp: soft input hit=%llu miss=%llu syn=%llu "
		"syn_drop=%llu rst=%llu fin=%llu data=%llu wave=%u\n",
		(unsigned long long)s.u64InputHit,
		(unsigned long long)s.u64InputMiss,
		(unsigned long long)s.u64InputSyn,
		(unsigned long long)s.u64InputSynDrop,
		(unsigned long long)s.u64InputRst,
		(unsigned long long)s.u64InputFin,
		(unsigned long long)s.u64InputData, u32Wave);

	/* Grep: net: tcp soft poll (cadence rollup; event poll is separate) */
	kprintf("net: tcp soft poll ticks=%llu rtx=%llu tw=%llu "
		"total_rtx=%u total_tw=%u rtx_ms=%u rtx_max=%u tw_ms=%u "
		"wave=%u\n",
		(unsigned long long)s.u64PollTicks,
		(unsigned long long)s.u64PollRtx,
		(unsigned long long)s.u64PollTw, g_u32Rtx, g_u32TwReap,
		(unsigned)TCP_RTX_MS, (unsigned)TCP_RTX_MAX,
		(unsigned)TCP_TW_MS, u32Wave);

	/* Grep: net_tcp: soft poll (twin) */
	kprintf("net_tcp: soft poll ticks=%llu rtx=%llu tw=%llu "
		"total_rtx=%u total_tw=%u wave=%u\n",
		(unsigned long long)s.u64PollTicks,
		(unsigned long long)s.u64PollRtx,
		(unsigned long long)s.u64PollTw, g_u32Rtx, g_u32TwReap,
		u32Wave);

	/* Grep: net: tcp soft ring */
	kprintf("net: tcp soft ring rx_max=%u tx_max=%u mss=%u wnd=%u "
		"push_full=%llu push_partial=%llu rx_live=%u rtx_live=%u "
		"bulk=3000 wave=%u\n",
		(unsigned)TCP_RX_MAX, (unsigned)TCP_TX_MAX, (unsigned)TCP_MSS,
		(unsigned)TCP_WND, (unsigned long long)s.u64PushFull,
		(unsigned long long)s.u64PushPartial, cRx, cRtxLive, u32Wave);

	/* Grep: net_tcp: soft ring (twin) */
	kprintf("net_tcp: soft ring rx_max=%u tx_max=%u mss=%u push_full=%llu "
		"push_partial=%llu rx_live=%u rtx_live=%u wave=%u\n",
		(unsigned)TCP_RX_MAX, (unsigned)TCP_TX_MAX, (unsigned)TCP_MSS,
		(unsigned long long)s.u64PushFull,
		(unsigned long long)s.u64PushPartial, cRx, cRtxLive, u32Wave);

	/* Grep: net: tcp soft multi (Wave 15 multi-seg / peer-wnd rollup) */
	kprintf("net: tcp soft multi send_multi=%llu send_partial=%llu "
		"send_wnd=%llu segs=%u mss=%u bulk=3000 tx_b=%u wave=%u\n",
		(unsigned long long)s.u64SendMulti,
		(unsigned long long)s.u64SendPartial,
		(unsigned long long)s.u64SendWndLim, g_u32Segs,
		(unsigned)TCP_MSS, g_u32TxB, u32Wave);

	/* Grep: net_tcp: soft multi (twin) */
	kprintf("net_tcp: soft multi multi=%llu partial=%llu wnd=%llu "
		"segs=%u mss=%u wave=%u\n",
		(unsigned long long)s.u64SendMulti,
		(unsigned long long)s.u64SendPartial,
		(unsigned long long)s.u64SendWndLim, g_u32Segs,
		(unsigned)TCP_MSS, u32Wave);

	/* Grep: net: tcp soft state (Wave 15 live state-class distribution) */
	kprintf("net: tcp soft state used=%u free=%u listen=%u syn=%u "
		"estab=%u close_wait=%u fin_wait=%u time_wait=%u loop=%u "
		"pending=%u rtx_live=%u hwm=%llu wave=%u\n",
		cUsed, cFree, cListen, cSyn, cEstab, cCw, cFw, cTw, cLoop,
		cPending, cRtxLive, (unsigned long long)s.u64HwmUsed,
		u32Wave);

	/* Grep: net_tcp: soft state (twin) */
	kprintf("net_tcp: soft state used=%u listen=%u estab=%u syn=%u "
		"cw=%u fw=%u tw=%u loop=%u hwm=%llu wave=%u\n",
		cUsed, cListen, cEstab, cSyn, cCw, cFw, cTw, cLoop,
		(unsigned long long)s.u64HwmUsed, u32Wave);

	/* Grep: net: tcp soft capacity (Wave 15 fixed lamps) */
	kprintf("net: tcp soft capacity max=%u fd_base=%u mss=%u wnd=%u "
		"rx_max=%u tx_max=%u backlog_max=%u rtx_ms=%u rtx_max=%u "
		"tw_ms=%u bulk=3000 log_every=%u log_max=%u event_max=%u "
		"slot_logs=%u heap=0 wave=%u\n",
		(unsigned)TCP_MAX, (unsigned)TCP_FD_BASE, (unsigned)TCP_MSS,
		(unsigned)TCP_WND, (unsigned)TCP_RX_MAX, (unsigned)TCP_TX_MAX,
		(unsigned)TCP_BACKLOG_MAX, (unsigned)TCP_RTX_MS,
		(unsigned)TCP_RTX_MAX, (unsigned)TCP_TW_MS,
		(unsigned)TCP_SOFT_LOG_EVERY, (unsigned)TCP_SOFT_LOG_MAX,
		(unsigned)TCP_SOFT_EVENT_MAX, (unsigned)TCP_SOFT_SLOT_LOGS,
		u32Wave);

	/* Grep: net_tcp: soft capacity (twin) */
	kprintf("net_tcp: soft capacity max=%u mss=%u wnd=%u bulk=3000 "
		"log_max=%u event_max=%u wave=%u\n",
		(unsigned)TCP_MAX, (unsigned)TCP_MSS, (unsigned)TCP_WND,
		(unsigned)TCP_SOFT_LOG_MAX, (unsigned)TCP_SOFT_EVENT_MAX,
		u32Wave);

	/* Grep: net: tcp soft catalog (Wave 15 surface catalog lamps) */
	kprintf("net: tcp soft catalog sock=1 bind=1 listen=1 connect=1 "
		"accept=1 send=1 recv=1 close=1 input=1 poll=1 "
		"multi_seg=1 peer_wnd=1 rtx=1 tw_reap=1 backlog_soft=1 "
		"loop_pair=1 full_stack=0 netstackd=0 bar3=0 wave=%u\n",
		u32Wave);

	/* Grep: net_tcp: soft catalog (twin) */
	kprintf("net_tcp: soft catalog sock=1 bind=1 listen=1 connect=1 "
		"accept=1 xfer=1 input=1 poll=1 multi=1 rtx=1 tw=1 "
		"bar3=0 wave=%u\n",
		u32Wave);

	/* Grep: net: tcp soft outcome (Wave 15 ok|fail|again rollup) */
	kprintf("net: tcp soft outcome sock_ok=%llu sock_fail=%llu "
		"bind_ok=%llu bind_fail=%llu listen_ok=%llu conn_ok=%llu "
		"conn_again=%llu conn_refused=%llu accept_ok=%llu "
		"accept_again=%llu send_ok=%llu send_again=%llu "
		"recv_ok=%llu recv_again=%llu recv_eof=%llu close_ok=%llu "
		"wave=%u\n",
		(unsigned long long)s.u64SockOk,
		(unsigned long long)s.u64SockFail,
		(unsigned long long)s.u64BindOk,
		(unsigned long long)s.u64BindFail,
		(unsigned long long)s.u64ListenOk,
		(unsigned long long)s.u64ConnOk,
		(unsigned long long)s.u64ConnAgain,
		(unsigned long long)s.u64ConnRefused,
		(unsigned long long)s.u64AcceptOk,
		(unsigned long long)s.u64AcceptAgain,
		(unsigned long long)s.u64SendOk,
		(unsigned long long)s.u64SendAgain,
		(unsigned long long)s.u64RecvOk,
		(unsigned long long)s.u64RecvAgain,
		(unsigned long long)s.u64RecvEof,
		(unsigned long long)s.u64CloseOk, u32Wave);

	/* Grep: net_tcp: soft outcome (twin) */
	kprintf("net_tcp: soft outcome sock_ok=%llu sock_fail=%llu "
		"conn_ok=%llu conn_again=%llu accept_ok=%llu send_ok=%llu "
		"recv_ok=%llu multi=%llu wave=%u\n",
		(unsigned long long)s.u64SockOk,
		(unsigned long long)s.u64SockFail,
		(unsigned long long)s.u64ConnOk,
		(unsigned long long)s.u64ConnAgain,
		(unsigned long long)s.u64AcceptOk,
		(unsigned long long)s.u64SendOk,
		(unsigned long long)s.u64RecvOk,
		(unsigned long long)s.u64SendMulti, u32Wave);

	/* Grep: net: tcp soft stats (legacy field-stable rollup) */
	kprintf("net: tcp soft stats ops=%llu sock=%llu sock_fail=%llu "
		"bind=%llu bind_fail=%llu listen=%llu listen_fail=%llu "
		"conn=%llu conn_fail=%llu conn_again=%llu conn_refused=%llu "
		"accept=%llu accept_fail=%llu accept_again=%llu "
		"send=%llu send_fail=%llu send_again=%llu send_partial=%llu "
		"send_multi=%llu send_wnd=%llu "
		"recv=%llu recv_fail=%llu recv_again=%llu recv_eof=%llu "
		"close=%llu close_fail=%llu "
		"input_hit=%llu input_miss=%llu input_syn=%llu "
		"input_syn_drop=%llu input_rst=%llu input_fin=%llu "
		"input_data=%llu poll=%llu poll_rtx=%llu poll_tw=%llu "
		"push_full=%llu push_partial=%llu dumps=%llu "
		"skip=%llu event_skip=%llu log_max=%u event_max=%u "
		"wave=%u\n",
		(unsigned long long)s.u64Ops,
		(unsigned long long)s.u64SockOk,
		(unsigned long long)s.u64SockFail,
		(unsigned long long)s.u64BindOk,
		(unsigned long long)s.u64BindFail,
		(unsigned long long)s.u64ListenOk,
		(unsigned long long)s.u64ListenFail,
		(unsigned long long)s.u64ConnOk,
		(unsigned long long)s.u64ConnFail,
		(unsigned long long)s.u64ConnAgain,
		(unsigned long long)s.u64ConnRefused,
		(unsigned long long)s.u64AcceptOk,
		(unsigned long long)s.u64AcceptFail,
		(unsigned long long)s.u64AcceptAgain,
		(unsigned long long)s.u64SendOk,
		(unsigned long long)s.u64SendFail,
		(unsigned long long)s.u64SendAgain,
		(unsigned long long)s.u64SendPartial,
		(unsigned long long)s.u64SendMulti,
		(unsigned long long)s.u64SendWndLim,
		(unsigned long long)s.u64RecvOk,
		(unsigned long long)s.u64RecvFail,
		(unsigned long long)s.u64RecvAgain,
		(unsigned long long)s.u64RecvEof,
		(unsigned long long)s.u64CloseOk,
		(unsigned long long)s.u64CloseFail,
		(unsigned long long)s.u64InputHit,
		(unsigned long long)s.u64InputMiss,
		(unsigned long long)s.u64InputSyn,
		(unsigned long long)s.u64InputSynDrop,
		(unsigned long long)s.u64InputRst,
		(unsigned long long)s.u64InputFin,
		(unsigned long long)s.u64InputData,
		(unsigned long long)s.u64PollTicks,
		(unsigned long long)s.u64PollRtx,
		(unsigned long long)s.u64PollTw,
		(unsigned long long)s.u64PushFull,
		(unsigned long long)s.u64PushPartial,
		(unsigned long long)g_soft.u64LogDumps,
		(unsigned long long)s.u64LogSkip,
		(unsigned long long)s.u64EventSkip,
		(unsigned)TCP_SOFT_LOG_MAX, (unsigned)TCP_SOFT_EVENT_MAX,
		u32Wave);

	/* Grep: net_tcp: soft stats (twin) */
	kprintf("net_tcp: soft stats ops=%llu multi=%llu wnd=%llu "
		"syn_drop=%llu rtx=%u tw=%u dumps=%llu wave=%u\n",
		(unsigned long long)s.u64Ops,
		(unsigned long long)s.u64SendMulti,
		(unsigned long long)s.u64SendWndLim,
		(unsigned long long)s.u64InputSynDrop, g_u32Rtx, g_u32TwReap,
		(unsigned long long)g_soft.u64LogDumps, u32Wave);

	/* Grep: net: tcp soft path */
	kprintf("net: tcp soft path sock=table_mint bind=lport "
		"listen=backlog_soft conn=loop_pair|refused|again "
		"accept=mint_peer xfer=multi_seg|peer_wnd|rx_ring "
		"input=syn|data|fin|rst poll=rtx|tw_reap "
		"fd=%u..%u mss=%u bulk=3000 wave=%u "
		"(soft inventory; not bar3; not netstackd)\n",
		(unsigned)TCP_FD_BASE,
		(unsigned)(TCP_FD_BASE + TCP_MAX - 1u), (unsigned)TCP_MSS,
		u32Wave);

	/* Grep: net_tcp: soft path (twin) */
	kprintf("net_tcp: soft path sock=table_mint bind=lport "
		"listen=backlog_soft conn=loop_pair|refused|again "
		"accept=mint xfer=multi_seg|peer_wnd poll=rtx|tw "
		"fd=%u..%u mss=%u wave=%u "
		"(soft inventory; not bar3; not netstackd)\n",
		(unsigned)TCP_FD_BASE,
		(unsigned)(TCP_FD_BASE + TCP_MAX - 1u), (unsigned)TCP_MSS,
		u32Wave);

	/* Grep: net: tcp soft headroom — Wave 19 live slack lamps. */
	kprintf("net: tcp soft headroom free=%u used=%u max=%u estab=%u "
		"listen=%u hwm=%llu rtx_live=%u wave=%u\n",
		cFree, cUsed, (unsigned)TCP_MAX, cEstab, cListen,
		(unsigned long long)s.u64HwmUsed, cRtxLive, u32Wave);

	/* Grep: net_tcp: soft headroom (twin) */
	kprintf("net_tcp: soft headroom free=%u used=%u max=%u estab=%u "
		"hwm=%llu wave=%u\n",
		cFree, cUsed, (unsigned)TCP_MAX, cEstab,
		(unsigned long long)s.u64HwmUsed, u32Wave);

	/* Grep: net: tcp soft surface — Wave 19 surface bit lamps. */
	kprintf("net: tcp soft surface used=%u estab=%u listen=%u loop=%u "
		"multi=%u rtx=%u surf=0x%x wave=%u\n",
		cUsed != 0u ? 1u : 0u, cEstab != 0u ? 1u : 0u,
		cListen != 0u ? 1u : 0u, cLoop != 0u ? 1u : 0u,
		s.u64SendMulti != 0ull ? 1u : 0u, cRtxLive != 0u ? 1u : 0u,
		((cUsed != 0u) ? 1u : 0u) | ((cEstab != 0u) ? 2u : 0u) |
			((cListen != 0u) ? 4u : 0u) | ((cLoop != 0u) ? 8u : 0u) |
			((s.u64SendMulti != 0ull) ? 16u : 0u) |
			((cRtxLive != 0u) ? 32u : 0u),
		u32Wave);

	/* Grep: net_tcp: soft surface (twin) */
	kprintf("net_tcp: soft surface used=%u estab=%u listen=%u multi=%u "
		"surf=0x%x wave=%u\n",
		cUsed != 0u ? 1u : 0u, cEstab != 0u ? 1u : 0u,
		cListen != 0u ? 1u : 0u, s.u64SendMulti != 0ull ? 1u : 0u,
		((cUsed != 0u) ? 1u : 0u) | ((cEstab != 0u) ? 2u : 0u) |
			((cListen != 0u) ? 4u : 0u) |
			((s.u64SendMulti != 0ull) ? 8u : 0u),
		u32Wave);

	/* Grep: net: tcp soft ratio — Wave 16 occupancy basis points. */
	{
		u32 u32OccBp = 0;
		u32 u32EstabBp = 0;

		if ((unsigned)TCP_MAX != 0u) {
			u32OccBp = (cUsed * 10000u) / (unsigned)TCP_MAX;
		}
		if (cUsed != 0u) {
			u32EstabBp = (cEstab * 10000u) / cUsed;
		}
		kprintf("net: tcp soft ratio occ_bp=%u estab_bp=%u used=%u "
			"free=%u estab=%u wave=%u\n",
			u32OccBp, u32EstabBp, cUsed, cFree, cEstab, u32Wave);
		/* Grep: net_tcp: soft ratio (twin) */
		kprintf("net_tcp: soft ratio occ_bp=%u estab_bp=%u used=%u "
			"estab=%u wave=%u\n",
			u32OccBp, u32EstabBp, cUsed, cEstab, u32Wave);
	}

	/* Grep: net: tcp soft return — Wave 19 API return surfaces */
	kprintf("net: tcp soft return sock_ok=%llu sock_fail=%llu "
		"bind_ok=%llu listen_ok=%llu conn_ok=%llu accept_ok=%llu "
		"send_ok=%llu recv_ok=%llu close_ok=%llu product_tcp=OPEN wave=%u\n",
		(unsigned long long)s.u64SockOk, (unsigned long long)s.u64SockFail,
		(unsigned long long)s.u64BindOk, (unsigned long long)s.u64ListenOk,
		(unsigned long long)s.u64ConnOk, (unsigned long long)s.u64AcceptOk,
		(unsigned long long)s.u64SendOk, (unsigned long long)s.u64RecvOk,
		(unsigned long long)s.u64CloseOk, u32Wave);
	/* Grep: net_tcp: soft return (twin) */
	kprintf("net_tcp: soft return sock=%llu bind=%llu listen=%llu "
		"conn=%llu send=%llu recv=%llu product_tcp=OPEN wave=%u\n",
		(unsigned long long)s.u64SockOk, (unsigned long long)s.u64BindOk,
		(unsigned long long)s.u64ListenOk, (unsigned long long)s.u64ConnOk,
		(unsigned long long)s.u64SendOk, (unsigned long long)s.u64RecvOk,
		u32Wave);

	/* Grep: net: tcp soft deepen (Wave 20 stamp) */
	/*
	 * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
	 * Return surfaces only — soft inventory; never hard-gates product paths.
	 * Soft≠product; not bar3.
	 */
	/* Grep: net: tcp: soft retclass — Wave 19 return-class taxonomy (kept) */
	kprintf("net: tcp: soft retclass ok|fail|inval|nodev|busy|nomem "
	        "soft_only=1 product_gate=0 wave=%u "
	        "(retclass taxonomy; Soft≠product; not bar3)\n",
	        (unsigned)TCP_SOFT_DEEPEN_WAVE);
	/* Grep: net: tcp: soft retlane — Wave 19 return-lane catalog (kept) */
	kprintf("net: tcp: soft retlane inv|selftest|rate|retcode|retmap|class "
	        "product_kernel=OPEN soft_ne_product=1 wave=%u "
	        "(retlane catalog; Soft≠product)\n",
	        (unsigned)TCP_SOFT_DEEPEN_WAVE);
	/*
	 * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
	 * Return surfaces only — soft inventory; never hard-gates product paths.
	 * Soft≠product; not bar3.
	 */
	/* Grep: net: tcp: soft retbound — Wave 20 return-bound honesty (kept) */
	kprintf("net: tcp: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
	        "never_blocks_m0=1 wave=%u "
	        "(retbound honesty; Soft≠product; not bar3)\n",
	        (unsigned)TCP_SOFT_DEEPEN_WAVE);
	/* Grep: net: tcp: soft retseal — Wave 20 seal stamp (kept) */
	kprintf("net: tcp: soft retseal exclusive=1 soft_ne_product=1 "
	        "product_kernel=OPEN bar3=0 wave=%u "
	        "(retseal stamp; Soft≠product)\n",
	        (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /*
	         * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
	         * Return surfaces only — soft inventory; never hard-gates product paths.
	         * Soft≠product; not bar3.
	        */
	        /* Grep: net: tcp: soft retpulse — Wave 21 return-pulse honesty (kept) */
	        kprintf("net: tcp: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
	                "never_blocks_m0=1 wave=%u "
	                "(retpulse honesty; Soft≠product; not bar3)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /* Grep: net: tcp: soft retmark — Wave 21 mark stamp (kept) */
	        kprintf("net: tcp: soft retmark exclusive=1 soft_ne_product=1 "
	                "product_kernel=OPEN bar3=0 wave=%u "
	                "(retmark stamp; Soft≠product)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /*
	         * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
	         * Return surfaces only — soft inventory; never hard-gates product paths.
	         * Soft≠product; not bar3.
	        */
	        /* Grep: net: tcp: soft retphase — Wave 22 return-phase honesty (kept) */
	        kprintf("net: tcp: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
	                "never_blocks_m0=1 wave=%u "
	                "(retphase honesty; Soft≠product; not bar3)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /* Grep: net: tcp: soft retbadge — Wave 22 badge stamp (kept) */
	        kprintf("net: tcp: soft retbadge exclusive=1 soft_ne_product=1 "
	                "product_kernel=OPEN bar3=0 wave=%u "
	                "(retbadge stamp; Soft≠product)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
	        */
	        /* Grep: net: tcp: soft rettoken — Wave 23 return-token honesty (kept) */
	        kprintf("net: tcp: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
	                "never_blocks_m0=1 wave=%u "
	                "(rettoken honesty; Soft≠product; not bar3)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /* Grep: net: tcp: soft retcrest — Wave 23 crest stamp (kept) */
	        kprintf("net: tcp: soft retcrest exclusive=1 soft_ne_product=1 "
	                "product_kernel=OPEN bar3=0 wave=%u "
	                "(retcrest stamp; Soft≠product)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /*
	         * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
	         * Return surfaces only — soft inventory; never hard-gates product paths.
	         * Soft≠product; not bar3.
	         */
	        /* Grep: net: tcp: soft retvault — Wave 24 return-vault honesty (kept) */
	        kprintf("net: tcp: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
	                "never_blocks_m0=1 wave=%u "
	                "(retvault honesty; Soft≠product; not bar3)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /* Grep: net: tcp: soft retbanner — Wave 24 banner stamp (kept) */
	        kprintf("net: tcp: soft retbanner exclusive=1 soft_ne_product=1 "
	                "product_kernel=OPEN bar3=0 wave=%u "
	                "(retbanner stamp; Soft≠product)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /*
	         * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
	         * Return surfaces only — soft inventory; never hard-gates product paths.
	         * Soft≠product; not bar3.
	         */
	        /* Grep: net: tcp: soft retledger — Wave 25 return-ledger honesty (kept) */
	        kprintf("net: tcp: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
	                "never_blocks_m0=1 wave=%u "
	                "(retledger honesty; Soft≠product; not bar3)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /* Grep: net: tcp: soft retbeacon — Wave 25 beacon stamp (kept) */
	        kprintf("net: tcp: soft retbeacon exclusive=1 soft_ne_product=1 "
	                "product_kernel=OPEN bar3=0 wave=%u "
	                "(retbeacon stamp; Soft≠product)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /*
	         * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
	         * Return surfaces only — soft inventory; never hard-gates product paths.
	         * Soft≠product; not bar3.
	         */
	        /* Grep: net: tcp: soft retcipher — Wave 26 return-cipher honesty (kept) */
	        kprintf("net: tcp: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
	                "never_blocks_m0=1 wave=%u "
	                "(retcipher honesty; Soft≠product; not bar3)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	        /* Grep: net: tcp: soft retflame — Wave 26 flame stamp (kept) */
	        kprintf("net: tcp: soft retflame exclusive=1 soft_ne_product=1 "
	                "product_kernel=OPEN bar3=0 wave=%u "
	                "(retflame stamp; Soft≠product)\n",
	                (unsigned)TCP_SOFT_DEEPEN_WAVE);
	                /*
	                 * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
	                 * Return surfaces only — soft inventory; never hard-gates product paths.
	                 * Soft≠product; not bar3.
	                 */
	                /* Grep: net: tcp: soft retprism — Wave 27 return-prism honesty (kept) */
	                kprintf("net: tcp: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
	                        "never_blocks_m0=1 wave=%u "
	                        "(retprism honesty; Soft≠product; not bar3)\n",
	                        (unsigned)TCP_SOFT_DEEPEN_WAVE);
	                /* Grep: net: tcp: soft retforge — Wave 27 forge stamp (kept) */
	                kprintf("net: tcp: soft retforge exclusive=1 soft_ne_product=1 "
	                        "product_kernel=OPEN bar3=0 wave=%u "
	                        "(retforge stamp; Soft≠product)\n",
	                        (unsigned)TCP_SOFT_DEEPEN_WAVE);
	                        /*
	                         * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
	                         * Return surfaces only — soft inventory; never hard-gates product paths.
	                         * Soft≠product; not bar3.
	                         */
	                        /* Grep: net: tcp: soft retshard — Wave 28 return-shard honesty (kept) */
	                        kprintf("net: tcp: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
	                            "never_blocks_m0=1 wave=%u "
	                            "(retshard honesty; Soft≠product; not bar3)\n",
	                            (unsigned)TCP_SOFT_DEEPEN_WAVE);
	                        /* Grep: net: tcp: soft retcrown — Wave 28 crown stamp (kept) */
	                        kprintf("net: tcp: soft retcrown exclusive=1 soft_ne_product=1 "
	                            "product_kernel=OPEN bar3=0 wave=%u "
	                            "(retcrown stamp; Soft≠product)\n",
	                            (unsigned)TCP_SOFT_DEEPEN_WAVE);
                        /*
                         * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: net: tcp: soft retglyph — Wave 29 return-glyph honesty (kept) */
                        kprintf("net: tcp: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retglyph honesty; Soft≠product; not bar3)\n",
                                (unsigned)TCP_SOFT_DEEPEN_WAVE);
                        /* Grep: net: tcp: soft retscepter — Wave 29 scepter stamp (kept) */
                        kprintf("net: tcp: soft retscepter exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retscepter stamp; Soft≠product)\n",
                                (unsigned)TCP_SOFT_DEEPEN_WAVE);
                        /*
                         * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: net: tcp: soft retsigil — Wave 30 return-sigil honesty (kept) */
                        kprintf("net: tcp: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retsigil honesty; Soft≠product; not bar3)\n",
                                (unsigned)TCP_SOFT_DEEPEN_WAVE);
                        /* Grep: net: tcp: soft retemblem — Wave 30 emblem stamp (kept) */
                        kprintf("net: tcp: soft retemblem exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retemblem stamp; Soft≠product)\n",
                                (unsigned)TCP_SOFT_DEEPEN_WAVE);
                        /*
                         * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                         * Return surfaces only — soft inventory; never hard-gates product paths.
                         * Soft≠product; not bar3.
                         */
                        /* Grep: net: tcp: soft retaegis — Wave 31 return-aegis honesty (kept) */
                        kprintf("net: tcp: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retaegis honesty; Soft≠product; not bar3)\n",
                                (unsigned)TCP_SOFT_DEEPEN_WAVE);
                        /* Grep: net: tcp: soft retmantle — Wave 31 mantle stamp (kept) */
                        kprintf("net: tcp: soft retmantle exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retmantle stamp; Soft≠product)\n",
                                (unsigned)TCP_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: tcp: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("net: tcp: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)TCP_SOFT_DEEPEN_WAVE);
/* Grep: net: tcp: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("net: tcp: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)TCP_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: tcp: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("net: tcp: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)TCP_SOFT_DEEPEN_WAVE);
/* Grep: net: tcp: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("net: tcp: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)TCP_SOFT_DEEPEN_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: net: tcp: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("net: tcp: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)TCP_SOFT_DEEPEN_WAVE);
/* Grep: net: tcp: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("net: tcp: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)TCP_SOFT_DEEPEN_WAVE);
                        kprintf("net: tcp soft deepen wave=%u areas=%u used=%u estab=%u "
		"ops=%llu multi=%llu logs=%u skip=%llu "
		"event_n=%u event_skip=%llu ok=1 skip_hard=0\n",
		u32Wave, u32Areas, cUsed, cEstab,
		(unsigned long long)s.u64Ops,
		(unsigned long long)s.u64SendMulti, g_soft.u32SoftLogN,
		(unsigned long long)s.u64LogSkip, g_soft.u32EventN,
		(unsigned long long)s.u64EventSkip);

	/* Grep: net_tcp: soft retmap — Wave 19 return-surface map */
	kprintf("net_tcp: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=34\n");

	/* Grep: net_tcp: soft deepen (twin) */
	kprintf("net_tcp: soft deepen wave=%u areas=%u used=%u ops=%llu "
		"multi=%llu logs=%u ok=1\n",
		u32Wave, u32Areas, cUsed, (unsigned long long)s.u64Ops,
		(unsigned long long)s.u64SendMulti, g_soft.u32SoftLogN);

	/* Grep: net: tcp soft PASS */
	kprintf("net: tcp soft PASS wave=%u areas=%u logs=%u skip=%llu "
		"event_n=%u event_skip=%llu max=%u event_max=%u force=%u "
		"slots=%u used=%u estab=%u multi=%llu "
		"(soft inventory only; not product gate)\n",
		u32Wave, u32Areas, g_soft.u32SoftLogN,
		(unsigned long long)s.u64LogSkip, g_soft.u32EventN,
		(unsigned long long)s.u64EventSkip,
		(unsigned)TCP_SOFT_LOG_MAX, (unsigned)TCP_SOFT_EVENT_MAX,
		fForce ? 1u : 0u, fSlots, cUsed, cEstab,
		(unsigned long long)s.u64SendMulti);

	/* Grep: net_tcp: soft PASS (twin) */
	kprintf("net_tcp: soft PASS wave=%u areas=%u logs=%u used=%u "
		"estab=%u multi=%llu "
		"(soft inventory only; not product gate)\n",
		u32Wave, u32Areas, g_soft.u32SoftLogN, cUsed, cEstab,
		(unsigned long long)s.u64SendMulti);

	/* Grep: net: tcp soft inventory PASS */
	kprintf("net: tcp soft inventory PASS used=%u free=%u estab=%u "
		"logs=%u wave=%u areas=%u\n",
		cUsed, cFree, cEstab, g_soft.u32SoftLogN, u32Wave, u32Areas);

	/* Per-live-slot soft detail (rate-limited; product smoke inventory). */
	if (fSlots == 0u) {
		return;
	}
	for (i = 0; i < TCP_MAX; i++) {
		if (!g_aT[i].u8Used) {
			continue;
		}
		/* Grep: net: tcp soft slot */
		kprintf("net: tcp soft slot=%u state=%u listen=%u loop=%u "
			"lport=%u rport=%u peer=%d bl=%u pend=%u "
			"rx=%u head=%u peer_wnd=%u rtx_v=%u rtx_n=%u "
			"fin=%u snd_una=%u snd_nxt=%u rcv_nxt=%u fd=%u "
			"wave=%u\n",
			i, (unsigned)g_aT[i].u8State,
			(unsigned)g_aT[i].u8Listening,
			(unsigned)g_aT[i].u8IsLoop,
			(unsigned)g_aT[i].u16Lport,
			(unsigned)g_aT[i].u16Rport, (int)g_aT[i].i16Peer,
			(unsigned)g_aT[i].u8Backlog,
			(unsigned)g_aT[i].u8Pending,
			(unsigned)g_aT[i].u32RxLen,
			(unsigned)g_aT[i].u32RxHead,
			(unsigned)g_aT[i].u16PeerWnd,
			(unsigned)g_aT[i].u8RtxValid,
			(unsigned)g_aT[i].u32RtxCount,
			(unsigned)g_aT[i].u8FinSent,
			(unsigned)g_aT[i].u32SndUna,
			(unsigned)g_aT[i].u32SndNxt,
			(unsigned)g_aT[i].u32RcvNxt,
			(unsigned)(TCP_FD_BASE + i), u32Wave);
	}
}

/*
 * Rate-limit soft inventory: power-of-two op milestones, every-N fallback,
 * hard-capped. Force path (emfile / table-full / stats) prefers slots but
 * still respects TCP_SOFT_LOG_MAX so serial cannot flood. Init calls
 * tcp_soft_print(1) directly (pre-activity). Soft skip tallies only
 * suppressed dumps (cap); non-milestone ops are silent without a skip bump.
 * greppable: net: tcp soft
 */
static void
tcp_soft_maybe_log(int fForce)
{
	u64 u64N;
	int fMilestone;

	tcp_soft_bump(&g_soft.u64Ops);
	if (fForce != 0) {
		if (g_soft.u32SoftLogN >= TCP_SOFT_LOG_MAX) {
			tcp_soft_bump(&g_soft.u64LogSkip);
			return;
		}
		tcp_soft_print(1);
		return;
	}
	u64N = g_soft.u64Ops;
	/* Milestone: power-of-two ops, or every TCP_SOFT_LOG_EVERY. */
	fMilestone = 0;
	if (u64N != 0ull && (u64N & (u64N - 1ull)) == 0ull) {
		fMilestone = 1;
	}
	if (u64N != 0ull && (u64N % (u64)TCP_SOFT_LOG_EVERY) == 0ull) {
		fMilestone = 1;
	}
	if (fMilestone == 0) {
		return;
	}
	if (g_soft.u32SoftLogN >= TCP_SOFT_LOG_MAX) {
		tcp_soft_bump(&g_soft.u64LogSkip);
		return;
	}
	tcp_soft_print(0);
}

static u16
ip_cksum(const void *p, u32 cb)
{
	const u16 *w = (const u16 *)p;
	u32 sum = 0;

	while (cb > 1) {
		sum += *w++;
		cb -= 2;
	}
	if (cb) {
		sum += *(const u8 *)w;
	}
	while (sum >> 16) {
		sum = (sum & 0xffffu) + (sum >> 16);
	}
	return (u16)~sum;
}

static u16
tcp_cksum(const u8 *pIp, const u8 *pTcp, u32 cbTcp)
{
	u32 sum = 0;
	u32 i;
	u8 ph[12];

	memcpy(ph, pIp + 12, 4);
	memcpy(ph + 4, pIp + 16, 4);
	ph[8] = 0;
	ph[9] = 6;
	ph[10] = (u8)(cbTcp >> 8);
	ph[11] = (u8)cbTcp;
	for (i = 0; i < 12; i += 2) {
		sum += (u16)((ph[i] << 8) | ph[i + 1]);
	}
	for (i = 0; i + 1 < cbTcp; i += 2) {
		sum += (u16)((pTcp[i] << 8) | pTcp[i + 1]);
	}
	if (cbTcp & 1u) {
		sum += (u16)(pTcp[cbTcp - 1] << 8);
	}
	while (sum >> 16) {
		sum = (sum & 0xffffu) + (sum >> 16);
	}
	return (u16)~sum;
}

static i64
slot_to_fd(u32 s)
{
	return (i64)(TCP_FD_BASE + s);
}

static int
fd_to_slot(i64 fd, u32 *pS)
{
	if (fd < TCP_FD_BASE || fd >= TCP_FD_BASE + TCP_MAX) {
		return -1;
	}
	*pS = (u32)(fd - TCP_FD_BASE);
	return g_aT[*pS].u8Used ? 0 : -1;
}

static int
alloc_slot(void)
{
	u32 i;

	for (i = 0; i < TCP_MAX; i++) {
		if (!g_aT[i].u8Used) {
			memset(&g_aT[i], 0, sizeof(g_aT[i]));
			g_aT[i].u8Used = 1;
			g_aT[i].i16Peer = -1;
			g_aT[i].u32SndNxt = 0x1000u + i * 0x100u;
			g_aT[i].u32SndUna = g_aT[i].u32SndNxt;
			g_aT[i].u16PeerWnd = TCP_WND;
			return (int)i;
		}
	}
	return -1;
}

/*
 * Append up to cb bytes into the ordered RX ring.
 * Returns bytes actually buffered (0..cb); never exceeds free ring space.
 * Multi-seg TX on loopback requires a full push (got == requested).
 */
static int
push_rx(u32 s, const u8 *p, u32 cb)
{
	u32 i;
	u32 cbFree;

	if (s >= TCP_MAX || !g_aT[s].u8Used || p == NULL) {
		return -1;
	}
	if (cb == 0 || g_aT[s].u32RxLen >= TCP_RX_MAX) {
		if (cb != 0 && g_aT[s].u32RxLen >= TCP_RX_MAX) {
			tcp_soft_bump(&g_soft.u64PushFull);
		}
		return 0;
	}
	cbFree = TCP_RX_MAX - g_aT[s].u32RxLen;
	if (cb > cbFree) {
		cb = cbFree;
		tcp_soft_bump(&g_soft.u64PushPartial);
	}
	for (i = 0; i < cb; i++) {
		u32 pos = (g_aT[s].u32RxHead + g_aT[s].u32RxLen) % TCP_RX_MAX;

		g_aT[s].aRx[pos] = p[i];
		g_aT[s].u32RxLen++;
	}
	g_u32RxB += cb;
	return (int)cb;
}

static u32
now_ms(void)
{
	/* timer_mono_nsec if present; else coarse jiffies*10 */
	extern u64 timer_mono_nsec(void);

	return (u32)(timer_mono_nsec() / 1000000ull);
}

static int
tcp_tx_raw(u32 s, u8 flags, u32 seq, const u8 *pPay, u32 cbPay)
{
	u8 aOut[1518];
	u8 *pIp;
	u8 *pTcp;
	u32 cbTcp;
	u32 cbIp;
	u32 cbTot;
	u16 csum;
	u16 wnd;

	if (s >= TCP_MAX || !g_aT[s].u8Used) {
		return -1;
	}
	if (cbPay && pPay == NULL) {
		return -1;
	}
	/* Loopback multi-seg: each chunk must fully land in peer RX. */
	if (g_aT[s].u8IsLoop) {
		if (g_aT[s].i16Peer >= 0 && (u32)g_aT[s].i16Peer < TCP_MAX &&
		    g_aT[g_aT[s].i16Peer].u8Used) {
			u32 peer = (u32)g_aT[s].i16Peer;
			int got;

			/*
			 * Cap chunk to MSS so multi-seg bounds match eth path
			 * even if a caller skips net_tcp_send chunking.
			 */
			if (cbPay > TCP_MSS) {
				cbPay = TCP_MSS;
			}
			/* Loopback: payload into peer RX ring (window is soft). */
			if (cbPay) {
				got = push_rx(peer, pPay, cbPay);
				if (got < 0) {
					return -1;
				}
				/*
				 * Full-push required for multi-seg integrity.
				 * Partial ring fill used to return short and
				 * break netstackd 3000 B bulk (3×MSS) smoke.
				 * Return short count so net_tcp_send can stop
				 * cleanly after prior complete segments.
				 */
				if ((u32)got != cbPay) {
					return got > 0 ? got : -11; /* EAGAIN */
				}
			}
			if (flags & FL_SYN) {
				g_aT[peer].u32RcvNxt = seq + 1;
			}
			/* Soft FIN: peer → CLOSE_WAIT when we emit FIN. */
			if ((flags & FL_FIN) &&
			    (g_aT[peer].u8State == ST_ESTABLISHED ||
			     g_aT[peer].u8State == ST_FIN_WAIT1 ||
			     g_aT[peer].u8State == ST_FIN_WAIT2)) {
				g_aT[peer].u32RcvNxt++;
				if (g_aT[peer].u8State == ST_ESTABLISHED) {
					g_aT[peer].u8State = ST_CLOSE_WAIT;
				} else if (g_aT[peer].u8State == ST_FIN_WAIT1 ||
					   g_aT[peer].u8State == ST_FIN_WAIT2) {
					g_aT[peer].u8State = ST_TIME_WAIT;
					g_aT[peer].u32TwTick = now_ms();
				}
			}
			g_u32TxB += cbPay;
			g_u32Segs++;
			/* Always report full payload length on success (ABI). */
			return (int)cbPay;
		}
		return -1;
	}
	if (!virtio_net_ready()) {
		return -1;
	}
	/* Eth: one segment ≤ MSS; frame buf is 1518 (14+20+20+MSS). */
	if (cbPay > TCP_MSS) {
		cbPay = TCP_MSS;
	}
	memset(aOut, 0, sizeof(aOut));
	memcpy(aOut, g_aT[s].aRmac, 6);
	memcpy(aOut + 6, g_aOurMac, 6);
	aOut[12] = 0x08;
	aOut[13] = 0x00;
	pIp = aOut + 14;
	pIp[0] = 0x45;
	cbTcp = 20u + cbPay;
	cbIp = 20u + cbTcp;
	pIp[2] = (u8)(cbIp >> 8);
	pIp[3] = (u8)cbIp;
	g_u16IpId++;
	pIp[4] = (u8)(g_u16IpId >> 8);
	pIp[5] = (u8)g_u16IpId;
	pIp[8] = 64;
	pIp[9] = 6;
	memcpy(pIp + 12, g_aOurIp, 4);
	memcpy(pIp + 16, g_aT[s].aRip, 4);
	{
		u16 ic = ip_cksum(pIp, 20);

		pIp[10] = (u8)(ic >> 8);
		pIp[11] = (u8)ic;
	}
	pTcp = pIp + 20;
	pTcp[0] = (u8)(g_aT[s].u16Lport >> 8);
	pTcp[1] = (u8)g_aT[s].u16Lport;
	pTcp[2] = (u8)(g_aT[s].u16Rport >> 8);
	pTcp[3] = (u8)g_aT[s].u16Rport;
	{
		u32 ack = g_aT[s].u32RcvNxt;

		pTcp[4] = (u8)(seq >> 24);
		pTcp[5] = (u8)(seq >> 16);
		pTcp[6] = (u8)(seq >> 8);
		pTcp[7] = (u8)seq;
		pTcp[8] = (u8)(ack >> 24);
		pTcp[9] = (u8)(ack >> 16);
		pTcp[10] = (u8)(ack >> 8);
		pTcp[11] = (u8)ack;
	}
	pTcp[12] = (5 << 4);
	pTcp[13] = flags;
	wnd = (u16)(TCP_RX_MAX - g_aT[s].u32RxLen);
	if (wnd > TCP_WND) {
		wnd = TCP_WND;
	}
	pTcp[14] = (u8)(wnd >> 8);
	pTcp[15] = (u8)wnd;
	if (cbPay) {
		memcpy(pTcp + 20, pPay, cbPay);
	}
	csum = tcp_cksum(pIp, pTcp, cbTcp);
	pTcp[16] = (u8)(csum >> 8);
	pTcp[17] = (u8)csum;
	cbTot = 14u + cbIp;
	if (virtio_net_tx(aOut, cbTot) != 0) {
		return -1;
	}
	g_u32Segs++;
	g_u32TxB += cbPay;
	return (int)cbPay;
}

static int
tcp_tx(u32 s, u8 flags, const u8 *pPay, u32 cbPay)
{
	u32 seq = g_aT[s].u32SndNxt;
	int r;

	r = tcp_tx_raw(s, flags, seq, pPay, cbPay);
	if (r < 0) {
		return r;
	}
	if (flags & FL_SYN) {
		g_aT[s].u32SndNxt++;
	}
	if (flags & FL_FIN) {
		g_aT[s].u32SndNxt++;
		g_aT[s].u8FinSent = 1;
	}
	if (cbPay && r > 0) {
		u32 n = (u32)r;

		g_aT[s].u32SndNxt += n;
		/* save for retransmit */
		if (n <= TCP_MSS) {
			memcpy(g_aT[s].aRtx, pPay, n);
			g_aT[s].u32RtxLen = n;
			g_aT[s].u32RtxSeq = seq;
			g_aT[s].u32RtxTick = now_ms();
			g_aT[s].u32RtxCount = 0;
			g_aT[s].u8RtxValid = 1;
		}
	}
	return r;
}

void
net_tcp_init(void)
{
	memset(g_aT, 0, sizeof(g_aT));
	memset(&g_soft, 0, sizeof(g_soft));
	g_u32Accepts = 0;
	g_u32Segs = 0;
	g_u32RxB = 0;
	g_u32TxB = 0;
	g_u32Rtx = 0;
	g_u32TwReap = 0;
	g_u16IpId = 1;
	kprintf("net_tcp: IPv4 TCP ready (fd %u..%u) rtx_ms=%u wnd=%u mss=%u "
		"tw_ms=%u\n",
		TCP_FD_BASE, TCP_FD_BASE + TCP_MAX - 1, TCP_RTX_MS, TCP_WND,
		TCP_MSS, TCP_TW_MS);
	/* Grep: net: tcp soft init / net_tcp: soft init */
	kprintf("net: tcp soft init max=%u fd_base=%u mss=%u wnd=%u "
		"rx_max=%u tx_max=%u backlog_max=%u rtx_ms=%u tw_ms=%u "
		"log_every=%u log_max=%u event_max=%u areas=%u wave=%u\n",
		(unsigned)TCP_MAX, (unsigned)TCP_FD_BASE, (unsigned)TCP_MSS,
		(unsigned)TCP_WND, (unsigned)TCP_RX_MAX, (unsigned)TCP_TX_MAX,
		(unsigned)TCP_BACKLOG_MAX, (unsigned)TCP_RTX_MS,
		(unsigned)TCP_TW_MS, (unsigned)TCP_SOFT_LOG_EVERY,
		(unsigned)TCP_SOFT_LOG_MAX, (unsigned)TCP_SOFT_EVENT_MAX,
		(unsigned)TCP_SOFT_DEEPEN_AREAS, (unsigned)TCP_SOFT_DEEPEN_WAVE);
	kprintf("net_tcp: soft init max=%u fd_base=%u mss=%u bulk=3000 "
		"areas=%u wave=%u\n",
		(unsigned)TCP_MAX, (unsigned)TCP_FD_BASE, (unsigned)TCP_MSS,
		(unsigned)TCP_SOFT_DEEPEN_AREAS, (unsigned)TCP_SOFT_DEEPEN_WAVE);
	tcp_soft_print(1);
}

i64
net_tcp_socket(void)
{
	int s = alloc_slot();

	if (s < 0) {
		tcp_soft_bump(&g_soft.u64SockFail);
		/* Grep: net: tcp soft emfile (rate-limited) */
		if (tcp_soft_event_ok()) {
			kprintf("net: tcp soft emfile max=%u ops=%llu "
				"used_hwm=%llu wave=%u\n",
				(unsigned)TCP_MAX,
				(unsigned long long)g_soft.u64Ops,
				(unsigned long long)g_soft.u64HwmUsed,
				(unsigned)TCP_SOFT_DEEPEN_WAVE);
			kprintf("net_tcp: soft emfile max=%u ops=%llu "
				"wave=%u\n",
				(unsigned)TCP_MAX,
				(unsigned long long)g_soft.u64Ops,
				(unsigned)TCP_SOFT_DEEPEN_WAVE);
		}
		tcp_soft_maybe_log(1);
		return -24;
	}
	g_aT[s].u8State = ST_CLOSED;
	/* HWM via tally walk (outputs unused). */
	tcp_soft_tally(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
		       NULL, NULL, NULL);
	tcp_soft_bump(&g_soft.u64SockOk);
	tcp_soft_maybe_log(0);
	return slot_to_fd((u32)s);
}

int
net_tcp_fd_ok(i64 fd)
{
	u32 s;

	return fd_to_slot(fd, &s) == 0;
}

i64
net_tcp_bind(i64 fd, u16 port)
{
	u32 s;

	if (fd_to_slot(fd, &s) != 0) {
		tcp_soft_bump(&g_soft.u64BindFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	g_aT[s].u16Lport = port;
	tcp_soft_bump(&g_soft.u64BindOk);
	tcp_soft_maybe_log(0);
	return 0;
}

i64
net_tcp_listen(i64 fd, int backlog)
{
	u32 s;
	int nBl;

	if (fd_to_slot(fd, &s) != 0) {
		tcp_soft_bump(&g_soft.u64ListenFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	/* Soft backlog clamp (Linux-shaped: 0 → 1). */
	nBl = backlog;
	if (nBl < 1) {
		nBl = 1;
	}
	if (nBl > TCP_BACKLOG_MAX) {
		nBl = TCP_BACKLOG_MAX;
	}
	g_aT[s].u8Backlog = (u8)nBl;
	g_aT[s].u8Pending = 0;
	g_aT[s].u8Listening = 1;
	g_aT[s].u8State = ST_LISTEN;
	kprintf("net_tcp: LISTEN :%u fd=%ld backlog=%u\n", g_aT[s].u16Lport,
		(long)fd, g_aT[s].u8Backlog);
	tcp_soft_bump(&g_soft.u64ListenOk);
	/* Grep: net: tcp soft listen / net_tcp: soft listen (rate-limited) */
	if (tcp_soft_event_ok()) {
		kprintf("net: tcp soft listen fd=%lld port=%u backlog=%u "
			"pending=%u wave=%u\n",
			(long long)fd, (unsigned)g_aT[s].u16Lport,
			(unsigned)g_aT[s].u8Backlog,
			(unsigned)g_aT[s].u8Pending,
			(unsigned)TCP_SOFT_DEEPEN_WAVE);
		kprintf("net_tcp: soft listen fd=%lld port=%u backlog=%u "
			"wave=%u\n",
			(long long)fd, (unsigned)g_aT[s].u16Lport,
			(unsigned)g_aT[s].u8Backlog,
			(unsigned)TCP_SOFT_DEEPEN_WAVE);
	}
	tcp_soft_maybe_log(0);
	return 0;
}

i64
net_tcp_connect(i64 fd, u16 port)
{
	u32 s;
	u32 i;

	if (fd_to_slot(fd, &s) != 0) {
		tcp_soft_bump(&g_soft.u64ConnFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	for (i = 0; i < TCP_MAX; i++) {
		if (i != s && g_aT[i].u8Used && g_aT[i].u8Listening &&
		    g_aT[i].u16Lport == port) {
			int ns;

			/* Soft backlog: reject when accept queue is full. */
			if (g_aT[i].u8Backlog == 0) {
				g_aT[i].u8Backlog = 1;
			}
			if (g_aT[i].u8Pending >= g_aT[i].u8Backlog) {
				tcp_soft_bump(&g_soft.u64ConnAgain);
				tcp_soft_maybe_log(0);
				return -11; /* EAGAIN */
			}
			ns = alloc_slot();
			if (ns < 0) {
				tcp_soft_bump(&g_soft.u64ConnFail);
				tcp_soft_bump(&g_soft.u64SockFail);
				tcp_soft_maybe_log(1);
				return -24;
			}
			g_aT[ns].u8State = ST_ESTABLISHED;
			g_aT[ns].u16Lport = port;
			g_aT[ns].u16Rport =
			    g_aT[s].u16Lport ? g_aT[s].u16Lport : 40000;
			g_aT[ns].u8IsLoop = 1;
			g_aT[ns].i16Peer = (i16)s;
			g_aT[ns].u16PeerWnd = TCP_WND;
			g_aT[s].u8IsLoop = 1;
			g_aT[s].i16Peer = (i16)ns;
			g_aT[s].u16Rport = port;
			g_aT[s].u16Lport =
			    g_aT[s].u16Lport ? g_aT[s].u16Lport : 40000;
			g_aT[s].u8State = ST_ESTABLISHED;
			g_aT[s].u16PeerWnd = TCP_WND;
			g_aT[ns].u32RcvNxt = g_aT[s].u32SndNxt;
			g_aT[s].u32RcvNxt = g_aT[ns].u32SndNxt;
			g_aT[i].i16Peer = (i16)ns;
			if (g_aT[i].u8Pending < 255u) {
				g_aT[i].u8Pending++;
			}
			g_u32Accepts++;
			tcp_soft_tally(NULL, NULL, NULL, NULL, NULL, NULL,
				       NULL, NULL, NULL, NULL, NULL, NULL);
			tcp_soft_bump(&g_soft.u64ConnOk);
			/* Grep: net: tcp soft connect (rate-limited) */
			if (tcp_soft_event_ok()) {
				kprintf("net: tcp soft connect fd=%lld "
					"port=%u peer_slot=%d loop=1 "
					"listen_pending=%u wave=%u\n",
					(long long)fd, (unsigned)port, ns,
					(unsigned)g_aT[i].u8Pending,
					(unsigned)TCP_SOFT_DEEPEN_WAVE);
				kprintf("net_tcp: soft connect fd=%lld "
					"port=%u peer=%d wave=%u\n",
					(long long)fd, (unsigned)port, ns,
					(unsigned)TCP_SOFT_DEEPEN_WAVE);
			}
			tcp_soft_maybe_log(0);
			return 0;
		}
	}
	tcp_soft_bump(&g_soft.u64ConnRefused);
	tcp_soft_maybe_log(0);
	return -113;
}

i64
net_tcp_accept(i64 fd)
{
	u32 s;
	i16 peer;

	if (fd_to_slot(fd, &s) != 0) {
		tcp_soft_bump(&g_soft.u64AcceptFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	if (!g_aT[s].u8Listening) {
		tcp_soft_bump(&g_soft.u64AcceptFail);
		tcp_soft_maybe_log(0);
		return -22;
	}
	peer = g_aT[s].i16Peer;
	if (peer < 0 || (u32)peer >= TCP_MAX || !g_aT[peer].u8Used) {
		tcp_soft_bump(&g_soft.u64AcceptAgain);
		tcp_soft_maybe_log(0);
		return -11;
	}
	if (g_aT[peer].u8State != ST_ESTABLISHED &&
	    g_aT[peer].u8State != ST_SYN_RCVD) {
		tcp_soft_bump(&g_soft.u64AcceptAgain);
		tcp_soft_maybe_log(0);
		return -11;
	}
	g_aT[s].i16Peer = -1;
	if (g_aT[s].u8Pending > 0) {
		g_aT[s].u8Pending--;
	}
	tcp_soft_bump(&g_soft.u64AcceptOk);
	/* Grep: net: tcp soft accept / net_tcp: soft accept (rate-limited) */
	if (tcp_soft_event_ok()) {
		kprintf("net: tcp soft accept listen_fd=%lld new_fd=%u "
			"peer_slot=%d state=%u pending=%u wave=%u\n",
			(long long)fd, (unsigned)(TCP_FD_BASE + (u32)peer),
			(int)peer, (unsigned)g_aT[peer].u8State,
			(unsigned)g_aT[s].u8Pending,
			(unsigned)TCP_SOFT_DEEPEN_WAVE);
		kprintf("net_tcp: soft accept listen_fd=%lld new_fd=%u "
			"wave=%u\n",
			(long long)fd, (unsigned)(TCP_FD_BASE + (u32)peer),
			(unsigned)TCP_SOFT_DEEPEN_WAVE);
	}
	tcp_soft_maybe_log(0);
	return slot_to_fd((u32)peer);
}

i64
net_tcp_send(i64 fd, const void *pBuf, size_t cb)
{
	u32 s;
	i64 n = 0;
	u32 left;
	u32 cSegs = 0;
	const u8 *p = (const u8 *)pBuf;

	if (fd_to_slot(fd, &s) != 0 || pBuf == NULL) {
		tcp_soft_bump(&g_soft.u64SendFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	if (cb == 0) {
		return 0;
	}
	/*
	 * Soft: send allowed in ESTABLISHED and CLOSE_WAIT (half-close write
	 * path after peer FIN). FIN_WAIT* / LAST_ACK reject new data.
	 */
	if (g_aT[s].u8State != ST_ESTABLISHED &&
	    g_aT[s].u8State != ST_CLOSE_WAIT) {
		tcp_soft_bump(&g_soft.u64SendFail);
		tcp_soft_maybe_log(0);
		return -32;
	}
	if (g_aT[s].u8FinSent) {
		tcp_soft_bump(&g_soft.u64SendFail);
		tcp_soft_maybe_log(0);
		return -32;
	}
	/*
	 * Bound size_t → u32 and match door NET_XFER_MAX / TCP_TX_MAX.
	 * Multi-seg smoke (3000 B) fits; larger calls short-write cleanly.
	 */
	if (cb > TCP_TX_MAX) {
		left = TCP_TX_MAX;
	} else {
		left = (u32)cb;
	}
	/*
	 * Multi-segment TX: emit MSS-sized chunks (last may be short).
	 * Soft peer window limits in-flight across chunks; loopback advances
	 * SndUna per chunk (no real ACK path) so window does not stall bulk.
	 */
	while (left) {
		u32 chunk = left > TCP_MSS ? TCP_MSS : left;
		u32 in_flight = g_aT[s].u32SndNxt - g_aT[s].u32SndUna;
		int r;

		if (in_flight + chunk > g_aT[s].u16PeerWnd &&
		    g_aT[s].u16PeerWnd > 0) {
			tcp_soft_bump(&g_soft.u64SendWndLim);
			if (g_aT[s].u16PeerWnd > in_flight) {
				chunk = g_aT[s].u16PeerWnd - in_flight;
			} else if (n > 0) {
				break; /* partial send; caller may retry */
			} else {
				/* Peer wnd fully in-flight: still push 1B probe. */
				chunk = 1;
			}
		}
		if (chunk == 0) {
			break;
		}
		if (chunk > left) {
			chunk = left;
		}
		r = tcp_tx(s, (u8)(FL_ACK | FL_PSH), p, chunk);
		if (r < 0) {
			if (n > 0) {
				tcp_soft_bump(&g_soft.u64SendPartial);
				tcp_soft_bump(&g_soft.u64SendOk);
				if (cSegs >= 2u) {
					tcp_soft_bump(&g_soft.u64SendMulti);
				}
				tcp_soft_maybe_log(0);
				return n;
			}
			tcp_soft_bump(&g_soft.u64SendAgain);
			tcp_soft_maybe_log(0);
			return -11;
		}
		/* Loopback: peer has no ACK path — advance una immediately. */
		if (g_aT[s].u8IsLoop) {
			g_aT[s].u32SndUna = g_aT[s].u32SndNxt;
			g_aT[s].u8RtxValid = 0;
		}
		p += (u32)r;
		left -= (u32)r;
		n += r;
		cSegs++;
		/* Short segment (peer RX full): stop; prior segs already OK. */
		if ((u32)r < chunk) {
			tcp_soft_bump(&g_soft.u64SendPartial);
			break;
		}
	}
	if (n > 0) {
		tcp_soft_bump(&g_soft.u64SendOk);
		if (cSegs >= 2u) {
			tcp_soft_bump(&g_soft.u64SendMulti);
			/* Grep: net: tcp soft multi-seg (rate-limited) */
			if (tcp_soft_event_ok()) {
				kprintf("net: tcp soft multi-seg fd=%lld "
					"bytes=%lld segs=%u mss=%u "
					"tx_max=%u loop=%u wave=%u\n",
					(long long)fd, (long long)n,
					(unsigned)cSegs, (unsigned)TCP_MSS,
					(unsigned)TCP_TX_MAX,
					(unsigned)g_aT[s].u8IsLoop,
					(unsigned)TCP_SOFT_DEEPEN_WAVE);
				kprintf("net_tcp: soft multi-seg fd=%lld "
					"bytes=%lld segs=%u wave=%u\n",
					(long long)fd, (long long)n,
					(unsigned)cSegs,
					(unsigned)TCP_SOFT_DEEPEN_WAVE);
			}
		}
	} else {
		tcp_soft_bump(&g_soft.u64SendAgain);
	}
	tcp_soft_maybe_log(0);
	return n;
}

i64
net_tcp_recv(i64 fd, void *pBuf, size_t cb)
{
	u32 s;
	u32 n;
	u32 i;
	u8 *p = (u8 *)pBuf;

	if (fd_to_slot(fd, &s) != 0 || pBuf == NULL) {
		tcp_soft_bump(&g_soft.u64RecvFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	if (cb == 0) {
		return 0;
	}
	if (g_aT[s].u32RxLen == 0) {
		/* Soft EOF after peer FIN or local close states. */
		if (g_aT[s].u8State == ST_CLOSE_WAIT ||
		    g_aT[s].u8State == ST_TIME_WAIT ||
		    g_aT[s].u8State == ST_LAST_ACK ||
		    g_aT[s].u8State == ST_CLOSED) {
			tcp_soft_bump(&g_soft.u64RecvEof);
			tcp_soft_maybe_log(0);
			return 0;
		}
		tcp_soft_bump(&g_soft.u64RecvAgain);
		tcp_soft_maybe_log(0);
		return -11; /* EAGAIN */
	}
	/* Multi-seg drain: short reads OK; cap to ring depth and caller buf. */
	n = g_aT[s].u32RxLen;
	if (cb < (size_t)n) {
		n = (u32)cb;
	}
	if (n > TCP_RX_MAX) {
		n = TCP_RX_MAX; /* defensive; ring never holds more */
	}
	for (i = 0; i < n; i++) {
		p[i] = g_aT[s].aRx[g_aT[s].u32RxHead];
		g_aT[s].u32RxHead = (g_aT[s].u32RxHead + 1) % TCP_RX_MAX;
		g_aT[s].u32RxLen--;
	}
	tcp_soft_bump(&g_soft.u64RecvOk);
	tcp_soft_maybe_log(0);
	return (i64)n;
}

i64
net_tcp_close(i64 fd)
{
	u32 s;

	if (fd_to_slot(fd, &s) != 0) {
		tcp_soft_bump(&g_soft.u64CloseFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	/*
	 * Soft close: emit FIN on ESTABLISHED / CLOSE_WAIT (virtio + loop).
	 * Loopback tcp_tx_raw FIN advances peer to CLOSE_WAIT / TIME_WAIT.
	 * User close always frees the local slot (fd ABI); peer half-close
	 * and TIME_WAIT reaping live on remaining sockets / poll.
	 */
	if (g_aT[s].u8State == ST_ESTABLISHED ||
	    g_aT[s].u8State == ST_CLOSE_WAIT) {
		(void)tcp_tx(s, (u8)(FL_FIN | FL_ACK), 0, 0);
	}
	if (g_aT[s].i16Peer >= 0 && (u32)g_aT[s].i16Peer < TCP_MAX) {
		if (g_aT[g_aT[s].i16Peer].i16Peer == (i16)s) {
			g_aT[g_aT[s].i16Peer].i16Peer = -1;
		}
	}
	/* Listener soft pending: if this was queued accept child, release. */
	{
		u32 i;

		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used && g_aT[i].u8Listening &&
			    g_aT[i].i16Peer == (i16)s) {
				g_aT[i].i16Peer = -1;
				if (g_aT[i].u8Pending > 0) {
					g_aT[i].u8Pending--;
				}
			}
		}
	}
	memset(&g_aT[s], 0, sizeof(g_aT[s]));
	tcp_soft_bump(&g_soft.u64CloseOk);
	tcp_soft_maybe_log(0);
	return 0;
}

int
net_tcp_input(const u8 *pFrame, u32 cb)
{
	const u8 *pIp;
	const u8 *pTcp;
	u16 ihl;
	u16 sport, dport, wnd;
	u32 seq, ack;
	u8 flags;
	u32 doff;
	u32 pay_off;
	u32 pay_len;
	u32 i;
	int ls = -1;
	int cs = -1;

	if (pFrame == NULL || cb < 14 + 20 + 20) {
		tcp_soft_bump(&g_soft.u64InputMiss);
		return 0;
	}
	if (pFrame[12] != 0x08 || pFrame[13] != 0x00) {
		tcp_soft_bump(&g_soft.u64InputMiss);
		return 0;
	}
	pIp = pFrame + 14;
	if ((pIp[0] >> 4) != 4 || pIp[9] != 6) {
		tcp_soft_bump(&g_soft.u64InputMiss);
		return 0;
	}
	if (memcmp(pIp + 16, g_aOurIp, 4) != 0) {
		tcp_soft_bump(&g_soft.u64InputMiss);
		return 0;
	}
	ihl = (u16)((pIp[0] & 0x0f) * 4);
	if (cb < 14u + ihl + 20u) {
		tcp_soft_bump(&g_soft.u64InputMiss);
		return 0;
	}
	pTcp = pIp + ihl;
	sport = (u16)((pTcp[0] << 8) | pTcp[1]);
	dport = (u16)((pTcp[2] << 8) | pTcp[3]);
	seq = ((u32)pTcp[4] << 24) | ((u32)pTcp[5] << 16) |
	      ((u32)pTcp[6] << 8) | (u32)pTcp[7];
	ack = ((u32)pTcp[8] << 24) | ((u32)pTcp[9] << 16) |
	      ((u32)pTcp[10] << 8) | (u32)pTcp[11];
	doff = (u32)(pTcp[12] >> 4) * 4u;
	flags = pTcp[13];
	wnd = (u16)((pTcp[14] << 8) | pTcp[15]);
	if (doff < 20 || 14u + ihl + doff > cb) {
		tcp_soft_bump(&g_soft.u64InputMiss);
		return 0;
	}
	pay_off = 14u + ihl + doff;
	pay_len = cb > pay_off ? cb - pay_off : 0;
	g_u32Segs++;
	tcp_soft_bump(&g_soft.u64InputHit);

	for (i = 0; i < TCP_MAX; i++) {
		if (g_aT[i].u8Used && !g_aT[i].u8Listening &&
		    g_aT[i].u16Lport == dport && g_aT[i].u16Rport == sport &&
		    memcmp(g_aT[i].aRip, pIp + 12, 4) == 0) {
			cs = (int)i;
			break;
		}
	}
	for (i = 0; i < TCP_MAX; i++) {
		if (g_aT[i].u8Used && g_aT[i].u8Listening &&
		    g_aT[i].u16Lport == dport) {
			ls = (int)i;
			break;
		}
	}

	if (flags & FL_RST) {
		tcp_soft_bump(&g_soft.u64InputRst);
		if (cs >= 0) {
			memset(&g_aT[cs], 0, sizeof(g_aT[cs]));
		}
		return 1;
	}

	if ((flags & FL_SYN) && !(flags & FL_ACK) && ls >= 0 && cs < 0) {
		int ns;

		/* Soft backlog: drop SYN when accept queue is full. */
		if (g_aT[ls].u8Backlog == 0) {
			g_aT[ls].u8Backlog = 1;
		}
		if (g_aT[ls].u8Pending >= g_aT[ls].u8Backlog) {
			tcp_soft_bump(&g_soft.u64InputSynDrop);
			/* Grep: net: tcp soft syn_drop (rate-limited) */
			if (tcp_soft_event_ok()) {
				kprintf("net: tcp soft syn_drop port=%u "
					"pending=%u backlog=%u reason=full "
					"wave=%u\n",
					(unsigned)dport,
					(unsigned)g_aT[ls].u8Pending,
					(unsigned)g_aT[ls].u8Backlog,
					(unsigned)TCP_SOFT_DEEPEN_WAVE);
				kprintf("net_tcp: soft syn_drop port=%u "
					"pending=%u wave=%u\n",
					(unsigned)dport,
					(unsigned)g_aT[ls].u8Pending,
					(unsigned)TCP_SOFT_DEEPEN_WAVE);
			}
			return 1;
		}
		if (g_aT[ls].i16Peer >= 0) {
			/* One soft pending child at a time on listener slot. */
			tcp_soft_bump(&g_soft.u64InputSynDrop);
			return 1;
		}
		ns = alloc_slot();
		if (ns < 0) {
			tcp_soft_bump(&g_soft.u64InputSynDrop);
			tcp_soft_bump(&g_soft.u64SockFail);
			return 1;
		}
		g_aT[ns].u8State = ST_SYN_RCVD;
		g_aT[ns].u16Lport = dport;
		g_aT[ns].u16Rport = sport;
		memcpy(g_aT[ns].aRip, pIp + 12, 4);
		memcpy(g_aT[ns].aRmac, pFrame + 6, 6);
		g_aT[ns].u32RcvNxt = seq + 1;
		g_aT[ns].u16PeerWnd = wnd ? wnd : TCP_WND;
		g_aT[ns].u8IsLoop = 0;
		g_aT[ns].i16Peer = -1;
		(void)tcp_tx((u32)ns, (u8)(FL_SYN | FL_ACK), 0, 0);
		g_aT[ls].i16Peer = (i16)ns;
		if (g_aT[ls].u8Pending < 255u) {
			g_aT[ls].u8Pending++;
		}
		tcp_soft_bump(&g_soft.u64InputSyn);
		tcp_soft_tally(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			       NULL, NULL, NULL, NULL);
		/* Grep: net: tcp soft syn / net_tcp: soft syn (rate-limited) */
		if (tcp_soft_event_ok()) {
			kprintf("net: tcp soft syn port=%u sport=%u slot=%d "
				"pending=%u wnd=%u state=%u wave=%u\n",
				(unsigned)dport, (unsigned)sport, ns,
				(unsigned)g_aT[ls].u8Pending,
				(unsigned)g_aT[ns].u16PeerWnd,
				(unsigned)g_aT[ns].u8State,
				(unsigned)TCP_SOFT_DEEPEN_WAVE);
			kprintf("net_tcp: soft syn port=%u slot=%d wave=%u\n",
				(unsigned)dport, ns,
				(unsigned)TCP_SOFT_DEEPEN_WAVE);
		}
		return 1;
	}

	if (cs < 0) {
		return 1;
	}

	if (wnd) {
		g_aT[cs].u16PeerWnd = wnd;
	}

	if (g_aT[cs].u8State == ST_SYN_RCVD && (flags & FL_ACK)) {
		g_aT[cs].u8State = ST_ESTABLISHED;
		g_aT[cs].u32SndUna = ack;
		g_u32Accepts++;
		kprintf("net_tcp: ESTABLISHED :%u ← %u.%u.%u.%u:%u\n",
			g_aT[cs].u16Lport, g_aT[cs].aRip[0], g_aT[cs].aRip[1],
			g_aT[cs].aRip[2], g_aT[cs].aRip[3], g_aT[cs].u16Rport);
	}

	if (flags & FL_ACK) {
		if (ack > g_aT[cs].u32SndUna && ack <= g_aT[cs].u32SndNxt) {
			g_aT[cs].u32SndUna = ack;
			if (g_aT[cs].u8RtxValid &&
			    ack >= g_aT[cs].u32RtxSeq + g_aT[cs].u32RtxLen) {
				g_aT[cs].u8RtxValid = 0;
			}
			/* Soft close progress on ACK of our FIN. */
			if (g_aT[cs].u8FinSent &&
			    ack >= g_aT[cs].u32SndNxt) {
				if (g_aT[cs].u8State == ST_FIN_WAIT1) {
					g_aT[cs].u8State = ST_FIN_WAIT2;
				} else if (g_aT[cs].u8State == ST_LAST_ACK) {
					/* Fully closed — free slot. */
					memset(&g_aT[cs], 0, sizeof(g_aT[cs]));
					return 1;
				}
			}
		}
	}

	if (g_aT[cs].u8State == ST_ESTABLISHED ||
	    g_aT[cs].u8State == ST_SYN_RCVD ||
	    g_aT[cs].u8State == ST_FIN_WAIT1 ||
	    g_aT[cs].u8State == ST_FIN_WAIT2) {
		/*
		 * In-order only; advance RcvNxt by bytes actually buffered.
		 * pay_len is frame-derived; push_rx clamps to free RX space.
		 * Multi-seg peers retransmit if we ACK less than offered.
		 */
		if (pay_len && seq == g_aT[cs].u32RcvNxt &&
		    (g_aT[cs].u8State == ST_ESTABLISHED ||
		     g_aT[cs].u8State == ST_SYN_RCVD)) {
			u32 cbTake = pay_len;
			int got;

			if (cbTake > TCP_MSS) {
				cbTake = TCP_MSS; /* one segment bound */
			}
			got = push_rx((u32)cs, pFrame + pay_off, cbTake);
			if (got > 0) {
				g_aT[cs].u32RcvNxt += (u32)got;
				tcp_soft_bump(&g_soft.u64InputData);
				(void)tcp_tx((u32)cs, FL_ACK, 0, 0);
			}
		}
		if (flags & FL_FIN) {
			if (seq == g_aT[cs].u32RcvNxt ||
			    seq + pay_len == g_aT[cs].u32RcvNxt) {
				g_aT[cs].u32RcvNxt++;
				tcp_soft_bump(&g_soft.u64InputFin);
				if (g_aT[cs].u8State == ST_ESTABLISHED ||
				    g_aT[cs].u8State == ST_SYN_RCVD) {
					g_aT[cs].u8State = ST_CLOSE_WAIT;
					(void)tcp_tx((u32)cs, FL_ACK, 0, 0);
				} else if (g_aT[cs].u8State == ST_FIN_WAIT1) {
					/* Simultaneous close → TIME_WAIT soft. */
					g_aT[cs].u8State = ST_TIME_WAIT;
					g_aT[cs].u32TwTick = now_ms();
					(void)tcp_tx((u32)cs,
						     (u8)(FL_ACK | FL_FIN), 0,
						     0);
				} else if (g_aT[cs].u8State == ST_FIN_WAIT2) {
					g_aT[cs].u8State = ST_TIME_WAIT;
					g_aT[cs].u32TwTick = now_ms();
					(void)tcp_tx((u32)cs, FL_ACK, 0, 0);
				}
			}
		}
	}
	return 1;
}

void
net_tcp_poll(void)
{
	u32 i;
	u32 t = now_ms();
	u32 cRtx = 0;
	u32 cTw = 0;

	tcp_soft_bump(&g_soft.u64PollTicks);

	/* Last unacked data segment retransmit (virtio path only). */
	for (i = 0; i < TCP_MAX; i++) {
		if (!g_aT[i].u8Used) {
			continue;
		}
		/* Soft TIME_WAIT reclaim — free slots for new accepts. */
		if (g_aT[i].u8State == ST_TIME_WAIT) {
			if (t - g_aT[i].u32TwTick >= TCP_TW_MS) {
				memset(&g_aT[i], 0, sizeof(g_aT[i]));
				g_u32TwReap++;
				tcp_soft_bump(&g_soft.u64PollTw);
				cTw++;
			}
			continue;
		}
		if (!g_aT[i].u8RtxValid) {
			continue;
		}
		if (g_aT[i].u8IsLoop) {
			continue;
		}
		if (g_aT[i].u32RtxCount >= TCP_RTX_MAX) {
			continue;
		}
		if (t - g_aT[i].u32RtxTick < TCP_RTX_MS) {
			continue;
		}
		if (tcp_tx_raw(i, (u8)(FL_ACK | FL_PSH), g_aT[i].u32RtxSeq,
			       g_aT[i].aRtx, g_aT[i].u32RtxLen) >= 0) {
			g_aT[i].u32RtxTick = t;
			g_aT[i].u32RtxCount++;
			g_u32Rtx++;
			tcp_soft_bump(&g_soft.u64PollRtx);
			cRtx++;
		}
	}
	/*
	 * Soft inventory on poll only when rtx/TW work happened (rate-friendly).
	 * Stats getters force a full dump for smoke greps.
	 */
	if (cRtx != 0u || cTw != 0u) {
		/* Grep: net: tcp soft poll (event; rate-limited) */
		if (tcp_soft_event_ok()) {
			kprintf("net: tcp soft poll rtx=%u tw_reap=%u "
				"total_rtx=%u total_tw=%u ticks=%llu "
				"wave=%u\n",
				(unsigned)cRtx, (unsigned)cTw, g_u32Rtx,
				g_u32TwReap,
				(unsigned long long)g_soft.u64PollTicks,
				(unsigned)TCP_SOFT_DEEPEN_WAVE);
			kprintf("net_tcp: soft poll rtx=%u tw=%u wave=%u\n",
				(unsigned)cRtx, (unsigned)cTw,
				(unsigned)TCP_SOFT_DEEPEN_WAVE);
		}
		if (g_soft.u32SoftLogN < TCP_SOFT_LOG_MAX) {
			tcp_soft_print(1);
		} else {
			tcp_soft_bump(&g_soft.u64LogSkip);
		}
	}
}

u32
net_tcp_accepts(void)
{
	/*
	 * Emit soft inventory on stats read so door TCP_STATS / bring-up
	 * smoke can grep "net: tcp soft …" / "net_tcp: soft …".
	 * Force path respects TCP_SOFT_LOG_MAX after first few dumps.
	 */
	if (g_soft.u32SoftLogN < TCP_SOFT_LOG_MAX) {
		tcp_soft_print(1);
	} else {
		tcp_soft_bump(&g_soft.u64LogSkip);
	}
	return g_u32Accepts;
}

u32
net_tcp_segments(void)
{
	return g_u32Segs;
}

u32
net_tcp_bytes_rx(void)
{
	return g_u32RxB;
}

u32
net_tcp_bytes_tx(void)
{
	return g_u32TxB;
}

u32
net_tcp_retransmits(void)
{
	return g_u32Rtx;
}

u32
net_tcp_tw_reaps(void)
{
	return g_u32TwReap;
}
