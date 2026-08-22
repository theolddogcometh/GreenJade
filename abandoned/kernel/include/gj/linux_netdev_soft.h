/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux netdev lifecycle seed (clean-room).
 * Dual MIT OR Apache-2.0. No GPL / no Linux source.
 *
 * Purpose
 * -------
 * Soft surface so r8169-style init can register a soft net_device and later
 * feed RX/TX without a product Linux net stack. Complements F2 linux_ksym
 * resolution: this TU owns the real soft bodies; linux_netdev_soft_init()
 * registers them via linux_ksym_register when F2 is linked.
 *
 * Soft != ABI-stable
 * -----------------
 * struct net_device here is an INCOMPLETE soft shape (front fields only,
 * pad to ≥2 KiB). Field order/size are NOT guaranteed to match any Linux
 * kernel version. Soft modules / tests built against this header are the
 * intended consumers until a product ABI exists. Soft != product RX/TX path.
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
 *   linux_netdev_soft: soft l2 bridge ON
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
 *   main: linux path HYBRID wire=freestanding soft=r8169  (phase 4a; via lamp_once)
 *   STATUS hold14: l2 br rx=N tx=M  · hold15: HYBRID wire=fs soft=r8169
 *   linux_netdev_soft: soft hold14 freestanding R/T mirror rx=... tx=...
 *
 * Soft residual lean (ABI / hostability eng · Soft!=product · G-AC-1)
 * -------------------------------------------------------------------
 * Soft netdev seed residual hosts a Linux-shaped control object for module-
 * path / ksym hostability eng only. Never claim freestanding wire product.
 * Freestanding rtl default SKIP (GJ_RTL8168_PROBE=0). Product NIC =
 * userspace UDX (rtl8168_udx) over hot+cold ABI / DDI caps. Soft!=product.
 * Once-capped lamps only - no version stamp, no stamp storms. Dual MIT OR
 * Apache-2.0. Grep: soft residual lean PASS · freestanding_rtl=SKIP ·
 * product_nic=UDX
 *
 * Hybrid residual E (gate0; opt-in freestanding only): when freestanding
 * rtl is live (GJ_RTL8168_PROBE=1 residual), it may own 10ec:8168 BAR/wire.
 * Soft open / NAPI / xmit fail closed for live 8168 (.ko ndo never dual-
 * drive). Hostish dual blob is inventory-only. Logs once-capped (no stamp
 * storms). hold14 freestanding R/T mirror companion when live. Soft!=product;
 * G-AC-1. Hybrid SOFT must not steal BAR. Default freestanding_rtl=SKIP.
 *
 * Dual DoD B residual (gate0 hybrid friendliness · eng only)
 * ---------------------------------------------------------
 * Lab dual DoD B may still use freestanding rtl wire for arping/ping/:22
 * honesty when deliberately enabled; default freestanding_rtl=SKIP.
 * Soft r8169 is Linux-shaped control object only. Soft residual MUST:
 *   - never steal freestanding BAR / wire (g_fMmioHandoff stays 0 at gate0)
 *   - never real BAR thrash that yields freestanding R0 (H4)
 *   - honor RUN_INIT=0 / freestanding_no_exec (no .ko init exec in kernel)
 *   - never claim freestanding wire as product NIC (product = UDX userspace)
 * Soft residual != product G-AC-1 lean: once-capped lamps only; no version
 * stamp; no stamp storms; dual MIT OR Apache-2.0. Dual DoD B OPEN until DUT.
 * Grep: soft dual DoD B residual gate0 · soft residual lean PASS
 *       soft residual freestanding_rtl=SKIP · soft residual product_nic=UDX
 *       soft residual RUN_INIT=0 · soft no_bar_steal
 *       soft residual R0 thrash refuse · soft residual wire sole freestanding
 *       soft freestanding owns wire
 *
 * See docs/LINUX_MODULE_PATH.md · docs/LAPTOP_LINUX_DRIVER_HOST.md ·
 * docs/ABI_FIRST_PIVOT.md · docs/R8169_MMIO_HANDOFF.md · docs/TODO.md.
 */
#pragma once

#include <gj/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Soft pool bounds (r8169 soft bring-up density). */
#define LINUX_NETDEV_SOFT_MAX         4u
#define LINUX_NETDEV_SOFT_SKB_MAX     16u
#define LINUX_NETDEV_SOFT_NAME_LEN    16u
#define LINUX_NETDEV_SOFT_ADDR_LEN    32u
#define LINUX_NETDEV_SOFT_ND_BYTES    2048u
#define LINUX_NETDEV_SOFT_PRIV_MAX    4096u
#define LINUX_NETDEV_SOFT_SKB_DATA    2048u

/*
 * Soft layout deepen stamp (eng residual for UDX/Linux-shaped userspace).
 * Bump when soft net_device front field set grows; Soft!=host ABI.
 * Grep: soft layout deepen PASS layout_ver=
 */
#define LINUX_NETDEV_SOFT_LAYOUT_VER  2u

/* Soft ether constants (Linux-shaped names; not host ABI). */
#define LINUX_NETDEV_SOFT_ARPHRD_ETHER  1u
#define LINUX_NETDEV_SOFT_ETH_HLEN      14u
#define LINUX_NETDEV_SOFT_ETH_ALEN      6u
#define LINUX_NETDEV_SOFT_MIN_MTU       68u
#define LINUX_NETDEV_SOFT_MAX_MTU       9000u

/*
 * Soft-owned netdev_ops table magic (LNDS). Distinguishes our soft table
 * from a .ko net_device_ops pointer. Soft ops layout != host ops offsets:
 *   Soft: open/stop first after magic/ver; ver2 appends more slots
 *   Host: ndo_init@0 ndo_open@0x10 ndo_start_xmit@0x20 (see hostish_off.h)
 * Soft!=product; never call .ko ops while freestanding owns wire.
 */
#define LINUX_NETDEV_SOFT_OPS_MAGIC   0x4C4E4453u /* 'LNDS' */
#define LINUX_NETDEV_SOFT_OPS_VER     2u

/**
 * Soft-shaped netdev_ops (incomplete Soft!=Linux ABI).
 * Ver1 prefix: ndo_open / ndo_stop. Ver2 deepen: init/uninit/xmit/mac/mtu/
 * timeout/stats64 - useful for UDX/Linux-shaped userspace residual later.
 * Soft start_xmit must fail closed for live freestanding 8168 (residual E).
 */
struct soft_netdev_ops {
    u32  u32Magic;
    u32  u32Ver;
    /* Ver1 prefix (keep stable offsets for soft open path). */
    int (*ndo_open)(void *pDev);
    int (*ndo_stop)(void *pDev);
    /* Ver2 soft deepen (UDX residual; Soft!=host net_device_ops layout). */
    int  (*ndo_init)(void *pDev);
    void (*ndo_uninit)(void *pDev);
    int  (*ndo_start_xmit)(void *pSkb, void *pDev);
    int  (*ndo_set_mac_address)(void *pDev, void *pSa);
    int  (*ndo_validate_addr)(void *pDev);
    int  (*ndo_change_mtu)(void *pDev, int nNewMtu);
    void (*ndo_tx_timeout)(void *pDev, unsigned uTxq);
    void (*ndo_get_stats64)(void *pDev, void *pStats);
};

/**
 * Soft-shaped ethtool_ops residual (Soft!=Linux ethtool_ops ABI).
 * Soft get_link / get_ts_info map to existing ksym soft bodies.
 */
#define LINUX_NETDEV_SOFT_ETHTOOL_MAGIC  0x4C4E4554u /* 'LNET' */
#define LINUX_NETDEV_SOFT_ETHTOOL_VER    1u

struct soft_ethtool_ops {
    u32  u32Magic;
    u32  u32Ver;
    u32 (*get_link)(void *pDev);
    int (*get_ts_info)(void *pDev, void *pInfo);
};

/**
 * Soft rtnl_link_stats64-ish blob (Soft!=Linux ABI; fixed soft size).
 * Used by soft ndo_get_stats64 / netdev_stats_to_stats64 residual.
 */
struct soft_netdev_stats64 {
    u64 u64RxPackets;
    u64 u64TxPackets;
    u64 u64RxBytes;
    u64 u64TxBytes;
    u64 u64RxErrors;
    u64 u64TxErrors;
    u64 u64RxDropped;
    u64 u64TxDropped;
    u64 u64Multicast;
    u64 u64Collisions;
    u64 u64RxLengthErrors;
    u64 u64RxOverErrors;
    u64 u64RxCrcErrors;
    u64 u64RxFrameErrors;
    u64 u64RxFifoErrors;
    u64 u64TxAbortedErrors;
};

/**
 * Soft net_device (Linux *shape*, not layout-stable).
 *
 * Front fields probe code may write: name / ml_priv / netdev_ops / flags /
 * mtu / dev_addr. Soft deepen residual (layout_ver≥2): type / hard_header /
 * addr_len / ifindex / features / ethtool_ops / soft stats counters.
 * Remaining bytes zeroed pad so accidental mid-struct stores within 2 KiB
 * do not walk off the object.
 *
 * Soft bookkeeping lives after the classic front fields (not Linux ABI).
 * Hostish dual blob (0xaf0) is separate - Soft!=host field order.
 */
struct net_device {
    char  name[LINUX_NETDEV_SOFT_NAME_LEN];
    void *ml_priv;
    void *netdev_ops;
    u32   flags;
    u32   mtu;
    u8    dev_addr[LINUX_NETDEV_SOFT_ADDR_LEN];

    /* Soft-only bookkeeping (not Linux ABI). */
    u8    u8Live;
    u8    u8Registered;
    u8    u8Carrier;
    u8    u8Attached;
    u8    u8QueueStopped;
    u8    u8Devm;
    u8    u8Open;           /* soft open state (not .ko ndo_open) */
    u8    u8SoftOpsOwned;   /* 1 if netdev_ops is soft default table */
    u16   u16TxQs;
    u16   u16RxQs;
    int   nSizeofPriv;
    void *pParentDev;
    void *pNapi;
    void *pfnNapiPoll;
    int   nNapiWeight;
    u32   u32TsoMaxSize;
    u32   u32TsoMaxSegs;
    u32   u32Slot;

    /*
     * Soft layout deepen (LINUX_NETDEV_SOFT_LAYOUT_VER ≥ 2).
     * Linux-shaped residual for UDX userspace later - Soft!=host offsets.
     * Grep: soft layout deepen PASS
     */
    u16   u16Type;            /* ARPHRD_ETHER-ish */
    u16   u16HardHeaderLen;   /* ETH_HLEN-ish */
    u8    u8AddrLen;          /* ETH_ALEN-ish */
    u8    u8PadLayout[1];
    u16   u16MinMtu;
    u32   u32MaxMtu;
    int   nIfindex;           /* soft ifindex (1 + slot) */
    void *ethtool_ops;        /* soft ethtool table or NULL */
    u64   u64Features;        /* soft netdev features bits */
    u64   u64HwFeatures;
    u64   u64WantedFeatures;
    u64   u64VlanFeatures;
    /* Soft datapath counters (bookkeeping; freestanding owns wire). */
    u64   u64RxPackets;
    u64   u64TxPackets;
    u64   u64RxBytes;
    u64   u64TxBytes;
    u64   u64RxDropped;
    u64   u64TxDropped;
    u32   u32SoftLayoutVer;   /* LINUX_NETDEV_SOFT_LAYOUT_VER at alloc */
    u32   u32PadAlign;

    /*
     * Pad so sizeof(struct net_device) == LINUX_NETDEV_SOFT_ND_BYTES.
     * Soft modules may touch early fields without walking off the slab.
     * Front used = 240 (LP64); update if fields added above.
     */
    u8 abFrontPad[LINUX_NETDEV_SOFT_ND_BYTES - 240u];
};

/*
 * Compile-time soft layout check: deepen front ends at offset 240
 * (x86_64 / aarch64 freestanding LP64). If a field is added above, grow
 * the constant or shrink abFrontPad so the total remains 2048.
 */
_Static_assert(sizeof(struct net_device) == LINUX_NETDEV_SOFT_ND_BYTES,
               "soft net_device must be exactly 2048 bytes");
_Static_assert(sizeof(struct soft_netdev_ops) >= 48u,
               "soft_netdev_ops must cover ver2 deepen slots");
_Static_assert(sizeof(struct soft_netdev_stats64) == 128u,
               "soft_netdev_stats64 fixed soft size");
/**
 * Soft init: idempotent ready lamp + optional ksym export of soft bodies.
 * Prefer: linux_ksym_register("register_netdev", register_netdev) ...
 * when F2 linux_ksym is linked (weak-call safe if absent).
 * Grep: linux_netdev_soft: soft init PASS
 */
void linux_netdev_soft_init(void);

/** Non-zero after successful soft init. */
int  linux_netdev_soft_ready(void);

/**
 * First successfully registered soft netdev still live, or NULL.
 * Soft getter for r8169 / later RX-TX wiring.
 */
void *linux_netdev_soft_primary(void);

/** Count of currently registered soft netdevs. */
int   linux_netdev_soft_count(void);

/**
 * Soft L2 bridge (freestanding HW ↔ soft Linux netdev path).
 * Soft!=product: freestanding rtl/virtio still owns MMIO; bridge copies frames
 * into soft skb/receive accounting so REAL probe + soft netdev share a path.
 * Grep: linux_netdev_soft: soft l2 bridge ...
 */
void  linux_netdev_soft_l2_bridge_enable(int fOn);
int   linux_netdev_soft_l2_bridge_enabled(void);
/**
 * Soft open primary netdev (carrier/queue/start_queue). Soft!=product.
 * Does NOT call .ko ndo_open (MMIO freestanding owns). Soft-shaped ops
 * tables (LNDS magic) may run optional soft ndo_open only.
 * Grep: linux_netdev_soft: soft open primary
 */
void  linux_netdev_soft_open_primary(void);
/**
 * Read-only ops layout diagnostic on soft primary (Soft!=product).
 * Logs soft netdev_ops field vs hostish word at +0x8 (RHEL 5.14 oracle)
 * and whether either pointer falls in soft-loaded r8169 image range.
 * Never calls ndo_open / never enables Option B. Once-log.
 * Grep: linux_netdev_soft: soft ops diagnostic
 * See docs/R8169_KO_NDO_OPEN.md.
 */
void  linux_netdev_soft_ops_diagnostic(void);

/*
 * Strategy A hostish net_device (≥0xaf0 blob; Soft!=product).
 * After REAL register_netdev, soft primary bytes are memcpy'd into a dual
 * hostish pool slot (captures .ko host-offset stores into the soft slab).
 * Soft bookkeeping stays on soft struct. No ndo_open call. Option B later.
 * Offsets: gj/linux_netdev_hostish_off.h (RHEL 5.14 oracle).
 * Grep: linux_netdev_soft: soft hostish net_device fill PASS
 */

/**
 * Dual hostish net_device blob for soft primary (or NULL if not filled).
 * Size LINUX_NETDEV_HOSTISH_BLOB_BYTES (≥ host sizeof 0xaf0).
 */
void *linux_netdev_soft_hostish_primary(void);

/** Non-zero if hostish blob filled after register. Soft!=product. */
int   linux_netdev_soft_hostish_filled(void);

/**
 * Hostish netdev_ops pointer: *(void **)((u8 *)pNd + host OFF_NETDEV_OPS).
 * pNd may be soft primary VA (same slab .ko mutated) or hostish blob.
 * NULL if pNd NULL. Never calls ndo_open.
 */
void *linux_netdev_soft_hostish_ops(void *pNd);

/**
 * Hostish ndo_open: ops = hostish_ops(pNd); then *(void **)(ops + 0x10).
 * NULL if ops missing. Cached on fill for Option B readiness. No call.
 */
void *linux_netdev_soft_hostish_ndo_open(void *pNd);

/**
 * Hostish ndo_stop @ ops+0x18 / ndo_start_xmit @ ops+0x20 (RHEL 5.14).
 * Soft deepen residual for UDX ops-table inventory. Never invokes. Soft!=product.
 */
void *linux_netdev_soft_hostish_ndo_stop(void *pNd);
void *linux_netdev_soft_hostish_ndo_start_xmit(void *pNd);

/**
 * Cached hostish ops / ndo slots from last hostish_fill (Option B readiness).
 * Soft!=product; inventory only - never call through these pointers under
 * residual E (live freestanding 8168 owns wire).
 */
void *linux_netdev_soft_hostish_ops_cached(void);
void *linux_netdev_soft_hostish_ndo_open_cached(void);
void *linux_netdev_soft_hostish_ndo_stop_cached(void);
void *linux_netdev_soft_hostish_ndo_xmit_cached(void);

/**
 * Hostish residual refuse: never invoke hostish ndo_open / ndo_start_xmit.
 * Residual E honesty (gate0 dual-drive guard). Once-lamp:
 *   linux_netdev_soft: soft hostish residual refuse ...
 * Returns LNDS-shaped -EOPNOTSUPP (-95). Soft!=product; no .ko ndo call.
 */
int   linux_netdev_soft_hostish_ndo_open_refuse(void);
int   linux_netdev_soft_hostish_ndo_xmit_refuse(void);

/**
 * Residual E fail-closed tallies (open / NAPI poll / soft xmit). Soft!=product.
 * Incremented when live freestanding 8168 blocks .ko-shaped open/poll/xmit.
 */
u32   linux_netdev_soft_live8168_fc_open(void);
u32   linux_netdev_soft_live8168_fc_napi(void);
u32   linux_netdev_soft_live8168_fc_xmit(void);

/**
 * Fill / refresh dual hostish blob from soft primary (or pSoftNd).
 * memcpy first min(0xaf0, soft slab) then optional soft name/mtu/MAC overlay.
 * Logs once: soft hostish net_device fill PASS ops=... ndo_open=... in_r8169=...
 * Never calls ndo_open / ndo_start_xmit. Soft!=product.
 * @return 0 ok, -1 no soft source / pool fail.
 */
int   linux_netdev_soft_hostish_fill(void *pSoftNd);

/**
 * Soft layout deepen stamp (LINUX_NETDEV_SOFT_LAYOUT_VER) on primary, or 0.
 * Soft!=product; eng residual for UDX/Linux-shaped userspace.
 */
u32   linux_netdev_soft_layout_ver(void);

/**
 * Soft primary ifindex (1+slot) or 0 if no primary. Soft!=product.
 */
int   linux_netdev_soft_primary_ifindex(void);

/**
 * Soft primary features word (u64) or 0. Soft!=product bookkeeping.
 */
u64   linux_netdev_soft_primary_features(void);

/**
 * Soft default netdev_ops table pointer (LNDS magic, ver2). Soft!=product.
 * Installable on soft alloc; never a .ko ops table.
 */
const struct soft_netdev_ops *linux_netdev_soft_default_ops(void);

/**
 * Soft default ethtool_ops table pointer (LNET magic). Soft!=product.
 */
const struct soft_ethtool_ops *linux_netdev_soft_default_ethtool_ops(void);

/**
 * Once-lamp soft layout deepen honesty (fields / ops / freestanding wire).
 * Grep: linux_netdev_soft: soft layout deepen PASS
 * Grep: linux_netdev_soft: soft residual layout
 * Soft!=product; hard-capped; safe from open/register (not IRQ storm).
 * @return 1 if lamp fired this call, 0 if already logged / not ready.
 */
int   linux_netdev_soft_layout_deepen_lamp_once(void);
/** Non-zero if primary soft-open state is set. Soft!=product. */
int   linux_netdev_soft_primary_open(void);
/** Soft open call count (idempotent open still counts once per open edge). */
u32   linux_netdev_soft_open_count(void);
/**
 * Feed one RX frame from freestanding L2 into soft netif_receive_skb.
 * Counts attempt/fed/drop; refreshes hold14 (incl. freestanding R/T mirror
 * when feed rare / bridge lag under SOFT hybrid). Soft!=product; copy-only -
 * never dual-drives freestanding BAR.
 */
void  linux_netdev_soft_l2_feed_rx(const void *pFrame, u32 cbLen);
/** Soft TX via freestanding net_l2_tx (0 ok, -1 fail/absent). */
int   linux_netdev_soft_l2_xmit(const void *pFrame, u32 cbLen);
/**
 * Soft-originated TX: soft-pool skb -> freestanding net_l2_tx via l2_xmit.
 * Validates soft pool cookie; frees soft skb always when accepted.
 * Counts attempt/ok/fail (and shares l2 bridge TX counters via l2_xmit).
 * Soft!=product; does NOT call .ko ndo_start_xmit.
 * Grep: linux_netdev_soft: soft l2 tx from_skb
 * @return 0 ok, -1 fail closed / absent / bad skb.
 */
int   linux_netdev_soft_l2_tx_from_skb(void *pSkb);
/**
 * Count freestanding TX that already went out (no second send).
 * Soft accounting only; Soft!=product datapath.
 */
void  linux_netdev_soft_l2_note_tx(int fOk);
/**
 * Soft L2 bridge RX for panel/hold14 (Soft!=product).
 * Returns max(soft_fed, freestanding rtl R) while freestanding owns wire so
 * hold14 tracks freestanding R climb when feed_rx is rare (bridge off SOFT).
 * Poll-side: also force-refreshes hold14 STATUS line (freestanding R/T).
 */
u32   linux_netdev_soft_l2_rx_fed(void);
/**
 * Soft L2 bridge RX attempts (bridge ON + eth-sized frame).
 * drop = attempt − fed (no primary / skb pool full / etc.). Soft!=product.
 */
u32   linux_netdev_soft_l2_rx_attempt(void);
u32   linux_netdev_soft_l2_rx_drop(void);
/**
 * Soft L2 bridge TX for panel/hold14 (Soft!=product).
 * max(soft tx_ok, freestanding rtl TX) while freestanding owns - same mirror.
 * Poll-side: also force-refreshes hold14 STATUS line (freestanding R/T).
 */
u32   linux_netdev_soft_l2_tx_ok(void);
u32   linux_netdev_soft_l2_tx_fail(void);
/** Soft TX attempts (note_tx + l2_xmit entries while bridge on). */
u32   linux_netdev_soft_l2_tx_attempt(void);
/** Soft-accounted TX packets while freestanding owns the wire. */
u32   linux_netdev_soft_l2_tx_accounted(void);
/**
 * Force STATUS hold14 paint from current soft + freestanding R/T mirror.
 * Soft!=product; never dual-drives BAR. Safe from idle/poll paths (like hold6
 * force refresh). Skips paint when counters unchanged after first paint.
 * Grep / panel: L2 BR RX · hold14 · freestanding R/T
 */
void  linux_netdev_soft_hold14_refresh(void);
/**
 * Raw freestanding rtl RX/TX counts sampled for hold14 mirror (0 if not
 * freestanding-owns / rtl not ready / unlinked). Soft!=product read-only.
 */
u32   linux_netdev_soft_hold14_fs_rx(void);
u32   linux_netdev_soft_hold14_fs_tx(void);
/**
 * Soft NAPI poll once on primary if ENABLED in soft side-table.
 * Default: schedule/complete bookkeeping only - does NOT call .ko pfnPoll
 * while freestanding owns MMIO (g_fMmioHandoff==0). Soft!=product.
 * Grep: linux_netdev_soft: soft napi poll skip .ko (mmio freestanding)
 */
void  linux_netdev_soft_napi_soft_poll(void);

/**
 * Soft IRQ -> NAPI kick: napi_schedule_prep + __napi_schedule only.
 * Never calls .ko ISR/poll. Called from feed_rx before soft_poll.
 * Grep: linux_netdev_soft: soft softirq kick
 */
void  linux_netdev_soft_softirq_kick(void);
u32   linux_netdev_soft_softirq_kick_count(void);

/** Count of soft NAPI slots currently ENABLED / SCHED. */
u32   linux_netdev_soft_napi_enabled_count(void);
u32   linux_netdev_soft_napi_sched_count(void);

/** Soft poll actually invoked pfnPoll / skipped (.ko or null). Soft!=product. */
u32   linux_netdev_soft_napi_poll_invokes(void);
u32   linux_netdev_soft_napi_poll_skips(void);

/**
 * MMIO handoff gate (default 0 = freestanding owns MMIO -> block .ko poll).
 * Minimal setter for handoff agent - not full handoff. Soft!=product.
 * set(1) refused when GJ_SOFT_R8169_MMIO_HANDOFF==0 (gate off) or while
 * freestanding rtl still ready (dual-drive guard). Grep: mmio_handoff SET refuse.
 */
int   linux_netdev_soft_napi_mmio_handoff(void);
void  linux_netdev_soft_napi_set_mmio_handoff(int fOn);

/*
 * Soft bodies with Linux C names - ksym / modules resolve these symbols.
 * Arguments are void * so F2 stubs and soft callers share one signature.
 * Soft!=ABI-stable struct layout under the pointer.
 */

/** Soft etherdev alloc (priv after 2 KiB front; txqs/rxqs recorded soft). */
void *alloc_etherdev_mqs(int nSizeofPriv, unsigned uTxQs, unsigned uRxQs);

/**
 * Soft managed alloc - parent recorded, same pool as alloc_etherdev_mqs.
 * Soft: no real devres free-on-detach; free_netdev still required soft.
 */
void *devm_alloc_etherdev_mqs(void *pDev, int nSizeofPriv, unsigned uTxQs,
                              unsigned uRxQs);

/**
 * Soft register. Logs once:
 *   linux_netdev_soft: soft register PASS name=...
 * Returns 0 on success, negative on bad args / pool / already registered.
 */
int   register_netdev(void *pDev);
void  unregister_netdev(void *pDev);
void  free_netdev(void *pDev);

void  netif_carrier_on(void *pDev);
void  netif_carrier_off(void *pDev);
void  netif_device_attach(void *pDev);
void  netif_device_detach(void *pDev);
void  netif_tx_wake_queue(void *pDev); /* soft no-op */
void  netif_start_queue(void *pDev);
void  netif_stop_queue(void *pDev);
void  netif_wake_queue(void *pDev);
void  netif_napi_add_weight_locked(void *pDev, void *pNapi, void *pfnPoll,
                                   int nWeight);
/** Soft: free/drop skb from pool; return 0 (NET_RX_SUCCESS soft). */
int   netif_receive_skb(void *pSkb);

void  dev_kfree_skb_any(void *pSkb);
void  dev_kfree_skb_any_reason(void *pSkb, u8 u8Reason);
void *napi_alloc_skb(void *pNapi, unsigned uLen);

/*
 * Soft skb path (future r8169 soft TX). Soft!=product; pool cookies only.
 * Fail closed on non-soft skb. Do NOT call .ko ndo_start_xmit.
 * Grep: linux_netdev_soft: soft skb_copy_bits|__skb_pad|skb_put|pskb_*|
 *       __netdev_alloc_skb
 */
/** Copy soft abData[offset..) -> pTo; 0 ok, -EFAULT fail closed. */
int   skb_copy_bits(const void *pSkb, int nOffset, void *pTo, int nLen);
/** Zero pad after soft data if room; free skb if fFreeOnError and fail. */
int   __skb_pad(void *pSkb, int nPad, int fFreeOnError);
/** Grow soft uLen; return previous tail pointer or NULL. */
void *skb_put(void *pSkb, unsigned uLen);
/** Soft linear: 1 if soft skb uLen >= uLen, else 0. */
int   pskb_may_pull(void *pSkb, unsigned uLen);
/** Soft no-frags pull; return abData or NULL. */
void *__pskb_pull_tail(void *pSkb, int nDelta);
/** Soft fixed slab expand; 0 if fits, -ENOMEM else. */
int   pskb_expand_head(void *pSkb, int nHead, int nTail, unsigned uGfp);
/** Soft pool alloc; logical len starts 0 (skb_put). Soft!=product. */
void *__netdev_alloc_skb(void *pDev, unsigned uLen, unsigned uGfp);
/** Alias of soft __netdev_alloc_skb(NULL, uLen, 0). */
void *dev_alloc_skb(unsigned uLen);
/** Soft pool RX-shaped alloc (uLen preset). Soft!=product. */
void *__napi_alloc_skb(void *pNapi, unsigned uLen, unsigned uGfp);

/**
 * Soft dev_queue_xmit(skb) - reverse soft->freestanding L2 when safe.
 * Soft!=product; never dual-drive.
 *   bridge ON + freestanding owns MMIO (handoff==0, not pending):
 *     -> linux_netdev_soft_l2_tx_from_skb
 *   handoff pending / mmio_handoff / bridge off: fail closed (free soft skb)
 * Grep: linux_netdev_soft: soft dev_queue_xmit
 * @return 0 ok, negative fail closed.
 */
int   dev_queue_xmit(void *pSkb);

/*
 * Soft NAPI lifecycle (post-probe r8169 path). Side-table keyed by pNapi
 * cookie - Soft!=Linux napi_struct state bits.
 * Grep: linux_netdev_soft: soft napi enable|disable|prep|complete
 */
void  napi_enable(void *pNapi);
void  napi_disable(void *pNapi);
/** Soft: 1 if transitioned to SCHED, 0 if disabled/already scheduled. */
int   napi_schedule_prep(void *pNapi);
/** Soft: clear SCHED; return 1 if MISSED (caller should reschedule). */
int   napi_complete_done(void *pNapi, int nWorkDone);
/** Soft: after prep, mark SCHED (no product poll list). */
void  __napi_schedule(void *pNapi);
void  napi_consume_skb(void *pSkb, int nBudget);
int   napi_gro_receive(void *pNapi, void *pSkb);

/** Soft printk helpers - kprintf once-ish (rate-capped). */
int   netdev_err(const void *pDev, const char *szFmt, ...);
int   netdev_info(const void *pDev, const char *szFmt, ...);
int   netdev_warn(const void *pDev, const char *szFmt, ...);
int   netdev_notice(const void *pDev, const char *szFmt, ...);
int   netdev_printk(const char *szLevel, const void *pDev, const char *szFmt,
                    ...);

void  netdev_update_features(void *pDev);
void  netdev_stats_to_stats64(void *pS64, const void *pStats);
void  netdev_sw_irq_coalesce_default_on(void *pDev);
void  netif_schedule_queue(void *pTxq);
void  netif_set_tso_max_size(void *pDev, u32 u32Size);
void  netif_set_tso_max_segs(void *pDev, u32 u32Segs);

/*
 * Post-probe open/datapath helpers (replace empty ksym stubs). Soft!=product.
 * rtnl_*: nesting counter only (no real RTNL wait).
 * eth_platform_get_mac_address: always -EOPNOTSUPP (driver falls back to chip).
 * eth_mac_addr / eth_validate_addr: soft netdev front-field path only.
 * eth_type_trans: soft skb ethertype at offset 12, else 0.
 * synchronize_net: soft no-op (no RCU/NAPI wait).
 * ethtool_op_get_link: soft carrier or freestanding ready (1=up).
 * ethtool_op_get_ts_info: soft zero info head (no PHC).
 * net_ratelimit: soft allow (1); empty stub suppressed all.
 * No dual-drive / no BAR touch. Soft!=product.
 */
void  rtnl_lock(void);
void  rtnl_unlock(void);
int   eth_platform_get_mac_address(void *pDev, u8 *pu8Addr);
int   eth_mac_addr(void *pDev, void *pSa);
int   eth_validate_addr(void *pDev);
u16   eth_type_trans(void *pSkb, void *pDev);
void  synchronize_net(void);
u32   ethtool_op_get_link(void *pDev);
int   ethtool_op_get_ts_info(void *pDev, void *pInfo);
int   net_ratelimit(void);

/** Soft diagnostics. */
u32   linux_netdev_soft_alloc_calls(void);
u32   linux_netdev_soft_register_calls(void);
u32   linux_netdev_soft_skb_live(void);

/**
 * Soft MMIO handoff readiness (phase 2 sole-owner deepen; Soft!=product).
 * Gate GJ_SOFT_R8169_MMIO_HANDOFF==0: log once SKIP, return 0.
 * Gate 1 return codes:
 *   0  = skip (gate off; compile-time path)
 *   1  = wait (prepare or primary not ready)
 *   2  = ready-for-open (sole-owner candidate; still no .ko / ndo_open)
 *  -1  = fault (dual-drive / fail closed)
 * Does NOT call .ko / ndo_open; does NOT set g_fMmioHandoff (phase 3).
 * See docs/R8169_MMIO_HANDOFF.md.
 * Grep: linux_netdev_soft: soft mmio handoff
 */
int   linux_netdev_soft_mmio_handoff_ready(void);

/**
 * Phase-2 sole-owner marker: non-zero only when ready()==2
 * (prepare done + freestanding not ready + primary + no fault).
 * Gate 0 -> 0. Still no .ko open / no NAPI mmio_handoff allow.
 * Grep: soft mmio handoff sole-owner
 */
int   linux_netdev_soft_mmio_sole_owner(void);

/**
 * Phase-3 gated soft-open stub after sole-owner (Soft!=product).
 * Gate GJ_SOFT_R8169_MMIO_HANDOFF==0: log once SKIP, return 0.
 * Gate 1 + !sole_owner: log WAIT once, return -16 (soft EBUSY).
 * Gate 1 + sole_owner (Option A, this wave):
 *   - FAULT if freestanding still ready
 *   - set g_fMmioHandoff=1 (allow soft NAPI .ko poll path)
 *   - soft open only (carrier/queue via open_primary; no .ko ndo_open)
 * Option B (.ko ndo_open): separate gate GJ_SOFT_R8169_KO_NDO_OPEN
 * default 0 - not taken this wave (next lab; layout must be proven).
 * Returns: 0 ok/skip, -16 wait, -1 fault.
 * Grep: linux_netdev_soft: soft mmio handoff phase3
 * See docs/R8169_MMIO_HANDOFF.md.
 */
int   linux_netdev_soft_mmio_try_open(void);

/**
 * Phase-4a hybrid active (Soft!=product; default lab path under gate 0).
 * Non-zero when ALL of:
 *   - last probe mode SOFT/EMU (0) OR REAL (1) - not REAL-only (gate0 hybrid)
 *   - soft L2 bridge ON
 *   - soft primary registered
 *   - freestanding rtl8168_ready()
 *   - g_fMmioHandoff==0 and freestanding owns (handoff not pending/fault)
 * Slogan: Linux driver hosted, freestanding datapath.
 * Never dual-drives freestanding BAR. Hybrid SOFT must not steal BAR.
 * Residual E: open/NAPI/xmit fail-closed for live 8168. Soft!=product.
 * See docs/R8169_MMIO_HANDOFF.md phase 4a.
 */
int   linux_netdev_soft_hybrid_active(void);

/**
 * Once-lamp when hybrid_active(): serial + STATUS hold14/15.
 * Under hybrid SOFT, may auto-enable L2 bridge when freestanding ready +
 * primary (bridge can lag net_l2 note). Not REAL probe mode only.
 * Grep: main: linux path HYBRID wire=freestanding soft=r8169
 * Grep: linux_netdev_soft: soft hybrid zero-touch PASS ... Soft!=product
 * Grep: linux_netdev_soft: soft hybrid residual E ...
 * Grep: linux_netdev_soft: soft dual DoD B residual gate0 PASS ...
 * hold14: l2 br rx=N tx=M (freestanding R/T mirror companion when feed rare)
 * hold15: HYBRID wire=fs soft=r8169
 * Soft!=product; not product AC / not 4b .ko wire.
 * Refuses if mmio_handoff or not freestanding-owns (dual-drive safety).
 * Hybrid SOFT must not steal BAR. Lean lamps only (no stamp storms).
 * @return 1 if lamp fired this call, 0 if inactive or already logged.
 */
int   linux_netdev_soft_hybrid_lamp_once(void);

/**
 * Dual DoD B residual honesty lamp (gate0 hybrid friendliness; Soft!=product).
 * Once-capped lean serial: RUN_INIT=0 / freestanding_no_exec + no_bar_steal +
 * R0 thrash refuse + residual E fail-closed tallies + freestanding_rtl SKIP +
 * product_nic=UDX. Safe when soft ready (does not require hybrid_active).
 * Never sets mmio_handoff / never BAR thrash. Never claims freestanding wire
 * product. Grep: linux_netdev_soft: soft dual DoD B residual gate0 PASS
 * Grep: linux_netdev_soft: soft residual RUN_INIT=0 freestanding_no_exec
 * Grep: linux_netdev_soft: soft residual freestanding_rtl=SKIP
 * Grep: linux_netdev_soft: soft residual product_nic=UDX
 * Grep: linux_netdev_soft: soft residual R0 thrash refuse
 * Grep: linux_netdev_soft: soft residual wire sole freestanding
 * Soft!=product; G-AC-1 lean; dual DoD B OPEN until DUT proof.
 * No version stamp; no stamp storms. Dual MIT OR Apache-2.0.
 * @return 1 if lamp fired this call, 0 if already logged / not ready.
 */
int   linux_netdev_soft_gate0_residual_lamp_once(void);

/**
 * Soft residual lean once-lamp (ABI / hostability eng; Soft!=product).
 * Soft netdev seed residual only - never freestanding wire product claim.
 * Freestanding rtl default SKIP (GJ_RTL8168_PROBE=0). Product NIC = UDX
 * userspace (rtl8168_udx). No Option B .ko ndo_open product
 * (GJ_SOFT_R8169_KO_NDO_OPEN default 0). Once-capped; no version stamp;
 * no stamp storms. G-AC-1. Dual MIT OR Apache-2.0.
 * Grep: linux_netdev_soft: soft residual lean PASS Soft!=product
 * Grep: linux_netdev_soft: soft residual freestanding_rtl=SKIP
 * Grep: linux_netdev_soft: soft residual product_nic=UDX
 * @return 1 if lamp fired this call, 0 if already logged / not ready.
 */
int   linux_netdev_soft_residual_lean_lamp_once(void);

/**
 * Non-zero when soft has not claimed MMIO handoff (g_fMmioHandoff==0).
 * Soft residual "no BAR steal" honesty for dual DoD B / gate0 hybrid.
 * Soft!=product; does not probe freestanding readiness. Forces gate0 clear.
 */
int   linux_netdev_soft_no_bar_steal(void);

/**
 * Freestanding owns wire residual (dual DoD B / residual E honesty).
 * Non-zero when soft has not claimed MMIO AND reverse/TX freestanding path
 * is allowed (not handoff pending/fault). Soft!=product; G-AC-1 lean.
 * Forces gate0 no_bar_steal. Never dual-drives BAR. Eng residual only.
 * Default freestanding rtl SKIP - this is lab residual, not product NIC.
 * Grep: soft freestanding owns wire
 */
int   linux_netdev_soft_freestanding_owns_wire(void);

/**
 * Count of mmio_handoff SET refuses (gate0 / freestanding-ready dual-drive).
 * Soft!=product; eng residual tally (no stamp storm).
 */
u32   linux_netdev_soft_bar_steal_refuse_count(void);

/**
 * Hostish residual refuse tallies (inventory honesty; never called .ko ndo).
 * Soft!=product; residual E companion. No stamp storm.
 */
u32   linux_netdev_soft_hostish_open_refuse_count(void);
u32   linux_netdev_soft_hostish_xmit_refuse_count(void);

#ifdef __cplusplus
}
#endif
