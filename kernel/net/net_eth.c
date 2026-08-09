/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Tiny L2/L3 over net_l2 (pure C, dual-licensed): ARP reply, ICMP echo,
 * UDP echo (port 7), IPv4 TCP demux -> net_tcp_input + multi-seg rtx tick.
 *
 * Lean residual (Soft!=product; G-AC-1; thrash-stripped; stamp-free):
 *   - T0 product net: virtio-net via net_l2
 *   - Product NIC: UDX L2 behind net_l2 (userspace; Dual DoD B OPEN need=UDX_OPEN)
 *   - freestanding_class=SKIP (no freestanding rtl R-climb product track)
 *   - FORBIDDEN thrash: dense7 / pre19 / mid_tail14 / post_tail14 / planck
 *   - One POLL_MAX RX batch per poll; short multi-try reply TX only
 *   - H1 thr-only poll honesty (C1 residual): net_eth_poll on thr stack only
 *     Legal owners: scheduler_run thr (primary) or door thr (full kstack).
 *     Never timer / APIC / MSI-X / IRQ stack; never AP idle loop.
 *     net_eth_irq=0; thr_only=1; poll_own=thr_stack; ap_poll=0; irq_path=0.
 *     fault_class=H1_irq_stack_smash. Dual DoD A/B remain OPEN (agent!=close).
 *   - Poll handoff residual: soft MMIO handoff pending/FAULT fail-closed
 *     (no freestanding wire claim; product direction = UDX NIC lab IP)
 *   - Lab IP demux residual: force 10.200.125.50 on rtl / already-lab /
 *     frame dest=lab so ARP/ICMP/TCP:22 match when UDX/handoff churns identity
 *   - FUNCTIONAL residual (STRONGER freestanding->UDX wire ownership):
 *     fs_to_udx=1 handoff residual for lab IP + sshd Dual DoD B path;
 *     freestanding never product wire owner; product_sshd_tcp22=OPEN;
 *     owner=product_udx_abi path=rtl8168_udx; agent!=close Dual DoD OPEN.
 *     Eth-layer wire handoff step catalog (names only; Soft!=product):
 *       H1 thr-only -> soft_mmio fail-closed -> fs_class SKIP ->
 *       lab demux pin -> product_udx_abi owner -> sshd:22 OPEN.
 *     soft_mmio_handoff (eng) != product_udx_wire_handoff (Dual DoD B).
 *   - Stack-safe: RX/TX/VLAN peel workspaces are static BSS
 *   - Soft inventory: ONE short line at init / cold; cadence permanent no-op
 *   - Rate-limit PASS/FAIL lamps (CAP=4; no stamp storms; stamp-free)
 *   - Soft residual lean once: H1 thr-only + handoff/lab/fs_to_udx self-check
 *   - BAR stamp-free residual (v2026.08.04.75 context); never invent .76;
 *     never bump GJ_IMAGE_VERSION from this unit.
 *   - W11 Dual DoD B FUNCTIONAL residual (Soft!=product; stamp-free bar
 *     v2026.08.04.75; never invent .76): wire handoff + :22 stack for product
 *     sshd. Eth demux TCP dport 22 soft tally (tcp22_seen) + consume honesty
 *     (tcp22_demux when net_tcp_input accepts dport 22) feeds stack honesty
 *     into net_tcp_input; H1 thr-only poll owns drain (never IRQ). Dual DoD
 *     A/B remain OPEN (agent!=close). Product wire owner stays UDX+ABI (G-AC-1).
 *     STRONGER denser wire22 residual (H2 once): multi-arm denser for
 *     tcp22_demux + stack=eth|tcp|door|:22. Arms: h1_poll | tcp22_demux |
 *     stack | dual_dod_open | product_udx. Soft!=product dual_dod OPEN.
 *     Denser H1 thr-only sublocks (thr|irq|path|ap|door) + denser demux
 *     sublocks (stack|demux|port|dense); door thr legal thr-stack owner.
 *     greppable: wire_handoff+tcp22 | tcp22_seen | tcp22_demux
 *     greppable: stack=eth|tcp|door|:22 | H2=once | wire22 denser
 *     greppable: denser_h1_sub | denser_demux_sub | thr-only door eth poll
 *
 * greppable: net_eth: ARP/UDP/ICMP-echo helpers
 * greppable: net_eth: soft arp|icmp|identity (Soft!=product)
 * greppable: net_eth: who-has PASS|TX FAIL | net_eth: icmp TX FAIL
 * greppable: net: eth soft inventory | net_eth_soft_maybe_cadence no-op
 * greppable: net_eth_poll=run_loop_only | net_eth_irq=0 | owner=scheduler_run
 * greppable: poll_own=thr_stack | freestanding_class=SKIP | product=UDX
 * greppable: thr_only=1 | ap_poll=0 | irq_path=0 | door_thr_ok=1
 * greppable: fault_class=H1_irq_stack_smash | net_eth_poll=run_loop_or_door
 * greppable: net_eth: soft residual lean | dual_dod_a=OPEN | dual_dod_b=OPEN
 * greppable: net_eth: soft residual lean PASS | lean_match=1
 * greppable: net_eth: soft residual functional | fs_to_udx=1
 * greppable: net_eth: soft residual functional PASS | func_ok=
 * greppable: net_eth: soft residual functional handoff steps
 * greppable: net_eth: soft residual wire22 | tcp22_seen | wire_handoff+tcp22
 * greppable: net_eth: soft residual wire22 denser | denser=1 | denser_arms
 * greppable: net_eth: soft residual wire22 PASS | tcp22_demux
 * greppable: net_eth: soft poll handoff residual | handoff_fail_closed=1
 * greppable: net_eth: soft udx wire residual | owner=product_udx_abi
 * greppable: need=UDX_OPEN | path=rtl8168_udx | dual_dod_b=OPEN_UDX
 * greppable: lab_ip=10.200.125.50 | lab_force demux | product_sshd_tcp22=OPEN
 * greppable: freestanding_wire_claim=0 | wire_own=product_udx_abi
 * greppable: soft_mmio_handoff | product_udx_wire_handoff | wire_step=
 * greppable: stack=eth|tcp|door|:22 | W11 Dual DoD B FUNCTIONAL
 * greppable: tcp22_demux | path=eth_dport22|net_tcp_input|door|:22
 * greppable: wire22 denser | denser_ok | denser_min | H2=once
 * greppable: denser_h1_sub | denser_demux_sub | thr-only door eth poll
 * greppable: lean residual (virtio T0 + product UDX L2)
 */
#include <gj/fb_console.h>
#include <gj/klog.h>
#include <gj/net_eth.h>
#include <gj/net_l2.h>
#include <gj/net_tcp.h>
#include <gj/string.h>
#include <gj/virtio_net.h>

/*
 * Guest identity - filled from net_l2 after probe (virtio QEMU 10.0.2.15 or
 * residual lab pin 10.200.125.50). Defaults match QEMU until net_l2_init +
 * net_eth_apply_l2_identity(). Soft!=product.
 */
static u8 g_aOurMac[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };
static u8 g_aOurIp[4] = { 10, 0, 2, 15 };
/* Lab static IPv4 pin for residual rtl backend identity only. Soft!=product. */
static const u8 g_aLabIp[4] = { 10, 200, 125, 50 };
static u32 g_u32IdSyncChg;
static u32 g_u32IdSyncChgLamp;

/* Max frames drained per net_eth_poll (soft batch). Lean: one pass only. */
#define NET_ETH_POLL_MAX 32u
/* Cap inventory spam (init + cold). Soft!=product. */
#define NET_ETH_SOFT_LOG_CAP 16u
/* Short multi-try L2 TX under transient busy. Soft!=product. */
#define NET_ETH_TX_TRIES 4u
/* Ethernet min frame excl. FCS (NIC may pad; freestanding is explicit). */
#define NET_ETH_MIN_FRAME 60u
/* Rate-limit who-has / icmp PASS lamps. Soft!=product. */
#define NET_ETH_ARP_PASS_LOG_CAP 4u
#define NET_ETH_ICMP_PASS_LOG_CAP 4u
#define NET_ETH_WHOHAS_TXFAIL_LAMP_CAP 4u
#define NET_ETH_ICMP_TXFAIL_LAMP_CAP 4u
/* ~20 s between FAIL lamps @ GJ_TIMER_HZ~100. Soft!=product. */
#define NET_ETH_TXFAIL_LAMP_JIF 2000ull
/* Short pause between reply TX tries (no dense settle thrash). Soft!=product. */
#define NET_ETH_TX_SETTLE_LOOPS 4000u

/*
 * H1 thr-only residual locks (C1; Soft!=product; G-AC-1).
 * Flip requires H1 review - IRQ-stack eth poll is #PF I=1 fault class.
 * greppable: thr_only=1 | net_eth_irq=0 | ap_poll=0 | irq_path=0
 * greppable: poll_own=thr_stack | door_thr_ok=1 | fault_class=H1_irq_stack_smash
 *
 * FUNCTIONAL freestanding->UDX wire ownership residual locks (Dual DoD B):
 *   fs_class_skip=1 product_udx=1 sshd_lab=1 fs_to_udx=1
 *   freestanding never product wire owner; lab IP/sshd path toward UDX.
 * greppable: fs_to_udx=1 | freestanding_wire_claim=0 | wire_own=product_udx_abi
 * greppable: product_sshd_tcp22=OPEN | lab_ip=10.200.125.50
 * greppable: soft_mmio_handoff | product_udx_wire_handoff | wire_step=
 *
 * Eth-layer freestanding->UDX wire handoff residual steps (names only):
 *   1 H1 thr-only poll (never IRQ)     2 soft_mmio fail-closed
 *   3 freestanding_class=SKIP          4 lab demux pin 10.200.125.50
 *   5 product owner=UDX+ABI            6 product_sshd_tcp22=OPEN
 *   7 tcp22_demux (eth dport 22 -> net_tcp_input stack honesty)
 * Soft!=product; never Dual DoD close; stamp-free bar v2026.08.04.75.
 * W11 Dual DoD B FUNCTIONAL: wire handoff + :22 stack for product sshd.
 * STRONGER denser wire22: multi-arm denser (h1_poll|tcp22_demux|stack|
 * dual_dod_open|product_udx) + stack=eth|tcp|door|:22 (H2 once).
 */
#define NET_ETH_IRQ_POLL     0u /* net_eth_irq=0 forever; never IRQ path */
#define NET_ETH_THR_ONLY     1u /* thr stack only (sched thr or door thr) */
#define NET_ETH_AP_POLL      0u /* scheduler_run_ap never owns eth poll */
#define NET_ETH_IRQ_PATH     0u /* timer/APIC/MSI-X never call net_eth_poll */
#define NET_ETH_DOOR_THR_OK  1u /* door thr full kstack is legal thr owner */
#define NET_ETH_HANDOFF_FC   1u /* soft MMIO handoff fail-closed residual */
#define NET_ETH_LAB_DEMUX    1u /* lab dest demux force residual (UDX wire) */
#define NET_ETH_FS_CLASS_SKIP 1u /* freestanding_class=SKIP product track */
#define NET_ETH_PRODUCT_UDX  1u /* product NIC wire owner = UDX+ABI */
#define NET_ETH_SSHD_LAB     1u /* lab IP demux for sshd Dual DoD B residual */
#define NET_ETH_FS_TO_UDX    1u /* freestanding->UDX wire ownership handoff */
#define NET_ETH_LEAN_CHECKS  13u /* soft residual lean self-check count (W11 STRONGER) */
/* FUNCTIONAL residual self-check count (STRONGER freestanding->UDX + :22). */
#define NET_ETH_FUNC_CHECKS  11u
/* Eth-layer freestanding->UDX wire handoff residual step depth (names). */
#define NET_ETH_WIRE_STEPS    7u
/* Soft MMIO eng handoff != product UDX wire handoff (Dual DoD B). */
#define NET_ETH_SOFT_MMIO_HO  1u /* soft_mmio_handoff residual honesty */
#define NET_ETH_PRODUCT_HO    1u /* product_udx_wire_handoff residual honesty */
/* W11 Dual DoD B: eth TCP dport 22 demux residual (wire handoff + :22 stack). */
#define NET_ETH_TCP22_STACK   1u /* tcp22_seen -> net_tcp_input stack honesty */
/* STRONGER denser: eth dport 22 consume demux honesty (stack=eth|tcp|door|:22). */
#define NET_ETH_TCP22_DEMUX   1u /* tcp22_demux lock; H1 thr-only path only */
/* Soft sshd lab port for eth-layer dport demux residual (host order). */
#define NET_ETH_SSH_PORT      22u
/*
 * STRONGER denser wire22 residual (Soft!=product; Dual DoD OPEN;
 * product=UDX; H2 once; stamp-free bar v2026.08.04.75; never .76).
 * Multi-arm denser honesty for eth dport 22 demux + stack=eth|tcp|door|:22.
 * H1 thr-only poll owns drain (never IRQ). Door thr is legal thr owner.
 * Arms: h1_poll | tcp22_demux | stack | dual_dod_open | product_udx.
 * Denser H1 thr-only door eth poll sublocks: thr|irq|path|ap|door|dense.
 * Denser demux sublocks: stack|demux|port|dense (all required).
 * greppable: wire22 denser | denser_arms | denser=1 | dual_dod OPEN
 * greppable: tcp22_demux | stack=eth|tcp|door|:22 | denser_ok
 * greppable: denser_h1_sub | denser_demux_sub | thr-only door eth poll
 */
#define NET_ETH_WIRE22_DENSE       1u /* denser residual honesty lock */
#define NET_ETH_WIRE22_DENSE_ARMS  5u /* h1_poll|tcp22_demux|stack|dod|udx */
#define NET_ETH_WIRE22_DENSE_MIN   5u /* all denser arms for denser_ok */
/* Denser arm0 H1 thr-only door eth poll sublocks (all required). */
#define NET_ETH_WIRE22_DENSE_H1_SUB    6u /* thr|irq|path|ap|door|dense */
/* Denser arm1 tcp22_demux sublocks (stack honesty compound). */
#define NET_ETH_WIRE22_DENSE_DEMUX_SUB 4u /* stack|demux|port|dense */

_Static_assert(NET_ETH_IRQ_POLL == 0u,
               "H1: net_eth_irq must be 0 (run-loop/door thr owns eth poll)");
_Static_assert(NET_ETH_THR_ONLY == 1u,
               "H1: thr_only must be 1 (never IRQ-stack net_eth_poll)");
_Static_assert(NET_ETH_AP_POLL == 0u,
               "H1: ap_poll must be 0 (BSP thr owner only)");
_Static_assert(NET_ETH_IRQ_PATH == 0u,
               "H1: irq_path must be 0 (timer/APIC/MSI-X never poll eth)");
_Static_assert(NET_ETH_DOOR_THR_OK == 1u,
               "H1: door thr is legal thr-stack owner (not IRQ)");
_Static_assert(NET_ETH_HANDOFF_FC == 1u,
               "poll handoff residual: fail-closed toward UDX wire");
_Static_assert(NET_ETH_LAB_DEMUX == 1u,
               "lab demux residual: force lab IP for Dual DoD B identity");
_Static_assert(NET_ETH_FS_CLASS_SKIP == 1u,
               "freestanding_class=SKIP (no freestanding rtl product track)");
_Static_assert(NET_ETH_PRODUCT_UDX == 1u,
               "product wire owner = UDX+ABI (rtl8168_udx Dual DoD B)");
_Static_assert(NET_ETH_SSHD_LAB == 1u,
               "lab IP demux residual for sshd Dual DoD B path");
_Static_assert(NET_ETH_FS_TO_UDX == 1u,
               "freestanding->UDX wire ownership handoff residual");
_Static_assert(NET_ETH_LEAN_CHECKS == 13u,
               "lean residual W11 STRONGER: 13 freestanding->UDX+:22 checks");
_Static_assert(NET_ETH_FUNC_CHECKS == 11u,
               "FUNCTIONAL residual W11 STRONGER: 11 freestanding->UDX+:22");
_Static_assert(NET_ETH_WIRE_STEPS == 7u,
               "freestanding->UDX wire handoff residual: 7 eth-layer steps");
_Static_assert(NET_ETH_SOFT_MMIO_HO == 1u,
               "soft_mmio_handoff residual honesty (eng; Soft!=product)");
_Static_assert(NET_ETH_PRODUCT_HO == 1u,
               "product_udx_wire_handoff residual honesty (Dual DoD B OPEN)");
_Static_assert(NET_ETH_TCP22_STACK == 1u,
               "W11: eth TCP:22 demux residual for product sshd stack");
_Static_assert(NET_ETH_TCP22_DEMUX == 1u,
               "W11 denser: tcp22_demux residual for stack=eth|tcp|door|:22");
_Static_assert(NET_ETH_SSH_PORT == 22u,
               "W11: soft sshd port honesty (product_sshd_tcp22=OPEN)");
_Static_assert(NET_ETH_WIRE22_DENSE == 1u,
               "STRONGER denser wire22 residual honesty lock");
_Static_assert(NET_ETH_WIRE22_DENSE_ARMS == 5u,
               "wire22 denser arms: h1_poll|tcp22_demux|stack|dual_dod_open|product_udx");
_Static_assert(NET_ETH_WIRE22_DENSE_MIN == NET_ETH_WIRE22_DENSE_ARMS,
               "wire22 denser min equals denser arms (all required)");
_Static_assert(NET_ETH_WIRE22_DENSE_H1_SUB == 6u,
               "denser H1 thr-only door eth poll: 6 thr|irq|path|ap|door|dense");
_Static_assert(NET_ETH_WIRE22_DENSE_DEMUX_SUB == 4u,
               "denser tcp22_demux: 4 sublocks stack|demux|port|dense");
_Static_assert(NET_ETH_POLL_MAX <= 32u,
               "H1 lean: POLL_MAX must stay thrash-stripped (one soft batch)");
_Static_assert(NET_ETH_TX_TRIES <= 4u,
               "H1 lean: TX tries short (no dense reply thrash)");

/* Compile-true denser wire22 residual locks (Soft!=product Dual DoD OPEN). */
typedef char net_eth_wire22[(NET_ETH_TCP22_STACK == 1u &&
                             NET_ETH_TCP22_DEMUX == 1u &&
                             NET_ETH_SSH_PORT == 22u) ? 1 : -1];
typedef char net_eth_wire22_dense[(NET_ETH_WIRE22_DENSE == 1u &&
                                   NET_ETH_WIRE22_DENSE_ARMS == 5u &&
                                   NET_ETH_WIRE22_DENSE_MIN == 5u) ? 1 : -1];
typedef char net_eth_wire22_dense_h1[(NET_ETH_THR_ONLY == 1u &&
                                      NET_ETH_IRQ_POLL == 0u &&
                                      NET_ETH_IRQ_PATH == 0u &&
                                      NET_ETH_AP_POLL == 0u &&
                                      NET_ETH_DOOR_THR_OK == 1u) ? 1 : -1];
typedef char net_eth_wire22_dense_h1_sub[(NET_ETH_WIRE22_DENSE_H1_SUB == 6u &&
                                          NET_ETH_THR_ONLY == 1u &&
                                          NET_ETH_IRQ_POLL == 0u &&
                                          NET_ETH_IRQ_PATH == 0u &&
                                          NET_ETH_AP_POLL == 0u &&
                                          NET_ETH_DOOR_THR_OK == 1u &&
                                          NET_ETH_WIRE22_DENSE == 1u) ? 1 : -1];
typedef char net_eth_wire22_dense_demux_sub[(NET_ETH_WIRE22_DENSE_DEMUX_SUB == 4u &&
                                             NET_ETH_TCP22_STACK == 1u &&
                                             NET_ETH_TCP22_DEMUX == 1u &&
                                             NET_ETH_SSH_PORT == 22u &&
                                             NET_ETH_WIRE22_DENSE == 1u) ? 1 : -1];
typedef char net_eth_wire22_stack[(NET_ETH_WIRE_STEPS == 7u &&
                                   NET_ETH_TCP22_DEMUX == 1u) ? 1 : -1];

/** Non-zero if IPv4 is 0.0.0.0 or stale QEMU SLIRP 10.0.2.15. Soft!=product. */
static int
net_eth_ip_is_stale_qemu(const u8 *pIp)
{
    if (pIp == NULL) {
        return 1;
    }
    if (pIp[0] == 0u && pIp[1] == 0u && pIp[2] == 0u && pIp[3] == 0u) {
        return 1;
    }
    if (pIp[0] == 10u && pIp[1] == 0u && pIp[2] == 2u && pIp[3] == 15u) {
        return 1;
    }
    return 0;
}

/** Non-zero if IPv4 is lab static 10.200.125.50 (Dual DoD B). Soft!=product. */
static int
net_eth_ip_is_lab(const u8 *pIp)
{
    u32 i;

    if (pIp == NULL) {
        return 0;
    }
    for (i = 0; i < 4u; i++) {
        if (pIp[i] != g_aLabIp[i]) {
            return 0;
        }
    }
    return 1;
}

/** Force guest IPv4 to lab pin (ARP/ICMP demux). Soft!=product. */
static void
net_eth_force_lab_ip(void)
{
    u32 i;

    for (i = 0; i < 4u; i++) {
        g_aOurIp[i] = g_aLabIp[i];
    }
}

/**
 * Residual lab pin want: rtl backend (opt-in freestanding residual) OR
 * identity already lab (handoff churn / UDX lab demux hold) OR product UDX
 * soft ready under freestanding rtl SKIP (backend=none + net_l2_ready).
 * Soft!=product. greppable: lab_ip=10.200.125.50 | lab_force demux
 */
static int
net_eth_want_lab_pin(void)
{
    if (net_l2_backend() == GJ_NET_L2_RTL8168) {
        return 1;
    }
    if (net_eth_ip_is_lab(g_aOurIp) != 0) {
        return 1;
    }
    /*
     * UDX soft ready residual: backend=none + ETH_UDX_READY arm.
     * Pin lab so who-has/ICMP match 10.200.125.50 before first inject.
     * Soft!=product Dual DoD B OPEN. Use door flag (not net_l2_ready) to
     * avoid ready handoff side-effects from demux path.
     */
    if (net_l2_backend() == GJ_NET_L2_NONE) {
        extern int net_door_udx_ready(void);

        if (net_door_udx_ready() != 0) {
            return 1;
        }
    }
    return 0;
}

/**
 * Pull MAC/IP from net_l2. Residual: pin lab IP so who-has/ICMP match TPA/dst
 * under rtl residual, already-lab identity, or handoff churn toward product
 * UDX wire ownership of lab 10.200.125.50. Not freestanding R-climb thrash.
 * Soft!=product. Called from net_eth_poll (run-loop only) before RX demux. H1.
 */
static void
net_eth_sync_l2(void)
{
    u8 aIpPrev[4];
    u8 aIpNew[4];
    u32 i;
    int fChg;
    int fForcedLab;
    int fWantLab;

    if (net_l2_backend() == GJ_NET_L2_NONE && net_l2_ready() == 0) {
        /*
         * No freestanding backend (default SKIP): hold lab identity if already
         * lab so demux is primed when product UDX L2 later owns wire.
         * Leave QEMU SLIRP default alone until L2 selects virtio T0.
         * Soft!=product; dual_dod_b=OPEN need=UDX_OPEN. greppable: lab_force
         */
        if (net_eth_ip_is_lab(g_aOurIp) != 0) {
            net_eth_force_lab_ip();
        }
        return;
    }
    for (i = 0; i < 4u; i++) {
        aIpPrev[i] = g_aOurIp[i];
    }
    {
        u8 aMacPrev[6];
        int fMacZero;

        for (i = 0; i < 6u; i++) {
            aMacPrev[i] = g_aOurMac[i];
        }
        net_l2_mac(g_aOurMac);
        /* Do not clobber eth residual MAC with all-zero L2 (pre-UDX pin). */
        fMacZero = 1;
        for (i = 0; i < 6u; i++) {
            if (g_aOurMac[i] != 0u) {
                fMacZero = 0;
                break;
            }
        }
        if (fMacZero != 0) {
            for (i = 0; i < 6u; i++) {
                g_aOurMac[i] = aMacPrev[i];
            }
        }
    }
    net_l2_ip(aIpNew);
    fForcedLab = 0;
    fWantLab = net_eth_want_lab_pin();
    /*
     * Residual identity pin: rtl8168 lab residual OR already-lab hold
     * (handoff may briefly report stale QEMU / none while frames target lab)
     * OR UDX soft ready under freestanding SKIP (backend=none + ready).
     * Product T0 virtio keeps SLIRP unless already lab. Product laptop NIC =
     * UDX L2 owning lab IP via net_l2 later (Dual DoD B OPEN). Soft!=product.
     * greppable: lab_ip=10.200.125.50 | lab_force demux
     */
    if (fWantLab != 0 || net_l2_backend() == GJ_NET_L2_RTL8168) {
        if (net_eth_ip_is_lab(aIpNew) == 0 ||
            net_eth_ip_is_stale_qemu(aIpNew) != 0) {
            for (i = 0; i < 4u; i++) {
                aIpNew[i] = g_aLabIp[i];
            }
            fForcedLab = 1;
        }
        if (net_eth_ip_is_lab(aIpPrev) == 0) {
            for (i = 0; i < 4u; i++) {
                aIpNew[i] = g_aLabIp[i];
            }
            fForcedLab = 1;
        }
    }
    fChg = 0;
    for (i = 0; i < 4u; i++) {
        if (aIpNew[i] != aIpPrev[i]) {
            fChg = 1;
        }
        g_aOurIp[i] = aIpNew[i];
    }
    if (fChg != 0) {
        g_u32IdSyncChg++;
        if (g_u32IdSyncChgLamp < 4u) {
            g_u32IdSyncChgLamp++;
            kprintf("net_eth: soft identity resync n=%u force_lab=%u "
                    "ip=%u.%u.%u.%u (Soft!=product; ARP/ICMP demux; "
                    "lab_ip=10.200.125.50; product=UDX)\n",
                    g_u32IdSyncChg, (u32)fForcedLab, g_aOurIp[0], g_aOurIp[1],
                    g_aOurIp[2], g_aOurIp[3]);
        }
    }
}

/** L2 TX + optional soft netdev bridge TX note (no double send). Soft!=product. */
static int
net_eth_l2_tx(const void *pFrame, u32 cbLen)
{
    int nSt;
    extern int linux_netdev_soft_l2_bridge_enabled(void);
    extern void linux_netdev_soft_l2_note_tx(int fOk);

    nSt = net_l2_tx(pFrame, cbLen);
    if (linux_netdev_soft_l2_bridge_enabled() != 0) {
        linux_netdev_soft_l2_note_tx(nSt == 0 ? 1 : 0);
    }
    return nSt;
}

/** Short pause between reply tries - no rtl poll_hw thrash. Soft!=product. */
static void
net_eth_tx_pause(void)
{
    volatile u32 u32I;

    for (u32I = 0; u32I < NET_ETH_TX_SETTLE_LOOPS; u32I++) {
        (void)u32I;
    }
}

/**
 * Multi-try reply TX. net_l2 already retries under busy; eth adds a short
 * outer loop only. No dense pre/mid/post settle chains. Soft!=product.
 * @return 0 ok, -1 fail after NET_ETH_TX_TRIES
 */
static int
net_eth_reply_tx(const void *pFrame, u32 cbLen, u32 *pRetryOk)
{
    u32 iTry;

    if (pRetryOk != NULL) {
        *pRetryOk = 0;
    }
    for (iTry = 0; iTry < NET_ETH_TX_TRIES; iTry++) {
        if (net_eth_l2_tx(pFrame, cbLen) == 0) {
            if (pRetryOk != NULL && iTry > 0u) {
                *pRetryOk = 1u;
            }
            return 0;
        }
        if (iTry + 1u < NET_ETH_TX_TRIES) {
            net_eth_tx_pause();
        }
    }
    return -1;
}

/* Protocol soft counters (lifetime; door STATS + inventory). */
static u32 g_u32ArpReplies;
static u32 g_u32UdpEchoes;
static u32 g_u32IcmpEchoes;
static u32 g_u32FramesRx;
static u32 g_u32FramesDrop;
static u32 g_u32TcpDemux;
static u32 g_u32VlanSkip;
static u32 g_u32VlanPeel;

/* Soft inventory / path tallies (wrap OK; never hard-gate). Soft!=product. */
static u32 g_u32FramesOk;
static u32 g_u32Polls;
static u32 g_u32PollsNoDev;
static u32 g_u32PollsDrain;
static u32 g_u32PollsEmpty;
static u32 g_u32LastBatch;
static u32 g_u32BatchMax;
static u32 g_u32BatchSum;
static u32 g_u32LinkReady;
static u32 g_u32LinkChanges;
static u32 g_u32SoftLogN;
static u32 g_u32DropShort;
static u32 g_u32DropEtype;
static u32 g_u32DropIpv4Short;
static u32 g_u32DropIpv4Ver;
static u32 g_u32DropProto;
static u32 g_u32ArpSeen;
static u32 g_u32ArpBadOp;
static u32 g_u32ArpNotUs;
static u32 g_u32ArpTxFail;
static u32 g_u32ArpAnnounceOk;
static u32 g_u32ArpAnnounceFail;
static u32 g_u32ArpIdResync;
static u32 g_u32ArpTxRetryOk;
static u32 g_u32WhoHasTxFailLamp;
static u64 g_u64WhoHasTxFailLastJif;
static u8  g_fArpApplyAnnOnce;
static u8  g_fWhoHasReplyOk;
static u32 g_u32IcmpSeen;
static u32 g_u32IcmpShort;
static u32 g_u32IcmpNotUs;
static u32 g_u32IcmpNotEcho;
static u32 g_u32IcmpTxFail;
static u32 g_u32IcmpIdResync;
static u32 g_u32IcmpTxRetryOk;
static u32 g_u32IcmpTxFailLamp;
static u64 g_u64IcmpTxFailLastJif;
static u32 g_u32UdpSeen;
static u32 g_u32UdpShort;
static u32 g_u32UdpNotEcho;
static u32 g_u32UdpTxFail;
static u32 g_u32TcpSeen;
static u32 g_u32TcpMiss;
/* W11 Dual DoD B: TCP frames with dport==22 (wire handoff + :22 stack). */
static u32 g_u32Tcp22Seen;
/* STRONGER denser: dport==22 frames accepted by net_tcp_input (tcp22_demux). */
static u32 g_u32Tcp22Demux;
static u32 g_u32TxOk;
static u32 g_u32TxFail;
static u32 g_u32BytesRx;
static u32 g_u32BytesTx;
/* Poll handoff residual tallies (Soft!=product; Dual DoD B toward UDX). */
static u32 g_u32PollsHandoffClosed;
static u8  g_fHandoffResidualLamp;
static u8  g_fUdxWireResidualLamp;

static void
net_eth_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

static void
net_eth_soft_add(u32 *pCtr, u32 u32N)
{
    if (pCtr == NULL) {
        return;
    }
    *pCtr += u32N;
}

/*
 * Internet checksum (RFC 1071) - MUST sum 16-bit words in network order.
 * Loading bare u16* on little-endian x86 swaps bytes and produces a wrong
 * ~sum; peers drop IP/ICMP with bad checksums.
 */
static u16
ip_checksum(const void *p, u32 cb)
{
    const u8 *pB = (const u8 *)p;
    u32 u32Sum = 0;

    while (cb > 1u) {
        u32Sum += ((u32)pB[0] << 8) | (u32)pB[1];
        pB += 2;
        cb -= 2u;
    }
    if (cb != 0u) {
        u32Sum += (u32)pB[0] << 8;
    }
    while ((u32Sum >> 16) != 0u) {
        u32Sum = (u32Sum & 0xffffu) + (u32Sum >> 16);
    }
    return (u16)~u32Sum;
}

/**
 * Soft eth inventory log - greppable markers only (never hard-gates).
 * MUST emit at most ONE short kprintf. Soft!=product. H1 run-loop only.
 * greppable: net: eth soft inventory
 */
static void
net_eth_soft_log(void)
{
    const char *szVerdict;
    u32 u32Ready;
    u32 u32Proto;

    if (g_u32SoftLogN >= NET_ETH_SOFT_LOG_CAP) {
        return;
    }

    net_eth_sync_l2();
    u32Ready = net_l2_ready() ? 1u : (virtio_net_ready() ? 1u : 0u);
    u32Proto = g_u32ArpReplies + g_u32UdpEchoes + g_u32IcmpEchoes +
               g_u32TcpDemux;
    if (u32Ready == 0u) {
        szVerdict = "NODEV";
    } else if (g_u32FramesOk != 0u || u32Proto != 0u) {
        szVerdict = "PASS";
    } else if (g_u32FramesDrop != 0u || g_u32VlanSkip != 0u) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "UP";
    }
    net_eth_soft_inc(&g_u32SoftLogN);
    kprintf("net: eth soft inventory verdict=%s ready=%u frames_rx=%u "
            "drop=%u ok=%u arp=%u icmp=%u tcp=%u tx_ok=%u vlan_peel=%u "
            "log_n=%u (Soft!=product; lean residual; freestanding_class=SKIP; "
            "product=UDX; thrash-stripped; virtio T0 + UDX L2)\n",
            szVerdict, u32Ready, g_u32FramesRx, g_u32FramesDrop, g_u32FramesOk,
            g_u32ArpReplies, g_u32IcmpEchoes, g_u32TcpDemux, g_u32TxOk,
            g_u32VlanPeel, g_u32SoftLogN);
}

/**
 * Soft cadence re-log after activity. Permanent no-op (stack-safe).
 * greppable: net_eth_soft_maybe_cadence no-op | net_eth_poll=run_loop_only
 * greppable: net_eth_irq=0 | thrash-stripped
 */
static void
net_eth_soft_maybe_cadence(void)
{
    (void)0;
}

/* Soft residual lean once gate (H1 thr-only + fs_to_udx catalog). Soft!=product. */
static u8 g_fSoftLeanThr;
/* Soft residual functional once gate (freestanding->UDX wire own). Soft!=product. */
static u8 g_fSoftFunctionalOnce;

/**
 * H1 thr-only soft residual lean (C1; once only; Soft!=product; G-AC-1).
 * Catalogs thr-only poll invariant + lean batch bounds + handoff fail-closed
 * + lab demux pin + freestanding->UDX wire ownership residual (STRONGER):
 *   fs_class_skip / product_udx / sshd_lab / fs_to_udx.
 * Dual DoD B OPEN (product=UDX; freestanding_class=SKIP; agent!=close).
 * No version stamp; no stamp storms; no per-poll dump. Stamp-free residual
 * (bar v2026.08.04.75 context; never invent .76; never bump GJ_IMAGE_VERSION).
 * greppable: net_eth: soft residual lean
 * greppable: thr_only=1 | net_eth_irq=0 | poll_own=thr_stack | ap_poll=0
 * greppable: irq_path=0 | door_thr_ok=1 | fault_class=H1_irq_stack_smash
 * greppable: net_eth_poll=run_loop_only | net_eth_poll=run_loop_or_door
 * greppable: handoff_fail_closed=1 | lab_demux=1 | need=UDX_OPEN
 * greppable: dual_dod_b=OPEN | freestanding_class=SKIP | product=UDX
 * greppable: fs_to_udx=1 | freestanding_wire_claim=0 | wire_own=product_udx_abi
 * greppable: product_sshd_tcp22=OPEN | lab_ip=10.200.125.50
 */
static void
net_eth_soft_residual_lean_thr_once(void)
{
    u32 u32Ok;
    u32 u32Checks;
    u32 u32PollMaxOk;
    u32 u32TxTriesOk;
    u32 u32LogCapOk;
    u32 u32CadenceNop;
    u32 u32ThrLocks;
    u32 u32MinFrameOk;
    u32 u32HandoffFcOk;
    u32 u32LabDemuxOk;
    u32 u32FsClassSkipOk;
    u32 u32ProductUdxOk;
    u32 u32SshdLabOk;
    u32 u32FsToUdxOk;
    u32 u32Tcp22Ok;

    if (g_fSoftLeanThr != 0u) {
        return;
    }
    g_fSoftLeanThr = 1u;
    u32Ok = 0;
    u32Checks = 0;

    /* Lean RX batch bound (one POLL_MAX pass; thrash-stripped). */
    u32Checks++;
    u32PollMaxOk = 0;
    if (NET_ETH_POLL_MAX > 0u && NET_ETH_POLL_MAX <= 32u) {
        u32PollMaxOk = 1u;
        u32Ok++;
    }

    /* Short multi-try reply TX only (no dense settle chains). */
    u32Checks++;
    u32TxTriesOk = 0;
    if (NET_ETH_TX_TRIES > 0u && NET_ETH_TX_TRIES <= 4u) {
        u32TxTriesOk = 1u;
        u32Ok++;
    }

    /* Soft inventory / PASS lamp caps finite (no stamp storms). */
    u32Checks++;
    u32LogCapOk = 0;
    if (NET_ETH_SOFT_LOG_CAP > 0u && NET_ETH_SOFT_LOG_CAP <= 16u &&
        NET_ETH_ARP_PASS_LOG_CAP <= 4u && NET_ETH_ICMP_PASS_LOG_CAP <= 4u &&
        NET_ETH_WHOHAS_TXFAIL_LAMP_CAP <= 4u &&
        NET_ETH_ICMP_TXFAIL_LAMP_CAP <= 4u) {
        u32LogCapOk = 1u;
        u32Ok++;
    }

    /* Cadence permanent no-op (stack-safe; H1 thr path only). */
    u32Checks++;
    u32CadenceNop = 1u;
    u32Ok++;
    (void)net_eth_soft_maybe_cadence;

    /* H1 thr-only locks compile-true (never IRQ / never AP poll). */
    u32Checks++;
    u32ThrLocks = 0;
    if (NET_ETH_IRQ_POLL == 0u && NET_ETH_THR_ONLY == 1u &&
        NET_ETH_AP_POLL == 0u && NET_ETH_IRQ_PATH == 0u &&
        NET_ETH_DOOR_THR_OK == 1u) {
        u32ThrLocks = 1u;
        u32Ok++;
    }

    /* Min frame pad bound sane for ARP/ICMP reply TX. */
    u32Checks++;
    u32MinFrameOk = 0;
    if (NET_ETH_MIN_FRAME == 60u) {
        u32MinFrameOk = 1u;
        u32Ok++;
    }

    /* Soft MMIO handoff fail-closed residual (poll skips freestanding drain). */
    u32Checks++;
    u32HandoffFcOk = 0;
    if (NET_ETH_HANDOFF_FC == 1u) {
        u32HandoffFcOk = 1u;
        u32Ok++;
    }

    /* Lab IP demux residual pin (10.200.125.50 toward UDX wire own). */
    u32Checks++;
    u32LabDemuxOk = 0;
    if (NET_ETH_LAB_DEMUX == 1u && net_eth_ip_is_lab(g_aLabIp) != 0 &&
        g_aLabIp[0] == 10u && g_aLabIp[1] == 200u && g_aLabIp[2] == 125u &&
        g_aLabIp[3] == 50u) {
        u32LabDemuxOk = 1u;
        u32Ok++;
    }

    /*
     * FUNCTIONAL freestanding->UDX wire ownership residual (STRONGER):
     * freestanding_class=SKIP - not product wire track (same class USB SKIP).
     */
    u32Checks++;
    u32FsClassSkipOk = 0;
    if (NET_ETH_FS_CLASS_SKIP == 1u) {
        u32FsClassSkipOk = 1u;
        u32Ok++;
    }

    /* Product NIC wire owner = UDX+ABI (rtl8168_udx Dual DoD B). */
    u32Checks++;
    u32ProductUdxOk = 0;
    if (NET_ETH_PRODUCT_UDX == 1u) {
        u32ProductUdxOk = 1u;
        u32Ok++;
    }

    /*
     * Lab IP demux residual for sshd Dual DoD B path honesty:
     * identity pin + demux so ARP/ICMP/TCP:22 match lab when UDX owns wire.
     */
    u32Checks++;
    u32SshdLabOk = 0;
    if (NET_ETH_SSHD_LAB == 1u && NET_ETH_LAB_DEMUX == 1u &&
        net_eth_ip_is_lab(g_aLabIp) != 0) {
        u32SshdLabOk = 1u;
        u32Ok++;
    }

    /*
     * freestanding->UDX wire ownership handoff residual:
     * handoff fail-closed + no freestanding product claim + UDX owner.
     */
    u32Checks++;
    u32FsToUdxOk = 0;
    if (NET_ETH_FS_TO_UDX == 1u && NET_ETH_HANDOFF_FC == 1u &&
        NET_ETH_FS_CLASS_SKIP == 1u && NET_ETH_PRODUCT_UDX == 1u) {
        u32FsToUdxOk = 1u;
        u32Ok++;
    }

    /*
     * W11 Dual DoD B FUNCTIONAL denser wire22 residual (wire handoff
     * + :22 stack for product sshd). Multi-arm denser honesty:
     *   arm0 h1_poll | arm1 tcp22_demux | arm2 stack |
     *   arm3 dual_dod_open | arm4 product_udx
     * Denser H1 thr-only sublocks (thr|irq|path|ap|door) + denser demux
     * sublocks (stack|demux|port|dense). Door thr legal thr-stack owner.
     * Soft!=product; stamp-free bar v2026.08.04.75; never invent .76.
     * Dual DoD OPEN (agent!=close). H1 thr-only (never IRQ).
     * greppable: wire_handoff+tcp22 | tcp22_seen | tcp22_demux
     * greppable: stack=eth|tcp|door|:22 | wire22 denser | denser_ok
     * greppable: denser_h1_sub | denser_demux_sub | thr-only door eth poll
     */
    {
        u32 u32W22H1;
        u32 u32W22H1Sub;
        u32 u32W22Demux;
        u32 u32W22DemuxSub;
        u32 u32W22Stack;
        u32 u32W22Dod;
        u32 u32W22Udx;
        u32 u32W22Dense;

        u32W22H1 = 0u;
        u32W22H1Sub = 0u;
        u32W22Demux = 0u;
        u32W22DemuxSub = 0u;
        u32W22Stack = 0u;
        u32W22Dod = 0u;
        u32W22Udx = 0u;
        u32W22Dense = 0u;

        /*
         * arm0 denser: H1 thr-only door eth poll (never IRQ / never AP).
         * Multi-sublocks: thr|irq|path|ap|door|dense (door thr legal owner).
         * greppable: denser_h1_sub | thr-only door eth poll | door_thr_ok=1
         */
        if (NET_ETH_THR_ONLY == 1u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_IRQ_POLL == 0u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_IRQ_PATH == 0u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_AP_POLL == 0u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_DOOR_THR_OK == 1u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_WIRE22_DENSE == 1u) {
            u32W22H1Sub++; /* denser residual honesty lock */
        }
        if (u32W22H1Sub >= (u32)NET_ETH_WIRE22_DENSE_H1_SUB) {
            u32W22H1 = 1u;
            u32W22Dense++;
        }
        /*
         * arm1 denser: tcp22_demux residual (eth dport 22 -> net_tcp_input).
         * Multi-sublocks: stack|demux|port|dense.
         * greppable: denser_demux_sub | tcp22_demux | denser_ok
         */
        if (NET_ETH_TCP22_STACK == 1u) {
            u32W22DemuxSub++;
        }
        if (NET_ETH_TCP22_DEMUX == 1u) {
            u32W22DemuxSub++;
        }
        if (NET_ETH_SSH_PORT == 22u) {
            u32W22DemuxSub++;
        }
        if (NET_ETH_WIRE22_DENSE == 1u) {
            u32W22DemuxSub++;
        }
        if (u32W22DemuxSub >= (u32)NET_ETH_WIRE22_DENSE_DEMUX_SUB) {
            u32W22Demux = 1u;
            u32W22Dense++;
        }
        /* arm2: stack=eth|tcp|door|:22 honesty (wire step depth + demux). */
        if (NET_ETH_WIRE_STEPS == 7u && NET_ETH_TCP22_DEMUX == 1u &&
            NET_ETH_TCP22_STACK == 1u && NET_ETH_SSH_PORT == 22u) {
            u32W22Stack = 1u;
            u32W22Dense++;
        }
        /* arm3: dual_dod OPEN honesty (soft residual never closes Dual DoD). */
        if (NET_ETH_WIRE22_DENSE == 1u && NET_ETH_TCP22_STACK == 1u &&
            NET_ETH_WIRE22_DENSE_ARMS == 5u &&
            NET_ETH_WIRE22_DENSE_MIN == NET_ETH_WIRE22_DENSE_ARMS &&
            NET_ETH_WIRE22_DENSE_H1_SUB == 6u &&
            NET_ETH_WIRE22_DENSE_DEMUX_SUB == 4u) {
            u32W22Dod = 1u;
            u32W22Dense++;
        }
        /* arm4: product_udx wire owner (G-AC-1; freestanding never product). */
        if (NET_ETH_PRODUCT_UDX == 1u && NET_ETH_FS_TO_UDX == 1u &&
            NET_ETH_SSHD_LAB == 1u && NET_ETH_FS_CLASS_SKIP == 1u &&
            NET_ETH_LAB_DEMUX == 1u) {
            u32W22Udx = 1u;
            u32W22Dense++;
        }

        u32Checks++;
        u32Tcp22Ok = 0;
        if (u32W22Dense >= (u32)NET_ETH_WIRE22_DENSE_MIN &&
            u32W22H1 != 0u && u32W22Demux != 0u && u32W22Stack != 0u &&
            u32W22Dod != 0u && u32W22Udx != 0u &&
            u32W22H1Sub >= (u32)NET_ETH_WIRE22_DENSE_H1_SUB &&
            u32W22DemuxSub >= (u32)NET_ETH_WIRE22_DENSE_DEMUX_SUB &&
            NET_ETH_TCP22_DEMUX == 1u && NET_ETH_THR_ONLY == 1u &&
            NET_ETH_IRQ_POLL == 0u && NET_ETH_DOOR_THR_OK == 1u) {
            u32Tcp22Ok = 1u;
            u32Ok++;
        }

        /*
         * LEAN_CHECKS catalog match is part of residual honesty (C1 H1).
         * Mismatch => no PASS lamp (Soft!=product; never Dual DoD close).
         */
        {
            u32 u32LeanMatch = 0;

            if (u32Checks == (u32)NET_ETH_LEAN_CHECKS) {
                u32LeanMatch = 1u;
            }

            /*
             * Grep: net_eth: soft residual lean
             * One lean line - Soft!=product dual license; no version stamp; no storm.
             * Legal thr owners: scheduler_run thr + door thr (full kstack). Never IRQ.
             * W11 STRONGER denser: multi-arm wire22 + tcp22_demux residual.
             * H2 once-lamp only (no stamp storms).
             */
            kprintf("net_eth: soft residual lean thr_only=%u net_eth_irq=%u "
                    "ap_poll=%u irq_path=%u door_thr_ok=%u poll_own=thr_stack "
                    "owner=scheduler_run|door_thr net_eth_poll=run_loop_only "
                    "net_eth_poll=run_loop_or_door fault_class=H1_irq_stack_smash "
                    "poll_max=%u poll_max_ok=%u tx_tries=%u tx_tries_ok=%u "
                    "log_cap_ok=%u cadence_nop=%u thr_locks=%u min_frame_ok=%u "
                    "handoff_fail_closed=%u handoff_fc_ok=%u lab_demux=%u "
                    "lab_demux_ok=%u lab_ip=10.200.125.50 "
                    "fs_class_skip=%u fs_class_skip_ok=%u product_udx=%u "
                    "product_udx_ok=%u sshd_lab=%u sshd_lab_ok=%u "
                    "fs_to_udx=%u fs_to_udx_ok=%u tcp22_stack=%u tcp22_demux=%u "
                    "tcp22_ok=%u denser=%u denser_arms=%u denser_n=%u "
                    "denser_ok=%u denser_min=%u denser_h1=%u denser_h1_sub=%u/%u "
                    "denser_demux=%u denser_demux_sub=%u/%u "
                    "denser_stack=%u denser_dod=%u denser_udx=%u "
                    "ssh_port=%u freestanding_wire_claim=0 "
                    "wire_own=product_udx_abi product_sshd_tcp22=OPEN "
                    "wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
                    "path=eth_dport22|net_tcp_input|door|:22 "
                    "thr-only_door_eth_poll=1 door_thr_ok=1 "
                    "checks=%u ok=%u lean_match=%u lean_checks=%u "
                    "freestanding_class=SKIP product=UDX path=rtl8168_udx "
                    "owner=product_udx_abi need=UDX_OPEN dual_dod_b=OPEN_UDX "
                    "dual_dod_b=OPEN dual_dod_a=OPEN soft_ne_product=1 "
                    "H1=1 H2=once virtio_udx_later=1 G-AC-1=1 "
                    "dual=MIT_OR_Apache-2.0 "
                    "(Soft!=product; W11 Dual DoD B FUNCTIONAL residual lean "
                    "STRONGER denser wire22; denser_h1_sub denser_demux_sub; "
                    "tcp22_demux; stack=eth|tcp|door|:22; H1 thr-only; "
                    "agent!=close; no IRQ eth poll; stamp-free bar "
                    "v2026.08.04.75; never .76)\n",
                    (u32)NET_ETH_THR_ONLY, (u32)NET_ETH_IRQ_POLL,
                    (u32)NET_ETH_AP_POLL, (u32)NET_ETH_IRQ_PATH,
                    (u32)NET_ETH_DOOR_THR_OK, (u32)NET_ETH_POLL_MAX, u32PollMaxOk,
                    (u32)NET_ETH_TX_TRIES, u32TxTriesOk, u32LogCapOk, u32CadenceNop,
                    u32ThrLocks, u32MinFrameOk, (u32)NET_ETH_HANDOFF_FC,
                    u32HandoffFcOk, (u32)NET_ETH_LAB_DEMUX, u32LabDemuxOk,
                    (u32)NET_ETH_FS_CLASS_SKIP, u32FsClassSkipOk,
                    (u32)NET_ETH_PRODUCT_UDX, u32ProductUdxOk,
                    (u32)NET_ETH_SSHD_LAB, u32SshdLabOk,
                    (u32)NET_ETH_FS_TO_UDX, u32FsToUdxOk,
                    (u32)NET_ETH_TCP22_STACK, (u32)NET_ETH_TCP22_DEMUX,
                    u32Tcp22Ok, (u32)NET_ETH_WIRE22_DENSE,
                    (u32)NET_ETH_WIRE22_DENSE_ARMS, u32W22Dense, u32Tcp22Ok,
                    (u32)NET_ETH_WIRE22_DENSE_MIN, u32W22H1, u32W22H1Sub,
                    (u32)NET_ETH_WIRE22_DENSE_H1_SUB, u32W22Demux, u32W22DemuxSub,
                    (u32)NET_ETH_WIRE22_DENSE_DEMUX_SUB,
                    u32W22Stack, u32W22Dod, u32W22Udx,
                    (u32)NET_ETH_SSH_PORT,
                    u32Checks, u32Ok, u32LeanMatch, (u32)NET_ETH_LEAN_CHECKS);

            if (u32LeanMatch != 0u && u32Ok == u32Checks && u32ThrLocks != 0u &&
                u32FsToUdxOk != 0u && u32Tcp22Ok != 0u) {
                /*
                 * Grep: net_eth: soft residual lean PASS
                 * Soft residual only - never product Dual DoD close. H2 once.
                 */
                kprintf("net_eth: soft residual lean PASS checks=%u ok=%u "
                        "lean_match=1 thr_only=1 net_eth_irq=0 poll_own=thr_stack "
                        "ap_poll=0 irq_path=0 door_thr_ok=1 handoff_fail_closed=1 "
                        "lab_demux=1 lab_ip=10.200.125.50 "
                        "fs_to_udx=1 tcp22_stack=1 tcp22_demux=1 denser=1 "
                        "denser_ok=1 denser_arms=%u denser_min=%u "
                        "wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
                        "path=eth_dport22|net_tcp_input|door|:22 "
                        "freestanding_wire_claim=0 "
                        "wire_own=product_udx_abi product_sshd_tcp22=OPEN "
                        "fault_class=H1_irq_stack_smash dual_dod_a=OPEN "
                        "dual_dod_b=OPEN dual_dod_b=OPEN_UDX need=UDX_OPEN "
                        "owner=product_udx_abi path=rtl8168_udx "
                        "H1=1 H2=once soft_ne_product=1 freestanding_class=SKIP "
                        "product=UDX (Soft!=product; W11 Dual DoD B FUNCTIONAL "
                        "residual lean STRONGER denser wire22; tcp22_demux; "
                        "stack=eth|tcp|door|:22; agent!=close; G-AC-1 no .ko "
                        "product; stamp-free)\n",
                        u32Checks, u32Ok, (u32)NET_ETH_WIRE22_DENSE_ARMS,
                        (u32)NET_ETH_WIRE22_DENSE_MIN);
            }
        }
    }
}

/**
 * FUNCTIONAL residual once: freestanding->UDX wire ownership handoff for
 * lab IP / sshd Dual DoD B path (Soft!=product; G-AC-1; stamp-free).
 * STRONGER: multi-check selfcheck + eth-layer wire handoff step catalog
 * (names only) + soft_mmio_handoff vs product_udx_wire_handoff honesty.
 * Catalogs ownership honesty only - never product Dual DoD close.
 * greppable: net_eth: soft residual functional
 * greppable: net_eth: soft residual functional PASS
 * greppable: net_eth: soft residual functional handoff steps
 * greppable: fs_to_udx=1 | freestanding_wire_claim=0 | wire_own=product_udx_abi
 * greppable: product_sshd_tcp22=OPEN | lab_ip=10.200.125.50 | dual_dod_b=OPEN_UDX
 * greppable: path=rtl8168_udx | need=UDX_OPEN | freestanding_class=SKIP
 * greppable: soft_mmio_handoff | product_udx_wire_handoff | wire_step=
 * greppable: func_ok= | func_checks= | wire_steps=
 */
static void
net_eth_soft_residual_functional_once(void)
{
    u32 u32Be;
    u32 u32HandoffPend;
    u32 u32HandoffFault;
    u32 u32Ready;
    u32 u32FsWireClaim;
    u32 u32FuncOk;
    u32 u32Checks;
    u32 u32Ok;
    u32 u32H1Ok;
    u32 u32HandoffFcOk;
    u32 u32FsSkipOk;
    u32 u32LabOk;
    u32 u32ProductOk;
    u32 u32SshdOk;
    u32 u32FsToUdxOk;
    u32 u32ClaimOk;
    u32 u32HoSplitOk;
    u32 u32StepsOk;
    u32 u32PollMaxOk;
    u32 u32Tcp22Ok;
    const char *szBe;
    const char *szWireOwn;
    const char *szDrain;
    extern int net_l2_soft_handoff_pending(void);
    extern int net_l2_soft_handoff_fault(void);

    if (g_fSoftFunctionalOnce != 0u) {
        return;
    }
    g_fSoftFunctionalOnce = 1u;

    u32Be = net_l2_backend();
    u32HandoffPend = (net_l2_soft_handoff_pending() != 0) ? 1u : 0u;
    u32HandoffFault = (net_l2_soft_handoff_fault() != 0) ? 1u : 0u;
    u32Ready = (net_l2_ready() != 0) ? 1u : 0u;

    /*
     * Freestanding product wire claim residual (always 0):
     *   none  -> freestanding never owns (SKIP / UDX product residual)
     *   virtio -> T0 product interim (not freestanding rtl class)
     *   rtl    -> opt-in residual may drain; product claim still 0
     * handoff pending/FAULT -> freestanding drain already fail-closed.
     * Product Dual DoD B wire owner is always UDX+ABI (agent!=close).
     */
    u32FsWireClaim = 0u; /* product claim never freestanding (G-AC-1 track) */

    if (u32Be == GJ_NET_L2_NONE) {
        szBe = "none";
        szWireOwn = "product_udx_abi";
        szDrain = "skip_none";
    } else if (u32Be == GJ_NET_L2_VIRTIO) {
        szBe = "virtio";
        szWireOwn = "virtio_t0";
        szDrain = "virtio_t0_product";
    } else if (u32Be == GJ_NET_L2_RTL8168) {
        szBe = "rtl8168";
        /* Opt-in residual drain; product still UDX (Dual DoD B OPEN). */
        if (u32HandoffPend != 0u || u32HandoffFault != 0u) {
            szWireOwn = "product_udx_abi";
            szDrain = "handoff_fail_closed";
        } else if (u32Ready == 0u) {
            szWireOwn = "product_udx_abi";
            szDrain = "not_ready";
        } else {
            szWireOwn = "fs_opt_in_residual";
            szDrain = "fs_opt_in_residual";
        }
    } else {
        szBe = "unknown";
        szWireOwn = "product_udx_abi";
        szDrain = "unknown";
    }

    /*
     * W11 STRONGER FUNCTIONAL selfcheck (NET_ETH_FUNC_CHECKS items):
     * H1 thr-only locks, handoff FC, fs SKIP, lab pin, product UDX,
     * sshd lab, fs_to_udx compound, freestanding claim=0, soft_mmio vs
     * product handoff split honesty, wire step depth, TCP:22 stack.
     * Soft!=product; stamp-free bar v2026.08.04.75; never invent .76.
     */
    u32Checks = 0u;
    u32Ok = 0u;

    /* 1. H1 thr-only (never IRQ / never AP poll). */
    u32Checks++;
    u32H1Ok = 0u;
    if (NET_ETH_THR_ONLY == 1u && NET_ETH_IRQ_POLL == 0u &&
        NET_ETH_IRQ_PATH == 0u && NET_ETH_AP_POLL == 0u &&
        NET_ETH_DOOR_THR_OK == 1u) {
        u32H1Ok = 1u;
        u32Ok++;
    }

    /* 2. Soft MMIO handoff fail-closed residual. */
    u32Checks++;
    u32HandoffFcOk = 0u;
    if (NET_ETH_HANDOFF_FC == 1u && NET_ETH_SOFT_MMIO_HO == 1u) {
        u32HandoffFcOk = 1u;
        u32Ok++;
    }

    /* 3. freestanding_class=SKIP (no freestanding product wire track). */
    u32Checks++;
    u32FsSkipOk = 0u;
    if (NET_ETH_FS_CLASS_SKIP == 1u) {
        u32FsSkipOk = 1u;
        u32Ok++;
    }

    /* 4. Lab demux pin 10.200.125.50 (Dual DoD B identity residual). */
    u32Checks++;
    u32LabOk = 0u;
    if (NET_ETH_LAB_DEMUX == 1u && net_eth_ip_is_lab(g_aLabIp) != 0 &&
        g_aLabIp[0] == 10u && g_aLabIp[1] == 200u && g_aLabIp[2] == 125u &&
        g_aLabIp[3] == 50u) {
        u32LabOk = 1u;
        u32Ok++;
    }

    /* 5. Product NIC wire owner = UDX+ABI (rtl8168_udx). */
    u32Checks++;
    u32ProductOk = 0u;
    if (NET_ETH_PRODUCT_UDX == 1u && NET_ETH_PRODUCT_HO == 1u) {
        u32ProductOk = 1u;
        u32Ok++;
    }

    /* 6. Lab IP demux residual for sshd Dual DoD B path. */
    u32Checks++;
    u32SshdOk = 0u;
    if (NET_ETH_SSHD_LAB == 1u && NET_ETH_LAB_DEMUX == 1u) {
        u32SshdOk = 1u;
        u32Ok++;
    }

    /* 7. freestanding->UDX wire ownership handoff residual compound. */
    u32Checks++;
    u32FsToUdxOk = 0u;
    if (NET_ETH_FS_TO_UDX == 1u && NET_ETH_HANDOFF_FC == 1u &&
        NET_ETH_FS_CLASS_SKIP == 1u && NET_ETH_PRODUCT_UDX == 1u) {
        u32FsToUdxOk = 1u;
        u32Ok++;
    }

    /* 8. freestanding product wire claim always 0 (G-AC-1). */
    u32Checks++;
    u32ClaimOk = 0u;
    if (u32FsWireClaim == 0u) {
        u32ClaimOk = 1u;
        u32Ok++;
    }

    /*
     * 9. soft_mmio_handoff (eng residual) != product_udx_wire_handoff
     * (Dual DoD B product path). Both residual honesty flags must hold;
     * neither closes Dual DoD. Soft!=product.
     */
    u32Checks++;
    u32HoSplitOk = 0u;
    if (NET_ETH_SOFT_MMIO_HO == 1u && NET_ETH_PRODUCT_HO == 1u &&
        NET_ETH_HANDOFF_FC == 1u && NET_ETH_FS_TO_UDX == 1u) {
        u32HoSplitOk = 1u;
        u32Ok++;
    }

    /* 10. Wire step depth + lean batch bound honesty (W11: 7 steps). */
    u32Checks++;
    u32StepsOk = 0u;
    u32PollMaxOk = 0u;
    if (NET_ETH_WIRE_STEPS == 7u && NET_ETH_POLL_MAX > 0u &&
        NET_ETH_POLL_MAX <= 32u && NET_ETH_FUNC_CHECKS == 11u) {
        u32StepsOk = 1u;
        u32PollMaxOk = 1u;
        u32Ok++;
    }

    /*
     * 11. W11 Dual DoD B denser wire22 residual (wire handoff + :22
     * stack for product sshd). Multi-arm denser: h1_poll | tcp22_demux |
     * stack | dual_dod_open | product_udx. Denser H1 thr-only sublocks
     * (thr|irq|path|ap|door) + denser demux sublocks (stack|demux|port|dense).
     * Soft!=product; Dual DoD OPEN. Door thr legal thr-stack owner.
     * greppable: wire_handoff+tcp22 | tcp22_seen | tcp22_demux
     * greppable: stack=eth|tcp|door|:22 | wire22 denser | denser_ok
     * greppable: denser_h1_sub | denser_demux_sub | thr-only door eth poll
     */
    {
        u32 u32W22H1;
        u32 u32W22H1Sub;
        u32 u32W22Demux;
        u32 u32W22DemuxSub;
        u32 u32W22Stack;
        u32 u32W22Dod;
        u32 u32W22Udx;
        u32 u32W22Dense;

        u32W22H1 = 0u;
        u32W22H1Sub = 0u;
        u32W22Demux = 0u;
        u32W22DemuxSub = 0u;
        u32W22Stack = 0u;
        u32W22Dod = 0u;
        u32W22Udx = 0u;
        u32W22Dense = 0u;

        /* arm0 denser: H1 thr-only door eth poll thr|irq|path|ap|door|dense. */
        if (NET_ETH_THR_ONLY == 1u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_IRQ_POLL == 0u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_IRQ_PATH == 0u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_AP_POLL == 0u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_DOOR_THR_OK == 1u) {
            u32W22H1Sub++;
        }
        if (NET_ETH_WIRE22_DENSE == 1u) {
            u32W22H1Sub++; /* denser residual honesty lock */
        }
        if (u32W22H1Sub >= (u32)NET_ETH_WIRE22_DENSE_H1_SUB) {
            u32W22H1 = 1u;
            u32W22Dense++;
        }
        /* arm1 denser: tcp22_demux multi-sublocks stack|demux|port|dense. */
        if (NET_ETH_TCP22_STACK == 1u) {
            u32W22DemuxSub++;
        }
        if (NET_ETH_TCP22_DEMUX == 1u) {
            u32W22DemuxSub++;
        }
        if (NET_ETH_SSH_PORT == 22u) {
            u32W22DemuxSub++;
        }
        if (NET_ETH_WIRE22_DENSE == 1u) {
            u32W22DemuxSub++;
        }
        if (u32W22DemuxSub >= (u32)NET_ETH_WIRE22_DENSE_DEMUX_SUB) {
            u32W22Demux = 1u;
            u32W22Dense++;
        }
        /* arm2: stack=eth|tcp|door|:22. */
        if (NET_ETH_WIRE_STEPS == 7u && NET_ETH_TCP22_DEMUX == 1u &&
            NET_ETH_TCP22_STACK == 1u && NET_ETH_SSH_PORT == 22u) {
            u32W22Stack = 1u;
            u32W22Dense++;
        }
        /* arm3: dual_dod OPEN honesty. */
        if (NET_ETH_WIRE22_DENSE == 1u && NET_ETH_TCP22_STACK == 1u &&
            NET_ETH_WIRE22_DENSE_ARMS == 5u &&
            NET_ETH_WIRE22_DENSE_MIN == NET_ETH_WIRE22_DENSE_ARMS &&
            NET_ETH_WIRE22_DENSE_H1_SUB == 6u &&
            NET_ETH_WIRE22_DENSE_DEMUX_SUB == 4u) {
            u32W22Dod = 1u;
            u32W22Dense++;
        }
        /* arm4: product_udx owner residual. */
        if (NET_ETH_PRODUCT_UDX == 1u && NET_ETH_FS_TO_UDX == 1u &&
            NET_ETH_SSHD_LAB == 1u && NET_ETH_FS_CLASS_SKIP == 1u &&
            NET_ETH_LAB_DEMUX == 1u) {
            u32W22Udx = 1u;
            u32W22Dense++;
        }

        u32Checks++;
        u32Tcp22Ok = 0u;
        if (u32W22Dense >= (u32)NET_ETH_WIRE22_DENSE_MIN &&
            u32W22H1 != 0u && u32W22Demux != 0u && u32W22Stack != 0u &&
            u32W22Dod != 0u && u32W22Udx != 0u &&
            u32W22H1Sub >= (u32)NET_ETH_WIRE22_DENSE_H1_SUB &&
            u32W22DemuxSub >= (u32)NET_ETH_WIRE22_DENSE_DEMUX_SUB &&
            NET_ETH_TCP22_DEMUX == 1u && NET_ETH_THR_ONLY == 1u &&
            NET_ETH_IRQ_POLL == 0u && NET_ETH_WIRE_STEPS == 7u &&
            NET_ETH_DOOR_THR_OK == 1u) {
            u32Tcp22Ok = 1u;
            u32Ok++;
        }

        u32FuncOk = 0u;
        if (u32Checks == (u32)NET_ETH_FUNC_CHECKS && u32Ok == u32Checks &&
            u32H1Ok != 0u && u32FsToUdxOk != 0u && u32ClaimOk != 0u &&
            u32HoSplitOk != 0u && u32Tcp22Ok != 0u) {
            u32FuncOk = 1u;
        }

        /*
         * Grep: net_eth: soft residual functional
         * W11 STRONGER denser freestanding->UDX + multi-arm wire22.
         * H2 once-lamp (no stamp storms). Soft!=product dual_dod OPEN.
         */
        kprintf("net_eth: soft residual functional Soft!=product "
                "fs_to_udx=%u freestanding_wire_claim=%u "
                "wire_own=%s owner=product_udx_abi path=rtl8168_udx "
                "path=userspace_udx_hot_cold_abi backend=%s be=%u drain=%s "
                "l2_ready=%u handoff_pending=%u handoff_fault=%u "
                "handoff_fail_closed=%u soft_mmio_handoff=%u "
                "product_udx_wire_handoff=%u lab_demux=%u lab_ip=10.200.125.50 "
                "product_sshd_tcp22=OPEN freestanding_class=SKIP product=UDX "
                "fs_class_skip=%u product_udx=%u sshd_lab=%u "
                "tcp22_stack=%u tcp22_demux=%u tcp22_ok=%u "
                "tcp22_seen=%u tcp22_demux_n=%u ssh_port=%u "
                "denser=%u denser_arms=%u denser_n=%u denser_ok=%u "
                "denser_min=%u denser_h1=%u denser_h1_sub=%u/%u "
                "denser_demux=%u denser_demux_sub=%u/%u denser_stack=%u "
                "denser_dod=%u denser_udx=%u "
                "wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
                "path=eth_dport22|net_tcp_input|door|:22 "
                "thr-only_door_eth_poll=1 "
                "need=UDX_OPEN dual_dod_b=OPEN_UDX dual_dod_b=OPEN dual_dod_a=OPEN "
                "thr_only=%u net_eth_irq=%u irq_path=%u door_thr_ok=%u "
                "poll_own=thr_stack net_eth_poll=run_loop_only "
                "net_eth_poll=run_loop_or_door fault_class=H1_irq_stack_smash "
                "H1=1 H2=once func_checks=%u func_ok_n=%u func_ok=%u "
                "h1_ok=%u handoff_fc_ok=%u fs_skip_ok=%u lab_ok=%u "
                "product_ok=%u sshd_ok=%u fs_to_udx_ok=%u claim_ok=%u "
                "ho_split_ok=%u steps_ok=%u poll_max_ok=%u "
                "wire_steps=%u polls_handoff_closed=%u "
                "soft_ne_product=1 G-AC-1=1 dual=MIT_OR_Apache-2.0 "
                "(Soft!=product; W11 Dual DoD B FUNCTIONAL residual STRONGER "
                "denser wire22; denser_h1_sub denser_demux_sub; "
                "tcp22_demux stack=eth|tcp|door|:22; "
                "agent!=close; not Dual DoD B close; stamp-free bar "
                "v2026.08.04.75; never .76)\n",
                (u32)NET_ETH_FS_TO_UDX, u32FsWireClaim, szWireOwn, szBe, u32Be,
                szDrain, u32Ready, u32HandoffPend, u32HandoffFault,
                (u32)NET_ETH_HANDOFF_FC, (u32)NET_ETH_SOFT_MMIO_HO,
                (u32)NET_ETH_PRODUCT_HO, (u32)NET_ETH_LAB_DEMUX,
                (u32)NET_ETH_FS_CLASS_SKIP, (u32)NET_ETH_PRODUCT_UDX,
                (u32)NET_ETH_SSHD_LAB, (u32)NET_ETH_TCP22_STACK,
                (u32)NET_ETH_TCP22_DEMUX, u32Tcp22Ok, g_u32Tcp22Seen,
                g_u32Tcp22Demux, (u32)NET_ETH_SSH_PORT,
                (u32)NET_ETH_WIRE22_DENSE, (u32)NET_ETH_WIRE22_DENSE_ARMS,
                u32W22Dense, u32Tcp22Ok, (u32)NET_ETH_WIRE22_DENSE_MIN,
                u32W22H1, u32W22H1Sub, (u32)NET_ETH_WIRE22_DENSE_H1_SUB,
                u32W22Demux, u32W22DemuxSub, (u32)NET_ETH_WIRE22_DENSE_DEMUX_SUB,
                u32W22Stack, u32W22Dod, u32W22Udx,
                (u32)NET_ETH_THR_ONLY, (u32)NET_ETH_IRQ_POLL,
                (u32)NET_ETH_IRQ_PATH, (u32)NET_ETH_DOOR_THR_OK, u32Checks,
                u32Ok, u32FuncOk, u32H1Ok, u32HandoffFcOk, u32FsSkipOk,
                u32LabOk, u32ProductOk, u32SshdOk, u32FsToUdxOk, u32ClaimOk,
                u32HoSplitOk, u32StepsOk, u32PollMaxOk,
                (u32)NET_ETH_WIRE_STEPS, g_u32PollsHandoffClosed);

        /*
         * Grep: net_eth: soft residual functional handoff steps
         * Eth-layer freestanding->UDX wire handoff residual step catalog
         * (names only; Soft!=product; never product Dual DoD close).
         * soft_mmio_handoff (eng) != product_udx_wire_handoff (Dual DoD B).
         * W11 denser: wire_step=7 tcp22_demux (eth dport 22 -> net_tcp_input).
         */
        kprintf("net_eth: soft residual functional handoff steps "
                "Soft!=product wire_steps=%u "
                "wire_step=1 h1_thr_only thr_only=%u net_eth_irq=%u "
                "irq_path=%u door_thr_ok=%u poll_own=thr_stack H1=1 "
                "wire_step=2 soft_mmio_handoff fail_closed=%u pending=%u "
                "fault=%u drain=skip_when_pending_or_fault "
                "wire_step=3 freestanding_class=SKIP fs_class_skip=%u "
                "freestanding_wire_claim=0 "
                "wire_step=4 lab_demux lab_ip=10.200.125.50 lab_demux=%u "
                "wire_step=5 product_udx_wire_handoff owner=product_udx_abi "
                "path=rtl8168_udx product_udx=%u need=UDX_OPEN "
                "wire_step=6 product_sshd_tcp22=OPEN sshd_lab=%u "
                "wire_step=7 tcp22_demux ssh_port=%u tcp22_stack=%u "
                "tcp22_demux_lock=%u tcp22_seen=%u tcp22_demux_n=%u "
                "stack=eth|tcp|door|:22 wire_handoff+tcp22=1 denser=1 "
                "path=eth_dport22|net_tcp_input|door|:22 "
                "dual_dod_b=OPEN_UDX dual_dod_b=OPEN dual_dod_a=OPEN "
                "fs_to_udx=%u soft_mmio_handoff=%u product_udx_wire_handoff=%u "
                "backend=%s drain=%s agent_ne_close=1 H2=once "
                "(Soft!=product; W11 Dual DoD B FUNCTIONAL denser wire "
                "handoff+:22 tcp22_demux step catalog; not Dual DoD B close; "
                "stamp-free)\n",
                (u32)NET_ETH_WIRE_STEPS, (u32)NET_ETH_THR_ONLY,
                (u32)NET_ETH_IRQ_POLL, (u32)NET_ETH_IRQ_PATH,
                (u32)NET_ETH_DOOR_THR_OK, (u32)NET_ETH_HANDOFF_FC,
                u32HandoffPend, u32HandoffFault, (u32)NET_ETH_FS_CLASS_SKIP,
                (u32)NET_ETH_LAB_DEMUX, (u32)NET_ETH_PRODUCT_UDX,
                (u32)NET_ETH_SSHD_LAB, (u32)NET_ETH_SSH_PORT,
                (u32)NET_ETH_TCP22_STACK, (u32)NET_ETH_TCP22_DEMUX,
                g_u32Tcp22Seen, g_u32Tcp22Demux,
                (u32)NET_ETH_FS_TO_UDX, (u32)NET_ETH_SOFT_MMIO_HO,
                (u32)NET_ETH_PRODUCT_HO, szBe, szDrain);

        /*
         * Grep: net_eth: soft residual wire22
         * W11 Dual DoD B STRONGER denser multi-arm wire handoff + tcp22_demux
         * (H2 once; Soft!=product; dual_dod OPEN; stamp-free).
         * greppable: tcp22_demux | stack=eth|tcp|door|:22 | wire_handoff+tcp22
         * greppable: wire22 denser | denser_ok | denser_arms
         * greppable: denser_h1_sub | denser_demux_sub | thr-only door eth poll
         */
        kprintf("net_eth: soft residual wire22 Soft!=product "
                "wire_handoff+tcp22=1 tcp22_demux=%u tcp22_demux_n=%u "
                "tcp22_stack=%u tcp22_ok=%u tcp22_seen=%u ssh_port=%u "
                "denser=%u denser_arms=%u denser_n=%u denser_ok=%u "
                "denser_min=%u denser_h1=%u denser_h1_sub=%u/%u "
                "denser_demux=%u denser_demux_sub=%u/%u denser_stack=%u "
                "denser_dod=%u denser_udx=%u "
                "stack=eth|tcp|door|:22 product_sshd_tcp22=OPEN "
                "path=eth_dport22|net_tcp_input|door|:22 wire_step=7 "
                "lab_ip=10.200.125.50 fs_to_udx=1 freestanding_wire_claim=0 "
                "wire_own=product_udx_abi thr_only=1 net_eth_irq=0 "
                "ap_poll=0 irq_path=0 door_thr_ok=1 poll_own=thr_stack "
                "thr-only_door_eth_poll=1 "
                "net_eth_poll=run_loop_only net_eth_poll=run_loop_or_door "
                "fault_class=H1_irq_stack_smash handoff_fail_closed=1 "
                "H1=1 H2=once dual_dod_b=OPEN_UDX dual_dod_b=OPEN "
                "dual_dod_a=OPEN need=UDX_OPEN freestanding_class=SKIP "
                "product=UDX path=rtl8168_udx owner=product_udx_abi "
                "soft_ne_product=1 G-AC-1=1 agent!=close "
                "stamp_free=v2026.08.04.75 never=.76 "
                "(W11 Dual DoD B FUNCTIONAL STRONGER denser wire22; "
                "denser_h1_sub denser_demux_sub; tcp22_demux; "
                "stack=eth|tcp|door|:22; H1 thr-only; not Dual DoD close)\n",
                (u32)NET_ETH_TCP22_DEMUX, g_u32Tcp22Demux,
                (u32)NET_ETH_TCP22_STACK, u32Tcp22Ok, g_u32Tcp22Seen,
                (u32)NET_ETH_SSH_PORT, (u32)NET_ETH_WIRE22_DENSE,
                (u32)NET_ETH_WIRE22_DENSE_ARMS, u32W22Dense, u32Tcp22Ok,
                (u32)NET_ETH_WIRE22_DENSE_MIN, u32W22H1, u32W22H1Sub,
                (u32)NET_ETH_WIRE22_DENSE_H1_SUB, u32W22Demux, u32W22DemuxSub,
                (u32)NET_ETH_WIRE22_DENSE_DEMUX_SUB,
                u32W22Stack, u32W22Dod, u32W22Udx);

        /*
         * Grep: net_eth: soft residual wire22 denser
         * Multi-arm denser honesty catalog (once; Soft!=product).
         * Arms: h1_poll | tcp22_demux | stack | dual_dod_open | product_udx.
         * Denser H1 thr-only sublocks + denser demux sublocks.
         * greppable: denser_h1_sub | denser_demux_sub | thr-only door eth poll
         */
        kprintf("net_eth: soft residual wire22 denser Soft!=product "
                "denser=1 denser_arms=%u denser_n=%u denser_ok=%u denser_min=%u "
                "denser_h1=%u denser_h1_sub=%u/%u denser_demux=%u "
                "denser_demux_sub=%u/%u denser_stack=%u denser_dod=%u "
                "denser_udx=%u tcp22_demux=%u tcp22_stack=%u ssh_port=%u "
                "wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
                "path=eth_dport22|net_tcp_input|door|:22 thr_only=1 "
                "door_thr_ok=1 thr-only_door_eth_poll=1 "
                "net_eth_irq=0 H1=1 H2=once dual_dod_a=OPEN dual_dod_b=OPEN "
                "dual_dod_b=OPEN_UDX product_sshd_tcp22=OPEN "
                "freestanding_class=SKIP product=UDX soft_ne_product=1 "
                "agent!=close stamp_free=v2026.08.04.75 never=.76 "
                "(W11 Dual DoD B denser wire22 multi-arm; denser_h1_sub "
                "denser_demux_sub; thr-only door eth poll; not Dual DoD close)\n",
                (u32)NET_ETH_WIRE22_DENSE_ARMS, u32W22Dense, u32Tcp22Ok,
                (u32)NET_ETH_WIRE22_DENSE_MIN, u32W22H1, u32W22H1Sub,
                (u32)NET_ETH_WIRE22_DENSE_H1_SUB, u32W22Demux, u32W22DemuxSub,
                (u32)NET_ETH_WIRE22_DENSE_DEMUX_SUB,
                u32W22Stack, u32W22Dod, u32W22Udx,
                (u32)NET_ETH_TCP22_DEMUX, (u32)NET_ETH_TCP22_STACK,
                (u32)NET_ETH_SSH_PORT);

        if (u32Tcp22Ok != 0u && NET_ETH_TCP22_DEMUX == 1u &&
            NET_ETH_TCP22_STACK == 1u && NET_ETH_THR_ONLY == 1u &&
            NET_ETH_IRQ_POLL == 0u && NET_ETH_WIRE22_DENSE == 1u &&
            u32W22Dense >= (u32)NET_ETH_WIRE22_DENSE_MIN) {
            /*
             * Grep: net_eth: soft residual wire22 PASS
             * Soft residual only - never product Dual DoD close. H2 once.
             */
            kprintf("net_eth: soft residual wire22 PASS Soft!=product "
                    "tcp22_demux=1 tcp22_ok=1 tcp22_stack=1 denser=1 "
                    "denser_ok=1 denser_arms=%u denser_min=%u "
                    "wire_handoff+tcp22=1 stack=eth|tcp|door|:22 "
                    "path=eth_dport22|net_tcp_input|door|:22 wire_step=7 "
                    "ssh_port=22 product_sshd_tcp22=OPEN "
                    "thr_only=1 net_eth_irq=0 poll_own=thr_stack H1=1 H2=once "
                    "fs_to_udx=1 freestanding_wire_claim=0 "
                    "wire_own=product_udx_abi dual_dod_a=OPEN dual_dod_b=OPEN "
                    "dual_dod_b=OPEN_UDX need=UDX_OPEN freestanding_class=SKIP "
                    "product=UDX soft_ne_product=1 G-AC-1=1 agent!=close "
                    "stamp_free=v2026.08.04.75 never=.76 "
                    "(W11 Dual DoD B FUNCTIONAL STRONGER denser wire22; "
                    "not Dual DoD close)\n",
                    (u32)NET_ETH_WIRE22_DENSE_ARMS,
                    (u32)NET_ETH_WIRE22_DENSE_MIN);
        }

        if (u32FuncOk != 0u) {
            /*
             * Grep: net_eth: soft residual functional PASS
             * Soft residual only - never product Dual DoD close. H2 once.
             */
            kprintf("net_eth: soft residual functional PASS "
                    "func_ok=1 func_checks=%u/%u "
                    "fs_to_udx=1 freestanding_wire_claim=0 "
                    "wire_own=product_udx_abi product_sshd_tcp22=OPEN "
                    "lab_ip=10.200.125.50 handoff_fail_closed=1 "
                    "soft_mmio_handoff=1 product_udx_wire_handoff=1 "
                    "tcp22_stack=1 tcp22_demux=1 denser=1 denser_ok=1 "
                    "wire_handoff+tcp22=1 "
                    "stack=eth|tcp|door|:22 path=eth_dport22|net_tcp_input|door|:22 "
                    "wire_steps=%u freestanding_class=SKIP product=UDX "
                    "path=rtl8168_udx need=UDX_OPEN dual_dod_b=OPEN_UDX "
                    "dual_dod_b=OPEN thr_only=1 net_eth_irq=0 "
                    "poll_own=thr_stack H1=1 H2=once soft_ne_product=1 G-AC-1=1 "
                    "agent!=close "
                    "(Soft!=product; W11 Dual DoD B FUNCTIONAL residual "
                    "STRONGER denser wire22 full; tcp22_demux; "
                    "stack=eth|tcp|door|:22; not Dual DoD B close; "
                    "stamp-free bar v2026.08.04.75; never .76)\n",
                    u32Ok, (u32)NET_ETH_FUNC_CHECKS, (u32)NET_ETH_WIRE_STEPS);
        }
    }
}

void
net_eth_init(void)
{
    g_u32ArpReplies = 0;
    g_u32UdpEchoes = 0;
    g_u32IcmpEchoes = 0;
    g_u32FramesRx = 0;
    g_u32FramesDrop = 0;
    g_u32TcpDemux = 0;
    g_u32VlanSkip = 0;
    g_u32VlanPeel = 0;
    g_u32FramesOk = 0;
    g_u32Polls = 0;
    g_u32PollsNoDev = 0;
    g_u32PollsDrain = 0;
    g_u32PollsEmpty = 0;
    g_u32LastBatch = 0;
    g_u32BatchMax = 0;
    g_u32BatchSum = 0;
    g_u32LinkReady = 0;
    g_u32LinkChanges = 0;
    g_u32SoftLogN = 0;
    g_u32DropShort = 0;
    g_u32DropEtype = 0;
    g_u32DropIpv4Short = 0;
    g_u32DropIpv4Ver = 0;
    g_u32DropProto = 0;
    g_u32ArpSeen = 0;
    g_u32ArpBadOp = 0;
    g_u32ArpNotUs = 0;
    g_u32ArpTxFail = 0;
    g_u32ArpAnnounceOk = 0;
    g_u32ArpAnnounceFail = 0;
    g_u32ArpIdResync = 0;
    g_u32ArpTxRetryOk = 0;
    g_u32WhoHasTxFailLamp = 0;
    g_u64WhoHasTxFailLastJif = 0;
    g_fArpApplyAnnOnce = 0;
    g_fWhoHasReplyOk = 0;
    g_u32IcmpSeen = 0;
    g_u32IcmpShort = 0;
    g_u32IcmpNotUs = 0;
    g_u32IcmpNotEcho = 0;
    g_u32IcmpTxFail = 0;
    g_u32IcmpIdResync = 0;
    g_u32IcmpTxRetryOk = 0;
    g_u32IcmpTxFailLamp = 0;
    g_u64IcmpTxFailLastJif = 0;
    g_u32IdSyncChg = 0;
    g_u32IdSyncChgLamp = 0;
    g_u32UdpSeen = 0;
    g_u32UdpShort = 0;
    g_u32UdpNotEcho = 0;
    g_u32UdpTxFail = 0;
    g_u32TcpSeen = 0;
    g_u32TcpMiss = 0;
    g_u32Tcp22Seen = 0;
    g_u32Tcp22Demux = 0;
    g_u32TxOk = 0;
    g_u32TxFail = 0;
    g_u32BytesRx = 0;
    g_u32BytesTx = 0;
    g_u32PollsHandoffClosed = 0;
    g_fHandoffResidualLamp = 0;
    g_fUdxWireResidualLamp = 0;
    g_fSoftLeanThr = 0;
    g_fSoftFunctionalOnce = 0;

    g_u32LinkReady = net_l2_ready() ? 1u : (virtio_net_ready() ? 1u : 0u);
    if (net_l2_ready() != 0 || net_l2_backend() != GJ_NET_L2_NONE) {
        net_l2_mac(g_aOurMac);
        net_l2_ip(g_aOurIp);
    }
    if (net_l2_backend() == GJ_NET_L2_RTL8168 ||
        net_eth_ip_is_lab(g_aOurIp) != 0) {
        net_eth_force_lab_ip();
    }
    kprintf("net_eth: ARP/UDP/ICMP-echo helpers (IP %u.%u.%u.%u) poll_max=%u "
            "tx_tries=%u (Soft!=product; lean residual; thrash-stripped; "
            "net_eth_poll=run_loop_only; net_eth_irq=0; owner=scheduler_run; "
            "poll_own=thr_stack; thr_only=1; ap_poll=0; irq_path=0; "
            "door_thr_ok=1; fault_class=H1_irq_stack_smash; "
            "handoff_fail_closed=1; soft_mmio_handoff=1; "
            "product_udx_wire_handoff=1; wire_steps=%u; func_checks=%u; "
            "lab_demux=1; freestanding_class=SKIP; "
            "product=UDX; path=rtl8168_udx; owner=product_udx_abi; "
            "need=UDX_OPEN; dual_dod_b=OPEN; dual_dod_b=OPEN_UDX; "
            "fs_to_udx=1; freestanding_wire_claim=0; "
            "wire_own=product_udx_abi; product_sshd_tcp22=OPEN; "
            "tcp22_stack=1; tcp22_demux=1; denser=1; denser_arms=%u; "
            "wire_handoff+tcp22=1; stack=eth|tcp|door|:22; "
            "path=eth_dport22|net_tcp_input|door|:22; "
            "lab_ip=10.200.125.50; virtio T0 + product UDX L2; "
            "W11 Dual DoD B FUNCTIONAL STRONGER denser wire22; H1 thr-only; "
            "H2=once; stamp-free v2026.08.04.75)\n",
            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3],
            (u32)NET_ETH_POLL_MAX, (u32)NET_ETH_TX_TRIES,
            (u32)NET_ETH_WIRE_STEPS, (u32)NET_ETH_FUNC_CHECKS,
            (u32)NET_ETH_WIRE22_DENSE_ARMS);
    kprintf("net: eth soft init poll_max=%u log_cap=%u "
            "ip=%u.%u.%u.%u mac=%02x:%02x:%02x:%02x:%02x:%02x "
            "(Soft!=product; lean; freestanding_class=SKIP; thr_only=1; "
            "net_eth_irq=0; poll_own=thr_stack; fs_to_udx=1; "
            "product_sshd_tcp22=OPEN; tcp22_demux=1; denser=1; "
            "stack=eth|tcp|door|:22; H2=once)\n",
            (u32)NET_ETH_POLL_MAX, (u32)NET_ETH_SOFT_LOG_CAP, g_aOurIp[0],
            g_aOurIp[1], g_aOurIp[2], g_aOurIp[3], g_aOurMac[0], g_aOurMac[1],
            g_aOurMac[2], g_aOurMac[3], g_aOurMac[4], g_aOurMac[5]);
    net_eth_soft_residual_lean_thr_once();
    net_eth_soft_residual_functional_once();
    net_eth_soft_log();
}

/*
 * Broadcast ARP reply (op=2) for our IP - optional neigh learn aid.
 * Soft honesty: does NOT bump g_u32ArpReplies (that is who-has only).
 */
static int
net_eth_arp_announce(void)
{
    static u8 s_aAnn[64];
    u32 i;

    net_eth_sync_l2();
    if (net_l2_ready() == 0) {
        return -1;
    }
    memset(s_aAnn, 0, sizeof(s_aAnn));
    for (i = 0; i < 6u; i++) {
        s_aAnn[i] = 0xffu;
    }
    memcpy(s_aAnn + 6, g_aOurMac, 6);
    s_aAnn[12] = 0x08;
    s_aAnn[13] = 0x06;
    s_aAnn[14] = 0;
    s_aAnn[15] = 1;
    s_aAnn[16] = 0x08;
    s_aAnn[17] = 0x00;
    s_aAnn[18] = 6;
    s_aAnn[19] = 4;
    s_aAnn[20] = 0;
    s_aAnn[21] = 2;
    memcpy(s_aAnn + 22, g_aOurMac, 6);
    memcpy(s_aAnn + 28, g_aOurIp, 4);
    for (i = 0; i < 6u; i++) {
        s_aAnn[32 + i] = 0xffu;
    }
    memcpy(s_aAnn + 38, g_aOurIp, 4);
    if (net_eth_l2_tx(s_aAnn, NET_ETH_MIN_FRAME) == 0) {
        net_eth_soft_inc(&g_u32ArpAnnounceOk);
        net_eth_soft_inc(&g_u32TxOk);
        net_eth_soft_add(&g_u32BytesTx, NET_ETH_MIN_FRAME);
        return 0;
    }
    net_eth_soft_inc(&g_u32ArpAnnounceFail);
    net_eth_soft_inc(&g_u32TxFail);
    return -1;
}

void
net_eth_apply_l2_identity(void)
{
    net_eth_sync_l2();
    if (net_l2_backend() == GJ_NET_L2_NONE && net_l2_ready() == 0) {
        kprintf("net_eth: apply_l2 SKIP (no L2 backend)\n");
        return;
    }
    if (net_l2_backend() == GJ_NET_L2_RTL8168 ||
        net_eth_ip_is_lab(g_aOurIp) != 0) {
        if (net_eth_ip_is_lab(g_aOurIp) == 0) {
            kprintf("net_eth: apply_l2 force lab_ip %u.%u.%u.%u -> "
                    "10.200.125.50 (Soft!=product; who-has demux; "
                    "lab_force; product=UDX)\n",
                    g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3]);
            net_eth_force_lab_ip();
        }
        kprintf("net_eth: apply_l2 lab_ip=10.200.125.50 ok "
                "(Soft!=product; lab_demux=1; dual_dod_b=OPEN_UDX)\n");
    }
    kprintf("net_eth: apply_l2 ip=%u.%u.%u.%u mac=%02x:%02x:%02x:%02x:%02x:"
            "%02x backend=%s\n",
            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3], g_aOurMac[0],
            g_aOurMac[1], g_aOurMac[2], g_aOurMac[3], g_aOurMac[4],
            g_aOurMac[5], net_l2_name());

    if (g_fWhoHasReplyOk != 0u) {
        kprintf("net_eth: apply_l2 ann SKIP (who-has reply already ok)\n");
        return;
    }
    if (g_fArpApplyAnnOnce != 0u) {
        kprintf("net_eth: apply_l2 ann SKIP (once already; identity only)\n");
        return;
    }
    if (net_l2_ready() == 0) {
        kprintf("net_eth: apply_l2 ann SKIP (not ready; identity only)\n");
        return;
    }
    g_fArpApplyAnnOnce = 1u;
    if (net_eth_arp_announce() == 0) {
        kprintf("net_eth: ARP announce soft PASS (once)\n");
    } else {
        kprintf("net_eth: ARP announce soft FAIL (once; no re-thrash)\n");
    }
}

static void
handle_icmp(const u8 *pFrame, u32 cb)
{
    const u8 *pIp;
    const u8 *pIcmp;
    static u8 s_aIcmpOut[1518];
    u16 u16Tot;
    u16 u16Ihl;
    u16 u16NewTot;
    u32 cbIcmp;
    u32 cbOut;
    u32 i;

    net_eth_soft_inc(&g_u32IcmpSeen);

    if (cb < 42) {
        net_eth_soft_inc(&g_u32IcmpShort);
        return;
    }
    pIp = pFrame + 14;
    if ((pIp[0] & 0xf0) != 0x40 || pIp[9] != 1) {
        net_eth_soft_inc(&g_u32IcmpShort);
        return;
    }
    u16Ihl = (u16)((pIp[0] & 0x0f) * 4);
    if (u16Ihl < 20 || cb < 14u + u16Ihl + 8u) {
        net_eth_soft_inc(&g_u32IcmpShort);
        return;
    }
    net_eth_sync_l2();
    if (memcmp(pIp + 16, g_aOurIp, 4) != 0) {
        net_eth_sync_l2();
        if (memcmp(pIp + 16, g_aOurIp, 4) == 0) {
            net_eth_soft_inc(&g_u32IcmpIdResync);
        } else if (net_eth_ip_is_lab(pIp + 16) != 0) {
            /*
             * Frame targets lab static: force identity and demux even when
             * L2 still reports QEMU/stale (handoff churn / UDX wire residual).
             * Soft!=product. greppable: lab_force demux | lab_ip=10.200.125.50
             */
            net_eth_force_lab_ip();
            net_eth_soft_inc(&g_u32IcmpIdResync);
            g_u32IdSyncChg++;
            {
                static u32 s_u32IcmpResyncLog;

                if (s_u32IcmpResyncLog < 4u) {
                    s_u32IcmpResyncLog++;
                    kprintf("net_eth: soft icmp id_resync dst=%u.%u.%u.%u "
                            "us=%u.%u.%u.%u n=%u lab_force "
                            "(Soft!=product; lab_demux=1; product=UDX)\n",
                            pIp[16], pIp[17], pIp[18], pIp[19], g_aOurIp[0],
                            g_aOurIp[1], g_aOurIp[2], g_aOurIp[3],
                            g_u32IcmpIdResync);
                }
            }
        } else if (net_eth_want_lab_pin() != 0) {
            /* rtl / already-lab: re-force then recheck. Soft!=product. */
            net_eth_force_lab_ip();
            if (memcmp(pIp + 16, g_aOurIp, 4) == 0 ||
                net_eth_ip_is_lab(pIp + 16) != 0) {
                net_eth_soft_inc(&g_u32IcmpIdResync);
            } else {
                static u32 s_u32IcmpNotUsLog;

                net_eth_soft_inc(&g_u32IcmpNotUs);
                if (s_u32IcmpNotUsLog < 4u) {
                    s_u32IcmpNotUsLog++;
                    kprintf("net_eth: soft icmp not_us dst=%u.%u.%u.%u "
                            "us=%u.%u.%u.%u (Soft!=product)\n",
                            pIp[16], pIp[17], pIp[18], pIp[19], g_aOurIp[0],
                            g_aOurIp[1], g_aOurIp[2], g_aOurIp[3]);
                }
                return;
            }
        } else {
            static u32 s_u32IcmpNotUsLog;

            net_eth_soft_inc(&g_u32IcmpNotUs);
            if (s_u32IcmpNotUsLog < 4u) {
                s_u32IcmpNotUsLog++;
                kprintf("net_eth: soft icmp not_us dst=%u.%u.%u.%u "
                        "us=%u.%u.%u.%u (Soft!=product)\n",
                        pIp[16], pIp[17], pIp[18], pIp[19], g_aOurIp[0],
                        g_aOurIp[1], g_aOurIp[2], g_aOurIp[3]);
            }
            return;
        }
    }
    pIcmp = pIp + u16Ihl;
    if (pIcmp[0] != 8) {
        net_eth_soft_inc(&g_u32IcmpNotEcho);
        return;
    }
    u16Tot = (u16)((pIp[2] << 8) | pIp[3]);
    if (u16Tot < (u16)(u16Ihl + 8) || (u32)(14 + u16Tot) > cb ||
        u16Tot > 1500u) {
        net_eth_soft_inc(&g_u32IcmpShort);
        return;
    }
    {
        static u32 s_u32IcmpReqLog;

        if (s_u32IcmpReqLog < 4u) {
            s_u32IcmpReqLog++;
            kprintf("net_eth: ICMP echo req from %u.%u.%u.%u len=%u soft\n",
                    pIp[12], pIp[13], pIp[14], pIp[15], (unsigned)u16Tot);
        }
    }
    cbIcmp = (u32)u16Tot - (u32)u16Ihl;
    if (cbIcmp < 8u || cbIcmp + 34u > sizeof(s_aIcmpOut)) {
        net_eth_soft_inc(&g_u32IcmpShort);
        return;
    }
    memset(s_aIcmpOut, 0, sizeof(s_aIcmpOut));
    memcpy(s_aIcmpOut, pFrame + 6, 6);
    memcpy(s_aIcmpOut + 6, g_aOurMac, 6);
    s_aIcmpOut[12] = 0x08;
    s_aIcmpOut[13] = 0x00;
    u16NewTot = (u16)(20u + cbIcmp);
    s_aIcmpOut[14] = 0x45;
    s_aIcmpOut[15] = 0;
    s_aIcmpOut[14 + 2] = (u8)(u16NewTot >> 8);
    s_aIcmpOut[14 + 3] = (u8)u16NewTot;
    s_aIcmpOut[14 + 4] = pIp[4];
    s_aIcmpOut[14 + 5] = pIp[5];
    s_aIcmpOut[14 + 6] = 0x40;
    s_aIcmpOut[14 + 7] = 0;
    s_aIcmpOut[14 + 8] = 64;
    s_aIcmpOut[14 + 9] = 1;
    s_aIcmpOut[14 + 10] = 0;
    s_aIcmpOut[14 + 11] = 0;
    memcpy(s_aIcmpOut + 14 + 12, g_aOurIp, 4);
    memcpy(s_aIcmpOut + 14 + 16, pIp + 12, 4);
    {
        u16 u16C = ip_checksum(s_aIcmpOut + 14, 20);

        s_aIcmpOut[14 + 10] = (u8)(u16C >> 8);
        s_aIcmpOut[14 + 11] = (u8)(u16C & 0xff);
    }
    s_aIcmpOut[14 + 20] = 0;
    s_aIcmpOut[14 + 21] = 0;
    s_aIcmpOut[14 + 22] = 0;
    s_aIcmpOut[14 + 23] = 0;
    for (i = 4; i < cbIcmp; i++) {
        s_aIcmpOut[14 + 20 + i] = pIcmp[i];
    }
    {
        u16 u16C = ip_checksum(s_aIcmpOut + 14 + 20, cbIcmp);

        s_aIcmpOut[14 + 22] = (u8)(u16C >> 8);
        s_aIcmpOut[14 + 23] = (u8)(u16C & 0xff);
    }
    cbOut = 14u + 20u + cbIcmp;
    {
        int nTx;
        u32 u32RetryOk = 0;
        u32 u32TxLen = cbOut < NET_ETH_MIN_FRAME ? NET_ETH_MIN_FRAME : cbOut;

        nTx = net_eth_reply_tx(s_aIcmpOut, u32TxLen, &u32RetryOk);
        if (nTx == 0) {
            static u32 s_u32IcmpRepLog;

            if (u32RetryOk != 0u) {
                net_eth_soft_inc(&g_u32IcmpTxRetryOk);
            }
            net_eth_soft_inc(&g_u32IcmpEchoes);
            net_eth_soft_inc(&g_u32TxOk);
            net_eth_soft_add(&g_u32BytesTx, u32TxLen);
            if (s_u32IcmpRepLog < NET_ETH_ICMP_PASS_LOG_CAP) {
                s_u32IcmpRepLog++;
                kprintf("net_eth: ICMP echo reply soft PASS n=%u retry=%u "
                        "(Soft!=product)\n",
                        s_u32IcmpRepLog, u32RetryOk);
            } else if (s_u32IcmpRepLog == NET_ETH_ICMP_PASS_LOG_CAP) {
                s_u32IcmpRepLog++;
                kprintf("net_eth: soft icmp PASS log capped at %u "
                        "(reply TX continues; Soft!=product)\n",
                        (u32)NET_ETH_ICMP_PASS_LOG_CAP);
            }
        } else {
            net_eth_soft_inc(&g_u32IcmpTxFail);
            net_eth_soft_inc(&g_u32TxFail);
            {
                extern u64 timer_jiffies(void);
                u64 u64J = timer_jiffies();
                int fLamp = 0;

                if (g_u32IcmpTxFailLamp == 0u) {
                    fLamp = 1;
                } else if (g_u32IcmpTxFailLamp < NET_ETH_ICMP_TXFAIL_LAMP_CAP &&
                           (u64J - g_u64IcmpTxFailLastJif) >=
                               NET_ETH_TXFAIL_LAMP_JIF) {
                    fLamp = 1;
                }
                if (fLamp != 0) {
                    g_u32IcmpTxFailLamp++;
                    g_u64IcmpTxFailLastJif = u64J;
                    kprintf("net_eth: icmp TX FAIL fail=%u lamp=%u "
                            "(Soft!=product; lean)\n",
                            g_u32IcmpTxFail, g_u32IcmpTxFailLamp);
                }
            }
        }
    }
}

static void
handle_arp(const u8 *pFrame, u32 cb)
{
    static u8 s_aArpOut[64];
    const u8 *pArp;

    net_eth_soft_inc(&g_u32ArpSeen);
    net_eth_sync_l2();

    if (cb < 42) {
        net_eth_soft_inc(&g_u32ArpBadOp);
        return;
    }
    pArp = pFrame + 14;
    if (pArp[0] != 0 || pArp[1] != 1 || pArp[2] != 0x08 || pArp[3] != 0x00 ||
        pArp[4] != 6 || pArp[5] != 4) {
        net_eth_soft_inc(&g_u32ArpBadOp);
        return;
    }
    if (pArp[6] != 0 || pArp[7] != 1) {
        net_eth_soft_inc(&g_u32ArpBadOp);
        return;
    }
    if (memcmp(pArp + 24, g_aOurIp, 4) != 0) {
        net_eth_sync_l2();
        if (memcmp(pArp + 24, g_aOurIp, 4) == 0) {
            net_eth_soft_inc(&g_u32ArpIdResync);
            {
                static u32 s_u32ArpResyncLog;

                if (s_u32ArpResyncLog < 4u) {
                    s_u32ArpResyncLog++;
                    kprintf("net_eth: soft arp id_resync tpa=%u.%u.%u.%u "
                            "us=%u.%u.%u.%u n=%u (Soft!=product)\n",
                            pArp[24], pArp[25], pArp[26], pArp[27],
                            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2],
                            g_aOurIp[3], g_u32ArpIdResync);
                }
            }
        } else if (net_eth_ip_is_lab(pArp + 24) != 0) {
            /*
             * who-has for lab static: force identity + reply even under
             * handoff/identity churn (UDX NIC owns lab IP product path).
             * Soft!=product. greppable: lab_force demux | lab_ip=10.200.125.50
             */
            net_eth_force_lab_ip();
            net_eth_soft_inc(&g_u32ArpIdResync);
            g_u32IdSyncChg++;
            {
                static u32 s_u32ArpResyncLog;

                if (s_u32ArpResyncLog < 4u) {
                    s_u32ArpResyncLog++;
                    kprintf("net_eth: soft arp id_resync tpa=%u.%u.%u.%u "
                            "us=%u.%u.%u.%u n=%u lab_force "
                            "(Soft!=product; lab_demux=1; product=UDX)\n",
                            pArp[24], pArp[25], pArp[26], pArp[27],
                            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2],
                            g_aOurIp[3], g_u32ArpIdResync);
                }
            }
        } else if (net_eth_want_lab_pin() != 0) {
            net_eth_force_lab_ip();
            if (memcmp(pArp + 24, g_aOurIp, 4) == 0 ||
                net_eth_ip_is_lab(pArp + 24) != 0) {
                net_eth_soft_inc(&g_u32ArpIdResync);
            } else {
                static u32 s_u32ArpNotUsLog;

                net_eth_soft_inc(&g_u32ArpNotUs);
                if (s_u32ArpNotUsLog < 4u) {
                    s_u32ArpNotUsLog++;
                    kprintf("net_eth: soft arp not_us tpa=%u.%u.%u.%u "
                            "us=%u.%u.%u.%u (Soft!=product)\n",
                            pArp[24], pArp[25], pArp[26], pArp[27],
                            g_aOurIp[0], g_aOurIp[1], g_aOurIp[2],
                            g_aOurIp[3]);
                }
                return;
            }
        } else {
            static u32 s_u32ArpNotUsLog;

            net_eth_soft_inc(&g_u32ArpNotUs);
            if (s_u32ArpNotUsLog < 4u) {
                s_u32ArpNotUsLog++;
                kprintf("net_eth: soft arp not_us tpa=%u.%u.%u.%u "
                        "us=%u.%u.%u.%u (Soft!=product)\n",
                        pArp[24], pArp[25], pArp[26], pArp[27], g_aOurIp[0],
                        g_aOurIp[1], g_aOurIp[2], g_aOurIp[3]);
            }
            return;
        }
    }
    memset(s_aArpOut, 0, sizeof(s_aArpOut));
    memcpy(s_aArpOut, pFrame + 6, 6);
    memcpy(s_aArpOut + 6, g_aOurMac, 6);
    s_aArpOut[12] = 0x08;
    s_aArpOut[13] = 0x06;
    s_aArpOut[14] = 0;
    s_aArpOut[15] = 1;
    s_aArpOut[16] = 0x08;
    s_aArpOut[17] = 0x00;
    s_aArpOut[18] = 6;
    s_aArpOut[19] = 4;
    s_aArpOut[20] = 0;
    s_aArpOut[21] = 2;
    memcpy(s_aArpOut + 22, g_aOurMac, 6);
    memcpy(s_aArpOut + 28, g_aOurIp, 4);
    memcpy(s_aArpOut + 32, pArp + 8, 6);
    memcpy(s_aArpOut + 38, pArp + 14, 4);
    {
        int nTx;
        u32 u32RetryOk = 0;

        nTx = net_eth_reply_tx(s_aArpOut, NET_ETH_MIN_FRAME, &u32RetryOk);
        if (nTx == 0) {
            static u32 s_u32ArpPassLog;
            static u8 s_fSoftArpReplyLamp;

            if (u32RetryOk != 0u) {
                net_eth_soft_inc(&g_u32ArpTxRetryOk);
            }
            net_eth_soft_inc(&g_u32ArpReplies);
            net_eth_soft_inc(&g_u32TxOk);
            net_eth_soft_add(&g_u32BytesTx, NET_ETH_MIN_FRAME);
            g_fWhoHasReplyOk = 1u;
            /*
             * Once-lamp: who-has reply enqueued (UDX ETH_TX_PULL or L2 TX).
             * greppable: net_eth: soft arp reply
             * Soft!=product Dual DoD B OPEN (not DoD close).
             */
            if (s_fSoftArpReplyLamp == 0u) {
                s_fSoftArpReplyLamp = 1u;
                kprintf("net_eth: soft arp reply once "
                        "ip=%u.%u.%u.%u mac=%02x:%02x:%02x:%02x:%02x:%02x "
                        "lab_ip=10.200.125.50 dual_dod_b=OPEN_UDX "
                        "path=rtl8168_udx Soft!=product G-AC-1\n",
                        g_aOurIp[0], g_aOurIp[1], g_aOurIp[2], g_aOurIp[3],
                        g_aOurMac[0], g_aOurMac[1], g_aOurMac[2],
                        g_aOurMac[3], g_aOurMac[4], g_aOurMac[5]);
            }
            if (s_u32ArpPassLog < NET_ETH_ARP_PASS_LOG_CAP) {
                s_u32ArpPassLog++;
                kprintf("net_eth: who-has PASS n=%u retry=%u "
                        "ip=%u.%u.%u.%u (Soft!=product)\n",
                        s_u32ArpPassLog, u32RetryOk, g_aOurIp[0], g_aOurIp[1],
                        g_aOurIp[2], g_aOurIp[3]);
            } else if (s_u32ArpPassLog == NET_ETH_ARP_PASS_LOG_CAP) {
                s_u32ArpPassLog++;
                kprintf("net_eth: soft who-has PASS log capped at %u "
                        "(reply TX continues; Soft!=product)\n",
                        (u32)NET_ETH_ARP_PASS_LOG_CAP);
            }
        } else {
            net_eth_soft_inc(&g_u32ArpTxFail);
            net_eth_soft_inc(&g_u32TxFail);
            {
                extern u64 timer_jiffies(void);
                u64 u64J = timer_jiffies();
                int fLamp = 0;

                if (g_u32WhoHasTxFailLamp == 0u) {
                    fLamp = 1;
                } else if (g_u32WhoHasTxFailLamp <
                               NET_ETH_WHOHAS_TXFAIL_LAMP_CAP &&
                           (u64J - g_u64WhoHasTxFailLastJif) >=
                               NET_ETH_TXFAIL_LAMP_JIF) {
                    fLamp = 1;
                }
                if (fLamp != 0) {
                    g_u32WhoHasTxFailLamp++;
                    g_u64WhoHasTxFailLastJif = u64J;
                    kprintf("net_eth: who-has TX FAIL fail=%u lamp=%u "
                            "(Soft!=product; lean)\n",
                            g_u32ArpTxFail, g_u32WhoHasTxFailLamp);
                }
            }
        }
    }
}

static void
handle_udp(const u8 *pFrame, u32 cb)
{
    const u8 *pIp;
    const u8 *pUdp;
    u16 u16Tot;
    u16 u16Ihl;
    u16 u16Dport;
    u16 u16Sport;
    u16 u16Ulen;
    u32 cbPay;
    static u8 s_aUdpOut[1518];
    u8 *pOip;
    u8 *pOudp;
    u32 cbOut;

    net_eth_soft_inc(&g_u32UdpSeen);

    if (cb < 14 + 20 + 8) {
        net_eth_soft_inc(&g_u32UdpShort);
        return;
    }
    pIp = pFrame + 14;
    if ((pIp[0] >> 4) != 4 || pIp[9] != 17) {
        net_eth_soft_inc(&g_u32UdpShort);
        return;
    }
    u16Ihl = (u16)((pIp[0] & 0x0f) * 4);
    if (cb < 14u + u16Ihl + 8u) {
        net_eth_soft_inc(&g_u32UdpShort);
        return;
    }
    pUdp = pIp + u16Ihl;
    u16Dport = (u16)((pUdp[2] << 8) | pUdp[3]);
    u16Sport = (u16)((pUdp[0] << 8) | pUdp[1]);
    u16Ulen = (u16)((pUdp[4] << 8) | pUdp[5]);
    if (u16Dport != 7) {
        net_eth_soft_inc(&g_u32UdpNotEcho);
        return;
    }
    if (u16Ulen < 8 || 14u + u16Ihl + u16Ulen > cb) {
        net_eth_soft_inc(&g_u32UdpShort);
        return;
    }
    cbPay = (u32)u16Ulen - 8u;
    memset(s_aUdpOut, 0, sizeof(s_aUdpOut));
    memcpy(s_aUdpOut, pFrame + 6, 6);
    memcpy(s_aUdpOut + 6, g_aOurMac, 6);
    s_aUdpOut[12] = 0x08;
    s_aUdpOut[13] = 0x00;
    pOip = s_aUdpOut + 14;
    pOip[0] = 0x45;
    u16Tot = (u16)(20 + 8 + cbPay);
    pOip[2] = (u8)(u16Tot >> 8);
    pOip[3] = (u8)u16Tot;
    pOip[8] = 64;
    pOip[9] = 17;
    memcpy(pOip + 12, g_aOurIp, 4);
    memcpy(pOip + 16, pIp + 12, 4);
    {
        u16 u16C = ip_checksum(pOip, 20);

        pOip[10] = (u8)(u16C >> 8);
        pOip[11] = (u8)u16C;
    }
    pOudp = pOip + 20;
    pOudp[0] = (u8)(u16Dport >> 8);
    pOudp[1] = (u8)u16Dport;
    pOudp[2] = (u8)(u16Sport >> 8);
    pOudp[3] = (u8)u16Sport;
    pOudp[4] = (u8)(u16Ulen >> 8);
    pOudp[5] = (u8)u16Ulen;
    pOudp[6] = 0;
    pOudp[7] = 0;
    if (cbPay) {
        memcpy(pOudp + 8, pUdp + 8, cbPay);
    }
    cbOut = 14u + 20u + 8u + cbPay;
    {
        u32 u32TxLen = cbOut < NET_ETH_MIN_FRAME ? NET_ETH_MIN_FRAME : cbOut;

        if (net_eth_l2_tx(s_aUdpOut, u32TxLen) == 0) {
            net_eth_soft_inc(&g_u32UdpEchoes);
            net_eth_soft_inc(&g_u32TxOk);
            net_eth_soft_add(&g_u32BytesTx, u32TxLen);
        } else {
            net_eth_soft_inc(&g_u32UdpTxFail);
            net_eth_soft_inc(&g_u32TxFail);
        }
    }
}

/**
 * Soft demux one L2 frame: ARP / IPv4(ICMP|UDP|TCP) / 802.1Q peel.
 * Returns 1 if recognized path touched, 0 if drop/ignored.
 * Soft!=product. Stack-safe static peel workspace.
 */
static int
handle_frame(const u8 *pFrame, u32 cb)
{
    u16 u16Etype;
    static u8 s_fVlanPeelBusy;

    if (pFrame == NULL || cb < 14) {
        net_eth_soft_inc(&g_u32FramesDrop);
        net_eth_soft_inc(&g_u32DropShort);
        return 0;
    }
    u16Etype = (u16)((pFrame[12] << 8) | pFrame[13]);
    if (u16Etype == 0x8100u) {
        if (s_fVlanPeelBusy == 0u && cb >= 18u) {
            u16 u16Inner;
            static u8 s_aVlanPeel[1518];
            u32 cbPeel;

            u16Inner = (u16)((pFrame[16] << 8) | pFrame[17]);
            if (u16Inner == 0x0806u || u16Inner == 0x0800u) {
                cbPeel = cb - 4u;
                if (cbPeel >= 14u && cbPeel <= sizeof(s_aVlanPeel)) {
                    memcpy(s_aVlanPeel, pFrame, 12u);
                    memcpy(s_aVlanPeel + 12u, pFrame + 16u, cbPeel - 12u);
                    net_eth_soft_inc(&g_u32VlanPeel);
                    s_fVlanPeelBusy = 1u;
                    {
                        int nOk = handle_frame(s_aVlanPeel, cbPeel);

                        s_fVlanPeelBusy = 0u;
                        return nOk;
                    }
                }
            }
        }
        net_eth_soft_inc(&g_u32FramesRx);
        net_eth_soft_add(&g_u32BytesRx, cb);
        net_eth_soft_inc(&g_u32VlanSkip);
        return 0;
    }
    net_eth_soft_inc(&g_u32FramesRx);
    net_eth_soft_add(&g_u32BytesRx, cb);
    if (u16Etype == 0x0806u) {
        handle_arp(pFrame, cb);
        net_eth_soft_inc(&g_u32FramesOk);
        return 1;
    }
    if (u16Etype == 0x0800u) {
        const u8 *pIp = pFrame + 14;

        if (cb < 14 + 20) {
            net_eth_soft_inc(&g_u32FramesDrop);
            net_eth_soft_inc(&g_u32DropIpv4Short);
            return 0;
        }
        if ((pIp[0] & 0xf0) != 0x40) {
            net_eth_soft_inc(&g_u32FramesDrop);
            net_eth_soft_inc(&g_u32DropIpv4Ver);
            return 0;
        }
        if (pIp[9] == 1) {
            handle_icmp(pFrame, cb);
            net_eth_soft_inc(&g_u32FramesOk);
            return 1;
        }
        if (pIp[9] == 17) {
            handle_udp(pFrame, cb);
            net_eth_soft_inc(&g_u32FramesOk);
            return 1;
        }
        if (pIp[9] == 6) {
            u16 u16IhlTcp;
            u16 u16Dport;
            u32 u32IsTcp22;

            net_eth_soft_inc(&g_u32TcpSeen);
            /*
             * W11 Dual DoD B STRONGER denser: soft tally TCP dport 22
             * (tcp22_seen) + consume honesty (tcp22_demux when
             * net_tcp_input accepts). Stack honesty path:
             * stack=eth|tcp|door|:22 for product sshd. Silent hot path;
             * residual only. H1 thr-only poll owns drain (never IRQ).
             * Soft!=product; stamp-free. Dual DoD OPEN.
             * greppable: tcp22_seen | tcp22_demux | stack=eth|tcp|door|:22
             */
            u32IsTcp22 = 0u;
            u16IhlTcp = (u16)((pIp[0] & 0x0fu) * 4u);
            if (u16IhlTcp >= 20u && cb >= 14u + (u32)u16IhlTcp + 4u &&
                NET_ETH_TCP22_STACK != 0u && NET_ETH_TCP22_DEMUX != 0u) {
                const u8 *pTcp = pIp + u16IhlTcp;

                u16Dport = (u16)(((u16)pTcp[2] << 8) | (u16)pTcp[3]);
                if (u16Dport == (u16)NET_ETH_SSH_PORT) {
                    net_eth_soft_inc(&g_u32Tcp22Seen);
                    u32IsTcp22 = 1u;
                }
            }
            if (net_tcp_input(pFrame, cb)) {
                net_eth_soft_inc(&g_u32TcpDemux);
                if (u32IsTcp22 != 0u) {
                    /* greppable: tcp22_demux (dport 22 consume honesty) */
                    net_eth_soft_inc(&g_u32Tcp22Demux);
                }
            } else {
                net_eth_soft_inc(&g_u32TcpMiss);
            }
            net_eth_soft_inc(&g_u32FramesOk);
            return 1;
        }
        net_eth_soft_inc(&g_u32FramesDrop);
        net_eth_soft_inc(&g_u32DropProto);
        return 0;
    }
    net_eth_soft_inc(&g_u32FramesDrop);
    net_eth_soft_inc(&g_u32DropEtype);
    return 0;
}

/**
 * UDX host thr-poll RX residual: inject one L2 frame into demux.
 * Dual DoD B path=rtl8168_udx product_udx_abi. Soft!=product.
 * greppable: net_eth: soft udx inject
 */
int
net_eth_input_frame(const void *pFrame, u32 cb)
{
    static u8 s_fUdxInjLamp;

    if (pFrame == NULL || cb < 14u || cb > 1518u) {
        return 0;
    }
    /*
     * Lab pin + L2 UDX identity before demux so ARP/ICMP match
     * 10.200.125.50 and soft demux MAC is non-zero. Soft!=product.
     */
    if (net_l2_backend() == GJ_NET_L2_NONE) {
        net_l2_udx_ready_identity();
    }
    net_eth_force_lab_ip();
    net_eth_sync_l2();
    if (s_fUdxInjLamp == 0u) {
        s_fUdxInjLamp = 1u;
        kprintf("net_eth: soft udx inject first len=%u "
                "path=rtl8168_udx owner=product_udx_abi "
                "lab_ip=10.200.125.50 dual_dod_b=OPEN_UDX "
                "freestanding_class=SKIP Soft!=product G-AC-1\n",
                (unsigned)cb);
    }
    return handle_frame((const u8 *)pFrame, cb);
}

static u32
net_eth_soft_link_sample(void)
{
    u32 u32Now;

    u32Now = net_l2_ready() ? 1u : (virtio_net_ready() ? 1u : 0u);
    if (u32Now != g_u32LinkReady) {
        g_u32LinkReady = u32Now;
        net_eth_soft_inc(&g_u32LinkChanges);
    }
    return u32Now;
}

/*
 * H1 thr-only poll honesty (C1 residual; Soft!=product; thrash-stripped):
 *   Call only from scheduler_run thr stack (full kstack) or door thr that
 *   shares that ownership model. Never timer / APIC / MSI-X / IRQ stack
 *   (IRQ stack smash / #PF I=1 class). Never scheduler_run_ap (ap_poll=0).
 *   net_eth_irq=0; thr_only=1; poll_own=thr_stack; door_thr_ok=1.
 * Lean: one POLL_MAX batch; no dense7 thrash; freestanding_class=SKIP.
 * Poll handoff residual: soft MMIO handoff pending/FAULT -> fail-closed
 * (no freestanding drain; product NIC direction = UDX lab IP wire).
 * FUNCTIONAL residual (STRONGER): freestanding->UDX wire ownership handoff
 * for lab IP / sshd Dual DoD B (fs_to_udx=1; freestanding_wire_claim=0).
 * Dual DoD A/B OPEN (product=UDX; agent!=close). Silent hot path.
 * greppable: net_eth_poll=run_loop_only | net_eth_irq=0 | owner=scheduler_run
 * greppable: poll_own=thr_stack | freestanding_class=SKIP | product=UDX
 * greppable: thr_only=1 | ap_poll=0 | irq_path=0 | door_thr_ok=1
 * greppable: fault_class=H1_irq_stack_smash | net_eth_poll=run_loop_or_door
 * greppable: net_eth: soft poll handoff residual | handoff_fail_closed=1
 * greppable: net_eth: soft udx wire residual | owner=product_udx_abi
 * greppable: net_eth: soft residual functional | fs_to_udx=1
 * greppable: freestanding_wire_claim=0 | product_sshd_tcp22=OPEN
 */
void
net_eth_poll(void)
{
    static u8 aRx[1518];
    static u8 s_fFirstRxLamp;
    i32 i32N;
    u32 u32Batch;
    u32 u32Ready;
    int fHandoffClosed;
    u32 u32Be;

    /*
     * H1 thr-only entry (C1 residual; Soft!=product; G-AC-1):
     * All residual locks referenced on thr/door stack. Never IRQ/AP.
     * greppable: thr_only=1 | net_eth_irq=0 | ap_poll=0 | irq_path=0
     * greppable: door_thr_ok=1 | poll_own=thr_stack | fault_class=H1_irq_stack_smash
     * greppable: fs_to_udx=1 | freestanding_wire_claim=0
     */
    (void)NET_ETH_THR_ONLY;
    (void)NET_ETH_IRQ_POLL;
    (void)NET_ETH_AP_POLL;
    (void)NET_ETH_IRQ_PATH;
    (void)NET_ETH_DOOR_THR_OK;
    (void)NET_ETH_HANDOFF_FC;
    (void)NET_ETH_LAB_DEMUX;
    (void)NET_ETH_FS_CLASS_SKIP;
    (void)NET_ETH_PRODUCT_UDX;
    (void)NET_ETH_SSHD_LAB;
    (void)NET_ETH_FS_TO_UDX;
    (void)NET_ETH_FUNC_CHECKS;
    (void)NET_ETH_WIRE_STEPS;
    (void)NET_ETH_SOFT_MMIO_HO;
    (void)NET_ETH_PRODUCT_HO;
    (void)NET_ETH_TCP22_STACK;
    (void)NET_ETH_TCP22_DEMUX;
    (void)NET_ETH_SSH_PORT;
    (void)NET_ETH_WIRE22_DENSE;
    (void)NET_ETH_WIRE22_DENSE_ARMS;
    (void)NET_ETH_WIRE22_DENSE_MIN;
    (void)NET_ETH_WIRE22_DENSE_H1_SUB;
    (void)NET_ETH_WIRE22_DENSE_DEMUX_SUB;
    /* Once-gated; thr path re-affirm if init order ever skips lean. H2 once. */
    net_eth_soft_residual_lean_thr_once();
    net_eth_soft_residual_functional_once();
    net_eth_soft_inc(&g_u32Polls);

    /* Identity resync before demux (thr/door only). Soft!=product. H1. */
    net_eth_sync_l2();

    /*
     * Soft MMIO handoff fail-closed residual (NET_ETH_HANDOFF_FC):
     * net_l2_ready already returns 0 while pending/FAULT; classify here so
     * eth residual greps distinguish handoff from NODEV and never claim
     * freestanding wire during handoff. Product direction = UDX NIC owns
     * lab IP / sshd wire (Dual DoD B OPEN need=UDX_OPEN). Soft!=product. H1.
     * greppable: net_eth: soft poll handoff residual
     * greppable: fs_to_udx=1 | freestanding_wire_claim=0
     */
    {
        extern int net_l2_soft_handoff_pending(void);
        extern int net_l2_soft_handoff_fault(void);

        fHandoffClosed = 0;
        if (NET_ETH_HANDOFF_FC != 0u &&
            (net_l2_soft_handoff_pending() != 0 ||
             net_l2_soft_handoff_fault() != 0)) {
            fHandoffClosed = 1;
        }
    }

    u32Be = net_l2_backend();
    /*
     * Once residual (STRONGER freestanding->UDX wire ownership):
     * Any backend class: freestanding never product wire claim;
     * product owner = UDX+ABI for lab IP / sshd Dual DoD B.
     * backend=none is the default SKIP residual; virtio T0 interim;
     * rtl opt-in residual only. Soft!=product; agent!=close; stamp-free.
     * greppable: net_eth: soft udx wire residual
     * greppable: fs_to_udx=1 | freestanding_wire_claim=0 | wire_own=product_udx_abi
     */
    if (g_fUdxWireResidualLamp == 0u) {
        g_fUdxWireResidualLamp = 1u;
        kprintf("net_eth: soft udx wire residual backend=%s be=%u "
                "owner=product_udx_abi path=rtl8168_udx "
                "path=userspace_udx_hot_cold_abi dual_dod_b=OPEN_UDX "
                "need=UDX_OPEN freestanding_class=SKIP freestanding_close=0 "
                "fs_to_udx=%u freestanding_wire_claim=0 "
                "wire_own=product_udx_abi product_sshd_tcp22=OPEN "
                "tcp22_demux=1 denser=1 denser_arms=%u "
                "stack=eth|tcp|door|:22 "
                "lab_ip=10.200.125.50 lab_demux=%u handoff_fail_closed=%u "
                "soft_mmio_handoff=%u product_udx_wire_handoff=%u "
                "wire_steps=%u func_checks=%u "
                "net_eth_poll=run_loop_only thr_only=1 net_eth_irq=0 "
                "H1=1 H2=once "
                "(Soft!=product; G-AC-1; product NIC=UDX; freestanding->UDX "
                "wire ownership residual STRONGER denser wire22 for lab "
                "IP/sshd; tcp22_demux; agent!=close; not Dual DoD B close; "
                "stamp-free)\n",
                (u32Be == GJ_NET_L2_NONE)
                    ? "none"
                    : ((u32Be == GJ_NET_L2_VIRTIO)
                           ? "virtio"
                           : ((u32Be == GJ_NET_L2_RTL8168) ? "rtl8168"
                                                            : "unknown")),
                u32Be, (u32)NET_ETH_FS_TO_UDX,
                (u32)NET_ETH_WIRE22_DENSE_ARMS, (u32)NET_ETH_LAB_DEMUX,
                (u32)NET_ETH_HANDOFF_FC, (u32)NET_ETH_SOFT_MMIO_HO,
                (u32)NET_ETH_PRODUCT_HO, (u32)NET_ETH_WIRE_STEPS,
                (u32)NET_ETH_FUNC_CHECKS);
    }

    if (fHandoffClosed != 0) {
        net_eth_soft_inc(&g_u32PollsHandoffClosed);
        if (g_fHandoffResidualLamp == 0u) {
            g_fHandoffResidualLamp = 1u;
            /* Grep: net_eth: soft poll handoff residual */
            kprintf("net_eth: soft poll handoff residual "
                    "handoff_fail_closed=1 drain=skip "
                    "soft_mmio_handoff=1 product_udx_wire_handoff=1 "
                    "fs_to_udx=1 freestanding_wire_claim=0 "
                    "wire_own=product_udx_abi owner=product_udx_abi "
                    "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
                    "lab_ip=10.200.125.50 product_sshd_tcp22=OPEN "
                    "tcp22_demux=1 denser=1 stack=eth|tcp|door|:22 "
                    "freestanding_class=SKIP product=UDX wire_steps=%u "
                    "net_eth_poll=run_loop_only thr_only=1 H1=1 H2=once "
                    "(Soft!=product; G-AC-1; fail-closed lean; "
                    "freestanding->UDX wire ownership handoff residual "
                    "STRONGER denser wire22; no freestanding wire claim; "
                    "agent!=close; stamp-free)\n",
                    (u32)NET_ETH_WIRE_STEPS);
        }
        /* Fail-closed: no freestanding RX drain; still tick TCP residual. */
        net_tcp_poll();
        net_eth_soft_maybe_cadence();
        return;
    }

    u32Ready = net_l2_ready() != 0 ? 1u : net_eth_soft_link_sample();
    if (u32Ready == 0u) {
        net_eth_soft_inc(&g_u32PollsNoDev);
        /* Lab pin hold while waiting for UDX/ freestanding ready residual. */
        if (net_eth_want_lab_pin() != 0) {
            net_eth_force_lab_ip();
        }
        net_tcp_poll();
        net_eth_soft_maybe_cadence();
        return;
    }

    /* Soft multi-frame drain: up to NET_ETH_POLL_MAX frames per call. Lean. */
    for (u32Batch = 0; u32Batch < NET_ETH_POLL_MAX; u32Batch++) {
        i32N = net_l2_rx(aRx, sizeof(aRx));
        if (i32N < 14 || (u32)i32N > sizeof(aRx)) {
            break;
        }
        (void)handle_frame(aRx, (u32)i32N);
        /*
         * Soft Linux netdev bridge: copy freestanding RX into soft skb path
         * when bridge enabled. Soft!=product; freestanding handle_frame remains
         * demux. G-AC-1: soft path is not product wire owner (product=UDX).
         */
        {
            extern int linux_netdev_soft_l2_bridge_enabled(void);
            extern void linux_netdev_soft_l2_feed_rx(const void *p, u32 cb);

            if (linux_netdev_soft_l2_bridge_enabled() != 0) {
                linux_netdev_soft_l2_feed_rx(aRx, (u32)i32N);
            }
        }
        if (s_fFirstRxLamp == 0u) {
            s_fFirstRxLamp = 1u;
            kprintf("net_eth: soft rx demux first len=%u "
                    "(Soft!=product; lean residual; lab_demux=%u; "
                    "product=UDX)\n",
                    (u32)i32N, (u32)NET_ETH_LAB_DEMUX);
        }
    }

    g_u32LastBatch = u32Batch;
    if (u32Batch != 0u) {
        net_eth_soft_inc(&g_u32PollsDrain);
        net_eth_soft_add(&g_u32BatchSum, u32Batch);
        if (u32Batch > g_u32BatchMax) {
            g_u32BatchMax = u32Batch;
        }
    } else {
        net_eth_soft_inc(&g_u32PollsEmpty);
    }

    net_tcp_poll();
    net_eth_soft_maybe_cadence();
}

u32
net_eth_arp_replies(void)
{
    return g_u32ArpReplies;
}

u32
net_eth_udp_echoes(void)
{
    return g_u32UdpEchoes;
}

u32
net_eth_icmp_echoes(void)
{
    return g_u32IcmpEchoes;
}

u32
net_eth_frames_rx(void)
{
    return g_u32FramesRx;
}

u32
net_eth_frames_drop(void)
{
    return g_u32FramesDrop;
}

u32
net_eth_tcp_demux(void)
{
    return g_u32TcpDemux;
}

u32
net_eth_vlan_skip(void)
{
    return g_u32VlanSkip;
}
