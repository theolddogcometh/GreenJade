/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * L2 backend mux: virtio-net (QEMU T0 product) preferred when present;
 * freestanding rtl8168 only when GJ_RTL8168_PROBE=1 (default 0 - product
 * SKIP, same policy as freestanding USB). Pure C11. Dual MIT OR Apache-2.0.
 *
 * Freestanding rtl SKIP residual (GJ_RTL8168_PROBE=0; Soft!=product * G-AC-1):
 *   *** C0 FREESTANDING SKIP RESIDUAL HONESTY · NOT PRODUCT Dual DoD B ***
 *   Product laptop NIC = Linux-shaped userspace UDX over hot+cold ABI
 *   (path=userspace_udx_hot_cold_abi; path=rtl8168_udx; not in-kernel .ko
 *   wire; not freestanding class thrash). Virtio T0 still selected when
 *   present (T0 product net until UDX owns laptop wire). Do NOT re-enable
 *   freestanding rtl by default. SKIP lamps denser product=UDX+ABI; soft
 *   residual lean only (once / hard-cap; C0_SKIP=1 freestanding_close=0
 *   stamp_storm=0 version_stamp=0). Under SKIP + backend=none:
 *   wire/owner=product_udx_abi lean honesty (not freestanding wire claim).
 *   Under SKIP + virtio T0: freestanding_rtl=SKIP still holds; laptop
 *   product NIC direction remains UDX+ABI (Dual DoD B OPEN need=UDX_OPEN).
 *   TX/RX under backend=none: once residual lamp only (no freestanding
 *   thrash; fail closed lean). Soft listen :22 residual != Dual DoD B
 *   close (DoD B = UDX NIC wire/stack/sshd). Soft!=product * G-AC-1 *
 *   dual MIT OR Apache-2.0. Stamp-free residual (never bump GJ_IMAGE_VERSION).
 *   Greppable: net_l2: freestanding rtl SKIP GJ_RTL8168_PROBE=0
 *   Greppable: net_l2: soft residual freestanding rtl SKIP
 *   Greppable: net_l2: soft residual product=UDX+ABI
 *   Greppable: net_l2: soft residual lean
 *   Greppable: net_l2: soft residual product_udx_abi honesty
 *   Greppable: net_l2: soft residual skip tx/rx none
 *   Greppable: path=userspace_udx_hot_cold_abi | path=rtl8168_udx
 *   Greppable: role=skip_rtl_product_udx_abi
 *   Greppable: owner=product_udx_abi
 *   Greppable: need=UDX_OPEN | freestanding_rtl=SKIP | C0_SKIP=1
 *   Greppable: freestanding_close=0 | dual_dod_b=OPEN_UDX
 *
 * Soft Linux netdev coexistence (lamp only; Soft!=product * G-AC-1):
 *   When linux_netdev_soft_count()≥1 and primary exists, net_l2 may note
 *   that a soft Linux-shaped netdev is present. TX/RX stay on freestanding
 *   backend when selected (virtio|rtl8168). Soft note does not dual-drive
 *   BAR/rings. Soft r8169 is never product TX/RX owner (no .ko wire;
 *   Soft!=product). Lab rtl8168 static IP: 10.200.125.50. MAC refresh
 *   after reclaim_wire. Under freestanding rtl SKIP: denser product=UDX+ABI
 *   lean residual only (no freestanding rtl RX/TX thrash; no L2 bridge
 *   dual DoD B residual under SKIP). Soft!=product once-lamps / hard-cap.
 *   Greppable: net_l2: soft linux netdev note n=N (Soft!=product)
 *   Greppable: net_l2: soft linux note no dual-drive (Soft!=product)
 *   Greppable: net_l2: soft refresh mac=... (after reclaim; Soft!=product)
 *   Greppable: net_l2: soft bridge hold14 freestanding wire ... (Soft!=product)
 *   Greppable: net_l2: lab wire honesty ... (Soft!=product; G-AC-1)
 *   Greppable: net_l2: soft ready handoff listen ... (feeds net_tcp :22)
 *   Greppable: net_l2: soft gate0 honesty ... (hybrid SOFT; bridge!=MMIO handoff)
 *
 * Soft ready handoff -> net_tcp listen path (Dual DoD B residual):
 *   net_l2_ready()!=0 is the freestanding usable-wire signal. net_tcp
 *   tcp_soft_ensure_listen22 mints / holds soft listen :22 only after ready.
 *   Handoff pending/FAULT -> ready=0 (fail-closed; no dead :22 mint).
 *   Soft L2 bridge ON is copy-only - never a substitute for freestanding
 *   ready (Soft L2 bridge ON != freestanding MMIO handoff * Soft!=product).
 *
 * Hybrid SOFT gate0 honesty (default lab path):
 *   GJ_SOFT_R8169_MMIO_HANDOFF==0 -> freestanding owns BAR/wire; soft EMU
 *   bind only; Soft L2 bridge may ON as R/T mirror. Soft!=product * G-AC-1.
 *
 * Lab dual DoD B residual - freestanding L2 TX under host arping+ping/sshd
 * eth (Soft!=product):
 *   net_l2_tx must not sticky-fail ring-busy as permanent error so ARP/ICMP/TCP
 *   replies leave. rtl8168 free-scan + gentle OWN wait; L2 adds pre-TX settle,
 *   sliced poll_hw reclaim, and short busy re-try (busy != fail; not sticky).
 *   Virtio path: freestanding T0 product counters only (no OWN settle).
 *   Handoff pending/FAULT still fail-closed (!= busy). Soft L2 bridge is
 *   copy-only - hold14 R/T mirror tracks freestanding (bridge != product wire).
 *   Greppable: net_l2: soft tx busy residual ... (Soft!=product)
 *   Greppable: net_l2: soft tx busy retry ok ... (Soft!=product)
 *   Greppable: net_l2: soft tx fail residual ... (Soft!=product)
 *   Greppable: net_l2: soft tx handoff residual ... (Soft!=product)
 *   Greppable: net_l2: soft tx residual freestanding ... (Soft!=product)
 *   Greppable: net_l2: soft bridge hold14 freestanding wire ... (Soft!=product)
 *
 * L2 bridge RX residual - freestanding rtl frames -> eth/tcp demux
 * (Dual DoD B * Soft!=product * G-AC-1):
 *   DUT symptom class: L2 BR RX=0 while TX>0 under hybrid SOFT gate0
 *   freestanding UP; host arping 0 replies. Strengthen net_l2_rx multi-try
 *   poll_hw reclaim so freestanding completions leave the ring when rtl
 *   delivers; caller (net_eth) demuxes ARP/ICMP and feeds net_tcp :22.
 *   pre-RX settle+poll * denser empty re-try * denser settle slice * final
 *   poll after empty. Soft bridge tallies lean: SoftRxOk + hold14 refresh
 *   on ok (eth owns feed_rx copy - no double-count / no stamp flood).
 *   Lab IP identity 10.200.125.50; MAC identity honesty from rtl station.
 *   Greppable: net_l2: soft rx residual freestanding ... (Soft!=product)
 *   Greppable: net_l2: soft rx demux eth/tcp ... (Soft!=product)
 *   Greppable: net_l2: soft bridge rx tally lean ... (Soft!=product)
 *   Greppable: net_l2: soft mac identity honesty ... (Soft!=product)
 *   Greppable: net_l2: soft lab ip identity ... (Soft!=product)
 *   No stamp storms: residual lamps once / hard-cap (NET_L2_SOFT_*_LAMP_CAP).
 *   No inventory flood: soft note + ready handoff are once-only; no poll dump.
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/linux_netdev_soft.h>
#include <gj/linux_pci_soft.h>
#include <gj/net_door.h>
#include <gj/net_l2.h>
#include <gj/rtl8168.h>
#include <gj/string.h>
#include <gj/types.h>
#include <gj/virtio_net.h>

/* QEMU user-net shaped (SLIRP) */
static const u8 g_aVirtIp[4] = { 10, 0, 2, 15 };
static const u8 g_aVirtMac[6] = { 0x52, 0x54, 0x00, 0x12, 0x34, 0x56 };

/*
 * Lab static IPv4 when Realtek is up (G752 on operator LAN).
 * DUT sits on 10.200.125.0/24 - pick a free host; not a DHCP client (honest).
 * Override by editing this constant and rebuilding if .50 is taken.
 */
static const u8 g_aRtlIp[4] = { 10, 200, 125, 50 };

/*
 * Soft demux lab MAC residual (backend=none + ETH_UDX_READY).
 * LAB_MAC_UDX=02:00:00:47:4a:50 — identical 6 bytes as rtl8168_udx product
 * IDR lab_fallback (user/drivers/rtl8168_udx/rtl8168_udx.c). Soft demux MAC
 * aligns with product idr lab_fallback so ARP SHA matches on wire residual.
 * Soft!=product Dual DoD B OPEN.
 * OPEN: product idr keep=1 (EEPROM station) still leaves soft demux on this
 * lab LAA until a path publishes NIC station MAC into net_l2 (no door API).
 * greppable: LAB_MAC_UDX=02:00:00:47:4a:50
 */
static const u8 g_aLabMacUdx[6] = { 0x02, 0x00, 0x00, 0x47, 0x4a, 0x50 };

static u32 g_u32Backend = GJ_NET_L2_NONE;
static u8 g_aIp[4];
static u8 g_aMac[6];
/** Once-ish soft coexistence lamp (does not switch TX/RX). */
static int g_fSoftLinuxNoted;
/** Soft MMIO handoff: freestanding quiesced, soft open not complete. */
static int g_fSoftHandoffPending;
/** Soft MMIO handoff: dual-drive FAULT (fail closed). */
static int g_fSoftHandoffFault;
static int g_fSoftHandoffFaultLogged;

/*
 * Soft freestanding TX residual under host arping+ping (lab dual DoD B).
 * Soft!=product. busy != fail: ring OWN full is retryable; handoff/nodev/bad-args
 * are hard fail. SoftTxBusy exhaust is not sticky - next frame may leave.
 * Grep: net_l2: soft tx
 */
static u32 g_u32SoftTxOk;
static u32 g_u32SoftTxBusy;          /* still busy after L2 re-tries (not sticky) */
static u32 g_u32SoftTxFail;          /* hard fail (handoff / nodev / backend) */
static u32 g_u32SoftTxBusyRetryOk;   /* ok after L2 busy re-try (try>0) */
static u32 g_u32SoftTxHandoffClosed; /* fail-closed while handoff pending/FAULT */
static u32 g_u32SoftTxBusyLamp;      /* rate-limit busy residual lamps */
static u32 g_u32SoftTxFailLamp;      /* rate-limit hard-fail residual lamps */
static u32 g_u32SoftTxBusyPre;       /* pre-first-try busy samples (flood residual) */
static u32 g_u32SoftReadyLamp;       /* rate-limit soft ready handoff lamps */
static u32 g_u32SoftNoteLamp;        /* rate-limit soft linux note residual lines */
static u32 g_u32SoftMacRefreshLamp;  /* rate-limit MAC refresh residual */
static int g_fSoftTxResidualNoted;   /* once residual honesty lamp */
static int g_fSoftBridgeHonestyNoted; /* once hold14 bridge != product wire lamp */
static int g_fLabWireHonestyNoted;   /* once lab wire owner honesty lamp */
static int g_fSoftReadyHandoffNoted; /* once soft ready -> net_tcp listen handoff */
static int g_fSoftGate0HonestyNoted; /* once hybrid SOFT gate0 honesty */
/*
 * Freestanding rtl SKIP + product_udx_abi denser residual honesty (once).
 * Soft!=product * G-AC-1. No freestanding NIC thrash. Dual DoD B OPEN via UDX.
 */
static int g_fSoftSkipProductUdxAbiResidualNoted;
/* TX/RX under backend=none + SKIP: once residual lamp (no thrash). Soft!=product. */
static int g_fSoftSkipTxRxNoneResidualNoted;

/*
 * Soft freestanding RX residual (lab dual DoD B * L2 bridge -> eth/tcp).
 * Soft!=product. Frames leave freestanding rtl; net_eth demuxes; soft bridge
 * tallies lean (hold14). Soft r8169 never owns RX. Grep: net_l2: soft rx
 */
static u32 g_u32SoftRxOk;            /* freestanding frames returned to demux */
static u32 g_u32SoftRxEmpty;         /* empty after L2 multi-try reclaim */
static u32 g_u32SoftRxHandoffClosed; /* fail-closed while handoff pending/FAULT */
static u32 g_u32SoftRxRetryOk;       /* ok after L2 empty re-try (try>0) */
static u32 g_u32SoftRxLamp;          /* rate-limit RX residual lamps */
static u32 g_u32SoftRxBridgeLamp;    /* rate-limit lean bridge RX tally lamps */
static int g_fSoftRxResidualNoted;   /* once RX residual freestanding lamp */
static int g_fSoftRxDemuxNoted;      /* once demux eth/tcp honesty lamp */
static int g_fSoftMacIdHonestyNoted; /* once MAC identity honesty lamp */
static int g_fSoftLabIpHonestyNoted; /* once lab IP 10.200.125.50 honesty */

/*
 * L2 busy re-tries under flood residual (lab dual DoD B). Soft!=product.
 * Single-try callers (TCP SYN-ACK / soft l2_xmit) need room under ring pressure;
 * net_eth has its own multi-try - L2 covers everyone else.
 */
#define NET_L2_TX_BUSY_TRIES 5u
/* Cap residual TX lamps (continuous arping must not flood serial -> stack risk). */
#define NET_L2_SOFT_TX_LAMP_CAP 6u
/* Cap busy->ok salvage lamps (subset of TX residual; no stamp storm). */
#define NET_L2_SOFT_TX_RETRY_OK_LAMP_CAP 4u
/* Cap soft ready / note residual lines (no inventory flood on poll paths). */
#define NET_L2_SOFT_READY_LAMP_CAP 2u
#define NET_L2_SOFT_NOTE_LAMP_CAP  8u
#define NET_L2_SOFT_MAC_LAMP_CAP   2u
/* Soft settle bases (pause loops) - keep short under flood. Soft!=product. */
#define NET_L2_TX_SETTLE_BASE  4000u
#define NET_L2_TX_SETTLE_STEP  4000u
#define NET_L2_TX_SETTLE_BUSY  8000u
#define NET_L2_TX_SETTLE_PRE   5000u
#define NET_L2_TX_SETTLE_SLICE 512u
/*
 * L2 RX multi-try reclaim (Dual DoD B residual * .68 L2 BR RX=0 class).
 * Soft!=product. DUT freestanding UP + TX>0 while host arping 0 replies:
 * strengthen multi-try poll_hw reclaim so rtl completions leave ring for
 * net_eth demux (ARP/ICMP) + net_tcp :22. Cap still short enough that
 * net_eth batch drain is not starved. Cap RX lamps (no stamp flood).
 *   PRE settle  - reclaim OWN / IntrStatus before first rtl8168_rx
 *   EMPTY_TRIES - denser re-try under R0/R1 lag (rtl residual needs polls)
 *   SETTLE+STEP - between empty tries; denser SLICE -> more poll_hw
 *   final poll  - after empty exhaust so next eth drain can hit
 */
#define NET_L2_RX_EMPTY_TRIES 5u
#define NET_L2_RX_SETTLE      4000u
#define NET_L2_RX_SETTLE_STEP 2500u
#define NET_L2_RX_SETTLE_PRE  3000u
#define NET_L2_RX_SETTLE_SLICE 256u
#define NET_L2_SOFT_RX_LAMP_CAP        4u
#define NET_L2_SOFT_RX_BRIDGE_LAMP_CAP 3u
#define NET_L2_SOFT_RX_RETRY_OK_LAMP_CAP 3u

/* Forward: honesty lamps use backend name before net_l2_name body. */
static const char *net_l2_name_local(void);
static void net_l2_soft_ready_handoff_listen_once(int fReady);
static void net_l2_soft_gate0_honesty_once(int nProbeMode);
static void net_l2_soft_gate0_honesty_denser(int nProbeMode);
static void net_l2_soft_mac_ip_identity_honesty_once(void);
static void net_l2_soft_rx_bridge_lean(u32 u32Len);
static void net_l2_soft_skip_product_udx_abi_residual_once(void);
static void net_l2_soft_skip_tx_rx_none_residual_once(const char *szOp);

/*
 * Freestanding rtl SKIP + product_udx_abi denser residual honesty (once).
 * Soft!=product * G-AC-1. Product laptop NIC = userspace rtl8168_udx over
 * hot+cold ABI (not freestanding class thrash; not in-kernel .ko wire).
 * Dual DoD B stays OPEN_UDX need=UDX_OPEN - freestanding residual never
 * closes. Virtio T0 still product net when present. NEVER re-enable
 * freestanding rtl. Dual MIT OR Apache-2.0. Once only - no stamp storm /
 * no inventory flood. C0_SKIP=1 freestanding_close=0 stamp_storm=0
 * version_stamp=0 (stamp-free residual; never bump GJ_IMAGE_VERSION).
 * Grep: net_l2: soft residual lean
 * Grep: net_l2: soft residual product_udx_abi honesty
 * Grep: owner=product_udx_abi
 * Grep: role=skip_rtl_product_udx_abi
 * Grep: need=UDX_OPEN
 * Grep: freestanding_rtl=SKIP
 * Grep: path=userspace_udx_hot_cold_abi | path=rtl8168_udx
 * Grep: C0_SKIP=1 | freestanding_close=0 | dual_dod_b=OPEN_UDX
 */
static void
net_l2_soft_skip_product_udx_abi_residual_once(void)
{
#if !GJ_RTL8168_PROBE
    if (g_fSoftSkipProductUdxAbiResidualNoted != 0) {
        return;
    }
    g_fSoftSkipProductUdxAbiResidualNoted = 1;

    /* Grep: net_l2: soft residual lean */
    kprintf("net_l2: soft residual lean soft=1 product=0 claim=0 "
            "freestanding_rtl=SKIP rtl_probe=0 "
            "owner=product_udx_abi role=skip_rtl_product_udx_abi "
            "product=UDX+ABI path=userspace_udx_hot_cold_abi "
            "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
            "backend=%s soft_r8169=not_tx_rx "
            "not=in_kernel_r8169_ko not=freestanding_rtl_as_product "
            "not=freestanding_class_thrash "
            "t0_product_net=virtio-net thr_only_eth=1 "
            "freestanding_close=0 C0_SKIP=1 stamp_storm=0 version_stamp=0 "
            "dual=MIT_OR_Apache-2.0 "
            "(Soft!=product; G-AC-1=1; lean residual once; "
            "not re-enable freestanding rtl; not Dual DoD B close)\n",
            net_l2_name_local());

    /* Grep: net_l2: soft residual product_udx_abi honesty */
    kprintf("net_l2: soft residual product_udx_abi honesty "
            "wire=product_udx_abi owner=product_udx_abi "
            "role=skip_rtl_product_udx_abi "
            "product=UDX+ABI path=userspace_udx_hot_cold_abi "
            "path=rtl8168_udx "
            "direction=userspace_rtl8168_udx_over_hot_cold_abi "
            "freestanding_rtl=SKIP dual_dod_b=OPEN_UDX "
            "need=UDX_OPEN soft=1 product=0 G-AC-1=1 "
            "backend=%s soft_r8169=not_tx_rx "
            "freestanding_close=0 C0_SKIP=1 stamp_storm=0 version_stamp=0 "
            "(Soft!=product; freestanding rtl SKIP default; "
            "not freestanding wire claim; not re-enable freestanding rtl; "
            "lean residual only; not Dual DoD B close; "
            "dual MIT OR Apache-2.0)\n",
            net_l2_name_local());
#else
    /* Opt-in freestanding rtl residual: no SKIP product_udx denser lamp. */
    (void)0;
#endif
}

/*
 * TX/RX under freestanding rtl SKIP + backend=none residual honesty (once).
 * Soft!=product * G-AC-1. No freestanding wire - product NIC = UDX+ABI.
 * Fail closed lean; no OWN thrash / no dual DoD B freestanding climb.
 * Grep: net_l2: soft residual skip tx/rx none
 */
static void
net_l2_soft_skip_tx_rx_none_residual_once(const char *szOp)
{
#if !GJ_RTL8168_PROBE
    if (g_fSoftSkipTxRxNoneResidualNoted != 0) {
        return;
    }
    if (g_u32Backend != GJ_NET_L2_NONE) {
        return;
    }
    g_fSoftSkipTxRxNoneResidualNoted = 1;
    /* Grep: net_l2: soft residual skip tx/rx none */
    kprintf("net_l2: soft residual skip tx/rx none op=%s "
            "backend=none freestanding_rtl=SKIP "
            "owner=product_udx_abi role=skip_rtl_product_udx_abi "
            "product=UDX+ABI path=userspace_udx_hot_cold_abi "
            "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
            "soft_r8169=not_tx_rx freestanding_close=0 C0_SKIP=1 "
            "(Soft!=product; G-AC-1; fail closed lean; no freestanding "
            "wire thrash; not Dual DoD B close; not re-enable freestanding rtl)\n",
            (szOp != NULL) ? szOp : "?");
#else
    (void)szOp;
#endif
}

/*
 * Lab wire honesty (once). Freestanding mux owns TX/RX; soft r8169 does not.
 * Soft!=product * G-AC-1. Grep: net_l2: lab wire honesty
 */
static void
net_l2_lab_wire_honesty_once(void)
{
    const char *szRole;

    if (g_fLabWireHonestyNoted != 0) {
        return;
    }
    g_fLabWireHonestyNoted = 1;

    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        szRole = "t0_product_virtio";
    } else if (g_u32Backend == GJ_NET_L2_RTL8168) {
        szRole = "lab_rtl_dual_dod_b";
    } else {
        /*
         * backend=none: freestanding rtl SKIP residual (default) is
         * product=UDX+ABI lean honesty - not freestanding class thrash.
         * Soft!=product * G-AC-1.
         */
#if !GJ_RTL8168_PROBE
        szRole = "skip_rtl_product_udx_abi";
#else
        szRole = "none";
#endif
    }

    /* Grep: net_l2: lab wire honesty ... (Soft!=product; G-AC-1) */
#if !GJ_RTL8168_PROBE
    if (g_u32Backend == GJ_NET_L2_NONE) {
        /*
         * Freestanding rtl SKIP + no freestanding backend: product NIC
         * direction = userspace UDX over hot+cold ABI. Not freestanding
         * wire claim. Soft!=product * G-AC-1.
         * Grep: role=skip_rtl_product_udx_abi
         * Grep: path=userspace_udx_hot_cold_abi
         * Grep: owner=product_udx_abi
         * Grep: need=UDX_OPEN
         */
        kprintf("net_l2: lab wire honesty backend=none "
                "owner=product_udx_abi soft_r8169=not_tx_rx role=%s "
                "product=UDX+ABI path=userspace_udx_hot_cold_abi "
                "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
                "freestanding_rtl=SKIP freestanding_close=0 C0_SKIP=1 "
                "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
                "mux residual; no dual-drive; not freestanding class thrash; "
                "not Dual DoD B close; not re-enable freestanding rtl)\n",
                szRole);
        return;
    }
    /*
     * Freestanding backend under SKIP default: only virtio T0 can own
     * wire (rtl never selected when GJ_RTL8168_PROBE=0). Tag
     * freestanding_rtl=SKIP so serial residual matches product law
     * (laptop NIC = UDX+ABI; Dual DoD B OPEN_UDX need=UDX_OPEN). Soft!=product
     * * G-AC-1 * C0 * once. Grep: freestanding_rtl=SKIP | need=UDX_OPEN
     * Grep: C0_SKIP=1 | freestanding_close=0
     */
    kprintf("net_l2: lab wire honesty backend=%s owner=freestanding "
            "soft_r8169=not_tx_rx role=%s freestanding_rtl=SKIP "
            "product=UDX+ABI path=userspace_udx_hot_cold_abi "
            "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
            "freestanding_close=0 C0_SKIP=1 "
            "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
            "virtio T0 wire when present; mux residual; no dual-drive; "
            "not freestanding class thrash; not Dual DoD B close; "
            "not re-enable freestanding rtl)\n",
            net_l2_name_local(), szRole);
#else
    kprintf("net_l2: lab wire honesty backend=%s owner=freestanding "
            "soft_r8169=not_tx_rx role=%s (Soft!=product; G-AC-1; "
            "mux residual; no dual-drive; Soft!=product wire)\n",
            net_l2_name_local(), szRole);
#endif
}

void
net_l2_init(void)
{
    g_u32Backend = GJ_NET_L2_NONE;
    memset(g_aIp, 0, 4);
    memset(g_aMac, 0, 6);

    if (virtio_net_ready() != 0) {
        g_u32Backend = GJ_NET_L2_VIRTIO;
        memcpy(g_aIp, g_aVirtIp, 4);
        memcpy(g_aMac, g_aVirtMac, 6);
        kprintf("net_l2: backend=virtio ip=%u.%u.%u.%u (QEMU SLIRP)\n",
                g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3]);
        /*
         * T0 product freestanding residual: virtio owns wire on QEMU.
         * Soft r8169 never selected. Soft!=product * G-AC-1.
         * Grep: net_l2: soft tx residual freestanding
         */
        kprintf("net_l2: soft tx residual freestanding backend=virtio "
                "role=t0_product (Soft!=product; freestanding wire; "
                "soft_r8169=not_tx_rx)\n");
        g_fSoftTxResidualNoted = 1;
        net_l2_lab_wire_honesty_once();
        /*
         * Soft ready handoff residual: freestanding T0 virtio usable ->
         * net_tcp may soft listen :22. Soft!=product.
         * Grep: net_l2: soft ready handoff listen
         */
        (void)net_l2_ready();
        /* gate0 honesty early (soft may arrive later). Soft!=product. */
        net_l2_soft_gate0_honesty_once(/*nProbeMode*/ 0);
        /*
         * Freestanding rtl SKIP residual still holds when virtio T0 owns
         * wire. Product laptop NIC direction remains UDX+ABI (not
         * freestanding rtl class thrash). Dual DoD B OPEN need=UDX_OPEN.
         * Once-only denser lean (no stamp storm). Soft!=product * G-AC-1.
         * Grep: freestanding_rtl=SKIP | soft residual lean
         * Grep: soft residual product_udx_abi honesty | need=UDX_OPEN
         */
#if !GJ_RTL8168_PROBE
        net_l2_soft_skip_product_udx_abi_residual_once();
#endif
        return;
    }

    /*
     * Freestanding rtl SKIP default (operator 2026-08): not product.
     * Same class as freestanding USB MSC SKIP. Product laptop NIC =
     * Linux-shaped userspace UDX over hot+cold ABI (G-AC-1; not .ko
     * in-kernel wire). Opt-in: -DGJ_RTL8168_PROBE=1 residual only.
     * Do NOT re-enable freestanding rtl by default. Virtio T0 already
     * returned above when present. Soft residual lean only - denser
     * product=UDX+ABI SKIP lamps (no dual DoD B freestanding R-climb).
     * Grep: net_l2: freestanding rtl SKIP GJ_RTL8168_PROBE=0
     * Grep: net_l2: soft residual freestanding rtl SKIP
     * Grep: net_l2: soft residual product=UDX+ABI
     */
#if !GJ_RTL8168_PROBE
    /* Grep: net_l2: freestanding rtl SKIP GJ_RTL8168_PROBE=0 */
    kprintf("net_l2: freestanding rtl SKIP GJ_RTL8168_PROBE=0 "
            "product=UDX+ABI path=userspace_udx_hot_cold_abi "
            "path=rtl8168_udx owner=product_udx_abi freestanding_rtl=SKIP "
            "dual_dod_b=OPEN_UDX need=UDX_OPEN "
            "freestanding_close=0 C0_SKIP=1 stamp_storm=0 version_stamp=0 "
            "(Soft!=product; G-AC-1; not in-kernel .ko wire; "
            "not freestanding class thrash; virtio T0 still ok when present; "
            "soft residual lean only; not Dual DoD B close; "
            "not re-enable freestanding rtl)\n");
    /* Grep: net_l2: soft residual freestanding rtl SKIP */
    kprintf("net_l2: soft residual freestanding rtl SKIP "
            "rtl_probe=0 product=UDX+ABI path=userspace_udx_hot_cold_abi "
            "path=rtl8168_udx owner=product_udx_abi role=skip_rtl_product_udx_abi "
            "dual_dod_b=OPEN_UDX need=UDX_OPEN freestanding_rtl=SKIP "
            "freestanding_close=0 C0_SKIP=1 stamp_storm=0 "
            "(Soft!=product; G-AC-1; Linux-shaped userspace UDX; "
            "not freestanding class thrash; lean residual only; "
            "virtio T0 when present; not Dual DoD B close; "
            "not re-enable freestanding rtl)\n");
    /* Grep: net_l2: soft residual product=UDX+ABI */
    /* Grep: path=userspace_udx_hot_cold_abi | path=rtl8168_udx */
    /* Grep: owner=product_udx_abi need=UDX_OPEN freestanding_rtl=SKIP */
    /* Grep: C0_SKIP=1 freestanding_close=0 */
    kprintf("net_l2: soft residual product=UDX+ABI "
            "backend=none rtl_probe=0 virtio_t0=prefer "
            "owner=product_udx_abi role=skip_rtl_product_udx_abi "
            "path=userspace_udx_hot_cold_abi path=rtl8168_udx "
            "direction=userspace_nic_host_over_hot_cold_abi "
            "not=in_kernel_r8169_ko not=freestanding_rtl_as_product "
            "dual_dod_b=OPEN_UDX need=UDX_OPEN freestanding_rtl=SKIP "
            "soft=1 product=0 freestanding_close=0 C0_SKIP=1 "
            "stamp_storm=0 version_stamp=0 thr_only_eth=1 "
            "(Soft!=product; G-AC-1; freestanding rtl SKIP default; "
            "not re-enable freestanding rtl; lean residual only; "
            "not Dual DoD B close)\n");
    kprintf("net_l2: backend=none (no virtio-net; freestanding rtl SKIP; "
            "product=UDX+ABI; path=userspace_udx_hot_cold_abi; "
            "path=rtl8168_udx; owner=product_udx_abi; "
            "dual_dod_b=OPEN_UDX; C0_SKIP=1)\n");
    net_l2_lab_wire_honesty_once();
    net_l2_soft_gate0_honesty_once(/*nProbeMode*/ -1);
    /* Denser SKIP + product_udx_abi residual honesty (once). Soft!=product. */
    net_l2_soft_skip_product_udx_abi_residual_once();
    return;
#else
    (void)rtl8168_probe();
    if (rtl8168_ready() != 0) {
        g_u32Backend = GJ_NET_L2_RTL8168;
        memcpy(g_aIp, g_aRtlIp, 4);
        rtl8168_mac(g_aMac);
        kprintf("net_l2: backend=rtl8168 ip=%u.%u.%u.%u mac=%02x:%02x:%02x:"
                "%02x:%02x:%02x link=%u\n",
                g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3], g_aMac[0], g_aMac[1],
                g_aMac[2], g_aMac[3], g_aMac[4], g_aMac[5],
                rtl8168_link_up() ? 1u : 0u);
        kprintf("net_l2: lab static IP - set host route/ARP; ssh root@%u.%u."
                "%u.%u (when link+sshd up)\n",
                g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3]);
        /*
         * Lab dual DoD B residual honesty: freestanding wire owns TX under
         * host arping/ping/sshd. Soft!=product - not bar3 / not G-AC-1 product.
         * Grep: net_l2: soft tx residual freestanding
         */
        kprintf("net_l2: soft tx residual freestanding backend=rtl8168 "
                "busy_tries=%u (Soft!=product; busy!=fail; not sticky; "
                "arping/ping/TCP replies leave; soft_r8169=not_tx_rx)\n",
                (unsigned)NET_L2_TX_BUSY_TRIES);
        g_fSoftTxResidualNoted = 1;
        net_l2_lab_wire_honesty_once();
        /*
         * Dual DoD B identity residual: MAC honesty + lab IP 10.200.125.50
         * before first RX demux (ARP/ICMP/TCP match). Soft!=product.
         * Grep: net_l2: soft mac identity honesty
         * Grep: net_l2: soft lab ip identity
         */
        net_l2_soft_mac_ip_identity_honesty_once();
        /*
         * L2 bridge RX residual honesty (once at init - not poll path).
         * Freestanding rtl frames -> eth/tcp demux; soft bridge tallies lean.
         * Soft!=product. Grep: net_l2: soft rx residual freestanding
         */
        kprintf("net_l2: soft rx residual freestanding backend=rtl8168 "
                "empty_tries=%u ip=%u.%u.%u.%u "
                "(Soft!=product; dual DoD B; demux->eth/tcp; "
                "soft bridge tallies lean; soft_r8169=not_tx_rx)\n",
                (unsigned)NET_L2_RX_EMPTY_TRIES, g_aIp[0], g_aIp[1],
                g_aIp[2], g_aIp[3]);
        g_fSoftRxResidualNoted = 1;
        /*
         * Soft ready handoff residual: freestanding rtl usable -> net_tcp
         * soft listen :22 path may mint (Dual DoD B). Soft!=product.
         * Grep: net_l2: soft ready handoff listen
         */
        (void)net_l2_ready();
        /* Hybrid SOFT gate0 honesty (handoff gate default 0). Soft!=product. */
        net_l2_soft_gate0_honesty_once(/*nProbeMode*/ 0);
        return;
    }

    kprintf("net_l2: backend=none (no virtio-net, no rtl8168)\n");
    net_l2_lab_wire_honesty_once();
    net_l2_soft_gate0_honesty_once(/*nProbeMode*/ -1);
#endif /* GJ_RTL8168_PROBE */
}

u32
net_l2_backend(void)
{
    return g_u32Backend;
}

void
net_l2_refresh_mac(void)
{
    if (g_u32Backend != GJ_NET_L2_RTL8168) {
        return;
    }
    rtl8168_mac(g_aMac);
    /* Lab IP re-assert after reclaim (demux identity). Soft!=product. */
    memcpy(g_aIp, g_aRtlIp, 4);
    /* Hard-cap: reclaim may re-enter; no MAC stamp storm. Soft!=product. */
    if (g_u32SoftMacRefreshLamp < NET_L2_SOFT_MAC_LAMP_CAP) {
        g_u32SoftMacRefreshLamp++;
        kprintf("net_l2: soft refresh mac=%02x:%02x:%02x:%02x:%02x:%02x "
                "ip=%u.%u.%u.%u lamp=%u "
                "(after reclaim; Soft!=product; hard-cap; "
                "MAC+lab IP identity honesty)\n",
                g_aMac[0], g_aMac[1], g_aMac[2], g_aMac[3], g_aMac[4],
                g_aMac[5], g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3],
                (unsigned)g_u32SoftMacRefreshLamp);
    }
}

/*
 * Soft ready handoff residual for net_tcp listen :22 (Dual DoD B).
 * net_l2_ready()!=0 is the freestanding usable-wire signal. Once ready,
 * net_tcp may mint/hold soft listen :22 (tcp_soft_ensure_listen22).
 * Handoff pending/FAULT -> ready=0 (fail-closed; no dead mint).
 * Soft L2 bridge ON is not a ready substitute (copy-only != MMIO handoff).
 * Soft!=product * G-AC-1 * hard-cap (no inventory flood on poll).
 * Grep: net_l2: soft ready handoff listen
 */
static void
net_l2_soft_ready_handoff_listen_once(int fReady)
{
    const char *szRole;

    if (fReady == 0) {
        return;
    }
    if (g_fSoftReadyHandoffNoted != 0) {
        return;
    }
    if (g_u32SoftReadyLamp >= NET_L2_SOFT_READY_LAMP_CAP) {
        return;
    }
    g_fSoftReadyHandoffNoted = 1;
    g_u32SoftReadyLamp++;

    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        szRole = "t0_product_virtio";
    } else if (g_u32Backend == GJ_NET_L2_RTL8168) {
        szRole = "lab_rtl_dual_dod_b";
    } else {
        szRole = "none";
    }

    /* Grep: net_l2: soft ready handoff listen ... (Soft!=product) */
#if !GJ_RTL8168_PROBE
    /*
     * Freestanding rtl SKIP residual: soft listen :22 residual is eng
     * only. Dual DoD B product close = UDX NIC (need=UDX_OPEN), not
     * freestanding ready / soft :22 lamp. Soft!=product * G-AC-1.
     * Grep: dual_dod_b=OPEN_UDX | freestanding_rtl=SKIP | C0_SKIP=1
     */
    kprintf("net_l2: soft ready handoff listen backend=%s "
            "ip=%u.%u.%u.%u role=%s handoff_pending=%u handoff_fault=%u "
            "bridge=%s freestanding_rtl=SKIP product=UDX+ABI "
            "path=userspace_udx_hot_cold_abi dual_dod_b=OPEN_UDX "
            "need=UDX_OPEN freestanding_close=0 C0_SKIP=1 "
            "(Soft!=product; G-AC-1; feeds net_tcp soft listen :22 eng; "
            "soft :22 != Dual DoD B close; ready=1; "
            "Soft L2 bridge ON!=MMIO handoff; no inventory flood)\n",
            net_l2_name_local(),
            g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3], szRole,
            (g_fSoftHandoffPending != 0) ? 1u : 0u,
            (g_fSoftHandoffFault != 0) ? 1u : 0u,
            (linux_netdev_soft_l2_bridge_enabled() != 0) ? "copy_only" : "off");
#else
    kprintf("net_l2: soft ready handoff listen backend=%s "
            "ip=%u.%u.%u.%u role=%s handoff_pending=%u handoff_fault=%u "
            "bridge=%s (Soft!=product; feeds net_tcp soft listen :22; "
            "ready=1; Soft L2 bridge ON!=MMIO handoff; no inventory flood)\n",
            net_l2_name_local(),
            g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3], szRole,
            (g_fSoftHandoffPending != 0) ? 1u : 0u,
            (g_fSoftHandoffFault != 0) ? 1u : 0u,
            (linux_netdev_soft_l2_bridge_enabled() != 0) ? "copy_only" : "off");
#endif
}

/*
 * Hybrid SOFT gate0 honesty (once). Default lab path: freestanding owns
 * BAR/wire when a freestanding backend is selected; soft EMU bind; Soft L2
 * bridge may ON as copy-only R/T mirror. Soft L2 bridge ON != freestanding
 * MMIO handoff (gate still off). Under freestanding rtl SKIP + backend=none:
 * product=UDX+ABI lean (wire=product_udx_abi; not freestanding wire claim).
 * Soft!=product * G-AC-1. Grep: net_l2: soft gate0 honesty
 */
static void
net_l2_soft_gate0_honesty_once(int nProbeMode)
{
    u32 u32HandoffGate;
    const char *szProbe;
    const char *szGate;
    const char *szWire;

    if (g_fSoftGate0HonestyNoted != 0) {
        return;
    }
    g_fSoftGate0HonestyNoted = 1;

    u32HandoffGate = (u32)GJ_SOFT_R8169_MMIO_HANDOFF;
    if (nProbeMode == 1) {
        szProbe = "REAL";
    } else if (nProbeMode == 0) {
        szProbe = "SOFT";
    } else {
        szProbe = "EMU";
    }
    szGate = (u32HandoffGate == 0u) ? "gate0" : "gate1";
    /*
     * Freestanding rtl SKIP residual honesty: no freestanding NIC backend
     * selected -> product laptop NIC direction is UDX+ABI (not freestanding
     * wire thrash). Virtio T0 still wire=freestanding when present.
     * Soft!=product * G-AC-1.
     */
#if !GJ_RTL8168_PROBE
    if (g_u32Backend == GJ_NET_L2_NONE) {
        szWire = "product_udx_abi";
        /* Grep: net_l2: soft gate0 honesty ... product=UDX+ABI */
        /* Grep: owner=product_udx_abi need=UDX_OPEN freestanding_rtl=SKIP */
        /* Grep: C0_SKIP=1 freestanding_close=0 dual_dod_b=OPEN_UDX */
        kprintf("net_l2: soft gate0 honesty hybrid=%s wire=%s "
                "owner=product_udx_abi "
                "probe=%s handoff_gate=%u bridge=%s mmio_handoff=%s "
                "soft_r8169=not_tx_rx product=UDX+ABI "
                "path=userspace_udx_hot_cold_abi path=rtl8168_udx "
                "role=skip_rtl_product_udx_abi "
                "dual_dod_b=OPEN_UDX need=UDX_OPEN freestanding_rtl=SKIP "
                "freestanding_close=0 C0_SKIP=1 stamp_storm=0 "
                "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
                "Soft L2 bridge ON!=MMIO handoff; not freestanding class thrash; "
                "not re-enable freestanding rtl; lean residual only; "
                "not Dual DoD B close)\n",
                szGate, szWire, szProbe, (unsigned)u32HandoffGate,
                (linux_netdev_soft_l2_bridge_enabled() != 0) ? "copy_only" : "off",
                (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0)
                    ? "pending_or_fault"
                    : "off");
        return;
    }
    /*
     * Virtio T0 wire under freestanding rtl SKIP: wire=freestanding (virtio)
     * but freestanding_rtl=SKIP residual still holds (laptop product NIC =
     * UDX+ABI; Dual DoD B OPEN_UDX). Soft!=product * G-AC-1 * C0.
     * Grep: freestanding_rtl=SKIP | need=UDX_OPEN | C0_SKIP=1
     */
    szWire = "freestanding";
    kprintf("net_l2: soft gate0 honesty hybrid=%s wire=%s "
            "probe=%s handoff_gate=%u bridge=%s mmio_handoff=%s "
            "soft_r8169=not_tx_rx freestanding_rtl=SKIP "
            "product=UDX+ABI path=userspace_udx_hot_cold_abi "
            "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
            "role=t0_product_virtio_under_rtl_skip "
            "freestanding_close=0 C0_SKIP=1 "
            "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
            "virtio T0 wire when present; Soft L2 bridge ON!=MMIO handoff; "
            "soft ready feeds net_tcp listen :22 eng only; "
            "not Dual DoD B close; not freestanding class thrash)\n",
            szGate, szWire, szProbe, (unsigned)u32HandoffGate,
            (linux_netdev_soft_l2_bridge_enabled() != 0) ? "copy_only" : "off",
            (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0)
                ? "pending_or_fault"
                : "off");
    return;
#endif
    szWire = "freestanding";

    /* Grep: net_l2: soft gate0 honesty ... (Soft!=product; G-AC-1) */
    kprintf("net_l2: soft gate0 honesty hybrid=%s wire=%s "
            "probe=%s handoff_gate=%u bridge=%s mmio_handoff=%s "
            "soft_r8169=not_tx_rx (Soft!=product; G-AC-1; "
            "Soft L2 bridge ON!=freestanding MMIO handoff; "
            "soft ready feeds net_tcp listen :22 only)\n",
            szGate, szWire, szProbe, (unsigned)u32HandoffGate,
            (linux_netdev_soft_l2_bridge_enabled() != 0) ? "copy_only" : "off",
            (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0)
                ? "pending_or_fault"
                : "off");
}

/*
 * Denser gate0 honesty after soft note + optional Soft L2 bridge ON.
 * Init may have lamped provisional gate0 (bridge=off) before soft present;
 * soft_linux_note re-lamps once under NOTE hard-cap with live probe/bridge.
 * Soft L2 bridge copy-only != MMIO handoff. Under freestanding rtl SKIP +
 * backend=none: denser product=UDX+ABI lean (wire=product_udx_abi).
 * Soft!=product * G-AC-1. Grep: net_l2: soft gate0 honesty
 */
static void
net_l2_soft_gate0_honesty_denser(int nProbeMode)
{
    u32 u32HandoffGate;
    const char *szProbe;
    const char *szGate;
    const char *szBridge;
    const char *szWire;

    /* Prefer first-time once path when init never lamped. */
    if (g_fSoftGate0HonestyNoted == 0) {
        net_l2_soft_gate0_honesty_once(nProbeMode);
        return;
    }
    /* Already once - denser residual only under note hard-cap. */
    if (g_u32SoftNoteLamp >= NET_L2_SOFT_NOTE_LAMP_CAP) {
        return;
    }
    g_u32SoftNoteLamp++;

    u32HandoffGate = (u32)GJ_SOFT_R8169_MMIO_HANDOFF;
    if (nProbeMode == 1) {
        szProbe = "REAL";
    } else if (nProbeMode == 0) {
        szProbe = "SOFT";
    } else {
        szProbe = "EMU";
    }
    szGate = (u32HandoffGate == 0u) ? "gate0" : "gate1";
    szBridge = (linux_netdev_soft_l2_bridge_enabled() != 0) ? "copy_only" : "off";
#if !GJ_RTL8168_PROBE
    if (g_u32Backend == GJ_NET_L2_NONE) {
        szWire = "product_udx_abi";
        /* Grep: net_l2: soft gate0 honesty denser ... product=UDX+ABI */
        /* Grep: owner=product_udx_abi need=UDX_OPEN freestanding_rtl=SKIP */
        /* Grep: C0_SKIP=1 freestanding_close=0 dual_dod_b=OPEN_UDX */
        kprintf("net_l2: soft gate0 honesty denser hybrid=%s wire=%s "
                "owner=product_udx_abi "
                "probe=%s handoff_gate=%u bridge=%s mmio_handoff=%s "
                "soft_r8169=not_tx_rx product=UDX+ABI "
                "path=userspace_udx_hot_cold_abi path=rtl8168_udx "
                "role=skip_rtl_product_udx_abi "
                "dual_dod_b=OPEN_UDX need=UDX_OPEN freestanding_rtl=SKIP "
                "freestanding_close=0 C0_SKIP=1 "
                "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
                "Soft L2 bridge ON!=MMIO handoff; not freestanding class thrash; "
                "not re-enable freestanding rtl; not Dual DoD B close; "
                "hard-cap note lamp=%u)\n",
                szGate, szWire, szProbe, (unsigned)u32HandoffGate, szBridge,
                (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0)
                    ? "pending_or_fault"
                    : "off",
                (unsigned)g_u32SoftNoteLamp);
        return;
    }
    /*
     * Virtio T0 under freestanding rtl SKIP denser residual. Soft!=product.
     * Grep: freestanding_rtl=SKIP | dual_dod_b=OPEN_UDX | C0_SKIP=1
     */
    szWire = "freestanding";
    kprintf("net_l2: soft gate0 honesty denser hybrid=%s wire=%s "
            "probe=%s handoff_gate=%u bridge=%s mmio_handoff=%s "
            "soft_r8169=not_tx_rx freestanding_rtl=SKIP "
            "product=UDX+ABI path=userspace_udx_hot_cold_abi "
            "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
            "role=t0_product_virtio_under_rtl_skip "
            "freestanding_close=0 C0_SKIP=1 "
            "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
            "virtio T0 wire when present; Soft L2 bridge ON!=MMIO handoff; "
            "soft ready->net_tcp :22 eng only; not Dual DoD B close; "
            "hard-cap note lamp=%u)\n",
            szGate, szWire, szProbe, (unsigned)u32HandoffGate, szBridge,
            (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0)
                ? "pending_or_fault"
                : "off",
            (unsigned)g_u32SoftNoteLamp);
    return;
#endif
    szWire = "freestanding";

    /* Grep: net_l2: soft gate0 honesty ... denser (Soft!=product; G-AC-1) */
    kprintf("net_l2: soft gate0 honesty denser hybrid=%s wire=%s "
            "probe=%s handoff_gate=%u bridge=%s mmio_handoff=%s "
            "soft_r8169=not_tx_rx (Soft!=product; G-AC-1; "
            "Soft L2 bridge ON!=MMIO handoff; soft ready->net_tcp :22; "
            "hard-cap note lamp=%u)\n",
            szGate, szWire, szProbe, (unsigned)u32HandoffGate, szBridge,
            (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0)
                ? "pending_or_fault"
                : "off",
            (unsigned)g_u32SoftNoteLamp);
}

/*
 * Soft UDX ready identity residual: under freestanding rtl SKIP + backend=none,
 * ETH_UDX_READY arms net_l2_ready but init left g_aIp/g_aMac zero. Pin lab
 * IPv4 10.200.125.50 and lab soft demux MAC LAB_MAC_UDX=02:00:00:47:4a:50 so
 * net_eth ARP/ICMP match and replies enqueue for ETH_TX_PULL. Soft demux MAC
 * aligns with rtl8168_udx product idr lab_fallback (same 6 bytes; Soft!=product).
 * Not g_aVirtMac (QEMU 52:54:…) — virt residual would desync ARP SHA vs product
 * IDR lab LAA. OPEN: product idr keep=1 (EEPROM) still uses lab LAA here until
 * station MAC can publish into net_l2. Soft!=product Dual DoD B OPEN.
 * greppable: net_l2: soft udx ready
 * greppable: LAB_MAC_UDX=02:00:00:47:4a:50
 */
void
net_l2_udx_ready_identity(void)
{
    u32 i;
    int fMacZero;

    if (g_u32Backend != GJ_NET_L2_NONE) {
        return;
    }
    if (net_door_udx_ready() == 0) {
        return;
    }
    /* Lab IPv4 pin (never leave 0.0.0.0 / QEMU SLIRP under UDX ready). */
    memcpy(g_aIp, g_aRtlIp, 4);
    /*
     * Soft demux MAC residual: freestanding has no station under SKIP.
     * Fill zero only — lab LAA matches product IDR lab_fallback.
     * Soft!=product; Soft demux MAC aligns with rtl8168_udx product idr
     * lab_fallback Soft!=product.
     */
    fMacZero = 1;
    for (i = 0; i < 6u; i++) {
        if (g_aMac[i] != 0u) {
            fMacZero = 0;
            break;
        }
    }
    if (fMacZero != 0) {
        /* LAB_MAC_UDX=02:00:00:47:4a:50 (not g_aVirtMac). Soft!=product. */
        memcpy(g_aMac, g_aLabMacUdx, 6);
    }
    {
        static u8 s_fUdxIdLamp;

        if (s_fUdxIdLamp == 0u) {
            s_fUdxIdLamp = 1u;
            kprintf("net_l2: soft udx ready backend=none "
                    "owner=product_udx_abi path=rtl8168_udx "
                    "lab_ip=10.200.125.50 "
                    "mac=%02x:%02x:%02x:%02x:%02x:%02x "
                    "lab_mac_udx=1 "
                    "ip=%u.%u.%u.%u dual_dod_b=OPEN_UDX "
                    "freestanding_rtl=SKIP Soft!=product G-AC-1 "
                    "(identity pin; Soft demux MAC aligns with "
                    "rtl8168_udx product idr lab_fallback Soft!=product; "
                    "OPEN keep=1 EEPROM station not published to net_l2)\n",
                    g_aMac[0], g_aMac[1], g_aMac[2], g_aMac[3], g_aMac[4],
                    g_aMac[5], g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3]);
        }
    }
}

int
net_l2_ready(void)
{
    int fReady;

    /* Handoff pending / FAULT: freestanding not usable until soft open. */
    if (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0) {
        return 0;
    }
    fReady = 0;
    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        fReady = virtio_net_ready();
    } else if (g_u32Backend == GJ_NET_L2_RTL8168) {
        fReady = rtl8168_ready();
    } else if (g_u32Backend == GJ_NET_L2_NONE &&
               net_door_udx_ready() != 0) {
        /*
         * Product UDX L2 soft ready (rtl8168_udx park thr ETH_UDX_READY).
         * Freestanding rtl SKIP; Dual DoD B path=rtl8168_udx. Soft!=product.
         * Pin lab IP + soft demux MAC so ARP/ICMP demux has identity.
         * greppable: net_l2: soft udx ready
         */
        net_l2_udx_ready_identity();
        fReady = 1;
    }
    /*
     * Soft ready handoff residual (once): freestanding wire usable ->
     * net_tcp may mint soft listen :22 (Dual DoD B). Hard-cap / no flood.
     * Soft!=product. Grep: net_l2: soft ready handoff listen
     */
    if (fReady != 0) {
        net_l2_soft_ready_handoff_listen_once(fReady);
    }
    return fReady;
}

/*
 * Soft settle under TX OWN: pause in slices, re-poll_hw so reclaim can free
 * slots early (host arping+ping flood). Early-abort when busy tally still 0
 * (pre-flood). Soft!=product; never hard-gates demux.
 */
static void
net_l2_tx_settle(u32 u32Settle)
{
    u32 u32Spin;

    for (u32Spin = 0; u32Spin < u32Settle; u32Spin++) {
        __asm__ volatile("pause");
        if ((u32Spin % NET_L2_TX_SETTLE_SLICE) ==
            (NET_L2_TX_SETTLE_SLICE - 1u)) {
            rtl8168_poll_hw();
            /*
             * Pre-flood only: rtl8168_tx_busy() is a cumulative tally - once
             * non-zero we keep settling under pressure. Soft!=product.
             */
            if (rtl8168_tx_busy() == 0u) {
                return;
            }
        }
    }
}

/*
 * Soft hold14 bridge honesty (once). Soft L2 bridge is copy-only; freestanding
 * owns wire - R/T mirror tracks freestanding climb. Soft!=product * G-AC-1.
 * Grep: net_l2: soft bridge hold14 freestanding wire
 */
static void
net_l2_soft_bridge_hold14_honesty(void)
{
    if (g_fSoftBridgeHonestyNoted != 0) {
        return;
    }
    if (linux_netdev_soft_l2_bridge_enabled() == 0) {
        return;
    }
    g_fSoftBridgeHonestyNoted = 1;
    /* Grep: net_l2: soft bridge hold14 freestanding wire ... (Soft!=product) */
    kprintf("net_l2: soft bridge hold14 freestanding wire backend=%s "
            "(Soft!=product; L2 bridge copy-only != product wire; "
            "R/T mirror tracks freestanding; lab dual DoD B residual)\n",
            net_l2_name_local());
}

/*
 * Soft rate-limited residual TX lamps (never hard-gate demux). Soft!=product.
 * busy != fail is residual honesty under host arping+ping (lab dual DoD B):
 *   busy  = ring OWN full after L2 re-tries (retryable; SoftTxBusy++; not sticky)
 *   fail  = hard error (not ready / bad args / backend; SoftTxFail++)
 *   handoff = fail-closed during MMIO handoff (SoftTxHandoffClosed++; not busy)
 * Grep: net_l2: soft tx busy residual
 * Grep: net_l2: soft tx fail residual
 * Grep: net_l2: soft tx handoff residual
 */
static void
net_l2_soft_tx_lamp_busy(u32 u32Try, u32 u32RtlBusy, u32 u32RtlFail)
{
    if (g_u32SoftTxBusyLamp >= NET_L2_SOFT_TX_LAMP_CAP) {
        return;
    }
    g_u32SoftTxBusyLamp++;
    /* Grep: net_l2: soft tx busy residual ... (Soft!=product) */
    kprintf("net_l2: soft tx busy residual try=%u backend=%s "
            "ok=%u busy=%u fail=%u handoff_closed=%u retry_ok=%u pre=%u "
            "rtl_b=%u rtl_f=%u lamp=%u (Soft!=product; busy!=fail; not sticky; "
            "ring OWN)\n",
            (unsigned)u32Try, net_l2_name_local(),
            (unsigned)g_u32SoftTxOk, (unsigned)g_u32SoftTxBusy,
            (unsigned)g_u32SoftTxFail, (unsigned)g_u32SoftTxHandoffClosed,
            (unsigned)g_u32SoftTxBusyRetryOk, (unsigned)g_u32SoftTxBusyPre,
            (unsigned)u32RtlBusy, (unsigned)u32RtlFail,
            (unsigned)g_u32SoftTxBusyLamp);
}

static void
net_l2_soft_tx_lamp_fail(u32 u32Try, u32 u32RtlBusy, u32 u32RtlFail)
{
    if (g_u32SoftTxFailLamp >= NET_L2_SOFT_TX_LAMP_CAP) {
        return;
    }
    g_u32SoftTxFailLamp++;
    /* Grep: net_l2: soft tx fail residual ... (Soft!=product) */
    kprintf("net_l2: soft tx fail residual try=%u backend=%s "
            "ok=%u busy=%u fail=%u handoff_closed=%u retry_ok=%u "
            "rtl_b=%u rtl_f=%u lamp=%u (Soft!=product; hard fail; not busy)\n",
            (unsigned)u32Try, net_l2_name_local(),
            (unsigned)g_u32SoftTxOk, (unsigned)g_u32SoftTxBusy,
            (unsigned)g_u32SoftTxFail, (unsigned)g_u32SoftTxHandoffClosed,
            (unsigned)g_u32SoftTxBusyRetryOk, (unsigned)u32RtlBusy,
            (unsigned)u32RtlFail, (unsigned)g_u32SoftTxFailLamp);
}

static void
net_l2_soft_tx_lamp_handoff(void)
{
    if (g_u32SoftTxFailLamp >= NET_L2_SOFT_TX_LAMP_CAP) {
        return;
    }
    g_u32SoftTxFailLamp++;
    /* Grep: net_l2: soft tx handoff residual ... (Soft!=product) */
    kprintf("net_l2: soft tx handoff residual backend=%s "
            "ok=%u busy=%u fail=%u handoff_closed=%u "
            "(Soft!=product; fail-closed; not busy; no dual-drive)\n",
            net_l2_name_local(),
            (unsigned)g_u32SoftTxOk, (unsigned)g_u32SoftTxBusy,
            (unsigned)g_u32SoftTxFail, (unsigned)g_u32SoftTxHandoffClosed);
}

static const char *
net_l2_name_local(void)
{
    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        return "virtio";
    }
    if (g_u32Backend == GJ_NET_L2_RTL8168) {
        return "rtl8168";
    }
    return "none";
}

int
net_l2_tx(const void *pFrame, u32 cbLen)
{
    int nSt;
    u32 u32Try;
    u32 u32Fail0;
    u32 u32Busy1;
    u32 u32Fail1;
    int fBusyClass;

    /*
     * Handoff pending / FAULT: fail closed - never dual-drive freestanding.
     * Soft!=product residual honesty: handoff-closed != ring-busy (no re-try).
     */
    if (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0) {
        if (g_u32SoftTxHandoffClosed < 0xffffffffu) {
            g_u32SoftTxHandoffClosed++;
        }
        if (g_u32SoftTxFail < 0xffffffffu) {
            g_u32SoftTxFail++;
        }
        net_l2_soft_tx_lamp_handoff();
        return -1;
    }
    if (pFrame == NULL || cbLen < 14u) {
        if (g_u32SoftTxFail < 0xffffffffu) {
            g_u32SoftTxFail++;
        }
        return -1;
    }

    /*
     * Product UDX soft TX under freestanding rtl SKIP + backend=none:
     * enqueue demux reply for rtl8168_udx park thr (ETH_TX_PULL). Soft!=product.
     * greppable: net_l2: soft udx tx enqueue
     */
    if (g_u32Backend == GJ_NET_L2_NONE && net_door_udx_ready() != 0) {
        if (net_door_udx_tx_soft(pFrame, cbLen) == 0) {
            if (g_u32SoftTxOk < 0xffffffffu) {
                g_u32SoftTxOk++;
            }
            {
                static u8 s_fUdxTxLamp;

                if (s_fUdxTxLamp == 0u) {
                    s_fUdxTxLamp = 1u;
                    kprintf("net_l2: soft udx tx enqueue first len=%u "
                            "path=rtl8168_udx dual_dod_b=OPEN_UDX "
                            "Soft!=product G-AC-1\n",
                            (unsigned)cbLen);
                }
            }
            return 0;
        }
        if (g_u32SoftTxFail < 0xffffffffu) {
            g_u32SoftTxFail++;
        }
        return -1;
    }

    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        /*
         * Freestanding T0 product mux residual. Soft counters only - virtio
         * has no rtl OWN model. Soft r8169 never selected. Soft!=product.
         * Under freestanding rtl SKIP: laptop product NIC still UDX+ABI
         * (virtio is T0 only; Dual DoD B OPEN_UDX). Soft!=product * G-AC-1.
         */
        nSt = virtio_net_tx(pFrame, cbLen);
        if (nSt == 0) {
            if (g_u32SoftTxOk < 0xffffffffu) {
                g_u32SoftTxOk++;
            }
        } else if (g_u32SoftTxFail < 0xffffffffu) {
            g_u32SoftTxFail++;
        }
        return nSt;
    }

    if (g_u32Backend == GJ_NET_L2_RTL8168) {
        /*
         * Lab dual DoD B residual deepen: freestanding L2 TX under host
         * arping+ping/sshd eth. rtl8168_tx free-scans + gentle OWN wait
         * (busy != fail). L2 adds:
         *   - pre-TX settle when ring already OWN (prior reply under flood)
         *   - sliced poll_hw settle between re-tries (OWN free early)
         *   - busy re-try so single-try callers (TCP SYN-ACK / soft l2_xmit)
         *     still get replies out; hard fail (fail counter rose) stops now
         * Soft!=product. Soft r8169 never owns TX. Return -1 after busy
         * exhaust is not sticky.
         * Grep: net_l2: soft tx busy residual
         * Grep: net_l2: soft tx busy retry ok
         * Grep: net_l2: soft tx fail residual
         */
        if (g_fSoftTxResidualNoted == 0) {
            g_fSoftTxResidualNoted = 1;
            /* Grep: net_l2: soft tx residual freestanding ... (Soft!=product) */
            kprintf("net_l2: soft tx residual freestanding backend=rtl8168 "
                    "busy_tries=%u (Soft!=product; busy!=fail; not sticky; "
                    "arping/ping/TCP replies leave; soft_r8169=not_tx_rx)\n",
                    (unsigned)NET_L2_TX_BUSY_TRIES);
        }
        /* hold14 soft bridge honesty when bridge ON (copy-only != product). */
        net_l2_soft_bridge_hold14_honesty();

        /*
         * Reclaim OWN / IntrStatus before first try under ring pressure
         * (host arping flood may leave head OWN from prior reply). Soft!=product.
         */
        rtl8168_poll_hw();
        /*
         * Pre-TX busy residual: under continuous host arping+ping the ring is
         * often still OWN from the previous reply. Short settle+reclaim before
         * first try raises first-try hit rate. Soft!=product.
         */
        {
            u32 u32BusyPre;

            u32BusyPre = rtl8168_tx_busy();
            if (u32BusyPre != 0u) {
                if (g_u32SoftTxBusyPre < 0xffffffffu) {
                    g_u32SoftTxBusyPre++;
                }
                net_l2_tx_settle(NET_L2_TX_SETTLE_PRE +
                                 (u32BusyPre > 8u
                                      ? (NET_L2_TX_SETTLE_BUSY / 2u)
                                      : 0u));
                rtl8168_poll_hw();
            }
        }

        for (u32Try = 0; u32Try < NET_L2_TX_BUSY_TRIES; u32Try++) {
            if (u32Try != 0u) {
                u32 u32Settle;

                /*
                 * Kick TE|RE + IntrStatus; sliced settle so OWN can free
                 * between tries without burning a full poll quantum.
                 * Soft!=product.
                 */
                rtl8168_poll_hw();
                u32Settle = NET_L2_TX_SETTLE_BASE +
                            u32Try * NET_L2_TX_SETTLE_STEP +
                            NET_L2_TX_SETTLE_BUSY;
                net_l2_tx_settle(u32Settle);
                rtl8168_poll_hw();
            }

            u32Fail0 = rtl8168_tx_fail();
            nSt = rtl8168_tx(pFrame, cbLen);
            if (nSt == 0) {
                if (g_u32SoftTxOk < 0xffffffffu) {
                    g_u32SoftTxOk++;
                }
                if (u32Try != 0u) {
                    if (g_u32SoftTxBusyRetryOk < 0xffffffffu) {
                        g_u32SoftTxBusyRetryOk++;
                    }
                    /*
                     * First few busy->ok salvages only (arping flood safe).
                     * Grep: net_l2: soft tx busy retry ok ... (Soft!=product)
                     */
                    if (g_u32SoftTxBusyRetryOk <=
                        NET_L2_SOFT_TX_RETRY_OK_LAMP_CAP) {
                        kprintf("net_l2: soft tx busy retry ok try=%u "
                                "retry_ok=%u (Soft!=product; reply left; "
                                "busy!=fail; not sticky; hard-cap)\n",
                                (unsigned)u32Try,
                                (unsigned)g_u32SoftTxBusyRetryOk);
                    }
                }
                /*
                 * Mid-flood: free OWN ASAP so next same-batch who-has/ICMP
                 * reply sees a slot. Soft!=product.
                 */
                rtl8168_poll_hw();
                return 0;
            }

            /*
             * busy != fail residual honesty (lab dual DoD B under arping+ping):
             * hard fail only when rtl fail counter rose (not ready / bad args).
             * Otherwise treat as ring-busy / retryable - never sticky-fail
             * OWN-full as permanent SoftTxFail. Soft!=product.
             *   busy: SoftTxBusy path (continue / exhaust -> SoftTxBusy++)
             *   fail: SoftTxFail path (immediate -1)
             */
            u32Busy1 = rtl8168_tx_busy();
            u32Fail1 = rtl8168_tx_fail();
            fBusyClass = (u32Fail1 == u32Fail0) ? 1 : 0;
            if (fBusyClass != 0) {
                continue;
            }

            /* Hard fail from backend (not ready / bad args). Not ring-busy. */
            if (g_u32SoftTxFail < 0xffffffffu) {
                g_u32SoftTxFail++;
            }
            net_l2_soft_tx_lamp_fail(u32Try, u32Busy1, u32Fail1);
            return -1;
        }

        /*
         * Exhausted busy re-tries - still ring pressure.
         * Residual honesty: SoftTxBusy++ only (not SoftTxFail). Return -1
         * is transient under flood; next ARP/ICMP/TCP reply may leave.
         * No sticky flag, no demux poison. Final poll_hw so the next
         * same-batch who-has/ICMP is not stuck behind OWN. Soft!=product.
         */
        if (g_u32SoftTxBusy < 0xffffffffu) {
            g_u32SoftTxBusy++;
        }
        net_l2_soft_tx_lamp_busy(NET_L2_TX_BUSY_TRIES, rtl8168_tx_busy(),
                                 rtl8168_tx_fail());
        rtl8168_poll_hw();
        return -1;
    }

    /*
     * backend=none residual: under freestanding rtl SKIP default this is
     * product=UDX+ABI lean (no freestanding wire). Once-lamp only.
     * Soft!=product * G-AC-1. Grep: net_l2: soft residual skip tx/rx none
     */
    net_l2_soft_skip_tx_rx_none_residual_once("tx");
    if (g_u32SoftTxFail < 0xffffffffu) {
        g_u32SoftTxFail++;
    }
    return -1;
}

/*
 * Soft settle under RX empty: pause in denser slices, re-poll_hw so rtl
 * residual can free completions (R0/R1 lag under host arping). Soft!=product;
 * never hard-gates demux. Slice denser than TX (RX residual needs more
 * poll_hw per unit settle so multi-desc / hole re-OWN can land).
 */
static void
net_l2_rx_settle(u32 u32Settle)
{
    u32 u32Spin;

    for (u32Spin = 0; u32Spin < u32Settle; u32Spin++) {
        __asm__ volatile("pause");
        if ((u32Spin % NET_L2_RX_SETTLE_SLICE) ==
            (NET_L2_RX_SETTLE_SLICE - 1u)) {
            rtl8168_poll_hw();
        }
    }
}

/*
 * Soft MAC + lab IP identity honesty (once). Dual DoD B demux residual:
 * eth ARP/ICMP/TCP match station MAC + 10.200.125.50. Soft!=product * G-AC-1.
 * Grep: net_l2: soft mac identity honesty
 * Grep: net_l2: soft lab ip identity
 */
static void
net_l2_soft_mac_ip_identity_honesty_once(void)
{
    u8 aMacHw[6];
    u32 i;
    int fMacZero;
    int fIpOk;

    if (g_u32Backend != GJ_NET_L2_RTL8168) {
        return;
    }

    /*
     * Lab IP identity: rtl backend must stay 10.200.125.50 (never QEMU
     * 10.0.2.15). Re-assert if corrupted so demux matches host TPA/dst.
     * Soft!=product. Grep: net_l2: soft lab ip identity
     */
    fIpOk = 1;
    for (i = 0; i < 4u; i++) {
        if (g_aIp[i] != g_aRtlIp[i]) {
            fIpOk = 0;
            break;
        }
    }
    if (fIpOk == 0) {
        memcpy(g_aIp, g_aRtlIp, 4);
    }
    if (g_fSoftLabIpHonestyNoted == 0) {
        g_fSoftLabIpHonestyNoted = 1;
        /* Grep: net_l2: soft lab ip identity ... (Soft!=product) */
        kprintf("net_l2: soft lab ip identity %u.%u.%u.%u backend=rtl8168 "
                "fixed=%u (Soft!=product; dual DoD B; demux ARP/ICMP/TCP; "
                "never QEMU 10.0.2.15)\n",
                g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3],
                (fIpOk == 0) ? 1u : 0u);
    }

    /*
     * MAC identity honesty: pull station MAC from freestanding rtl when
     * mux cache is zero or diverged after reclaim. Soft!=product.
     * Grep: net_l2: soft mac identity honesty
     */
    fMacZero = 1;
    for (i = 0; i < 6u; i++) {
        if (g_aMac[i] != 0u) {
            fMacZero = 0;
            break;
        }
    }
    rtl8168_mac(aMacHw);
    if (fMacZero != 0) {
        memcpy(g_aMac, aMacHw, 6);
    } else {
        for (i = 0; i < 6u; i++) {
            if (g_aMac[i] != aMacHw[i]) {
                memcpy(g_aMac, aMacHw, 6);
                break;
            }
        }
    }
    if (g_fSoftMacIdHonestyNoted == 0) {
        g_fSoftMacIdHonestyNoted = 1;
        /* Grep: net_l2: soft mac identity honesty ... (Soft!=product) */
        kprintf("net_l2: soft mac identity honesty "
                "mac=%02x:%02x:%02x:%02x:%02x:%02x "
                "ip=%u.%u.%u.%u backend=rtl8168 "
                "(Soft!=product; dual DoD B; station honesty; demux match)\n",
                g_aMac[0], g_aMac[1], g_aMac[2], g_aMac[3], g_aMac[4],
                g_aMac[5], g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3]);
    }
}

/*
 * Soft bridge RX lean tally (no feed_rx - eth owns copy path). Soft!=product.
 * hold14 refresh so freestanding R climb paints L2 BR; hard-cap lamps.
 * Grep: net_l2: soft bridge rx tally lean
 * Grep: net_l2: soft rx demux eth/tcp
 */
static void
net_l2_soft_rx_bridge_lean(u32 u32Len)
{
    /* hold14 freestanding R/T mirror companion - lean, skip-if-unchanged. */
    if (linux_netdev_soft_l2_bridge_enabled() != 0) {
        linux_netdev_soft_hold14_refresh();
        net_l2_soft_bridge_hold14_honesty();
        if (g_u32SoftRxBridgeLamp < NET_L2_SOFT_RX_BRIDGE_LAMP_CAP) {
            g_u32SoftRxBridgeLamp++;
            /* Grep: net_l2: soft bridge rx tally lean ... (Soft!=product) */
            kprintf("net_l2: soft bridge rx tally lean ok=%u empty=%u "
                    "len=%u hold14=refresh bridge=copy_only "
                    "(Soft!=product; dual DoD B; no feed double-count; "
                    "no stamp flood; lamp=%u)\n",
                    (unsigned)g_u32SoftRxOk, (unsigned)g_u32SoftRxEmpty,
                    (unsigned)u32Len, (unsigned)g_u32SoftRxBridgeLamp);
        }
    }

    if (g_fSoftRxDemuxNoted == 0) {
        g_fSoftRxDemuxNoted = 1;
        /* Grep: net_l2: soft rx demux eth/tcp ... (Soft!=product) */
        kprintf("net_l2: soft rx demux eth/tcp backend=%s "
                "ok=%u len=%u ip=%u.%u.%u.%u "
                "(Soft!=product; dual DoD B; freestanding frame -> "
                "net_eth ARP/ICMP + net_tcp :22; Soft L2 bridge "
                "copy-only != product wire)\n",
                net_l2_name_local(), (unsigned)g_u32SoftRxOk,
                (unsigned)u32Len, g_aIp[0], g_aIp[1], g_aIp[2], g_aIp[3]);
    }
}

i32
net_l2_rx(void *pOut, u32 cbMax)
{
    i32 i32N;
    u32 u32Try;
    u32 u32Settle;

    if (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0) {
        if (g_u32SoftRxHandoffClosed < 0xffffffffu) {
            g_u32SoftRxHandoffClosed++;
        }
        return -1;
    }
    if (pOut == NULL || cbMax < 14u) {
        return -1;
    }

    if (g_u32Backend == GJ_NET_L2_VIRTIO) {
        /*
         * Freestanding T0 product RX; soft r8169 never owns RX.
         * Lean SoftRx tallies only - no rtl OWN multi-try. Soft!=product.
         */
        i32N = virtio_net_rx(pOut, cbMax);
        if (i32N >= 14) {
            if (g_u32SoftRxOk < 0xffffffffu) {
                g_u32SoftRxOk++;
            }
            net_l2_soft_rx_bridge_lean((u32)i32N);
        } else if (g_u32SoftRxEmpty < 0xffffffffu) {
            g_u32SoftRxEmpty++;
        }
        return i32N;
    }

    if (g_u32Backend == GJ_NET_L2_RTL8168) {
        /*
         * Lab dual DoD B residual: freestanding rtl RX -> eth/tcp demux.
         * Soft!=product * G-AC-1 * soft r8169 never owns RX.
         *
         * DUT class L2 BR RX=0 TX>0 freestanding UP / host arping 0 replies
         * under hybrid SOFT gate0: strengthen multi-try poll_hw reclaim so
         * completions leave ring when rtl delivers (R0/R1 lag). Caller
         * net_eth demuxes ARP/ICMP + feeds soft bridge copy. Soft bridge
         * tallies lean (SoftRxOk + hold14; eth owns feed_rx - no double).
         * MAC + lab IP identity honesty before first reclaim.
         *   1) pre-RX settle+poll (IntrStatus / residual tick headroom)
         *   2) empty re-try with denser settle slice (more poll_hw/unit)
         *   3) final poll after empty (next eth drain can hit; not sticky)
         * Grep: net_l2: soft rx residual freestanding
         * Grep: net_l2: soft bridge rx tally lean
         * Grep: net_l2: soft mac identity honesty
         */
        net_l2_soft_mac_ip_identity_honesty_once();
        /* Init once-lamp may race late probe - keep greppable residual. */
        if (g_fSoftRxResidualNoted == 0) {
            g_fSoftRxResidualNoted = 1;
            /* Grep: net_l2: soft rx residual freestanding ... (Soft!=product) */
            kprintf("net_l2: soft rx residual freestanding backend=rtl8168 "
                    "empty_tries=%u ip=%u.%u.%u.%u "
                    "(Soft!=product; dual DoD B; demux->eth/tcp; "
                    "soft bridge tallies lean; soft_r8169=not_tx_rx)\n",
                    (unsigned)NET_L2_RX_EMPTY_TRIES, g_aIp[0], g_aIp[1],
                    g_aIp[2], g_aIp[3]);
        }

        /*
         * Pre-RX reclaim: TE|RE + IntrStatus + rtl residual tick, short
         * denser settle, re-poll so first try sees completions that were
         * mid-flight after prior TX reply / empty poll. Soft!=product.
         */
        rtl8168_poll_hw();
        net_l2_rx_settle(NET_L2_RX_SETTLE_PRE);
        rtl8168_poll_hw();

        for (u32Try = 0; u32Try < NET_L2_RX_EMPTY_TRIES; u32Try++) {
            /*
             * TE|RE + IntrStatus + rtl soft RX residual (fill/multi-desc).
             * Soft!=product; no stamp here.
             */
            if (u32Try != 0u) {
                rtl8168_poll_hw();
            }
            i32N = rtl8168_rx(pOut, cbMax);
            if (i32N >= 14) {
                if (g_u32SoftRxOk < 0xffffffffu) {
                    g_u32SoftRxOk++;
                }
                if (u32Try != 0u) {
                    if (g_u32SoftRxRetryOk < 0xffffffffu) {
                        g_u32SoftRxRetryOk++;
                    }
                    /*
                     * First few empty->ok salvages only (no flood).
                     * Grep: net_l2: soft rx residual freestanding
                     */
                    if (g_u32SoftRxRetryOk <=
                        NET_L2_SOFT_RX_RETRY_OK_LAMP_CAP) {
                        kprintf("net_l2: soft rx residual freestanding "
                                "retry ok try=%u retry_ok=%u len=%u "
                                "(Soft!=product; frame left ring; "
                                "demux->eth/tcp; hard-cap)\n",
                                (unsigned)u32Try,
                                (unsigned)g_u32SoftRxRetryOk,
                                (unsigned)i32N);
                    }
                }
                /*
                 * Mid-batch: re-arm residual so next same-batch who-has
                 * is not stuck behind OWN hole. Soft!=product.
                 */
                rtl8168_poll_hw();
                /* Lean soft bridge tally + hold14; eth still feed_rx. */
                net_l2_soft_rx_bridge_lean((u32)i32N);
                return i32N;
            }

            /*
             * Empty: denser settle + re-poll so same-batch completions under
             * R0/R1 lag can land without starving net_eth drain. Soft!=product.
             */
            if (u32Try + 1u < NET_L2_RX_EMPTY_TRIES) {
                u32Settle = NET_L2_RX_SETTLE +
                            u32Try * NET_L2_RX_SETTLE_STEP;
                net_l2_rx_settle(u32Settle);
            }
        }

        if (g_u32SoftRxEmpty < 0xffffffffu) {
            g_u32SoftRxEmpty++;
        }
        /*
         * Empty residual lamp hard-cap only (poll path must not flood serial).
         * Soft!=product. Grep: net_l2: soft rx residual freestanding
         */
        if (g_u32SoftRxLamp < NET_L2_SOFT_RX_LAMP_CAP) {
            g_u32SoftRxLamp++;
            kprintf("net_l2: soft rx residual freestanding empty "
                    "ok=%u empty=%u handoff_closed=%u retry_ok=%u "
                    "rtl_r=%u rtl_drop=%u lamp=%u "
                    "(Soft!=product; dual DoD B; reclaim not sticky; "
                    "no stamp flood)\n",
                    (unsigned)g_u32SoftRxOk, (unsigned)g_u32SoftRxEmpty,
                    (unsigned)g_u32SoftRxHandoffClosed,
                    (unsigned)g_u32SoftRxRetryOk,
                    (unsigned)rtl8168_rx_count(),
                    (unsigned)rtl8168_rx_drop(),
                    (unsigned)g_u32SoftRxLamp);
        }
        /*
         * Bridge ON + empty: still lean-refresh hold14 so freestanding T
         * climb stays painted when R lags (DUT TX>0 RX=0 class). Soft!=product.
         */
        if (linux_netdev_soft_l2_bridge_enabled() != 0) {
            linux_netdev_soft_hold14_refresh();
        }
        /*
         * Final poll after empty exhaust: residual tick + TE|RE so the next
         * net_eth drain is not stuck behind mid-flight OWN. Empty is not
         * sticky. Soft!=product.
         */
        rtl8168_poll_hw();
        return -1;
    }

    /*
     * backend=none residual: freestanding rtl SKIP default -> product=UDX+ABI
     * lean (no freestanding RX thrash). Once-lamp only. Soft!=product * G-AC-1.
     * Grep: net_l2: soft residual skip tx/rx none
     */
    net_l2_soft_skip_tx_rx_none_residual_once("rx");
    return -1;
}

void
net_l2_mac(u8 *pMac)
{
    u32 i;

    if (pMac == NULL) {
        return;
    }
    for (i = 0; i < 6u; i++) {
        pMac[i] = g_aMac[i];
    }
}

void
net_l2_ip(u8 *pIp)
{
    u32 i;

    if (pIp == NULL) {
        return;
    }
    for (i = 0; i < 4u; i++) {
        pIp[i] = g_aIp[i];
    }
}

const char *
net_l2_name(void)
{
    return net_l2_name_local();
}

/*
 * Soft Linux netdev note - coexistence lamp only. Soft!=product * G-AC-1.
 * Call after r8169 / linux_netdev_soft register path (main soft module wave).
 * Does not change g_u32Backend or net_l2_tx/rx routing (freestanding stays).
 * Does not dual-drive BAR/rings (no soft TE|RE / ring program from this path).
 * Soft r8169 is never product TX/RX owner (lab wire honesty).
 * L2 bridge is copy-only (hybrid 4a); refuse if handoff pending/FAULT.
 * Under freestanding rtl SKIP (GJ_RTL8168_PROBE=0): denser product=UDX+ABI
 * soft residual lean only - no freestanding rtl L2 bridge dual DoD B
 * residual; no freestanding RX/TX thrash. Virtio T0 still selected when
 * present. Hard-cap residual lines (NET_L2_SOFT_NOTE_LAMP_CAP) - no
 * inventory flood. Soft!=product once-lamps / hard-cap only.
 * Grep: net_l2: soft linux netdev note n=N (Soft!=product)
 * Grep: net_l2: soft linux note no dual-drive (Soft!=product)
 * Grep: net_l2: lab wire honesty ... (Soft!=product; G-AC-1)
 * Grep: net_l2: soft residual freestanding rtl SKIP
 * Grep: net_l2: soft residual product=UDX+ABI
 * Grep: net_l2: soft residual lean
 * Grep: net_l2: soft residual product_udx_abi honesty
 * Grep: path=userspace_udx_hot_cold_abi
 * Grep: role=skip_rtl_product_udx_abi
 * Grep: owner=product_udx_abi
 * Grep: need=UDX_OPEN | freestanding_rtl=SKIP
 * Grep: net_l2: soft gate0 honesty ... (Soft!=product; G-AC-1)
 * Grep: net_l2: soft ready handoff listen ... (feeds net_tcp :22)
 */
void
net_l2_soft_linux_note(void)
{
    int nSoft;
    void *pPrimary;
    int nMode;
    int fReady;

    if (g_fSoftLinuxNoted != 0) {
        return;
    }

    /* Soft surface may not be init yet - treat as absent (no lamp). */
    if (linux_netdev_soft_ready() == 0) {
        return;
    }

    nSoft = linux_netdev_soft_count();
    pPrimary = linux_netdev_soft_primary();
    if (nSoft < 1 || pPrimary == NULL) {
        return;
    }

    g_fSoftLinuxNoted = 1;

    /*
     * Compact once residual (hard-cap). Keep greppable markers; avoid
     * multi-line inventory stamp storm on soft present. Soft!=product.
     * Under freestanding rtl SKIP + backend=none: do not claim
     * tx=freestanding (honesty residual - product_udx_abi owns direction).
     */
    if (g_u32SoftNoteLamp < NET_L2_SOFT_NOTE_LAMP_CAP) {
        g_u32SoftNoteLamp++;
        /* Grep: net_l2: soft linux netdev note n=N (Soft!=product) */
#if !GJ_RTL8168_PROBE
        if (g_u32Backend == GJ_NET_L2_NONE) {
            kprintf("net_l2: soft linux netdev note n=%d backend=none "
                    "tx=product_udx_abi soft_r8169=not_tx_rx "
                    "owner=product_udx_abi role=skip_rtl_product_udx_abi "
                    "product=UDX+ABI path=userspace_udx_hot_cold_abi "
                    "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
                    "freestanding_rtl=SKIP freestanding_close=0 C0_SKIP=1 "
                    "(Soft!=product; freestanding rtl SKIP; "
                    "inventory/control only; no dual-drive; "
                    "not freestanding class thrash; not Dual DoD B close)\n",
                    nSoft);
        } else {
            /*
             * Virtio T0 under freestanding rtl SKIP: tx=freestanding (virtio)
             * but laptop product NIC = UDX+ABI residual. Soft!=product.
             * Grep: freestanding_rtl=SKIP | C0_SKIP=1 | dual_dod_b=OPEN_UDX
             */
            kprintf("net_l2: soft linux netdev note n=%d backend=%s "
                    "tx=freestanding soft_r8169=not_tx_rx "
                    "freestanding_rtl=SKIP product=UDX+ABI "
                    "path=userspace_udx_hot_cold_abi path=rtl8168_udx "
                    "dual_dod_b=OPEN_UDX need=UDX_OPEN "
                    "freestanding_close=0 C0_SKIP=1 "
                    "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
                    "virtio T0 wire when present; inventory/control only; "
                    "no dual-drive; not Dual DoD B close)\n",
                    nSoft, net_l2_name());
        }
#else
        {
            kprintf("net_l2: soft linux netdev note n=%d backend=%s "
                    "tx=freestanding soft_r8169=not_tx_rx "
                    "(Soft!=product; inventory/control only; no dual-drive)\n",
                    nSoft, net_l2_name());
        }
#endif
    }
    if (g_u32SoftNoteLamp < NET_L2_SOFT_NOTE_LAMP_CAP) {
        g_u32SoftNoteLamp++;
        /*
         * Explicit dual-drive refusal: soft note never programs CHIPCMD/rings.
         * When a freestanding backend is selected, wire owner stays
         * freestanding; under SKIP + backend=none product=UDX+ABI lean.
         * Soft r8169 is not TX/RX product. Soft!=product * G-AC-1.
         * Grep: net_l2: soft linux note no dual-drive
         */
#if !GJ_RTL8168_PROBE
        if (g_u32Backend == GJ_NET_L2_NONE) {
            kprintf("net_l2: soft linux note no dual-drive backend=none "
                    "owner=product_udx_abi soft_r8169=not_tx_rx "
                    "product=UDX+ABI path=userspace_udx_hot_cold_abi "
                    "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
                    "freestanding_rtl=SKIP freestanding_close=0 C0_SKIP=1 "
                    "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
                    "not freestanding class thrash; not Dual DoD B close; "
                    "not re-enable freestanding rtl)\n");
        } else {
            kprintf("net_l2: soft linux note no dual-drive backend=%s "
                    "tx=freestanding soft_r8169=not_tx_rx "
                    "freestanding_rtl=SKIP product=UDX+ABI "
                    "path=userspace_udx_hot_cold_abi path=rtl8168_udx "
                    "dual_dod_b=OPEN_UDX need=UDX_OPEN "
                    "freestanding_close=0 C0_SKIP=1 "
                    "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
                    "virtio T0 wire when present; no dual-drive; "
                    "not Dual DoD B close)\n",
                    net_l2_name());
        }
#else
        {
            kprintf("net_l2: soft linux note no dual-drive backend=%s "
                    "tx=freestanding soft_r8169=not_tx_rx "
                    "(Soft!=product; G-AC-1)\n",
                    net_l2_name());
        }
#endif
    }
    if (g_u32SoftNoteLamp < NET_L2_SOFT_NOTE_LAMP_CAP) {
        g_u32SoftNoteLamp++;
        /*
         * Lab wire honesty with soft present: freestanding owns wire when a
         * freestanding backend is selected. Under freestanding rtl SKIP +
         * backend=none: product=UDX+ABI lean (not freestanding wire claim).
         * Soft netdev = inventory/control; never product datapath here.
         * Grep: net_l2: lab wire honesty
         * Grep: role=skip_rtl_product_udx_abi
         * Grep: owner=product_udx_abi need=UDX_OPEN
         */
#if !GJ_RTL8168_PROBE
        if (g_u32Backend == GJ_NET_L2_NONE) {
            kprintf("net_l2: lab wire honesty soft_present=%d "
                    "owner=product_udx_abi backend=none "
                    "role=skip_rtl_product_udx_abi "
                    "soft_r8169=not_tx_rx product=UDX+ABI "
                    "path=userspace_udx_hot_cold_abi path=rtl8168_udx "
                    "dual_dod_b=OPEN_UDX need=UDX_OPEN freestanding_rtl=SKIP "
                    "freestanding_close=0 C0_SKIP=1 "
                    "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
                    "mux residual; no dual-drive; not freestanding class thrash; "
                    "not Dual DoD B close; not re-enable freestanding rtl)\n",
                    nSoft);
        } else {
            /*
             * Virtio T0 wire under freestanding rtl SKIP: owner stays
             * freestanding (virtio) but freestanding_rtl=SKIP residual
             * still holds (laptop product NIC = UDX+ABI). Soft!=product.
             * Grep: freestanding_rtl=SKIP | need=UDX_OPEN | C0_SKIP=1
             */
            kprintf("net_l2: lab wire honesty soft_present=%d "
                    "owner=freestanding backend=%s soft_r8169=not_tx_rx "
                    "freestanding_rtl=SKIP product=UDX+ABI "
                    "path=userspace_udx_hot_cold_abi path=rtl8168_udx "
                    "dual_dod_b=OPEN_UDX need=UDX_OPEN "
                    "freestanding_close=0 C0_SKIP=1 "
                    "(Soft!=product; G-AC-1; freestanding rtl SKIP; "
                    "virtio T0 wire when present; mux residual; "
                    "no dual-drive; not freestanding class thrash; "
                    "not Dual DoD B close)\n",
                    nSoft, net_l2_name());
        }
#else
        {
            kprintf("net_l2: lab wire honesty soft_present=%d "
                    "owner=freestanding backend=%s soft_r8169=not_tx_rx "
                    "(Soft!=product; G-AC-1; mux residual; no dual-drive)\n",
                    nSoft, net_l2_name());
        }
#endif
    }
    /* Init-time honesty may have run before soft present - re-arm is once. */
    g_fLabWireHonestyNoted = 1;

    nMode = linux_pci_soft_last_probe_mode();

#if !GJ_RTL8168_PROBE
    /*
     * Freestanding rtl SKIP residual denser (soft note path): product
     * direction = UDX+ABI (path=userspace_udx_hot_cold_abi). Soft residual
     * lean only - do not enable freestanding rtl L2 bridge dual DoD B
     * residual under SKIP; do not thrash freestanding RX/TX. Virtio T0
     * still selected when present (backend may be virtio).
     * Soft!=product * G-AC-1 * once / hard-cap.
     * Grep: net_l2: soft residual freestanding rtl SKIP
     * Grep: net_l2: soft residual product=UDX+ABI
     * Grep: path=userspace_udx_hot_cold_abi
     * Grep: owner=product_udx_abi need=UDX_OPEN freestanding_rtl=SKIP
     */
    if (g_u32SoftNoteLamp < NET_L2_SOFT_NOTE_LAMP_CAP) {
        g_u32SoftNoteLamp++;
        kprintf("net_l2: soft residual freestanding rtl SKIP "
                "backend=%s soft_present=%d rtl_probe=0 product=UDX+ABI "
                "path=userspace_udx_hot_cold_abi path=rtl8168_udx "
                "owner=product_udx_abi role=skip_rtl_product_udx_abi "
                "dual_dod_b=OPEN_UDX need=UDX_OPEN freestanding_rtl=SKIP "
                "freestanding_close=0 C0_SKIP=1 stamp_storm=0 "
                "(Soft!=product; G-AC-1; lean residual only; "
                "virtio T0 when present; not freestanding class thrash; "
                "not Dual DoD B close; not re-enable freestanding rtl)\n",
                net_l2_name(), nSoft);
    }
    if (g_u32SoftNoteLamp < NET_L2_SOFT_NOTE_LAMP_CAP) {
        g_u32SoftNoteLamp++;
        kprintf("net_l2: soft residual product=UDX+ABI "
                "backend=%s soft_present=%d rtl_probe=0 "
                "owner=product_udx_abi role=skip_rtl_product_udx_abi "
                "path=userspace_udx_hot_cold_abi path=rtl8168_udx "
                "direction=userspace_nic_host_over_hot_cold_abi "
                "not=in_kernel_r8169_ko not=freestanding_rtl_as_product "
                "dual_dod_b=OPEN_UDX need=UDX_OPEN freestanding_rtl=SKIP "
                "soft=1 product=0 freestanding_close=0 C0_SKIP=1 "
                "stamp_storm=0 version_stamp=0 thr_only_eth=1 "
                "t0_product_net=virtio-net "
                "(Soft!=product; G-AC-1; freestanding rtl SKIP default; "
                "not re-enable freestanding rtl; lean residual only; "
                "not Dual DoD B close)\n",
                net_l2_name(), nSoft);
    }
    /* Denser SKIP + product_udx_abi residual honesty (once; own flag). */
    net_l2_soft_skip_product_udx_abi_residual_once();
    if (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0) {
        if (g_u32SoftNoteLamp < NET_L2_SOFT_NOTE_LAMP_CAP) {
            g_u32SoftNoteLamp++;
            kprintf("net_l2: soft linux L2 bridge SKIP handoff=%s "
                    "(Soft!=product; no dual-drive; ready handoff closed; "
                    "Soft L2 bridge!=MMIO handoff; product=UDX+ABI)\n",
                    (g_fSoftHandoffFault != 0) ? "FAULT" : "pending");
        }
    }
    /* No freestanding rtl L2 bridge residual under SKIP (lean only). */
    net_l2_soft_gate0_honesty_denser(nMode);
    fReady = net_l2_ready();
    (void)fReady;
    return;
#else
    /*
     * Soft L2 bridge when freestanding rtl owns wire + soft netdev present.
     * Gate0 hybrid: EMU soft bind (REAL .ko probe skipped so BAR stays
     * freestanding). Copy-only - not dual-drive. Soft!=product.
     * Skip if MMIO handoff pending/FAULT (fail closed; no soft wire yet).
     * Opt-in freestanding rtl residual only (GJ_RTL8168_PROBE=1).
     */
    if (g_fSoftHandoffPending != 0 || g_fSoftHandoffFault != 0) {
        if (g_u32SoftNoteLamp < NET_L2_SOFT_NOTE_LAMP_CAP) {
            g_u32SoftNoteLamp++;
            kprintf("net_l2: soft linux L2 bridge SKIP handoff=%s "
                    "(Soft!=product; no dual-drive; ready handoff closed; "
                    "Soft L2 bridge!=MMIO handoff)\n",
                    (g_fSoftHandoffFault != 0) ? "FAULT" : "pending");
        }
        /*
         * gate0 honesty denser even when bridge skipped (handoff closed).
         * Soft!=product * Soft L2 bridge != MMIO handoff.
         */
        net_l2_soft_gate0_honesty_denser(nMode);
        return;
    }
    if (rtl8168_ready() != 0 && g_u32Backend == GJ_NET_L2_RTL8168) {
        linux_netdev_soft_l2_bridge_enable(1);
        if (g_u32SoftNoteLamp < NET_L2_SOFT_NOTE_LAMP_CAP) {
            g_u32SoftNoteLamp++;
            kprintf("net_l2: soft linux datapath bridge ON mode=%s "
                    "backend=%s (Soft!=product; freestanding L2+soft "
                    "netdev; copy-only no dual-drive; "
                    "soft_r8169=not_tx_rx; Soft L2 bridge ON!=MMIO "
                    "handoff)\n",
                    (nMode == 1) ? "REAL" : "SOFT", net_l2_name());
        }
        /* hold14 R mirror honesty: bridge ON != freestanding product wire. */
        net_l2_soft_bridge_hold14_honesty();
    } else if (nMode == 1 /* REAL */) {
        linux_netdev_soft_l2_bridge_enable(1);
        if (g_u32SoftNoteLamp < NET_L2_SOFT_NOTE_LAMP_CAP) {
            g_u32SoftNoteLamp++;
            kprintf("net_l2: soft linux datapath bridge ON mode=REAL "
                    "backend=%s (Soft!=product; freestanding L2+soft "
                    "netdev; copy-only no dual-drive; "
                    "soft_r8169=not_tx_rx; Soft L2 bridge ON!=MMIO "
                    "handoff)\n",
                    net_l2_name());
        }
        net_l2_soft_bridge_hold14_honesty();
    }

    /*
     * Hybrid SOFT gate0 honesty denser after bridge decision (live probe +
     * bridge state). Init may have provisional gate0 (bridge=off); denser
     * under NOTE hard-cap. Soft L2 bridge ON != MMIO handoff. Soft!=product.
     * Grep: net_l2: soft gate0 honesty
     */
    net_l2_soft_gate0_honesty_denser(nMode);

    /*
     * Soft ready handoff residual: if freestanding already usable, emit
     * once greppable handoff for net_tcp soft listen :22 (Dual DoD B).
     * Soft L2 bridge is not a ready substitute. Soft!=product.
     * Grep: net_l2: soft ready handoff listen
     */
    fReady = net_l2_ready();
    (void)fReady;
#endif /* GJ_RTL8168_PROBE */
}

/*
 * Soft MMIO handoff pending (phase 1). Called from rtl8168_soft_handoff_prepare
 * after freestanding TE/RE stop. Soft!=product. Grep: net_l2: soft mmio handoff
 */
void
net_l2_soft_handoff_mark_pending(void)
{
    if (g_fSoftHandoffFault != 0) {
        return;
    }
    g_fSoftHandoffPending = 1;
    /* Grep: net_l2: soft mmio handoff pending */
    kprintf("net_l2: soft mmio handoff pending backend was=%s "
            "tx=fail-closed (Soft!=product; no soft open yet)\n",
            net_l2_name());
}

int
net_l2_soft_handoff_pending(void)
{
    return g_fSoftHandoffPending != 0 ? 1 : 0;
}

int
net_l2_soft_handoff_fault(void)
{
    return g_fSoftHandoffFault != 0 ? 1 : 0;
}

void
net_l2_soft_handoff_set_fault(const char *szWhy)
{
    g_fSoftHandoffFault = 1;
    g_fSoftHandoffPending = 1; /* fail closed TX/RX */
    if (g_fSoftHandoffFaultLogged == 0) {
        g_fSoftHandoffFaultLogged = 1;
        /* Grep: net_l2: soft mmio handoff FAULT */
        kprintf("net_l2: soft mmio handoff FAULT why=%s (Soft!=product; "
                "fail closed)\n",
                (szWhy != NULL) ? szWhy : "?");
    }
}
