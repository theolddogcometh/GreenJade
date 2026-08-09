/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room RTL8111/8168 (10ec:8168) freestanding residual — LEAN SKIP.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL paste.
 *
 * *** C0 FREESTANDING SKIP RESIDUAL HONESTY · NOT PRODUCT Dual DoD B ***
 * GJ_RTL8168_PROBE stays 0: freestanding NIC class SKIP (same policy as
 * freestanding USB MSC). Product laptop NIC = userspace rtl8168_udx +
 * hot+cold ABI + DDI/UDX caps. Dual DoD B = OPEN_UDX / need=UDX_OPEN
 * (UDX NIC wire / stack / sshd) — freestanding residual never closes
 * Dual DoD B. T0 product net = virtio-net until UDX owns wire.
 * Soft!=product · G-AC-1 (no in-kernel r8169.ko product wire).
 * H1 thr-only eth residual; H2 no stamp storms (once-shot lamps only).
 * Stamp-free residual: residual never carries GJ_IMAGE_VERSION / no invent
 * image stamp (fly bar lives in config only; residual lamps version_stamp=0).
 *
 * STOP freestanding rtl rabbit hole. NEVER re-enable freestanding NIC as
 * default / product. NEVER thrash R-climb as product engineering. Dead
 * thrash (OWN climb / gentle rearm / multi-desc residual) stripped safely —
 * public API retained as no-op / fail stubs for linkers (net_l2, main,
 * soft PCI). Opt-in -DGJ_RTL8168_PROBE=1 does not restore BAR/rings.
 *
 * Align: xhci C0 freestanding SKIP residual · net_l2 product_udx_abi honesty
 * · Dual DoD B OPEN_UDX need=UDX_OPEN (not freestanding close).
 *
 * greppable: rtl8168: probe SKIP | GJ_RTL8168_PROBE=0 | freestanding_rtl=SKIP
 * greppable: rtl8168: freestanding rtl SKIP | claim=0 | need=UDX_OPEN
 * greppable: rtl8168: soft residual product=UDX+ABI
 * greppable: rtl8168: soft residual product_udx_abi honesty
 * greppable: rtl8168: soft residual lean
 * greppable: dual_dod_b=OPEN_UDX | thr_only_eth=1 | Soft!=product | G-AC-1
 * greppable: C0 freestanding SKIP residual | role=skip_rtl_product_udx_abi
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/rtl8168.h>
#include <gj/types.h>

/* Once-shot honesty lamps (H2 no stamp storms). Soft!=product · G-AC-1. */
static u8 g_fProbeOnce;
static u8 g_fHandoffSkipOnce;
static u8 g_fKickSkipOnce;
static u8 g_fPostTeSkipOnce;
static u8 g_fReclaimSkipOnce;

int
rtl8168_probe(void)
{
    u32 u32Probe;
    u32 u32SkipDef;
    const char *szFsRtl;

    /*
     * C0 freestanding NIC SKIP residual honesty only (NOT product Dual DoD B).
     * product=UDX+ABI · G-AC-1. GJ_RTL8168_PROBE stays 0: do not claim
     * BAR/rings; lean residual honesty only. Dual DoD B = OPEN_UDX /
     * need=UDX_OPEN (not freestanding close). H1 thr-only eth residual;
     * H2 no stamp storms. NEVER re-enable freestanding NIC default /
     * product. NEVER thrash freestanding R-climb. Soft!=product.
     * Grep: rtl8168: probe SKIP | GJ_RTL8168_PROBE=0 | freestanding_rtl=SKIP
     * Grep: rtl8168: freestanding rtl SKIP | claim=0 | need=UDX_OPEN
     * Grep: rtl8168: soft residual product=UDX+ABI
     * Grep: rtl8168: soft residual product_udx_abi honesty
     * Grep: rtl8168: soft residual lean
     * Grep: dual_dod_b=OPEN_UDX | thr_only_eth=1 | C0 freestanding SKIP residual
     */
    if (g_fProbeOnce != 0) {
        return 0;
    }
    g_fProbeOnce = 1;

#if GJ_RTL8168_PROBE
    u32Probe = 1u;
    u32SkipDef = 0u;
    szFsRtl = "OPTIN_RESIDUAL";
#else
    u32Probe = 0u;
    u32SkipDef = 1u;
    szFsRtl = "SKIP";
#endif

#if !GJ_RTL8168_PROBE
    /* Grep: rtl8168: probe SKIP GJ_RTL8168_PROBE=0 | freestanding_rtl=SKIP */
    kprintf("rtl8168: probe SKIP GJ_RTL8168_PROBE=0 "
            "freestanding_rtl=SKIP claim=0 "
            "(C0 freestanding rtl SKIP residual; product=UDX+ABI; "
            "path=rtl8168_udx; dual_dod_b=OPEN_UDX need=UDX_OPEN; "
            "thr_only_eth=1; Soft!=product; G-AC-1; "
            "not in-kernel .ko wire; not Dual DoD B close; "
            "virtio T0 when present; no R-climb thrash; "
            "stamp_storm=0 version_stamp=0)\n");
    /* Grep: rtl8168: freestanding rtl SKIP GJ_RTL8168_PROBE=0 */
    kprintf("rtl8168: freestanding rtl SKIP GJ_RTL8168_PROBE=0 "
            "freestanding_rtl=SKIP claim=0 "
            "product=UDX+ABI path=rtl8168_udx dual_dod_b=OPEN_UDX "
            "need=UDX_OPEN soft=1 product=0 C0=1 "
            "(Soft!=product; G-AC-1; Linux-shaped userspace UDX; "
            "not freestanding class thrash; lean residual only; "
            "not re-enable freestanding rtl; not Dual DoD B close)\n");
#else
    /*
     * Opt-in residual: thrash stripped. Still no BAR/rings/R-climb.
     * Product remains UDX+ABI. Dual DoD B stays OPEN_UDX. Soft!=product.
     * NEVER restore freestanding NIC as product wire.
     */
    kprintf("rtl8168: probe SKIP (GJ_RTL8168_PROBE=1 residual lean; "
            "thrash stripped; no BAR claim; freestanding_rtl=OPTIN_RESIDUAL; "
            "claim=0 product=UDX+ABI path=rtl8168_udx; "
            "dual_dod_b=OPEN_UDX need=UDX_OPEN thr_only_eth=1; "
            "Soft!=product; G-AC-1; no R-climb; not Dual DoD B close; "
            "stamp_storm=0 version_stamp=0)\n");
#endif

    /*
     * Grep: rtl8168: soft residual product=UDX+ABI
     * Product laptop NIC = userspace rtl8168_udx + hot+cold ABI + DDI (G-AC-1).
     * Freestanding rtl residual lean only - not Dual DoD B close, not T1.
     * C0 freestanding SKIP residual honesty only (NOT product Dual DoD B).
     */
    kprintf("rtl8168: soft residual product=UDX+ABI "
            "probe=%u skip_default=%u rtl_probe=%u backend=none "
            "claim=0 freestanding_rtl=%s "
            "path=rtl8168_udx dual_dod_b=OPEN_UDX need=UDX_OPEN "
            "thr_only_eth=1 soft=1 product=0 G-AC-1=1 C0=1 "
            "(userspace rtl8168_udx + ABI; freestanding residual lean only; "
            "no freestanding rtl thrash; != .ko; != product T1; "
            "not freestanding Dual DoD B close; no R-climb thrash; "
            "Soft!=product; stamp_storm=0 version_stamp=0)\n",
            (unsigned)u32Probe, (unsigned)u32SkipDef,
            (unsigned)u32Probe, szFsRtl);

    /*
     * Grep: rtl8168: soft residual product_udx_abi honesty
     * Align net_l2 / xhci product_udx_abi honesty (NIC side = Dual DoD B).
     * OPEN until UDX host proof; freestanding rtl never product close.
     * Grep: role=skip_rtl_product_udx_abi | owner=product_udx_abi
     */
    kprintf("rtl8168: soft residual product_udx_abi honesty "
            "net=product_udx_abi owner=product_udx_abi "
            "role=skip_rtl_product_udx_abi "
            "product=UDX+ABI path=userspace_rtl8168_udx_hot_cold_abi "
            "direction=userspace_rtl8168_udx_over_hot_cold_abi "
            "freestanding_rtl=%s dual_dod_b=OPEN_UDX need=UDX_OPEN "
            "claim=0 soft=1 product=0 G-AC-1=1 C0=1 "
            "probe=%u skip_default=%u thr_only_eth=1 "
            "(Soft!=product; freestanding rtl SKIP residual; "
            "not freestanding BAR claim as product; "
            "not re-enable freestanding rtl; lean residual only; "
            "dual MIT OR Apache-2.0; not Dual DoD B close; "
            "stamp_storm=0 version_stamp=0)\n",
            szFsRtl, (unsigned)u32Probe, (unsigned)u32SkipDef);

    /*
     * Grep: rtl8168: soft residual lean
     * Compact Dual DoD B seed honesty - dual MIT/Apache; no version stamp.
     * R-climb / rings dead under SKIP (no freestanding thrash fields).
     * Grep: freestanding_rtl=SKIP | need=UDX_OPEN | claim=0 | C0
     */
    kprintf("rtl8168: soft residual lean soft=1 product=0 "
            "probe=%u skip_default=%u claim=0 rings=0 r_climb=0 "
            "freestanding_rtl=%s path=rtl8168_udx+ABI "
            "dual=MIT_OR_Apache-2.0 dual_dod_b=OPEN_UDX need=UDX_OPEN "
            "thr_only_eth=1 freestanding_close=0 C0_SKIP=1 C0=1 "
            "stamp_storm=0 version_stamp=0 Soft!=product G-AC-1=1\n",
            (unsigned)u32Probe, (unsigned)u32SkipDef, szFsRtl);
    return 0;
}

int
rtl8168_ready(void)
{
    /* Freestanding never owns wire. Soft!=product · Dual DoD B OPEN_UDX. */
    return 0;
}

int
rtl8168_link_up(void)
{
    return 0;
}

int
rtl8168_tx(const void *pFrame, u32 cbLen)
{
    (void)pFrame;
    (void)cbLen;
    /* Product TX = rtl8168_udx + ABI. Soft!=product. G-AC-1. */
    return -1;
}

i32
rtl8168_rx(void *pOut, u32 cbMax)
{
    (void)pOut;
    (void)cbMax;
    /* Product RX = rtl8168_udx + ABI. Soft!=product. G-AC-1. */
    return -1;
}

void
rtl8168_mac(u8 *pMac)
{
    if (pMac != NULL) {
        pMac[0] = 0;
        pMac[1] = 0;
        pMac[2] = 0;
        pMac[3] = 0;
        pMac[4] = 0;
        pMac[5] = 0;
    }
}

void
rtl8168_poll_hw(void)
{
    /* No freestanding TE|RE / R-climb thrash. Soft!=product. G-AC-1. */
}

u32
rtl8168_tx_count(void)
{
    return 0;
}

u32
rtl8168_rx_count(void)
{
    return 0;
}

u32
rtl8168_tx_fail(void)
{
    return 0;
}

u32
rtl8168_tx_busy(void)
{
    return 0;
}

u32
rtl8168_rx_drop(void)
{
    return 0;
}

void
rtl8168_soft_handoff_prepare(void)
{
    /*
     * C0 residual SKIP honesty only. No MMIO touch. Soft!=product · G-AC-1.
     * Dual DoD B stays OPEN_UDX need=UDX_OPEN. H2 once-shot.
     * Grep: rtl8168: soft mmio handoff SKIP
     */
    if (g_fHandoffSkipOnce == 0) {
        g_fHandoffSkipOnce = 1;
        kprintf("rtl8168: soft mmio handoff SKIP "
                "(C0 freestanding residual lean; freestanding_rtl=SKIP; "
                "claim=0 product=UDX+ABI path=rtl8168_udx; "
                "dual_dod_b=OPEN_UDX need=UDX_OPEN thr_only_eth=1; "
                "Soft!=product; G-AC-1; not Dual DoD B close; "
                "stamp_storm=0 version_stamp=0)\n");
    }
}

int
rtl8168_soft_handoff_prepared(void)
{
    return 0;
}

void
rtl8168_kick_wire(void)
{
    /*
     * No freestanding thrash. thr-only eth residual. Soft!=product · G-AC-1.
     * Dual DoD B OPEN_UDX need=UDX_OPEN. Grep: rtl8168: soft kick wire SKIP
     */
    if (g_fKickSkipOnce == 0) {
        g_fKickSkipOnce = 1;
        kprintf("rtl8168: soft kick wire SKIP "
                "(C0 freestanding residual lean; freestanding_rtl=SKIP; "
                "claim=0 product=UDX+ABI path=rtl8168_udx; "
                "dual_dod_b=OPEN_UDX need=UDX_OPEN thr_only_eth=1; "
                "Soft!=product; G-AC-1; not Dual DoD B close; "
                "stamp_storm=0 version_stamp=0)\n");
    }
}

int
rtl8168_post_te_rearm(void)
{
    /*
     * Residual SKIP. Soft!=product · G-AC-1. Dual DoD B OPEN_UDX need=UDX_OPEN.
     * Grep: rtl8168: soft post-te rearm SKIP
     */
    if (g_fPostTeSkipOnce == 0) {
        g_fPostTeSkipOnce = 1;
        kprintf("rtl8168: soft post-te rearm SKIP "
                "(C0 freestanding residual lean; freestanding_rtl=SKIP; "
                "claim=0 product=UDX+ABI path=rtl8168_udx; "
                "dual_dod_b=OPEN_UDX need=UDX_OPEN thr_only_eth=1; "
                "Soft!=product; G-AC-1; not Dual DoD B close; "
                "stamp_storm=0 version_stamp=0)\n");
    }
    return -1;
}

int
rtl8168_reclaim_wire(void)
{
    /*
     * Residual SKIP. Soft!=product · G-AC-1. Dual DoD B OPEN_UDX need=UDX_OPEN.
     * Grep: rtl8168: soft reclaim wire SKIP
     */
    if (g_fReclaimSkipOnce == 0) {
        g_fReclaimSkipOnce = 1;
        kprintf("rtl8168: soft reclaim wire SKIP "
                "(C0 freestanding residual lean; freestanding_rtl=SKIP; "
                "claim=0 product=UDX+ABI path=rtl8168_udx; "
                "dual_dod_b=OPEN_UDX need=UDX_OPEN thr_only_eth=1; "
                "Soft!=product; G-AC-1; not Dual DoD B close; "
                "stamp_storm=0 version_stamp=0)\n");
    }
    return -1;
}
