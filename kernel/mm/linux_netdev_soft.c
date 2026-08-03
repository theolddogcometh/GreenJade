/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft Linux netdev lifecycle seed.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL / no Linux .ko source.
 *
 * Soft only:
 *   - linux_netdev_soft_init: ready lamp + optional linux_ksym_register
 *   - alloc_etherdev_mqs / devm_alloc_etherdev_mqs: static pool of 4 netdevs
 *   - register_netdev / unregister_netdev / free_netdev lifecycle
 *   - soft skb pool (16) for napi_alloc_skb / receive drop / kfree_skb
 *   - carrier / queue / napi / netdev_* printk soft no-ops or bookkeeping
 *
 * Soft ≠ ABI-stable: struct layouts are incomplete soft shapes, not Linux ABI.
 * Soft ≠ product: no live RX/TX, no NAPI poll scheduling, no DMA.
 *
 * Greppable markers (keep stable):
 *   linux_netdev_soft: soft init PASS
 *   linux_netdev_soft: soft register PASS name=…
 *   linux_netdev_soft: soft register SKIP
 *   linux_netdev_soft: soft ksym register PASS|SKIP
 */
#include <gj/klog.h>
#include <gj/linux_netdev_soft.h>
#include <gj/string.h>
#include <gj/types.h>
#include <stdarg.h>

/*
 * F2 linux_ksym may be linked later. Weak unresolved → NULL; init skips export.
 * Coordinator may also point ksym at these symbols by C name without this call.
 */
int linux_ksym_register(const char *szName, void *pFn) __attribute__((weak));

/* Soft log cap for netdev_* printk helpers (once-ish). */
#define LNDS_LOG_CAP 16u

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

/** First successfully registered soft netdev still live. */
static struct net_device *g_pPrimary;

/**
 * Soft netdev slab: 2 KiB front (struct net_device) + priv area.
 * Priv is accessed as (u8 *)pNd + LINUX_NETDEV_SOFT_ND_BYTES.
 */
struct soft_nd_slab {
    struct net_device nd;
    u8 abPriv[LINUX_NETDEV_SOFT_PRIV_MAX];
};

static struct soft_nd_slab g_aNd[LINUX_NETDEV_SOFT_MAX];

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

    if (g_cLive < 0xffffffffu) {
        g_cLive++;
    }
    return pNd;
}

static void
lnds_skb_free(void *pSkb)
{
    struct soft_skb *pSoft;
    u32 i;

    if (pSkb == NULL) {
        return;
    }
    pSoft = (struct soft_skb *)pSkb;
    for (i = 0u; i < LINUX_NETDEV_SOFT_SKB_MAX; i++) {
        if (&g_aSkb[i] == pSoft && pSoft->u8Live != 0u) {
            memset(pSoft, 0, sizeof(*pSoft));
            if (g_cSkbLive > 0u) {
                g_cSkbLive--;
            }
            if (g_cSkbFree < 0xffffffffu) {
                g_cSkbFree++;
            }
            return;
        }
    }
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

/* ---- Public init / getters ---------------------------------------------- */

void
linux_netdev_soft_init(void)
{
    u32 u32KsymOk;
    u32 u32KsymSkip;

    g_cInitCalls++;
    if (g_fReady) {
        /* Grep: linux_netdev_soft: soft init PASS */
        kprintf("linux_netdev_soft: soft init PASS (idempotent call=%u)\n",
                (unsigned)g_cInitCalls);
        return;
    }

    g_pPrimary = NULL;
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
    memset(g_aNd, 0, sizeof(g_aNd));
    memset(g_aSkb, 0, sizeof(g_aSkb));
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

    /* Grep: linux_netdev_soft: soft init PASS */
    kprintf("linux_netdev_soft: soft init PASS nd_max=%u skb_max=%u "
            "nd_bytes=%u priv_max=%u soft_ne_abi=1 product=OPEN\n",
            (unsigned)LINUX_NETDEV_SOFT_MAX, (unsigned)LINUX_NETDEV_SOFT_SKB_MAX,
            (unsigned)LINUX_NETDEV_SOFT_ND_BYTES,
            (unsigned)LINUX_NETDEV_SOFT_PRIV_MAX);

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
        /* Grep: linux_netdev_soft: soft register SKIP */
        kprintf("linux_netdev_soft: soft register SKIP (inval dev)\n");
        return -1;
    }
    if (pNd->u8Registered != 0u) {
        if (g_cRegSkip < 0xffffffffu) {
            g_cRegSkip++;
        }
        /* Grep: linux_netdev_soft: soft register SKIP */
        kprintf("linux_netdev_soft: soft register SKIP (already registered "
                "name=%s)\n",
                lnds_name_of(pNd));
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
            "txqs=%u rxqs=%u slot=%u\n",
            lnds_name_of(pNd), (unsigned)pNd->mtu, (unsigned)pNd->u16TxQs,
            (unsigned)pNd->u16RxQs, (unsigned)pNd->u32Slot);
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
    }
    kprintf("linux_netdev_soft: soft unregister name=%s\n", lnds_name_of(pNd));
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

/* ---- Soft skb ----------------------------------------------------------- */

void *
napi_alloc_skb(void *pNapi, unsigned uLen)
{
    u32 i;
    struct soft_skb *pSkb;

    if (!g_fReady) {
        linux_netdev_soft_init();
    }
    if (uLen > LINUX_NETDEV_SOFT_SKB_DATA) {
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
    pSkb->uLen = uLen;
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

int
netif_receive_skb(void *pSkb)
{
    /* Soft: free/drop — no product stack demux yet. */
    if (pSkb != NULL) {
        if (g_cSkbDrop < 0xffffffffu) {
            g_cSkbDrop++;
        }
        lnds_skb_free(pSkb);
    }
    return 0; /* NET_RX_SUCCESS soft */
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
    (void)pDev;
}

void
netdev_stats_to_stats64(void *pS64, const void *pStats)
{
    /*
     * Soft: zero-fill a 64-byte soft stats64 blob when present.
     * Layout not Linux-ABI; prevents NULL-pointer probe crashes only.
     */
    if (pS64 != NULL) {
        memset(pS64, 0, 64);
    }
    (void)pStats;
}

void
netdev_sw_irq_coalesce_default_on(void *pDev)
{
    (void)pDev;
}
