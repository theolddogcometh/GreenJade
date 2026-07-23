/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * IPv4 TCP over virtio-net + loopback pairs for product sshd / netstackd.
 * Pure C11, dual-licensed (MIT OR Apache-2.0).
 *
 * Features: SYN handshake, ordered RX, multi-segment TX, advertised window,
 * last-segment retransmit on poll, basic RTT ticks.
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

/* Compile-time sizing guards (pure C; fail if multi-seg room shrinks). */
typedef char tcp_rx_holds_bulk[(TCP_RX_MAX >= 3000u) ? 1 : -1];
typedef char tcp_tx_holds_bulk[(TCP_TX_MAX >= 3000u) ? 1 : -1];
typedef char tcp_mss_multi[(TCP_MSS > 0 && TCP_TX_MAX > TCP_MSS) ? 1 : -1];

#define ST_CLOSED      0
#define ST_LISTEN      1
#define ST_SYN_RCVD    2
#define ST_ESTABLISHED 3
#define ST_CLOSE_WAIT  4

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
};

static struct tcp_sock g_aT[TCP_MAX];
static u32 g_u32Accepts;
static u32 g_u32Segs;
static u32 g_u32RxB;
static u32 g_u32TxB;
static u32 g_u32Rtx;
static u16 g_u16IpId;

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
		return 0;
	}
	cbFree = TCP_RX_MAX - g_aT[s].u32RxLen;
	if (cb > cbFree) {
		cb = cbFree;
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
	g_u32Accepts = 0;
	g_u32Segs = 0;
	g_u32RxB = 0;
	g_u32TxB = 0;
	g_u32Rtx = 0;
	g_u16IpId = 1;
	kprintf("net_tcp: IPv4 TCP ready (fd %u..%u) rtx_ms=%u wnd=%u mss=%u\n",
		TCP_FD_BASE, TCP_FD_BASE + TCP_MAX - 1, TCP_RTX_MS, TCP_WND,
		TCP_MSS);
}

i64
net_tcp_socket(void)
{
	int s = alloc_slot();

	if (s < 0) {
		return -24;
	}
	g_aT[s].u8State = ST_CLOSED;
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
		return -9;
	}
	g_aT[s].u16Lport = port;
	return 0;
}

i64
net_tcp_listen(i64 fd, int backlog)
{
	u32 s;

	(void)backlog;
	if (fd_to_slot(fd, &s) != 0) {
		return -9;
	}
	g_aT[s].u8Listening = 1;
	g_aT[s].u8State = ST_LISTEN;
	kprintf("net_tcp: LISTEN :%u fd=%ld\n", g_aT[s].u16Lport, (long)fd);
	return 0;
}

i64
net_tcp_connect(i64 fd, u16 port)
{
	u32 s;
	u32 i;

	if (fd_to_slot(fd, &s) != 0) {
		return -9;
	}
	for (i = 0; i < TCP_MAX; i++) {
		if (i != s && g_aT[i].u8Used && g_aT[i].u8Listening &&
		    g_aT[i].u16Lport == port) {
			int ns = alloc_slot();

			if (ns < 0) {
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
			g_u32Accepts++;
			return 0;
		}
	}
	return -113;
}

i64
net_tcp_accept(i64 fd)
{
	u32 s;
	i16 peer;

	if (fd_to_slot(fd, &s) != 0) {
		return -9;
	}
	if (!g_aT[s].u8Listening) {
		return -22;
	}
	peer = g_aT[s].i16Peer;
	if (peer < 0 || (u32)peer >= TCP_MAX || !g_aT[peer].u8Used) {
		return -11;
	}
	if (g_aT[peer].u8State != ST_ESTABLISHED &&
	    g_aT[peer].u8State != ST_SYN_RCVD) {
		return -11;
	}
	g_aT[s].i16Peer = -1;
	return slot_to_fd((u32)peer);
}

i64
net_tcp_send(i64 fd, const void *pBuf, size_t cb)
{
	u32 s;
	i64 n = 0;
	u32 left;
	const u8 *p = (const u8 *)pBuf;

	if (fd_to_slot(fd, &s) != 0 || pBuf == NULL) {
		return -9;
	}
	if (cb == 0) {
		return 0;
	}
	if (g_aT[s].u8State != ST_ESTABLISHED) {
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
			return n > 0 ? n : -11;
		}
		/* Loopback: peer has no ACK path — advance una immediately. */
		if (g_aT[s].u8IsLoop) {
			g_aT[s].u32SndUna = g_aT[s].u32SndNxt;
			g_aT[s].u8RtxValid = 0;
		}
		p += (u32)r;
		left -= (u32)r;
		n += r;
		/* Short segment (peer RX full): stop; prior segs already OK. */
		if ((u32)r < chunk) {
			break;
		}
	}
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
		return -9;
	}
	if (cb == 0) {
		return 0;
	}
	if (g_aT[s].u32RxLen == 0) {
		if (g_aT[s].u8State == ST_CLOSE_WAIT) {
			return 0; /* EOF after FIN */
		}
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
	return (i64)n;
}

i64
net_tcp_close(i64 fd)
{
	u32 s;

	if (fd_to_slot(fd, &s) != 0) {
		return -9;
	}
	if (g_aT[s].u8State == ST_ESTABLISHED && !g_aT[s].u8IsLoop) {
		(void)tcp_tx(s, (u8)(FL_FIN | FL_ACK), 0, 0);
	}
	if (g_aT[s].i16Peer >= 0 && (u32)g_aT[s].i16Peer < TCP_MAX) {
		g_aT[g_aT[s].i16Peer].i16Peer = -1;
	}
	memset(&g_aT[s], 0, sizeof(g_aT[s]));
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
		return 0;
	}
	if (pFrame[12] != 0x08 || pFrame[13] != 0x00) {
		return 0;
	}
	pIp = pFrame + 14;
	if ((pIp[0] >> 4) != 4 || pIp[9] != 6) {
		return 0;
	}
	if (memcmp(pIp + 16, g_aOurIp, 4) != 0) {
		return 0;
	}
	ihl = (u16)((pIp[0] & 0x0f) * 4);
	if (cb < 14u + ihl + 20u) {
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
		return 0;
	}
	pay_off = 14u + ihl + doff;
	pay_len = cb > pay_off ? cb - pay_off : 0;
	g_u32Segs++;

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
		if (cs >= 0) {
			memset(&g_aT[cs], 0, sizeof(g_aT[cs]));
		}
		return 1;
	}

	if ((flags & FL_SYN) && !(flags & FL_ACK) && ls >= 0 && cs < 0) {
		int ns = alloc_slot();

		if (ns < 0) {
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
		if (g_aT[ls].i16Peer < 0) {
			g_aT[ls].i16Peer = (i16)ns;
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
		}
	}

	if (g_aT[cs].u8State == ST_ESTABLISHED ||
	    g_aT[cs].u8State == ST_SYN_RCVD) {
		/*
		 * In-order only; advance RcvNxt by bytes actually buffered.
		 * pay_len is frame-derived; push_rx clamps to free RX space.
		 * Multi-seg peers retransmit if we ACK less than offered.
		 */
		if (pay_len && seq == g_aT[cs].u32RcvNxt) {
			u32 cbTake = pay_len;
			int got;

			if (cbTake > TCP_MSS) {
				cbTake = TCP_MSS; /* one segment bound */
			}
			got = push_rx((u32)cs, pFrame + pay_off, cbTake);
			if (got > 0) {
				g_aT[cs].u32RcvNxt += (u32)got;
				(void)tcp_tx((u32)cs, FL_ACK, 0, 0);
			}
		}
		if (flags & FL_FIN) {
			g_aT[cs].u32RcvNxt++;
			g_aT[cs].u8State = ST_CLOSE_WAIT;
			(void)tcp_tx((u32)cs, (u8)(FL_ACK | FL_FIN), 0, 0);
		}
	}
	return 1;
}

void
net_tcp_poll(void)
{
	u32 i;
	u32 t = now_ms();

	/* Last unacked data segment retransmit (virtio path only). */
	for (i = 0; i < TCP_MAX; i++) {
		if (!g_aT[i].u8Used || !g_aT[i].u8RtxValid) {
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
		}
	}
}

u32
net_tcp_accepts(void)
{
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
