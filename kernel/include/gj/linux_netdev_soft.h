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
 *
 * See docs/LINUX_MODULE_PATH.md · docs/LAPTOP_LINUX_DRIVER_HOST.md.
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
    u8    u8PadFlags[2];
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

/** Soft diagnostics. */
u32   linux_netdev_soft_alloc_calls(void);
u32   linux_netdev_soft_register_calls(void);
u32   linux_netdev_soft_skb_live(void);

#ifdef __cplusplus
}
#endif
