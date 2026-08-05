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
 * Soft ≠ ABI-stable
 * -----------------
 * struct net_device here is an INCOMPLETE soft shape (front fields only,
 * pad to ≥2 KiB). Field order/size are NOT guaranteed to match any Linux
 * kernel version. Soft modules / tests built against this header are the
 * intended consumers until a product ABI exists. Soft ≠ product RX/TX path.
 *
 * Greppable markers (keep stable):
 *   linux_netdev_soft: soft init PASS
 *   linux_netdev_soft: soft register PASS name=…
 *   linux_netdev_soft: soft register SKIP
 *   linux_netdev_soft: soft ksym register PASS|SKIP
 *   linux_netdev_soft: soft napi enable|disable|prep|complete
 *   linux_netdev_soft: soft napi bridge enable
 *   linux_netdev_soft: soft napi poll skip .ko (mmio freestanding)
 *   linux_netdev_soft: soft softirq kick
 *   linux_netdev_soft: soft l2 rx napi
 *   linux_netdev_soft: soft open primary
 *   linux_netdev_soft: soft open skip .ko ops
 *   linux_netdev_soft: soft ops diagnostic …
 *   linux_netdev_soft: soft hostish net_device fill PASS …
 *   linux_netdev_soft: soft l2 bridge first tx
 *   linux_netdev_soft: soft rtnl …
 *   linux_netdev_soft: soft eth_platform EOPNOTSUPP
 *   linux_netdev_soft: soft eth_mac …
 *   linux_netdev_soft: soft synchronize_net
 *   linux_netdev_soft: soft ethtool_op_get_link …
 *   linux_netdev_soft: soft ethtool_op_get_ts_info …
 *   linux_netdev_soft: soft net_ratelimit …
 *   linux_netdev_soft: soft skb_copy_bits …
 *   linux_netdev_soft: soft __skb_pad …
 *   linux_netdev_soft: soft skb_put …
 *   linux_netdev_soft: soft pskb_may_pull …
 *   linux_netdev_soft: soft __netdev_alloc_skb …
 *   linux_netdev_soft: soft l2 tx from_skb …
 *   linux_netdev_soft: soft dev_queue_xmit …
 *   main: linux path HYBRID wire=freestanding soft=r8169  (phase 4a; via lamp_once)
 *
 * See docs/LINUX_MODULE_PATH.md · docs/LAPTOP_LINUX_DRIVER_HOST.md ·
 * docs/R8169_MMIO_HANDOFF.md (phase 4a hybrid).
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

/**
 * Soft net_device (Linux *shape*, not layout-stable).
 *
 * Front fields probe code may write: name / ml_priv / netdev_ops / flags /
 * mtu / dev_addr. Remaining bytes zeroed pad so accidental mid-struct
 * stores within 2 KiB do not walk off the object.
 *
 * Soft bookkeeping lives after the classic front fields (not Linux ABI).
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
    u8    u8PadFlags[1];
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
     * Pad so sizeof(struct net_device) == LINUX_NETDEV_SOFT_ND_BYTES.
     * Soft modules may touch early fields without walking off the slab.
     */
    u8 abFrontPad[LINUX_NETDEV_SOFT_ND_BYTES - 128u];
};

/*
 * Compile-time soft layout check: front bookkeeping ends at offset 128
 * (x86_64 / aarch64 freestanding LP64). If a field is added above, grow
 * the constant or shrink abFrontPad so the total remains 2048.
 */
_Static_assert(sizeof(struct net_device) == LINUX_NETDEV_SOFT_ND_BYTES,
               "soft net_device must be exactly 2048 bytes");

/**
 * Soft init: idempotent ready lamp + optional ksym export of soft bodies.
 * Prefer: linux_ksym_register("register_netdev", register_netdev) …
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
 * Soft≠product: freestanding rtl/virtio still owns MMIO; bridge copies frames
 * into soft skb/receive accounting so REAL probe + soft netdev share a path.
 * Grep: linux_netdev_soft: soft l2 bridge …
 */
void  linux_netdev_soft_l2_bridge_enable(int fOn);
int   linux_netdev_soft_l2_bridge_enabled(void);
/**
 * Soft open primary netdev (carrier/queue/start_queue). Soft≠product.
 * Does NOT call .ko ndo_open (MMIO freestanding owns). Soft-shaped ops
 * tables (LNDS magic) may run optional soft ndo_open only.
 * Grep: linux_netdev_soft: soft open primary
 */
void  linux_netdev_soft_open_primary(void);
/**
 * Read-only ops layout diagnostic on soft primary (Soft≠product).
 * Logs soft netdev_ops field vs hostish word at +0x8 (RHEL 5.14 oracle)
 * and whether either pointer falls in soft-loaded r8169 image range.
 * Never calls ndo_open / never enables Option B. Once-log.
 * Grep: linux_netdev_soft: soft ops diagnostic
 * See docs/R8169_KO_NDO_OPEN.md.
 */
void  linux_netdev_soft_ops_diagnostic(void);

/*
 * Strategy A hostish net_device (≥0xaf0 blob; Soft≠product).
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

/** Non-zero if hostish blob filled after register. Soft≠product. */
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
 * Fill / refresh dual hostish blob from soft primary (or pSoftNd).
 * memcpy first min(0xaf0, soft slab) then optional soft name/mtu/MAC overlay.
 * Logs once: soft hostish net_device fill PASS ops=… ndo_open=… in_r8169=…
 * Never calls ndo_open. Soft≠product.
 * @return 0 ok, -1 no soft source / pool fail.
 */
int   linux_netdev_soft_hostish_fill(void *pSoftNd);
/** Non-zero if primary soft-open state is set. Soft≠product. */
int   linux_netdev_soft_primary_open(void);
/** Soft open call count (idempotent open still counts once per open edge). */
u32   linux_netdev_soft_open_count(void);
/** Feed one RX frame from freestanding L2 into soft netif_receive_skb. */
void  linux_netdev_soft_l2_feed_rx(const void *pFrame, u32 cbLen);
/** Soft TX via freestanding net_l2_tx (0 ok, -1 fail/absent). */
int   linux_netdev_soft_l2_xmit(const void *pFrame, u32 cbLen);
/**
 * Soft-originated TX: soft-pool skb → freestanding net_l2_tx via l2_xmit.
 * Validates soft pool cookie; frees soft skb always when accepted.
 * Counts attempt/ok/fail (and shares l2 bridge TX counters via l2_xmit).
 * Soft≠product; does NOT call .ko ndo_start_xmit.
 * Grep: linux_netdev_soft: soft l2 tx from_skb
 * @return 0 ok, -1 fail closed / absent / bad skb.
 */
int   linux_netdev_soft_l2_tx_from_skb(void *pSkb);
/**
 * Count freestanding TX that already went out (no second send).
 * Soft accounting only; Soft≠product datapath.
 */
void  linux_netdev_soft_l2_note_tx(int fOk);
u32   linux_netdev_soft_l2_rx_fed(void);
u32   linux_netdev_soft_l2_tx_ok(void);
u32   linux_netdev_soft_l2_tx_fail(void);
/** Soft TX attempts (note_tx + l2_xmit entries while bridge on). */
u32   linux_netdev_soft_l2_tx_attempt(void);
/** Soft-accounted TX packets while freestanding owns the wire. */
u32   linux_netdev_soft_l2_tx_accounted(void);
/**
 * Soft NAPI poll once on primary if ENABLED in soft side-table.
 * Default: schedule/complete bookkeeping only — does NOT call .ko pfnPoll
 * while freestanding owns MMIO (g_fMmioHandoff==0). Soft≠product.
 * Grep: linux_netdev_soft: soft napi poll skip .ko (mmio freestanding)
 */
void  linux_netdev_soft_napi_soft_poll(void);

/**
 * Soft IRQ → NAPI kick: napi_schedule_prep + __napi_schedule only.
 * Never calls .ko ISR/poll. Called from feed_rx before soft_poll.
 * Grep: linux_netdev_soft: soft softirq kick
 */
void  linux_netdev_soft_softirq_kick(void);
u32   linux_netdev_soft_softirq_kick_count(void);

/** Count of soft NAPI slots currently ENABLED / SCHED. */
u32   linux_netdev_soft_napi_enabled_count(void);
u32   linux_netdev_soft_napi_sched_count(void);

/** Soft poll actually invoked pfnPoll / skipped (.ko or null). Soft≠product. */
u32   linux_netdev_soft_napi_poll_invokes(void);
u32   linux_netdev_soft_napi_poll_skips(void);

/**
 * MMIO handoff gate (default 0 = freestanding owns MMIO → block .ko poll).
 * Minimal setter for handoff agent — not full handoff. Soft≠product.
 */
int   linux_netdev_soft_napi_mmio_handoff(void);
void  linux_netdev_soft_napi_set_mmio_handoff(int fOn);

/*
 * Soft bodies with Linux C names — ksym / modules resolve these symbols.
 * Arguments are void * so F2 stubs and soft callers share one signature.
 * Soft≠ABI-stable struct layout under the pointer.
 */

/** Soft etherdev alloc (priv after 2 KiB front; txqs/rxqs recorded soft). */
void *alloc_etherdev_mqs(int nSizeofPriv, unsigned uTxQs, unsigned uRxQs);

/**
 * Soft managed alloc — parent recorded, same pool as alloc_etherdev_mqs.
 * Soft: no real devres free-on-detach; free_netdev still required soft.
 */
void *devm_alloc_etherdev_mqs(void *pDev, int nSizeofPriv, unsigned uTxQs,
                              unsigned uRxQs);

/**
 * Soft register. Logs once:
 *   linux_netdev_soft: soft register PASS name=…
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
 * Soft skb path (future r8169 soft TX). Soft≠product; pool cookies only.
 * Fail closed on non-soft skb. Do NOT call .ko ndo_start_xmit.
 * Grep: linux_netdev_soft: soft skb_copy_bits|__skb_pad|skb_put|pskb_*|
 *       __netdev_alloc_skb
 */
/** Copy soft abData[offset..) → pTo; 0 ok, -EFAULT fail closed. */
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
/** Soft pool alloc; logical len starts 0 (skb_put). Soft≠product. */
void *__netdev_alloc_skb(void *pDev, unsigned uLen, unsigned uGfp);
/** Alias of soft __netdev_alloc_skb(NULL, uLen, 0). */
void *dev_alloc_skb(unsigned uLen);
/** Soft pool RX-shaped alloc (uLen preset). Soft≠product. */
void *__napi_alloc_skb(void *pNapi, unsigned uLen, unsigned uGfp);

/**
 * Soft dev_queue_xmit(skb) — reverse soft→freestanding L2 when safe.
 * Soft≠product; never dual-drive.
 *   bridge ON + freestanding owns MMIO (handoff==0, not pending):
 *     → linux_netdev_soft_l2_tx_from_skb
 *   handoff pending / mmio_handoff / bridge off: fail closed (free soft skb)
 * Grep: linux_netdev_soft: soft dev_queue_xmit
 * @return 0 ok, negative fail closed.
 */
int   dev_queue_xmit(void *pSkb);

/*
 * Soft NAPI lifecycle (post-probe r8169 path). Side-table keyed by pNapi
 * cookie — Soft≠Linux napi_struct state bits.
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

/** Soft printk helpers — kprintf once-ish (rate-capped). */
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
 * Post-probe open/datapath helpers (replace empty ksym stubs). Soft≠product.
 * rtnl_*: nesting counter only (no real RTNL wait).
 * eth_platform_get_mac_address: always -EOPNOTSUPP (driver falls back to chip).
 * eth_mac_addr / eth_validate_addr: soft netdev front-field path only.
 * eth_type_trans: soft skb ethertype at offset 12, else 0.
 * synchronize_net: soft no-op (no RCU/NAPI wait).
 * ethtool_op_get_link: soft carrier or freestanding ready (1=up).
 * ethtool_op_get_ts_info: soft zero info head (no PHC).
 * net_ratelimit: soft allow (1); empty stub suppressed all.
 * No dual-drive / no BAR touch. Soft≠product.
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
 * Soft MMIO handoff readiness (phase 2 sole-owner deepen; Soft≠product).
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
 * Gate 0 → 0. Still no .ko open / no NAPI mmio_handoff allow.
 * Grep: soft mmio handoff sole-owner
 */
int   linux_netdev_soft_mmio_sole_owner(void);

/**
 * Phase-3 gated soft-open stub after sole-owner (Soft≠product).
 * Gate GJ_SOFT_R8169_MMIO_HANDOFF==0: log once SKIP, return 0.
 * Gate 1 + !sole_owner: log WAIT once, return -16 (soft EBUSY).
 * Gate 1 + sole_owner (Option A, this wave):
 *   - FAULT if freestanding still ready
 *   - set g_fMmioHandoff=1 (allow soft NAPI .ko poll path)
 *   - soft open only (carrier/queue via open_primary; no .ko ndo_open)
 * Option B (.ko ndo_open): separate gate GJ_SOFT_R8169_KO_NDO_OPEN
 * default 0 — not taken this wave (next lab; layout must be proven).
 * Returns: 0 ok/skip, -16 wait, -1 fault.
 * Grep: linux_netdev_soft: soft mmio handoff phase3
 * See docs/R8169_MMIO_HANDOFF.md.
 */
int   linux_netdev_soft_mmio_try_open(void);

/**
 * Phase-4a hybrid active (Soft≠product; default lab path under gate 0).
 * Non-zero when ALL of:
 *   - last probe mode REAL
 *   - soft L2 bridge ON
 *   - soft primary registered
 *   - freestanding rtl8168_ready()
 *   - g_fMmioHandoff==0 and handoff not pending/fault
 * Slogan: Linux driver hosted, freestanding datapath.
 * See docs/R8169_MMIO_HANDOFF.md phase 4a.
 */
int   linux_netdev_soft_hybrid_active(void);

/**
 * Once-lamp when hybrid_active(): serial + optional STATUS hold 15.
 * Grep: main: linux path HYBRID wire=freestanding soft=r8169
 * hold15: HYBRID wire=fs soft=r8169
 * Soft≠product; not product AC / not 4b .ko wire.
 * @return 1 if lamp fired this call, 0 if inactive or already logged.
 */
int   linux_netdev_soft_hybrid_lamp_once(void);

#ifdef __cplusplus
}
#endif
