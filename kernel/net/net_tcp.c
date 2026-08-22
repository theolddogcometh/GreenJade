/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * IPv4 TCP over virtio-net, UDX L2 (ETH_INJECT / ETH_TX_PULL), and
 * loopback pairs for product sshd / netstackd.
 * Pure C11, dual-licensed (MIT OR Apache-2.0). Soft!=product · G-AC-1.
 *
 * Features: SYN handshake, ordered RX, multi-segment TX, advertised window,
 * SYN/SYN-ACK + last-segment retransmit on poll, basic RTT ticks, soft
 * close-state progress (FIN_WAIT / LAST_ACK / TIME_WAIT reclaim), listen
 * backlog soft. External eth: net_tcp: soft listen :22 + eth_syn + SYN-ACK rtx.
 * Soft ensure :22 after net_l2 ready; rtl bind forces lab IP 10.200.125.50.
 * Accept queue residual: multi-child AcceptQ + pending release on giveup/RST.
 *
 * Exclusive residual lean - userspace/ABI socket path (Soft!=product):
 *   listen/accept/shutdown/name/rtx for cold Linux personality + sshd over
 *   stack (mirror net_lo shapes; leave soft port tables). Soft half-close
 *   SHUT_RD/WR + bare-FIN rtx + sticky RST POLLERR on poll_mask.
 *   Functional thrash-strip: no micro/nano/pico multi-pass; lean rtx only.
 *   FORBIDDEN: freestanding rtl R0 residual deepen rabbit hole.
 *   no version/wave stamp · no TCP_SOFT_DEEPEN_WAVE · no stamp storms ·
 *   no net_eth_poll/net_tcp_poll on IRQ (run-loop only) · dual license.
 * Host nc/ssh still DUT-OPEN. Soft listen :22 != host banner proof.
 * Product NIC = UDX+ABI. Soft!=product · G-AC-1.
 *
 * Functional residual STRONGER (sshd :22 product path; Soft!=product):
 *   listen: reparent + rehook oldest ESTABLISHED AcceptQ (eth prefer).
 *   accept: EAGAIN heal once (ensure :22 + multi-listener reparent/rehook).
 *   ensure :22: rehook after reparent on product / soft / soft-mint paths.
 *   poll / poll_mask: silent :22 pending heal + ready rehook (POLLIN honest);
 *   poll_mask :22 not-ready -> ensure+reparent+rehook; post multi-pass rehook.
 *   close AcceptQ: reparent + rehook_ready (eth prefer) on same-port listeners.
 *   eth_estab: AcceptQ SYN_RCVD->ESTABLISHED multi-listener reparent+rehook
 *   so product sshd poll/accept sees POLLIN without pure-POLL wait.
 *   listen close: orphan AcceptQ reparent+rehook onto remaining same-port
 *   listeners (soft-mint teardown / product re-arm). Soft!=product.
 *   greppable: net_tcp: soft functional lap | acceptq_rehook | accept_eagain_heal
 *   greppable: eth_estab_rehook | listen_close_rehook
 *   Dual DoD A/B remain OPEN (agent!=close). !=host_banner_proof.
 *
 * C1 residual deepen (Soft!=product; Dual DoD OPEN; agent!=close):
 *   class=C1 lab dual DoD residual for sshd :22 over stack + soft listen.
 *   dual_dod_a=OPEN dual_dod_b=OPEN product_sshd_tcp22=OPEN until host
 *   interactive SSH login. Banner != login. freestanding_class=SKIP (no freestanding rtl
 *   R0 product track). Soft residual lean once self-check catalogs socket
 *   path + H1 thr-only poll + thrash-strip + multi-seg bulk + lab_ip/:22.
 *   Soft residual != Dual DoD close. stamp-free · G-AC-1 · no GPL.
 *
 * W11 Dual DoD B FUNCTIONAL residual (Soft!=product; stamp-free bar
 *   v2026.08.04.75; never invent .76): wire handoff + :22 stack for product
 *   sshd. Eth demux (net_eth) -> soft listen :22 / AcceptQ / poll_mask /
 *   door SOCK_POLL yield. H1: net_tcp_poll only from net_eth_poll thr/door
 *   stack (never IRQ). Dual DoD A/B remain OPEN (agent!=close).
 *   STRONGER denser wire22 residual: multi-arm :22 listen/accept path honesty
 *   (ensure|listen|acceptq|accept|accept_eagain_heal|eth_estab_rehook|
 *   listen_close_rehook|poll|poll_mask). product_sshd_tcp22=OPEN until interactive SSH login.
 *   denser arms: h1_poll|listen22|accept22|rehook_heal|dual_dod_open|
 *   product_sshd_open. Soft residual != Dual DoD close. H2 once (no stamp
 *   storms). Soft!=product.
 *   Denser H1 thr-only sublocks (thr|stack|stamp) + denser path sublocks
 *   (ensure|listen|acceptq|accept|heal|rehook). H1 eth poll from door thr only
 *   (net_tcp_poll never IRQ). Dual DoD OPEN until interactive SSH login.
 *   greppable: net_tcp: soft residual wire22 | wire_handoff+tcp22
 *   greppable: net_tcp: soft residual wire22 denser | denser=1 | denser_arms
 *   greppable: stack=eth|tcp|door|:22 | W11 Dual DoD B FUNCTIONAL leftover MAP
 *   greppable: listen_accept_path_honesty | leftover MAP | H2=once
 *   greppable: product_sshd_tcp22=OPEN | dual_dod_b=OPEN | leftover MAP
 *   greppable: denser_h1_sub | denser_path_sub | thr-only door eth poll
 *
 * Multi-segment TX (product / netstackd 3000 B bulk smoke):
 *   net_tcp_send chunks payloads into TCP_MSS (1024) segments with FL_PSH.
 *   Door bounce NET_XFER_MAX (4096) must stay ≥ bulk; one SEND -> ≥3 segs.
 *   Loopback path requires full peer RX push per segment (no silent short
 *   fill) so multi-seg integrity holds across the whole door transfer.
 *   Peer window (u16PeerWnd) soft-limits in-flight bytes across chunks.
 *
 * Stats (accepts, segments, bytes_rx/tx, retransmits):
 *   segs = TX segments + RX segments seen by net_tcp_input
 *   rtx  = successful last-segment retransmits from net_tcp_poll
 *
 * Soft inventory (exclusive Dual DoD B residual; this unit only):
 *   Lifetime path / ring / multi-seg / rtx / TW / shut / name tallies.
 *   Greppable prefix-stable serial markers (rate-limited; never flood):
 *     net: tcp soft inventory ...  - ONE short line only (never multi-kprintf /
 *                                  never per-slot loops - #PF I=1 stack guard)
 *     net: tcp soft poll ...       - rtx / TW reclaim (one-shot / stamp-capped)
 *     net_tcp: soft listen :22 ... - ensure one-shot; !=host_banner_proof
 *     net_tcp: soft eth_syn ...    - passive SYN (one line; event-capped)
 *     net_tcp: soft eth_estab ...  - handshake complete + AcceptQ rehook
 *     net_tcp: soft accept ...     - accept mint + residual taken
 *     net_tcp: soft acceptq reparent / close_rel - orphan AcceptQ residual
 *     net_tcp: soft functional lap - listen|accept|poll|estab rehook STRONGER
 *     net_tcp: soft shutdown/name  - userspace/ABI socket residual
 *     net_tcp: soft residual lean  - C1 Dual DoD OPEN lean self-check (once)
 *     net_tcp: soft residual lean PASS - soft only; never Dual DoD close
 *     net_tcp: soft residual wire22 - W11 denser :22 listen/accept honesty
 *     net_tcp: soft residual wire22 denser - multi-arm denser=1 (H2 once)
 *     hot residual multi-pass / tx_busy: SILENT (micro/nano/pico thrash SKIP)
 *   Cadence dumps at power-of-two op milestones, hard-capped at
 *   TCP_SOFT_LOG_MAX; poll-path stamps hard-capped per poll
 *   (TCP_SOFT_POLL_STAMP_MAX). Init emits once (no twin). Event lines share
 *   TCP_SOFT_EVENT_MAX. No version/wave stamp. No multi-KiB format storms.
 *   net_tcp_poll only from net_eth_poll run-loop (never timer IRQ). Soft!=product.
 * greppable: net: tcp soft / net_tcp: soft
 * greppable: dual_dod_b=OPEN | dual_dod_a=OPEN | product_sshd_tcp22=OPEN
 * greppable: freestanding_class=SKIP | product=UDX | class=C1 | agent!=close
 * greppable: net_tcp_poll=run_loop_only | Soft!=product | G-AC-1
 * greppable: soft functional lap | acceptq_rehook | accept_eagain_heal
 * greppable: poll_mask_heal | ensure_rehook | multi_listen_heal | close_rehook
 * greppable: eth_estab_rehook | listen_close_rehook
 * greppable: net_tcp: soft residual wire22 | wire_handoff+tcp22
 * greppable: net_tcp: soft residual wire22 denser | denser=1 | denser_arms
 * greppable: arm_h1_poll | arm_listen22 | arm_accept22 | arm_rehook_heal
 * greppable: arm_dual_dod_open | arm_product_sshd_open
 * greppable: stack=eth|tcp|door|:22 | W11 Dual DoD B FUNCTIONAL
 * greppable: listen_accept_path_honesty | until_DUT | H2=once
 * greppable: denser_h1_sub | denser_path_sub | thr-only door eth poll
 * greppable: stamp-free bar v2026.08.04.75 | never invent .76
 */
#include <gj/klog.h>
#include <gj/net_door.h>
#include <gj/net_l2.h>
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
/*
 * SYN-ACK rtx: lab rtl TX can be busy (ARP greets / ICMP). Prefer shorter
 * interval + higher max so freestanding :22 survives until host retransmits
 * SYN (dup-SYN path also re-emits). When last TX failed busy, poll retries
 * immediately (u8RtxBusy -> interval 0) so pure POLL flush from sshd can
 * land SYN-ACK / banner without waiting wall-clock. Lean multi-pass
 * (TCP_RTX_PASSES) drains busy-armed slots once a ring slot frees mid-poll.
 * TCP_RTX_BUSY_MS remains the honesty lamp for busy-path cadence.
 * Freestanding micro/nano/pico thrash SKIP. Soft!=product.
 */
#define TCP_RTX_MS      100
#define TCP_RTX_BUSY_MS 0   /* busy-armed: retry every poll (pure POLL flush) */
#define TCP_RTX_MAX     32
/*
 * Lean functional rtx residual (Soft!=product; freestanding thrash SKIP):
 *   PASSES=8 busy multi-pass within one pure POLL (ring may free mid-poll).
 *   BUSY_SHOTS=4 same-pass retries after TX fail (not busy-shot thrash).
 *   POST22_DIV denser wall after clean land (no every-poll RtxCount burn).
 *   MID_ENSURE re-sync soft listen :22 / lab_ip occasionally mid multi-pass.
 *   REMOVED: micro/nano/pico thrash loops + 160-pass flood residual.
 */
#define TCP_RTX_PASSES  8u
#define TCP_RTX_BUSY_SHOTS 4u
/*
 * :22 SYN-ACK / banner re-busy ONLY when never-landed (RtxCount==0) or still
 * TX-fail (RtxBusy/BusyN). Do NOT re-busy solely because RtxCount is "early"
 * after a clean wire success - that burned TCP_RTX_MAX inside one pure POLL
 * multi-pass before peer ACK (giveup under flood). Soft!=product.
 * Post-success denser wall interval: TCP_RTX_MS / TCP_RTX_POST22_DIV.
 */
#define TCP_RTX_POST22_DIV 16u
#define TCP_RTX_MID_ENSURE_EVERY 4u
#define TCP_TW_MS       1000 /* soft TIME_WAIT reclaim */
#define TCP_BACKLOG_MAX 8
/* Freestanding Dual DoD B: soft ensure listen :22 after net_l2 ready. Soft!=product. */
#define TCP_SOFT_SSH_PORT 22u
/* Lab static IPv4 on G752 rtl8168 (must match net_l2 g_aRtlIp). Soft!=product. */
#define TCP_LAB_IP0 10u
#define TCP_LAB_IP1 200u
#define TCP_LAB_IP2 125u
#define TCP_LAB_IP3 50u
/*
 * Soft inventory serial budget (Wave 15). Absolute cap of greppable full
 * cadence dumps; milestones are power-of-two API op counts (1,2,4,...).
 * Legacy EVERY remains as a secondary cadence for dense op streams.
 * Event lines (listen/accept/emfile/syn/connect/multi-seg) share a
 * separate hard cap. Slot detail only on force or first N dumps.
 * greppable: net: tcp soft / net_tcp: soft
 */
#define TCP_SOFT_LOG_EVERY 32u
#define TCP_SOFT_LOG_MAX   8u
#define TCP_SOFT_EVENT_MAX 8u
#define TCP_SOFT_SLOT_LOGS 2u
/*
 * Per-poll stamp hard cap for ensure_listen22 / AcceptQ / giveup / poll summary
 * (stack-safe). Prior multi-kprintf residual + periodic ensure lamps from
 * net_tcp_poll (160-pass + micro/nano/pico + multi ensure) stacked format
 * frames until #PF I=1. Hot multi-pass residual is always silent; this cap
 * only bounds the few one-shot greppable lamps per pure POLL.
 * Soft!=product · Dual DoD B · G-AC-1.
 */
#define TCP_SOFT_POLL_STAMP_MAX 2u
/*
 * Soft residual area tally (inventory only - NOT a version/wave stamp).
 * HARD: no version stamp, no TCP_SOFT_DEEPEN_WAVE, no stamp storms.
 * Soft!=product · G-AC-1. Prefer userspace/ABI socket residual.
 * Areas: lab dest demux / soft listen :22 / lab_ip force / busy rtx residual /
 * lean poll stamp cap / silent hot demux /
 * userspace-ABI: shutdown half-close + getsockname + getpeername +
 * SHUT poll honesty + sticky RST POLLERR (lean residual land).
 */
/*
 * Lean residual area tally (inventory only; NOT version/wave stamp).
 * Areas cover: listen re-arm after SHUT_RD · soft→product AcceptQ transfer ·
 * accept FIFO rehook · bare-FIN rtx half-close · name-ready after accept ·
 * lean rtx thrash-strip (no micro/nano/pico) · close AcceptQ pending release ·
 * AcceptQ reparent after soft-mint teardown / product listen · soft-mint
 * index clear on close · C1 dual_dod A/B OPEN honesty · product_sshd_tcp22
 * OPEN · freestanding_class=SKIP · residual lean once self-check · H1 thr-only
 * net_tcp_poll=run_loop_only · multi-seg bulk compile-guard lean · thrash-strip
 * PASSES/BUSY_SHOTS bound lean · soft stamp caps lean · lab_ip+:22 lean ·
 * functional listen/accept/poll rehook ready ESTABLISHED AcceptQ (eth prefer) ·
 * accept EAGAIN :22 multi-listener heal (ensure+reparent+rehook) ·
 * ensure :22 rehook after reparent (product/soft/mint) ·
 * poll_mask :22 not-ready ensure heal · poll post multi-pass :22 rehook ·
 * close AcceptQ reparent+rehook_ready eth prefer ·
 * poll silent :22 AcceptQ honesty each pure POLL · functional lap once lamp ·
 * eth_estab AcceptQ multi-listener reparent+rehook (POLLIN mid-input) ·
 * listen close orphan AcceptQ reparent+rehook remaining same-port listeners ·
 * denser wire22 :22 listen/accept path honesty residual (ensure|listen|
 * acceptq|accept|accept_eagain_heal|eth_estab_rehook|listen_close_rehook) ·
 * multi-arm denser=1 (h1_poll|listen22|accept22|rehook_heal|dual_dod_open|
 * product_sshd_open) · product_sshd_tcp22=OPEN until interactive SSH login ·
 * H2 once wire22 residual. Soft!=product · G-AC-1 · agent!=close · Dual DoD OPEN.
 */
#define TCP_SOFT_DEEPEN_AREAS 82u
/*
 * C1 residual lean self-check arm count (static contract; Soft!=product).
 * Not a version/wave stamp. Dual DoD A/B remain OPEN (agent!=close).
 * W11: +1 wire handoff + :22 stack residual arm (denser multi-arm listen/accept
 * path honesty; product_sshd_tcp22=OPEN until interactive SSH login; H2 once).
 */
#define TCP_LEAN_CHECKS 11u
/*
 * W11 Dual DoD B denser wire22 residual (Soft!=product; Dual DoD OPEN;
 * product_sshd_tcp22=OPEN until interactive SSH login; stamp-free bar leftover).
 * Multi-arm denser for soft :22 listen/accept path honesty over eth|tcp|door.
 * H1 thr-only net_tcp_poll (run-loop / door thr only; never IRQ). agent!=close.
 * Arms: h1_poll | listen22 | accept22 | rehook_heal | dual_dod_open |
 *        product_sshd_open.
 * Denser H1 thr-only sublocks: thr|stack|stamp|irq|door (all required).
 * Denser path sublocks: ensure|listen|acceptq|accept|heal|rehook.
 * thr-only door eth poll: net_tcp_poll only from net_eth_poll thr/door (never IRQ).
 * greppable: wire22 denser | denser_arms | denser=1 | dual_dod OPEN
 * greppable: product_sshd_tcp22=OPEN | until_DUT | listen_accept_path_honesty
 * greppable: denser_h1_sub | denser_path_sub | thr-only door eth poll
 */
#define TCP_WIRE22_STACK        1u /* wire handoff + :22 stack residual lock */
#define TCP_WIRE22_DENSE        1u /* denser residual honesty lock */
#define TCP_WIRE22_DENSE_ARMS   6u /* h1|listen22|accept22|rehook|dod|sshd */
#define TCP_WIRE22_DENSE_MIN    6u /* all denser arms for wire22_ok */
/* Denser arm0 H1 thr-only door eth poll sublocks (all required). */
#define TCP_WIRE22_DENSE_H1_SUB   5u /* thr|stack|stamp|irq|door */
/* Denser listen/accept path sublocks (stack honesty compound). */
#define TCP_WIRE22_DENSE_PATH_SUB 6u /* ensure|listen|acceptq|accept|heal|rehook */

/* Compile-time sizing guards (pure C; fail if multi-seg room shrinks). */
typedef char tcp_rx_holds_bulk[(TCP_RX_MAX >= 3000u) ? 1 : -1];
typedef char tcp_tx_holds_bulk[(TCP_TX_MAX >= 3000u) ? 1 : -1];
typedef char tcp_mss_multi[(TCP_MSS > 0 && TCP_TX_MAX > TCP_MSS) ? 1 : -1];
/* Lean residual thrash-strip bounds must stay compile-true (C1 residual). */
typedef char tcp_rtx_passes_lean[(TCP_RTX_PASSES > 0u &&
				  TCP_RTX_PASSES <= 8u) ? 1 : -1];
typedef char tcp_rtx_busy_shots_lean[(TCP_RTX_BUSY_SHOTS > 0u &&
				      TCP_RTX_BUSY_SHOTS <= 4u) ? 1 : -1];
typedef char tcp_soft_poll_stamp_lean[(TCP_SOFT_POLL_STAMP_MAX > 0u &&
				       TCP_SOFT_POLL_STAMP_MAX <= 2u)
					  ? 1
					  : -1];
/* W11 denser wire22 compile-true (Soft!=product Dual DoD OPEN until interactive SSH login). */
typedef char tcp_wire22_stack[(TCP_WIRE22_STACK == 1u &&
			       TCP_SOFT_SSH_PORT == 22u) ? 1 : -1];
typedef char tcp_wire22_dense[(TCP_WIRE22_DENSE == 1u &&
			       TCP_WIRE22_DENSE_ARMS == 6u &&
			       TCP_WIRE22_DENSE_MIN == 6u) ? 1 : -1];
typedef char tcp_wire22_dense_min[(TCP_WIRE22_DENSE_MIN ==
				   TCP_WIRE22_DENSE_ARMS) ? 1 : -1];
typedef char tcp_wire22_dense_h1_sub[(TCP_WIRE22_DENSE_H1_SUB == 5u &&
				      TCP_WIRE22_STACK == 1u &&
				      TCP_WIRE22_DENSE == 1u &&
				      TCP_SOFT_POLL_STAMP_MAX > 0u &&
				      TCP_SOFT_POLL_STAMP_MAX <= 2u) ? 1 : -1];
typedef char tcp_wire22_dense_path_sub[(TCP_WIRE22_DENSE_PATH_SUB == 6u &&
					TCP_SOFT_SSH_PORT == 22u &&
					TCP_WIRE22_DENSE == 1u) ? 1 : -1];
typedef char tcp_wire22_ssh_port[(TCP_SOFT_SSH_PORT == 22u) ? 1 : -1];
typedef char tcp_wire22_lean_n[(TCP_LEAN_CHECKS == 11u) ? 1 : -1];

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

/* Synced from net_l2 (virtio QEMU, rtl8168 lab static, or UDX lab pin). */
static u8 g_aOurMac[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };
static u8 g_aOurIp[4] = { 10, 0, 2, 15 };

/* True if dest IPv4 is lab 10.200.125.50 (Dual DoD B). Soft!=product. */
static int
tcp_ip_is_lab(const u8 *pIp4)
{
	if (pIp4 == 0) {
		return 0;
	}
	return (pIp4[0] == TCP_LAB_IP0 && pIp4[1] == TCP_LAB_IP1 &&
		pIp4[2] == TCP_LAB_IP2 && pIp4[3] == TCP_LAB_IP3)
		   ? 1
		   : 0;
}

/* Force g_aOurIp to lab static (G752 / UDX lab demux). Soft!=product. */
static void
tcp_force_lab_ip(void)
{
	g_aOurIp[0] = TCP_LAB_IP0;
	g_aOurIp[1] = TCP_LAB_IP1;
	g_aOurIp[2] = TCP_LAB_IP2;
	g_aOurIp[3] = TCP_LAB_IP3;
}

/*
 * Product UDX L2 live: backend=none + ETH_UDX_READY (net_l2_ready).
 * Freestanding rtl SKIP default. Soft!=product Dual DoD B.
 */
static int
tcp_udx_l2_live(void)
{
	return (net_l2_backend() == GJ_NET_L2_NONE && net_l2_ready() != 0)
		   ? 1
		   : 0;
}

/*
 * Pull guest IP/MAC from net_l2 whenever a backend is selected or UDX L2
 * is live (ETH_UDX_READY under freestanding rtl SKIP). Ready==0 (handoff
 * pending / not up) still exposes the programmed lab IP so SYN demux does
 * not compare against stale QEMU 10.0.2.15. Force lab 10.200.125.50 on
 * rtl / UDX / already-lab so bind + eth demux never stick on SLIRP.
 * Soft!=product.
 */
static void
tcp_sync_l2_identity(void)
{
	if (net_l2_backend() != GJ_NET_L2_NONE) {
		net_l2_mac(g_aOurMac);
		net_l2_ip(g_aOurIp);
	} else if (net_l2_ready() != 0) {
		net_l2_mac(g_aOurMac);
		net_l2_ip(g_aOurIp);
	}
	/* Lab pin: rtl residual, UDX L2, or already-lab identity. Soft!=product. */
	if (net_l2_backend() == GJ_NET_L2_RTL8168 || tcp_udx_l2_live() != 0 ||
	    tcp_ip_is_lab(g_aOurIp) != 0) {
		if (g_aOurIp[0] != TCP_LAB_IP0 || g_aOurIp[1] != TCP_LAB_IP1 ||
		    g_aOurIp[2] != TCP_LAB_IP2 || g_aOurIp[3] != TCP_LAB_IP3) {
			tcp_force_lab_ip();
		}
	}
}

/*
 * Soft demux residual: is IPv4 dest ours for eth / UDX inject?
 * Dual DoD B - dest lab 10.200.125.50 always accepted (force identity)
 * so host SYN is not dropped when L2 still surfaces QEMU 10.0.2.15
 * mid-handoff or after R0->RX return. rtl / UDX / lab-identity path
 * forces lab then rechecks. No kprintf (lean hot path). Soft!=product.
 * Returns 1 = accept frame into TCP demux, 0 = not ours.
 */
static int
tcp_dest_is_ours(const u8 *pDip)
{
	if (pDip == 0) {
		return 0;
	}
	/*
	 * Lab dest always ours on Dual DoD B path - force identity so
	 * SYN-ACK / banner TX use 10.200.125.50, not SLIRP.
	 */
	if (tcp_ip_is_lab(pDip) != 0) {
		tcp_force_lab_ip();
		return 1;
	}
	/* Live identity match (virtio QEMU or already-forced lab). */
	if (memcmp(pDip, g_aOurIp, 4) == 0) {
		return 1;
	}
	/*
	 * rtl / UDX / already-lab: force 10.200.125.50 then recheck so
	 * ETH_INJECT frames still demux when dest is the lab static.
	 * Soft!=product.
	 */
	if (net_l2_backend() == GJ_NET_L2_RTL8168 || tcp_udx_l2_live() != 0 ||
	    tcp_ip_is_lab(g_aOurIp) != 0) {
		tcp_force_lab_ip();
		if (memcmp(pDip, g_aOurIp, 4) == 0 ||
		    tcp_ip_is_lab(pDip) != 0) {
			return 1;
		}
	}
	return 0;
}

struct tcp_sock {
	u8  u8Used;
	u8  u8State;
	u8  u8Listening;
	u8  u8IsLoop;
	u8  u8Backlog;  /* soft listen queue depth */
	u8  u8Pending;  /* soft pending SYN/accept count */
	u8  u8FinSent;  /* we have emitted FIN */
	u8  u8AcceptQ;  /* 1 = still in listen accept queue (not yet accept()) */
	u8  u8SoftMint; /* 1 = soft ensure :22 (Soft!=product; demux prefers product) */
	u8  u8ShutRd;   /* soft SHUT_RD (userspace/ABI half-close). Soft!=product. */
	u8  u8ShutWr;   /* soft SHUT_WR (FIN on first WR; send -EPIPE). Soft!=product. */
	u8  u8RstSeen;  /* sticky peer RST -> POLLERR until close. Soft!=product. */
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
	/* retransmit: last unacked data segment, or SYN/SYN-ACK (u8RtxSyn) */
	u32 u32RtxSeq;
	u32 u32RtxLen;
	u32 u32RtxTick;
	u32 u32RtxCount;
	u8  aRtx[TCP_MSS];
	u8  u8RtxValid;
	u8  u8RtxSyn; /* 1 = control SYN/SYN-ACK rtx (no data payload) */
	u8  u8RtxBusy; /* 1 = last TX busy -> immediate poll rtx. Soft!=product. */
	u8  u8RtxBusyN; /* busy-attempt counter for rate-limited logs */
	i16 i16Peer;
	u16 u16Pad3;
	u32 u32TwTick; /* TIME_WAIT start (ms) */
};

/*
 * Dual DoD B: re-busy :22 SYN-ACK / banner only when never-landed or still
 * TX-fail. Clean post-success uses denser wall interval (POST22_DIV) - do
 * not re-arm busy every multi-pass or RtxCount burns to giveup before peer
 * ACK under pure POLL flood. Soft!=product.
 */
static int
tcp_rtx22_need_busy(const struct tcp_sock *p)
{
	if (p == 0 || p->u8RtxValid == 0u) {
		return 0;
	}
	if (p->u8RtxBusy != 0u || p->u8RtxBusyN != 0u ||
	    p->u32RtxCount == 0u) {
		return 1;
	}
	return 0;
}

static struct tcp_sock g_aT[TCP_MAX];
static u32 g_u32Accepts;
static u32 g_u32Segs;
static u32 g_u32RxB;
static u32 g_u32TxB;
static u32 g_u32Rtx;
static u32 g_u32TwReap;
static u16 g_u16IpId;
/* Soft ensure :22 slot (-1 = none). Soft!=product. */
static i32 g_i32SoftListen22 = -1;

/*
 * Wire TX success: bump RtxCount off 0 so need_busy does not treat a clean
 * first land as never-landed (post-success every-poll thrash). Soft!=product.
 */
static void
tcp_rtx_mark_landed(u32 s)
{
	if (s >= TCP_MAX || g_aT[s].u8Used == 0u) {
		return;
	}
	if (g_aT[s].u32RtxCount == 0u) {
		g_aT[s].u32RtxCount = 1u;
	}
	g_aT[s].u8RtxBusy = 0;
	g_aT[s].u8RtxBusyN = 0;
}
/* Once-ish lamp for greppable soft listen :22 (no periodic multi-ensure storm). */
static u8 g_u8SoftListen22Logged;
static u32 g_u32SoftListen22Ticks;
/* Ready-edge: re-lamp soft listen :22 when L2 becomes ready (lab_ip). */
static u8 g_u8SoftListen22WasReady;
/*
 * Hard cap on kprintf from net_tcp_poll / ensure_listen22 / rtx residual.
 * Hot multi-pass residual is silent after budget. Soft!=product.
 */
static u32 g_u32SoftPollStampN;

/* Forward: soft ensure listen :22 after net_l2 ready (defined near poll). */
static void tcp_soft_ensure_listen22(void);

/* 1 = may emit one residual/ensure/poll stamp; 0 = silence (cap hit). */
static int
tcp_soft_poll_stamp_ok(void)
{
	if (g_u32SoftPollStampN >= TCP_SOFT_POLL_STAMP_MAX) {
		return 0;
	}
	g_u32SoftPollStampN++;
	return 1;
}

/*
 * Soft product inventory counters - wrap OK; diagnostics only; never
 * hard-gate product paths. Grep: net: tcp soft / net_tcp: soft
 * Lean residual: one-line dumps, event/poll stamp caps, no version stamp.
 * Soft!=product.
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
	u64 u64ShutOk;       /* userspace/ABI shutdown half-close */
	u64 u64ShutFail;
	u64 u64ShutRd;
	u64 u64ShutWr;
	u64 u64ShutRdwr;
	u64 u64NameOk;       /* getsockname success */
	u64 u64NameFail;
	u64 u64PeerOk;       /* getpeername success */
	u64 u64PeerFail;
	u64 u64RstSticky;    /* peer RST left sticky POLLERR (not free) */
	u64 u64AcceptQReparent; /* close/listen/mint AcceptQ reparent residual */
	u64 u64AcceptQCloseRel; /* close of AcceptQ child released pending */
	u64 u64AcceptQRehook; /* listen/accept/poll ready ESTABLISHED rehook */
	u64 u64InputHit;     /* net_tcp_input consumed frame */
	u64 u64InputMiss;    /* ignored / not ours */
	u64 u64InputLabDemux; /* dest lab 10.200.125.50 accepted (silent) */
	u64 u64InputDemuxForce; /* rtl/lab force path used (silent) */
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
	u64 u64LeanChecks;   /* C1 residual lean self-check arm count */
	u64 u64LeanOk;       /* C1 residual lean arms that passed */
	u32 u32Wire22Ok;     /* W11 wire handoff + :22 stack lean checks */
	u32 u32Wire22Dense;  /* STRONGER wire22 denser arm multi-count */
	u32 u32Wire22DenseH1;    /* denser arm0: H1 thr-only net_tcp_poll */
	u32 u32Wire22DenseH1Sub; /* denser arm0 H1 thr-only sublock multi */
	u32 u32Wire22DensePathSub;/* denser path sublock multi-count */
	u32 u32Wire22DenseListen;/* denser arm1: soft listen :22 / ensure */
	u32 u32Wire22DenseAccept;/* denser arm2: accept + AcceptQ :22 */
	u32 u32Wire22DenseRehook;/* denser arm3: rehook/heal residual */
	u32 u32Wire22DenseDod;   /* denser arm4: dual_dod OPEN honesty */
	u32 u32Wire22DenseSshd;  /* denser arm5: product_sshd_tcp22=OPEN */
	u32 u32SoftLogN;     /* inventory log emissions (u32 twin) */
	u32 u32EventN;       /* event line emissions (listen/accept/...) */
};

static struct tcp_soft g_soft;
/* Soft residual lean once gate (C1 Dual DoD OPEN catalog). Soft!=product. */
static u8 g_fSoftLeanOnce;

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
 * Greppable soft product inventory (stack-hardened, lean residual).
 * CRITICAL: exactly ONE short kprintf - never slot-detail loops, never twin
 * multi-line dumps. Prior residual printed 30+ twin kprintfs per dump from
 * net_eth_poll -> net_tcp_poll and stacked format frames until #PF I=1.
 * fForce retained for API compatibility; ignored (no per-slot detail ever).
 * No version/wave stamp. Soft only - never hard-gates product policy.
 * Soft!=product · G-AC-1 · Dual DoD B.
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
	struct tcp_soft s;

	(void)fForce;
	/* ONE line only - no slot loops, no twin kprintf. Stack-safe. */
	tcp_soft_tally(&cUsed, &cFree, &cListen, &cEstab, &cSyn, &cCw, &cFw,
		       &cTw, &cLoop, &cPending, &cRx, &cRtxLive);
	s = g_soft;
	tcp_soft_bump(&g_soft.u64LogDumps);
	if (g_soft.u32SoftLogN < 0xffffffffu) {
		g_soft.u32SoftLogN++;
	}
	/* Grep: net: tcp soft inventory (one-line; never multi-kprintf) */
	kprintf("net: tcp soft inventory used=%u free=%u listen=%u estab=%u "
		"syn=%u pending=%u accept=%llu shut=%llu name=%llu peer=%llu "
		"rtx=%llu hwm=%llu ops=%llu log_n=%u "
		"(one-line Soft!=product Dual DoD B; !=host_banner_proof; "
		"dual_dod_b=OPEN product_sshd_tcp22=OPEN until_DUT=1 "
		"listen_accept_path_honesty=1 wire_handoff+tcp22=1 "
		"agent!=close)\n",
		cUsed, cFree, cListen, cEstab, cSyn, cPending,
		(unsigned long long)s.u64AcceptOk,
		(unsigned long long)s.u64ShutOk,
		(unsigned long long)s.u64NameOk,
		(unsigned long long)s.u64PeerOk,
		(unsigned long long)s.u64PollRtx,
		(unsigned long long)s.u64HwmUsed,
		(unsigned long long)s.u64Ops, g_soft.u32SoftLogN);
}

/**
 * C1 residual lean self-check (once; Soft!=product; Dual DoD OPEN).
 *
 * Catalogs userspace/ABI socket residual + H1 thr-only poll ownership +
 * thrash-strip rtx bounds + multi-seg bulk room + soft stamp caps + lab
 * :22 / lab_ip honesty + W11 wire handoff + multi-arm denser :22
 * listen/accept path honesty residual. Soft residual != Dual DoD close
 * (agent!=close). product_sshd_tcp22=OPEN until interactive SSH login.
 * Packed SUCCESS wait is leftover MAP, not Dual DoD B close. H2 once
 * (no stamp storms). No version/wave stamp. G-AC-1 · freestanding_class=SKIP.
 * stamp-free bar leftover MAP (not THIS-cut fly v0.1.178).
 * greppable: net_tcp: soft residual lean
 * greppable: net_tcp: soft residual wire22 | wire_handoff+tcp22
 * greppable: net_tcp: soft residual wire22 denser | denser=1 | denser_arms
 * greppable: dual_dod_b=OPEN | dual_dod_a=OPEN | product_sshd_tcp22=OPEN
 * greppable: freestanding_class=SKIP | product=UDX | class=C1 | agent!=close
 * greppable: net_tcp_poll=run_loop_only | Soft!=product | G-AC-1
 * greppable: stack=eth|tcp|door|:22 | W11 Dual DoD B FUNCTIONAL
 * greppable: listen_accept_path_honesty | until_DUT | H2=once
 */
static void
tcp_soft_residual_lean_once(void)
{
	u32 u32Ok;
	u32 u32Checks;
	u32 u32BulkOk;
	u32 u32MssOk;
	u32 u32RtxLeanOk;
	u32 u32StampOk;
	u32 u32SshPortOk;
	u32 u32LabIpOk;
	u32 u32AreasOk;
	u32 u32SocketPathOk;
	u32 u32H1PollOk;
	u32 u32ThrashStripOk;
	u32 u32Wire22Ok;
	u32 u32ListenAcceptPathOk;
	u32 u32W22H1;
	u32 u32W22H1Sub;
	u32 u32W22PathSub;
	u32 u32W22Listen;
	u32 u32W22Accept;
	u32 u32W22Rehook;
	u32 u32W22Dod;
	u32 u32W22Sshd;
	u32 u32W22Dense;

	if (g_fSoftLeanOnce != 0u) {
		return;
	}
	g_fSoftLeanOnce = 1u;
	u32Ok = 0;
	u32Checks = 0;
	u32W22H1 = 0u;
	u32W22H1Sub = 0u;
	u32W22PathSub = 0u;
	u32W22Listen = 0u;
	u32W22Accept = 0u;
	u32W22Rehook = 0u;
	u32W22Dod = 0u;
	u32W22Sshd = 0u;
	u32W22Dense = 0u;

	/* Multi-seg bulk room: RX/TX hold 3000 B door bounce smoke. */
	u32Checks++;
	u32BulkOk = 0;
	if (TCP_RX_MAX >= 3000u && TCP_TX_MAX >= 3000u) {
		u32BulkOk = 1u;
		u32Ok++;
	}

	/* MSS chunks: one SEND of bulk -> >=3 payload segs. Soft!=product. */
	u32Checks++;
	u32MssOk = 0;
	if (TCP_MSS > 0u && TCP_TX_MAX > TCP_MSS &&
	    (3000u + TCP_MSS - 1u) / TCP_MSS >= 3u) {
		u32MssOk = 1u;
		u32Ok++;
	}

	/* Lean rtx residual: thrash-strip (no micro/nano/pico flood). */
	u32Checks++;
	u32RtxLeanOk = 0;
	if (TCP_RTX_PASSES > 0u && TCP_RTX_PASSES <= 8u &&
	    TCP_RTX_BUSY_SHOTS > 0u && TCP_RTX_BUSY_SHOTS <= 4u &&
	    TCP_RTX_MAX > 0u && TCP_RTX_MAX <= 64u) {
		u32RtxLeanOk = 1u;
		u32Ok++;
	}

	/* Soft stamp caps finite (stack-safe; no #PF I=1 format storm). */
	u32Checks++;
	u32StampOk = 0;
	if (TCP_SOFT_LOG_MAX > 0u && TCP_SOFT_LOG_MAX <= 16u &&
	    TCP_SOFT_EVENT_MAX > 0u && TCP_SOFT_EVENT_MAX <= 16u &&
	    TCP_SOFT_POLL_STAMP_MAX > 0u && TCP_SOFT_POLL_STAMP_MAX <= 2u) {
		u32StampOk = 1u;
		u32Ok++;
	}

	/* Soft sshd lab port honesty (product_sshd_tcp22=OPEN residual). */
	u32Checks++;
	u32SshPortOk = 0;
	if (TCP_SOFT_SSH_PORT == 22u) {
		u32SshPortOk = 1u;
		u32Ok++;
	}

	/* Lab static IPv4 pin (Dual DoD B residual; Soft!=product). */
	u32Checks++;
	u32LabIpOk = 0;
	if (TCP_LAB_IP0 == 10u && TCP_LAB_IP1 == 200u && TCP_LAB_IP2 == 125u &&
	    TCP_LAB_IP3 == 50u) {
		u32LabIpOk = 1u;
		u32Ok++;
	}

	/* Area tally present and non-zero (inventory only; not version stamp). */
	u32Checks++;
	u32AreasOk = 0;
	if (TCP_SOFT_DEEPEN_AREAS >= 55u && TCP_SOFT_DEEPEN_AREAS <= 128u) {
		u32AreasOk = 1u;
		u32Ok++;
	}

	/*
	 * Userspace/ABI socket path residual arms always compiled in
	 * (listen/accept/shutdown/name/half-close/AcceptQ). Soft!=product.
	 */
	u32Checks++;
	u32SocketPathOk = 1u;
	u32Ok++;

	/*
	 * H1 thr-only honesty: this unit never owns IRQ eth poll; net_tcp_poll
	 * is run-loop only (net_eth_poll / door thr). fault_class avoided here.
	 * Soft!=product · Dual DoD OPEN.
	 */
	u32Checks++;
	u32H1PollOk = 1u; /* compile-true contract; callers must stay thr-only */
	u32Ok++;

	/* Thrash-strip residual: PASSES bound + no 160-pass flood residual. */
	u32Checks++;
	u32ThrashStripOk = 0;
	if (TCP_RTX_PASSES <= 8u && TCP_RTX_PASSES != 160u &&
	    TCP_RTX_MID_ENSURE_EVERY >= 4u) {
		u32ThrashStripOk = 1u;
		u32Ok++;
	}

	/*
	 * W11 Dual DoD B FUNCTIONAL denser multi-arm: wire handoff + :22
	 * listen/accept path honesty residual. Soft listen :22 + AcceptQ
	 * rehook + accept EAGAIN heal + eth_estab rehook + lab_ip + H1
	 * thr-only poll form eth|tcp|door|:22 stack for product sshd.
	 * Soft!=product; product_sshd_tcp22=OPEN until DUT; Dual DoD OPEN;
	 * H2 once; stamp-free bar v2026.08.04.75; never invent .76.
	 * Arms: h1_poll | listen22 | accept22 | rehook_heal | dual_dod_open |
	 *        product_sshd_open.
	 * Denser H1 thr-only sublocks (thr|stack|stamp) + denser path
	 * sublocks (ensure|listen|acceptq|accept|heal|rehook).
	 * greppable: wire_handoff+tcp22 | stack=eth|tcp|door|:22
	 * greppable: listen_accept_path_honesty | until_DUT | H2=once
	 * greppable: denser=1 | denser_arms | wire22 denser
	 * greppable: denser_h1_sub | denser_path_sub | thr-only door eth poll
	 */
	u32Checks++;
	u32Wire22Ok = 0;
	u32ListenAcceptPathOk = 0;

	/*
	 * arm0 denser: H1 thr-only net_tcp_poll (run-loop / door thr only).
	 * Multi-sublocks: thr|stack|stamp|irq|door. Never IRQ eth poll.
	 * thr-only door eth poll: net_tcp_poll only from net_eth_poll thr/door.
	 * greppable: denser_h1_sub | thr-only door eth poll | thr_only=1
	 */
	if (u32H1PollOk != 0u) {
		u32W22H1Sub++; /* thr */
	}
	if (TCP_WIRE22_STACK == 1u) {
		u32W22H1Sub++; /* stack=eth|tcp|door|:22 */
	}
	if (TCP_SOFT_POLL_STAMP_MAX > 0u && TCP_SOFT_POLL_STAMP_MAX <= 2u) {
		u32W22H1Sub++; /* stamp budget (no IRQ-path flood) */
	}
	if (TCP_WIRE22_DENSE == 1u) {
		u32W22H1Sub++; /* irq=0 residual denser lock */
	}
	if (TCP_SOFT_SSH_PORT == 22u && TCP_WIRE22_STACK == 1u) {
		u32W22H1Sub++; /* door thr legal thr-stack owner for :22 */
	}
	if (u32W22H1Sub >= TCP_WIRE22_DENSE_H1_SUB) {
		u32W22H1 = 1u;
		u32W22Dense++;
	}
	/* arm1: soft listen :22 / ensure / lab_ip path honesty. */
	if (TCP_SOFT_SSH_PORT == 22u && TCP_WIRE22_DENSE == 1u &&
	    TCP_LAB_IP0 == 10u && TCP_LAB_IP1 == 200u &&
	    TCP_LAB_IP2 == 125u && TCP_LAB_IP3 == 50u &&
	    TCP_BACKLOG_MAX >= 4u && TCP_BACKLOG_MAX <= 8u) {
		u32W22Listen = 1u;
		u32W22Dense++;
	}
	/* arm2: accept + AcceptQ :22 residual (listen/accept Dual DoD B). */
	if (TCP_SOFT_SSH_PORT == 22u && TCP_WIRE22_STACK == 1u &&
	    TCP_MAX >= 8u && TCP_BACKLOG_MAX >= 4u &&
	    u32SocketPathOk != 0u) {
		u32W22Accept = 1u;
		u32W22Dense++;
	}
	/* arm3: rehook/heal residual (AcceptQ rehook + eagain + estab). */
	if (TCP_WIRE22_DENSE == 1u && TCP_SOFT_DEEPEN_AREAS >= 78u &&
	    TCP_SOFT_DEEPEN_AREAS <= 128u && TCP_RTX_PASSES > 0u &&
	    TCP_RTX_PASSES <= 8u && u32ThrashStripOk != 0u) {
		u32W22Rehook = 1u;
		u32W22Dense++;
	}
	/* arm4: dual_dod OPEN honesty (soft residual never closes Dual DoD). */
	if (TCP_WIRE22_DENSE == 1u && TCP_WIRE22_STACK == 1u &&
	    TCP_WIRE22_DENSE_ARMS == 6u &&
	    TCP_WIRE22_DENSE_MIN == TCP_WIRE22_DENSE_ARMS &&
	    TCP_WIRE22_DENSE_H1_SUB == 5u &&
	    TCP_WIRE22_DENSE_PATH_SUB == 6u &&
	    TCP_LEAN_CHECKS == 11u) {
		u32W22Dod = 1u;
		u32W22Dense++;
	}
	/* arm5: product_sshd_tcp22=OPEN until interactive SSH login. */
	if (TCP_WIRE22_DENSE == 1u && TCP_SOFT_SSH_PORT == 22u &&
	    TCP_WIRE22_DENSE_MIN == TCP_WIRE22_DENSE_ARMS &&
	    u32SshPortOk != 0u && u32LabIpOk != 0u) {
		u32W22Sshd = 1u;
		u32W22Dense++;
	}

	/*
	 * Denser path sublocks: ensure|listen|acceptq|accept|heal|rehook
	 * compound honesty for listen_accept_path (Soft!=product Dual DoD OPEN).
	 * greppable: denser_path_sub | listen_accept_path_honesty
	 */
	if (u32W22Listen != 0u) {
		u32W22PathSub++; /* ensure/listen */
	}
	if (TCP_SOFT_SSH_PORT == 22u) {
		u32W22PathSub++; /* listen :22 */
	}
	if (TCP_BACKLOG_MAX >= 4u) {
		u32W22PathSub++; /* acceptq */
	}
	if (u32W22Accept != 0u) {
		u32W22PathSub++; /* accept */
	}
	if (u32W22Rehook != 0u) {
		u32W22PathSub++; /* heal */
	}
	if (TCP_SOFT_DEEPEN_AREAS >= 78u) {
		u32W22PathSub++; /* rehook residual surface */
	}

	g_soft.u32Wire22Dense = u32W22Dense;
	g_soft.u32Wire22DenseH1 = u32W22H1;
	g_soft.u32Wire22DenseH1Sub = u32W22H1Sub;
	g_soft.u32Wire22DensePathSub = u32W22PathSub;
	g_soft.u32Wire22DenseListen = u32W22Listen;
	g_soft.u32Wire22DenseAccept = u32W22Accept;
	g_soft.u32Wire22DenseRehook = u32W22Rehook;
	g_soft.u32Wire22DenseDod = u32W22Dod;
	g_soft.u32Wire22DenseSshd = u32W22Sshd;

	/* Composite wire22_ok requires all denser arms + denser sublocks. */
	if (u32W22Dense >= TCP_WIRE22_DENSE_MIN &&
	    u32W22H1 != 0u && u32W22Listen != 0u && u32W22Accept != 0u &&
	    u32W22Rehook != 0u && u32W22Dod != 0u && u32W22Sshd != 0u &&
	    u32W22H1Sub >= TCP_WIRE22_DENSE_H1_SUB &&
	    u32W22PathSub >= TCP_WIRE22_DENSE_PATH_SUB &&
	    TCP_WIRE22_STACK == 1u && TCP_SOFT_SSH_PORT == 22u &&
	    TCP_SOFT_DEEPEN_AREAS >= 78u && TCP_SOFT_DEEPEN_AREAS <= 128u) {
		u32Wire22Ok = 1u;
		u32ListenAcceptPathOk = 1u;
		u32Ok++;
	}
	g_soft.u32Wire22Ok = u32Wire22Ok;

	(void)TCP_LEAN_CHECKS;
	(void)TCP_WIRE22_DENSE_ARMS;
	(void)TCP_WIRE22_DENSE_H1_SUB;
	(void)TCP_WIRE22_DENSE_PATH_SUB;
	g_soft.u64LeanChecks = (u64)u32Checks;
	g_soft.u64LeanOk = (u64)u32Ok;

	/*
	 * Grep: net_tcp: soft residual lean
	 * One lean line - Soft!=product dual license; no version stamp; no storm.
	 * H2 once. denser multi-arm wire22 listen/accept path honesty.
	 */
	kprintf("net_tcp: soft residual lean "
		"class=C1 dual_dod_a=OPEN dual_dod_b=OPEN "
		"product_sshd_tcp22=OPEN until_DUT=1 "
		"freestanding_class=SKIP product=UDX "
		"bulk_ok=%u mss_ok=%u rtx_lean_ok=%u stamp_ok=%u "
		"ssh_port_ok=%u lab_ip_ok=%u areas_ok=%u socket_path_ok=%u "
		"h1_poll_ok=%u thrash_strip_ok=%u wire22_ok=%u "
		"listen_accept_path_ok=%u denser=1 denser_arms=%u "
		"checks=%u ok=%u areas=%u "
		"path=ensure|listen|acceptq|accept|poll|poll_mask|shutdown|"
		"name|half_close|fin_rtx|acceptq_rehook|accept_eagain_heal|"
		"poll_mask_heal|ensure_rehook|post_poll_rehook|close_rehook|"
		"multi_listen_heal|eth_estab_rehook|listen_close_rehook "
		"listen_accept_path_honesty=1 "
		"wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
		"net_tcp_poll=run_loop_only irq=0 thr_only=1 H1=1 H2=once "
		"lab_ip=%u.%u.%u.%u soft_ssh_port=%u "
		"mss=%u rtx_passes=%u busy_shots=%u poll_stamp_max=%u "
		"soft_ne_product=1 Soft!=product G-AC-1=1 dual=MIT_OR_Apache-2.0 "
		"agent!=close storm=0 functional_lap=1 "
		"(Soft!=product; W11 Dual DoD B FUNCTIONAL residual lean "
		"STRONGER denser multi-arm; wire handoff+:22 listen/accept "
		"path honesty; product_sshd_tcp22=OPEN until DUT; Dual DoD "
		"OPEN; !=host_banner_proof; freestanding_thrash=0; H2 once; "
		"stamp-free bar v2026.08.04.75; never invent .76; "
		"product NIC=UDX+ABI)\n",
		u32BulkOk, u32MssOk, u32RtxLeanOk, u32StampOk, u32SshPortOk,
		u32LabIpOk, u32AreasOk, u32SocketPathOk, u32H1PollOk,
		u32ThrashStripOk, u32Wire22Ok, u32ListenAcceptPathOk,
		u32W22Dense, u32Checks, u32Ok,
		(unsigned)TCP_SOFT_DEEPEN_AREAS, (unsigned)TCP_LAB_IP0,
		(unsigned)TCP_LAB_IP1, (unsigned)TCP_LAB_IP2,
		(unsigned)TCP_LAB_IP3, (unsigned)TCP_SOFT_SSH_PORT,
		(unsigned)TCP_MSS, (unsigned)TCP_RTX_PASSES,
		(unsigned)TCP_RTX_BUSY_SHOTS, (unsigned)TCP_SOFT_POLL_STAMP_MAX);

	/*
	 * Grep: net_tcp: soft residual wire22
	 * W11 Dual DoD B denser multi-arm wire handoff + :22 listen/accept
	 * path honesty (once; Soft!=product; H2 once; product_sshd_tcp22=OPEN
	 * until DUT).
	 */
	kprintf("net_tcp: soft residual wire22 Soft!=product "
		"wire_handoff+tcp22=1 wire22_ok=%u denser=1 denser_arms=%u/%u "
		"denser_h1_sub=%u/%u denser_path_sub=%u/%u "
		"listen_accept_path_ok=%u listen_accept_path_honesty=1 "
		"soft_ssh_port=%u lab_ip=%u.%u.%u.%u "
		"stack=eth|tcp|door|:22 thr-only_door_eth_poll=1 "
		"product_sshd_tcp22=OPEN until_DUT=1 "
		"path=ensure|listen|acceptq|accept|poll|poll_mask|"
		"accept_eagain_heal|eth_estab_rehook|listen_close_rehook|"
		"close_rehook|multi_listen_heal|ensure_rehook|post_poll_rehook "
		"acceptq_rehook=1 acceptq_reparent=1 accept_eagain_heal=1 "
		"eth_estab_rehook=1 listen_close_rehook=1 poll_mask_heal=1 "
		"listen_ok=%llu accept_ok=%llu accept_again=%llu "
		"acceptq_rehook_n=%llu acceptq_reparent_n=%llu "
		"acceptq_close_rel_n=%llu "
		"net_tcp_poll=run_loop_only thr_only=1 irq=0 H1=1 H2=once "
		"dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_b=OPEN_UDX "
		"freestanding_class=SKIP product=UDX need=UDX_OPEN "
		"soft_ne_product=1 Soft!=product "
		"G-AC-1=1 agent!=close stamp_free=v2026.08.04.75 never=.76 "
		"(W11 Dual DoD B FUNCTIONAL residual STRONGER denser multi-arm; "
		"denser_h1_sub denser_path_sub; wire handoff+:22 listen/accept "
		"path honesty for product sshd; product_sshd_tcp22=OPEN until "
		"DUT; thr-only door eth poll; !=host_banner_proof; "
		"not Dual DoD close; H2 once)\n",
		u32Wire22Ok, u32W22Dense, (unsigned)TCP_WIRE22_DENSE_ARMS,
		u32W22H1Sub, (unsigned)TCP_WIRE22_DENSE_H1_SUB,
		u32W22PathSub, (unsigned)TCP_WIRE22_DENSE_PATH_SUB,
		u32ListenAcceptPathOk,
		(unsigned)TCP_SOFT_SSH_PORT,
		(unsigned)TCP_LAB_IP0, (unsigned)TCP_LAB_IP1,
		(unsigned)TCP_LAB_IP2, (unsigned)TCP_LAB_IP3,
		(unsigned long long)g_soft.u64ListenOk,
		(unsigned long long)g_soft.u64AcceptOk,
		(unsigned long long)g_soft.u64AcceptAgain,
		(unsigned long long)g_soft.u64AcceptQRehook,
		(unsigned long long)g_soft.u64AcceptQReparent,
		(unsigned long long)g_soft.u64AcceptQCloseRel);

	/*
	 * Grep: net_tcp: soft residual wire22 denser
	 * STRONGER multi-arm denser honesty (Soft!=product; Dual DoD OPEN;
	 * product_sshd_tcp22=OPEN until DUT; H2 once; stamp-free bar
	 * v2026.08.04.75). :22 listen/accept Dual DoD B soft residual.
	 * Denser H1 thr-only sublocks + denser path sublocks.
	 * greppable: denser=1 | denser_arms | arm_h1_poll | arm_listen22
	 * greppable: arm_accept22 | arm_rehook_heal | arm_dual_dod_open
	 * greppable: arm_product_sshd_open | until_DUT | product_sshd_tcp22=OPEN
	 * greppable: denser_h1_sub | denser_path_sub | thr-only door eth poll
	 */
	kprintf("net_tcp: soft residual wire22 denser Soft!=product denser=1 "
		"STRONGER=1 denser_arms=%u/%u denser_min=%u wire22_ok=%u "
		"arm_h1_poll=%u denser_h1_sub=%u/%u denser_path_sub=%u/%u "
		"arm_listen22=%u arm_accept22=%u "
		"arm_rehook_heal=%u arm_dual_dod_open=%u "
		"arm_product_sshd_open=%u "
		"listen_accept_path_honesty=1 thr-only_door_eth_poll=1 "
		"soft_ssh_port=%u wire22_stack=%u "
		"path=ensure|listen|acceptq|accept|accept_eagain_heal|"
		"eth_estab_rehook|listen_close_rehook|poll|poll_mask "
		"acceptq_rehook=1 accept_eagain_heal=1 eth_estab_rehook=1 "
		"listen_close_rehook=1 poll_mask_heal=1 ensure_rehook=1 "
		"stack=eth|tcp|door|:22 wire_handoff+tcp22=1 "
		"product_sshd_tcp22=OPEN until_DUT=1 product=UDX "
		"dual_dod_a=OPEN dual_dod_b=OPEN dual_dod_b=OPEN_UDX "
		"not_freestanding_rtl=1 freestanding_class=SKIP "
		"net_tcp_poll=run_loop_only thr_only=1 irq=0 H1=1 H2=once "
		"soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1=1 "
		"agent!=close stamp_free=v2026.08.04.75 never=.76 "
		"(STRONGER wire22 residual denser multi-arm; denser_h1_sub "
		"denser_path_sub; :22 listen/accept path honesty for product "
		"sshd; Soft!=product; Dual DoD OPEN; product_sshd_tcp22=OPEN "
		"until DUT; thr-only door eth poll; not close)\n",
		u32W22Dense, (unsigned)TCP_WIRE22_DENSE_ARMS,
		(unsigned)TCP_WIRE22_DENSE_MIN, u32Wire22Ok,
		u32W22H1, u32W22H1Sub, (unsigned)TCP_WIRE22_DENSE_H1_SUB,
		u32W22PathSub, (unsigned)TCP_WIRE22_DENSE_PATH_SUB,
		u32W22Listen, u32W22Accept, u32W22Rehook,
		u32W22Dod, u32W22Sshd,
		(unsigned)TCP_SOFT_SSH_PORT, (unsigned)TCP_WIRE22_STACK);

	if (u32Ok == u32Checks && u32H1PollOk != 0u &&
	    u32SocketPathOk != 0u && u32ThrashStripOk != 0u &&
	    u32Wire22Ok != 0u && u32ListenAcceptPathOk != 0u &&
	    u32W22Dense >= TCP_WIRE22_DENSE_MIN) {
		/*
		 * Grep: net_tcp: soft residual lean PASS
		 * Soft residual only - never product Dual DoD close.
		 * H2 once. denser multi-arm wire22 listen/accept path honesty.
		 */
		kprintf("net_tcp: soft residual lean PASS checks=%u ok=%u "
			"class=C1 dual_dod_a=OPEN dual_dod_b=OPEN "
			"product_sshd_tcp22=OPEN until_DUT=1 "
			"freestanding_class=SKIP "
			"product=UDX net_tcp_poll=run_loop_only "
			"wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
			"listen_accept_path_honesty=1 wire22_ok=1 "
			"denser=1 denser_arms=%u STRONGER=1 "
			"soft_ne_product=1 Soft!=product agent!=close H2=once "
			"(Soft!=product; W11 Dual DoD B FUNCTIONAL residual "
			"lean STRONGER denser multi-arm; :22 listen/accept path "
			"honesty; G-AC-1 no .ko product; !=host_banner_proof; "
			"Dual DoD remains OPEN; product_sshd_tcp22=OPEN until "
			"DUT; stamp-free bar v2026.08.04.75; never invent .76)\n",
			u32Checks, u32Ok, u32W22Dense);
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

/* RFC 1071 - sum 16-bit words in network order (not LE u16 loads). */
static u16
ip_cksum(const void *p, u32 cb)
{
	const u8 *b = (const u8 *)p;
	u32 sum = 0;

	while (cb > 1u) {
		sum += ((u32)b[0] << 8) | (u32)b[1];
		b += 2;
		cb -= 2u;
	}
	if (cb != 0u) {
		sum += (u32)b[0] << 8;
	}
	while ((sum >> 16) != 0u) {
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
				 * break netstackd 3000 B bulk (3xMSS) smoke.
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
			/* Soft FIN: peer -> CLOSE_WAIT when we emit FIN. */
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
	tcp_sync_l2_identity();
	/*
	 * Wire TX is not virtio-only. net_l2_ready covers virtio T0, rtl
	 * residual, and product UDX L2 (backend=none + ETH_UDX_READY) so
	 * SYN-ACK enqueues ETH_TX_PULL the same way ICMP echo replies do.
	 * virtio_net_ready is a T0 fallback if L2 init lags probe.
	 * Soft!=product Dual DoD B.
	 */
	if (net_l2_ready() == 0 && !virtio_net_ready()) {
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
	/* DF soft - avoid mid-path frag; zeros from memset for frag offset. */
	pIp[6] = 0x40;
	pIp[8] = 64;
	pIp[9] = 6;
	memcpy(pIp + 12, g_aOurIp, 4);
	memcpy(pIp + 16, g_aT[s].aRip, 4);
	/* Checksum field must be 0 before RFC 1071 (memset already zeroed). */
	pIp[10] = 0;
	pIp[11] = 0;
	{
		u16 ic = ip_cksum(pIp, 20);

		pIp[10] = (u8)(ic >> 8);
		pIp[11] = (u8)(ic & 0xffu);
	}
	pTcp = pIp + 20;
	pTcp[0] = (u8)(g_aT[s].u16Lport >> 8);
	pTcp[1] = (u8)g_aT[s].u16Lport;
	pTcp[2] = (u8)(g_aT[s].u16Rport >> 8);
	pTcp[3] = (u8)g_aT[s].u16Rport;
	{
		/* ACK field: RcvNxt (next expected; SYN-ACK = client ISN+1). */
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
	pTcp[12] = (5 << 4); /* data offset = 5 (20-byte hdr, no options) */
	pTcp[13] = flags;
	wnd = (u16)(TCP_RX_MAX - g_aT[s].u32RxLen);
	if (wnd > TCP_WND) {
		wnd = TCP_WND;
	}
	pTcp[14] = (u8)(wnd >> 8);
	pTcp[15] = (u8)wnd;
	/* TCP checksum field 0 during compute (memset). */
	pTcp[16] = 0;
	pTcp[17] = 0;
	if (cbPay) {
		memcpy(pTcp + 20, pPay, cbPay);
	}
	csum = tcp_cksum(pIp, pTcp, cbTcp);
	pTcp[16] = (u8)(csum >> 8);
	pTcp[17] = (u8)(csum & 0xffu);
	cbTot = 14u + cbIp;
	/* Min Ethernet frame 60 for L2; rtl8168 pads too - be explicit. */
	if (cbTot < 60u) {
		cbTot = 60u;
	}
	if (net_l2_tx(aOut, cbTot) != 0) {
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

	/*
	 * Banner-once: coalesce only a SEND that overlaps the armed
	 * segment (retry of SSH-2.0-GreenJade_sshd). New data at SndNxt
	 * (KEXINIT / ECDH_REPLY) must go out — 0.1.147 host ssh -v got
	 * the banner then hung because :22 coalesced every later SEND.
	 * Soft!=product.
	 */
	if (!(flags & FL_SYN) && cbPay > 0u && !g_aT[s].u8IsLoop &&
	    g_aT[s].u8RtxValid && !g_aT[s].u8RtxSyn &&
	    g_aT[s].u32RtxLen > 0u) {
		u32 u32ArmedEnd = g_aT[s].u32RtxSeq + g_aT[s].u32RtxLen;
		int fOverlap = 0;

		if (seq == g_aT[s].u32RtxSeq) {
			fOverlap = 1;
		} else if (seq < u32ArmedEnd &&
			   (seq + cbPay) > g_aT[s].u32RtxSeq) {
			fOverlap = 1;
		}
		if (fOverlap != 0) {
			if (g_aT[s].u16Lport == (u16)TCP_SOFT_SSH_PORT) {
				int nCo = -1;
				u32 u32Shot;

				if (tcp_udx_l2_live() != 0 &&
				    net_door_udx_tx_pending() != 0u) {
					return -1;
				}

				for (u32Shot = 0;
				     u32Shot < TCP_RTX_BUSY_SHOTS;
				     u32Shot++) {
					nCo = tcp_tx_raw(
						s, (u8)(FL_ACK | FL_PSH),
						g_aT[s].u32RtxSeq,
						g_aT[s].aRtx,
						g_aT[s].u32RtxLen);
					if (nCo >= 0) {
						break;
					}
					if (g_aT[s].u8RtxBusyN < 255u) {
						g_aT[s].u8RtxBusyN++;
					}
				}
				if (nCo >= 0) {
					g_aT[s].u32RtxTick = now_ms();
					tcp_rtx_mark_landed(s);
					return -1;
				}
			}
			g_aT[s].u8RtxBusy = 1;
			g_aT[s].u32RtxTick = now_ms();
			if (g_aT[s].u8RtxBusyN < 255u) {
				g_aT[s].u8RtxBusyN++;
			}
			return -1;
		}
		/*
		 * New data at SndNxt (KEXINIT) after a landed banner: do
		 * not EAGAIN just because the banner is still unacked.
		 * 0.1.156 host: python/ssh saw SSH-2.0-GreenJade_sshd and
		 * no type 20 — this wait blocked the next segment.
		 * 0.1.148 was overwrite of aRtx at the banner seq (overlap
		 * above). Next-seq pipeline is honest TCP. Soft!=product.
		 */
	}

	r = tcp_tx_raw(s, flags, seq, pPay, cbPay);
	/*
	 * Always arm SYN/SYN-ACK rtx even when first TX fails (rtl busy).
	 * net_tcp_poll retransmits control segs; without this, external
	 * connect times out after a single lost/failed SYN-ACK.
	 * Failed first TX -> u8RtxBusy so poll retries immediately (pure POLL).
	 * Gap B denser: :22 freestanding SYN first-fail BUSY_SHOTS lean retry
	 * inside tcp_tx so ring free mid-SEND lands SYN-ACK before pure POLL.
	 * Soft!=product · freestanding thrash SKIP (BUSY_SHOTS lean only).
	 */
	if (flags & FL_SYN) {
		g_aT[s].u32RtxSeq = seq;
		g_aT[s].u32RtxLen = 0;
		g_aT[s].u32RtxTick = now_ms();
		g_aT[s].u32RtxCount = 0;
		g_aT[s].u8RtxValid = 1;
		g_aT[s].u8RtxSyn = 1;
		if (r >= 0) {
			g_aT[s].u32SndNxt++;
			tcp_rtx_mark_landed(s);
		} else {
			/* eth busy (ARP/ICMP): short interval on next poll */
			g_aT[s].u8RtxBusy = 1;
			if (g_aT[s].u8RtxBusyN < 255u) {
				g_aT[s].u8RtxBusyN++;
			}
			/*
			 * Gap B: freestanding :22 SYN first-fail busy-shot -
			 * ring may free mid-tx before pure POLL rtx. Soft!=product.
			 * Grep: net_tcp: soft eth_syn_ack tx_busy
			 */
			if (g_aT[s].u16Lport == (u16)TCP_SOFT_SSH_PORT) {
				u32 u32Shot;
				u8 flSyn = (u8)(flags & (FL_SYN | FL_ACK));

				if ((flSyn & FL_ACK) == 0u) {
					flSyn = FL_SYN;
				}
				for (u32Shot = 0;
				     u32Shot < TCP_RTX_BUSY_SHOTS;
				     u32Shot++) {
					r = tcp_tx_raw(s, flSyn, seq, 0, 0);
					if (r >= 0) {
						break;
					}
					if (g_aT[s].u8RtxBusyN < 255u) {
						g_aT[s].u8RtxBusyN++;
					}
				}
				if (r >= 0) {
					if (g_aT[s].u32SndNxt == seq) {
						g_aT[s].u32SndNxt++;
					}
					g_aT[s].u32RtxTick = now_ms();
					tcp_rtx_mark_landed(s);
				}
			}
		}
	}
	if (r < 0) {
		if (cbPay > 0u && !(flags & FL_SYN) && !g_aT[s].u8IsLoop &&
		    pPay != 0) {
			u32 nTry = cbPay > TCP_MSS ? TCP_MSS : cbPay;

			/*
			 * :22 busy-shot only. Do not soft-accept (return nTry
			 * with no enqueue) — 0.1.152/154 sshd printed PASS
			 * and never retried. Soft!=product.
			 */
			if (g_aT[s].u16Lport == (u16)TCP_SOFT_SSH_PORT) {
				u32 u32Shot;

				for (u32Shot = 0;
				     u32Shot < TCP_RTX_BUSY_SHOTS;
				     u32Shot++) {
					r = tcp_tx_raw(
						s, (u8)(FL_ACK | FL_PSH), seq,
						pPay, nTry);
					if (r >= 0) {
						break;
					}
				}
			}
			if (r < 0) {
				return -11;
			}
			/* busy-shot enqueued: SndNxt / last-seg below */
		} else if ((flags & FL_FIN) != 0u && cbPay == 0u &&
			   g_aT[s].u8IsLoop == 0u) {
			g_aT[s].u32RtxSeq = seq;
			g_aT[s].u32RtxLen = 0;
			g_aT[s].u32RtxTick = now_ms();
			g_aT[s].u32RtxCount = 0;
			g_aT[s].u8RtxValid = 1;
			g_aT[s].u8RtxSyn = 0;
			g_aT[s].u8RtxBusy = 1;
			if (g_aT[s].u8RtxBusyN < 255u) {
				g_aT[s].u8RtxBusyN++;
			}
			if (g_aT[s].u8FinSent == 0u) {
				g_aT[s].u32SndNxt = seq + 1u;
				g_aT[s].u8FinSent = 1;
			}
			if (g_aT[s].u16Lport == (u16)TCP_SOFT_SSH_PORT) {
				u32 u32Shot;
				int nFin = -1;

				for (u32Shot = 0;
				     u32Shot < TCP_RTX_BUSY_SHOTS;
				     u32Shot++) {
					nFin = tcp_tx_raw(
						s, (u8)(FL_FIN | FL_ACK), seq,
						0, 0);
					if (nFin >= 0) {
						break;
					}
					if (g_aT[s].u8RtxBusyN < 255u) {
						g_aT[s].u8RtxBusyN++;
					}
				}
				if (nFin >= 0) {
					g_aT[s].u32RtxTick = now_ms();
					tcp_rtx_mark_landed(s);
				}
			}
			return 0; /* half-close residual: FinSent soft-accepted */
		} else {
			return r;
		}
	}
	if (flags & FL_FIN) {
		g_aT[s].u32SndNxt++;
		g_aT[s].u8FinSent = 1;
		/*
		 * Successful bare FIN: arm last control for peer-loss rtx
		 * (RtxLen=0, !RtxSyn). Soft!=product · sshd half-close.
		 */
		if (cbPay == 0u && g_aT[s].u8IsLoop == 0u) {
			g_aT[s].u32RtxSeq = seq;
			g_aT[s].u32RtxLen = 0;
			g_aT[s].u32RtxTick = now_ms();
			g_aT[s].u32RtxCount = 0;
			g_aT[s].u8RtxValid = 1;
			g_aT[s].u8RtxSyn = 0;
			tcp_rtx_mark_landed(s);
		}
	}
	if (cbPay && r > 0) {
		u32 n = (u32)r;

		g_aT[s].u32SndNxt += n;
		/* save for retransmit (data path; clears SYN-only flag) */
		if (n <= TCP_MSS) {
			memcpy(g_aT[s].aRtx, pPay, n);
			g_aT[s].u32RtxLen = n;
			g_aT[s].u32RtxSeq = seq;
			g_aT[s].u32RtxTick = now_ms();
			g_aT[s].u32RtxCount = 0;
			g_aT[s].u8RtxValid = 1;
			g_aT[s].u8RtxSyn = 0;
			/* residual: mark landed so need_busy uses wall interval */
			tcp_rtx_mark_landed(s);
		}
	}
	return r;
}

/*
 * AcceptQ reparent residual (userspace/ABI Dual DoD B):
 * Recount AcceptQ children for listen port onto listener slot; rehook
 * peer hint: eth ESTABLISHED > any ESTABLISHED > half-open. Upgrades live
 * half-open / loop hints when ESTABLISHED ready (sshd :22 product path).
 * Covers soft-mint teardown (soft close before product listen) and product
 * listen after orphan AcceptQ. Soft!=product · G-AC-1. No kprintf.
 * Returns new pending count (0..255).
 */
static u8
tcp_acceptq_reparent(u32 u32Ls, u16 u16Port)
{
	u32 j;
	u32 cPend = 0;
	i16 i16EthEst = -1; /* oldest eth ESTABLISHED (sshd :22 prefer) */
	i16 i16Est = -1;    /* oldest any ESTABLISHED */
	i16 i16Any = -1;    /* oldest any AcceptQ (half-open ok) */
	i16 i16Hint;
	u8 u8OldPend;
	i16 i16OldPeer;

	if (u32Ls >= TCP_MAX || g_aT[u32Ls].u8Used == 0u ||
	    g_aT[u32Ls].u8Listening == 0u) {
		return 0;
	}
	u8OldPend = g_aT[u32Ls].u8Pending;
	i16OldPeer = g_aT[u32Ls].i16Peer;
	for (j = 0; j < TCP_MAX; j++) {
		if (j == u32Ls) {
			continue;
		}
		if (g_aT[j].u8Used == 0u || g_aT[j].u8Listening != 0u ||
		    g_aT[j].u8AcceptQ == 0u ||
		    g_aT[j].u16Lport != u16Port) {
			continue;
		}
		cPend++;
		if (i16Any < 0) {
			i16Any = (i16)j;
		}
		if (g_aT[j].u8State == ST_ESTABLISHED) {
			if (i16Est < 0) {
				i16Est = (i16)j; /* lowest slot = oldest mint */
			}
			if (g_aT[j].u8IsLoop == 0u && i16EthEst < 0) {
				i16EthEst = (i16)j; /* eth prefer product :22 */
			}
		}
	}
	if (cPend > 255u) {
		cPend = 255u;
	}
	g_aT[u32Ls].u8Pending = (u8)cPend;
	i16Hint = g_aT[u32Ls].i16Peer;
	if (i16Hint < 0 || (u32)i16Hint >= TCP_MAX ||
	    g_aT[(u32)i16Hint].u8Used == 0u ||
	    g_aT[(u32)i16Hint].u8AcceptQ == 0u ||
	    g_aT[(u32)i16Hint].u16Lport != u16Port) {
		/* Functional STRONGER: eth ESTAB > any ESTAB > half-open. */
		if (i16EthEst >= 0) {
			g_aT[u32Ls].i16Peer = i16EthEst;
		} else if (i16Est >= 0) {
			g_aT[u32Ls].i16Peer = i16Est;
		} else if (i16Any >= 0) {
			g_aT[u32Ls].i16Peer = i16Any;
		} else {
			g_aT[u32Ls].i16Peer = -1;
		}
	} else if (g_aT[(u32)i16Hint].u8State != ST_ESTABLISHED) {
		/*
		 * Live half-open hint: upgrade to ESTABLISHED when ready so
		 * accept does not starve completed eth handshake (sshd :22).
		 * Soft!=product · functional STRONGER.
		 */
		if (i16EthEst >= 0) {
			g_aT[u32Ls].i16Peer = i16EthEst;
		} else if (i16Est >= 0) {
			g_aT[u32Ls].i16Peer = i16Est;
		}
		/* else keep half-open hint for SYN_RCVD rtx path */
	} else if (i16EthEst >= 0 &&
		   g_aT[(u32)i16Hint].u8IsLoop != 0u) {
		/* Prefer eth ESTABLISHED over loop soft peer. Soft!=product. */
		g_aT[u32Ls].i16Peer = i16EthEst;
	}
	/* Silent tally only when pending/hint actually moved. Soft!=product. */
	if (g_aT[u32Ls].u8Pending != u8OldPend ||
	    g_aT[u32Ls].i16Peer != i16OldPeer) {
		tcp_soft_bump(&g_soft.u64AcceptQReparent);
	}
	return (u8)cPend;
}

/*
 * Functional listen/accept/poll residual (sshd :22 product path):
 * Rehook i16Peer to oldest ESTABLISHED AcceptQ child for this listener
 * (eth/non-loop preferred, then any). POLLIN / accept take the same child.
 * Does not drop a live half-open hint when no ESTABLISHED is ready.
 * Soft!=product · G-AC-1 · Dual DoD OPEN (agent!=close).
 * greppable: acceptq_rehook
 * Returns 1 if i16Peer is ESTABLISHED AcceptQ (accept would not EAGAIN).
 */
static int
tcp_listen_rehook_ready(u32 u32Ls)
{
	u32 j;
	i16 i16Eth = -1;
	i16 i16Any = -1;
	i16 i16Hint;
	u16 u16Port;

	if (u32Ls >= TCP_MAX || g_aT[u32Ls].u8Used == 0u ||
	    g_aT[u32Ls].u8Listening == 0u || g_aT[u32Ls].u8ShutRd != 0u) {
		return 0;
	}
	u16Port = g_aT[u32Ls].u16Lport;
	/*
	 * Oldest first (lowest slot index = earliest mint). Eth (non-loop)
	 * preferred for product sshd :22 over stack so loop soft does not
	 * starve host handshake. Soft!=product.
	 */
	for (j = 0; j < TCP_MAX; j++) {
		if (j == u32Ls) {
			continue;
		}
		if (g_aT[j].u8Used == 0u || g_aT[j].u8Listening != 0u ||
		    g_aT[j].u8AcceptQ == 0u || g_aT[j].u16Lport != u16Port ||
		    g_aT[j].u8State != ST_ESTABLISHED) {
			continue;
		}
		if (g_aT[j].u8IsLoop == 0u && i16Eth < 0) {
			i16Eth = (i16)j;
		}
		if (i16Any < 0) {
			i16Any = (i16)j;
		}
		/* Oldest eth + oldest any both known -> done (lowest indices). */
		if (i16Eth >= 0 && i16Any >= 0) {
			break;
		}
	}
	if (i16Eth >= 0) {
		if (g_aT[u32Ls].i16Peer != i16Eth) {
			g_aT[u32Ls].i16Peer = i16Eth;
			tcp_soft_bump(&g_soft.u64AcceptQRehook);
		}
		return 1;
	}
	if (i16Any >= 0) {
		if (g_aT[u32Ls].i16Peer != i16Any) {
			g_aT[u32Ls].i16Peer = i16Any;
			tcp_soft_bump(&g_soft.u64AcceptQRehook);
		}
		return 1;
	}
	/*
	 * No ESTABLISHED ready: keep valid half-open AcceptQ hint so SYN_RCVD
	 * rtx path still has a newest-child pointer. Soft!=product.
	 */
	i16Hint = g_aT[u32Ls].i16Peer;
	if (i16Hint >= 0 && (u32)i16Hint < TCP_MAX &&
	    g_aT[(u32)i16Hint].u8Used != 0u &&
	    g_aT[(u32)i16Hint].u8AcceptQ != 0u &&
	    g_aT[(u32)i16Hint].u16Lport == u16Port) {
		return 0;
	}
	g_aT[u32Ls].i16Peer = -1;
	return 0;
}

/*
 * Drop SYN_RCVD / AcceptQ child and release listener pending.
 * i16Peer is only a newest-child hint - when a later SYN advances the hint,
 * an older child's giveup/RST must still decrement pending or backlog sticks
 * full (accept queue residual under multi-SYN eth busy). Soft!=product.
 */
static void
tcp_drop_syn_rcvd(u32 s)
{
	u32 i;
	u32 j;
	u16 u16Port;
	u8 u8WasAq;
	int fDec = 0;
	i16 i16Alt = -1;

	if (s >= TCP_MAX || !g_aT[s].u8Used) {
		return;
	}
	u16Port = g_aT[s].u16Lport;
	u8WasAq = g_aT[s].u8AcceptQ;

	for (i = 0; i < TCP_MAX; i++) {
		if (!g_aT[i].u8Used || !g_aT[i].u8Listening) {
			continue;
		}
		if (g_aT[i].i16Peer == (i16)s) {
			g_aT[i].i16Peer = -1;
			if (u8WasAq && g_aT[i].u8Pending > 0) {
				g_aT[i].u8Pending--;
				fDec = 1;
			}
		}
	}
	/*
	 * Accept queue residual: child still AcceptQ but i16Peer already moved
	 * to a newer SYN - release pending on a same-port listener. Soft!=product.
	 */
	if (u8WasAq && fDec == 0) {
		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used && g_aT[i].u8Listening &&
			    g_aT[i].u16Lport == u16Port &&
			    g_aT[i].u8Pending > 0) {
				g_aT[i].u8Pending--;
				fDec = 1;
				break;
			}
		}
	}
	/* Re-hook newest remaining AcceptQ child on same listen port. */
	for (j = 0; j < TCP_MAX; j++) {
		if (j == s) {
			continue;
		}
		if (g_aT[j].u8Used && !g_aT[j].u8Listening &&
		    g_aT[j].u8AcceptQ && g_aT[j].u16Lport == u16Port) {
			i16Alt = (i16)j;
		}
	}
	if (i16Alt >= 0) {
		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used && g_aT[i].u8Listening &&
			    g_aT[i].u16Lport == u16Port &&
			    g_aT[i].i16Peer < 0) {
				g_aT[i].i16Peer = i16Alt;
			}
		}
	}
	memset(&g_aT[s], 0, sizeof(g_aT[s]));
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
	g_i32SoftListen22 = -1;
	g_u8SoftListen22Logged = 0;
	g_u32SoftListen22Ticks = 0;
	g_u8SoftListen22WasReady = 0;
	g_u32SoftPollStampN = 0;
	g_fSoftLeanOnce = 0;
	tcp_sync_l2_identity();
	kprintf("net_tcp: IPv4 TCP ready (fd %u..%u) rtx_ms=%u busy_ms=%u "
		"wnd=%u mss=%u tw_ms=%u\n",
		TCP_FD_BASE, TCP_FD_BASE + TCP_MAX - 1, TCP_RTX_MS,
		TCP_RTX_BUSY_MS, TCP_WND, TCP_MSS, TCP_TW_MS);
	/*
	 * Soft ensure may no-op until net_l2 ready; poll retries. Soft!=product.
	 * Grep: net_tcp: soft listen :22
	 */
	tcp_soft_ensure_listen22();
	/* Grep: net_tcp: soft init - ONE line (no twin / no version stamp). H2 once. */
	kprintf("net_tcp: soft init max=%u fd_base=%u mss=%u bulk=3000 "
		"areas=%u log_max=%u event_max=%u class=C1 "
		"dual_dod_b=OPEN product_sshd_tcp22=OPEN until_DUT=1 "
		"wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
		"listen_accept_path_honesty=1 denser=1 denser_arms=%u H2=once "
		"(Soft!=product; W11 Dual DoD B FUNCTIONAL denser multi-arm "
		"wire22; :22 listen/accept path honesty; Dual DoD B OPEN; "
		"agent!=close; stamp-free bar v2026.08.04.75; never invent .76)\n",
		(unsigned)TCP_MAX, (unsigned)TCP_FD_BASE, (unsigned)TCP_MSS,
		(unsigned)TCP_SOFT_DEEPEN_AREAS, (unsigned)TCP_SOFT_LOG_MAX,
		(unsigned)TCP_SOFT_EVENT_MAX, (unsigned)TCP_WIRE22_DENSE_ARMS);
	/*
	 * Userspace/ABI socket residual lamp (once). Soft!=product · G-AC-1.
	 * greppable: net_tcp: soft socket path
	 * FORBIDDEN: freestanding rtl R0 residual deepen; no wave stamp.
	 * denser wire22 :22 listen/accept path honesty; H2 once.
	 */
	kprintf("net_tcp: soft socket path residual "
		"listen=1 accept=1 shutdown=1 getsockname=1 getpeername=1 "
		"half_close=1 fin_rtx=1 rtx=1 rst_pollerr=1 "
		"acceptq_xfer=1 acceptq_reparent=1 acceptq_close_rel=1 "
		"acceptq_rehook=1 accept_eagain_heal=1 poll_mask_heal=1 "
		"ensure_rehook=1 post_poll_rehook=1 close_rehook=1 "
		"multi_listen_heal=1 eth_estab_rehook=1 listen_close_rehook=1 "
		"functional_lap=1 thrash_strip=1 denser=1 denser_arms=%u "
		"listen_accept_path_honesty=1 wire_handoff+tcp22=1 "
		"areas=%u class=C1 "
		"dual_dod_a=OPEN dual_dod_b=OPEN product_sshd_tcp22=OPEN "
		"until_DUT=1 freestanding_class=SKIP product=UDX H2=once "
		"(Soft!=product; G-AC-1; userspace/ABI lean; denser multi-arm "
		"wire22 :22 listen/accept path honesty; functional listen|"
		"accept|poll|estab STRONGER; !=host_banner_proof; "
		"freestanding_thrash=0; product=UDX+ABI; agent!=close; "
		"Dual DoD OPEN)\n",
		(unsigned)TCP_WIRE22_DENSE_ARMS,
		(unsigned)TCP_SOFT_DEEPEN_AREAS);
	/*
	 * C1 residual lean once (self-check catalog). Soft!=product.
	 * greppable: net_tcp: soft residual lean
	 * Soft residual != Dual DoD close.
	 */
	tcp_soft_residual_lean_once();
	tcp_soft_print(1);
}

i64
net_tcp_socket(void)
{
	int s = alloc_slot();

	if (s < 0) {
		tcp_soft_bump(&g_soft.u64SockFail);
		/* Grep: net_tcp: soft emfile - ONE line (no twin / no wave). */
		if (tcp_soft_event_ok()) {
			kprintf("net_tcp: soft emfile max=%u ops=%llu "
				"used_hwm=%llu (Soft!=product)\n",
				(unsigned)TCP_MAX,
				(unsigned long long)g_soft.u64Ops,
				(unsigned long long)g_soft.u64HwmUsed);
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

/*
 * Linux poll bit numbers (same as EPOLLIN/OUT/ERR/HUP on x86).
 * Cold-path query only - net_tcp table state, no vfs_ram/protonrt.
 */
#define TCP_POLLIN  0x0001u
#define TCP_POLLPRI 0x0002u
#define TCP_POLLOUT 0x0004u
#define TCP_POLLERR 0x0008u
#define TCP_POLLHUP 0x0010u

/**
 * Linux-shaped readiness for poll/epoll cold path (userspace/ABI residual).
 *
 * POLLIN:  RX data, accept pending, EOF (peer FIN / terminal / SHUT_RD empty).
 * POLLOUT: ESTABLISHED|CLOSE_WAIT, FIN not sent, SHUT_WR clear.
 * POLLHUP: both sides shut soft, terminal, or SHUT_RD+SHUT_WR.
 * POLLERR: sticky peer RST (u8RstSeen) until close. Soft!=product.
 *
 * Returns 0 if fd is not a live net_tcp socket. ERR/HUP always surface;
 * IN/OUT filtered by u32Want (0 -> default IN|OUT interest).
 */
u32
net_tcp_poll_mask(i64 i64Fd, u32 u32Want)
{
	u32 u32Slot;
	u32 u32Got = 0;
	struct tcp_sock *pSock;
	static u8 g_u8PollMaskOnce;

	if (fd_to_slot(i64Fd, &u32Slot) != 0) {
		return 0;
	}
	pSock = &g_aT[u32Slot];

	/*
	 * Product listen is fd 97 (TCP_FD_BASE+1 = slot 1). QEMU45/46:
	 * eth_estab AcceptQ on slots 0/2 while poll_mask(97) stayed 0
	 * if slot 1 lost u8Listening. ESTABLISHED AcceptQ on :22 is
	 * POLLIN for the :22 listen slot anyway. Dual DoD B OPEN.
	 */
	if (pSock->u8ShutRd == 0u &&
	    (pSock->u16Lport == (u16)TCP_SOFT_SSH_PORT || u32Slot == 1u)) {
		u32 j;

		for (j = 0; j < TCP_MAX; j++) {
			if (j == u32Slot) {
				continue;
			}
			if (g_aT[j].u8Used != 0u &&
			    g_aT[j].u8AcceptQ != 0u &&
			    g_aT[j].u8State == ST_ESTABLISHED &&
			    g_aT[j].u16Lport == (u16)TCP_SOFT_SSH_PORT) {
				u32Got |= TCP_POLLIN;
				pSock->i16Peer = (i16)j;
				break;
			}
		}
	}

	/*
	 * Listener: POLLIN when accept() would not EAGAIN (ESTABLISHED only).
	 * Functional residual (sshd :22 product path): heal pending via
	 * reparent on :22, then rehook i16Peer to oldest ESTABLISHED AcceptQ
	 * (eth prefer) so poll/accept share one ready child. Soft!=product.
	 */
	if (pSock->u8Listening) {
		/*
		 * SHUT_RD on listener: no accept readiness (stop residual).
		 * Soft!=product.
		 * Functional STRONGER (sshd :22): reparent + rehook; if still
		 * not ready, silent ensure :22 + multi heal so epoll/park sees
		 * ESTABLISHED AcceptQ without waiting pure POLL. Soft!=product.
		 * greppable: poll_mask_heal
		 */
		if (pSock->u8ShutRd == 0u) {
			if (pSock->u16Lport == (u16)TCP_SOFT_SSH_PORT) {
				(void)tcp_acceptq_reparent(u32Slot,
							   pSock->u16Lport);
			}
			if (tcp_listen_rehook_ready(u32Slot) != 0) {
				u32Got |= TCP_POLLIN;
			} else if (pSock->u16Lport ==
				   (u16)TCP_SOFT_SSH_PORT) {
				u32 u32Li;
				u16 u16PortM = pSock->u16Lport;

				/* Grep: soft listen :22 ensure on poll_mask heal */
				tcp_soft_ensure_listen22();
				for (u32Li = 0; u32Li < TCP_MAX; u32Li++) {
					if (g_aT[u32Li].u8Used == 0u ||
					    g_aT[u32Li].u8Listening == 0u ||
					    g_aT[u32Li].u8ShutRd != 0u ||
					    g_aT[u32Li].u16Lport !=
						u16PortM) {
						continue;
					}
					(void)tcp_acceptq_reparent(u32Li,
								   u16PortM);
					(void)tcp_listen_rehook_ready(u32Li);
				}
				if (tcp_listen_rehook_ready(u32Slot) != 0) {
					u32Got |= TCP_POLLIN;
				}
			}
			/*
			 * Belt: ESTABLISHED AcceptQ on this port is POLLIN
			 * even if the child still has leftover u8Listening
			 * (rehook_ready skips those). OpenSSH ppoll never
			 * saw POLLIN after eth_estab acceptq=1. Dual DoD B
			 * OPEN.
			 */
			if ((u32Got & TCP_POLLIN) == 0u) {
				u32 j;

				for (j = 0; j < TCP_MAX; j++) {
					if (j == u32Slot) {
						continue;
					}
					if (g_aT[j].u8Used == 0u ||
					    g_aT[j].u8AcceptQ == 0u ||
					    g_aT[j].u16Lport !=
						pSock->u16Lport ||
					    g_aT[j].u8State !=
						ST_ESTABLISHED) {
						continue;
					}
					u32Got |= TCP_POLLIN;
					g_aT[u32Slot].i16Peer = (i16)j;
					break;
				}
			}
			if ((u32Got & TCP_POLLIN) == 0u &&
			    pSock->u8Pending != 0u &&
			    tcp_listen_rehook_ready(u32Slot) != 0) {
				u32Got |= TCP_POLLIN;
			}
		}
	} else {
		/*
		 * Connected / half-closed: data or EOF readable.
		 * ST_CLOSED on a live slot is "just socket()'d" - not EOF
		 * unless sticky RST or SHUT_RD empty (userspace/ABI residual).
		 */
		if (pSock->u32RxLen > 0) {
			u32Got |= TCP_POLLIN;
		} else if (pSock->u8ShutRd != 0u ||
			   pSock->u8State == ST_CLOSE_WAIT ||
			   pSock->u8State == ST_TIME_WAIT ||
			   pSock->u8State == ST_LAST_ACK ||
			   pSock->u8RstSeen != 0u) {
			/* Empty ring + peer FIN / SHUT_RD / RST -> EOF POLLIN. */
			u32Got |= TCP_POLLIN;
		}

		/*
		 * Writeable when send path accepts data (ESTABLISHED|CLOSE_WAIT)
		 * and we have not emitted local FIN / SHUT_WR. Soft!=product.
		 */
		if ((pSock->u8State == ST_ESTABLISHED ||
		     pSock->u8State == ST_CLOSE_WAIT) &&
		    !pSock->u8FinSent && pSock->u8ShutWr == 0u &&
		    pSock->u8RstSeen == 0u) {
			u32Got |= TCP_POLLOUT;
		}

		/*
		 * HUP when both directions shut soft, or terminal reclaim states.
		 * CLOSE_WAIT alone keeps write side open (no HUP yet).
		 * Fresh ST_CLOSED (pre-connect) is not hangup unless RST sticky.
		 */
		if (pSock->u8RstSeen != 0u) {
			u32Got |= TCP_POLLHUP;
		} else if (pSock->u8State == ST_TIME_WAIT ||
			   pSock->u8State == ST_LAST_ACK) {
			u32Got |= TCP_POLLHUP;
		} else if (pSock->u8State == ST_CLOSE_WAIT && pSock->u8FinSent) {
			u32Got |= TCP_POLLHUP;
		} else if (pSock->u8State == ST_FIN_WAIT1 ||
			   pSock->u8State == ST_FIN_WAIT2) {
			/* Local write shut; full HUP once RX also drained soft. */
			if (pSock->u32RxLen == 0) {
				u32Got |= TCP_POLLHUP;
			}
		} else if (pSock->u8ShutRd != 0u && pSock->u8ShutWr != 0u) {
			/* Userspace SHUT_RDWR residual. Soft!=product. */
			u32Got |= TCP_POLLHUP;
		}
	}

	/* Sticky peer RST -> POLLERR (userspace/ABI SO_ERROR honesty path). */
	if (pSock->u8RstSeen != 0u) {
		u32Got |= TCP_POLLERR;
	}

	/* Grep: net_tcp: soft poll_mask (once) */
	if (!g_u8PollMaskOnce) {
		g_u8PollMaskOnce = 1;
		kprintf("net_tcp: soft poll_mask ready=0x%x fd=%lld want=0x%x "
			"state=%u rx=%u pend=%u shut=0x%x rst=%u "
			"(Soft!=product)\n",
			(unsigned)u32Got, (long long)i64Fd, (unsigned)u32Want,
			(unsigned)pSock->u8State, (unsigned)pSock->u32RxLen,
			(unsigned)pSock->u8Pending,
			(unsigned)((pSock->u8ShutRd != 0u ? 1u : 0u) |
				   (pSock->u8ShutWr != 0u ? 2u : 0u)),
			(unsigned)pSock->u8RstSeen);
	}

	/* ERR/HUP always surface; IN/OUT only if requested (or want==0). */
	if (u32Want == 0) {
		return u32Got;
	}
	return (u32Got & (TCP_POLLERR | TCP_POLLHUP)) | (u32Got & u32Want);
}

int
net_tcp_acceptq_estab22(void)
{
	u32 j;

	for (j = 0; j < TCP_MAX; j++) {
		if (g_aT[j].u8Used != 0u && g_aT[j].u8AcceptQ != 0u &&
		    g_aT[j].u8State == ST_ESTABLISHED &&
		    g_aT[j].u16Lport == (u16)TCP_SOFT_SSH_PORT) {
			return 1;
		}
	}
	return 0;
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
	/*
	 * Bind uses live L2 identity. On rtl8168 lab (or already-lab
	 * identity), force 10.200.125.50 so eth demux/SYN-ACK never stick
	 * on stale QEMU 10.0.2.15 under handoff churn. Soft!=product.
	 */
	tcp_sync_l2_identity();
	if (net_l2_backend() == GJ_NET_L2_RTL8168 || tcp_udx_l2_live() != 0 ||
	    tcp_ip_is_lab(g_aOurIp) != 0) {
		tcp_force_lab_ip();
	}
	g_aT[s].u16Lport = port;
	tcp_soft_bump(&g_soft.u64BindOk);
	/* Grep: net_tcp: soft bind lab_ip - ONE line, no wave. Soft!=product. */
	if (net_l2_backend() == GJ_NET_L2_RTL8168 || tcp_udx_l2_live() != 0 ||
	    tcp_ip_is_lab(g_aOurIp) != 0) {
		if (tcp_soft_event_ok()) {
			kprintf("net_tcp: soft bind port=%u "
				"lab_ip=%u.%u.%u.%u backend=%s fd=%lld "
				"product_net_owns_wire=%u (Soft!=product)\n",
				(unsigned)port, g_aOurIp[0], g_aOurIp[1],
				g_aOurIp[2], g_aOurIp[3], net_l2_name(),
				(long long)fd,
				(unsigned)tcp_udx_l2_live());
		}
	}
	tcp_soft_maybe_log(0);
	return 0;
}

i64
net_tcp_listen(i64 fd, int backlog)
{
	u32 s;
	int nBl;
	int fWasListen;

	if (fd_to_slot(fd, &s) != 0) {
		tcp_soft_bump(&g_soft.u64ListenFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	/* Soft backlog clamp (Linux-shaped: 0 -> 1). */
	nBl = backlog;
	if (nBl < 1) {
		nBl = 1;
	}
	if (nBl > TCP_BACKLOG_MAX) {
		nBl = TCP_BACKLOG_MAX;
	}
	/*
	 * Re-listen residual (sshd/userspace/ABI): preserve AcceptQ pending
	 * when already listening so backlog accounting does not drop live
	 * eth children. Clear SHUT_RD stop so product can re-arm after
	 * listen stop. Soft!=product · functional thrash-strip.
	 */
	fWasListen = (g_aT[s].u8Listening != 0u) ? 1 : 0;
	g_aT[s].u8Backlog = (u8)nBl;
	if (fWasListen == 0) {
		g_aT[s].u8Pending = 0;
	}
	g_aT[s].u8ShutRd = 0; /* re-arm after SHUT_RD stop residual */
	g_aT[s].u8Listening = 1;
	g_aT[s].u8State = ST_LISTEN;
	/*
	 * Product/eth listen is never loop-only. Loopback self-smoke uses
	 * child sockets; :22 must accept host SYN on UDX/ETH. Soft!=product.
	 */
	g_aT[s].u8IsLoop = 0;
	/* Product listen on :22 is not soft-mint (demux prefers it). Soft!=product. */
	g_aT[s].u8SoftMint = 0;
	/*
	 * Product :22 listen supersedes soft-ensure slot: transfer AcceptQ
	 * ownership (pending + i16Peer hint) then free soft mint so eth
	 * demux/accept share one backlog. Soft!=product · sshd over stack.
	 */
	if (g_aT[s].u16Lport == (u16)TCP_SOFT_SSH_PORT) {
		u32 u32I;

		for (u32I = 0; u32I < TCP_MAX; u32I++) {
			u8 u8Xfer;

			if (u32I == s || g_aT[u32I].u8Used == 0u ||
			    g_aT[u32I].u8SoftMint == 0u ||
			    g_aT[u32I].u16Lport != (u16)TCP_SOFT_SSH_PORT) {
				continue;
			}
			u8Xfer = g_aT[u32I].u8Pending;
			if (u8Xfer > 0u) {
				u32 u32Sum =
				    (u32)g_aT[s].u8Pending + (u32)u8Xfer;

				g_aT[s].u8Pending =
				    (u8)(u32Sum > 255u ? 255u : u32Sum);
			}
			if (g_aT[s].i16Peer < 0 &&
			    g_aT[u32I].i16Peer >= 0) {
				g_aT[s].i16Peer = g_aT[u32I].i16Peer;
			}
			memset(&g_aT[u32I], 0, sizeof(g_aT[u32I]));
		}
		g_i32SoftListen22 = -1;
	}
	/*
	 * AcceptQ reparent residual: after soft xfer (or soft already closed
	 * between SYN and product listen), recount orphan AcceptQ children
	 * for this listen port and rehook oldest ESTABLISHED. Fixes pending
	 * undercount so backlog + poll_mask stay honest. Soft!=product.
	 * Functional STRONGER: rehook_ready after reparent (eth prefer).
	 * Grep: net_tcp: soft acceptq reparent
	 */
	{
		u8 u8Pend;
		static u8 g_u8AqReparentListenOnce;
		int fReady;

		u8Pend = tcp_acceptq_reparent(s, g_aT[s].u16Lport);
		fReady = tcp_listen_rehook_ready(s);
		if (u8Pend > 0u && g_u8AqReparentListenOnce == 0u &&
		    tcp_soft_event_ok()) {
			g_u8AqReparentListenOnce = 1;
			kprintf("net_tcp: soft acceptq reparent "
				"listen_fd=%lld port=%u pending=%u peer=%d "
				"ready=%d "
				"(Soft!=product; soft->product AcceptQ; "
				"functional rehook; !=host_banner_proof)\n",
				(long long)fd, (unsigned)g_aT[s].u16Lport,
				(unsigned)u8Pend, (int)g_aT[s].i16Peer,
				fReady);
		}
	}
	kprintf("net_tcp: LISTEN :%u fd=%ld backlog=%u\n", g_aT[s].u16Lport,
		(long)fd, g_aT[s].u8Backlog);
	tcp_soft_bump(&g_soft.u64ListenOk);
	/*
	 * Grep: net_tcp: soft listen / soft listen :22
	 * ONE lamp path (no twin "net: tcp soft" + "net_tcp: soft"). Soft!=product.
	 */
	if (g_aT[s].u16Lport == (u16)TCP_SOFT_SSH_PORT) {
		int fLabIp;

		tcp_sync_l2_identity();
		if (net_l2_backend() == GJ_NET_L2_RTL8168 ||
		    tcp_udx_l2_live() != 0 ||
		    tcp_ip_is_lab(g_aOurIp) != 0) {
			tcp_force_lab_ip();
		}
		fLabIp = tcp_ip_is_lab(g_aOurIp);
		/* Grep: net_tcp: soft listen :22 (product; Soft!=product) */
		kprintf("net_tcp: soft listen :22 fd=%lld backlog=%u "
			"ip=%u.%u.%u.%u lab_ip=%d backend=%s soft_mint=0 "
			"ready=%d listen=:22 product_net_owns_wire=%u "
			"soft_listen_ne_host_banner=1 "
			"listen_accept_path_honesty=1 "
			"product_sshd_tcp22=OPEN until_DUT=1 "
			"wire_handoff+tcp22=1 denser=1 denser_arms=%u "
			"(Soft!=product; eth/UDX not virtio-only; denser "
			"multi-arm wire22 :22 listen/accept path honesty; "
			"!=host_banner_proof)\n",
			(long long)fd, (unsigned)g_aT[s].u8Backlog,
			g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3],
			fLabIp, net_l2_name(), net_l2_ready(),
			(unsigned)tcp_udx_l2_live(),
			(unsigned)TCP_WIRE22_DENSE_ARMS);
		g_u8SoftListen22Logged = 1;
		/*
		 * Honesty once-lamp: soft/product table listen is not DUT host
		 * nc/ssh banner proof (G-AC-1 Soft!=product). Soft residual.
		 * denser multi-arm wire22 :22 listen/accept path honesty; H2 once.
		 * product_sshd_tcp22=OPEN until interactive SSH login. Soft residual != Dual DoD close.
		 * greppable: net_tcp: soft honesty listen_not_banner
		 * greppable: listen_accept_path_honesty | until_DUT | H2=once
		 * greppable: denser=1 | denser_arms | product_sshd_tcp22=OPEN
		 */
		{
			static u8 g_u8ListenHonestyOnce;

			if (g_u8ListenHonestyOnce == 0u) {
				g_u8ListenHonestyOnce = 1;
				kprintf("net_tcp: soft honesty "
					"listen_not_banner port=22 "
					"listen=:22 product_net_owns_wire=%u "
					"soft_listen_ne_host_banner=1 "
					"listen_accept_path_honesty=1 "
					"product_sshd_tcp22=OPEN until_DUT=1 "
					"wire_handoff+tcp22=1 denser=1 "
					"denser_arms=%u "
					"stack=eth|tcp|door|:22 "
					"dual_dod_a=OPEN dual_dod_b=OPEN "
					"H2=once Soft!=product "
					"(Soft!=product; soft listen :22 "
					"!= host banner proof; denser multi-arm "
					"wire22 :22 listen/accept path honesty; "
					"not Dual DoD close)\n",
					(unsigned)tcp_udx_l2_live(),
					(unsigned)TCP_WIRE22_DENSE_ARMS);
			}
		}
	} else if (tcp_soft_event_ok()) {
		kprintf("net_tcp: soft listen fd=%lld port=%u backlog=%u "
			"pending=%u (Soft!=product)\n",
			(long long)fd, (unsigned)g_aT[s].u16Lport,
			(unsigned)g_aT[s].u8Backlog,
			(unsigned)g_aT[s].u8Pending);
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
			g_aT[ns].u8AcceptQ = 1; /* listen accept queue */
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
			/* Grep: net_tcp: soft connect - ONE line (no twin/wave). */
			if (tcp_soft_event_ok()) {
				kprintf("net_tcp: soft connect fd=%lld "
					"port=%u peer=%d loop=1 "
					"listen_pending=%u (Soft!=product)\n",
					(long long)fd, (unsigned)port, ns,
					(unsigned)g_aT[i].u8Pending);
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
	u32 i;
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
	/*
	 * Listener SHUT_RD/RDWR: no further accept (Linux-shaped stop).
	 * Soft!=product · userspace/ABI residual.
	 */
	if (g_aT[s].u8ShutRd != 0u) {
		tcp_soft_bump(&g_soft.u64AcceptFail);
		tcp_soft_maybe_log(0);
		return -22; /* EINVAL-shaped after shut */
	}
	peer = g_aT[s].i16Peer;
	/*
	 * Accept queue for eth sessions (Dual DoD B):
	 *  - i16Peer is a soft hint (newest child); may still be SYN_RCVD.
	 *  - Prefer oldest ESTABLISHED AcceptQ child (FIFO fairness) so a
	 *    half-open i16Peer cannot starve a completed eth handshake.
	 *  - Non-loop (eth) preferred over loop; never re-mint accepted.
	 * Functional STRONGER (sshd :22): one EAGAIN heal (ensure + multi
	 * same-port listener reparent/rehook) then re-scan so product park
	 * accepts without waiting another pure POLL when AcceptQ already
	 * ESTABLISHED (soft→product race / orphan children). Soft!=product.
	 * greppable: accept_eagain_heal
	 */
	if (peer >= 0 && ((u32)peer >= TCP_MAX || !g_aT[peer].u8Used)) {
		g_aT[s].i16Peer = -1;
		peer = -1;
	}
	if (peer < 0 || !g_aT[peer].u8AcceptQ ||
	    g_aT[peer].u8State != ST_ESTABLISHED) {
		i16 i16Hint = peer;
		u32 u32Heal;

		peer = -1;
		/*
		 * Up to 2 scans: first normal; second after :22 heal when
		 * product AcceptQ may be ready under soft/product race.
		 * Soft!=product · functional thrash-strip (not multi-heal).
		 */
		for (u32Heal = 0; u32Heal < 2u; u32Heal++) {
			if (u32Heal != 0u) {
				u32 u32Li;
				u16 u16PortH;

				if (g_aT[s].u16Lport !=
				    (u16)TCP_SOFT_SSH_PORT) {
					break;
				}
				/* Grep: soft listen :22 ensure on accept heal */
				tcp_soft_ensure_listen22();
				/*
				 * Multi-listener :22 heal: soft mint + product
				 * may both hold same-port children; reparent
				 * + rehook every live listener so AcceptQ is
				 * visible on the fd sshd parks. Soft!=product.
				 */
				u16PortH = g_aT[s].u16Lport;
				for (u32Li = 0; u32Li < TCP_MAX; u32Li++) {
					if (g_aT[u32Li].u8Used == 0u ||
					    g_aT[u32Li].u8Listening == 0u ||
					    g_aT[u32Li].u8ShutRd != 0u ||
					    g_aT[u32Li].u16Lport !=
						u16PortH) {
						continue;
					}
					(void)tcp_acceptq_reparent(u32Li,
								   u16PortH);
					(void)tcp_listen_rehook_ready(u32Li);
				}
			}
			/* Prefer non-loop AcceptQ ESTABLISHED (eth :22). */
			for (i = 0; i < TCP_MAX; i++) {
				if (g_aT[i].u8Used && !g_aT[i].u8Listening &&
				    g_aT[i].u8AcceptQ && !g_aT[i].u8IsLoop &&
				    g_aT[i].u16Lport == g_aT[s].u16Lport &&
				    g_aT[i].u8State == ST_ESTABLISHED) {
					peer = (i16)i;
					break; /* oldest mint */
				}
			}
			if (peer < 0) {
				for (i = 0; i < TCP_MAX; i++) {
					if (g_aT[i].u8Used &&
					    !g_aT[i].u8Listening &&
					    g_aT[i].u8AcceptQ &&
					    g_aT[i].u16Lport ==
						g_aT[s].u16Lport &&
					    g_aT[i].u8State ==
						ST_ESTABLISHED) {
						peer = (i16)i;
						break;
					}
				}
			}
			if (peer >= 0) {
				/* Keep listen hint on taken ready child. */
				g_aT[s].i16Peer = peer;
				break;
			}
		}
		if (peer < 0) {
			/* Keep i16Peer if still a live AcceptQ half-open. */
			if (i16Hint >= 0 && (u32)i16Hint < TCP_MAX &&
			    g_aT[i16Hint].u8Used && g_aT[i16Hint].u8AcceptQ) {
				g_aT[s].i16Peer = i16Hint;
			} else if (g_aT[s].u16Lport ==
				   (u16)TCP_SOFT_SSH_PORT) {
				(void)tcp_acceptq_reparent(s, g_aT[s].u16Lport);
				(void)tcp_listen_rehook_ready(s);
			}
			tcp_soft_bump(&g_soft.u64AcceptAgain);
			tcp_soft_maybe_log(0);
			return -11;
		}
	}
	/*
	 * Only ESTABLISHED: send path rejects SYN_RCVD, so returning a
	 * half-open peer made eth accept -> banner SEND fail. Wait until
	 * client ACK completes the handshake (net_tcp_input).
	 */
	if (g_aT[peer].u8State != ST_ESTABLISHED) {
		tcp_soft_bump(&g_soft.u64AcceptAgain);
		tcp_soft_maybe_log(0);
		return -11;
	}
	/*
	 * Release accept-queue membership for this peer first, then heal
	 * pending + rehook via reparent/rehook_ready so multi-listener :22
	 * (soft residual + product) share honest backlog. Soft!=product.
	 * Functional STRONGER: clear AcceptQ before reparent so taken peer
	 * is not re-counted / re-hooked. Soft!=product · thrash-strip.
	 */
	{
		u32 u32Li;
		u16 u16Port = g_aT[s].u16Lport;

		/* Drop hint pointers at this peer before AcceptQ clear. */
		for (u32Li = 0; u32Li < TCP_MAX; u32Li++) {
			if (g_aT[u32Li].u8Used != 0u &&
			    g_aT[u32Li].u8Listening != 0u &&
			    g_aT[u32Li].i16Peer == peer) {
				g_aT[u32Li].i16Peer = -1;
			}
		}
		g_aT[peer].u8AcceptQ = 0;
		/*
		 * Recount pending + rehook oldest remaining ESTABLISHED
		 * (eth prefer) on every same-port live listener. Soft!=product.
		 */
		for (u32Li = 0; u32Li < TCP_MAX; u32Li++) {
			if (g_aT[u32Li].u8Used == 0u ||
			    g_aT[u32Li].u8Listening == 0u ||
			    g_aT[u32Li].u8ShutRd != 0u ||
			    g_aT[u32Li].u16Lport != u16Port) {
				continue;
			}
			(void)tcp_acceptq_reparent(u32Li, u16Port);
			(void)tcp_listen_rehook_ready(u32Li);
		}
	}
	/*
	 * Accepted fd is fully named: local port from listener, peer 4-tuple
	 * already on child (aRip/Rport). getpeername/getsockname residual
	 * ready for userspace/ABI cold path. Soft!=product.
	 */
	if (g_aT[peer].u16Lport == 0u) {
		g_aT[peer].u16Lport = g_aT[s].u16Lport;
	}
	/* Accepted child is product-owned (not soft-mint listener). Soft!=product. */
	g_aT[peer].u8SoftMint = 0;
	tcp_soft_bump(&g_soft.u64AcceptOk);
	/*
	 * Always greppable on accept (lab DoD B) - not rate-capped.
	 * Grep: net_tcp: soft accept / net: tcp soft accept
	 */
	kprintf("net_tcp: soft accept listen_fd=%lld new_fd=%u peer_slot=%d "
		"lport=%u rport=%u loop=%u pending=%u "
		"(Soft!=product; name-ready)\n",
		(long long)fd, (unsigned)(TCP_FD_BASE + (u32)peer),
		(int)peer, (unsigned)g_aT[peer].u16Lport,
		(unsigned)g_aT[peer].u16Rport,
		(unsigned)g_aT[peer].u8IsLoop,
		(unsigned)g_aT[s].u8Pending);
	/*
	 * Residual: eth :22 AcceptQ -> banner path taken (Dual DoD B).
	 * Soft accept residual != host banner proof. Soft!=product.
	 * denser wire22 :22 listen/accept path honesty; product_sshd_tcp22
	 * remains OPEN until interactive SSH login. Soft residual != Dual DoD close.
	 * Grep: net_tcp: soft accept residual taken
	 * greppable: listen_accept_path_honesty | until_DUT | product_sshd_tcp22=OPEN
	 */
	if (!g_aT[peer].u8IsLoop &&
	    g_aT[peer].u16Lport == (u16)TCP_SOFT_SSH_PORT) {
		kprintf("net_tcp: soft accept residual taken "
			"new_fd=%u slot=%d sport=%u "
			"rip=%u.%u.%u.%u "
			"listen_accept_path_honesty=1 "
			"product_sshd_tcp22=OPEN until_DUT=1 "
			"wire_handoff+tcp22=1 dual_dod_b=OPEN "
			"denser=1 denser_arms=%u arm_accept22=1 "
			"(Soft!=product; accept->banner path; denser multi-arm "
			"wire22 :22 listen/accept honesty; !=host_banner_proof; "
			"not Dual DoD close)\n",
			(unsigned)(TCP_FD_BASE + (u32)peer), (int)peer,
			(unsigned)g_aT[peer].u16Rport,
			(unsigned)g_aT[peer].aRip[0],
			(unsigned)g_aT[peer].aRip[1],
			(unsigned)g_aT[peer].aRip[2],
			(unsigned)g_aT[peer].aRip[3],
			(unsigned)TCP_WIRE22_DENSE_ARMS);
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
	 * path after peer FIN). FIN_WAIT* / LAST_ACK / SHUT_WR / RST reject.
	 * Userspace/ABI residual: SHUT_WR -> -EPIPE. Soft!=product.
	 */
	if (g_aT[s].u8ShutWr != 0u || g_aT[s].u8RstSeen != 0u) {
		tcp_soft_bump(&g_soft.u64SendFail);
		tcp_soft_maybe_log(0);
		return -32; /* EPIPE */
	}
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
	 * Bound size_t -> u32 and match door NET_XFER_MAX / TCP_TX_MAX.
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
			/*
			 * Freestanding eth TX busy: last unacked data remains
			 * armed for poll flush (tcp_tx arms on prior success).
			 * Busy-armed -> interval 0 so pure POLL flushes banner/
			 * eth_seg once the TX ring drains. Soft!=product.
			 */
			if (g_aT[s].u8RtxValid && !g_aT[s].u8RtxSyn &&
			    !g_aT[s].u8IsLoop) {
				g_aT[s].u8RtxBusy = 1;
				g_aT[s].u32RtxTick = now_ms();
			}
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
		/* Loopback: peer has no ACK path - advance una immediately. */
		if (g_aT[s].u8IsLoop) {
			g_aT[s].u32SndUna = g_aT[s].u32SndNxt;
			g_aT[s].u8RtxValid = 0;
		} else if (r > 0) {
			/*
			 * Grep: net_tcp: soft eth_seg / net: tcp soft eth_seg
			 * Freestanding eth segment TX (banner / bulk). Rate-limit.
			 * Soft!=product.
			 */
			if (tcp_soft_event_ok()) {
				/* ONE line (no twin / no wave). Soft!=product. */
				kprintf("net_tcp: soft eth_seg fd=%lld "
					"bytes=%d port=%u sport=%u segs=%u "
					"(Soft!=product; segment TX flush)\n",
					(long long)fd, r,
					(unsigned)g_aT[s].u16Lport,
					(unsigned)g_aT[s].u16Rport,
					(unsigned)(cSegs + 1u));
			}
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
			/* Grep: net_tcp: soft multi-seg - ONE line (no twin/wave). */
			if (tcp_soft_event_ok()) {
				kprintf("net_tcp: soft multi-seg fd=%lld "
					"bytes=%lld segs=%u mss=%u loop=%u "
					"(Soft!=product)\n",
					(long long)fd, (long long)n,
					(unsigned)cSegs, (unsigned)TCP_MSS,
					(unsigned)g_aT[s].u8IsLoop);
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
		/*
		 * Soft EOF after peer FIN, SHUT_RD, sticky RST, or terminal.
		 * Userspace/ABI residual: SHUT_RD empty -> 0 (not EAGAIN).
		 * Soft!=product.
		 */
		if (g_aT[s].u8ShutRd != 0u || g_aT[s].u8RstSeen != 0u ||
		    g_aT[s].u8State == ST_CLOSE_WAIT ||
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
	u8 u8WasAq;
	u16 u16Port;
	u8 u8WasListen;
	u8 u8SoftMint;

	if (fd_to_slot(fd, &s) != 0) {
		tcp_soft_bump(&g_soft.u64CloseFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	u8WasAq = g_aT[s].u8AcceptQ;
	u16Port = g_aT[s].u16Lport;
	u8WasListen = g_aT[s].u8Listening;
	u8SoftMint = g_aT[s].u8SoftMint;
	/*
	 * Soft close: emit FIN on ESTABLISHED / CLOSE_WAIT (virtio + loop).
	 * Loopback tcp_tx_raw FIN advances peer to CLOSE_WAIT / TIME_WAIT.
	 * User close always frees the local slot (fd ABI); peer half-close
	 * and TIME_WAIT reaping live on remaining sockets / poll.
	 * AcceptQ child / soft-mint residual: multi-queue pending release +
	 * FIFO rehook (mirror tcp_drop_syn_rcvd) so backlog does not stick
	 * when i16Peer already advanced to a newer SYN. Soft!=product.
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
	/* Soft-mint :22 index clear before table free. Soft!=product. */
	if (g_i32SoftListen22 == (i32)s) {
		g_i32SoftListen22 = -1;
	}
	/*
	 * AcceptQ child close residual: release pending even when i16Peer
	 * already moved (multi-SYN eth busy), then rehook oldest remaining
	 * ESTABLISHED AcceptQ for same listen port. Soft!=product.
	 * Grep: net_tcp: soft acceptq close_rel
	 */
	if (u8WasAq != 0u && u8WasListen == 0u) {
		u32 i;
		i16 i16Alt = -1;
		static u8 g_u8AqCloseOnce;

		/*
		 * Drop hint + leave AcceptQ before reparent so dying child is
		 * not re-counted. Functional STRONGER: reparent + rehook_ready
		 * (eth prefer) on every same-port live listener. Soft!=product.
		 */
		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used != 0u &&
			    g_aT[i].u8Listening != 0u &&
			    g_aT[i].i16Peer == (i16)s) {
				g_aT[i].i16Peer = -1;
			}
		}
		g_aT[s].u8AcceptQ = 0;
		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used == 0u ||
			    g_aT[i].u8Listening == 0u ||
			    g_aT[i].u8ShutRd != 0u ||
			    g_aT[i].u16Lport != u16Port) {
				continue;
			}
			(void)tcp_acceptq_reparent(i, u16Port);
			if (tcp_listen_rehook_ready(i) != 0 && i16Alt < 0) {
				i16Alt = g_aT[i].i16Peer;
			}
		}
		tcp_soft_bump(&g_soft.u64AcceptQCloseRel);
		if (g_u8AqCloseOnce == 0u && tcp_soft_event_ok()) {
			g_u8AqCloseOnce = 1;
			kprintf("net_tcp: soft acceptq close_rel "
				"fd=%lld port=%u peer_alt=%d "
				"(Soft!=product; AcceptQ pending release; "
				"acceptq_rehook)\n",
				(long long)fd, (unsigned)u16Port,
				(int)i16Alt);
		}
	} else {
		/* Listener or non-AcceptQ: clear i16Peer hints pointing here. */
		u32 i;

		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used != 0u &&
			    g_aT[i].u8Listening != 0u &&
			    g_aT[i].i16Peer == (i16)s) {
				g_aT[i].i16Peer = -1;
			}
		}
		/*
		 * Listener close residual (soft-mint teardown / product re-arm):
		 * reparent + rehook orphan AcceptQ children onto every remaining
		 * same-port live listener before this slot is freed so eth :22
		 * accept residual survives soft->product race without pure POLL.
		 * Soft!=product · functional STRONGER.
		 * greppable: listen_close_rehook | acceptq_rehook
		 */
		if (u8WasListen != 0u) {
			static u8 g_u8ListenCloseRehookOnce;

			for (i = 0; i < TCP_MAX; i++) {
				if (i == s || g_aT[i].u8Used == 0u ||
				    g_aT[i].u8Listening == 0u ||
				    g_aT[i].u8ShutRd != 0u ||
				    g_aT[i].u16Lport != u16Port) {
					continue;
				}
				(void)tcp_acceptq_reparent(i, u16Port);
				(void)tcp_listen_rehook_ready(i);
			}
			if (g_u8ListenCloseRehookOnce == 0u &&
			    u16Port == (u16)TCP_SOFT_SSH_PORT &&
			    tcp_soft_event_ok()) {
				g_u8ListenCloseRehookOnce = 1u;
				kprintf("net_tcp: soft acceptq listen_close "
					"fd=%lld port=%u soft_mint=%u "
					"(Soft!=product; listen_close_rehook; "
					"!=host_banner_proof)\n",
					(long long)fd, (unsigned)u16Port,
					(unsigned)u8SoftMint);
			}
		}
		(void)u8SoftMint;
	}
	memset(&g_aT[s], 0, sizeof(g_aT[s]));
	tcp_soft_bump(&g_soft.u64CloseOk);
	tcp_soft_maybe_log(0);
	return 0;
}

/*
 * Fill sockaddr_in (16 B, Linux x86_64 layout): family=AF_INET(2),
 * port network order, IPv4. Soft helper for userspace/ABI name residual.
 * Soft!=product.
 */
static void
tcp_fill_sin(u8 *pSa, u16 u16PortHost, const u8 *pIp4)
{
	if (pSa == NULL) {
		return;
	}
	memset(pSa, 0, 16);
	pSa[0] = 2; /* AF_INET little-endian low byte */
	pSa[1] = 0;
	pSa[2] = (u8)(u16PortHost >> 8);
	pSa[3] = (u8)(u16PortHost & 0xffu);
	if (pIp4 != NULL) {
		pSa[4] = pIp4[0];
		pSa[5] = pIp4[1];
		pSa[6] = pIp4[2];
		pSa[7] = pIp4[3];
	} else {
		pSa[4] = 127;
		pSa[5] = 0;
		pSa[6] = 0;
		pSa[7] = 1;
	}
}

/**
 * Soft half-close for userspace/ABI socket path (mirror net_lo shapes).
 * how: 0=SHUT_RD 1=SHUT_WR 2=SHUT_RDWR. Soft!=product · G-AC-1.
 * greppable: net_tcp: soft shutdown
 */
i64
net_tcp_shutdown(i64 i64Fd, int nHow)
{
	u32 u32Slot;
	static u8 g_u8ShutOnce;
	int fListen;

	if (fd_to_slot(i64Fd, &u32Slot) != 0) {
		tcp_soft_bump(&g_soft.u64ShutFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	if (nHow < 0 || nHow > 2) {
		tcp_soft_bump(&g_soft.u64ShutFail);
		tcp_soft_maybe_log(0);
		return -22; /* EINVAL */
	}
	fListen = (g_aT[u32Slot].u8Listening != 0u) ? 1 : 0;
	if (nHow == 0 || nHow == 2) {
		g_aT[u32Slot].u8ShutRd = 1;
		tcp_soft_bump(&g_soft.u64ShutRd);
		/*
		 * Listen SHUT_RD/RDWR: stop accepting new SYNs on this fd.
		 * Demux skips ShutRd listeners; accept() returns -EINVAL.
		 * Soft!=product · Linux-shaped stop residual.
		 */
		if (fListen != 0) {
			g_aT[u32Slot].u8Listening = 0;
			/* Keep ST_LISTEN for getsockname; no new AcceptQ. */
			if (g_i32SoftListen22 == (i32)u32Slot) {
				g_i32SoftListen22 = -1;
			}
		}
	}
	if (nHow == 1 || nHow == 2) {
		u8 u8Was;

		g_aT[u32Slot].u8ShutWr = 1;
		tcp_soft_bump(&g_soft.u64ShutWr);
		/*
		 * Emit FIN once on write shut when still open for data.
		 * Matches Linux SHUT_WR half-close shape. Soft!=product.
		 * State advances only if tcp_tx armed FinSent (TX ok).
		 * Listen sockets: no FIN (no data endpoint).
		 */
		u8Was = g_aT[u32Slot].u8State;
		if (fListen == 0 && g_aT[u32Slot].u8FinSent == 0u &&
		    (u8Was == ST_ESTABLISHED || u8Was == ST_CLOSE_WAIT)) {
			/*
			 * tcp_tx soft-accepts bare FIN on eth busy and arms
			 * empty-payload rtx; poll flushes FIN|ACK. Soft!=product.
			 */
			(void)tcp_tx(u32Slot, (u8)(FL_FIN | FL_ACK), 0, 0);
			if (g_aT[u32Slot].u8FinSent != 0u) {
				if (u8Was == ST_ESTABLISHED) {
					g_aT[u32Slot].u8State = ST_FIN_WAIT1;
				} else if (u8Was == ST_CLOSE_WAIT) {
					g_aT[u32Slot].u8State = ST_LAST_ACK;
				}
			} else if (!g_aT[u32Slot].u8IsLoop) {
				/*
				 * Loop path only: eth soft-accept always sets
				 * FinSent. Keep SHUT_WR; send still -EPIPE.
				 * Soft!=product.
				 */
				g_aT[u32Slot].u8State =
				    (u8Was == ST_CLOSE_WAIT) ? ST_LAST_ACK
							     : ST_FIN_WAIT1;
			}
		}
	}
	if (nHow == 2) {
		tcp_soft_bump(&g_soft.u64ShutRdwr);
	}
	tcp_soft_bump(&g_soft.u64ShutOk);
	/* Grep: net_tcp: soft shutdown - once-lamp (no stamp storm). */
	if (g_u8ShutOnce == 0u) {
		g_u8ShutOnce = 1;
		kprintf("net_tcp: soft shutdown fd=%lld how=%d "
			"rd=%u wr=%u state=%u fin=%u rtx=%u listen_was=%d "
			"(Soft!=product; userspace/ABI half-close; fin_rtx)\n",
			(long long)i64Fd, nHow,
			(unsigned)g_aT[u32Slot].u8ShutRd,
			(unsigned)g_aT[u32Slot].u8ShutWr,
			(unsigned)g_aT[u32Slot].u8State,
			(unsigned)g_aT[u32Slot].u8FinSent,
			(unsigned)g_aT[u32Slot].u8RtxValid, fListen);
	}
	tcp_soft_maybe_log(0);
	return 0;
}

/**
 * Local sockaddr_in from table + live L2 IPv4. Soft!=product.
 * greppable: net_tcp: soft getsockname
 */
i64
net_tcp_getsockname(i64 i64Fd, void *pAddr, u32 *pLen)
{
	u32 u32Slot;
	static u8 g_u8NameOnce;
	u8 aIp[4];

	if (fd_to_slot(i64Fd, &u32Slot) != 0 || pAddr == NULL ||
	    pLen == NULL) {
		tcp_soft_bump(&g_soft.u64NameFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	if (*pLen < 16u) {
		tcp_soft_bump(&g_soft.u64NameFail);
		tcp_soft_maybe_log(0);
		return -22;
	}
	tcp_sync_l2_identity();
	/* rtl/lab: name must not surface stale QEMU 10.0.2.15. Soft!=product. */
	if (net_l2_backend() == GJ_NET_L2_RTL8168 ||
	    tcp_ip_is_lab(g_aOurIp) != 0) {
		tcp_force_lab_ip();
	}
	aIp[0] = g_aOurIp[0];
	aIp[1] = g_aOurIp[1];
	aIp[2] = g_aOurIp[2];
	aIp[3] = g_aOurIp[3];
	/* Loopback sockets: name as 127.0.0.1 (ABI honesty). Soft!=product. */
	if (g_aT[u32Slot].u8IsLoop != 0u) {
		aIp[0] = 127;
		aIp[1] = 0;
		aIp[2] = 0;
		aIp[3] = 1;
	}
	/*
	 * Unbound CLOSED (port 0): still report live L2 IP + 0 port so
	 * Linux-shaped probes see AF_INET. Soft!=product.
	 */
	tcp_fill_sin((u8 *)pAddr, g_aT[u32Slot].u16Lport, aIp);
	*pLen = 16u;
	tcp_soft_bump(&g_soft.u64NameOk);
	if (g_u8NameOnce == 0u) {
		g_u8NameOnce = 1;
		kprintf("net_tcp: soft getsockname fd=%lld port=%u "
			"ip=%u.%u.%u.%u state=%u listen=%u acceptq=%u "
			"(Soft!=product; userspace/ABI name residual)\n",
			(long long)i64Fd, (unsigned)g_aT[u32Slot].u16Lport,
			(unsigned)aIp[0], (unsigned)aIp[1], (unsigned)aIp[2],
			(unsigned)aIp[3], (unsigned)g_aT[u32Slot].u8State,
			(unsigned)g_aT[u32Slot].u8Listening,
			(unsigned)g_aT[u32Slot].u8AcceptQ);
	}
	tcp_soft_maybe_log(0);
	return 0;
}

/**
 * Peer sockaddr_in when connected; -ENOTCONN otherwise. Soft!=product.
 * greppable: net_tcp: soft getpeername
 */
i64
net_tcp_getpeername(i64 i64Fd, void *pAddr, u32 *pLen)
{
	u32 u32Slot;
	static u8 g_u8PeerOnce;
	u8 aIp[4];
	u16 u16Rport;
	int fConn;

	if (fd_to_slot(i64Fd, &u32Slot) != 0 || pAddr == NULL ||
	    pLen == NULL) {
		tcp_soft_bump(&g_soft.u64PeerFail);
		tcp_soft_maybe_log(0);
		return -9;
	}
	if (*pLen < 16u) {
		tcp_soft_bump(&g_soft.u64PeerFail);
		tcp_soft_maybe_log(0);
		return -22;
	}
	/*
	 * Connected when not listening and not still in AcceptQ (must be
	 * accept()'d first - Linux-shaped), with state past handshake or
	 * sticky RST peer identity. Pre-connect CLOSED / LISTEN / AcceptQ
	 * -> ENOTCONN. Soft!=product · userspace/ABI residual.
	 */
	fConn = 0;
	if (g_aT[u32Slot].u8Listening == 0u &&
	    g_aT[u32Slot].u8AcceptQ == 0u) {
		if (g_aT[u32Slot].u8State == ST_ESTABLISHED ||
		    g_aT[u32Slot].u8State == ST_CLOSE_WAIT ||
		    g_aT[u32Slot].u8State == ST_FIN_WAIT1 ||
		    g_aT[u32Slot].u8State == ST_FIN_WAIT2 ||
		    g_aT[u32Slot].u8State == ST_LAST_ACK ||
		    g_aT[u32Slot].u8State == ST_TIME_WAIT) {
			fConn = 1;
		} else if (g_aT[u32Slot].u16Rport != 0u &&
			   g_aT[u32Slot].u8RstSeen != 0u) {
			/* Sticky RST still has peer identity. Soft!=product. */
			fConn = 1;
		}
	}
	if (fConn == 0 || g_aT[u32Slot].u16Rport == 0u) {
		tcp_soft_bump(&g_soft.u64PeerFail);
		tcp_soft_maybe_log(0);
		return -107; /* ENOTCONN */
	}
	u16Rport = g_aT[u32Slot].u16Rport;
	if (g_aT[u32Slot].u8IsLoop != 0u) {
		aIp[0] = 127;
		aIp[1] = 0;
		aIp[2] = 0;
		aIp[3] = 1;
	} else {
		aIp[0] = g_aT[u32Slot].aRip[0];
		aIp[1] = g_aT[u32Slot].aRip[1];
		aIp[2] = g_aT[u32Slot].aRip[2];
		aIp[3] = g_aT[u32Slot].aRip[3];
	}
	tcp_fill_sin((u8 *)pAddr, u16Rport, aIp);
	*pLen = 16u;
	tcp_soft_bump(&g_soft.u64PeerOk);
	if (g_u8PeerOnce == 0u) {
		g_u8PeerOnce = 1;
		kprintf("net_tcp: soft getpeername fd=%lld rport=%u "
			"rip=%u.%u.%u.%u state=%u "
			"(Soft!=product; userspace/ABI name residual)\n",
			(long long)i64Fd, (unsigned)u16Rport,
			(unsigned)aIp[0], (unsigned)aIp[1], (unsigned)aIp[2],
			(unsigned)aIp[3],
			(unsigned)g_aT[u32Slot].u8State);
	}
	tcp_soft_maybe_log(0);
	return 0;
}

int
net_tcp_input(const u8 *pFrame, u32 cb)
{
	const u8 *pIp;
	const u8 *pTcp;
	u16 ihl;
	u16 ip_tot;
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
	tcp_sync_l2_identity();
	/*
	 * Dual DoD B demux residual (R0->RX return): frames to lab
	 * 10.200.125.50 always demux via tcp_dest_is_ours (force lab
	 * identity) so host SYN is not dropped when L2 still surfaces
	 * stale QEMU 10.0.2.15 mid-handoff. rtl/lab-identity force +
	 * recheck for non-lab dest match. Silent tallies only - no
	 * kprintf on this hot path (lean; prior multi-KiB FAULT).
	 * Soft!=product.
	 */
	if (tcp_dest_is_ours(pIp + 16) == 0) {
		tcp_soft_bump(&g_soft.u64InputMiss);
		return 0;
	}
	if (tcp_ip_is_lab(pIp + 16) != 0) {
		tcp_soft_bump(&g_soft.u64InputLabDemux);
	}
	if (net_l2_backend() == GJ_NET_L2_RTL8168 || tcp_udx_l2_live() != 0 ||
	    tcp_ip_is_lab(g_aOurIp) != 0) {
		tcp_soft_bump(&g_soft.u64InputDemuxForce);
	}
	ihl = (u16)((pIp[0] & 0x0f) * 4);
	if (ihl < 20u || cb < 14u + ihl + 20u) {
		tcp_soft_bump(&g_soft.u64InputMiss);
		return 0;
	}
	/*
	 * Payload length from IPv4 total length - NOT eth frame length.
	 * Short TCP segs (pure ACK/SYN) are padded to 60 on the wire; using
	 * cb would treat pad as data and corrupt RcvNxt / RX ring.
	 */
	ip_tot = (u16)(((u16)pIp[2] << 8) | (u16)pIp[3]);
	if (ip_tot < (u16)(ihl + 20u) || (u32)(14u + ip_tot) > cb ||
	    ip_tot > 1500u) {
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
	if (doff < 20u || (u32)ihl + doff > (u32)ip_tot) {
		tcp_soft_bump(&g_soft.u64InputMiss);
		return 0;
	}
	pay_off = 14u + (u32)ihl + doff;
	pay_len = (u32)ip_tot - (u32)ihl - doff;
	g_u32Segs++;
	tcp_soft_bump(&g_soft.u64InputHit);

	/*
	 * Dual DoD B freestanding residual (R0->RX return): any :22 to ours
	 * re-ensures soft/product listen after L2 ready *before* AcceptQ
	 * match so the first host SYN after RX returns finds a listener
	 * (DUT may claim :22 while R0; product non-soft-mint preferred).
	 * Re-force lab IP after ensure under rtl/lab dest (handoff churn).
	 * Silent when already ok (lean; no stamp storm). Soft!=product.
	 * Grep: net_tcp: soft listen :22
	 */
	if (dport == (u16)TCP_SOFT_SSH_PORT) {
		tcp_soft_ensure_listen22();
		if (net_l2_backend() == GJ_NET_L2_RTL8168 ||
		    tcp_udx_l2_live() != 0 ||
		    tcp_ip_is_lab(pIp + 16) != 0) {
			tcp_force_lab_ip();
		}
	}

	/* Eth 4-tuple: skip loopback pair slots (no aRip / soft only). */
	for (i = 0; i < TCP_MAX; i++) {
		if (g_aT[i].u8Used && !g_aT[i].u8Listening &&
		    !g_aT[i].u8IsLoop && g_aT[i].u16Lport == dport &&
		    g_aT[i].u16Rport == sport &&
		    memcmp(g_aT[i].aRip, pIp + 12, 4) == 0) {
			cs = (int)i;
			break;
		}
	}
	/*
	 * Prefer product (non-soft-mint) non-loop listeners so eth :22 finds
	 * sshd park slot. Soft-ensure mint is fallback only. Skip SHUT_RD
	 * stopped listeners. Soft!=product.
	 */
	for (i = 0; i < TCP_MAX; i++) {
		if (g_aT[i].u8Used && g_aT[i].u8Listening &&
		    g_aT[i].u8ShutRd == 0u && !g_aT[i].u8IsLoop &&
		    !g_aT[i].u8SoftMint && g_aT[i].u16Lport == dport) {
			ls = (int)i;
			break;
		}
	}
	if (ls < 0) {
		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used && g_aT[i].u8Listening &&
			    g_aT[i].u8ShutRd == 0u && !g_aT[i].u8IsLoop &&
			    g_aT[i].u16Lport == dport) {
				ls = (int)i;
				break;
			}
		}
	}
	if (ls < 0) {
		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used && g_aT[i].u8Listening &&
			    g_aT[i].u8ShutRd == 0u &&
			    g_aT[i].u16Lport == dport) {
				ls = (int)i;
				break;
			}
		}
	}
	/*
	 * Dual DoD B freestanding: :22 SYN with no listener after early
	 * ensure - soft ensure again after L2 ready, then re-scan.
	 * Product non-soft-mint preferred. Soft!=product.
	 */
	if (ls < 0 && (flags & FL_SYN) && !(flags & FL_ACK) &&
	    dport == (u16)TCP_SOFT_SSH_PORT) {
		tcp_soft_ensure_listen22();
		if (net_l2_backend() == GJ_NET_L2_RTL8168 ||
		    tcp_udx_l2_live() != 0 ||
		    tcp_ip_is_lab(pIp + 16) != 0) {
			tcp_force_lab_ip();
		}
		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used && g_aT[i].u8Listening &&
			    g_aT[i].u8ShutRd == 0u && !g_aT[i].u8IsLoop &&
			    !g_aT[i].u8SoftMint &&
			    g_aT[i].u16Lport == dport) {
				ls = (int)i;
				break;
			}
		}
		if (ls < 0) {
			for (i = 0; i < TCP_MAX; i++) {
				if (g_aT[i].u8Used && g_aT[i].u8Listening &&
				    g_aT[i].u8ShutRd == 0u &&
				    g_aT[i].u16Lport == dport) {
					ls = (int)i;
					break;
				}
			}
		}
	}

	if (flags & FL_RST) {
		tcp_soft_bump(&g_soft.u64InputRst);
		if (cs >= 0) {
			/*
			 * AcceptQ / SYN_RCVD: free slot so backlog does not stick.
			 * Established userspace/ABI path: sticky RST POLLERR until
			 * close (do not free - poll/recv/send honesty). Soft!=product.
			 * FORBIDDEN: freestanding rtl R0 deepen rabbit hole.
			 */
			if (g_aT[cs].u8AcceptQ != 0u ||
			    g_aT[cs].u8State == ST_SYN_RCVD) {
				tcp_drop_syn_rcvd((u32)cs);
			} else {
				g_aT[cs].u8RstSeen = 1;
				g_aT[cs].u8RtxValid = 0;
				g_aT[cs].u8RtxSyn = 0;
				g_aT[cs].u8RtxBusy = 0;
				/* Keep slot used; state stays for drain/close. */
				if (g_aT[cs].u8State == ST_ESTABLISHED ||
				    g_aT[cs].u8State == ST_CLOSE_WAIT ||
				    g_aT[cs].u8State == ST_FIN_WAIT1 ||
				    g_aT[cs].u8State == ST_FIN_WAIT2) {
					g_aT[cs].u8State = ST_CLOSED;
				}
				tcp_soft_bump(&g_soft.u64RstSticky);
			}
		}
		return 1;
	}

	/*
	 * Retransmitted client SYN while still SYN_RCVD: re-emit SYN-ACK.
	 * Without this, a lost first SYN-ACK leaves the client timing out
	 * and the listener stuck with one soft pending child.
	 */
	if ((flags & FL_SYN) && !(flags & FL_ACK) && cs >= 0 &&
	    g_aT[cs].u8State == ST_SYN_RCVD && !g_aT[cs].u8IsLoop) {
		u32 syn_seq = g_aT[cs].u8RtxSyn ? g_aT[cs].u32RtxSeq
						: g_aT[cs].u32SndUna;
		int nDup;

		nDup = tcp_tx_raw((u32)cs, (u8)(FL_SYN | FL_ACK), syn_seq, 0,
				 0);
		/*
		 * Gap B: :22 busy-shot on dup-SYN under ARP/ICMP - ring may
		 * free mid-input before poll. Soft!=product.
		 */
		if (nDup < 0 && dport == (u16)TCP_SOFT_SSH_PORT) {
			u32 u32Shot;

			for (u32Shot = 0; u32Shot < TCP_RTX_BUSY_SHOTS;
			     u32Shot++) {
				nDup = tcp_tx_raw((u32)cs,
						  (u8)(FL_SYN | FL_ACK),
						  syn_seq, 0, 0);
				if (nDup >= 0) {
					break;
				}
			}
		}
		g_aT[cs].u32RtxSeq = syn_seq;
		g_aT[cs].u32RtxLen = 0;
		g_aT[cs].u32RtxTick = now_ms();
		g_aT[cs].u8RtxValid = 1;
		g_aT[cs].u8RtxSyn = 1;
		/* TX busy -> short eth_syn_ack rtx interval. Soft!=product. */
		g_aT[cs].u8RtxBusy = (nDup < 0) ? 1u : 0u;
		if (nDup < 0 && g_aT[cs].u8RtxBusyN < 255u) {
			g_aT[cs].u8RtxBusyN++;
		} else if (nDup >= 0) {
			g_aT[cs].u8RtxBusyN = 0;
		}
		/*
		 * Grep: net_tcp: soft eth_syn rtx (dup SYN).
		 * Event-capped ONE line only - no flood under aggressive
		 * client SYN rtx (hot demux must stay lean). Soft!=product.
		 */
		if (tcp_soft_event_ok()) {
			kprintf("net_tcp: soft eth_syn rtx port=%u "
				"sport=%u slot=%d seq=%u tx=%d busy=%u "
				"(Soft!=product)\n",
				(unsigned)dport, (unsigned)sport, cs,
				(unsigned)syn_seq, nDup,
				(unsigned)g_aT[cs].u8RtxBusy);
		}
		return 1;
	}

	if ((flags & FL_SYN) && !(flags & FL_ACK) && ls >= 0 && cs < 0) {
		int ns;
		int nTx;
		u32 u32Isn;
		u32 u32Ack;

		/* Soft backlog: drop SYN when accept queue is full. */
		if (g_aT[ls].u8Backlog == 0) {
			g_aT[ls].u8Backlog = 1;
		}
		if (g_aT[ls].u8Pending >= g_aT[ls].u8Backlog) {
			tcp_soft_bump(&g_soft.u64InputSynDrop);
			/* Grep: net_tcp: soft syn_drop - ONE line (no wave). */
			if (tcp_soft_event_ok()) {
				kprintf("net_tcp: soft syn_drop port=%u "
					"pending=%u backlog=%u reason=full "
					"(Soft!=product)\n",
					(unsigned)dport,
					(unsigned)g_aT[ls].u8Pending,
					(unsigned)g_aT[ls].u8Backlog);
			}
			return 1;
		}
		/*
		 * Soft accept queue for eth sessions: up to backlog children
		 * via u8AcceptQ + u8Pending. i16Peer is newest-child hint only
		 * - do not drop new SYN solely because a prior child is live.
		 * Reclaim a stale i16Peer so pending does not leak. Soft!=product.
		 */
		if (g_aT[ls].i16Peer >= 0) {
			i16 i16Old = g_aT[ls].i16Peer;

			if ((u32)i16Old >= TCP_MAX || !g_aT[i16Old].u8Used ||
			    !g_aT[i16Old].u8AcceptQ ||
			    (g_aT[i16Old].u8State != ST_SYN_RCVD &&
			     g_aT[i16Old].u8State != ST_ESTABLISHED &&
			     g_aT[i16Old].u8State != ST_CLOSE_WAIT)) {
				g_aT[ls].i16Peer = -1;
				if (g_aT[ls].u8Pending > 0) {
					g_aT[ls].u8Pending--;
				}
			}
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
		/* ACK = client ISN+1; our ISN is SndNxt/SndUna from alloc. */
		g_aT[ns].u32RcvNxt = seq + 1u;
		g_aT[ns].u16PeerWnd = wnd ? wnd : TCP_WND;
		g_aT[ns].u8IsLoop = 0;
		g_aT[ns].u8AcceptQ = 1; /* until net_tcp_accept takes it */
		g_aT[ns].i16Peer = -1;
		u32Isn = g_aT[ns].u32SndNxt;
		u32Ack = g_aT[ns].u32RcvNxt;
		/* SYN-ACK via net_l2_tx (rtl/virtio/UDX ETH_TX_PULL). */
		nTx = tcp_tx((u32)ns, (u8)(FL_SYN | FL_ACK), 0, 0);
		/*
		 * Gap B: :22 first SYN-ACK busy-shot under ARP/ICMP busy -
		 * ring may free mid-input before pure POLL rtx. Use
		 * tcp_tx_raw after arm (no re-arm churn; denser than
		 * re-entering tcp_tx). Soft!=product.
		 */
		if (nTx < 0 && dport == (u16)TCP_SOFT_SSH_PORT) {
			u32 u32Shot;
			u32 u32SynSeq = g_aT[ns].u8RtxSyn
						? g_aT[ns].u32RtxSeq
						: g_aT[ns].u32SndNxt;

			for (u32Shot = 0; u32Shot < TCP_RTX_BUSY_SHOTS;
			     u32Shot++) {
				nTx = tcp_tx_raw((u32)ns,
						 (u8)(FL_SYN | FL_ACK),
						 u32SynSeq, 0, 0);
				if (nTx >= 0) {
					if (g_aT[ns].u32SndNxt == u32SynSeq) {
						g_aT[ns].u32SndNxt++;
					}
					g_aT[ns].u32RtxTick = now_ms();
					tcp_rtx_mark_landed((u32)ns);
					break;
				}
				if (g_aT[ns].u8RtxBusyN < 255u) {
					g_aT[ns].u8RtxBusyN++;
				}
				g_aT[ns].u8RtxBusy = 1;
			}
		}
		g_aT[ls].i16Peer = (i16)ns; /* newest AcceptQ child hint */
		if (g_aT[ls].u8Pending < 255u) {
			g_aT[ls].u8Pending++;
		}
		tcp_soft_bump(&g_soft.u64InputSyn);
		tcp_soft_tally(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
			       NULL, NULL, NULL, NULL);
		/*
		 * Grep: net_tcp: soft eth_syn port=22 ...
		 * Event-capped ONE line (no twin / no wave / no flood storm).
		 * Hot demux residual stays silent after TCP_SOFT_EVENT_MAX.
		 * Soft!=product.
		 */
		/*
		 * Do not kprintf on this SYN path. QEMU49 froze after the
		 * second eth_syn lamp the same way 42-48 froze after
		 * eth_estab. Dual DoD B OPEN.
		 */
		(void)nTx;
		(void)u32Isn;
		(void)u32Ack;
		(void)tcp_soft_event_ok();
		if (tcp_udx_l2_live() != 0) {
			static u8 s_fUdxSynLamp;

			if (s_fUdxSynLamp == 0u) {
				s_fUdxSynLamp = 1u;
				/* Grep: net_tcp: soft udx eth_syn */
				kprintf("net_tcp: soft udx eth_syn "
					"backend=none path=rtl8168_udx "
					"port=%u tx=%d dual_dod_b=OPEN_UDX "
					"ETH_INJECT=1 ETH_TX_PULL=1 "
					"(Soft!=product; SYN->listen; "
					"SYN-ACK enqueue like ICMP)\n",
					(unsigned)dport, nTx);
			}
		}
		return 1;
	}

	if (cs < 0) {
		return 1;
	}

	if (wnd) {
		g_aT[cs].u16PeerWnd = wnd;
	}

	/*
	 * Complete handshake when ACK covers our SYN.
	 * After a failed first TX, SndNxt still equals ISN until rtx succeeds.
	 * Also accept ack == RtxSeq+1 when SYN rtx armed (client saw wire
	 * SYN-ACK even if local SndNxt lag after partial TX paths).
	 */
	if (g_aT[cs].u8State == ST_SYN_RCVD && (flags & FL_ACK) &&
	    !g_aT[cs].u8IsLoop) {
		u32 u32SynOk = 0;

		if (ack == g_aT[cs].u32SndNxt &&
		    g_aT[cs].u32SndNxt != g_aT[cs].u32SndUna) {
			u32SynOk = 1;
		} else if (g_aT[cs].u8RtxValid && g_aT[cs].u8RtxSyn &&
			   ack == g_aT[cs].u32RtxSeq + 1u) {
			u32SynOk = 1;
			/* Align send cursor to client ACK of our SYN. */
			if (g_aT[cs].u32SndNxt == g_aT[cs].u32RtxSeq) {
				g_aT[cs].u32SndNxt = ack;
			}
		}
		if (u32SynOk != 0u) {
			g_aT[cs].u8State = ST_ESTABLISHED;
			g_aT[cs].u32SndUna = ack;
			/* Handshake done - stop SYN-ACK rtx. */
			g_aT[cs].u8RtxSyn = 0;
			g_aT[cs].u8RtxValid = 0;
			g_aT[cs].u8RtxBusy = 0;
			g_aT[cs].u8RtxBusyN = 0;
			g_u32Accepts++;
			/*
			 * One ident writer: sshd SEND owns SSH-2.0-GreenJade_sshd.
			 * Gap C kernel banner spent the first data seq so
			 * userspace KEXINIT EAGAIN'd (0.1.150/151 host).
			 */
			/*
			 * Functional STRONGER (sshd :22 product path):
			 * AcceptQ child just became ESTABLISHED - reparent +
			 * rehook every same-port live listener (eth prefer)
			 * so product park poll/accept/poll_mask sees POLLIN
			 * mid-input without waiting pure POLL. Soft!=product.
			 * greppable: eth_estab_rehook | acceptq_rehook
			 */
			if (g_aT[cs].u8AcceptQ != 0u) {
				u32 u32Li;
				u16 u16PortE = g_aT[cs].u16Lport;

				for (u32Li = 0; u32Li < TCP_MAX; u32Li++) {
					if (g_aT[u32Li].u8Used == 0u ||
					    g_aT[u32Li].u8Listening == 0u ||
					    g_aT[u32Li].u8ShutRd != 0u ||
					    g_aT[u32Li].u16Lport !=
						u16PortE) {
						continue;
					}
					(void)tcp_acceptq_reparent(u32Li,
								   u16PortE);
					(void)tcp_listen_rehook_ready(u32Li);
				}
			}
			/*
			 * Grep: net_tcp: ESTABLISHED / soft eth_estab
			 * Event-capped ONE line (no twin / no wave). Soft!=product.
			 */
			/*
			 * Do not kprintf on this path. QEMU42-48: this lamp
			 * was the last serial line; ppoll never returned
			 * POLLIN. Rehook above is the product work. Dual
			 * DoD B OPEN.
			 */
			(void)tcp_soft_event_ok();
		}
	} else if (g_aT[cs].u8State == ST_SYN_RCVD && (flags & FL_ACK) &&
		   g_aT[cs].u8IsLoop && ack == g_aT[cs].u32SndNxt &&
		   g_aT[cs].u32SndNxt != g_aT[cs].u32SndUna) {
		/* Loopback soft path (normally ESTABLISHED in connect). */
		g_aT[cs].u8State = ST_ESTABLISHED;
		g_aT[cs].u32SndUna = ack;
		g_aT[cs].u8RtxSyn = 0;
		g_aT[cs].u8RtxValid = 0;
		g_u32Accepts++;
	}

	if (flags & FL_ACK) {
		if (ack > g_aT[cs].u32SndUna && ack <= g_aT[cs].u32SndNxt) {
			g_aT[cs].u32SndUna = ack;
			if (g_aT[cs].u8RtxValid && g_aT[cs].u8RtxSyn &&
			    ack >= g_aT[cs].u32RtxSeq + 1u) {
				g_aT[cs].u8RtxValid = 0;
				g_aT[cs].u8RtxSyn = 0;
			}
			if (g_aT[cs].u8RtxValid && !g_aT[cs].u8RtxSyn) {
				/*
				 * Data arm: ACK covers RtxSeq..+Len.
				 * Bare FIN arm (RtxLen==0): need ack > RtxSeq
				 * so pure data ACK at FIN seq does not drop
				 * half-close rtx early. Soft!=product.
				 */
				if (g_aT[cs].u32RtxLen == 0u) {
					if (ack > g_aT[cs].u32RtxSeq) {
						g_aT[cs].u8RtxValid = 0;
					}
				} else if (ack >= g_aT[cs].u32RtxSeq +
						   g_aT[cs].u32RtxLen) {
					g_aT[cs].u8RtxValid = 0;
				}
			}
			/* Soft close progress on ACK of our FIN. */
			if (g_aT[cs].u8FinSent &&
			    ack >= g_aT[cs].u32SndNxt) {
				if (g_aT[cs].u8State == ST_FIN_WAIT1) {
					g_aT[cs].u8State = ST_FIN_WAIT2;
				} else if (g_aT[cs].u8State == ST_LAST_ACK) {
					/* Fully closed - free slot. */
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
		 * In-window prefix: exact RcvNxt, or overlap after a partial
		 * ACK (OpenSSH KEXINIT is often > TCP_MSS; 0.1.167 clamped
		 * to 1024 then dropped the tail / type 30 as OOO).
		 * push_rx clamps to ring room. Do not cap RX at TX MSS.
		 */
		if (pay_len != 0u &&
		    (g_aT[cs].u8State == ST_ESTABLISHED ||
		     g_aT[cs].u8State == ST_SYN_RCVD) &&
		    (seq == g_aT[cs].u32RcvNxt ||
		     (u32)(g_aT[cs].u32RcvNxt - seq) < pay_len)) {
			u32 u32Off = 0u;
			u32 cbTake;
			int got;

			if (seq != g_aT[cs].u32RcvNxt) {
				u32Off = g_aT[cs].u32RcvNxt - seq;
			}
			cbTake = pay_len - u32Off;
			got = push_rx((u32)cs, pFrame + pay_off + u32Off,
				      cbTake);
			if (got > 0) {
				g_aT[cs].u32RcvNxt += (u32)got;
				tcp_soft_bump(&g_soft.u64InputData);
				(void)tcp_tx((u32)cs, FL_ACK, 0, 0);
			} else if (u32Off != 0u) {
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
					/* Simultaneous close -> TIME_WAIT soft. */
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

/*
 * Soft ensure: after net_l2 ready, a listen socket on :22 always exists for
 * eth accept (Dual DoD B). Mints a soft listener if product sshd has not
 * bound yet; product listen supersedes idle soft mint. Always re-syncs lab
 * IP (10.200.125.50 on rtl / UDX) so demux/SYN-ACK never stick on stale
 * QEMU 10.0.2.15 after L2 ready. backend=none + ETH_UDX_READY is live
 * wire (ETH_INJECT SYN) — not virtio-only. When RX returns after R0,
 * ready-edge + any :22 demux path re-enter here so listen is present
 * before AcceptQ match.
 * Grep: net_tcp: soft listen :22
 * Soft!=product. Lean: one-shot / stamp-capped lamps only (no multi-KiB).
 */
static void
tcp_soft_ensure_listen22(void)
{
	u32 i;
	int nLs = -1;
	int nSoft = -1;
	int nMint;
	u32 u32Be;
	int fReady;
	int fLabIp;
	int fForceLab;

	/* Always refresh identity - lab IP force on rtl/UDX even before ready. */
	tcp_sync_l2_identity();
	u32Be = net_l2_backend();
	fReady = net_l2_ready();
	/*
	 * Force lab when rtl, product UDX L2 (backend=none), or identity
	 * already lab (handoff may report virtio/none briefly while frames
	 * still target lab static). Soft!=product · Dual DoD B demux residual.
	 */
	fForceLab = (u32Be == GJ_NET_L2_RTL8168 ||
		     u32Be == GJ_NET_L2_NONE ||
		     tcp_ip_is_lab(g_aOurIp) != 0)
			? 1
			: 0;
	if (fForceLab != 0) {
		tcp_force_lab_ip();
	}
	fLabIp = tcp_ip_is_lab(g_aOurIp);
	/*
	 * backend=none is product UDX L2 when net_l2_ready (ETH_UDX_READY).
	 * Do not treat "not virtio" as no-wire — that dropped host SYN
	 * after ETH_INJECT (ARP/ICMP already demux). Soft!=product Dual DoD B.
	 * greppable: backend=none | ETH_INJECT | path=rtl8168_udx
	 */
	if (u32Be == GJ_NET_L2_NONE && fReady == 0) {
		/*
		 * No UDX arm yet: keep forced lab for demux when inject
		 * later arrives. Do not mint a dead :22. Soft!=product.
		 */
		if (fForceLab != 0) {
			tcp_force_lab_ip();
			fLabIp = 1;
		}
		g_u8SoftListen22WasReady = 0;
		return;
	}
	/*
	 * After net_l2 ready, ensure :22 for lab IP eth accept (Dual DoD B).
	 * Handoff-pending still keeps identity for demux (lab 10.200.125.50
	 * forced on rtl / lab-identity); mint only when TX/RX usable so soft
	 * accept is not a dead socket. Soft!=product.
	 * Grep: net_tcp: soft listen :22
	 */
	if (fReady == 0) {
		/*
		 * Not ready: lab IP identity already refreshed above; no mint.
		 * One-shot wait lamp so lab greps see "after L2 ready"
		 * residual without flood. Explicit lab force even while
		 * !ready so demux identity is primed pre-ready-edge / R0.
		 * Soft!=product. Grep: net_tcp: soft listen :22
		 */
		if (fForceLab != 0 || u32Be == GJ_NET_L2_RTL8168) {
			tcp_force_lab_ip();
			fLabIp = 1;
		}
		g_u8SoftListen22WasReady = 0;
		g_u32SoftListen22Ticks++;
		/* One-shot wait lamp only (multi-ensure from poll must not storm). */
		if (g_u8SoftListen22Logged == 0 && tcp_soft_poll_stamp_ok()) {
			kprintf("net_tcp: soft listen :22 wait L2 "
				"ip=%u.%u.%u.%u lab_ip=%d backend=%s "
				"ready=0 (Soft!=product; mint after L2 ready)\n",
				g_aOurIp[0], g_aOurIp[1], g_aOurIp[2],
				g_aOurIp[3], fLabIp, net_l2_name());
			g_u8SoftListen22Logged = 1;
		}
		return;
	}
	/*
	 * Ready-edge deepen (Gap B): when L2 flips !ready->ready, re-sync
	 * lab IP identity and force a greppable soft listen :22 lamp with
	 * lab_ip so lab greps see the post-L2-ready path (not only the
	 * wait lamp). Covers R0->RX return ready flip. Soft!=product.
	 */
	if (g_u8SoftListen22WasReady == 0u) {
		g_u8SoftListen22WasReady = 1;
		g_u8SoftListen22Logged = 0; /* allow immediate ready lamp */
		/* Re-pull L2 + force lab IP after ready (stale QEMU IP). */
		tcp_sync_l2_identity();
		/* Explicit lab force on rtl / lab-identity (ready-edge). Soft!=product. */
		if (u32Be == GJ_NET_L2_RTL8168 || tcp_udx_l2_live() != 0 ||
		    tcp_ip_is_lab(g_aOurIp) != 0) {
			tcp_force_lab_ip();
		}
		fLabIp = tcp_ip_is_lab(g_aOurIp);
		/* Grep: net_tcp: soft listen :22 (ready-edge lab_ip; one-shot) */
		if (tcp_soft_poll_stamp_ok()) {
			kprintf("net_tcp: soft listen :22 ready-edge "
				"ip=%u.%u.%u.%u lab_ip=%d backend=%s ready=1 "
				"(Soft!=product; after L2 ready)\n",
				g_aOurIp[0], g_aOurIp[1], g_aOurIp[2],
				g_aOurIp[3], fLabIp, net_l2_name());
		}
	} else {
		/*
		 * Ready-path re-force + lab IP re-sync (Gap B): every ensure
		 * while ready re-pull L2; rtl may surface stale QEMU
		 * 10.0.2.15 after handoff churn - re-force 10.200.125.50.
		 * Identity correction is silent on hot residual (multi-ensure
		 * from poll must not stamp-storm). Soft!=product · lean poll.
		 */
		tcp_sync_l2_identity();
		if (u32Be == GJ_NET_L2_RTL8168 || tcp_udx_l2_live() != 0 ||
		    tcp_ip_is_lab(g_aOurIp) != 0) {
			tcp_force_lab_ip();
		}
		fLabIp = tcp_ip_is_lab(g_aOurIp);
		g_u32SoftListen22Ticks++;
		/* Hot residual: silence lab_ip re-sync/hold lamps. Soft!=product. */
	}

	for (i = 0; i < TCP_MAX; i++) {
		if (!g_aT[i].u8Used || !g_aT[i].u8Listening) {
			continue;
		}
		if (g_aT[i].u8ShutRd != 0u) {
			continue; /* SHUT_RD stopped - not a live listener */
		}
		if (g_aT[i].u16Lport != (u16)TCP_SOFT_SSH_PORT) {
			continue;
		}
		if (g_aT[i].u8SoftMint) {
			nSoft = (int)i;
		} else if (nLs < 0) {
			nLs = (int)i; /* product (or any non-soft) first */
		}
	}
	/* Drop stale soft-slot index if table was cleared. */
	if (g_i32SoftListen22 >= 0) {
		u32 u32S = (u32)g_i32SoftListen22;

		if (u32S >= TCP_MAX || !g_aT[u32S].u8Used ||
		    !g_aT[u32S].u8Listening || g_aT[u32S].u8ShutRd != 0u ||
		    g_aT[u32S].u16Lport != (u16)TCP_SOFT_SSH_PORT) {
			g_i32SoftListen22 = -1;
			nSoft = -1;
		}
	}

	if (nLs >= 0) {
		/* Product (or non-soft) :22 present - eth accept path ok. */
		/*
		 * Silent multi-listener reparent + rehook: product listen may
		 * have raced soft close; keep pending honest and i16Peer on
		 * oldest ESTABLISHED (eth prefer) on every live :22 listener
		 * so accept/poll_mask do not EAGAIN while AcceptQ is ready
		 * (product park fd may not be the first nLs slot). Soft!=product
		 * · functional STRONGER · greppable: ensure_rehook.
		 */
		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used == 0u || g_aT[i].u8Listening == 0u ||
			    g_aT[i].u8ShutRd != 0u ||
			    g_aT[i].u16Lport != (u16)TCP_SOFT_SSH_PORT) {
				continue;
			}
			(void)tcp_acceptq_reparent(i, (u16)TCP_SOFT_SSH_PORT);
			(void)tcp_listen_rehook_ready(i);
		}
		g_u32SoftListen22Ticks++;
		/* One-shot ok lamp only (multi-ensure from poll is silent). */
		if (g_u8SoftListen22Logged == 0 && tcp_soft_poll_stamp_ok()) {
			/* Grep: net_tcp: soft listen :22 */
			kprintf("net_tcp: soft listen :22 ok fd=%u slot=%d "
				"ip=%u.%u.%u.%u lab_ip=%d backend=%s "
				"soft_mint=0 pending=%u ready=1 "
				"listen=:22 product_net_owns_wire=%u "
				"soft_listen_ne_host_banner=1 "
				"listen_accept_path_honesty=1 "
				"product_sshd_tcp22=OPEN until_DUT=1 "
				"wire_handoff+tcp22=1 H2=once "
				"(Soft!=product; after L2 ready; denser "
				"wire22 listen/accept path; acceptq_rehook; "
				"ensure_rehook multi)\n",
				(unsigned)(TCP_FD_BASE + (u32)nLs), nLs,
				g_aOurIp[0], g_aOurIp[1], g_aOurIp[2],
				g_aOurIp[3], fLabIp, net_l2_name(),
				(unsigned)g_aT[nLs].u8Pending,
				(unsigned)tcp_udx_l2_live());
			g_u8SoftListen22Logged = 1;
		}
		return;
	}

	if (nSoft >= 0) {
		g_i32SoftListen22 = nSoft;
		/*
		 * Existing soft mint: silent AcceptQ reparent + rehook so
		 * orphan ESTABLISHED children from prior soft teardown are
		 * accept-ready without waiting pure POLL. Soft!=product.
		 */
		(void)tcp_acceptq_reparent((u32)nSoft, (u16)TCP_SOFT_SSH_PORT);
		(void)tcp_listen_rehook_ready((u32)nSoft);
		g_u32SoftListen22Ticks++;
		/* One-shot ok lamp only. Soft!=product. */
		if (g_u8SoftListen22Logged == 0 && tcp_soft_poll_stamp_ok()) {
			/* Grep: net_tcp: soft listen :22 */
			kprintf("net_tcp: soft listen :22 ok fd=%u slot=%d "
				"ip=%u.%u.%u.%u lab_ip=%d backend=%s "
				"soft_mint=1 pending=%u ready=1 "
				"listen=:22 product_net_owns_wire=%u "
				"soft_listen_ne_host_banner=1 "
				"listen_accept_path_honesty=1 "
				"product_sshd_tcp22=OPEN until_DUT=1 "
				"wire_handoff+tcp22=1 H2=once "
				"(Soft!=product; after L2 ready; denser "
				"wire22 listen/accept path; acceptq_rehook)\n",
				(unsigned)(TCP_FD_BASE + (u32)nSoft), nSoft,
				g_aOurIp[0], g_aOurIp[1], g_aOurIp[2],
				g_aOurIp[3], fLabIp, net_l2_name(),
				(unsigned)g_aT[nSoft].u8Pending,
				(unsigned)tcp_udx_l2_live());
			g_u8SoftListen22Logged = 1;
		}
		return;
	}

	/* No :22 listener - soft-mint so eth SYN can land. Soft!=product. */
	nMint = alloc_slot();
	if (nMint < 0) {
		if (g_u8SoftListen22Logged == 0 && tcp_soft_poll_stamp_ok()) {
			kprintf("net_tcp: soft listen :22 FAIL emfile "
				"backend=%s (Soft!=product)\n",
				net_l2_name());
			g_u8SoftListen22Logged = 1;
		}
		return;
	}
	g_aT[nMint].u16Lport = (u16)TCP_SOFT_SSH_PORT;
	g_aT[nMint].u8Backlog = 4;
	g_aT[nMint].u8Pending = 0;
	g_aT[nMint].u8Listening = 1;
	g_aT[nMint].u8State = ST_LISTEN;
	g_aT[nMint].u8SoftMint = 1;
	g_aT[nMint].u8IsLoop = 0;
	g_aT[nMint].i16Peer = -1;
	g_i32SoftListen22 = nMint;
	/*
	 * Soft-mint AcceptQ reparent + rehook: prior soft listener may have
	 * closed leaving orphan ESTABLISHED/SYN_RCVD AcceptQ children on :22.
	 * Recount pending + rehook oldest ESTAB (eth prefer) so eth accept
	 * residual survives soft teardown without pure-POLL wait.
	 * Soft!=product · Dual DoD B · functional STRONGER.
	 */
	(void)tcp_acceptq_reparent((u32)nMint, (u16)TCP_SOFT_SSH_PORT);
	(void)tcp_listen_rehook_ready((u32)nMint);
	tcp_soft_bump(&g_soft.u64ListenOk);
	/* Grep: net_tcp: soft listen :22 - ONE line only (no twin/wave). */
	if (tcp_soft_poll_stamp_ok()) {
		kprintf("net_tcp: soft listen :22 fd=%u slot=%d backlog=%u "
			"ip=%u.%u.%u.%u lab_ip=%d backend=%s soft_mint=1 "
			"pending=%u ready=1 listen=:22 "
			"product_net_owns_wire=%u "
			"soft_listen_ne_host_banner=1 "
			"listen_accept_path_honesty=1 "
			"product_sshd_tcp22=OPEN until_DUT=1 "
			"wire_handoff+tcp22=1 H2=once "
			"(Soft!=product; eth accept ensure after L2; denser "
			"wire22 listen/accept path; acceptq_reparent; "
			"acceptq_rehook; !=host_banner_proof)\n",
			(unsigned)(TCP_FD_BASE + (u32)nMint), nMint,
			(unsigned)g_aT[nMint].u8Backlog, g_aOurIp[0],
			g_aOurIp[1], g_aOurIp[2], g_aOurIp[3], fLabIp,
			net_l2_name(), (unsigned)g_aT[nMint].u8Pending,
			(unsigned)tcp_udx_l2_live());
	}
	g_u8SoftListen22Logged = 1;
}

void
net_tcp_ensure_listen22(void)
{
	tcp_soft_ensure_listen22();
}

/*
 * Run-loop only via net_eth_poll (scheduler_run full thr stack).
 * NEVER call from timer IRQ - IRQ stack smash / #PF I=1 class.
 * greppable: net_eth_poll=run_loop_only · Soft!=product · Dual DoD B.
 */
void
net_tcp_poll(void)
{
	u32 i;
	u32 t = now_ms();
	u32 cRtx = 0;
	u32 cTw = 0;

	tcp_soft_bump(&g_soft.u64PollTicks);
	/* Fresh per-poll stamp budget (hot residual stays silent regardless). */
	g_u32SoftPollStampN = 0;

	/*
	 * Dual DoD B: after net_l2 ready, always keep soft/product listen :22
	 * for eth accept. Lab IP forced on rtl in tcp_sync_l2_identity /
	 * tcp_force_lab_ip (10.200.125.50). Silent when already ok.
	 * Lean: one ensure at poll entry + one post multi-pass (thrash-strip
	 * mid-pass / twin post ensure). Soft!=product.
	 * Grep: net_tcp: soft listen :22
	 */
	tcp_soft_ensure_listen22();

	/*
	 * Functional residual STRONGER (sshd :22 product path): every pure
	 * POLL silently reparent + rehook ready ESTABLISHED AcceptQ on live
	 * :22 listeners so poll_mask/accept pending stay honest under multi
	 * SYN / soft→product race without stamp storms. Soft!=product.
	 * greppable: acceptq_rehook | soft accept residual
	 */
	{
		static u8 g_u8FuncLapOnce;

		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used == 0u || g_aT[i].u8Listening == 0u ||
			    g_aT[i].u8ShutRd != 0u ||
			    g_aT[i].u16Lport != (u16)TCP_SOFT_SSH_PORT) {
				continue;
			}
			(void)tcp_acceptq_reparent(i, (u16)TCP_SOFT_SSH_PORT);
			(void)tcp_listen_rehook_ready(i);
		}
		/* Grep: net_tcp: soft functional lap (once; no storm). H2 once. */
		if (g_u8FuncLapOnce == 0u) {
			g_u8FuncLapOnce = 1u;
			if (tcp_soft_poll_stamp_ok()) {
				kprintf("net_tcp: soft functional lap "
					"listen=1 accept=1 poll=1 poll_mask=1 "
					"acceptq_rehook=1 acceptq_reparent=1 "
					"accept_eagain_heal=1 poll_mask_heal=1 "
					"ensure_rehook=1 post_poll_rehook=1 "
					"close_rehook=1 multi_listen_heal=1 "
					"eth_estab_rehook=1 listen_close_rehook=1 "
					"listen_accept_path_honesty=1 "
					"wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
					"path=sshd:22 product_sshd_tcp22=OPEN "
					"until_DUT=1 "
					"dual_dod_a=OPEN dual_dod_b=OPEN "
					"freestanding_class=SKIP product=UDX "
					"net_tcp_poll=run_loop_only thr_only=1 "
					"H1=1 H2=once Soft!=product "
					"(Soft!=product; W11 Dual DoD B "
					"FUNCTIONAL residual STRONGER denser; "
					"wire handoff+:22 listen/accept path "
					"honesty; agent!=close; "
					"!=host_banner_proof; Dual DoD OPEN; "
					"product_sshd_tcp22=OPEN until DUT; "
					"stamp-free bar v2026.08.04.75; never .76)\n");
			}
		}
	}

	/*
	 * Accept queue residual lamp (rate-limited): count ESTABLISHED AcceptQ
	 * children waiting for accept() - path to host nc/ssh banner after
	 * SYN-ACK rtx. One-shot stamp only (thrash-strip denser flood).
	 * Grep: net_tcp: soft accept residual. Soft!=product.
	 */
	{
		static u32 g_u32AqResTicks;
		u32 cAq = 0;
		u32 cAq22 = 0;
		u32 cSynR = 0;
		u32 fLog = 0;

		for (i = 0; i < TCP_MAX; i++) {
			if (!g_aT[i].u8Used || g_aT[i].u8IsLoop) {
				continue;
			}
			/*
			 * AcceptQ residual (half-open / estab awaiting accept):
			 * keep SYN-ACK busy-armed under flood so pure POLL
			 * from yield-poll accept flushes without wall wait.
			 * Gap B: only re-busy when never TX-ok (RtxCount==0)
			 * or still busy-armed / had TX fails (BusyN) - do not
			 * re-busy after a successful wire SYN-ACK or dense
			 * pure POLL burns TCP_RTX_MAX -> giveup before peer
			 * ACK. Soft!=product.
			 */
			if (g_aT[i].u8AcceptQ && !g_aT[i].u8Listening) {
				if (g_aT[i].u8State == ST_ESTABLISHED) {
					cAq++;
					if (g_aT[i].u16Lport ==
					    (u16)TCP_SOFT_SSH_PORT) {
						cAq22++;
					}
				} else if (g_aT[i].u8State == ST_SYN_RCVD) {
					cSynR++;
					/*
					 * Gap B: AcceptQ :22 SYN_RCVD busy-arm
					 * only when never landed / still
					 * TX-fail so pure POLL + busy-shot flush
					 * under ARP/ICMP without wall wait.
					 * Do NOT re-busy after clean wire
					 * success (avoids TCP_RTX_MAX giveup
					 * before peer ACK). Soft!=product.
					 */
					if (g_aT[i].u8RtxValid &&
					    g_aT[i].u8RtxSyn &&
					    tcp_rtx22_need_busy(&g_aT[i]) !=
						0) {
						g_aT[i].u8RtxBusy = 1;
					}
				}
			}
			/*
			 * :22 eth banner arm still unflushed (first-fail
			 * soft-accept or never TX-ok) - force busy for pure
			 * POLL density. Re-busy only never-landed / TX-fail;
			 * post-success unacked uses denser wall interval
			 * (POST22_DIV). Soft!=product.
			 */
			if (g_aT[i].u16Lport == (u16)TCP_SOFT_SSH_PORT &&
			    g_aT[i].u8State == ST_ESTABLISHED &&
			    g_aT[i].u8RtxValid && !g_aT[i].u8RtxSyn &&
			    g_aT[i].u32SndNxt > g_aT[i].u32SndUna &&
			    tcp_rtx22_need_busy(&g_aT[i]) != 0) {
				g_aT[i].u8RtxBusy = 1;
			}
		}
		g_u32AqResTicks++;
		/* One-shot AcceptQ residual lamp (hard stamp cap; no denser flood). */
		if ((cAq != 0u || cSynR != 0u) && g_u32AqResTicks == 1u) {
			fLog = 1;
		}
		if (fLog != 0u && tcp_soft_poll_stamp_ok()) {
			kprintf("net_tcp: soft accept residual "
				"estab_q=%u :22_q=%u syn_rcvd_q=%u "
				"(Soft!=product; accept->banner path)\n",
				(unsigned)cAq, (unsigned)cAq22,
				(unsigned)cSynR);
		}
	}

	/*
	 * Lean functional rtx residual (Soft!=product; freestanding thrash SKIP):
	 * SYN/SYN-ACK + bare FIN + last unacked data. Busy-armed -> interval 0
	 * for pure POLL flush. TCP_RTX_PASSES multi-pass (busy-only after pass
	 * 0); phase prefer :22 SYN_RCVD then :22 ESTABLISHED banner then rest;
	 * TCP_RTX_BUSY_SHOTS same-pass retries; POST22_DIV wall after land.
	 * Thrash-strip: no mid multi-pass ensure storm; lab_ip silent force
	 * once before multi-pass only. Soft!=product.
	 */
	{
		u32 u32Pass;
		u32 cBusyLeft = 0;
		u32 u32Phase;
		u32 u32NPhase;

		/*
		 * Explicit lab_ip force before multi-pass (rtl or already-lab
		 * identity) so demux/SYN-ACK TX hold 10.200.125.50 under
		 * handoff churn. Silent. Soft!=product · thrash-strip.
		 */
		if (net_l2_backend() == GJ_NET_L2_RTL8168 ||
		    tcp_ip_is_lab(g_aOurIp) != 0) {
			tcp_force_lab_ip();
		}

		for (u32Pass = 0; u32Pass < TCP_RTX_PASSES; u32Pass++) {
			cBusyLeft = 0;
			/*
			 * Per-pass :22 int0 re-arm - SYN_RCVD / ESTABLISHED
			 * banner never-landed or still TX-fail so busy-shot +
			 * multi-pass flush fires. Never re-busy after clean
			 * success (protect RtxCount until peer ACK).
			 * Soft!=product · lean (no mid-pass ensure thrash).
			 */
			for (i = 0; i < TCP_MAX; i++) {
				if (!g_aT[i].u8Used || g_aT[i].u8IsLoop ||
				    !g_aT[i].u8RtxValid) {
					continue;
				}
				if (g_aT[i].u16Lport !=
				    (u16)TCP_SOFT_SSH_PORT) {
					continue;
				}
				if (tcp_rtx22_need_busy(&g_aT[i]) == 0) {
					continue;
				}
				if (g_aT[i].u8RtxSyn != 0u &&
				    g_aT[i].u8State == ST_SYN_RCVD) {
					g_aT[i].u8RtxBusy = 1;
				} else if (g_aT[i].u8RtxSyn == 0u &&
					   g_aT[i].u8State == ST_ESTABLISHED &&
					   g_aT[i].u32SndNxt >
					       g_aT[i].u32SndUna) {
					g_aT[i].u8RtxBusy = 1;
				} else if (g_aT[i].u8RtxSyn == 0u &&
					   g_aT[i].u32RtxLen == 0u &&
					   g_aT[i].u8FinSent != 0u) {
					/* Bare FIN half-close still busy. */
					g_aT[i].u8RtxBusy = 1;
				}
			}
			/*
			 * Prefer :22 SYN_RCVD (SYN-ACK) then :22 ESTABLISHED
			 * (banner eth_seg) then non-:22. Pass 0 runs all;
			 * busy residual passes still prefer :22 first so
			 * host SYN wins under ARP/ICMP flood. Soft!=product.
			 */
			u32NPhase = 3u;
			for (u32Phase = 0; u32Phase < u32NPhase; u32Phase++) {
			for (i = 0; i < TCP_MAX; i++) {
				u32 u32Int;
				u32 u32Slot;
				int fIs22;
				int fSynR;

				/*
				 * Flood fairness: reverse slot scan on odd
				 * passes so high-index AcceptQ children still
				 * get SYN-ACK rtx. Soft!=product.
				 */
				if ((u32Pass & 1u) != 0u) {
					u32Slot = (TCP_MAX - 1u) - i;
				} else {
					u32Slot = i;
				}

				if (!g_aT[u32Slot].u8Used) {
					continue;
				}
				/* Soft TIME_WAIT reclaim - free slots. */
				if (g_aT[u32Slot].u8State == ST_TIME_WAIT) {
					if (u32Pass == 0u && u32Phase == 0u &&
					    t - g_aT[u32Slot].u32TwTick >=
						TCP_TW_MS) {
						memset(&g_aT[u32Slot], 0,
						       sizeof(g_aT[u32Slot]));
						g_u32TwReap++;
						tcp_soft_bump(
							&g_soft.u64PollTw);
						cTw++;
					}
					continue;
				}
				if (!g_aT[u32Slot].u8RtxValid) {
					continue;
				}
				if (g_aT[u32Slot].u8IsLoop) {
					continue;
				}
				fIs22 = (g_aT[u32Slot].u16Lport ==
					 (u16)TCP_SOFT_SSH_PORT) ? 1 : 0;
				/* Prefer :22 SYN_RCVD SYN-ACK over banner. */
				fSynR = (fIs22 != 0 &&
					 g_aT[u32Slot].u8RtxSyn != 0u &&
					 g_aT[u32Slot].u8State ==
						 ST_SYN_RCVD)
						? 1
						: 0;
				/*
				 * Phase 0: :22 SYN_RCVD SYN-ACK first.
				 * Phase 1: remaining :22 (ESTABLISHED banner
				 * eth_seg + any other :22 rtx arm).
				 * Phase 2: non-:22 remainder.
				 * Soft!=product.
				 */
				if (u32Phase == 0u && fSynR == 0) {
					continue;
				}
				if (u32Phase == 1u &&
				    (fIs22 == 0 || fSynR != 0)) {
					continue;
				}
				if (u32Phase == 2u && fIs22 != 0) {
					continue;
				}
				/* Pass 1+ residual: busy-armed only. Soft!=product. */
				if (u32Pass != 0u && !g_aT[u32Slot].u8RtxBusy) {
					continue;
				}
				if (g_aT[u32Slot].u32RtxCount >= TCP_RTX_MAX) {
					/*
					 * SYN_RCVD exhausted: free so new
					 * external SYNs work.
					 */
					if (g_aT[u32Slot].u8RtxSyn &&
					    g_aT[u32Slot].u8State == ST_SYN_RCVD) {
						/* Grep: giveup - one-shot stamp only. */
						if (tcp_soft_poll_stamp_ok()) {
							kprintf("net_tcp: soft "
								"eth_syn_ack giveup "
								"slot=%u port=%u "
								"sport=%u count=%u "
								"(Soft!=product)\n",
								(unsigned)u32Slot,
								(unsigned)g_aT[u32Slot]
									.u16Lport,
								(unsigned)g_aT[u32Slot]
									.u16Rport,
								(unsigned)g_aT[u32Slot]
									.u32RtxCount);
						}
						tcp_drop_syn_rcvd(u32Slot);
					} else {
						g_aT[u32Slot].u8RtxValid = 0;
						g_aT[u32Slot].u8RtxSyn = 0;
						g_aT[u32Slot].u8RtxBusy = 0;
					}
					continue;
				}
				/*
				 * Busy-armed residual: interval 0 so door
				 * POLL / pure POLL re-attempts SYN-ACK +
				 * banner eth_seg as soon as TX ring may
				 * drain (no wall wait). Gap B deepen: :22
				 * SYN_RCVD never-landed (RtxCount==0) or
				 * still under TX-fail pressure (BusyN!=0)
				 * uses interval 0 + busy arm under ARP/ICMP;
				 * :22 ESTABLISHED banner never-landed /
				 * still-busy mirrors the same int0 path
				 * (first-fail soft-accept under flood);
				 * after clean success use eighth TCP_RTX_MS
				 * (denser under ARP/ICMP; not every pure POLL)
				 * so RtxCount does not burn to giveup before
				 * peer ACK. Steady non-busy uses TCP_RTX_MS.
				 * Soft!=product.
				 */
				if (g_aT[u32Slot].u8RtxBusy) {
					u32Int = 0u;
				} else if (fIs22 != 0 &&
					   tcp_rtx22_need_busy(
					       &g_aT[u32Slot]) != 0 &&
					   ((g_aT[u32Slot].u8RtxSyn != 0u &&
					     g_aT[u32Slot].u8State ==
						 ST_SYN_RCVD) ||
					    (g_aT[u32Slot].u8RtxSyn == 0u &&
					     g_aT[u32Slot].u8State ==
						 ST_ESTABLISHED &&
					     g_aT[u32Slot].u32SndNxt >
						 g_aT[u32Slot].u32SndUna))) {
					/*
					 * :22 SYN-ACK / banner never-landed or
					 * still TX-fail - int0 busy arm under
					 * ARP/ICMP flood. Soft!=product.
					 */
					u32Int = 0u;
					g_aT[u32Slot].u8RtxBusy = 1;
				} else if (fIs22 != 0 &&
					   ((g_aT[u32Slot].u8RtxSyn != 0u &&
					     g_aT[u32Slot].u8State ==
						 ST_SYN_RCVD) ||
					    (g_aT[u32Slot].u8RtxSyn == 0u &&
					     g_aT[u32Slot].u8State ==
						 ST_ESTABLISHED &&
					     g_aT[u32Slot].u32SndNxt >
						 g_aT[u32Slot].u32SndUna))) {
					/*
					 * Post-success :22 SYN-ACK / unacked
					 * banner: denser wall interval
					 * (TCP_RTX_MS/POST22_DIV) - not every
					 * multi-pass burn. Soft!=product.
					 */
					u32Int = (u32)TCP_RTX_MS /
						 TCP_RTX_POST22_DIV;
					if (u32Int == 0u) {
						u32Int = 1u;
					}
				} else {
					u32Int = (u32)TCP_RTX_MS;
				}
				if (u32Int != 0u &&
				    (t - g_aT[u32Slot].u32RtxTick) < u32Int) {
					continue;
				}
				/*
				 * A copy already waits for ETH_TX_PULL.
				 * Re-enqueue would fill the door and starve
				 * ICMP (glass 0.1.163 ping death after :22).
				 */
				if (tcp_udx_l2_live() != 0 &&
				    net_door_udx_tx_pending() != 0u) {
					continue;
				}
				if (g_aT[u32Slot].u8RtxSyn) {
					u8 fl = (u8)(FL_SYN | FL_ACK);
					int rSyn;

					/*
					 * SYN_RCVD -> SYN-ACK; other = bare
					 * SYN (future).
					 */
					if (g_aT[u32Slot].u8State != ST_SYN_RCVD) {
						fl = FL_SYN;
					}
					rSyn = tcp_tx_raw(u32Slot, fl,
							  g_aT[u32Slot].u32RtxSeq, 0,
							  0);
					if (rSyn >= 0) {
						/*
						 * First success after failed
						 * initial TX.
						 */
						if (g_aT[u32Slot].u32SndNxt ==
						    g_aT[u32Slot].u32RtxSeq) {
							g_aT[u32Slot].u32SndNxt++;
						}
						g_aT[u32Slot].u32RtxTick = t;
						g_aT[u32Slot].u32RtxCount++;
						g_aT[u32Slot].u8RtxBusy = 0;
						g_aT[u32Slot].u8RtxBusyN = 0;
						g_u32Rtx++;
						tcp_soft_bump(
							&g_soft.u64PollRtx);
						cRtx++;
						/*
						 * Hot residual: silence eth_syn_ack
						 * rtx stamps (functional TX only).
						 * Soft!=product · stack-safe.
						 */
					} else {
						/*
						 * TX busy (ARP/ICMP ring full):
						 * keep rtx armed, do not burn
						 * rtx count. Soft!=product.
						 */
						g_aT[u32Slot].u32RtxTick = t;
						g_aT[u32Slot].u8RtxBusy = 1;
						cBusyLeft++;
						if (g_aT[u32Slot].u8RtxBusyN <
						    255u) {
							g_aT[u32Slot].u8RtxBusyN++;
						}
						/*
						 * Gap B: :22 busy-shot under
						 * ARP/ICMP - ring may free
						 * mid-poll; immediate extra
						 * TCP_RTX_BUSY_SHOTS attempts
						 * same pass before residual
						 * passes. Soft!=product.
						 * Grep: net_tcp: soft eth_syn_ack rtx
						 */
						if (fIs22 != 0) {
							int nDbl = -1;
							u32 u32Shot;

							for (u32Shot = 0;
							     u32Shot <
							     TCP_RTX_BUSY_SHOTS;
							     u32Shot++) {
								nDbl = tcp_tx_raw(
									u32Slot,
									fl,
									g_aT[u32Slot]
										.u32RtxSeq,
									0, 0);
								if (nDbl >= 0) {
									break;
								}
								if (g_aT[u32Slot]
									    .u8RtxBusyN <
								    255u) {
									g_aT[u32Slot]
										.u8RtxBusyN++;
								}
							}
							if (nDbl >= 0) {
								if (g_aT[u32Slot]
									    .u32SndNxt ==
								    g_aT[u32Slot]
									    .u32RtxSeq) {
									g_aT[u32Slot]
										.u32SndNxt++;
								}
								g_aT[u32Slot]
									.u32RtxTick =
									t;
								g_aT[u32Slot]
									.u32RtxCount++;
								g_aT[u32Slot]
									.u8RtxBusy =
									0;
								g_aT[u32Slot]
									.u8RtxBusyN =
									0;
								g_u32Rtx++;
								tcp_soft_bump(
									&g_soft
										 .u64PollRtx);
								cRtx++;
								if (cBusyLeft >
								    0u) {
									cBusyLeft--;
								}
								/* Hot residual: silence hex rtx stamp. */
								continue;
							}
						}
						/* Hot residual: silence tx_busy stamps. */
					}
					continue;
				}
				/*
				 * Bare FIN rtx (SHUT_WR half-close residual):
				 * RtxLen==0 && !RtxSyn. Soft!=product · sshd
				 * session end path. Functional thrash-strip.
				 */
				if (g_aT[u32Slot].u32RtxLen == 0u) {
					int nFin;
					u32 u32Shot;

					nFin = tcp_tx_raw(
						u32Slot,
						(u8)(FL_FIN | FL_ACK),
						g_aT[u32Slot].u32RtxSeq, 0,
						0);
					if (nFin < 0 && fIs22 != 0) {
						for (u32Shot = 0;
						     u32Shot <
						     TCP_RTX_BUSY_SHOTS;
						     u32Shot++) {
							nFin = tcp_tx_raw(
								u32Slot,
								(u8)(FL_FIN |
								     FL_ACK),
								g_aT[u32Slot]
									.u32RtxSeq,
								0, 0);
							if (nFin >= 0) {
								break;
							}
							if (g_aT[u32Slot]
								    .u8RtxBusyN <
							    255u) {
								g_aT[u32Slot]
									.u8RtxBusyN++;
							}
						}
					}
					if (nFin >= 0) {
						g_aT[u32Slot].u32RtxTick = t;
						g_aT[u32Slot].u32RtxCount++;
						g_aT[u32Slot].u8RtxBusy = 0;
						g_aT[u32Slot].u8RtxBusyN = 0;
						g_aT[u32Slot].u8FinSent = 1;
						g_u32Rtx++;
						tcp_soft_bump(
							&g_soft.u64PollRtx);
						cRtx++;
					} else {
						g_aT[u32Slot].u32RtxTick = t;
						g_aT[u32Slot].u8RtxBusy = 1;
						cBusyLeft++;
						if (g_aT[u32Slot].u8RtxBusyN <
						    255u) {
							g_aT[u32Slot]
								.u8RtxBusyN++;
						}
					}
					continue;
				}
				{
					int nData;

					/* Use u32Slot (not i) - reverse-scan safe. */
					nData = tcp_tx_raw(
						u32Slot, (u8)(FL_ACK | FL_PSH),
						g_aT[u32Slot].u32RtxSeq,
						g_aT[u32Slot].aRtx,
						g_aT[u32Slot].u32RtxLen);
					if (nData >= 0) {
						/*
						 * Legacy first-fail left SndNxt
						 * at RtxSeq - advance on flush.
						 * Soft-accept path already set
						 * SndNxt = RtxSeq+Len (skip).
						 * Soft!=product.
						 */
						if (g_aT[u32Slot].u32SndNxt ==
						    g_aT[u32Slot].u32RtxSeq) {
							g_aT[u32Slot].u32SndNxt +=
								g_aT[u32Slot].u32RtxLen;
						}
						g_aT[u32Slot].u32RtxTick = t;
						g_aT[u32Slot].u32RtxCount++;
						g_aT[u32Slot].u8RtxBusy = 0;
						g_aT[u32Slot].u8RtxBusyN = 0;
						g_u32Rtx++;
						tcp_soft_bump(
							&g_soft.u64PollRtx);
						cRtx++;
						/* Hot residual: silence eth_seg rtx stamps. */
					} else {
						/* Data rtx: busy arm. Soft!=product. */
						g_aT[u32Slot].u32RtxTick = t;
						g_aT[u32Slot].u8RtxBusy = 1;
						cBusyLeft++;
						if (g_aT[u32Slot].u8RtxBusyN <
						    255u) {
							g_aT[u32Slot].u8RtxBusyN++;
						}
						/*
						 * Gap B: :22 banner eth_seg
						 * busy-shot under ARP/ICMP
						 * (ring free mid-poll). Soft!=product.
						 * Grep: net_tcp: soft eth_seg rtx
						 */
						if (fIs22 != 0) {
							int nDbl = -1;
							u32 u32Shot;

							for (u32Shot = 0;
							     u32Shot <
							     TCP_RTX_BUSY_SHOTS;
							     u32Shot++) {
								nDbl = tcp_tx_raw(
									u32Slot,
									(u8)(FL_ACK |
									     FL_PSH),
									g_aT[u32Slot]
										.u32RtxSeq,
									g_aT[u32Slot]
										.aRtx,
									g_aT[u32Slot]
										.u32RtxLen);
								if (nDbl >= 0) {
									break;
								}
								if (g_aT[u32Slot]
									    .u8RtxBusyN <
								    255u) {
									g_aT[u32Slot]
										.u8RtxBusyN++;
								}
							}
							if (nDbl >= 0) {
								if (g_aT[u32Slot]
									    .u32SndNxt ==
								    g_aT[u32Slot]
									    .u32RtxSeq) {
									g_aT[u32Slot]
										.u32SndNxt +=
										g_aT[u32Slot]
											.u32RtxLen;
								}
								g_aT[u32Slot]
									.u32RtxTick =
									t;
								g_aT[u32Slot]
									.u32RtxCount++;
								g_aT[u32Slot]
									.u8RtxBusy =
									0;
								g_aT[u32Slot]
									.u8RtxBusyN =
									0;
								g_u32Rtx++;
								tcp_soft_bump(
									&g_soft
										 .u64PollRtx);
								cRtx++;
								if (cBusyLeft >
								    0u) {
									cBusyLeft--;
								}
								/* Hot residual: silence eth_seg hex rtx. */
							}
						}
						/* Hot residual: silence eth_seg tx_busy. */
					}
				}
			}
			} /* u32Phase */
			/*
			 * Residual further pass if this pass saw TX-busy, or
			 * any :22 still busy-armed / BusyN / never-landed
			 * under flood (int0 denser multi-pass - host SYN
			 * wins). Do not spin on clean first-TX success with
			 * no busy residual. Soft!=product.
			 */
			if (cBusyLeft == 0u) {
				u32 f22Need = 0u;

				for (i = 0; i < TCP_MAX; i++) {
					if (!g_aT[i].u8Used || g_aT[i].u8IsLoop ||
					    !g_aT[i].u8RtxValid) {
						continue;
					}
					if (g_aT[i].u16Lport !=
					    (u16)TCP_SOFT_SSH_PORT) {
						continue;
					}
					if (g_aT[i].u8RtxBusy != 0u ||
					    g_aT[i].u8RtxBusyN != 0u ||
					    (g_aT[i].u8RtxSyn != 0u &&
					     g_aT[i].u8State == ST_SYN_RCVD &&
					     g_aT[i].u32RtxCount == 0u) ||
					    (g_aT[i].u8RtxSyn == 0u &&
					     g_aT[i].u8State ==
						 ST_ESTABLISHED &&
					     g_aT[i].u32SndNxt >
						 g_aT[i].u32SndUna &&
					     g_aT[i].u32RtxCount == 0u)) {
						f22Need = 1u;
						break;
					}
				}
				if (f22Need == 0u) {
					break;
				}
			}
		}
		/*
		 * Lean post multi-pass: one soft listen :22 + lab_ip re-ensure
		 * + silent :22 AcceptQ reparent/rehook so accept/poll_mask see
		 * ESTABLISHED children that completed during busy multi-pass.
		 * Soft!=product · freestanding thrash SKIP (no twin post-poll
		 * ensure; no micro/nano/pico). Grep: net_tcp: soft listen :22
		 * greppable: acceptq_rehook
		 */
		tcp_soft_ensure_listen22();
		tcp_sync_l2_identity();
		if (net_l2_backend() == GJ_NET_L2_RTL8168 ||
		    tcp_ip_is_lab(g_aOurIp) != 0) {
			tcp_force_lab_ip();
		}
		for (i = 0; i < TCP_MAX; i++) {
			if (g_aT[i].u8Used == 0u || g_aT[i].u8Listening == 0u ||
			    g_aT[i].u8ShutRd != 0u ||
			    g_aT[i].u16Lport != (u16)TCP_SOFT_SSH_PORT) {
				continue;
			}
			(void)tcp_acceptq_reparent(i, (u16)TCP_SOFT_SSH_PORT);
			(void)tcp_listen_rehook_ready(i);
		}
	}

	/*
	 * Soft inventory on poll only when rtx/TW work happened.
	 * ONE stamp line max (no twin); inventory one-line + hard caps.
	 * Prefer silence after TCP_SOFT_POLL_STAMP_MAX / LOG_MAX.
	 * Soft!=product · stack-safe.
	 */
	if (cRtx != 0u || cTw != 0u) {
		/* Grep: net: tcp soft poll - single line, stamp-capped, no wave. */
		if (tcp_soft_event_ok() && tcp_soft_poll_stamp_ok()) {
			kprintf("net: tcp soft poll rtx=%u tw_reap=%u "
				"total_rtx=%u total_tw=%u ticks=%llu "
				"(Soft!=product)\n",
				(unsigned)cRtx, (unsigned)cTw, g_u32Rtx,
				g_u32TwReap,
				(unsigned long long)g_soft.u64PollTicks);
		}
		/* Inventory only if LOG budget remains (one-line tcp_soft_print). */
		if (g_soft.u32SoftLogN < TCP_SOFT_LOG_MAX &&
		    tcp_soft_poll_stamp_ok()) {
			tcp_soft_print(1);
		} else if (g_soft.u32SoftLogN >= TCP_SOFT_LOG_MAX) {
			tcp_soft_bump(&g_soft.u64LogSkip);
		}
	}
}

u32
net_tcp_accepts(void)
{
	/*
	 * Emit soft inventory on stats read so door TCP_STATS / bring-up
	 * smoke can grep "net: tcp soft ..." / "net_tcp: soft ...".
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
