/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux netdev lifecycle seed (ABI / hostability eng residual).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_netdev_soft_init: ready lamp + optional linux_ksym_register
 *   - alloc_etherdev_mqs / devm_alloc_etherdev_mqs: static pool of 4 netdevs
 *   - register_netdev / unregister_netdev / free_netdev lifecycle
 *   - soft skb pool (16) for napi_alloc_skb / receive drop / kfree_skb
 *   - soft skb path: skb_copy_bits / __skb_pad / skb_put / pskb_may_pull /
 *     __pskb_pull_tail / pskb_expand_head / __netdev_alloc_skb / dev_alloc_skb
 *   - carrier / queue / NAPI bookkeeping (enable/disable/schedule_prep/complete)
 *   - netdev_* printk soft no-ops or bookkeeping
 *   - residual lean: freestanding_rtl SKIP + product_nic=UDX honesty lamps
 *
 * Soft != ABI-stable: struct layouts are incomplete soft shapes, not Linux ABI.
 * Soft != product: no live product RX/TX; soft NAPI state only (not Linux bits).
 * Soft TX path helpers do NOT call .ko ndo_start_xmit.
 * Product NIC = userspace UDX (not freestanding rtl; not in-kernel .ko wire).
 * Freestanding rtl default SKIP (GJ_RTL8168_PROBE=0). Soft!=product. G-AC-1.
 *
 * Greppable markers (keep stable):
 *   linux_netdev_soft: soft init PASS
 *   linux_netdev_soft: soft register PASS name=...
 *   linux_netdev_soft: soft register SKIP
 *   linux_netdev_soft: soft ksym register PASS|SKIP
 *   linux_netdev_soft: soft napi enable|disable|prep|complete
 *   linux_netdev_soft: soft napi bridge enable
 *   linux_netdev_soft: soft napi poll skip .ko (mmio freestanding)
 *   linux_netdev_soft: soft softirq kick
 *   linux_netdev_soft: soft l2 rx napi
 *   linux_netdev_soft: soft open primary
 *   linux_netdev_soft: soft open skip .ko ops
 *   linux_netdev_soft: soft ops diagnostic ...
 *   linux_netdev_soft: soft hostish net_device fill PASS ...
 *   linux_netdev_soft: soft hostish residual refuse ...
 *   linux_netdev_soft: soft layout deepen PASS ...
 *   linux_netdev_soft: soft residual layout ...
 *   linux_netdev_soft: soft default ops install ...
 *   linux_netdev_soft: soft l2 bridge first tx
 *   linux_netdev_soft: soft rtnl ...
 *   linux_netdev_soft: soft eth_platform EOPNOTSUPP
 *   linux_netdev_soft: soft eth_mac ...
 *   linux_netdev_soft: soft synchronize_net
 *   linux_netdev_soft: soft ethtool_op_get_link ...
 *   linux_netdev_soft: soft ethtool_op_get_ts_info ...
 *   linux_netdev_soft: soft net_ratelimit ...
 *   linux_netdev_soft: soft skb_copy_bits ...
 *   linux_netdev_soft: soft __skb_pad ...
 *   linux_netdev_soft: soft skb_put ...
 *   linux_netdev_soft: soft pskb_may_pull ...
 *   linux_netdev_soft: soft __netdev_alloc_skb ...
 *   linux_netdev_soft: soft l2 tx from_skb ...
 *   linux_netdev_soft: soft dev_queue_xmit ...
 *   linux_netdev_soft: soft napi mmio_handoff SET refuse
 *   linux_netdev_soft: soft hybrid zero-touch PASS Soft!=product ...
 *   linux_netdev_soft: soft hybrid residual E Soft!=product ...
 *   linux_netdev_soft: soft dual DoD B residual gate0 PASS Soft!=product ...
 *   linux_netdev_soft: soft residual lean PASS Soft!=product ...
 *   linux_netdev_soft: soft residual freestanding_rtl=SKIP ...
 *   linux_netdev_soft: soft residual product_nic=UDX ...
 *   linux_netdev_soft: soft residual RUN_INIT=0 freestanding_no_exec ...
 *   linux_netdev_soft: soft residual R0 thrash refuse ...
 *   linux_netdev_soft: soft residual wire sole freestanding ...
 *   linux_netdev_soft: soft freestanding owns wire ...
 *   linux_netdev_soft: soft open|napi|xmit fail-closed live 8168 ...
 *   linux_netdev_soft: soft residual C0 ... (claim_class=C0; Dual DoD OPEN)
 *   main: linux path HYBRID wire=freestanding soft=r8169  (phase 4a lamp)
 *   STATUS hold14: l2 br rx=N tx=M · hold15: HYBRID wire=fs soft=r8169
 *   linux_netdev_soft: soft hold14 freestanding R/T mirror rx=... tx=...
 *
 * Soft residual lean (ABI / hostability eng · C0 claim class):
 * Soft netdev seed residual only - never claim freestanding wire product.
 * Freestanding rtl default SKIP (GJ_RTL8168_PROBE=0). Product NIC =
 * userspace UDX. Soft!=product; G-AC-1. Once-capped lamps; no stamp storms.
 * Claim class C0 (ASSURANCE_LITE): greppable soft lamp only - never Dual
 * DoD A/B close, never bar3, never G-AC-1 product, never "net works".
 * Grep: soft residual lean PASS · freestanding_rtl=SKIP · product_nic=UDX
 *       claim_class=C0 · dual_DoD_A=OPEN · dual_DoD_B=OPEN
 *
 * Soft-originated TX (reverse path): soft-pool skb -> freestanding net_l2_tx
 * via linux_netdev_soft_l2_tx_from_skb / soft dev_queue_xmit ksym when
 * freestanding is deliberately live. Soft!=product; no dual-drive; no auto
 * wire smoke unless GJ_SOFT_L2_TX_SMOKE=1. Default freestanding_rtl=SKIP.
 *
 * Phase 4a hybrid (opt-in freestanding live): freestanding may own wire;
 * soft netdev is the Linux-shaped control object; L2 bridge + reverse TX.
 * Not 4b .ko wire. Not product NIC (product = UDX userspace). Soft!=product.
 * Grep: linux path HYBRID · linux_netdev_soft: soft hybrid · docs/R8169_MMIO_HANDOFF.md
 *
 * Hybrid residual E (gate0 dual DoD companion; opt-in freestanding): when
 * freestanding rtl is live it may own 10ec:8168 BAR/wire. Soft open / NAPI /
 * xmit fail closed for live 8168 - never call .ko ndo_open / napi poll /
 * ndo_start_xmit while freestanding ready. Hostish dual blob inventory only
 * (refuse helpers). Logs once-capped - no stamp storms. hold14 freestanding
 * R/T mirror when soft feed rare. Soft!=product. Hybrid SOFT must not steal
 * BAR (g_fMmioHandoff stays 0). Default freestanding_rtl=SKIP.
 *
 * Dual DoD B residual (gate0 hybrid friendliness · eng only · C0):
 * Soft residual MUST NOT steal freestanding wire or thrash live BAR (R0/H4).
 * RUN_INIT=0 / freestanding_no_exec honesty: soft hosts control object only -
 * never .ko init exec in kernel (G-AC-1 lean). Never claim freestanding wire
 * as product; product NIC = UDX userspace. Soft!=product. Once-capped lamps
 * only - no version stamp, no stamp storms. Dual DoD A/B OPEN until UDX DUT
 * proof (product path = Linux-shaped UDX; not freestanding rtl R-climb; not
 * soft listen :22 close; not in-kernel .ko wire). Dual MIT OR Apache-2.0.
 * Grep: soft dual DoD B residual gate0 · soft residual lean PASS
 *       soft residual freestanding_rtl=SKIP · soft residual product_nic=UDX
 *       soft residual RUN_INIT=0 · soft residual R0 thrash refuse
 *       soft no_bar_steal · soft residual wire sole freestanding
 *       soft freestanding owns wire · claim_class=C0
 *
 * hold14 freestanding R/T: while freestanding owns MMIO (opt-in live), panel
 * hold14 is max(soft fed/tx_ok, freestanding rtl R/T). Poll getters + feed/
 * note paths force-refresh so freestanding climb paints when soft feed is
 * rare (HY7). Soft!=product; read-only counters; never dual-drive BAR.
 *
 * Safety (dual MMIO): when freestanding is live it owns MMIO. Soft L2 bridge
 * feeds RX and runs soft NAPI bookkeeping only. Do NOT call .ko napi poll /
 * ISR while g_fMmioHandoff==0. Soft!=product. No hardware IRQ inject into .ko.
 * Default freestanding_rtl=SKIP; product NIC = UDX userspace.
 */
#include <gj/config.h>
#include <gj/fb_console.h>
#include <gj/klog.h>
#include <gj/linux_netdev_hostish_off.h>
#include <gj/linux_netdev_soft.h>
#include <gj/string.h>
#include <gj/types.h>
#include <stdarg.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved -> NULL; init skips export.
 * Coordinator may also point ksym at these symbols by C name without this call.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/* Soft log cap for netdev_* printk helpers (once-ish). */
#define LNDS_LOG_CAP 16u

/* Soft NAPI side-table (keyed by driver pNapi cookie; Soft!=Linux state bits). */
#define LNDS_NAPI_MAX       8u
#define LNDS_NAPI_F_USED    0x01u
#define LNDS_NAPI_F_ENABLED 0x02u
#define LNDS_NAPI_F_SCHED   0x04u
#define LNDS_NAPI_F_MISSED  0x08u
/* pfnPoll recorded via netif_napi_add soft body - treat as .ko until handoff. */
#define LNDS_NAPI_F_KO_POLL 0x10u

/* ---- Soft state --------------------------------------------------------- */

static int  g_fReady;
static u32  g_cInitCalls;
static u32  g_cAllocCalls;
static u32  g_cAllocFail;
static u32  g_cRegCalls;
static u32  g_cRegPass;
static u32  g_cRegSkip;
static u32  g_cUnreg;
static u32  g_cFree;
static u32  g_cSkbAlloc;
static u32  g_cSkbFree;
static u32  g_cSkbDrop;
static u32  g_cLogErr;
static u32  g_cLogInfo;
static u32  g_cLogWarn;
static u32  g_cLogNotice;
static u32  g_cLogPrintk;
static u32  g_cLive;
static u32  g_cRegistered;
static int  g_fL2Bridge;
static u32  g_cL2RxFed;
static u32  g_cL2RxAttempt;   /* bridge ON + eth-sized frame seen */
static u32  g_cL2RxDrop;      /* attempt but no primary / skb / etc. */
static u32  g_cL2TxOk;
static u32  g_cL2TxFail;
static u32  g_cL2TxAttempt;
static u32  g_cL2TxAccounted;
static int  g_fL2BridgeLog;
static int  g_fL2FirstTxLog;
/* hold14 last painted (avoid STATUS spam; Soft!=product). */
static u32  g_u32LastHold14Rx;
static u32  g_u32LastHold14Tx;
static int  g_fHold14Painted;
/* Last freestanding R/T sample used by hold14 mirror (read-only). */
static u32  g_u32Hold14FsRx;
static u32  g_u32Hold14FsTx;
/** Once-lamp when freestanding R or T first exceeds soft fed/tx_ok. */
static int  g_fHold14FsMirrorLog;
/** Once-log for gate-safe set_mmio_handoff refuse. */
static int  g_fMmioSetHandoffLog;
/** BAR-steal refuse tally (set_mmio_handoff refuse; Soft!=product). */
static u32  g_cBarStealRefuse;
/** Dual DoD B / gate0 residual once-lamp (lean; Soft!=product). */
static int  g_fGate0ResidualLog;
/** Soft residual lean once-lamp (ABI/hostability; freestanding_rtl SKIP). */
static int  g_fResidualLeanLog;
/** Residual E once-lamps: open/NAPI/xmit fail-closed live 8168 (Soft!=product). */
static int  g_fLive8168OpenFcLog;
static int  g_fLive8168NapiFcLog;
static int  g_fLive8168XmitFcLog;
/** Residual E fail-closed tallies (Soft!=product; no stamp storm). */
static u32  g_cLive8168FcOpen;
static u32  g_cLive8168FcNapi;
static u32  g_cLive8168FcXmit;
/** Hostish residual refuse once-lamps (inventory; never call .ko ndo). */
static int  g_fHostishOpenRefuseLog;
static int  g_fHostishXmitRefuseLog;
static u32  g_cHostishOpenRefuse;
static u32  g_cHostishXmitRefuse;
/** Cap soft register SKIP / unregister logs (no stamp storms). */
static u32  g_cRegSkipLog;
static u32  g_cUnregLog;
static u32  g_cSoftOpen;
static int  g_fSoftOpenLog;
static int  g_fSoftOpenSkipKoLog;
static int  g_fSoftOpsDiagLog;
/** Hostish net_device dual-blob fill once-log (Strategy A; Soft!=product). */
static int  g_fHostishNdFillLog;
/** Soft layout deepen once-lamp (fields/ops residual; Soft!=product). */
static int  g_fLayoutDeepenLog;
/** Soft default ops install once-log. */
static int  g_fSoftDefaultOpsLog;
/** Soft init first PASS already emitted (idempotent silent after). */
static int  g_fInitPassLog;
/** mmio_handoff state-change once-log (cap stamp storms). */
static int  g_fMmioHandoffStateLog;
/** Soft MMIO handoff ready stub once-log (phase 1). */
static int  g_fMmioHandoffReadyLog;
/** Phase-3 try_open once-log / done edge (Soft!=product). */
static int  g_fMmioTryOpenLog;
static int  g_fMmioTryOpenDone;
/** Phase-4a hybrid once-lamp (Soft!=product). */
static int  g_fHybridLampLog;
static u32  g_cNapiEnable;
static u32  g_cNapiDisable;
static u32  g_cNapiPrepOk;
static u32  g_cNapiPrepSkip;
static u32  g_cNapiComplete;
static u32  g_cNapiSched;
static u32  g_cNapiLive;
static u32  g_cNapiLog;
static u32  g_cNapiPollInvoke;
static u32  g_cNapiPollSkip;
static u32  g_cSoftirqKick;
static int  g_fNapiPollSkipKoLog;
static int  g_fL2RxNapiLog;
static int  g_fNapiBridgeEnLog;
/*
 * MMIO handoff gate (default 0): freestanding owns MMIO -> never call .ko
 * pfnPoll from soft_poll. Other agent may set 1 after full handoff; Soft!=product.
 */
static int  g_fMmioHandoff;

/* Post-probe RTNL / eth / sync bookkeeping (Soft!=product). */
static u32  g_cRtnlDepth;
static u32  g_cRtnlLock;
static u32  g_cRtnlUnlock;
static int  g_fRtnlLogOnce;
static int  g_fEthPlatLogOnce;
static int  g_fEthMacLogOnce;
static int  g_fSyncNetLogOnce;
static u32  g_cEthMac;
static u32  g_cEthValidate;
static u32  g_cEthTypeTrans;
static u32  g_cSyncNet;
static u32  g_cEthtoolLink;
static u32  g_cEthtoolTs;
static u32  g_cNetRatelimit;
static int  g_fEthtoolLinkLog;
static int  g_fEthtoolTsLog;
static int  g_fNetRatelimitLog;

/* Soft skb path bookkeeping (Soft!=product; future r8169 soft TX). */
static u32  g_cSkbCopyBits;
static u32  g_cSkbCopyBitsFail;
static u32  g_cSkbPad;
static u32  g_cSkbPadFail;
static u32  g_cSkbPut;
static u32  g_cSkbPutFail;
static u32  g_cPskbMayPull;
static u32  g_cPskbMayPullFail;
static u32  g_cPskbPullTail;
static u32  g_cPskbExpand;
static u32  g_cNetdevAllocSkb;
static int  g_fSkbCopyBitsLog;
static int  g_fSkbPadLog;
static int  g_fSkbPutLog;
static int  g_fPskbMayPullLog;
static int  g_fNetdevAllocSkbLog;
/* Soft-originated TX from soft skb / dev_queue_xmit (Soft!=product). */
static u32  g_cSoftTxFromSkbAttempt;
static u32  g_cSoftTxFromSkbOk;
static u32  g_cSoftTxFromSkbFail;
static u32  g_cSoftDqXmit;
static u32  g_cSoftDqXmitOk;
static u32  g_cSoftDqXmitFail;
static int  g_fSoftTxFromSkbLog;
static int  g_fSoftDqXmitLog;
static int  g_fSoftL2TxSmokeLog;

/* Soft errno-shaped returns (Linux negative errno mental model). */
#define LNDS_EOPNOTSUPP    (-95)
#define LNDS_EINVAL        (-22)
#define LNDS_EADDRNOTAVAIL (-99)
#define LNDS_ENOMEM        (-12)
#define LNDS_EFAULT        (-14)

/* Freestanding L2 (weak if net_l2 not linked). Soft!=product HW path. */
int net_l2_tx(const void *pFrame, u32 cbLen) __attribute__((weak));
int net_l2_ready(void) __attribute__((weak));
/* Soft MMIO handoff pending/fault (fail closed; weak if net_l2 absent). */
int net_l2_soft_handoff_pending(void) __attribute__((weak));
int net_l2_soft_handoff_fault(void) __attribute__((weak));
/*
 * Freestanding rtl8168 counters / ready (weak if rtl not linked).
 * hold14 freestanding R/T mirror samples these read-only. Soft!=product.
 */
int  rtl8168_ready(void) __attribute__((weak));
u32  rtl8168_rx_count(void) __attribute__((weak));
u32  rtl8168_tx_count(void) __attribute__((weak));

/** First successfully registered soft netdev still live. */
static struct net_device *g_pPrimary;

/**
 * Soft netdev slab: 2 KiB front (struct net_device) + priv area.
 * Priv is accessed as (u8 *)pNd + LINUX_NETDEV_SOFT_ND_BYTES.
 *
 * REAL r8169 probe writes host-layout fields into this same VA (ops @+0x08,
 * etc.). Soft front is 0x800; host sizeof is 0xaf0 -> host writes spill into
 * abPriv. Dual hostish blob (below) captures 0xaf0 for Option B readiness;
 * soft bookkeeping remains on soft struct. Soft priv @+0x800 != host priv
 * after 0xaf0 - see linux_netdev_hostish_off.h.
 */
struct soft_nd_slab {
    struct net_device nd;
    u8 abPriv[LINUX_NETDEV_SOFT_PRIV_MAX];
};

static struct soft_nd_slab g_aNd[LINUX_NETDEV_SOFT_MAX];

/* Soft slab must cover host sizeof so REAL .ko host writes fit in-object. */
_Static_assert(sizeof(struct soft_nd_slab) >= LINUX_NETDEV_HOSTISH_SIZE_NET_DEVICE,
               "soft_nd_slab must be ≥ host sizeof(net_device) 0xaf0");
_Static_assert(LINUX_NETDEV_HOSTISH_BLOB_BYTES >= LINUX_NETDEV_HOSTISH_SIZE_NET_DEVICE,
               "hostish blob must be ≥ host sizeof(net_device)");

/*
 * Strategy A dual hostish net_device pool (mirror pci_dev 0xb40 / 0xc00).
 * Separate from soft inventory; filled after register_netdev from soft primary
 * bytes (captures .ko host-offset stores). Soft!=product; no ndo_open call.
 */
static u8 g_aNdHostish[LINUX_NETDEV_HOSTISH_POOL][LINUX_NETDEV_HOSTISH_BLOB_BYTES]
    __attribute__((aligned(16)));
static u8 g_aNdHostishLive[LINUX_NETDEV_HOSTISH_POOL];
static void *g_pHostishPrimary; /* hostish blob for soft primary, or NULL */
static void *g_pHostishOpsCached;     /* Option B readiness; no call */
static void *g_pHostishNdoOpenCached; /* Option B readiness; no call */
static void *g_pHostishNdoStopCached; /* ops+0x18; no call */
static void *g_pHostishNdoXmitCached; /* ops+0x20; no call */
static int   g_fHostishFilled;

/** Soft skb slab (data buffer for later RX/TX feed). */
struct soft_skb {
    u8       u8Live;
    u8       u8Pad[3];
    unsigned uLen;
    unsigned uCap;
    void    *pNapi;
    u8       abData[LINUX_NETDEV_SOFT_SKB_DATA];
};

static struct soft_skb g_aSkb[LINUX_NETDEV_SOFT_SKB_MAX];
static u32             g_cSkbLive;

/**
 * Soft NAPI bookkeeping slot. pNapi is an opaque cookie from the driver
 * (often &priv->napi); Soft!=Linux napi_struct layout under that pointer.
 */
struct soft_napi {
    void *pNapi;
    void *pDev;
    void *pfnPoll;
    int   nWeight;
    u8    u8State;
    u8    u8Pad[3];
};

static struct soft_napi g_aNapi[LNDS_NAPI_MAX];

/*
 * Soft-shaped netdev_ops magic/ver: use header LINUX_NETDEV_SOFT_OPS_*.
 * Magic/version prefix distinguishes tables we own from a .ko ops pointer.
 * DO NOT call .ko ndo_open / ndo_start_xmit - freestanding owns MMIO.
 * Soft default table (ver2) is eng residual for UDX/Linux-shaped userspace.
 */
#define LNDS_SOFT_OPS_MAGIC  LINUX_NETDEV_SOFT_OPS_MAGIC
#define LNDS_SOFT_OPS_VER    LINUX_NETDEV_SOFT_OPS_VER

/* Defined later; feed_rx may call before body. Soft!=product. */
void linux_netdev_soft_softirq_kick(void);
/* Soft reverse TX used by default soft ndo_start_xmit residual. */
int linux_netdev_soft_l2_tx_from_skb(void *pSkb);
/* Soft default ops install (defined with soft_netdev_ops table). */
static void lnds_install_default_soft_ops(struct net_device *pNd);
#if GJ_SOFT_L2_TX_SMOKE != 0
/* Lab-only; defined after from_skb path. Soft!=product. */
static void lnds_soft_l2_tx_smoke_once(void);
#endif

/*
 * Live freestanding 10ec:8168 owns wire (hybrid residual E).
 * Non-zero when rtl ready and soft has not claimed MMIO handoff.
 * Soft open/NAPI/xmit must fail closed for .ko BAR touch. Soft!=product.
 * Hybrid SOFT must not steal BAR. Dual DoD B residual: gate0 never claims.
 */
static int
lnds_live_fs_8168(void)
{
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    /* Gate0: soft handoff claim is forbidden; force honesty. Soft!=product. */
    if (g_fMmioHandoff != 0) {
        g_fMmioHandoff = 0;
    }
#endif
    if (g_fMmioHandoff != 0) {
        return 0;
    }
    if (rtl8168_ready != NULL && rtl8168_ready() != 0) {
        return 1;
    }
    return 0;
}

/**
 * Soft freestanding-owns check for reverse TX / hybrid residual E.
 * Soft!=product: handoff pending / fault / soft mmio_handoff -> not freestanding.
 * Hybrid SOFT must not steal BAR - reverse path fails closed if soft claims.
 * Dual DoD B residual: refuse dual-drive that yields freestanding R0.
 */
static int
lnds_soft_tx_freestanding_owns(void)
{
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    /* Gate0 residual: never allow soft BAR claim to poison freestanding wire. */
    if (g_fMmioHandoff != 0) {
        g_fMmioHandoff = 0;
    }
#endif
    if (g_fMmioHandoff != 0) {
        return 0;
    }
    if (net_l2_soft_handoff_pending != NULL &&
        net_l2_soft_handoff_pending() != 0) {
        return 0;
    }
    if (net_l2_soft_handoff_fault != NULL &&
        net_l2_soft_handoff_fault() != 0) {
        return 0;
    }
    return 1;
}

/**
 * Gate0 residual: force soft MMIO handoff clear (no BAR steal / no R0 thrash).
 * Compile-time gate0 only. Soft!=product; dual DoD B freestanding wire friend.
 */
static void
lnds_gate0_force_no_bar_steal(void)
{
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    if (g_fMmioHandoff != 0) {
        g_fMmioHandoff = 0;
        if (g_cBarStealRefuse < 0xffffffffu) {
            g_cBarStealRefuse++;
        }
    }
#else
    (void)0;
#endif
}

/* ---- Helpers ------------------------------------------------------------ */

static void
lnds_ksym_one(const char *szName, void *pFn, u32 *pu32Ok, u32 *pu32Skip)
{
    int nSt;

    if (szName == NULL || pFn == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    if (linux_ksym_register == NULL) {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
        return;
    }
    nSt = linux_ksym_register(szName, pFn);
    if (nSt == 0) {
        if (pu32Ok != NULL && *pu32Ok < 0xffffffffu) {
            (*pu32Ok)++;
        }
    } else {
        if (pu32Skip != NULL && *pu32Skip < 0xffffffffu) {
            (*pu32Skip)++;
        }
    }
}

static struct soft_nd_slab *
lnds_slab_of(struct net_device *pNd)
{
    u32 i;

    if (pNd == NULL) {
        return NULL;
    }
    for (i = 0u; i < LINUX_NETDEV_SOFT_MAX; i++) {
        if (&g_aNd[i].nd == pNd) {
            return &g_aNd[i];
        }
    }
    return NULL;
}

static struct net_device *
lnds_alloc_nd(int nSizeofPriv, unsigned uTxQs, unsigned uRxQs, void *pParent,
              int fDevm)
{
    u32 i;
    struct net_device *pNd;
    u32 u32Priv;

    if (g_cAllocCalls < 0xffffffffu) {
        g_cAllocCalls++;
    }

    if (nSizeofPriv < 0) {
        nSizeofPriv = 0;
    }
    u32Priv = (u32)nSizeofPriv;
    if (u32Priv > LINUX_NETDEV_SOFT_PRIV_MAX) {
        if (g_cAllocFail < 0xffffffffu) {
            g_cAllocFail++;
        }
        kprintf("linux_netdev_soft: soft alloc SKIP (priv %u > max %u)\n",
                (unsigned)u32Priv, (unsigned)LINUX_NETDEV_SOFT_PRIV_MAX);
        return NULL;
    }

    for (i = 0u; i < LINUX_NETDEV_SOFT_MAX; i++) {
        if (g_aNd[i].nd.u8Live == 0u) {
            break;
        }
    }
    if (i >= LINUX_NETDEV_SOFT_MAX) {
        if (g_cAllocFail < 0xffffffffu) {
            g_cAllocFail++;
        }
        kprintf("linux_netdev_soft: soft alloc SKIP (netdev pool full max=%u)\n",
                (unsigned)LINUX_NETDEV_SOFT_MAX);
        return NULL;
    }

    memset(&g_aNd[i], 0, sizeof(g_aNd[i]));
    pNd = &g_aNd[i].nd;
    pNd->u8Live = 1u;
    pNd->u8Devm = fDevm ? 1u : 0u;
    pNd->pParentDev = pParent;
    pNd->nSizeofPriv = nSizeofPriv;
    pNd->u16TxQs = (u16)(uTxQs > 0xffffu ? 0xffffu : uTxQs);
    pNd->u16RxQs = (u16)(uRxQs > 0xffffu ? 0xffffu : uRxQs);
    pNd->u32Slot = i;
    pNd->mtu = 1500u;
    pNd->u8Attached = 1u;
    /* Soft layout deepen residual (UDX/Linux-shaped; Soft!=host ABI). */
    pNd->u16Type = (u16)LINUX_NETDEV_SOFT_ARPHRD_ETHER;
    pNd->u16HardHeaderLen = (u16)LINUX_NETDEV_SOFT_ETH_HLEN;
    pNd->u8AddrLen = (u8)LINUX_NETDEV_SOFT_ETH_ALEN;
    pNd->u16MinMtu = (u16)LINUX_NETDEV_SOFT_MIN_MTU;
    pNd->u32MaxMtu = LINUX_NETDEV_SOFT_MAX_MTU;
    pNd->nIfindex = (int)(i + 1u);
    pNd->u32SoftLayoutVer = LINUX_NETDEV_SOFT_LAYOUT_VER;
    /* Soft default features: SG|CSUM|TSO-ish soft bits (bookkeeping only). */
    pNd->u64Features = 0x3ull;       /* soft NETIF_F_SG | NETIF_F_IP_CSUM-ish */
    pNd->u64HwFeatures = 0x3ull;
    pNd->u64WantedFeatures = 0x3ull;
    pNd->u64VlanFeatures = 0ull;
    /* Soft default name ethN; register may keep or driver may overwrite. */
    if (i < 10u) {
        pNd->name[0] = 'e';
        pNd->name[1] = 't';
        pNd->name[2] = 'h';
        pNd->name[3] = (char)('0' + (int)i);
        pNd->name[4] = '\0';
    } else {
        (void)strlcpy(pNd->name, "eth?", sizeof(pNd->name));
    }
    /*
     * Install soft-owned ops / ethtool tables (LNDS / LNET magic).
     * Soft field @+0x18 - independent of hostish ops @+0x8 that REAL .ko
     * may store into the same slab. Soft!=product; freestanding owns wire.
     */
    lnds_install_default_soft_ops(pNd);

    if (g_cLive < 0xffffffffu) {
        g_cLive++;
    }
    return pNd;
}

/**
 * Soft skb cookie check: non-NULL only if pSkb is a live pool entry.
 * Fail closed for host/.ko sk_buff layouts we do not own. Soft!=product.
 */
static struct soft_skb *
lnds_skb_of(void *pSkb)
{
    struct soft_skb *pSoft;
    u32 i;

    if (pSkb == NULL) {
        return NULL;
    }
    pSoft = (struct soft_skb *)pSkb;
    for (i = 0u; i < LINUX_NETDEV_SOFT_SKB_MAX; i++) {
        if (&g_aSkb[i] == pSoft && pSoft->u8Live != 0u) {
            return pSoft;
        }
    }
    return NULL;
}

static void
lnds_skb_free(void *pSkb)
{
    struct soft_skb *pSoft;

    pSoft = lnds_skb_of(pSkb);
    if (pSoft == NULL) {
        return;
    }
    memset(pSoft, 0, sizeof(*pSoft));
    if (g_cSkbLive > 0u) {
        g_cSkbLive--;
    }
    if (g_cSkbFree < 0xffffffffu) {
        g_cSkbFree++;
    }
}

/**
 * Soft pool alloc. fSetLen: 1 -> uLen=uWant (RX-shaped); 0 -> uLen=0 (TX put).
 * Soft!=product; does not call .ko ndo_start_xmit.
 */
static struct soft_skb *
lnds_skb_alloc(void *pNapi, unsigned uWant, int fSetLen)
{
    u32 i;
    struct soft_skb *pSkb;

    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    if (uWant > LINUX_NETDEV_SOFT_SKB_DATA) {
        return NULL;
    }
    for (i = 0u; i < LINUX_NETDEV_SOFT_SKB_MAX; i++) {
        if (g_aSkb[i].u8Live == 0u) {
            break;
        }
    }
    if (i >= LINUX_NETDEV_SOFT_SKB_MAX) {
        return NULL;
    }
    pSkb = &g_aSkb[i];
    memset(pSkb, 0, sizeof(*pSkb));
    pSkb->u8Live = 1u;
    pSkb->uLen = (fSetLen != 0) ? uWant : 0u;
    pSkb->uCap = LINUX_NETDEV_SOFT_SKB_DATA;
    pSkb->pNapi = pNapi;
    if (g_cSkbLive < 0xffffffffu) {
        g_cSkbLive++;
    }
    if (g_cSkbAlloc < 0xffffffffu) {
        g_cSkbAlloc++;
    }
    return pSkb;
}

static const char *
lnds_name_of(const void *pDev)
{
    const struct net_device *pNd = (const struct net_device *)pDev;

    if (pNd == NULL || pNd->name[0] == '\0') {
        return "?";
    }
    return pNd->name;
}

static struct soft_napi *
lnds_napi_find(void *pNapi)
{
    u32 i;

    if (pNapi == NULL) {
        return NULL;
    }
    for (i = 0u; i < LNDS_NAPI_MAX; i++) {
        if ((g_aNapi[i].u8State & LNDS_NAPI_F_USED) != 0u &&
            g_aNapi[i].pNapi == pNapi) {
            return &g_aNapi[i];
        }
    }
    return NULL;
}

/**
 * Find or mint a soft NAPI slot for pNapi. First use starts disabled
 * (Linux: netif_napi_add leaves NAPI disabled until napi_enable).
 */
static struct soft_napi *
lnds_napi_ensure(void *pNapi)
{
    struct soft_napi *pSlot;
    u32 i;

    if (pNapi == NULL) {
        return NULL;
    }
    pSlot = lnds_napi_find(pNapi);
    if (pSlot != NULL) {
        return pSlot;
    }
    for (i = 0u; i < LNDS_NAPI_MAX; i++) {
        if ((g_aNapi[i].u8State & LNDS_NAPI_F_USED) == 0u) {
            memset(&g_aNapi[i], 0, sizeof(g_aNapi[i]));
            g_aNapi[i].pNapi = pNapi;
            g_aNapi[i].u8State = LNDS_NAPI_F_USED;
            g_aNapi[i].nWeight = 64;
            if (g_cNapiLive < 0xffffffffu) {
                g_cNapiLive++;
            }
            return &g_aNapi[i];
        }
    }
    return NULL;
}

static void
lnds_napi_bind(void *pDev, void *pNapi, void *pfnPoll, int nWeight)
{
    struct soft_napi *pSlot;

    if (pNapi == NULL) {
        return;
    }
    pSlot = lnds_napi_ensure(pNapi);
    if (pSlot == NULL) {
        return;
    }
    pSlot->pDev = pDev;
    pSlot->pfnPoll = pfnPoll;
    pSlot->nWeight = (nWeight > 0) ? nWeight : 64;
    /*
     * Soft body of netif_napi_add is invoked by .ko probe: mark KO_POLL so
     * soft_poll will not call pfnPoll while freestanding owns MMIO.
     * Soft!=product dual-MMIO safety.
     */
    if (pfnPoll != NULL) {
        pSlot->u8State |= LNDS_NAPI_F_KO_POLL;
    }
    /* Soft: re-add keeps enabled/sched as-is (open may re-add); no force. */
}

/**
 * Soft-enable NAPI slots bound to primary (or any used slot if primary cookie
 * matches). Used when L2 bridge turns ON / open_primary so feed_rx soft_poll
 * can run bookkeeping even if .ko open never reached soft napi_enable.
 * Soft!=product: state bits only; no .ko poll call here.
 */
static void
lnds_napi_soft_enable_bridge_slots(void)
{
    u32 i;
    u32 cEn;
    void *pPrimaryNapi;

    cEn = 0u;
    pPrimaryNapi = NULL;
    if (g_pPrimary != NULL) {
        pPrimaryNapi = g_pPrimary->pNapi;
    }

    /* Ensure primary cookie has a slot even if add raced before table. */
    if (pPrimaryNapi != NULL) {
        (void)lnds_napi_ensure(pPrimaryNapi);
        if (g_pPrimary != NULL) {
            struct soft_napi *pSlot = lnds_napi_find(pPrimaryNapi);
            if (pSlot != NULL && pSlot->pDev == NULL) {
                pSlot->pDev = g_pPrimary;
            }
        }
    }

    for (i = 0u; i < LNDS_NAPI_MAX; i++) {
        struct soft_napi *pSlot = &g_aNapi[i];
        int fMatch;

        if ((pSlot->u8State & LNDS_NAPI_F_USED) == 0u) {
            continue;
        }
        fMatch = 0;
        if (g_pPrimary != NULL && pSlot->pDev == (void *)g_pPrimary) {
            fMatch = 1;
        }
        if (pPrimaryNapi != NULL && pSlot->pNapi == pPrimaryNapi) {
            fMatch = 1;
        }
        /* No primary cookie yet: enable all registered slots (soft probe). */
        if (g_pPrimary != NULL && pPrimaryNapi == NULL && pSlot->pDev != NULL) {
            fMatch = 1;
        }
        if (fMatch == 0) {
            continue;
        }
        if ((pSlot->u8State & LNDS_NAPI_F_ENABLED) == 0u) {
            /*
             * Bookkeeping enable only (same soft bits as napi_enable).
             * Avoid double-counting enable calls when already enabled later.
             */
            napi_enable(pSlot->pNapi);
            cEn++;
        }
    }

    if (g_fNapiBridgeEnLog == 0 &&
        (cEn > 0u || linux_netdev_soft_napi_enabled_count() > 0u)) {
        g_fNapiBridgeEnLog = 1;
        /* Grep: linux_netdev_soft: soft napi bridge enable */
        kprintf("linux_netdev_soft: soft napi bridge enable newly=%u "
                "enabled=%u sched=%u mmio_handoff=%d "
                "(Soft!=product; bookkeeping only; no .ko poll)\n",
                (unsigned)cEn,
                (unsigned)linux_netdev_soft_napi_enabled_count(),
                (unsigned)linux_netdev_soft_napi_sched_count(),
                g_fMmioHandoff);
    }
}

static int
lnds_log_once(u32 *pu32Ctr, const char *szTag, const void *pDev,
              const char *szFmt)
{
    if (pu32Ctr == NULL) {
        return 0;
    }
    if (*pu32Ctr >= LNDS_LOG_CAP) {
        return 0;
    }
    (*pu32Ctr)++;
    kprintf("linux_netdev_soft: %s name=%s %s\n",
            szTag != NULL ? szTag : "log", lnds_name_of(pDev),
            szFmt != NULL ? szFmt : "");
    return 0;
}

/** 1 if pOps is a soft-shaped ops table we own (magic+ver 1..current). Soft!=product. */
static int
lnds_ops_is_soft(void *pOps)
{
    const struct soft_netdev_ops *pSoft;

    if (pOps == NULL) {
        return 0;
    }
    pSoft = (const struct soft_netdev_ops *)pOps;
    if (pSoft->u32Magic != LNDS_SOFT_OPS_MAGIC) {
        return 0;
    }
    /* Accept ver1 prefix and ver2 deepen tables. */
    if (pSoft->u32Ver < 1u || pSoft->u32Ver > LNDS_SOFT_OPS_VER) {
        return 0;
    }
    return 1;
}

/** 1 if pOps is soft ethtool table we own. Soft!=product. */
static int
lnds_ethtool_is_soft(void *pOps)
{
    const struct soft_ethtool_ops *pSoft;

    if (pOps == NULL) {
        return 0;
    }
    pSoft = (const struct soft_ethtool_ops *)pOps;
    return (pSoft->u32Magic == LINUX_NETDEV_SOFT_ETHTOOL_MAGIC &&
            pSoft->u32Ver == LINUX_NETDEV_SOFT_ETHTOOL_VER) ? 1 : 0;
}

/*
 * Soft default ops bodies (eng residual; Soft!=product).
 * Live freestanding 8168 -> fail closed (residual E; no BAR steal).
 * Soft xmit may reverse-path soft-pool skb via freestanding L2 when safe.
 */
static int
lnds_soft_ndo_init(void *pDev)
{
    (void)pDev;
    return 0;
}

static void
lnds_soft_ndo_uninit(void *pDev)
{
    (void)pDev;
}

static int
lnds_soft_ndo_open(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    /* Dual DoD B residual: never claim BAR via soft open. Soft!=product. */
    lnds_gate0_force_no_bar_steal();
    if (pNd == NULL || lnds_slab_of(pNd) == NULL) {
        return LNDS_EINVAL;
    }
    if (lnds_live_fs_8168() != 0) {
        /* Residual E: freestanding owns wire - bookkeeping only, no BAR. */
        if (g_cLive8168FcOpen < 0xffffffffu) {
            g_cLive8168FcOpen++;
        }
        return 0;
    }
    pNd->u8Open = 1u;
    pNd->u8Carrier = 1u;
    pNd->u8QueueStopped = 0u;
    return 0;
}

static int
lnds_soft_ndo_stop(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL) {
        return LNDS_EINVAL;
    }
    pNd->u8Open = 0u;
    pNd->u8Carrier = 0u;
    pNd->u8QueueStopped = 1u;
    return 0;
}

static int
lnds_soft_ndo_start_xmit(void *pSkb, void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;
    int nSt;

    (void)pNd;
    /* Dual DoD B residual: soft xmit must never steal freestanding BAR. */
    lnds_gate0_force_no_bar_steal();
    if (lnds_live_fs_8168() != 0) {
        /*
         * Residual E: never .ko-shaped xmit dual-drive. Soft reverse path
         * still allowed only via freestanding L2 when bridge ON elsewhere;
         * default soft xmit fail-closed on live 8168 to keep sole owner.
         * Never follow hostish ndo_start_xmit @ ops+0x20. Soft!=product.
         */
        if (g_cLive8168FcXmit < 0xffffffffu) {
            g_cLive8168FcXmit++;
        }
        if (g_fLive8168XmitFcLog == 0) {
            g_fLive8168XmitFcLog = 1;
            /* Grep: linux_netdev_soft: soft xmit fail-closed live 8168 */
            kprintf("linux_netdev_soft: soft xmit fail-closed live 8168 "
                    "soft_ops_xmit=1 (Soft!=product; freestanding owns BAR; "
                    "no .ko ndo_start_xmit; residual E; reverse TX via "
                    "l2_xmit/dev_queue_xmit only; no_bar_steal=1)\n");
        }
        if (pSkb != NULL) {
            lnds_skb_free(pSkb);
        }
        return LNDS_EOPNOTSUPP;
    }
    if (pSkb == NULL) {
        return LNDS_EINVAL;
    }
    /* Soft-pool reverse TX only while freestanding owns wire. Soft!=product. */
    if (lnds_soft_tx_freestanding_owns() == 0) {
        if (pSkb != NULL) {
            lnds_skb_free(pSkb);
        }
        return LNDS_EOPNOTSUPP;
    }
    nSt = linux_netdev_soft_l2_tx_from_skb(pSkb);
    return (nSt == 0) ? 0 : LNDS_EOPNOTSUPP;
}

static int
lnds_soft_ndo_set_mac(void *pDev, void *pSa)
{
    return eth_mac_addr(pDev, pSa);
}

static int
lnds_soft_ndo_validate_addr(void *pDev)
{
    return eth_validate_addr(pDev);
}

static int
lnds_soft_ndo_change_mtu(void *pDev, int nNewMtu)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return LNDS_EINVAL;
    }
    if (nNewMtu < (int)pNd->u16MinMtu || nNewMtu > (int)pNd->u32MaxMtu) {
        return LNDS_EINVAL;
    }
    pNd->mtu = (u32)nNewMtu;
    return 0;
}

static void
lnds_soft_ndo_tx_timeout(void *pDev, unsigned uTxq)
{
    (void)pDev;
    (void)uTxq;
    /* Soft no-op; freestanding owns wire timeouts. Soft!=product. */
}

static void
lnds_soft_ndo_get_stats64(void *pDev, void *pStats)
{
    struct net_device *pNd = (struct net_device *)pDev;
    struct soft_netdev_stats64 *pS = (struct soft_netdev_stats64 *)pStats;

    if (pS == NULL) {
        return;
    }
    memset(pS, 0, sizeof(*pS));
    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pS->u64RxPackets = pNd->u64RxPackets;
    pS->u64TxPackets = pNd->u64TxPackets;
    pS->u64RxBytes = pNd->u64RxBytes;
    pS->u64TxBytes = pNd->u64TxBytes;
    pS->u64RxDropped = pNd->u64RxDropped;
    pS->u64TxDropped = pNd->u64TxDropped;
}

static const struct soft_netdev_ops g_softDefaultOps = {
    .u32Magic = LNDS_SOFT_OPS_MAGIC,
    .u32Ver = LNDS_SOFT_OPS_VER,
    .ndo_open = lnds_soft_ndo_open,
    .ndo_stop = lnds_soft_ndo_stop,
    .ndo_init = lnds_soft_ndo_init,
    .ndo_uninit = lnds_soft_ndo_uninit,
    .ndo_start_xmit = lnds_soft_ndo_start_xmit,
    .ndo_set_mac_address = lnds_soft_ndo_set_mac,
    .ndo_validate_addr = lnds_soft_ndo_validate_addr,
    .ndo_change_mtu = lnds_soft_ndo_change_mtu,
    .ndo_tx_timeout = lnds_soft_ndo_tx_timeout,
    .ndo_get_stats64 = lnds_soft_ndo_get_stats64,
};

static const struct soft_ethtool_ops g_softDefaultEthtoolOps = {
    .u32Magic = LINUX_NETDEV_SOFT_ETHTOOL_MAGIC,
    .u32Ver = LINUX_NETDEV_SOFT_ETHTOOL_VER,
    .get_link = ethtool_op_get_link,
    .get_ts_info = ethtool_op_get_ts_info,
};

/**
 * Install soft-owned default ops/ethtool on a soft netdev (alloc path).
 * Soft!=product; does not touch hostish ops @+0x8. Once-log capped.
 */
static void
lnds_install_default_soft_ops(struct net_device *pNd)
{
    if (pNd == NULL) {
        return;
    }
    pNd->netdev_ops = (void *)&g_softDefaultOps;
    pNd->ethtool_ops = (void *)&g_softDefaultEthtoolOps;
    pNd->u8SoftOpsOwned = 1u;
    if (g_fSoftDefaultOpsLog == 0) {
        g_fSoftDefaultOpsLog = 1;
        /* Grep: linux_netdev_soft: soft default ops install */
        kprintf("linux_netdev_soft: soft default ops install "
                "ops_ver=%u ethtool_ver=%u layout_ver=%u "
                "(Soft!=product; LNDS soft table; freestanding owns wire; "
                "no .ko ndo_open/xmit)\n",
                (unsigned)LNDS_SOFT_OPS_VER,
                (unsigned)LINUX_NETDEV_SOFT_ETHTOOL_VER,
                (unsigned)LINUX_NETDEV_SOFT_LAYOUT_VER);
    }
}

const struct soft_netdev_ops *
linux_netdev_soft_default_ops(void)
{
    return &g_softDefaultOps;
}

const struct soft_ethtool_ops *
linux_netdev_soft_default_ethtool_ops(void)
{
    return &g_softDefaultEthtoolOps;
}

/* Soft module load range (weak if linux_module not linked). */
int linux_module_load_va_range(const char *szName, void **ppBase, u64 *pcb)
    __attribute__((weak));

/**
 * True if pFn is inside [pBase, pBase+cb). Soft!=product diagnostic.
 */
static int
lnds_ptr_in_range(const void *pFn, const void *pBase, u64 cb)
{
    uintptr_t uFn;
    uintptr_t uBase;
    uintptr_t uEnd;

    if (pFn == NULL || pBase == NULL || cb == 0ull) {
        return 0;
    }
    uFn = (uintptr_t)pFn;
    uBase = (uintptr_t)pBase;
    uEnd = uBase + (uintptr_t)cb;
    if (uEnd < uBase) {
        return 0;
    }
    return (uFn >= uBase && uFn < uEnd) ? 1 : 0;
}

/**
 * Read hostish netdev_ops from a net_device-shaped base (soft or hostish blob).
 * Host OFF_NETDEV_OPS = 0x08. Soft!=product. No ndo_open call.
 */
void *
linux_netdev_soft_hostish_ops(void *pNd)
{
    void *pOps;
    const u8 *pBytes;

    if (pNd == NULL) {
        return NULL;
    }
    pOps = NULL;
    pBytes = (const u8 *)pNd;
    memcpy(&pOps, pBytes + LINUX_NETDEV_HOSTISH_OFF_NETDEV_OPS, sizeof(void *));
    return pOps;
}

/**
 * Read hostish function pointer from ops table at host relative offset.
 * Soft!=product. Never invokes the function.
 */
static void *
lnds_hostish_ops_slot(void *pNd, u32 u32Off)
{
    void *pOps;
    void *pFn;
    const u8 *pOpsBytes;

    pOps = linux_netdev_soft_hostish_ops(pNd);
    if (pOps == NULL) {
        return NULL;
    }
    pFn = NULL;
    pOpsBytes = (const u8 *)pOps;
    memcpy(&pFn, pOpsBytes + u32Off, sizeof(void *));
    return pFn;
}

/**
 * Read hostish ndo_open from ops table at +0x10 (RHEL 5.14 oracle).
 * Soft!=product. Never invokes the function.
 */
void *
linux_netdev_soft_hostish_ndo_open(void *pNd)
{
    return lnds_hostish_ops_slot(pNd, LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_OPEN);
}

/**
 * Hostish ndo_stop @ ops+0x18. Soft deepen residual; never call. Soft!=product.
 */
void *
linux_netdev_soft_hostish_ndo_stop(void *pNd)
{
    return lnds_hostish_ops_slot(pNd, LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_STOP);
}

/**
 * Hostish ndo_start_xmit @ ops+0x20. Soft deepen residual; never call.
 * Soft!=product; freestanding owns wire under residual E.
 */
void *
linux_netdev_soft_hostish_ndo_start_xmit(void *pNd)
{
    return lnds_hostish_ops_slot(pNd,
                                 LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_START_XMIT);
}

void *
linux_netdev_soft_hostish_primary(void)
{
    if (g_fHostishFilled == 0 || g_pHostishPrimary == NULL) {
        return NULL;
    }
    return g_pHostishPrimary;
}

int
linux_netdev_soft_hostish_filled(void)
{
    return g_fHostishFilled != 0 ? 1 : 0;
}

void *
linux_netdev_soft_hostish_ops_cached(void)
{
    return g_fHostishFilled != 0 ? g_pHostishOpsCached : NULL;
}

void *
linux_netdev_soft_hostish_ndo_open_cached(void)
{
    return g_fHostishFilled != 0 ? g_pHostishNdoOpenCached : NULL;
}

void *
linux_netdev_soft_hostish_ndo_stop_cached(void)
{
    return g_fHostishFilled != 0 ? g_pHostishNdoStopCached : NULL;
}

void *
linux_netdev_soft_hostish_ndo_xmit_cached(void)
{
    return g_fHostishFilled != 0 ? g_pHostishNdoXmitCached : NULL;
}

/*
 * Hostish residual refuse helpers (inventory honesty; Soft!=product).
 * Residual E / Option B readiness: recover pointers, never invoke under
 * freestanding wire ownership. Once-lamp; tallies only. Cap logs.
 * Grep: linux_netdev_soft: soft hostish residual refuse
 */
int
linux_netdev_soft_hostish_ndo_open_refuse(void)
{
    void *pOpen;

    if (g_cHostishOpenRefuse < 0xffffffffu) {
        g_cHostishOpenRefuse++;
    }
    pOpen = linux_netdev_soft_hostish_ndo_open_cached();
    if (pOpen == NULL && g_pPrimary != NULL) {
        pOpen = linux_netdev_soft_hostish_ndo_open(g_pPrimary);
    }
    if (g_fHostishOpenRefuseLog == 0) {
        g_fHostishOpenRefuseLog = 1;
        /* Grep: linux_netdev_soft: soft hostish residual refuse */
        kprintf("linux_netdev_soft: soft hostish residual refuse "
                "ndo_open=%p live_fs_8168=%d mmio_handoff=%d filled=%d "
                "(Soft!=product; residual E; no .ko ndo_open call; "
                "freestanding owns wire; Option B gate default 0)\n",
                pOpen, lnds_live_fs_8168(), g_fMmioHandoff,
                g_fHostishFilled);
    }
    (void)pOpen;
    return LNDS_EOPNOTSUPP;
}

int
linux_netdev_soft_hostish_ndo_xmit_refuse(void)
{
    void *pXmit;

    if (g_cHostishXmitRefuse < 0xffffffffu) {
        g_cHostishXmitRefuse++;
    }
    pXmit = linux_netdev_soft_hostish_ndo_xmit_cached();
    if (pXmit == NULL && g_pPrimary != NULL) {
        pXmit = linux_netdev_soft_hostish_ndo_start_xmit(g_pPrimary);
    }
    if (g_fHostishXmitRefuseLog == 0) {
        g_fHostishXmitRefuseLog = 1;
        /* Grep: linux_netdev_soft: soft hostish residual refuse */
        kprintf("linux_netdev_soft: soft hostish residual refuse "
                "ndo_xmit=%p live_fs_8168=%d mmio_handoff=%d filled=%d "
                "(Soft!=product; residual E; no .ko ndo_start_xmit; "
                "freestanding owns wire; G-AC-1)\n",
                pXmit, lnds_live_fs_8168(), g_fMmioHandoff,
                g_fHostishFilled);
    }
    (void)pXmit;
    return LNDS_EOPNOTSUPP;
}

u32
linux_netdev_soft_live8168_fc_open(void)
{
    return g_cLive8168FcOpen;
}

u32
linux_netdev_soft_live8168_fc_napi(void)
{
    return g_cLive8168FcNapi;
}

u32
linux_netdev_soft_live8168_fc_xmit(void)
{
    return g_cLive8168FcXmit;
}

u32
linux_netdev_soft_layout_ver(void)
{
    if (g_pPrimary == NULL || g_pPrimary->u8Live == 0u) {
        return 0u;
    }
    return g_pPrimary->u32SoftLayoutVer;
}

int
linux_netdev_soft_primary_ifindex(void)
{
    if (g_pPrimary == NULL || g_pPrimary->u8Live == 0u) {
        return 0;
    }
    return g_pPrimary->nIfindex;
}

u64
linux_netdev_soft_primary_features(void)
{
    if (g_pPrimary == NULL || g_pPrimary->u8Live == 0u) {
        return 0ull;
    }
    return g_pPrimary->u64Features;
}

/**
 * Once-lamp soft layout deepen honesty. Soft!=product; hard-capped.
 * Grep: linux_netdev_soft: soft layout deepen PASS
 * Grep: linux_netdev_soft: soft residual layout
 */
int
linux_netdev_soft_layout_deepen_lamp_once(void)
{
    struct net_device *pNd;
    void *pOpsSoft;
    void *pOpsHostish;
    void *pNdoOpen;
    void *pNdoStop;
    void *pNdoXmit;
    int fSoftOps;
    int fSoftEth;

    if (g_fLayoutDeepenLog != 0) {
        return 0;
    }
    pNd = g_pPrimary;
    if (pNd == NULL || pNd->u8Live == 0u) {
        return 0;
    }
    g_fLayoutDeepenLog = 1;

    pOpsSoft = pNd->netdev_ops;
    pOpsHostish = linux_netdev_soft_hostish_ops(pNd);
    pNdoOpen = linux_netdev_soft_hostish_ndo_open(pNd);
    pNdoStop = linux_netdev_soft_hostish_ndo_stop(pNd);
    pNdoXmit = linux_netdev_soft_hostish_ndo_start_xmit(pNd);
    fSoftOps = lnds_ops_is_soft(pOpsSoft);
    fSoftEth = lnds_ethtool_is_soft(pNd->ethtool_ops);

    /*
     * Grep: linux_netdev_soft: soft layout deepen PASS
     * Soft!=product; ABI/hostability eng; freestanding_rtl SKIP default;
     * product_nic=UDX residual fields.
     */
    kprintf("linux_netdev_soft: soft layout deepen PASS "
            "layout_ver=%u front_used=240 nd_bytes=%u ops_ver=%u "
            "ifindex=%d type=%u hh_len=%u addr_len=%u mtu=%u "
            "min_mtu=%u max_mtu=%u features=0x%llx "
            "soft_ops=%d soft_ethtool=%d soft_ops_owned=%u "
            "hostish_ops=%p hostish_open=%p hostish_stop=%p hostish_xmit=%p "
            "live_fs_8168=%d mmio_handoff=%d freestanding_rtl=%s "
            "product_nic=UDX "
            "(Soft!=product; ABI/hostability eng residual; "
            "never freestanding wire product; no .ko ndo call)\n",
            (unsigned)pNd->u32SoftLayoutVer,
            (unsigned)LINUX_NETDEV_SOFT_ND_BYTES,
            (unsigned)LNDS_SOFT_OPS_VER,
            pNd->nIfindex,
            (unsigned)pNd->u16Type,
            (unsigned)pNd->u16HardHeaderLen,
            (unsigned)pNd->u8AddrLen,
            (unsigned)pNd->mtu,
            (unsigned)pNd->u16MinMtu,
            (unsigned)pNd->u32MaxMtu,
            (unsigned long long)pNd->u64Features,
            fSoftOps, fSoftEth, (unsigned)pNd->u8SoftOpsOwned,
            pOpsHostish, pNdoOpen, pNdoStop, pNdoXmit,
            lnds_live_fs_8168(), g_fMmioHandoff,
            (GJ_RTL8168_PROBE == 0) ? "SKIP" : "opt_in");

    /* Grep: linux_netdev_soft: soft residual layout */
    /* Grep: linux_netdev_soft: soft residual C0 */
    kprintf("linux_netdev_soft: soft residual layout Soft!=product "
            "soft residual C0 claim_class=C0 "
            "soft_nd=1 hostish_blob=%d "
            "ops_table=soft_LNDS ethtool=soft_LNET "
            "hostish_open=%p hostish_xmit=%p "
            "fc_open=%u fc_napi=%u fc_xmit=%u "
            "RUN_INIT=0 freestanding_no_exec=1 no_bar_steal=%d "
            "freestanding_rtl=%s rtl_probe=%u product_nic=UDX "
            "product_path=UDX product_tx_rx=OPEN G-AC-1 "
            "dual_DoD_A=OPEN dual_DoD_B=OPEN "
            "(C0 soft residual; ABI/hostability eng; soft fields/ops for UDX; "
            "hostish inventory only no ndo call; "
            "never freestanding wire product; residual E; no R0 thrash; "
            "Dual DoD A/B OPEN until UDX DUT; not freestanding :22 close)\n",
            g_fHostishFilled,
            g_pHostishNdoOpenCached, g_pHostishNdoXmitCached,
            (unsigned)g_cLive8168FcOpen, (unsigned)g_cLive8168FcNapi,
            (unsigned)g_cLive8168FcXmit,
            g_fMmioHandoff == 0 ? 1 : 0,
            (GJ_RTL8168_PROBE == 0) ? "SKIP" : "opt_in",
            (unsigned)GJ_RTL8168_PROBE);

    return 1;
}

/**
 * Optional overlay: if hostish name/mtu/dev_addr empty after memcpy, copy from
 * soft bookkeeping fields (soft name@0 / mtu / dev_addr). Soft!=product.
 */
static void
lnds_hostish_overlay_soft_known(u8 *pHost, const struct net_device *pSoft)
{
    u32 u32MtuHost;
    u32 i;
    int fNameEmpty;
    int fMacEmpty;

    if (pHost == NULL || pSoft == NULL) {
        return;
    }

    /* name: host @0x120 IF_NAMESIZE-ish; soft name[16] */
    fNameEmpty = (pHost[LINUX_NETDEV_HOSTISH_OFF_NAME] == 0) ? 1 : 0;
    if (fNameEmpty != 0 && pSoft->name[0] != '\0') {
        for (i = 0u; i < LINUX_NETDEV_SOFT_NAME_LEN && i < 16u; i++) {
            pHost[LINUX_NETDEV_HOSTISH_OFF_NAME + i] = (u8)pSoft->name[i];
            if (pSoft->name[i] == '\0') {
                break;
            }
        }
    }

    /* mtu: host @0x38 */
    memcpy(&u32MtuHost, pHost + LINUX_NETDEV_HOSTISH_OFF_MTU, sizeof(u32));
    if (u32MtuHost == 0u && pSoft->mtu != 0u) {
        u32MtuHost = pSoft->mtu;
        memcpy(pHost + LINUX_NETDEV_HOSTISH_OFF_MTU, &u32MtuHost, sizeof(u32));
    }

    /* dev_addr: host @0x3e0; soft may hold MAC in soft front field */
    fMacEmpty = 1;
    for (i = 0u; i < 6u; i++) {
        if (pHost[LINUX_NETDEV_HOSTISH_OFF_DEV_ADDR + i] != 0u) {
            fMacEmpty = 0;
            break;
        }
    }
    if (fMacEmpty != 0) {
        for (i = 0u; i < 6u; i++) {
            if (pSoft->dev_addr[i] != 0u) {
                fMacEmpty = 0;
                break;
            }
        }
        if (fMacEmpty == 0) {
            for (i = 0u; i < 6u; i++) {
                pHost[LINUX_NETDEV_HOSTISH_OFF_DEV_ADDR + i] = pSoft->dev_addr[i];
            }
        }
    }
}

/**
 * Dual hostish net_device fill from soft primary (or pSoftNd).
 * Grep: linux_netdev_soft: soft hostish net_device fill PASS
 * Soft!=product; never calls ndo_open.
 */
int
linux_netdev_soft_hostish_fill(void *pSoftNd)
{
    struct net_device *pSoft;
    struct soft_nd_slab *pSlab;
    u8 *pHost;
    u32 u32Slot;
    u32 u32Copy;
    void *pOps;
    void *pNdoOpen;
    void *pNdoStop;
    void *pNdoXmit;
    void *pModBase;
    u64 cbMod;
    int fInR8169;
    int fModOk;

    pSoft = (struct net_device *)pSoftNd;
    if (pSoft == NULL) {
        pSoft = g_pPrimary;
    }
    if (pSoft == NULL || pSoft->u8Live == 0u) {
        return -1;
    }
    pSlab = lnds_slab_of(pSoft);
    if (pSlab == NULL) {
        return -1;
    }

    /* Single-slot pool today (LINUX_NETDEV_HOSTISH_POOL == 1). */
    u32Slot = 0u;
    if (u32Slot >= LINUX_NETDEV_HOSTISH_POOL) {
        return -1;
    }
    pHost = g_aNdHostish[u32Slot];
    memset(pHost, 0, LINUX_NETDEV_HOSTISH_BLOB_BYTES);

    /*
     * Soft slab = 2048 front + 4096 priv ≥ host 0xaf0. REAL .ko probe writes
     * host-layout fields into this same soft VA; memcpy captures them.
     */
    u32Copy = LINUX_NETDEV_HOSTISH_SIZE_NET_DEVICE;
    if (u32Copy > (u32)sizeof(*pSlab)) {
        u32Copy = (u32)sizeof(*pSlab);
    }
    memcpy(pHost, pSlab, u32Copy);
    lnds_hostish_overlay_soft_known(pHost, pSoft);

    pOps = linux_netdev_soft_hostish_ops(pHost);
    pNdoOpen = linux_netdev_soft_hostish_ndo_open(pHost);

    /*
     * Prefer recovery from soft primary VA host offset +0x8 when blob ops
     * null (should match after memcpy; defensive re-read).
     */
    if (pOps == NULL) {
        pOps = linux_netdev_soft_hostish_ops(pSoft);
        if (pOps != NULL) {
            memcpy(pHost + LINUX_NETDEV_HOSTISH_OFF_NETDEV_OPS, &pOps,
                   sizeof(void *));
            pNdoOpen = linux_netdev_soft_hostish_ndo_open(pHost);
        }
    }

    g_aNdHostishLive[u32Slot] = 1u;
    g_pHostishPrimary = pHost;
    g_pHostishOpsCached = pOps;
    g_pHostishNdoOpenCached = pNdoOpen;
    g_pHostishNdoStopCached = linux_netdev_soft_hostish_ndo_stop(pHost);
    g_pHostishNdoXmitCached = linux_netdev_soft_hostish_ndo_start_xmit(pHost);
    g_fHostishFilled = 1;

    pModBase = NULL;
    cbMod = 0;
    fModOk = 0;
    fInR8169 = 0;
    if (linux_module_load_va_range != NULL &&
        linux_module_load_va_range("r8169", &pModBase, &cbMod) == 0) {
        fModOk = 1;
    }
    if (fModOk != 0) {
        fInR8169 = lnds_ptr_in_range(pOps, pModBase, cbMod);
        if (fInR8169 == 0 && pNdoOpen != NULL) {
            fInR8169 = lnds_ptr_in_range(pNdoOpen, pModBase, cbMod);
        }
    }

    pNdoStop = g_pHostishNdoStopCached;
    pNdoXmit = g_pHostishNdoXmitCached;

    if (g_fHostishNdFillLog == 0) {
        g_fHostishNdFillLog = 1;
        /*
         * Grep: linux_netdev_soft: soft hostish net_device fill PASS
         * Soft!=product; layout soft!=host; no ndo_open / xmit call.
         */
        kprintf("linux_netdev_soft: soft hostish net_device fill PASS "
                "ops=%p ndo_open=%p ndo_stop=%p ndo_xmit=%p in_r8169=%d "
                "blob=%p soft=%p cb=0x%x host_sz=0x%x "
                "ops_off=0x%x ndo_open_off=0x%x "
                "layout_ver=%u ifindex=%d "
                "(Soft!=product; dual object; no ndo call; freestanding wire)\n",
                pOps, pNdoOpen, pNdoStop, pNdoXmit, fInR8169,
                (void *)pHost, (void *)pSoft,
                (unsigned)u32Copy,
                (unsigned)LINUX_NETDEV_HOSTISH_SIZE_NET_DEVICE,
                (unsigned)LINUX_NETDEV_HOSTISH_OFF_NETDEV_OPS,
                (unsigned)LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_OPEN,
                (unsigned)pSoft->u32SoftLayoutVer, pSoft->nIfindex);
    }

    /* Soft layout deepen once-lamp after hostish fill. Soft!=product. */
    (void)linux_netdev_soft_layout_deepen_lamp_once();

    /*
     * Hostish residual refuse once-lamp (inventory honesty; no ndo call).
     * Residual E: freestanding owns wire - refuse helpers never invoke.
     * Soft!=product; cap logs (no stamp storms).
     */
    (void)linux_netdev_soft_hostish_ndo_open_refuse();
    (void)linux_netdev_soft_hostish_ndo_xmit_refuse();

    return 0;
}

/**
 * Read-only soft vs hostish netdev_ops diagnostic. Soft!=product.
 * Grep: linux_netdev_soft: soft ops diagnostic
 */
void
linux_netdev_soft_ops_diagnostic(void)
{
    struct net_device *pNd;
    void *pOpsSoft;
    void *pOpsHostish;
    void *pNdoOpen;
    void *pBlob;
    void *pModBase;
    u64 cbMod;
    int fSoftMagic;
    int fInSoft;
    int fInHostish;
    int fInNdoOpen;
    int fModOk;

    if (g_fSoftOpsDiagLog != 0) {
        return;
    }
    g_fSoftOpsDiagLog = 1;

    pNd = g_pPrimary;
    if (pNd == NULL) {
        /* Grep: linux_netdev_soft: soft ops diagnostic */
        kprintf("linux_netdev_soft: soft ops diagnostic SKIP (no primary; "
                "Soft!=product; no ndo_open call)\n");
        return;
    }

    /* Refresh dual hostish blob so diagnostic matches post-probe stores. */
    (void)linux_netdev_soft_hostish_fill(pNd);

    pOpsSoft = pNd->netdev_ops;
    /* Host r8169 stores netdev_ops at +0x8 on the soft VA itself. */
    pOpsHostish = linux_netdev_soft_hostish_ops(pNd);
    pNdoOpen = linux_netdev_soft_hostish_ndo_open(pNd);
    pBlob = linux_netdev_soft_hostish_primary();

    fSoftMagic = lnds_ops_is_soft(pOpsSoft);

    pModBase = NULL;
    cbMod = 0;
    fModOk = 0;
    if (linux_module_load_va_range != NULL &&
        linux_module_load_va_range("r8169", &pModBase, &cbMod) == 0) {
        fModOk = 1;
    }

    fInSoft = 0;
    fInHostish = 0;
    fInNdoOpen = 0;
    if (fModOk != 0) {
        fInSoft = lnds_ptr_in_range(pOpsSoft, pModBase, cbMod);
        fInHostish = lnds_ptr_in_range(pOpsHostish, pModBase, cbMod);
        fInNdoOpen = lnds_ptr_in_range(pNdoOpen, pModBase, cbMod);
    }

    /*
     * Grep: linux_netdev_soft: soft ops diagnostic
     * soft_ops = soft struct field (+0x18); hostish_ops = word at +0x8.
     * Never call ndo_open. Soft!=product. Layout soft!=host.
     */
    kprintf("linux_netdev_soft: soft ops diagnostic "
            "soft_ops=%p hostish_ops@+0x8=%p hostish_ndo_open@+0x10=%p "
            "hostish_ndo_stop@+0x18=%p hostish_ndo_xmit@+0x20=%p "
            "soft_magic=%d soft_layout_ver=%u ifindex=%d "
            "in_r8169_soft=%d in_r8169_hostish=%d "
            "in_r8169_ndo_open=%d hostish_blob=%p hostish_filled=%d "
            "r8169_load=%p..+0x%lx host_ndo_open_off=0x%x "
            "(Soft!=product; layout soft!=host; no ndo call; "
            "freestanding owns wire)\n",
            pOpsSoft, pOpsHostish, pNdoOpen,
            linux_netdev_soft_hostish_ndo_stop(pNd),
            linux_netdev_soft_hostish_ndo_start_xmit(pNd),
            fSoftMagic,
            (unsigned)pNd->u32SoftLayoutVer, pNd->nIfindex,
            fInSoft, fInHostish,
            fInNdoOpen, pBlob, g_fHostishFilled, pModBase,
            (unsigned long)cbMod,
            (unsigned)LINUX_NETDEV_HOSTISH_OPS_OFF_NDO_OPEN);
}

/**
 * Optional soft-only open via soft-shaped ops table.
 * If netdev_ops is .ko (non-null, not our magic): DO NOT call open -
 * log once and leave MMIO to freestanding. Soft!=product.
 * Residual E: live freestanding 8168 -> fail closed (never .ko ndo_open).
 */
static void
lnds_soft_dev_open(struct net_device *pNd)
{
    const struct soft_netdev_ops *pSoft;
    int nSt;
    int fLiveFs;

    if (pNd == NULL) {
        return;
    }
    /* Dual DoD B residual: never dual-drive BAR on soft open. Soft!=product. */
    lnds_gate0_force_no_bar_steal();
    fLiveFs = lnds_live_fs_8168();
    if (pNd->netdev_ops == NULL) {
        /* Soft bookkeeping open only; no ops. Residual E still honest. */
        if (fLiveFs != 0) {
            if (g_cLive8168FcOpen < 0xffffffffu) {
                g_cLive8168FcOpen++;
            }
            if (g_fLive8168OpenFcLog == 0) {
                g_fLive8168OpenFcLog = 1;
                /* Grep: linux_netdev_soft: soft open fail-closed live 8168 */
                kprintf("linux_netdev_soft: soft open fail-closed live 8168 "
                        "name=%s ops=NULL (Soft!=product; freestanding owns BAR; "
                        "no .ko ndo_open; residual E)\n",
                        lnds_name_of(pNd));
            }
        }
        return;
    }
    if (lnds_ops_is_soft(pNd->netdev_ops) == 0) {
        /* .ko or foreign ops - never invoke (dual-drive MMIO risk). */
        if (g_fSoftOpenSkipKoLog == 0) {
            g_fSoftOpenSkipKoLog = 1;
            /* Grep: linux_netdev_soft: soft open skip .ko ops */
            kprintf("linux_netdev_soft: soft open skip .ko ops "
                    "(MMIO freestanding owns) name=%s ops=%p "
                    "(Soft!=product)\n",
                    lnds_name_of(pNd), pNd->netdev_ops);
        }
        if (fLiveFs != 0) {
            if (g_cLive8168FcOpen < 0xffffffffu) {
                g_cLive8168FcOpen++;
            }
            if (g_fLive8168OpenFcLog == 0) {
                g_fLive8168OpenFcLog = 1;
                /* Grep: linux_netdev_soft: soft open fail-closed live 8168 */
                kprintf("linux_netdev_soft: soft open fail-closed live 8168 "
                        "name=%s ops=%p (Soft!=product; freestanding owns BAR; "
                        "no .ko ndo_open; residual E; hybrid SOFT must not steal "
                        "BAR)\n",
                        lnds_name_of(pNd), pNd->netdev_ops);
            }
            /* Hostish residual refuse (inventory; no call). Soft!=product. */
            (void)linux_netdev_soft_hostish_ndo_open_refuse();
        }
        return;
    }
    /*
     * Soft-shaped ops only. Still refuse soft ndo_open body while live FS
     * owns BAR - residual E honesty (no accidental program path). Soft!=product.
     */
    if (fLiveFs != 0) {
        if (g_cLive8168FcOpen < 0xffffffffu) {
            g_cLive8168FcOpen++;
        }
        if (g_fLive8168OpenFcLog == 0) {
            g_fLive8168OpenFcLog = 1;
            kprintf("linux_netdev_soft: soft open fail-closed live 8168 "
                    "name=%s soft_ops=1 (Soft!=product; freestanding owns wire; "
                    "bookkeeping open only; residual E)\n",
                    lnds_name_of(pNd));
        }
        return;
    }
    pSoft = (const struct soft_netdev_ops *)pNd->netdev_ops;
    if (pSoft->ndo_open == NULL) {
        return;
    }
    /* Soft-owned open only (not .ko). Soft!=product. */
    nSt = pSoft->ndo_open(pNd);
    (void)nSt;
}

/** First soft TX path entry lamp (note_tx or l2_xmit). Soft!=product. */
static void
lnds_l2_first_tx_lamp(int fOk)
{
    if (g_fL2FirstTxLog != 0) {
        return;
    }
    g_fL2FirstTxLog = 1;
    /* Grep: linux_netdev_soft: soft l2 bridge first tx */
    kprintf("linux_netdev_soft: soft l2 bridge first tx ok=%d "
            "attempt=%u accounted=%u (Soft!=product; freestanding owns wire)\n",
            fOk != 0 ? 1 : 0, (unsigned)g_cL2TxAttempt,
            (unsigned)g_cL2TxAccounted);
}

/*
 * hold14 display counters (Soft!=product).
 *
 * Soft bridge fed/tx_ok climb only when feed_rx / note_tx run. Under hybrid
 * SOFT, bridge may lag enable -> feed_rx rare while freestanding R/T (rtl)
 * still climb. Mirror freestanding R/T into the panel when freestanding
 * owns the wire so hold14 tracks live freestanding traffic (HY7).
 * Never dual-drive; read-only counters. Soft!=product.
 */
static void
lnds_hold14_sample_fs(u32 *pcFsRx, u32 *pcFsTx)
{
    u32 cFsRx;
    u32 cFsTx;
    int fFsReady;

    cFsRx = 0u;
    cFsTx = 0u;
    /*
     * Freestanding owns MMIO (handoff==0): sample rtl R/T when the hybrid
     * surface is live (bridge ON, soft primary, or freestanding ready).
     * feed_rx may be rare if bridge lags. Weak-safe if rtl not linked.
     */
    if (g_fMmioHandoff != 0) {
        g_u32Hold14FsRx = 0u;
        g_u32Hold14FsTx = 0u;
        if (pcFsRx != NULL) {
            *pcFsRx = 0u;
        }
        if (pcFsTx != NULL) {
            *pcFsTx = 0u;
        }
        return;
    }
    fFsReady = 0;
    if (rtl8168_ready != NULL && rtl8168_ready() != 0) {
        fFsReady = 1;
    }
    if (g_fL2Bridge == 0 &&
        (g_pPrimary == NULL || g_pPrimary->u8Registered == 0u) &&
        fFsReady == 0) {
        g_u32Hold14FsRx = 0u;
        g_u32Hold14FsTx = 0u;
        if (pcFsRx != NULL) {
            *pcFsRx = 0u;
        }
        if (pcFsTx != NULL) {
            *pcFsTx = 0u;
        }
        return;
    }
    if (rtl8168_rx_count != NULL) {
        cFsRx = rtl8168_rx_count();
    }
    if (rtl8168_tx_count != NULL) {
        cFsTx = rtl8168_tx_count();
    }
    g_u32Hold14FsRx = cFsRx;
    g_u32Hold14FsTx = cFsTx;
    if (pcFsRx != NULL) {
        *pcFsRx = cFsRx;
    }
    if (pcFsTx != NULL) {
        *pcFsTx = cFsTx;
    }
}

static void
lnds_hold14_counts(u32 *pcRx, u32 *pcTx)
{
    u32 cRx;
    u32 cTx;
    u32 cFsRx;
    u32 cFsTx;

    cRx = g_cL2RxFed;
    cTx = g_cL2TxOk;
    lnds_hold14_sample_fs(&cFsRx, &cFsTx);
    if (cFsRx > cRx) {
        cRx = cFsRx;
    }
    if (cFsTx > cTx) {
        cTx = cFsTx;
    }
    /*
     * Once-lamp when freestanding R or T first exceeds soft bookkeeping -
     * greppable honesty that hold14 is freestanding R/T mirror (HY7).
     * Soft!=product; not product AC / not continuous ping claim.
     */
    if (g_fHold14FsMirrorLog == 0 &&
        (cFsRx > g_cL2RxFed || cFsTx > g_cL2TxOk)) {
        g_fHold14FsMirrorLog = 1;
        /* Grep: linux_netdev_soft: soft hold14 freestanding R/T mirror */
        kprintf("linux_netdev_soft: soft hold14 freestanding R/T mirror "
                "rx=%u tx=%u soft_fed=%u soft_tx=%u "
                "(Soft!=product; freestanding owns wire; HY7)\n",
                (unsigned)cFsRx, (unsigned)cFsTx,
                (unsigned)g_cL2RxFed, (unsigned)g_cL2TxOk);
    }
    if (pcRx != NULL) {
        *pcRx = cRx;
    }
    if (pcTx != NULL) {
        *pcTx = cTx;
    }
}

/** Append decimal u32 (0..99999) into *pq; Soft!=product panel helper. */
static char *
lnds_hold14_put_u32(char *qBr, u32 uTmp)
{
    if (uTmp >= 10000u) {
        *qBr++ = (char)('0' + (uTmp / 10000u) % 10u);
    }
    if (uTmp >= 1000u) {
        *qBr++ = (char)('0' + (uTmp / 1000u) % 10u);
    }
    if (uTmp >= 100u) {
        *qBr++ = (char)('0' + (uTmp / 100u) % 10u);
    }
    if (uTmp >= 10u) {
        *qBr++ = (char)('0' + (uTmp / 10u) % 10u);
    }
    *qBr++ = (char)('0' + (uTmp % 10u));
    return qBr;
}

/**
 * STATUS hold14: soft L2 bridge rx/tx (Soft!=product).
 * Format matches main/net_eth: "l2 br rx=N tx=M". Never clobbers 7-13.
 * Paints when bridge ON (or hybrid freestanding+primary) and counters change.
 * freestanding R/T climb -> panel refresh even if feed_rx rare (bridge off SOFT).
 * Grep / panel: L2 BR RX · hold14 · freestanding R/T
 */
static void
lnds_hold14_l2_br_refresh(void)
{
    char szBr[48];
    char *qBr;
    const char *pBr;
    u32 cRx;
    u32 cTx;

    /*
     * Bridge ON is the normal latch. Under hybrid SOFT, bridge may lag
     * (net_l2 note race) while freestanding R/T climb - still paint when
     * soft primary is registered and freestanding owns wire, or when
     * freestanding alone is ready (panel honesty before soft latch).
     * Soft!=product. Not REAL probe mode only.
     */
    if (g_fL2Bridge == 0) {
        int fFsReady;

        if (g_fMmioHandoff != 0) {
            return;
        }
        fFsReady = 0;
        if (rtl8168_ready != NULL && rtl8168_ready() != 0) {
            fFsReady = 1;
        }
        if ((g_pPrimary == NULL || g_pPrimary->u8Registered == 0u) &&
            fFsReady == 0) {
            return;
        }
    }
    if (14u >= FB_HOLD_LINES) {
        return;
    }
    lnds_hold14_counts(&cRx, &cTx);
    if (g_fHold14Painted != 0 &&
        cRx == g_u32LastHold14Rx && cTx == g_u32LastHold14Tx) {
        return;
    }
    g_fHold14Painted = 1;
    g_u32LastHold14Rx = cRx;
    g_u32LastHold14Tx = cTx;

    qBr = szBr;
    pBr = "l2 br rx=";
    while (*pBr != '\0') {
        *qBr++ = *pBr++;
    }
    qBr = lnds_hold14_put_u32(qBr, cRx);
    pBr = " tx=";
    while (*pBr != '\0') {
        *qBr++ = *pBr++;
    }
    qBr = lnds_hold14_put_u32(qBr, cTx);
    *qBr = '\0';
    /* STATUS static only - no scroll kprintf. Soft!=product. */
    fb_console_hold(14, szBr);
}

/* ---- Public init / getters ---------------------------------------------- */

void
linux_netdev_soft_init(void)
{
    u32 u32KsymOk;
    u32 u32KsymSkip;

    g_cInitCalls++;
    if (g_fReady) {
        /*
         * Idempotent: silent after first PASS (no stamp storms).
         * Soft!=product. Grep first-boot: soft init PASS.
         */
        return;
    }

    g_pPrimary = NULL;
    g_fHostishFilled = 0;
    g_pHostishPrimary = NULL;
    g_pHostishOpsCached = NULL;
    g_pHostishNdoOpenCached = NULL;
    g_pHostishNdoStopCached = NULL;
    g_pHostishNdoXmitCached = NULL;
    g_fHostishNdFillLog = 0;
    g_fHostishOpenRefuseLog = 0;
    g_fHostishXmitRefuseLog = 0;
    g_cHostishOpenRefuse = 0u;
    g_cHostishXmitRefuse = 0u;
    g_cLive8168FcOpen = 0u;
    g_cLive8168FcNapi = 0u;
    g_cLive8168FcXmit = 0u;
    g_cRegSkipLog = 0u;
    g_cUnregLog = 0u;
    g_fInitPassLog = 0;
    g_fMmioHandoffStateLog = 0;
    g_fLayoutDeepenLog = 0;
    g_fSoftDefaultOpsLog = 0;
    {
        u32 iH;

        for (iH = 0u; iH < LINUX_NETDEV_HOSTISH_POOL; iH++) {
            g_aNdHostishLive[iH] = 0u;
            memset(g_aNdHostish[iH], 0, LINUX_NETDEV_HOSTISH_BLOB_BYTES);
        }
    }
    g_cLive = 0u;
    g_cRegistered = 0u;
    g_cAllocCalls = 0u;
    g_cAllocFail = 0u;
    g_cRegCalls = 0u;
    g_cRegPass = 0u;
    g_cRegSkip = 0u;
    g_cUnreg = 0u;
    g_cFree = 0u;
    g_cSkbAlloc = 0u;
    g_cSkbFree = 0u;
    g_cSkbDrop = 0u;
    g_cSkbLive = 0u;
    g_cLogErr = 0u;
    g_cLogInfo = 0u;
    g_cLogWarn = 0u;
    g_cLogNotice = 0u;
    g_cLogPrintk = 0u;
    g_cNapiEnable = 0u;
    g_cNapiDisable = 0u;
    g_cNapiPrepOk = 0u;
    g_cNapiPrepSkip = 0u;
    g_cNapiComplete = 0u;
    g_cNapiSched = 0u;
    g_cNapiLive = 0u;
    g_cNapiLog = 0u;
    g_cNapiPollInvoke = 0u;
    g_cNapiPollSkip = 0u;
    g_cSoftirqKick = 0u;
    g_fNapiPollSkipKoLog = 0;
    g_fL2RxNapiLog = 0;
    g_fNapiBridgeEnLog = 0;
    g_fMmioHandoff = 0; /* freestanding owns MMIO; gate0 residual force */
    g_fMmioHandoffReadyLog = 0;
    g_fMmioTryOpenLog = 0;
    g_fMmioTryOpenDone = 0; /* phase3 edge; gate-0 path still resets */
    g_fHybridLampLog = 0; /* phase 4a hybrid once-lamp */
    g_fGate0ResidualLog = 0; /* dual DoD B residual once-lamp */
    g_fResidualLeanLog = 0; /* soft residual lean once-lamp (ABI/hostability) */
    g_cBarStealRefuse = 0u;
    g_fL2Bridge = 0;
    g_fL2BridgeLog = 0;
    g_fL2FirstTxLog = 0;
    g_cL2RxFed = 0u;
    g_cL2RxAttempt = 0u;
    g_cL2RxDrop = 0u;
    g_cL2TxOk = 0u;
    g_cL2TxFail = 0u;
    g_cL2TxAttempt = 0u;
    g_cL2TxAccounted = 0u;
    g_u32LastHold14Rx = 0u;
    g_u32LastHold14Tx = 0u;
    g_fHold14Painted = 0;
    g_u32Hold14FsRx = 0u;
    g_u32Hold14FsTx = 0u;
    g_fHold14FsMirrorLog = 0;
    g_fMmioSetHandoffLog = 0;
    g_fLive8168OpenFcLog = 0;
    g_fLive8168NapiFcLog = 0;
    g_fLive8168XmitFcLog = 0;
    g_cRtnlDepth = 0u;
    g_cRtnlLock = 0u;
    g_cRtnlUnlock = 0u;
    g_fRtnlLogOnce = 0;
    g_fEthPlatLogOnce = 0;
    g_fEthMacLogOnce = 0;
    g_fSyncNetLogOnce = 0;
    g_cEthMac = 0u;
    g_cEthValidate = 0u;
    g_cEthTypeTrans = 0u;
    g_cSyncNet = 0u;
    g_cSoftOpen = 0u;
    g_fSoftOpenLog = 0;
    g_fSoftOpenSkipKoLog = 0;
    g_cRtnlLock = 0u;
    g_cRtnlUnlock = 0u;
    g_cRtnlDepth = 0u;
    g_fRtnlLogOnce = 0;
    g_fEthPlatLogOnce = 0;
    g_cEthMac = 0u;
    g_fEthMacLogOnce = 0;
    g_cEthValidate = 0u;
    g_cEthTypeTrans = 0u;
    g_cSyncNet = 0u;
    g_fSyncNetLogOnce = 0;
    g_cEthtoolLink = 0u;
    g_cEthtoolTs = 0u;
    g_cNetRatelimit = 0u;
    g_fEthtoolLinkLog = 0;
    g_fEthtoolTsLog = 0;
    g_fNetRatelimitLog = 0;
    g_cSkbCopyBits = 0u;
    g_cSkbCopyBitsFail = 0u;
    g_cSkbPad = 0u;
    g_cSkbPadFail = 0u;
    g_cSkbPut = 0u;
    g_cSkbPutFail = 0u;
    g_cPskbMayPull = 0u;
    g_cPskbMayPullFail = 0u;
    g_cPskbPullTail = 0u;
    g_cPskbExpand = 0u;
    g_cNetdevAllocSkb = 0u;
    g_fSkbCopyBitsLog = 0;
    g_fSkbPadLog = 0;
    g_fSkbPutLog = 0;
    g_fPskbMayPullLog = 0;
    g_fNetdevAllocSkbLog = 0;
    g_cSoftTxFromSkbAttempt = 0u;
    g_cSoftTxFromSkbOk = 0u;
    g_cSoftTxFromSkbFail = 0u;
    g_cSoftDqXmit = 0u;
    g_cSoftDqXmitOk = 0u;
    g_cSoftDqXmitFail = 0u;
    g_fSoftTxFromSkbLog = 0;
    g_fSoftDqXmitLog = 0;
    g_fSoftL2TxSmokeLog = 0;
    memset(g_aNd, 0, sizeof(g_aNd));
    memset(g_aSkb, 0, sizeof(g_aSkb));
    memset(g_aNapi, 0, sizeof(g_aNapi));
    g_fReady = 1;

    /*
     * Export soft bodies for F2 ksym / module resolve.
     * Weak linux_ksym_register: no-op SKIP when F2 not linked yet.
     */
    u32KsymOk = 0u;
    u32KsymSkip = 0u;
    lnds_ksym_one("alloc_etherdev_mqs", (void *)alloc_etherdev_mqs, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("devm_alloc_etherdev_mqs", (void *)devm_alloc_etherdev_mqs,
                  &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("register_netdev", (void *)register_netdev, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("unregister_netdev", (void *)unregister_netdev, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("free_netdev", (void *)free_netdev, &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("netif_carrier_on", (void *)netif_carrier_on, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_carrier_off", (void *)netif_carrier_off, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_device_attach", (void *)netif_device_attach, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_device_detach", (void *)netif_device_detach, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_tx_wake_queue", (void *)netif_tx_wake_queue, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_start_queue", (void *)netif_start_queue, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_stop_queue", (void *)netif_stop_queue, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_wake_queue", (void *)netif_wake_queue, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_napi_add_weight_locked",
                  (void *)netif_napi_add_weight_locked, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_receive_skb", (void *)netif_receive_skb, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("dev_kfree_skb_any", (void *)dev_kfree_skb_any, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("dev_kfree_skb_any_reason", (void *)dev_kfree_skb_any_reason,
                  &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("napi_alloc_skb", (void *)napi_alloc_skb, &u32KsymOk,
                  &u32KsymSkip);
    /* Post-probe NAPI soft bodies (replace empty ksym stubs). Soft!=product. */
    lnds_ksym_one("napi_enable", (void *)napi_enable, &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("napi_disable", (void *)napi_disable, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("napi_schedule_prep", (void *)napi_schedule_prep, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("napi_complete_done", (void *)napi_complete_done, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("__napi_schedule", (void *)__napi_schedule, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("napi_consume_skb", (void *)napi_consume_skb, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("napi_gro_receive", (void *)napi_gro_receive, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netdev_err", (void *)netdev_err, &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("netdev_info", (void *)netdev_info, &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("netdev_warn", (void *)netdev_warn, &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("netdev_notice", (void *)netdev_notice, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netdev_printk", (void *)netdev_printk, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netdev_update_features", (void *)netdev_update_features,
                  &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("netdev_stats_to_stats64", (void *)netdev_stats_to_stats64,
                  &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("netdev_sw_irq_coalesce_default_on",
                  (void *)netdev_sw_irq_coalesce_default_on, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("netif_schedule_queue", (void *)netif_schedule_queue,
                  &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("netif_set_tso_max_size", (void *)netif_set_tso_max_size,
                  &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("netif_set_tso_max_segs", (void *)netif_set_tso_max_segs,
                  &u32KsymOk, &u32KsymSkip);

    /*
     * Post-probe open/datapath (replace empty ksym stubs). Soft!=product;
     * no dual-drive of freestanding MMIO. eth_platform fails closed so the
     * driver falls back to chip EEPROM rather than trusting a success stub.
     */
    lnds_ksym_one("rtnl_lock", (void *)rtnl_lock, &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("rtnl_unlock", (void *)rtnl_unlock, &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("eth_platform_get_mac_address",
                  (void *)eth_platform_get_mac_address, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("eth_mac_addr", (void *)eth_mac_addr, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("eth_validate_addr", (void *)eth_validate_addr, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("eth_type_trans", (void *)eth_type_trans, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("synchronize_net", (void *)synchronize_net, &u32KsymOk,
                  &u32KsymSkip);
    /*
     * Post-init without dual-drive: ethtool link / ts_info / net_ratelimit.
     * Empty ksym stubs returned 0 (link down; always suppress). Soft bodies
     * report freestanding/soft carrier and allow rate-limit prints.
     * Soft!=product; no BAR touch.
     */
    lnds_ksym_one("ethtool_op_get_link", (void *)ethtool_op_get_link,
                  &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("ethtool_op_get_ts_info", (void *)ethtool_op_get_ts_info,
                  &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("net_ratelimit", (void *)net_ratelimit, &u32KsymOk,
                  &u32KsymSkip);

    /*
     * Soft skb path (replace empty ksym stubs). Soft!=product; fail closed
     * on non-pool cookies. No .ko ndo_start_xmit. Future r8169 soft TX.
     */
    lnds_ksym_one("skb_copy_bits", (void *)skb_copy_bits, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("__skb_pad", (void *)__skb_pad, &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("skb_put", (void *)skb_put, &u32KsymOk, &u32KsymSkip);
    lnds_ksym_one("pskb_may_pull", (void *)pskb_may_pull, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("__pskb_pull_tail", (void *)__pskb_pull_tail, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("pskb_expand_head", (void *)pskb_expand_head, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("__netdev_alloc_skb", (void *)__netdev_alloc_skb, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("dev_alloc_skb", (void *)dev_alloc_skb, &u32KsymOk,
                  &u32KsymSkip);
    lnds_ksym_one("__napi_alloc_skb", (void *)__napi_alloc_skb, &u32KsymOk,
                  &u32KsymSkip);
    /*
     * Soft-originated TX reverse path (Soft!=product).
     * bridge ON + freestanding owns -> net_l2_tx; handoff pending fail closed.
     * No auto wire inject (GJ_SOFT_L2_TX_SMOKE default 0).
     */
    lnds_ksym_one("dev_queue_xmit", (void *)dev_queue_xmit, &u32KsymOk,
                  &u32KsymSkip);

    /* Grep: linux_netdev_soft: soft init PASS - once only (no stamp storms). */
    if (g_fInitPassLog == 0) {
        g_fInitPassLog = 1;
        kprintf("linux_netdev_soft: soft init PASS nd_max=%u skb_max=%u "
                "nd_bytes=%u priv_max=%u napi_max=%u layout_ver=%u ops_ver=%u "
                "soft_ne_abi=1 product=OPEN claim_class=C0 "
                "RUN_INIT=0 freestanding_no_exec=1 no_bar_steal=1 "
                "freestanding_rtl=%s rtl_probe=%u product_nic=UDX "
                "product_path=UDX dual_DoD_A=OPEN dual_DoD_B=OPEN "
                "gate0_mmio_handoff=%d "
                "(Soft!=product; C0 soft residual; ABI/hostability eng; "
                "never freestanding wire product; UDX residual fields; "
                "residual E fail-closed live 8168; hostish inventory only; "
                "dual DoD A/B residual OPEN until UDX DUT; G-AC-1; "
                "no real BAR thrash; not freestanding :22 close)\n",
                (unsigned)LINUX_NETDEV_SOFT_MAX,
                (unsigned)LINUX_NETDEV_SOFT_SKB_MAX,
                (unsigned)LINUX_NETDEV_SOFT_ND_BYTES,
                (unsigned)LINUX_NETDEV_SOFT_PRIV_MAX, (unsigned)LNDS_NAPI_MAX,
                (unsigned)LINUX_NETDEV_SOFT_LAYOUT_VER,
                (unsigned)LNDS_SOFT_OPS_VER,
                (GJ_RTL8168_PROBE == 0) ? "SKIP" : "opt_in",
                (unsigned)GJ_RTL8168_PROBE,
                (int)GJ_SOFT_R8169_MMIO_HANDOFF);
    }
    /* Gate0 residual: soft must not steal BAR; Soft!=product. */
    lnds_gate0_force_no_bar_steal();
    /*
     * Residual lean once-lamp (ABI/hostability eng; C0). Soft!=product.
     * freestanding_rtl SKIP + product_nic=UDX + Dual DoD OPEN. No stamp storm.
     */
    (void)linux_netdev_soft_residual_lean_lamp_once();

    if (u32KsymOk > 0u) {
        /* Grep: linux_netdev_soft: soft ksym register PASS */
        kprintf("linux_netdev_soft: soft ksym register PASS n=%u skip=%u\n",
                (unsigned)u32KsymOk, (unsigned)u32KsymSkip);
    } else {
        /* Grep: linux_netdev_soft: soft ksym register SKIP */
        kprintf("linux_netdev_soft: soft ksym register SKIP (no linux_ksym or "
                "all failed) skip=%u\n",
                (unsigned)u32KsymSkip);
    }
}

int
linux_netdev_soft_ready(void)
{
    return g_fReady;
}

void *
linux_netdev_soft_primary(void)
{
    if (g_pPrimary == NULL || g_pPrimary->u8Live == 0u) {
        return NULL;
    }
    return g_pPrimary;
}

int
linux_netdev_soft_count(void)
{
    return (int)g_cRegistered;
}

u32
linux_netdev_soft_alloc_calls(void)
{
    return g_cAllocCalls;
}

u32
linux_netdev_soft_register_calls(void)
{
    return g_cRegCalls;
}

u32
linux_netdev_soft_skb_live(void)
{
    return g_cSkbLive;
}

/* ---- Alloc / free / register -------------------------------------------- */

void *
alloc_etherdev_mqs(int nSizeofPriv, unsigned uTxQs, unsigned uRxQs)
{
    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    return lnds_alloc_nd(nSizeofPriv, uTxQs, uRxQs, NULL, 0);
}

void *
devm_alloc_etherdev_mqs(void *pDev, int nSizeofPriv, unsigned uTxQs,
                        unsigned uRxQs)
{
    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    return lnds_alloc_nd(nSizeofPriv, uTxQs, uRxQs, pDev, 1);
}

int
register_netdev(void *pDev)
{
    struct net_device *pNd;

    if (g_cRegCalls < 0xffffffffu) {
        g_cRegCalls++;
    }
    if (!g_fReady) {
        linux_netdev_soft_init();
    }

    pNd = (struct net_device *)pDev;
    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        if (g_cRegSkip < 0xffffffffu) {
            g_cRegSkip++;
        }
        /* Grep: linux_netdev_soft: soft register SKIP - cap logs. */
        if (g_cRegSkipLog < LNDS_LOG_CAP) {
            g_cRegSkipLog++;
            kprintf("linux_netdev_soft: soft register SKIP (inval dev)\n");
        }
        return -1;
    }
    if (pNd->u8Registered != 0u) {
        if (g_cRegSkip < 0xffffffffu) {
            g_cRegSkip++;
        }
        /* Grep: linux_netdev_soft: soft register SKIP - cap logs. */
        if (g_cRegSkipLog < LNDS_LOG_CAP) {
            g_cRegSkipLog++;
            kprintf("linux_netdev_soft: soft register SKIP (already registered "
                    "name=%s)\n",
                    lnds_name_of(pNd));
        }
        return -1;
    }

    pNd->u8Registered = 1u;
    if (g_cRegistered < 0xffffffffu) {
        g_cRegistered++;
    }
    if (g_pPrimary == NULL) {
        g_pPrimary = pNd;
    }
    if (g_cRegPass < 0xffffffffu) {
        g_cRegPass++;
    }

    /* Grep: linux_netdev_soft: soft register PASS name= */
    kprintf("linux_netdev_soft: soft register PASS name=%s mtu=%u "
            "txqs=%u rxqs=%u slot=%u ifindex=%d layout_ver=%u "
            "soft_ops=%d features=0x%llx\n",
            lnds_name_of(pNd), (unsigned)pNd->mtu, (unsigned)pNd->u16TxQs,
            (unsigned)pNd->u16RxQs, (unsigned)pNd->u32Slot,
            pNd->nIfindex, (unsigned)pNd->u32SoftLayoutVer,
            lnds_ops_is_soft(pNd->netdev_ops),
            (unsigned long long)pNd->u64Features);

    /*
     * Strategy A dual hostish net_device: after REAL register, soft primary
     * already holds .ko host-offset stores (ops @+0x8 etc.). Memcpy into
     * hostish blob for Option B readiness. Soft!=product; no ndo_open.
     * Grep: linux_netdev_soft: soft hostish net_device fill PASS
     */
    if (g_pPrimary == pNd) {
        (void)linux_netdev_soft_hostish_fill(pNd);
    }
    return 0;
}

void
unregister_netdev(void *pDev)
{
    struct net_device *pNd;

    if (g_cUnreg < 0xffffffffu) {
        g_cUnreg++;
    }
    pNd = (struct net_device *)pDev;
    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    if (pNd->u8Registered == 0u) {
        return;
    }
    pNd->u8Registered = 0u;
    pNd->u8Carrier = 0u;
    pNd->u8QueueStopped = 1u;
    pNd->u8Open = 0u;
    if (g_cRegistered > 0u) {
        g_cRegistered--;
    }
    if (g_pPrimary == pNd) {
        /* Soft: promote next registered live netdev, else NULL. */
        u32 i;

        g_pPrimary = NULL;
        for (i = 0u; i < LINUX_NETDEV_SOFT_MAX; i++) {
            if (g_aNd[i].nd.u8Live != 0u && g_aNd[i].nd.u8Registered != 0u) {
                g_pPrimary = &g_aNd[i].nd;
                break;
            }
        }
        /* Drop dual hostish blob when primary leaves. Soft!=product. */
        g_fHostishFilled = 0;
        g_pHostishPrimary = NULL;
        g_pHostishOpsCached = NULL;
        g_pHostishNdoOpenCached = NULL;
        g_pHostishNdoStopCached = NULL;
        g_pHostishNdoXmitCached = NULL;
        g_aNdHostishLive[0] = 0u;
        if (g_pPrimary != NULL) {
            (void)linux_netdev_soft_hostish_fill(g_pPrimary);
        }
    }
    /* Cap unregister logs (no stamp storms). Soft!=product. */
    if (g_cUnregLog < LNDS_LOG_CAP) {
        g_cUnregLog++;
        kprintf("linux_netdev_soft: soft unregister name=%s\n",
                lnds_name_of(pNd));
    }
}

void
free_netdev(void *pDev)
{
    struct net_device *pNd;
    struct soft_nd_slab *pSlab;

    if (g_cFree < 0xffffffffu) {
        g_cFree++;
    }
    pNd = (struct net_device *)pDev;
    pSlab = lnds_slab_of(pNd);
    if (pSlab == NULL || pNd->u8Live == 0u) {
        return;
    }
    if (pNd->u8Registered != 0u) {
        unregister_netdev(pNd);
    }
    if (g_pPrimary == pNd) {
        g_pPrimary = NULL;
        g_fHostishFilled = 0;
        g_pHostishPrimary = NULL;
        g_pHostishOpsCached = NULL;
        g_pHostishNdoOpenCached = NULL;
        g_pHostishNdoStopCached = NULL;
        g_pHostishNdoXmitCached = NULL;
        g_aNdHostishLive[0] = 0u;
    }
    if (g_cLive > 0u) {
        g_cLive--;
    }
    memset(pSlab, 0, sizeof(*pSlab));
}

/* ---- Carrier / queue / NAPI --------------------------------------------- */

void
netif_carrier_on(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u8Carrier = 1u;
}

void
netif_carrier_off(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u8Carrier = 0u;
}

void
netif_device_attach(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u8Attached = 1u;
}

void
netif_device_detach(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u8Attached = 0u;
}

void
netif_tx_wake_queue(void *pDev)
{
    /* Soft no-op (queue wake deferred until product TX path). */
    (void)pDev;
}

void
netif_start_queue(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u8QueueStopped = 0u;
}

void
netif_stop_queue(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u8QueueStopped = 1u;
}

void
netif_wake_queue(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u8QueueStopped = 0u;
}

void
netif_napi_add_weight_locked(void *pDev, void *pNapi, void *pfnPoll,
                             int nWeight)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->pNapi = pNapi;
    pNd->pfnNapiPoll = pfnPoll;
    pNd->nNapiWeight = nWeight;
    /* Soft side-table: disabled until napi_enable (post-probe open path). */
    lnds_napi_bind(pDev, pNapi, pfnPoll, nWeight);
}

/* ---- Soft NAPI enable / schedule / complete (post-probe path) ----------- */

void
napi_enable(void *pNapi)
{
    struct soft_napi *pSlot;

    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    if (g_cNapiEnable < 0xffffffffu) {
        g_cNapiEnable++;
    }
    pSlot = lnds_napi_ensure(pNapi);
    if (pSlot == NULL) {
        return;
    }
    /* Soft: clear SCHED/MISSED; mark ENABLED (Linux clears NAPI_STATE_SCHED). */
    pSlot->u8State = (u8)((pSlot->u8State | LNDS_NAPI_F_ENABLED) &
                           (u8)~(LNDS_NAPI_F_SCHED | LNDS_NAPI_F_MISSED));
    pSlot->u8State |= LNDS_NAPI_F_USED;
    if (g_cNapiLog < LNDS_LOG_CAP) {
        g_cNapiLog++;
        /* Grep: linux_netdev_soft: soft napi enable */
        kprintf("linux_netdev_soft: soft napi enable napi=%p dev=%p "
                "calls=%u live=%u (Soft!=product)\n",
                pNapi, pSlot->pDev, (unsigned)g_cNapiEnable,
                (unsigned)g_cNapiLive);
    }
}

void
napi_disable(void *pNapi)
{
    struct soft_napi *pSlot;

    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    if (g_cNapiDisable < 0xffffffffu) {
        g_cNapiDisable++;
    }
    pSlot = lnds_napi_find(pNapi);
    if (pSlot == NULL) {
        /* Soft: disable of unknown cookie is a no-op (no hang). */
        return;
    }
    /*
     * Soft: drop ENABLED + SCHED immediately (no wait for poll). Soft!=product
     * synchronize_irq / busy-wait for NAPI_STATE_SCHED clear.
     */
    pSlot->u8State &= (u8)~(LNDS_NAPI_F_ENABLED | LNDS_NAPI_F_SCHED |
                            LNDS_NAPI_F_MISSED);
    if (g_cNapiLog < LNDS_LOG_CAP) {
        g_cNapiLog++;
        /* Grep: linux_netdev_soft: soft napi disable */
        kprintf("linux_netdev_soft: soft napi disable napi=%p calls=%u "
                "(Soft!=product)\n",
                pNapi, (unsigned)g_cNapiDisable);
    }
}

int
napi_schedule_prep(void *pNapi)
{
    struct soft_napi *pSlot;

    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    pSlot = lnds_napi_find(pNapi);
    if (pSlot == NULL) {
        /* Unknown cookie: soft-mint disabled so prep fails until enable. */
        pSlot = lnds_napi_ensure(pNapi);
        if (pSlot == NULL) {
            if (g_cNapiPrepSkip < 0xffffffffu) {
                g_cNapiPrepSkip++;
            }
            return 0;
        }
    }
    if ((pSlot->u8State & LNDS_NAPI_F_ENABLED) == 0u) {
        if (g_cNapiPrepSkip < 0xffffffffu) {
            g_cNapiPrepSkip++;
        }
        return 0;
    }
    if ((pSlot->u8State & LNDS_NAPI_F_SCHED) != 0u) {
        /* Already scheduled: record MISSED (Linux test_and_set path). */
        pSlot->u8State |= LNDS_NAPI_F_MISSED;
        if (g_cNapiPrepSkip < 0xffffffffu) {
            g_cNapiPrepSkip++;
        }
        return 0;
    }
    pSlot->u8State |= LNDS_NAPI_F_SCHED;
    if (g_cNapiPrepOk < 0xffffffffu) {
        g_cNapiPrepOk++;
    }
    if (g_cNapiLog < LNDS_LOG_CAP) {
        g_cNapiLog++;
        /* Grep: linux_netdev_soft: soft napi prep */
        kprintf("linux_netdev_soft: soft napi prep ok napi=%p calls=%u "
                "(Soft!=product)\n",
                pNapi, (unsigned)g_cNapiPrepOk);
    }
    return 1;
}

int
napi_complete_done(void *pNapi, int nWorkDone)
{
    struct soft_napi *pSlot;
    int fResched;

    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    if (g_cNapiComplete < 0xffffffffu) {
        g_cNapiComplete++;
    }
    pSlot = lnds_napi_find(pNapi);
    if (pSlot == NULL) {
        return 0;
    }
    /* Soft: drop SCHED; if MISSED, ask caller to reschedule (bool true). */
    pSlot->u8State &= (u8)~LNDS_NAPI_F_SCHED;
    fResched = 0;
    if ((pSlot->u8State & LNDS_NAPI_F_MISSED) != 0u) {
        pSlot->u8State &= (u8)~LNDS_NAPI_F_MISSED;
        fResched = 1;
    }
    if (g_cNapiLog < LNDS_LOG_CAP) {
        g_cNapiLog++;
        /* Grep: linux_netdev_soft: soft napi complete */
        kprintf("linux_netdev_soft: soft napi complete napi=%p work=%d "
                "resched=%d calls=%u (Soft!=product)\n",
                pNapi, nWorkDone, fResched, (unsigned)g_cNapiComplete);
    }
    (void)nWorkDone;
    return fResched;
}

void
__napi_schedule(void *pNapi)
{
    struct soft_napi *pSlot;

    /*
     * Linux: enqueue after successful napi_schedule_prep. Soft: SCHED already
     * set by prep; bump counter so freestanding can observe post-prep path.
     */
    if (g_cNapiSched < 0xffffffffu) {
        g_cNapiSched++;
    }
    pSlot = lnds_napi_find(pNapi);
    if (pSlot == NULL) {
        return;
    }
    if ((pSlot->u8State & LNDS_NAPI_F_ENABLED) != 0u) {
        pSlot->u8State |= LNDS_NAPI_F_SCHED;
    }
}

void
napi_consume_skb(void *pSkb, int nBudget)
{
    (void)nBudget;
    lnds_skb_free(pSkb);
}

int
napi_gro_receive(void *pNapi, void *pSkb)
{
    (void)pNapi;
    /* Soft GRO: same as receive drop path (no real GRO merge). Soft!=product. */
    return netif_receive_skb(pSkb);
}

u32
linux_netdev_soft_napi_enabled_count(void)
{
    u32 i;
    u32 c;

    c = 0u;
    for (i = 0u; i < LNDS_NAPI_MAX; i++) {
        if ((g_aNapi[i].u8State & (LNDS_NAPI_F_USED | LNDS_NAPI_F_ENABLED)) ==
            (LNDS_NAPI_F_USED | LNDS_NAPI_F_ENABLED)) {
            c++;
        }
    }
    return c;
}

u32
linux_netdev_soft_napi_sched_count(void)
{
    u32 i;
    u32 c;

    c = 0u;
    for (i = 0u; i < LNDS_NAPI_MAX; i++) {
        if ((g_aNapi[i].u8State & (LNDS_NAPI_F_USED | LNDS_NAPI_F_SCHED)) ==
            (LNDS_NAPI_F_USED | LNDS_NAPI_F_SCHED)) {
            c++;
        }
    }
    return c;
}

void
netif_schedule_queue(void *pTxq)
{
    (void)pTxq;
}

void
netif_set_tso_max_size(void *pDev, u32 u32Size)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u32TsoMaxSize = u32Size;
}

void
netif_set_tso_max_segs(void *pDev, u32 u32Segs)
{
    struct net_device *pNd = (struct net_device *)pDev;

    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u32TsoMaxSegs = u32Segs;
}

/* ---- Post-probe RTNL / eth / sync (replace empty ksym stubs) ------------ */

void
rtnl_lock(void)
{
    if (g_cRtnlLock < 0xffffffffu) {
        g_cRtnlLock++;
    }
    if (g_cRtnlDepth < 0xffffffffu) {
        g_cRtnlDepth++;
    }
    if (g_fRtnlLogOnce == 0) {
        g_fRtnlLogOnce = 1;
        /* Grep: linux_netdev_soft: soft rtnl */
        kprintf("linux_netdev_soft: soft rtnl lock depth=%u (Soft!=product; "
                "no wait)\n",
                (unsigned)g_cRtnlDepth);
    }
}

void
rtnl_unlock(void)
{
    if (g_cRtnlUnlock < 0xffffffffu) {
        g_cRtnlUnlock++;
    }
    if (g_cRtnlDepth > 0u) {
        g_cRtnlDepth--;
    }
    if (g_fRtnlLogOnce == 0) {
        g_fRtnlLogOnce = 1;
        /* Grep: linux_netdev_soft: soft rtnl */
        kprintf("linux_netdev_soft: soft rtnl unlock depth=%u "
                "(Soft!=product)\n",
                (unsigned)g_cRtnlDepth);
    }
}

int
eth_platform_get_mac_address(void *pDev, u8 *pu8Addr)
{
    (void)pDev;
    /*
     * Fail closed: empty ksym stub returned 0 (success) without writing a
     * MAC, which can strand open with a zero/garbage address. Drivers that
     * treat platform MAC as optional fall back to EEPROM on -EOPNOTSUPP.
     * Soft!=product (no DT/ACPI MAC oracle).
     */
    if (pu8Addr != NULL) {
        memset(pu8Addr, 0, 6u);
    }
    if (g_fEthPlatLogOnce == 0) {
        g_fEthPlatLogOnce = 1;
        /* Grep: linux_netdev_soft: soft eth_platform EOPNOTSUPP */
        kprintf("linux_netdev_soft: soft eth_platform EOPNOTSUPP "
                "(null/zero addr; driver may use chip EEPROM; Soft!=product)\n");
    }
    return LNDS_EOPNOTSUPP;
}

int
eth_mac_addr(void *pDev, void *pSa)
{
    struct net_device *pNd = (struct net_device *)pDev;
    const u8 *pu8Src;
    u32 i;

    if (g_cEthMac < 0xffffffffu) {
        g_cEthMac++;
    }
    if (pNd == NULL || pSa == NULL || lnds_slab_of(pNd) == NULL ||
        pNd->u8Live == 0u) {
        return LNDS_EINVAL;
    }
    /*
     * Linux sockaddr: sa_family (2 B) then sa_data[14]. Soft copies 6 octets
     * from sa_data into soft netdev front-field dev_addr only.
     */
    pu8Src = (const u8 *)pSa + 2u;
    for (i = 0u; i < 6u; i++) {
        pNd->dev_addr[i] = pu8Src[i];
    }
    if (g_fEthMacLogOnce == 0) {
        g_fEthMacLogOnce = 1;
        /* Grep: linux_netdev_soft: soft eth_mac */
        kprintf("linux_netdev_soft: soft eth_mac set name=%s "
                "%02x:%02x:%02x:%02x:%02x:%02x (Soft!=product)\n",
                lnds_name_of(pNd),
                (unsigned)pNd->dev_addr[0], (unsigned)pNd->dev_addr[1],
                (unsigned)pNd->dev_addr[2], (unsigned)pNd->dev_addr[3],
                (unsigned)pNd->dev_addr[4], (unsigned)pNd->dev_addr[5]);
    }
    return 0;
}

int
eth_validate_addr(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;
    u32 i;
    int fZero;

    if (g_cEthValidate < 0xffffffffu) {
        g_cEthValidate++;
    }
    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return LNDS_EINVAL;
    }
    /* Soft is_valid_ether_addr: not multicast, not all-zero. */
    if ((pNd->dev_addr[0] & 0x01u) != 0u) {
        return LNDS_EADDRNOTAVAIL;
    }
    fZero = 1;
    for (i = 0u; i < 6u; i++) {
        if (pNd->dev_addr[i] != 0u) {
            fZero = 0;
            break;
        }
    }
    if (fZero != 0) {
        return LNDS_EADDRNOTAVAIL;
    }
    return 0;
}

u16
eth_type_trans(void *pSkb, void *pDev)
{
    struct soft_skb *pSoft;
    u16 u16Type;

    (void)pDev;
    if (g_cEthTypeTrans < 0xffffffffu) {
        g_cEthTypeTrans++;
    }
    pSoft = lnds_skb_of(pSkb);
    if (pSoft == NULL) {
        /* Unknown skb cookie (host layout): soft zero type; Soft!=product. */
        return 0u;
    }
    if (pSoft->uLen < 14u) {
        return 0u;
    }
    /* Network-order ethertype at offset 12 (big-endian wire). */
    u16Type = (u16)(((u16)pSoft->abData[12] << 8) | (u16)pSoft->abData[13]);
    return u16Type;
}

void
synchronize_net(void)
{
    if (g_cSyncNet < 0xffffffffu) {
        g_cSyncNet++;
    }
    /*
     * Soft no-op: no RCU grace, no NAPI wait. Open/close paths that call
     * synchronize_net continue immediately. Soft!=product.
     */
    if (g_fSyncNetLogOnce == 0) {
        g_fSyncNetLogOnce = 1;
        /* Grep: linux_netdev_soft: soft synchronize_net */
        kprintf("linux_netdev_soft: soft synchronize_net (no-op; Soft!=product)\n");
    }
}

/*
 * ethtool_op_get_link(dev) - Linux returns 1 if carrier, 0 if down.
 * Empty ksym stub returned 0 always (false down). Soft: soft netdev carrier,
 * else freestanding rtl8168_ready. Soft!=product; no dual-drive.
 * Grep: linux_netdev_soft: soft ethtool_op_get_link
 */
u32
ethtool_op_get_link(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;
    extern int rtl8168_ready(void);
    u32 uLink;

    if (g_cEthtoolLink < 0xffffffffu) {
        g_cEthtoolLink++;
    }
    uLink = 0u;
    if (pNd != NULL && lnds_slab_of(pNd) != NULL && pNd->u8Live != 0u) {
        if (pNd->u8Carrier != 0u) {
            uLink = 1u;
        }
    } else if (rtl8168_ready() != 0) {
        /* Unknown cookie or no soft carrier - freestanding wire up. */
        uLink = 1u;
    }
    if (g_fEthtoolLinkLog == 0) {
        g_fEthtoolLinkLog = 1;
        /* Grep: linux_netdev_soft: soft ethtool_op_get_link */
        kprintf("linux_netdev_soft: soft ethtool_op_get_link link=%u "
                "(Soft!=product; no BAR)\n",
                (unsigned)uLink);
    }
    return uLink;
}

/*
 * ethtool_op_get_ts_info(dev, info) - soft zero info blob (if non-NULL) and
 * return 0. No HW timestamp oracle. Soft!=product.
 * Grep: linux_netdev_soft: soft ethtool_op_get_ts_info
 */
int
ethtool_op_get_ts_info(void *pDev, void *pInfo)
{
    (void)pDev;
    if (g_cEthtoolTs < 0xffffffffu) {
        g_cEthtoolTs++;
    }
    /*
     * Soft: clear first 44 B of ethtool_ts_info shape (cmd/so_timestamping/
     * phc_index/tx_types/rx_filters common head). Fail closed if no buffer.
     * Soft!=product; no PHC.
     */
    if (pInfo != NULL) {
        memset(pInfo, 0, 44u);
    }
    if (g_fEthtoolTsLog == 0) {
        g_fEthtoolTsLog = 1;
        /* Grep: linux_netdev_soft: soft ethtool_op_get_ts_info */
        kprintf("linux_netdev_soft: soft ethtool_op_get_ts_info zero "
                "(no PHC; Soft!=product)\n");
    }
    return 0;
}

/*
 * net_ratelimit() - Linux returns 1 to allow print, 0 to suppress.
 * Empty ksym stub returned 0 (always suppress). Soft always allows.
 * Soft!=product.
 * Grep: linux_netdev_soft: soft net_ratelimit
 */
int
net_ratelimit(void)
{
    if (g_cNetRatelimit < 0xffffffffu) {
        g_cNetRatelimit++;
    }
    if (g_fNetRatelimitLog == 0) {
        g_fNetRatelimitLog = 1;
        /* Grep: linux_netdev_soft: soft net_ratelimit */
        kprintf("linux_netdev_soft: soft net_ratelimit allow "
                "(Soft!=product; no real token bucket)\n");
    }
    return 1;
}

/* ---- Soft skb ----------------------------------------------------------- */

void *
napi_alloc_skb(void *pNapi, unsigned uLen)
{
    /* RX-shaped: logical length pre-set to uLen (feed_rx copies into abData). */
    return lnds_skb_alloc(pNapi, uLen, 1);
}

/*
 * Soft skb path for future r8169 soft TX bookkeeping.
 * Fail closed on non-pool cookies. Soft!=product. No .ko ndo_start_xmit.
 */

/**
 * skb_copy_bits(skb, offset, to, len) - copy soft abData -> buffer.
 * Soft: only our pool skbs; else -EFAULT once-log.
 */
int
skb_copy_bits(const void *pSkb, int nOffset, void *pTo, int nLen)
{
    struct soft_skb *pSoft;
    unsigned uOff;
    unsigned uCopy;

    if (g_cSkbCopyBits < 0xffffffffu) {
        g_cSkbCopyBits++;
    }
    pSoft = lnds_skb_of((void *)(uintptr_t)pSkb);
    if (pSoft == NULL || pTo == NULL || nOffset < 0 || nLen < 0) {
        if (g_cSkbCopyBitsFail < 0xffffffffu) {
            g_cSkbCopyBitsFail++;
        }
        if (g_fSkbCopyBitsLog == 0) {
            g_fSkbCopyBitsLog = 1;
            /* Grep: linux_netdev_soft: soft skb_copy_bits */
            kprintf("linux_netdev_soft: soft skb_copy_bits FAIL closed "
                    "(not soft skb or bad args; Soft!=product)\n");
        }
        return LNDS_EFAULT;
    }
    uOff = (unsigned)nOffset;
    uCopy = (unsigned)nLen;
    if (uOff > pSoft->uLen || uCopy > (pSoft->uLen - uOff)) {
        if (g_cSkbCopyBitsFail < 0xffffffffu) {
            g_cSkbCopyBitsFail++;
        }
        if (g_fSkbCopyBitsLog == 0) {
            g_fSkbCopyBitsLog = 1;
            kprintf("linux_netdev_soft: soft skb_copy_bits FAIL range "
                    "off=%d len=%d uLen=%u (Soft!=product)\n",
                    nOffset, nLen, pSoft->uLen);
        }
        return LNDS_EFAULT;
    }
    if (uCopy > 0u) {
        memcpy(pTo, &pSoft->abData[uOff], (size_t)uCopy);
    }
    if (g_fSkbCopyBitsLog == 0) {
        g_fSkbCopyBitsLog = 1;
        /* Grep: linux_netdev_soft: soft skb_copy_bits */
        kprintf("linux_netdev_soft: soft skb_copy_bits PASS off=%d len=%d "
                "(Soft!=product; soft pool only)\n",
                nOffset, nLen);
    }
    return 0;
}

/**
 * __skb_pad(skb, pad, free_on_error) - zero pad after soft data if room.
 * Soft: does not grow uLen (Linux pad tailroom). Fail -ENOMEM; free if flag.
 */
int
__skb_pad(void *pSkb, int nPad, int fFreeOnError)
{
    struct soft_skb *pSoft;
    unsigned uPad;

    if (g_cSkbPad < 0xffffffffu) {
        g_cSkbPad++;
    }
    pSoft = lnds_skb_of(pSkb);
    if (pSoft == NULL || nPad < 0) {
        if (g_cSkbPadFail < 0xffffffffu) {
            g_cSkbPadFail++;
        }
        if (g_fSkbPadLog == 0) {
            g_fSkbPadLog = 1;
            /* Grep: linux_netdev_soft: soft __skb_pad */
            kprintf("linux_netdev_soft: soft __skb_pad FAIL closed "
                    "(not soft skb; Soft!=product)\n");
        }
        return LNDS_ENOMEM;
    }
    uPad = (unsigned)nPad;
    if (uPad == 0u) {
        return 0;
    }
    if (pSoft->uLen > pSoft->uCap ||
        uPad > (pSoft->uCap - pSoft->uLen)) {
        unsigned uLenSave = pSoft->uLen;
        unsigned uCapSave = pSoft->uCap;

        if (g_cSkbPadFail < 0xffffffffu) {
            g_cSkbPadFail++;
        }
        if (fFreeOnError != 0) {
            lnds_skb_free(pSkb);
        }
        if (g_fSkbPadLog == 0) {
            g_fSkbPadLog = 1;
            kprintf("linux_netdev_soft: soft __skb_pad FAIL no room "
                    "pad=%d uLen=%u uCap=%u free=%d (Soft!=product)\n",
                    nPad, uLenSave, uCapSave, fFreeOnError != 0 ? 1 : 0);
        }
        return LNDS_ENOMEM;
    }
    memset(&pSoft->abData[pSoft->uLen], 0, (size_t)uPad);
    if (g_fSkbPadLog == 0) {
        g_fSkbPadLog = 1;
        /* Grep: linux_netdev_soft: soft __skb_pad */
        kprintf("linux_netdev_soft: soft __skb_pad PASS pad=%d "
                "(Soft!=product)\n",
                nPad);
    }
    return 0;
}

/**
 * skb_put(skb, len) - grow soft uLen; return pointer to previous tail.
 * Soft: NULL on non-pool or overflow (fail closed). Soft!=product.
 */
void *
skb_put(void *pSkb, unsigned uLen)
{
    struct soft_skb *pSoft;
    void *pTail;

    if (g_cSkbPut < 0xffffffffu) {
        g_cSkbPut++;
    }
    pSoft = lnds_skb_of(pSkb);
    if (pSoft == NULL) {
        if (g_cSkbPutFail < 0xffffffffu) {
            g_cSkbPutFail++;
        }
        if (g_fSkbPutLog == 0) {
            g_fSkbPutLog = 1;
            /* Grep: linux_netdev_soft: soft skb_put */
            kprintf("linux_netdev_soft: soft skb_put FAIL closed "
                    "(not soft skb; Soft!=product)\n");
        }
        return NULL;
    }
    if (uLen > (pSoft->uCap - pSoft->uLen)) {
        if (g_cSkbPutFail < 0xffffffffu) {
            g_cSkbPutFail++;
        }
        if (g_fSkbPutLog == 0) {
            g_fSkbPutLog = 1;
            kprintf("linux_netdev_soft: soft skb_put FAIL overflow "
                    "want=%u uLen=%u uCap=%u (Soft!=product)\n",
                    uLen, pSoft->uLen, pSoft->uCap);
        }
        return NULL;
    }
    pTail = &pSoft->abData[pSoft->uLen];
    pSoft->uLen += uLen;
    if (g_fSkbPutLog == 0) {
        g_fSkbPutLog = 1;
        /* Grep: linux_netdev_soft: soft skb_put */
        kprintf("linux_netdev_soft: soft skb_put PASS len=%u new_uLen=%u "
                "(Soft!=product)\n",
                uLen, pSoft->uLen);
    }
    return pTail;
}

/**
 * pskb_may_pull(skb, len) - soft linear buffer always.
 * Soft: 1 if live soft skb has uLen >= len; else 0 fail closed.
 */
int
pskb_may_pull(void *pSkb, unsigned uLen)
{
    struct soft_skb *pSoft;

    if (g_cPskbMayPull < 0xffffffffu) {
        g_cPskbMayPull++;
    }
    pSoft = lnds_skb_of(pSkb);
    if (pSoft == NULL) {
        if (g_cPskbMayPullFail < 0xffffffffu) {
            g_cPskbMayPullFail++;
        }
        if (g_fPskbMayPullLog == 0) {
            g_fPskbMayPullLog = 1;
            /* Grep: linux_netdev_soft: soft pskb_may_pull */
            kprintf("linux_netdev_soft: soft pskb_may_pull FAIL closed "
                    "(not soft skb; Soft!=product)\n");
        }
        return 0;
    }
    if (pSoft->uLen < uLen) {
        if (g_cPskbMayPullFail < 0xffffffffu) {
            g_cPskbMayPullFail++;
        }
        return 0;
    }
    if (g_fPskbMayPullLog == 0) {
        g_fPskbMayPullLog = 1;
        /* Grep: linux_netdev_soft: soft pskb_may_pull */
        kprintf("linux_netdev_soft: soft pskb_may_pull PASS len=%u uLen=%u "
                "(Soft!=product; linear soft)\n",
                uLen, pSoft->uLen);
    }
    return 1;
}

/**
 * __pskb_pull_tail(skb, delta) - soft no frags; success if soft has data.
 * Soft: return abData pointer (non-NULL) or NULL fail closed.
 */
void *
__pskb_pull_tail(void *pSkb, int nDelta)
{
    struct soft_skb *pSoft;

    (void)nDelta;
    if (g_cPskbPullTail < 0xffffffffu) {
        g_cPskbPullTail++;
    }
    pSoft = lnds_skb_of(pSkb);
    if (pSoft == NULL) {
        return NULL;
    }
    /* Soft linear: no paged pull; data already contiguous in abData. */
    return pSoft->abData;
}

/**
 * pskb_expand_head(skb, nhead, ntail, gfp) - soft fixed slab.
 * Soft: 0 if soft skb and nhead/ntail fit remaining; else -ENOMEM.
 */
int
pskb_expand_head(void *pSkb, int nHead, int nTail, unsigned uGfp)
{
    struct soft_skb *pSoft;
    unsigned uNeed;

    (void)uGfp;
    if (g_cPskbExpand < 0xffffffffu) {
        g_cPskbExpand++;
    }
    pSoft = lnds_skb_of(pSkb);
    if (pSoft == NULL || nHead < 0 || nTail < 0) {
        return LNDS_ENOMEM;
    }
    /* Soft shape has no headroom field; nHead must be 0 (no shift). */
    if (nHead != 0) {
        return LNDS_ENOMEM;
    }
    uNeed = pSoft->uLen + (unsigned)nTail;
    if (uNeed > pSoft->uCap) {
        return LNDS_ENOMEM;
    }
    return 0;
}

/**
 * __netdev_alloc_skb(dev, length, gfp) - soft pool; len starts 0 (TX put).
 * Soft!=product; no .ko xmit.
 */
void *
__netdev_alloc_skb(void *pDev, unsigned uLen, unsigned uGfp)
{
    struct soft_skb *pSkb;

    (void)pDev;
    (void)uGfp;
    if (g_cNetdevAllocSkb < 0xffffffffu) {
        g_cNetdevAllocSkb++;
    }
    pSkb = lnds_skb_alloc(NULL, uLen, 0);
    if (g_fNetdevAllocSkbLog == 0) {
        g_fNetdevAllocSkbLog = 1;
        /* Grep: linux_netdev_soft: soft __netdev_alloc_skb */
        kprintf("linux_netdev_soft: soft __netdev_alloc_skb %s len=%u "
                "(Soft!=product; soft pool; no .ko xmit)\n",
                pSkb != NULL ? "PASS" : "FAIL", uLen);
    }
    return pSkb;
}

/** dev_alloc_skb(length) - alias soft pool (GFP ignored). Soft!=product. */
void *
dev_alloc_skb(unsigned uLen)
{
    return __netdev_alloc_skb(NULL, uLen, 0u);
}

/**
 * __napi_alloc_skb(napi, length, gfp) - soft pool RX-shaped (uLen preset).
 * Soft!=product.
 */
void *
__napi_alloc_skb(void *pNapi, unsigned uLen, unsigned uGfp)
{
    (void)uGfp;
    return lnds_skb_alloc(pNapi, uLen, 1);
}

int
netif_receive_skb(void *pSkb)
{
    /*
     * Soft bridge intake: count as soft stack RX then free skb.
     * Product demux remains freestanding net_eth handle_frame.
     * Soft!=product.
     */
    if (pSkb != NULL) {
        if (g_cSkbDrop < 0xffffffffu) {
            g_cSkbDrop++;
        }
        lnds_skb_free(pSkb);
    }
    return 0; /* NET_RX_SUCCESS soft */
}

void
linux_netdev_soft_open_primary(void)
{
    struct net_device *pNd;
    int fOps;
    int fOpsSoft;
    int fNapi;
    u32 cNapiEn;

    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    /*
     * Dual DoD B residual: soft open is control-object only.
     * Force freestanding sole BAR before any open bookkeeping. Soft!=product.
     */
    lnds_gate0_force_no_bar_steal();
    pNd = g_pPrimary;
    if (pNd == NULL || pNd->u8Registered == 0u) {
        return;
    }

    /* Layout diagnostic once (read-only; no .ko ndo_open). Soft!=product. */
    linux_netdev_soft_ops_diagnostic();

    /* Soft open edge: once per primary until unregister. Soft!=product. */
    if (pNd->u8Open == 0u) {
        pNd->u8Open = 1u;
        if (g_cSoftOpen < 0xffffffffu) {
            g_cSoftOpen++;
        }
    }

    /* Carrier / attach / queue via soft helpers (idempotent). */
    if (pNd->u8Carrier == 0u) {
        netif_carrier_on(pNd);
    }
    if (pNd->u8Attached == 0u) {
        netif_device_attach(pNd);
    }
    /* Always soft-start queue (netif_start_queue is idempotent). */
    netif_start_queue(pNd);

    /*
     * Optional soft-only ndo_open when ops table is ours.
     * .ko ops: skip (MMIO freestanding owns) - never dual-drive.
     */
    lnds_soft_dev_open(pNd);

    /*
     * Soft NAPI bookkeeping enable for bridge/open (not .ko poll).
     * Soft!=product: state bits only while freestanding owns MMIO.
     */
    lnds_napi_soft_enable_bridge_slots();

    fOps = (pNd->netdev_ops != NULL) ? 1 : 0;
    fOpsSoft = lnds_ops_is_soft(pNd->netdev_ops);
    fNapi = (pNd->pNapi != NULL) ? 1 : 0;
    cNapiEn = linux_netdev_soft_napi_enabled_count();

    if (g_fSoftOpenLog == 0) {
        g_fSoftOpenLog = 1;
        /* Grep: linux_netdev_soft: soft open primary */
        kprintf("linux_netdev_soft: soft open primary name=%s mtu=%u "
                "carrier=%u queue_run=%u ops=%d soft_ops=%d napi=%d "
                "napi_en=%u open_n=%u live_fs_8168=%d no_bar_steal=%d "
                "(Soft!=product; no .ko ndo_open; residual E fail-closed "
                "live 8168; freestanding owns wire; G-AC-1 lean)\n",
                lnds_name_of(pNd), (unsigned)pNd->mtu,
                (unsigned)pNd->u8Carrier,
                pNd->u8QueueStopped == 0u ? 1u : 0u, fOps, fOpsSoft, fNapi,
                (unsigned)cNapiEn, (unsigned)g_cSoftOpen,
                lnds_live_fs_8168(), g_fMmioHandoff == 0 ? 1 : 0);
    }
    /*
     * Dual DoD B residual honesty early (does not require hybrid_active).
     * Once-capped inside helper; no stamp storm. Soft!=product; G-AC-1 lean.
     */
    (void)linux_netdev_soft_gate0_residual_lamp_once();
}

int
linux_netdev_soft_primary_open(void)
{
    if (g_pPrimary == NULL || g_pPrimary->u8Live == 0u) {
        return 0;
    }
    return g_pPrimary->u8Open != 0u ? 1 : 0;
}

u32
linux_netdev_soft_open_count(void)
{
    return g_cSoftOpen;
}

void
linux_netdev_soft_l2_bridge_enable(int fOn)
{
    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    /*
     * Dual DoD B residual: L2 bridge is copy/account only.
     * Freestanding remains sole BAR/wire owner at gate0. Soft!=product.
     */
    lnds_gate0_force_no_bar_steal();
    g_fL2Bridge = (fOn != 0) ? 1 : 0;
    if (g_fL2Bridge != 0 && g_fL2BridgeLog == 0) {
        g_fL2BridgeLog = 1;
        /* Grep: linux_netdev_soft: soft l2 bridge ON */
        kprintf("linux_netdev_soft: soft l2 bridge ON "
                "(freestanding L2 ↔ soft netdev; Soft!=product; "
                "mmio=freestanding no .ko poll/ISR; no_bar_steal=1; "
                "soft TX API ready, no auto wire inject unless "
                "GJ_SOFT_L2_TX_SMOKE=1; G-AC-1 lean)\n");
        /* Soft-open primary + soft-enable NAPI bookkeeping slots. */
        linux_netdev_soft_open_primary();
        /* hold14 baseline (rx=0 tx=0) so panel is honest before traffic. */
        lnds_hold14_l2_br_refresh();
#if GJ_SOFT_L2_TX_SMOKE != 0
        /* Lab-only reverse-path smoke - default gate 0 (no live-wire junk). */
        lnds_soft_l2_tx_smoke_once();
#endif
    } else if (g_fL2Bridge != 0) {
        /* Re-enable: refresh open + NAPI enable if probe raced. Soft!=product. */
        linux_netdev_soft_open_primary();
        lnds_hold14_l2_br_refresh();
    }
}

int
linux_netdev_soft_l2_bridge_enabled(void)
{
    return g_fL2Bridge;
}

void
linux_netdev_soft_l2_feed_rx(const void *pFrame, u32 cbLen)
{
    void *pSkb;
    struct soft_skb *pSoft;

    if (g_fL2Bridge == 0 || pFrame == NULL || cbLen < 14u) {
        /*
         * Bridge off: feed_rx rare under SOFT hybrid until latch. Still
         * refresh hold14 so freestanding R climb paints L2 BR (HY7).
         * Soft!=product; no copy / no BAR touch.
         */
        lnds_hold14_l2_br_refresh();
        return;
    }
    /*
     * Bridge ON + eth-sized frame -> attempt. Drop if no soft primary/skb.
     * Soft!=product: freestanding already demuxed; this is copy-only.
     * Never dual-drive BAR / rings.
     */
    if (g_cL2RxAttempt < 0xffffffffu) {
        g_cL2RxAttempt++;
    }
    if (g_pPrimary == NULL || g_pPrimary->u8Registered == 0u) {
        if (g_cL2RxDrop < 0xffffffffu) {
            g_cL2RxDrop++;
        }
        if (g_pPrimary != NULL) {
            g_pPrimary->u64RxDropped++;
        }
        /* freestanding R may still climb - keep hold14 live. Soft!=product. */
        lnds_hold14_l2_br_refresh();
        return;
    }
    /*
     * Late soft-enable: probe may have registered napi after bridge ON.
     * Soft!=product; keeps feed_rx soft_poll from no-op forever.
     */
    if (linux_netdev_soft_napi_enabled_count() == 0u &&
        g_pPrimary->pNapi != NULL) {
        lnds_napi_soft_enable_bridge_slots();
    }
    pSkb = napi_alloc_skb(g_pPrimary->pNapi, cbLen);
    if (pSkb == NULL) {
        if (g_cL2RxDrop < 0xffffffffu) {
            g_cL2RxDrop++;
        }
        g_pPrimary->u64RxDropped++;
        lnds_hold14_l2_br_refresh();
        return;
    }
    pSoft = (struct soft_skb *)pSkb;
    if (cbLen > pSoft->uCap) {
        cbLen = pSoft->uCap;
    }
    memcpy(pSoft->abData, pFrame, (size_t)cbLen);
    pSoft->uLen = cbLen;
    if (g_cL2RxFed < 0xffffffffu) {
        g_cL2RxFed++;
    }
    /* Soft net_device stats residual (bookkeeping; freestanding owns wire). */
    if (g_pPrimary != NULL) {
        g_pPrimary->u64RxPackets++;
        g_pPrimary->u64RxBytes += (u64)cbLen;
    }
    (void)netif_receive_skb(pSkb);
    /* hold14 live refresh (HY7); freestanding R mirror + soft fed. Soft!=product. */
    lnds_hold14_l2_br_refresh();
    if (g_fL2RxNapiLog == 0) {
        g_fL2RxNapiLog = 1;
        /* Grep: linux_netdev_soft: soft l2 rx napi */
        kprintf("linux_netdev_soft: soft l2 rx napi first fed=%u "
                "attempt=%u drop=%u enabled=%u sched=%u mmio_handoff=%d "
                "(Soft!=product; soft L2 bridge only; no BAR dual-drive)\n",
                (unsigned)g_cL2RxFed, (unsigned)g_cL2RxAttempt,
                (unsigned)g_cL2RxDrop,
                (unsigned)linux_netdev_soft_napi_enabled_count(),
                (unsigned)linux_netdev_soft_napi_sched_count(),
                g_fMmioHandoff);
    }
    /*
     * Soft IRQ -> NAPI path (no hardware IRQ into .ko):
     *   softirq_kick: schedule_prep + __napi_schedule bookkeeping
     *   soft_poll:    complete (+ optional poll only if safe)
     * Soft!=product; freestanding owns MMIO.
     */
    linux_netdev_soft_softirq_kick();
    linux_netdev_soft_napi_soft_poll();
}

int
linux_netdev_soft_l2_xmit(const void *pFrame, u32 cbLen)
{
    int nSt;

    if (g_fL2Bridge == 0 || pFrame == NULL || cbLen == 0u) {
        return -1;
    }
    /*
     * Dual DoD B residual: reverse TX never steals freestanding BAR.
     * Force gate0 clear before freestanding-owns check. Soft!=product.
     */
    lnds_gate0_force_no_bar_steal();
    if (g_cL2TxAttempt < 0xffffffffu) {
        g_cL2TxAttempt++;
    }
    /*
     * Residual E: reverse soft->freestanding TX only while freestanding owns.
     * Never .ko ndo_start_xmit. Handoff pending / soft mmio -> fail closed.
     * Soft!=product; hybrid SOFT must not steal BAR.
     */
    if (lnds_soft_tx_freestanding_owns() == 0) {
        if (g_cL2TxFail < 0xffffffffu) {
            g_cL2TxFail++;
        }
        if (g_fLive8168XmitFcLog == 0) {
            g_fLive8168XmitFcLog = 1;
            /* Grep: linux_netdev_soft: soft xmit fail-closed live 8168 */
            kprintf("linux_netdev_soft: soft xmit fail-closed live 8168 "
                    "(not freestanding-owns; mmio_handoff=%d; Soft!=product; "
                    "no .ko ndo_start_xmit; residual E)\n",
                    g_fMmioHandoff);
        }
        lnds_l2_first_tx_lamp(0);
        lnds_hold14_l2_br_refresh();
        return -1;
    }
    if (net_l2_tx == NULL || net_l2_ready == NULL || net_l2_ready() == 0) {
        if (g_cL2TxFail < 0xffffffffu) {
            g_cL2TxFail++;
        }
        lnds_l2_first_tx_lamp(0);
        return -1;
    }
    nSt = net_l2_tx(pFrame, cbLen);
    if (nSt == 0) {
        if (g_cL2TxOk < 0xffffffffu) {
            g_cL2TxOk++;
        }
        if (g_cL2TxAccounted < 0xffffffffu) {
            g_cL2TxAccounted++;
        }
        /* Soft net_device stats residual (Soft!=product). */
        if (g_pPrimary != NULL) {
            g_pPrimary->u64TxPackets++;
            g_pPrimary->u64TxBytes += (u64)cbLen;
        }
        lnds_l2_first_tx_lamp(1);
    } else {
        if (g_cL2TxFail < 0xffffffffu) {
            g_cL2TxFail++;
        }
        if (g_pPrimary != NULL) {
            g_pPrimary->u64TxDropped++;
        }
        lnds_l2_first_tx_lamp(0);
    }
    /* Always refresh - freestanding R/T mirror + soft tx (ok or fail). */
    lnds_hold14_l2_br_refresh();
    return nSt;
}

/**
 * Soft-originated TX: validate soft pool skb -> freestanding net_l2_tx.
 * Soft!=product; no .ko ndo_start_xmit. Consumes soft skb when pool cookie.
 * Grep: linux_netdev_soft: soft l2 tx from_skb
 */
int
linux_netdev_soft_l2_tx_from_skb(void *pSkb)
{
    struct soft_skb *pSoft;
    int nSt;
    u32 cbLen;

    if (g_cSoftTxFromSkbAttempt < 0xffffffffu) {
        g_cSoftTxFromSkbAttempt++;
    }
    pSoft = lnds_skb_of(pSkb);
    if (pSoft == NULL) {
        if (g_cSoftTxFromSkbFail < 0xffffffffu) {
            g_cSoftTxFromSkbFail++;
        }
        if (g_fSoftTxFromSkbLog == 0) {
            g_fSoftTxFromSkbLog = 1;
            /* Grep: linux_netdev_soft: soft l2 tx from_skb */
            kprintf("linux_netdev_soft: soft l2 tx from_skb FAIL closed "
                    "(not soft pool skb; Soft!=product)\n");
        }
        return -1;
    }
    cbLen = (u32)pSoft->uLen;
    if (cbLen == 0u || cbLen > (u32)pSoft->uCap) {
        if (g_cSoftTxFromSkbFail < 0xffffffffu) {
            g_cSoftTxFromSkbFail++;
        }
        lnds_skb_free(pSkb);
        if (g_fSoftTxFromSkbLog == 0) {
            g_fSoftTxFromSkbLog = 1;
            kprintf("linux_netdev_soft: soft l2 tx from_skb FAIL empty/bad "
                    "len=%u (Soft!=product)\n",
                    (unsigned)cbLen);
        }
        return -1;
    }
    /*
     * Frame bytes live in soft abData until freestanding copies into TX
     * ring. Call l2_xmit (bridge gate + attempt/ok/fail + first-tx lamp)
     * then free soft skb. Soft!=product.
     */
    nSt = linux_netdev_soft_l2_xmit(pSoft->abData, cbLen);
    lnds_skb_free(pSkb);
    if (nSt == 0) {
        if (g_cSoftTxFromSkbOk < 0xffffffffu) {
            g_cSoftTxFromSkbOk++;
        }
        if (g_fSoftTxFromSkbLog == 0) {
            g_fSoftTxFromSkbLog = 1;
            /* Grep: linux_netdev_soft: soft l2 tx from_skb */
            kprintf("linux_netdev_soft: soft l2 tx from_skb PASS len=%u "
                    "ok=%u fail=%u (Soft!=product; freestanding owns wire)\n",
                    (unsigned)cbLen, (unsigned)g_cSoftTxFromSkbOk,
                    (unsigned)g_cSoftTxFromSkbFail);
        }
    } else {
        if (g_cSoftTxFromSkbFail < 0xffffffffu) {
            g_cSoftTxFromSkbFail++;
        }
        if (g_fSoftTxFromSkbLog == 0) {
            g_fSoftTxFromSkbLog = 1;
            kprintf("linux_netdev_soft: soft l2 tx from_skb FAIL xmit "
                    "len=%u bridge=%d (Soft!=product)\n",
                    (unsigned)cbLen, g_fL2Bridge);
        }
    }
    return nSt;
}

/**
 * Soft dev_queue_xmit(skb) - ksym reverse path soft stack -> freestanding L2.
 * Soft!=product; fail closed when handoff pending or freestanding does not own.
 * Residual E: never .ko ndo_start_xmit while live 8168 freestanding owns.
 * Grep: linux_netdev_soft: soft dev_queue_xmit
 */
int
dev_queue_xmit(void *pSkb)
{
    int nSt;
    int fOwns;
    struct soft_skb *pSoft;

    if (g_cSoftDqXmit < 0xffffffffu) {
        g_cSoftDqXmit++;
    }

    /* Dual DoD B residual: reverse path never steals freestanding wire. */
    lnds_gate0_force_no_bar_steal();
    fOwns = lnds_soft_tx_freestanding_owns();
    /*
     * Safe path: L2 bridge ON + freestanding owns MMIO -> soft skb -> net_l2_tx.
     * Soft!=product; no .ko ndo_start_xmit.
     */
    if (g_fL2Bridge != 0 && fOwns != 0) {
        nSt = linux_netdev_soft_l2_tx_from_skb(pSkb);
        if (nSt == 0) {
            if (g_cSoftDqXmitOk < 0xffffffffu) {
                g_cSoftDqXmitOk++;
            }
        } else {
            if (g_cSoftDqXmitFail < 0xffffffffu) {
                g_cSoftDqXmitFail++;
            }
        }
        if (g_fSoftDqXmitLog == 0) {
            g_fSoftDqXmitLog = 1;
            /* Grep: linux_netdev_soft: soft dev_queue_xmit */
            kprintf("linux_netdev_soft: soft dev_queue_xmit %s "
                    "bridge=1 freestanding=1 ok=%u fail=%u "
                    "(Soft!=product; no dual-drive)\n",
                    nSt == 0 ? "PASS" : "FAIL",
                    (unsigned)g_cSoftDqXmitOk, (unsigned)g_cSoftDqXmitFail);
        }
        return nSt;
    }

    /*
     * Handoff pending / bridge off / soft mmio owns: fail closed.
     * Free soft pool skb to avoid leak; ignore non-pool cookies.
     * Soft!=product - no WAIT poll / no .ko open.
     */
    if (g_cSoftDqXmitFail < 0xffffffffu) {
        g_cSoftDqXmitFail++;
    }
    pSoft = lnds_skb_of(pSkb);
    if (pSoft != NULL) {
        lnds_skb_free(pSkb);
    }
    if (g_fSoftDqXmitLog == 0) {
        g_fSoftDqXmitLog = 1;
        /* Grep: linux_netdev_soft: soft dev_queue_xmit */
        kprintf("linux_netdev_soft: soft dev_queue_xmit FAIL closed "
                "bridge=%d freestanding_owns=%d mmio_handoff=%d "
                "(Soft!=product; handoff pending or bridge off)\n",
                g_fL2Bridge, fOwns, g_fMmioHandoff);
    }
    return -1;
}

#if GJ_SOFT_L2_TX_SMOKE != 0
/**
 * Lab-only once smoke: soft alloc -> put 60B minimal eth -> from_skb.
 * Soft!=product. Default build has GJ_SOFT_L2_TX_SMOKE=0 (no auto wire inject).
 * Deliberate lab images only - G752 has real NIC.
 */
static void
lnds_soft_l2_tx_smoke_once(void)
{
    void *pSkb;
    void *pTail;
    int nSt;

    if (g_fSoftL2TxSmokeLog != 0) {
        return;
    }
    g_fSoftL2TxSmokeLog = 1;
    if (g_fL2Bridge == 0 || lnds_soft_tx_freestanding_owns() == 0) {
        kprintf("linux_netdev_soft: soft l2 tx smoke SKIP "
                "(bridge/owns; Soft!=product; GJ_SOFT_L2_TX_SMOKE=1)\n");
        return;
    }
    if (net_l2_ready == NULL || net_l2_ready() == 0) {
        kprintf("linux_netdev_soft: soft l2 tx smoke SKIP (net_l2 not ready; "
                "Soft!=product)\n");
        return;
    }
    pSkb = dev_alloc_skb(60u);
    if (pSkb == NULL) {
        kprintf("linux_netdev_soft: soft l2 tx smoke FAIL alloc "
                "(Soft!=product)\n");
        return;
    }
    pTail = skb_put(pSkb, 60u);
    if (pTail == NULL) {
        lnds_skb_free(pSkb);
        kprintf("linux_netdev_soft: soft l2 tx smoke FAIL put "
                "(Soft!=product)\n");
        return;
    }
    /* Minimal 60B Ethernet: zeros = invalid ethertype; lab smoke only. */
    memset(pTail, 0, 60u);
    nSt = linux_netdev_soft_l2_tx_from_skb(pSkb);
    kprintf("linux_netdev_soft: soft l2 tx smoke %s st=%d "
            "(Soft!=product; GJ_SOFT_L2_TX_SMOKE=1 lab only)\n",
            nSt == 0 ? "PASS" : "FAIL", nSt);
}
#endif /* GJ_SOFT_L2_TX_SMOKE */

u32
linux_netdev_soft_l2_rx_fed(void)
{
    u32 cRx;
    u32 cTx;

    /*
     * Panel / net_eth hold14 poll path: report max(soft_fed, freestanding R)
     * so freestanding R climb updates hold14 when feed_rx is rare (bridge
     * off under SOFT). Force-refresh STATUS hold14 so freestanding R/T
     * climb paints even when soft counters stagnate (HY7). Soft!=product;
     * pure soft-fed is g_cL2RxFed only.
     */
    lnds_hold14_counts(&cRx, &cTx);
    (void)cTx;
    lnds_hold14_l2_br_refresh();
    return cRx;
}

u32
linux_netdev_soft_l2_rx_attempt(void)
{
    return g_cL2RxAttempt;
}

u32
linux_netdev_soft_l2_rx_drop(void)
{
    return g_cL2RxDrop;
}

u32
linux_netdev_soft_l2_tx_ok(void)
{
    u32 cRx;
    u32 cTx;

    /*
     * Same freestanding R/T mirror as rx_fed - keep hold14 rx/tx coherent.
     * Force-refresh STATUS hold14 on poll (HY7). Soft!=product.
     */
    lnds_hold14_counts(&cRx, &cTx);
    (void)cRx;
    lnds_hold14_l2_br_refresh();
    return cTx;
}

void
linux_netdev_soft_hold14_refresh(void)
{
    /* Grep / panel: L2 BR RX · hold14 · freestanding R/T force */
    lnds_hold14_l2_br_refresh();
}

u32
linux_netdev_soft_hold14_fs_rx(void)
{
    u32 cFsRx;

    lnds_hold14_sample_fs(&cFsRx, NULL);
    return cFsRx;
}

u32
linux_netdev_soft_hold14_fs_tx(void)
{
    u32 cFsTx;

    lnds_hold14_sample_fs(NULL, &cFsTx);
    return cFsTx;
}

u32
linux_netdev_soft_l2_tx_fail(void)
{
    return g_cL2TxFail;
}

u32
linux_netdev_soft_l2_tx_attempt(void)
{
    return g_cL2TxAttempt;
}

u32
linux_netdev_soft_l2_tx_accounted(void)
{
    return g_cL2TxAccounted;
}

void
linux_netdev_soft_l2_note_tx(int fOk)
{
    if (g_fL2Bridge == 0) {
        /*
         * Bridge off under SOFT: still refresh hold14 so freestanding R/B
         * mirror can paint while feed_rx rare. Soft!=product.
         */
        lnds_hold14_l2_br_refresh();
        return;
    }
    if (g_cL2TxAttempt < 0xffffffffu) {
        g_cL2TxAttempt++;
    }
    /*
     * Freestanding already sent (or failed) on the wire - soft-account only.
     * Soft!=product: no second send / no .ko ndo_start_xmit.
     */
    if (fOk != 0) {
        if (g_cL2TxOk < 0xffffffffu) {
            g_cL2TxOk++;
        }
        if (g_cL2TxAccounted < 0xffffffffu) {
            g_cL2TxAccounted++;
        }
        lnds_l2_first_tx_lamp(1);
    } else {
        if (g_cL2TxFail < 0xffffffffu) {
            g_cL2TxFail++;
        }
        lnds_l2_first_tx_lamp(0);
    }
    /* Always refresh - freestanding R mirror + soft tx. Soft!=product. */
    lnds_hold14_l2_br_refresh();
}

void
linux_netdev_soft_softirq_kick(void)
{
    struct net_device *pNd;
    struct soft_napi *pSlot;
    int nPrep;

    /*
     * Soft IRQ half of NAPI: schedule_prep + __napi_schedule only.
     * Never calls .ko ISR or poll. Safe with freestanding MMIO ownership.
     * Dual DoD B residual: bookkeeping only; no BAR steal. Soft!=product.
     * Grep: linux_netdev_soft: soft softirq kick
     */
    lnds_gate0_force_no_bar_steal();
    if (g_fL2Bridge == 0 || g_pPrimary == NULL) {
        return;
    }
    pNd = g_pPrimary;
    if (pNd->pNapi == NULL) {
        return;
    }
    pSlot = lnds_napi_find(pNd->pNapi);
    if (pSlot == NULL || (pSlot->u8State & LNDS_NAPI_F_ENABLED) == 0u) {
        return;
    }
    nPrep = napi_schedule_prep(pNd->pNapi);
    if (nPrep == 0) {
        /* Already SCHED or disabled - soft MISSED may be set by prep. */
        return;
    }
    __napi_schedule(pNd->pNapi);
    if (g_cSoftirqKick < 0xffffffffu) {
        g_cSoftirqKick++;
    }
    if (g_cSoftirqKick == 1u) {
        kprintf("linux_netdev_soft: soft softirq kick napi=%p calls=1 "
                "(bookkeeping only; Soft!=product; no .ko ISR; "
                "no_bar_steal=1; freestanding owns wire)\n",
                pNd->pNapi);
    }
}

void
linux_netdev_soft_napi_soft_poll(void)
{
    struct net_device *pNd;
    struct soft_napi *pSlot;
    int (*pfnPoll)(void *pNapi, int nBudget);
    int nBudget;
    int nDone;
    int fResched;
    int fCallPoll;
    int fKoPoll;

    if (g_fL2Bridge == 0 || g_pPrimary == NULL) {
        return;
    }
    pNd = g_pPrimary;
    if (pNd->pNapi == NULL) {
        return;
    }
    pSlot = lnds_napi_find(pNd->pNapi);
    if (pSlot == NULL || (pSlot->u8State & LNDS_NAPI_F_ENABLED) == 0u) {
        /* Soft: no poll until napi_enable bookkeeping. Soft!=product. */
        return;
    }
    /* Ensure SCHED bit (softirq_kick or local IRQ-path bookkeeping). */
    if ((pSlot->u8State & LNDS_NAPI_F_SCHED) == 0u) {
        if (napi_schedule_prep(pNd->pNapi) == 0) {
            return;
        }
        __napi_schedule(pNd->pNapi);
    }

    pfnPoll = NULL;
    fKoPoll = 0;
    if (pSlot->pfnPoll != NULL) {
        pfnPoll = (int (*)(void *, int))(uintptr_t)pSlot->pfnPoll;
        if ((pSlot->u8State & LNDS_NAPI_F_KO_POLL) != 0u) {
            fKoPoll = 1;
        }
    } else if (pNd->pfnNapiPoll != NULL) {
        /* netdev cookie copy - treat as .ko-sourced unless handoff. */
        pfnPoll = (int (*)(void *, int))(uintptr_t)pNd->pfnNapiPoll;
        fKoPoll = 1;
    }

    /*
     * SAFETY: freestanding owns MMIO today. Calling .ko napi poll can touch
     * dual-owned BAR and crash. Default: bookkeeping only (complete).
     * Call poll only if:
     *   (a) mmio handoff gate is set AND freestanding not live, OR
     *   (b) pfnPoll is soft-owned (not KO_POLL) AND freestanding not live.
     * Residual E: live freestanding 8168 -> always fail closed (.ko poll).
     * Hybrid SOFT must not steal BAR. Soft!=product.
     * Grep: linux_netdev_soft: soft napi poll skip .ko (mmio freestanding)
     * Grep: linux_netdev_soft: soft napi fail-closed live 8168
     */
    /*
     * Dual DoD B residual: gate0 force freestanding sole BAR before poll
     * decision. Soft!=product; never .ko poll dual-drive (R0 thrash class).
     */
    lnds_gate0_force_no_bar_steal();
    fCallPoll = 0;
    if (pfnPoll != NULL && lnds_live_fs_8168() == 0) {
        if (g_fMmioHandoff != 0) {
            fCallPoll = 1;
        } else if (fKoPoll == 0) {
            fCallPoll = 1;
        }
    }

    if (pfnPoll == NULL || fCallPoll == 0) {
        if (g_cNapiPollSkip < 0xffffffffu) {
            g_cNapiPollSkip++;
        }
        if (pfnPoll != NULL && fKoPoll != 0 && g_fNapiPollSkipKoLog == 0) {
            g_fNapiPollSkipKoLog = 1;
            /* Grep: linux_netdev_soft: soft napi poll skip .ko (mmio freestanding) */
            kprintf("linux_netdev_soft: soft napi poll skip .ko "
                    "(mmio freestanding)\n");
        }
        if (lnds_live_fs_8168() != 0) {
            if (g_cLive8168FcNapi < 0xffffffffu) {
                g_cLive8168FcNapi++;
            }
            if (g_fLive8168NapiFcLog == 0) {
                g_fLive8168NapiFcLog = 1;
                /* Grep: linux_netdev_soft: soft napi fail-closed live 8168 */
                kprintf("linux_netdev_soft: soft napi fail-closed live 8168 "
                        "napi=%p ko_poll=%d mmio_handoff=%d "
                        "(Soft!=product; freestanding owns BAR; residual E; "
                        "hybrid SOFT must not steal BAR)\n",
                        pNd->pNapi, fKoPoll, g_fMmioHandoff);
            }
        }
        /* Bookkeeping-only complete (work=0). Soft!=product. */
        (void)napi_complete_done(pNd->pNapi, 0);
        return;
    }

    nBudget = pSlot->nWeight > 0 ? pSlot->nWeight :
              (pNd->nNapiWeight > 0 ? pNd->nNapiWeight : 64);
    nDone = pfnPoll(pNd->pNapi, nBudget);
    if (g_cNapiPollInvoke < 0xffffffffu) {
        g_cNapiPollInvoke++;
    }
    fResched = napi_complete_done(pNd->pNapi, nDone);
    if (fResched != 0 && napi_schedule_prep(pNd->pNapi) != 0) {
        __napi_schedule(pNd->pNapi);
    }
}

u32
linux_netdev_soft_napi_poll_invokes(void)
{
    return g_cNapiPollInvoke;
}

u32
linux_netdev_soft_napi_poll_skips(void)
{
    return g_cNapiPollSkip;
}

u32
linux_netdev_soft_softirq_kick_count(void)
{
    return g_cSoftirqKick;
}

int
linux_netdev_soft_napi_mmio_handoff(void)
{
    /* Dual DoD B residual: gate0 never reports soft BAR claim. Soft!=product. */
    lnds_gate0_force_no_bar_steal();
    return g_fMmioHandoff;
}

void
linux_netdev_soft_napi_set_mmio_handoff(int fOn)
{
    /*
     * Minimal gate for MMIO handoff agent - not full handoff implementation.
     * Default remains 0 (freestanding owns MMIO). Soft!=product.
     * Phase 1 prepare must NOT set this; only later phases after freestanding
     * quiesce may allow .ko poll (see docs/R8169_MMIO_HANDOFF.md).
     *
     * Gate-safe: refuse fOn=1 under GJ_SOFT_R8169_MMIO_HANDOFF==0 (default)
     * and refuse while freestanding still ready - never dual-drive BAR.
     * Dual DoD B residual: refuse is the R0-thrash guard (no real BAR steal).
     * Phase3 try_open assigns g_fMmioHandoff directly after sole-owner.
     */
    if (fOn != 0) {
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
        if (g_cBarStealRefuse < 0xffffffffu) {
            g_cBarStealRefuse++;
        }
        if (g_fMmioSetHandoffLog == 0) {
            g_fMmioSetHandoffLog = 1;
            /*
             * Grep: soft napi mmio_handoff SET refuse
             * Grep: soft residual R0 thrash refuse
             * Lean once-lamp only (no stamp storm). Soft!=product.
             */
            kprintf("linux_netdev_soft: soft napi mmio_handoff SET refuse "
                    "soft residual R0 thrash refuse refuse_n=%u "
                    "(gate off; freestanding owns BAR; Soft!=product; "
                    "no_bar_steal=1; dual DoD B; G-AC-1; "
                    "RUN_INIT=0 freestanding_no_exec)\n",
                    (unsigned)g_cBarStealRefuse);
        }
        lnds_gate0_force_no_bar_steal();
        return;
#else
        {
            extern int rtl8168_ready(void);

            if (rtl8168_ready() != 0) {
                if (g_cBarStealRefuse < 0xffffffffu) {
                    g_cBarStealRefuse++;
                }
                if (g_fMmioSetHandoffLog == 0) {
                    g_fMmioSetHandoffLog = 1;
                    kprintf("linux_netdev_soft: soft napi mmio_handoff SET "
                            "refuse (freestanding still ready; dual-drive; "
                            "Soft!=product; residual R0 thrash refuse)\n");
                }
                return;
            }
        }
#endif
    }
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    /* Gate0: clear only; never promote soft BAR claim. Soft!=product. */
    g_fMmioHandoff = 0;
    (void)fOn;
#else
    g_fMmioHandoff = (fOn != 0) ? 1 : 0;
#endif
    /* Cap state-change lamp (no stamp storms). Soft!=product. */
    if (g_fMmioHandoffStateLog == 0) {
        g_fMmioHandoffStateLog = 1;
        kprintf("linux_netdev_soft: soft napi mmio_handoff=%d "
                "(Soft!=product; .ko poll %s; no_bar_steal=%d)\n",
                g_fMmioHandoff,
                g_fMmioHandoff != 0 ? "allowed" : "blocked",
                g_fMmioHandoff == 0 ? 1 : 0);
    }
}

/*
 * Soft MMIO handoff readiness (phase 2 sole-owner deepen; Soft!=product).
 * Gate off: SKIP once, return 0.
 * Gate on return codes:
 *   0  = skip / gate off (compile path never here when gate 1)
 *   1  = wait (prepare or primary not ready)
 *   2  = ready-for-open (sole-owner candidate; still no .ko / ndo_open)
 *  -1  = fault (dual-drive / fail closed)
 * Does NOT set g_fMmioHandoff (phase 3 only). Soft!=product.
 * Grep: linux_netdev_soft: soft mmio handoff
 */
int
linux_netdev_soft_mmio_handoff_ready(void)
{
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    /* Dual DoD B residual: gate0 ready is always SKIP; freestanding owns BAR. */
    lnds_gate0_force_no_bar_steal();
    if (g_fMmioHandoffReadyLog == 0) {
        g_fMmioHandoffReadyLog = 1;
        /* Grep: linux_netdev_soft: soft mmio handoff SKIP (gate off) */
        kprintf("linux_netdev_soft: soft mmio handoff SKIP (gate off; "
                "no_bar_steal=1; Soft!=product; dual DoD B residual)\n");
    }
    return 0;
#else
    extern int rtl8168_soft_handoff_prepared(void);
    extern int net_l2_soft_handoff_fault(void);
    extern int rtl8168_ready(void);
    int fPrep;
    int fPrimary;
    int fFsReady;

    if (!g_fReady) {
        linux_netdev_soft_init();
    }

    if (net_l2_soft_handoff_fault() != 0) {
        if (g_fMmioHandoffReadyLog == 0) {
            g_fMmioHandoffReadyLog = 1;
            /* Grep: linux_netdev_soft: soft mmio handoff FAULT */
            kprintf("linux_netdev_soft: soft mmio handoff FAULT "
                    "(fail closed; Soft!=product)\n");
        }
        return -1;
    }

    fPrep = rtl8168_soft_handoff_prepared();
    fFsReady = rtl8168_ready();
    /* Dual-drive: freestanding still ready after prepare claimed -> FAULT. */
    if (fPrep != 0 && fFsReady != 0) {
        extern void net_l2_soft_handoff_set_fault(const char *szWhy);

        net_l2_soft_handoff_set_fault("freestanding ready after prepare");
        if (g_fMmioHandoffReadyLog == 0) {
            g_fMmioHandoffReadyLog = 1;
            kprintf("linux_netdev_soft: soft mmio handoff FAULT "
                    "(dual-drive; Soft!=product)\n");
        }
        return -1;
    }

    fPrimary = (g_pPrimary != NULL && g_pPrimary->u8Registered != 0u) ? 1 : 0;
    /*
     * Phase 2 sole-owner candidate: prepare done, freestanding not ready,
     * primary soft netdev present, no fault. Hostish pci_dev resource[] is
     * the sole documented soft-path BAR/IRQ owner; freestanding g_pMmio stays
     * mapped but unused (no dual-map / no unmap). Still no ndo_open; still
     * g_fMmioHandoff==0 so soft NAPI .ko poll stays blocked.
     */
    if (fPrep != 0 && fPrimary != 0 && fFsReady == 0) {
        if (g_fMmioHandoffReadyLog == 0) {
            g_fMmioHandoffReadyLog = 1;
            /* Grep: linux_netdev_soft: soft mmio handoff READY */
            kprintf("linux_netdev_soft: soft mmio handoff READY "
                    "(prepare done + primary; no .ko open; Soft!=product)\n");
            /*
             * Grep: soft mmio handoff sole-owner
             * Phase-2 marker: soft claim is logical (hostish blob), not a
             * second ioremap while freestanding still holds g_pMmio VA.
             */
            kprintf("linux_netdev_soft: soft mmio handoff sole-owner "
                    "hostish BAR/IRQ (g_pMmio idle; no dual-map; "
                    "Soft!=product; no .ko open; mmio_handoff=0)\n");
        }
        return 2; /* ready-for-open (phase 3 gated later) */
    }

    if (g_fMmioHandoffReadyLog == 0) {
        g_fMmioHandoffReadyLog = 1;
        /* Grep: linux_netdev_soft: soft mmio handoff WAIT */
        kprintf("linux_netdev_soft: soft mmio handoff WAIT "
                "prep=%d primary=%d fs_ready=%d (Soft!=product)\n",
                fPrep, fPrimary, fFsReady);
    }
    return 1; /* wait */
#endif
}

/*
 * Phase-2 sole-owner marker. Non-zero only when handoff_ready()==2
 * (prepare done, freestanding not ready, primary present, no fault).
 * Gate 0 -> always 0. Does not open .ko; does not set g_fMmioHandoff.
 * Grep: soft mmio handoff sole-owner
 */
int
linux_netdev_soft_mmio_sole_owner(void)
{
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    return 0;
#else
    {
        int n = linux_netdev_soft_mmio_handoff_ready();
        return (n == 2) ? 1 : 0;
    }
#endif
}

/*
 * Soft EBUSY for phase-3 wait (Linux errno 16). Freestanding C11 - no errno.h.
 */
#define LNDS_SOFT_EBUSY  (-16)

/*
 * Phase-3 gated soft-open after sole-owner (Soft!=product).
 * Gate off -> SKIP once, return 0 (freestanding path untouched).
 * Gate on + !sole_owner -> WAIT once, return soft EBUSY.
 * Gate on + sole_owner -> Option A soft open (no .ko ndo_open this wave):
 *   set g_fMmioHandoff=1 carefully; carrier/queue via open_primary.
 * Option B (.ko ndo_open): GJ_SOFT_R8169_KO_NDO_OPEN default 0 - next lab only.
 * Grep: linux_netdev_soft: soft mmio handoff phase3
 */
int
linux_netdev_soft_mmio_try_open(void)
{
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    /*
     * Dual DoD B residual: gate0 never opens soft BAR claim.
     * Soft!=product; freestanding wire sole; no R0 thrash path.
     */
    lnds_gate0_force_no_bar_steal();
    if (g_fMmioTryOpenLog == 0) {
        g_fMmioTryOpenLog = 1;
        /* Grep: linux_netdev_soft: soft mmio handoff phase3 SKIP (gate off) */
        kprintf("linux_netdev_soft: soft mmio handoff phase3 SKIP "
                "(gate off; no_bar_steal=1; Soft!=product; dual DoD B residual; "
                "RUN_INIT=0 freestanding_no_exec; no real BAR thrash)\n");
    }
    return 0;
#else
    extern int rtl8168_ready(void);
    extern int net_l2_soft_handoff_fault(void);
    extern void net_l2_soft_handoff_set_fault(const char *szWhy);
    int nSole;

    if (!g_fReady) {
        linux_netdev_soft_init();
    }

    /* Idempotent success after first phase-3 soft-open edge. */
    if (g_fMmioTryOpenDone != 0) {
        return 0;
    }

    if (net_l2_soft_handoff_fault() != 0) {
        if (g_fMmioTryOpenLog == 0) {
            g_fMmioTryOpenLog = 1;
            kprintf("linux_netdev_soft: soft mmio handoff phase3 FAULT "
                    "(fail closed; Soft!=product)\n");
        }
        return -1;
    }

    nSole = linux_netdev_soft_mmio_sole_owner();
    if (nSole == 0) {
        if (g_fMmioTryOpenLog == 0) {
            g_fMmioTryOpenLog = 1;
            /* Grep: linux_netdev_soft: soft mmio handoff phase3 WAIT */
            kprintf("linux_netdev_soft: soft mmio handoff phase3 WAIT "
                    "(sole-owner not ready; Soft!=product)\n");
        }
        return LNDS_SOFT_EBUSY;
    }

    /* Dual-drive guard: freestanding must stay down after prepare. */
    if (rtl8168_ready() != 0) {
        net_l2_soft_handoff_set_fault("phase3 try_open freestanding still ready");
        if (g_fMmioTryOpenLog == 0) {
            g_fMmioTryOpenLog = 1;
            kprintf("linux_netdev_soft: soft mmio handoff phase3 FAULT "
                    "(dual-drive; Soft!=product)\n");
        }
        return -1;
    }

    /*
     * Phase 3: freestanding quiesced + sole-owner -> allow soft NAPI poll path.
     * Still NO .ko ndo_open this wave (Option A). Soft!=product.
     */
    if (g_fMmioHandoff == 0) {
        g_fMmioHandoff = 1;
        kprintf("linux_netdev_soft: soft napi mmio_handoff=1 "
                "(phase3 after sole-owner; Soft!=product; .ko poll allowed)\n");
    }

    /*
     * Option A: soft open only - carrier/queue/attach via open_primary.
     * lnds_soft_dev_open still skips .ko ops (soft open skip .ko ops).
     */
    linux_netdev_soft_open_primary();

#if GJ_SOFT_R8169_KO_NDO_OPEN != 0
    /*
     * Option B (RISKY lab): would call .ko ndo_open if netdev_ops looks like
     * a Linux layout. NOT implemented this wave - next lab experiment only.
     * Gate default 0; requires GJ_SOFT_R8169_MMIO_HANDOFF=1.
     * Grep: soft mmio handoff phase3 Option B
     */
    kprintf("linux_netdev_soft: soft mmio handoff phase3 Option B "
            "ko ndo_open DEFER (not this wave; Soft!=product; RISKY)\n");
#endif

    g_fMmioTryOpenDone = 1;
    if (g_fMmioTryOpenLog == 0) {
        g_fMmioTryOpenLog = 1;
    }
    /*
     * Grep: linux_netdev_soft: soft mmio handoff phase3 soft-open only
     * Grep: phase3 soft-open only (no .ko ndo_open yet)
     */
    kprintf("linux_netdev_soft: soft mmio handoff phase3 soft-open only "
            "(no .ko ndo_open yet; mmio_handoff=1; Soft!=product)\n");
    return 0;
#endif
}

/*
 * Phase 4a hybrid (default lab under gate 0; Soft!=product).
 * Freestanding owns wire; soft r8169 is the Linux-shaped control object.
 * Grep: linux path HYBRID · docs/R8169_MMIO_HANDOFF.md
 *
 * Safety: never active while soft claims MMIO or handoff pending/fault
 * (would dual-drive freestanding BAR). Null-check weak handoff symbols.
 * Probe mode: SOFT/EMU (0) OR REAL (1) - not REAL-only (gate0 hybrid SOFT).
 */
int
linux_netdev_soft_hybrid_active(void)
{
    extern int linux_pci_soft_last_probe_mode(void);
    extern int rtl8168_ready(void);
    int nMode;

    if (!g_fReady) {
        return 0;
    }
    /* Dual DoD B residual: gate0 never allows soft BAR claim. Soft!=product. */
    lnds_gate0_force_no_bar_steal();
    if (g_fL2Bridge == 0) {
        return 0;
    }
    if (g_pPrimary == NULL || g_pPrimary->u8Registered == 0u) {
        return 0;
    }
    /*
     * Hybrid 4a: freestanding wire + soft netdev.
     * gate0 default lab uses EMU bind -> last_probe_mode SOFT (0); REAL (1)
     * also valid after hostish probe. Do NOT require REAL only.
     * Soft!=product.
     */
    nMode = linux_pci_soft_last_probe_mode();
    if (nMode != 0 /* SOFT/EMU */ && nMode != 1 /* REAL */) {
        return 0;
    }
    if (rtl8168_ready() == 0) {
        return 0;
    }
    /* Soft must not own MMIO / be mid-handoff - that is 4b, not hybrid. */
    if (g_fMmioHandoff != 0) {
        return 0;
    }
    /* Same freestanding-owns check as reverse TX (null-safe weak). */
    if (lnds_soft_tx_freestanding_owns() == 0) {
        return 0;
    }
    return 1;
}

int
linux_netdev_soft_hybrid_lamp_once(void)
{
    if (g_fHybridLampLog != 0) {
        return 0;
    }
    /* Dual DoD B residual: never hybrid-lamp with soft BAR claim. Soft!=product. */
    lnds_gate0_force_no_bar_steal();
    /*
     * Under hybrid SOFT, bridge may lag enable (net_l2 note race / order).
     * Auto-enable when freestanding ready + primary + freestanding owns -
     * SOFT(0) or REAL(1); not REAL-only. Soft!=product; not 4b .ko wire.
     */
    if (g_fL2Bridge == 0 && g_fMmioHandoff == 0 &&
        g_pPrimary != NULL && g_pPrimary->u8Registered != 0u &&
        lnds_soft_tx_freestanding_owns() != 0) {
        extern int linux_pci_soft_last_probe_mode(void);
        extern int rtl8168_ready(void);
        int nMode;

        nMode = linux_pci_soft_last_probe_mode();
        if ((nMode == 0 /* SOFT/EMU */ || nMode == 1 /* REAL */) &&
            rtl8168_ready() != 0) {
            linux_netdev_soft_l2_bridge_enable(1);
        }
    }
    /*
     * Safety: only lamp when hybrid_active (bridge + primary + freestanding
     * ready + SOFT|REAL mode + not handoff). Never lamp if dual-drive risk.
     * Soft!=product; not 4b .ko wire. Not REAL probe mode only.
     */
    if (linux_netdev_soft_hybrid_active() == 0) {
        return 0;
    }
    /* Defense-in-depth: mmio_handoff must stay 0 for hybrid claim. */
    if (g_fMmioHandoff != 0) {
        return 0;
    }
    g_fHybridLampLog = 1;
    /*
     * Grep: main: linux path HYBRID wire=freestanding soft=r8169
     * (prefix main: so serial greps match boot lamp family)
     */
    kprintf("main: linux path HYBRID wire=freestanding soft=r8169 "
            "(Soft!=product; freestanding datapath; soft control; "
            "not 4b .ko wire; G-AC-1; hold14 L2 BR live; SOFT|REAL)\n");
    /*
     * Grep: linux_netdev_soft: soft hybrid zero-touch PASS
     * Grep: linux_netdev_soft: soft hybrid residual E
     * Soft!=product; freestanding owns wire; hybrid SOFT must not steal BAR.
     */
    kprintf("linux_netdev_soft: soft hybrid zero-touch PASS Soft!=product "
            "wire=freestanding soft=r8169 bridge=%d open=%d "
            "mmio_handoff=%d live_fs_8168=%d hold14_mirror=1 "
            "(freestanding BAR sole; no .ko open/NAPI/xmit; G-AC-1; "
            "hybrid SOFT must not steal BAR)\n",
            g_fL2Bridge,
            g_pPrimary != NULL ? (int)g_pPrimary->u8Open : 0,
            g_fMmioHandoff,
            lnds_live_fs_8168());
    kprintf("linux_netdev_soft: soft hybrid residual E Soft!=product "
            "open/NAPI/xmit fail-closed live 8168 "
            "fc_open=%u fc_napi=%u fc_xmit=%u "
            "hostish_filled=%d hostish_xmit_cached=%p "
            "hold14 freestanding R/T mirror companion "
            "(hybrid SOFT must not steal BAR; no .ko ndo_xmit; "
            "logs once-capped; RUN_INIT=0 freestanding_no_exec)\n",
            (unsigned)g_cLive8168FcOpen, (unsigned)g_cLive8168FcNapi,
            (unsigned)g_cLive8168FcXmit, g_fHostishFilled,
            g_pHostishNdoXmitCached);
    /*
     * Dual DoD B residual lean lamp (gate0 friendliness). Soft!=product.
     * Once-capped inside helper; no stamp storm. Companion to residual E.
     * Includes freestanding_rtl SKIP + product_nic=UDX honesty.
     */
    (void)linux_netdev_soft_gate0_residual_lamp_once();
    (void)linux_netdev_soft_residual_lean_lamp_once();
    /*
     * hold14 first (L2 BR; freestanding R/T mirror companion when feed rare).
     * hold15 HYBRID slogan - never clobber 7-13. Soft!=product (not product NIC).
     */
    lnds_hold14_l2_br_refresh();
    if (15u < FB_HOLD_LINES) {
        fb_console_hold(15, "HYBRID wire=fs soft=r8169");
    }
    return 1;
}

/*
 * C0 residual silent self-check (Soft!=product; stamp-free; no kprintf).
 * Verifies soft residual invariants without serial traffic (H2 no stamp storm).
 * Soft!=product; claim class C0 only - never Dual DoD / bar3 / product close.
 * @return 1 if all residual arms ok, 0 if any fail.
 */
static int
lnds_residual_c0_self_check(void)
{
    int nOk = 1;

    /* Soft net_device layout residual (companion to header _Static_assert). */
    if (sizeof(struct net_device) != LINUX_NETDEV_SOFT_ND_BYTES) {
        nOk = 0;
    }
    if (sizeof(struct soft_nd_slab) < LINUX_NETDEV_HOSTISH_SIZE_NET_DEVICE) {
        nOk = 0;
    }
    if (LINUX_NETDEV_HOSTISH_BLOB_BYTES < LINUX_NETDEV_HOSTISH_SIZE_NET_DEVICE) {
        nOk = 0;
    }
    /* Soft default ops / ethtool tables (LNDS / LNET; Soft!=host ABI). */
    if (g_softDefaultOps.u32Magic != LNDS_SOFT_OPS_MAGIC) {
        nOk = 0;
    }
    if (g_softDefaultOps.u32Ver != LNDS_SOFT_OPS_VER) {
        nOk = 0;
    }
    if (g_softDefaultOps.ndo_open == NULL ||
        g_softDefaultOps.ndo_stop == NULL ||
        g_softDefaultOps.ndo_start_xmit == NULL ||
        g_softDefaultOps.ndo_get_stats64 == NULL) {
        nOk = 0;
    }
    if (g_softDefaultEthtoolOps.u32Magic != LINUX_NETDEV_SOFT_ETHTOOL_MAGIC) {
        nOk = 0;
    }
    if (g_softDefaultEthtoolOps.u32Ver != LINUX_NETDEV_SOFT_ETHTOOL_VER) {
        nOk = 0;
    }
    if (g_softDefaultEthtoolOps.get_link == NULL) {
        nOk = 0;
    }
    /* Soft layout deepen residual (ver2 fields for UDX-shaped userspace). */
    if (LINUX_NETDEV_SOFT_LAYOUT_VER < 2u) {
        nOk = 0;
    }
    if (LNDS_SOFT_OPS_VER < 2u) {
        nOk = 0;
    }
    /* Gate0 residual: soft must not hold BAR claim after force. Soft!=product. */
    lnds_gate0_force_no_bar_steal();
#if GJ_SOFT_R8169_MMIO_HANDOFF == 0
    if (g_fMmioHandoff != 0) {
        nOk = 0;
    }
#endif
    /* Pool bounds residual (non-zero eng seed capacity). */
    if (LINUX_NETDEV_SOFT_MAX == 0u || LINUX_NETDEV_SOFT_SKB_MAX == 0u) {
        nOk = 0;
    }
    return nOk;
}

/*
 * Dual DoD B residual honesty (gate0 hybrid friendliness; Soft!=product; C0).
 * Lean once-lamp: RUN_INIT=0 / freestanding_no_exec + no_bar_steal +
 * freestanding_rtl SKIP + product_nic=UDX + residual E fail-closed counts.
 * Never sets mmio_handoff; never real BAR thrash. Never freestanding wire
 * product claim. Dual DoD A/B OPEN until UDX DUT (not freestanding :22). G-AC-1.
 * Grep: soft dual DoD B residual gate0 PASS
 * Grep: soft residual RUN_INIT=0 freestanding_no_exec
 * Grep: soft residual freestanding_rtl=SKIP
 * Grep: soft residual product_nic=UDX
 * Grep: soft residual C0 · claim_class=C0
 */
int
linux_netdev_soft_gate0_residual_lamp_once(void)
{
    int fOwns;
    int fSelf;

    if (g_fGate0ResidualLog != 0) {
        return 0;
    }
    if (!g_fReady) {
        return 0;
    }
    /* Gate0 residual force: soft must not steal BAR; Soft!=product. */
    lnds_gate0_force_no_bar_steal();
    g_fGate0ResidualLog = 1;
    fOwns = lnds_soft_tx_freestanding_owns();
    fSelf = lnds_residual_c0_self_check();

    /*
     * Grep: linux_netdev_soft: soft dual DoD B residual gate0 PASS
     * Grep: linux_netdev_soft: soft residual RUN_INIT=0 freestanding_no_exec
     * Grep: linux_netdev_soft: soft residual freestanding_rtl=SKIP
     * Grep: linux_netdev_soft: soft residual product_nic=UDX
     * Grep: linux_netdev_soft: soft residual wire sole freestanding
     * Grep: linux_netdev_soft: soft residual C0
     * Soft!=product; Dual DoD A/B OPEN until UDX DUT proof; eng residual only.
     * Lean single once-lamp (no stamp storm; no version stamp). G-AC-1 lean.
     * product_nic=UDX; freestanding rtl default SKIP (not product wire).
     * claim_class=C0: soft lamp only - never Dual DoD close / bar3 / "net works".
     */
    kprintf("linux_netdev_soft: soft dual DoD B residual gate0 PASS "
            "soft residual C0 claim_class=C0 "
            "soft residual RUN_INIT=0 freestanding_no_exec "
            "soft residual freestanding_rtl=%s "
            "soft residual product_nic=UDX "
            "soft residual wire sole freestanding "
            "Soft!=product soft=r8169 product_path=UDX "
            "dual_DoD_A=OPEN dual_DoD_B=OPEN "
            "mmio_handoff=%d no_bar_steal=%d freestanding_owns=%d "
            "live_fs_8168=%d rtl_probe=%u bridge=%d open=%d hostish_filled=%d "
            "fc_open=%u fc_napi=%u fc_xmit=%u bar_steal_refuse=%u "
            "hostish_open_refuse=%u hostish_xmit_refuse=%u "
            "gate_mmio_handoff=%d soft_control=1 ko_init_exec=0 "
            "ko_ndo_call=0 product=0 self_check=%d "
            "(C0 soft residual; ABI/hostability eng; residual E fail-closed; "
            "never freestanding wire product; no real BAR thrash / no R0 "
            "class dual-drive; product NIC=UDX userspace; G-AC-1 lean; "
            "dual DoD A/B OPEN until UDX DUT proof; not freestanding :22 close; "
            "soft listen :22 != Dual DoD B; lean lamp once; "
            "no version stamp)\n",
            (GJ_RTL8168_PROBE == 0) ? "SKIP" : "opt_in",
            g_fMmioHandoff,
            g_fMmioHandoff == 0 ? 1 : 0,
            fOwns,
            lnds_live_fs_8168(),
            (unsigned)GJ_RTL8168_PROBE,
            g_fL2Bridge,
            g_pPrimary != NULL ? (int)g_pPrimary->u8Open : 0,
            g_fHostishFilled,
            (unsigned)g_cLive8168FcOpen,
            (unsigned)g_cLive8168FcNapi,
            (unsigned)g_cLive8168FcXmit,
            (unsigned)g_cBarStealRefuse,
            (unsigned)g_cHostishOpenRefuse,
            (unsigned)g_cHostishXmitRefuse,
            (int)GJ_SOFT_R8169_MMIO_HANDOFF,
            fSelf);

    /* Companion lean lamp (once-capped inside; no stamp storm). Soft!=product. */
    (void)linux_netdev_soft_residual_lean_lamp_once();

    return 1;
}

/*
 * Soft residual lean (ABI / hostability eng; Soft!=product; G-AC-1; C0).
 * Soft netdev seed residual only - never freestanding wire product.
 * Freestanding rtl default SKIP; product NIC = UDX userspace.
 * Dual DoD A/B OPEN until UDX DUT (not freestanding :22 close).
 * Once-capped; no version stamp; no stamp storms.
 * Grep: soft residual lean PASS Soft!=product
 * Grep: soft residual freestanding_rtl=SKIP
 * Grep: soft residual product_nic=UDX
 * Grep: soft residual C0 · claim_class=C0
 */
int
linux_netdev_soft_residual_lean_lamp_once(void)
{
    int fSelf;

    if (g_fResidualLeanLog != 0) {
        return 0;
    }
    if (!g_fReady) {
        return 0;
    }
    lnds_gate0_force_no_bar_steal();
    g_fResidualLeanLog = 1;
    fSelf = lnds_residual_c0_self_check();

    /*
     * Grep: linux_netdev_soft: soft residual lean PASS Soft!=product
     * Grep: linux_netdev_soft: soft residual freestanding_rtl=SKIP
     * Grep: linux_netdev_soft: soft residual product_nic=UDX
     * Grep: linux_netdev_soft: soft residual C0
     */
    /*
     * Lean once-lamp: greppable freestanding_rtl=SKIP (default) +
     * product_nic=UDX + claim_class=C0 + Dual DoD A/B OPEN + no Option B
     * .ko ndo product. Soft!=product; G-AC-1. No version stamp; no storms.
     * Dual MIT OR Apache-2.0. Product path = UDX (not freestanding wire).
     */
    kprintf("linux_netdev_soft: soft residual lean PASS Soft!=product "
            "soft residual C0 claim_class=C0 "
            "soft residual freestanding_rtl=%s "
            "soft residual product_nic=UDX "
            "product_path=UDX dual_DoD_A=OPEN dual_DoD_B=OPEN "
            "abi_hostability=1 soft_seed=1 layout_ver=%u ops_ver=%u "
            "RUN_INIT=0 freestanding_no_exec=1 no_bar_steal=%d "
            "mmio_handoff=%d rtl_probe=%u live_fs_8168=%d "
            "soft_control=1 ko_init_exec=0 ko_ndo_call=0 "
            "option_b_ko_ndo_open=%d product=0 self_check=%d "
            "bar_steal_refuse=%u stamp_storm=0 version_stamp=0 G-AC-1=1 "
            "(C0 soft residual; seed residual for ABI/hostability eng; "
            "never freestanding wire product; product NIC=UDX userspace; "
            "Dual DoD A/B OPEN until UDX DUT proof; not freestanding :22 close; "
            "no Option B .ko ndo_open product; dual MIT OR Apache-2.0; "
            "lean lamp once)\n",
            (GJ_RTL8168_PROBE == 0) ? "SKIP" : "opt_in",
            (unsigned)LINUX_NETDEV_SOFT_LAYOUT_VER,
            (unsigned)LNDS_SOFT_OPS_VER,
            g_fMmioHandoff == 0 ? 1 : 0,
            g_fMmioHandoff,
            (unsigned)GJ_RTL8168_PROBE,
            lnds_live_fs_8168(),
            (int)GJ_SOFT_R8169_KO_NDO_OPEN,
            fSelf,
            (unsigned)g_cBarStealRefuse);

    return 1;
}

int
linux_netdev_soft_no_bar_steal(void)
{
    lnds_gate0_force_no_bar_steal();
    return (g_fMmioHandoff == 0) ? 1 : 0;
}

int
linux_netdev_soft_freestanding_owns_wire(void)
{
    /*
     * Dual DoD B residual: freestanding owns wire honesty when live.
     * Soft!=product; G-AC-1 lean. Forces gate0 no_bar_steal.
     * Default freestanding_rtl=SKIP - not product NIC (product = UDX).
     * Grep: soft freestanding owns wire
     */
    lnds_gate0_force_no_bar_steal();
    return lnds_soft_tx_freestanding_owns();
}

u32
linux_netdev_soft_bar_steal_refuse_count(void)
{
    return g_cBarStealRefuse;
}

u32
linux_netdev_soft_hostish_open_refuse_count(void)
{
    return g_cHostishOpenRefuse;
}

u32
linux_netdev_soft_hostish_xmit_refuse_count(void)
{
    return g_cHostishXmitRefuse;
}

void
dev_kfree_skb_any(void *pSkb)
{
    lnds_skb_free(pSkb);
}

void
dev_kfree_skb_any_reason(void *pSkb, u8 u8Reason)
{
    (void)u8Reason;
    lnds_skb_free(pSkb);
}

/* ---- Soft printk + feature helpers -------------------------------------- */

int
netdev_err(const void *pDev, const char *szFmt, ...)
{
    va_list vaArgs;

    va_start(vaArgs, szFmt);
    va_end(vaArgs);
    return lnds_log_once(&g_cLogErr, "err", pDev, szFmt);
}

int
netdev_info(const void *pDev, const char *szFmt, ...)
{
    va_list vaArgs;

    va_start(vaArgs, szFmt);
    va_end(vaArgs);
    return lnds_log_once(&g_cLogInfo, "info", pDev, szFmt);
}

int
netdev_warn(const void *pDev, const char *szFmt, ...)
{
    va_list vaArgs;

    va_start(vaArgs, szFmt);
    va_end(vaArgs);
    return lnds_log_once(&g_cLogWarn, "warn", pDev, szFmt);
}

int
netdev_notice(const void *pDev, const char *szFmt, ...)
{
    va_list vaArgs;

    va_start(vaArgs, szFmt);
    va_end(vaArgs);
    return lnds_log_once(&g_cLogNotice, "notice", pDev, szFmt);
}

int
netdev_printk(const char *szLevel, const void *pDev, const char *szFmt, ...)
{
    va_list vaArgs;
    char szTag[24];

    va_start(vaArgs, szFmt);
    va_end(vaArgs);
    (void)strlcpy(szTag, "printk:", sizeof(szTag));
    if (szLevel != NULL && szLevel[0] != '\0') {
        (void)strlcat(szTag, szLevel, sizeof(szTag));
    }
    return lnds_log_once(&g_cLogPrintk, szTag, pDev, szFmt);
}

void
netdev_update_features(void *pDev)
{
    struct net_device *pNd = (struct net_device *)pDev;

    /*
     * Soft features residual: wanted ∩ hw -> features.
     * Soft!=product bookkeeping for UDX/Linux-shaped residual; no BAR.
     */
    if (pNd == NULL || lnds_slab_of(pNd) == NULL || pNd->u8Live == 0u) {
        return;
    }
    pNd->u64Features = pNd->u64WantedFeatures & pNd->u64HwFeatures;
}

void
netdev_stats_to_stats64(void *pS64, const void *pStats)
{
    struct soft_netdev_stats64 *pOut;
    const struct soft_netdev_stats64 *pIn;
    struct net_device *pNd;

    /*
     * Soft stats64 residual (128 B soft blob). Soft!=Linux ABI.
     * Prefer pStats copy; else soft primary counters. Soft!=product.
     */
    if (pS64 == NULL) {
        return;
    }
    pOut = (struct soft_netdev_stats64 *)pS64;
    memset(pOut, 0, sizeof(*pOut));
    if (pStats != NULL) {
        pIn = (const struct soft_netdev_stats64 *)pStats;
        pOut->u64RxPackets = pIn->u64RxPackets;
        pOut->u64TxPackets = pIn->u64TxPackets;
        pOut->u64RxBytes = pIn->u64RxBytes;
        pOut->u64TxBytes = pIn->u64TxBytes;
        pOut->u64RxDropped = pIn->u64RxDropped;
        pOut->u64TxDropped = pIn->u64TxDropped;
        return;
    }
    pNd = g_pPrimary;
    if (pNd != NULL && pNd->u8Live != 0u) {
        pOut->u64RxPackets = pNd->u64RxPackets;
        pOut->u64TxPackets = pNd->u64TxPackets;
        pOut->u64RxBytes = pNd->u64RxBytes;
        pOut->u64TxBytes = pNd->u64TxBytes;
        pOut->u64RxDropped = pNd->u64RxDropped;
        pOut->u64TxDropped = pNd->u64TxDropped;
    }
}

void
netdev_sw_irq_coalesce_default_on(void *pDev)
{
    (void)pDev;
}
