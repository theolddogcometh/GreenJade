/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * ioremap / iounmap. Host: malloc iomem + window table.
 * Freestanding: static iomem pool (no libc heap).
 *
 * Soft inventory (exclusive deepen; this unit only) --
 * greppable "udx: mmio soft ...":
 *   udx: mmio soft inventory ...
 *   udx: mmio soft map ...
 *   udx: mmio soft unmap ...
 *   udx: mmio soft pool ...
 *   udx: mmio soft path ...
 *   udx: mmio soft wave ...
 *   udx: mmio soft product host ...
 *   udx: mmio soft residual lean ...
 *   udx: mmio soft residual lean layout_ver=
 *   udx: mmio soft residual lean UDX eng ...
 *   udx: mmio soft api honesty ...
 *   udx: mmio soft residual ioremap ...
 *   udx: mmio soft residual iounmap ...
 *   udx: mmio soft residual accessors ...
 *   udx: mmio soft residual write_flush ...
 *   udx: mmio soft residual query ...
 *   udx: mmio soft residual uc_map ...
 *   udx: mmio soft residual freestanding thrash refuse ...
 *   udx: mmio soft residual map_chain ...
 *   udx: mmio soft residual product_host ...
 *   udx: mmio soft residual dual_dod ...
 *   udx: mmio soft residual mint ...
 *   udx: mmio soft residual bar0 ...
 *   udx: mmio soft residual functional ...
 *   udx: mmio soft residual map_access ...
 *   udx: mmio soft residual denser ...
 *   udx: mmio soft residual denser prove ...
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft != skeleton PASS. greppable: udx: mmio soft
 * greppable: udx: mmio soft residual lean
 * greppable: udx: mmio soft residual lean layout_ver=
 * greppable: udx: mmio soft residual map_chain
 * greppable: udx: mmio soft residual write_flush
 * greppable: udx: mmio soft residual query
 * greppable: udx: mmio soft residual product_host
 * greppable: udx: mmio soft residual dual_dod
 * greppable: udx: mmio soft residual mint
 * greppable: udx: mmio soft residual bar0
 * greppable: udx: mmio soft residual functional
 * greppable: udx: mmio soft residual map_access
 * greppable: udx: mmio soft residual denser
 * greppable: udx: mmio soft residual denser prove
 * greppable: udx: mmio soft product host
 *
 * Freestanding DDI bind: udx_host_bind_* maps preferred BARs and
 * registers granted PA via udx_host_window_register; udx_ioremap(phys,len)
 * then resolves that window (same lookup as host inject). Soft != product.
 *
 * Soft residual deepen (UDX product hosts; Soft!=product; G-AC-1; C2):
 *   ioremap residual      -- window lookup != MMIO_FRAME cap mint
 *   ioremap/VA densify    -- STRONGER residual densify for IOREMAP/VA
 *                            used by freestanding product hosts
 *                            (MAP_BAR grant VA -> window_register ->
 *                            host_window_lookup -> cookie pVa);
 *                            product_hosts=UDX; Dual DoD OPEN
 *   iounmap residual      -- clear + free/pool != product Phase-A revoke
 *   readl/writel residual -- header volatile accessors + nat align
 *   write_flush residual  -- write*_flush + posted_flush catalog;
 *                            product-host ISR/mask residual pattern
 *   query residual        -- is_mapped/len/phys/va observation only
 *   UC map honesty        -- VA expects host UC (user_device /
 *                            map_device_uc PCD|PWT); UDX does not
 *                            re-attr pages; not WB identity thrash
 *   freestanding thrash refuse -- no freestanding class re-enable; no
 *                            pool expansion storm; product =
 *                            Linux-shaped userspace UDX hosts
 *   map_chain residual    -- MAP_BAR grant -> window_register ->
 *                            ioremap lookup (C2 product path catalog;
 *                            Dual DoD A/B OPEN; agent!=close)
 *   product_host residual -- rtl8168_udx DoD B / xhci_udx DoD A
 *                            BAR+accessor surface (Soft!=product);
 *                            product_hosts=UDX honesty
 *   dual_dod residual     -- Dual DoD A/B OPEN honesty (agent!=close)
 *   mint residual         -- MMIO_FRAME cap_mint honesty (always 0)
 *   bar0 residual         -- product BAR0 map/access surface catalog
 *                            (xhci BAR0; rtl DDI grant BAR0; pref masks)
 *   functional residual   -- map+access steps toward product BAR0
 *                            (fail-closed phys!=0; access-ready cookie)
 * Denser residual (product Dual DoD; Soft!=product; denser only; bar .75):
 *   denser residual       -- denser_prove rollup over map_chain + bar0 +
 *                            access + product_host + dual_dod + ioremap/VA
 *   map_chain denser      -- MAP_BAR->window->ioremap denser prove
 *   bar0 denser           -- product BAR0 map/access denser surface
 *   access denser         -- map+access after cookie_va_prove denser
 *   product_host denser   -- rtl8168_udx DoD B / xhci_udx DoD A denser
 *   dual_dod denser       -- Dual DoD A/B OPEN denser honesty (agent!=close)
 * Dual MIT OR Apache-2.0. Product hosts = Linux-shaped userspace UDX
 * (rtl8168_udx DoD B OPEN / xhci_udx DoD A OPEN) over hot+cold ABI + DDI.
 * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
 * greppable: udx: mmio soft residual ioremap densify
 * greppable: udx: mmio soft residual denser
 * greppable: denser_prove=1 | map_chain_dense=1 | bar0_dense=1
 * No stamp storms. No version stamp. No GJ_IMAGE_VERSION touch.
 * stamp-free residual bar v2026.08.04.75 (NEVER invent .76).
 */
#include "udx_internal.h"

#include <udx/mmio.h>
#include <udx/udx.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#include <stdlib.h>
#else
static struct udx_iomem g_aFsIomem[UDX_FS_IOMEM_SLOTS];
static u8               g_aFsIomemUsed[UDX_FS_IOMEM_SLOTS];
#endif

/*
 * Soft wave stamp + residual area count (MMIO residual deepen).
 * Soft wave only — never GJ_IMAGE_VERSION. stamp-free product bar.
 * AREAS: inventory + residual once-lamps (write_flush/query/
 * product_host/dual_dod/mint/bar0/functional + ioremap/VA densify C2 +
 * denser residual prove rollup). Soft!=product. Wave 75 aligns
 * stamp-free residual bar v2026.08.04.75. NEVER invent .76.
 */
#define UDX_MMIO_SOFT_WAVE  75u
#define UDX_MMIO_SOFT_AREAS 26u

/*
 * FUNCTIONAL residual locks (STRONGER toward product BAR0 map/access +
 * STRONGER residual densify for IOREMAP/VA used by freestanding product
 * hosts + denser residual prove rollup; Soft!=product; G-AC-1;
 * Dual DoD A/B OPEN; agent!=close; stamp-free bar v2026.08.04.75;
 * NEVER invent .76).
 * H1: MMIO accessors thr/process context only — never net_eth_poll from
 * hard IRQ. thr-only product hosts (rtl/xhci) map BAR then thr ISR ack +
 * schedule_work. IOREMAP/VA densify: MAP_BAR grant VA -> window_register
 * -> host_window_lookup -> cookie pVa (product_hosts=UDX).
 * denser residual: map_chain + bar0 + access + product_host + dual_dod
 * denser prove under denser_prove master (never Dual DoD close).
 * greppable: udx: mmio soft residual functional
 * greppable: udx: mmio soft residual map_access
 * greppable: udx: mmio soft residual ioremap densify
 * greppable: udx: mmio soft residual denser
 * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
 * greppable: thr_only=1 | H1=no_net_eth_poll | DoD_A=OPEN | DoD_B=OPEN
 * greppable: ioremap_va_dense=1 | cookie_va_prove=1 | map_bar_va_path=1
 * greppable: denser_prove=1 | map_chain_dense=1 | bar0_dense=1
 * greppable: access_dense=1 | product_host_dense=1 | dual_dod_dense=1
 */
#define UDX_MMIO_H1_NO_NET_ETH_POLL  1u /* H1: no net_eth_poll from MMIO path */
#define UDX_MMIO_HARD_IRQ_ACCESS     0u /* accessors never hard-IRQ product */
#define UDX_MMIO_THR_ONLY_CTX        1u /* map/access thr/process context */
#define UDX_MMIO_FAIL_CLOSED_PHYS0   1u /* product BAR0 refuse phys==0 */
#define UDX_MMIO_ACCESS_READY_GATE   1u /* non-null VA + non-zero len cookie */
#define UDX_MMIO_IOREMAP_VA_DENSE    1u /* STRONGER densify IOREMAP/VA residual */
#define UDX_MMIO_COOKIE_VA_PROVE     1u /* cookie pVa from window_lookup VA */
#define UDX_MMIO_MAP_BAR_VA_PATH     1u /* MAP_BAR->window->ioremap VA path */
#define UDX_MMIO_FS_PRODUCT_HOSTS    1u /* freestanding product hosts=UDX */
#define UDX_MMIO_PRODUCT_HOSTS_UDX   1u /* product_hosts=UDX honesty lock */
#define UDX_MMIO_STAMP_FREE_75       1u /* bar v2026.08.04.75; never invent .76 */
#define UDX_MMIO_MAP_CHAIN_DENSE     1u /* denser MAP_BAR->window->ioremap prove */
#define UDX_MMIO_BAR0_DENSE          1u /* denser product BAR0 map/access */
#define UDX_MMIO_ACCESS_DENSE        1u /* denser map+access after cookie prove */
#define UDX_MMIO_PRODUCT_HOST_DENSE  1u /* denser product_host Dual DoD surface */
#define UDX_MMIO_DUAL_DOD_DENSE      1u /* denser Dual DoD A/B OPEN honesty */
#define UDX_MMIO_DENSER_PROVE        1u /* denser residual prove master lock */
#define UDX_MMIO_LEAN_CHECKS        17u /* denser functional self-check count */

_Static_assert(UDX_MMIO_SOFT_CAP_MINT == 0u,
               "MMIO_FRAME cap mint remains OPEN (mint=0)");
_Static_assert(UDX_MMIO_SOFT_DOD_A_OPEN == 1u,
               "Dual DoD A remains OPEN (xhci_udx)");
_Static_assert(UDX_MMIO_SOFT_DOD_B_OPEN == 1u,
               "Dual DoD B remains OPEN (rtl8168_udx)");
_Static_assert(UDX_MMIO_SOFT_AGENT_CLOSE == 0u,
               "soft residual lamps never close Dual DoD");
_Static_assert(UDX_MMIO_SOFT_NE_PRODUCT == 1u,
               "Soft!=product honesty lock");
_Static_assert(UDX_MMIO_H1_NO_NET_ETH_POLL == 1u,
               "H1: MMIO residual never net_eth_poll");
_Static_assert(UDX_MMIO_HARD_IRQ_ACCESS == 0u,
               "MMIO accessors never hard-IRQ product");
_Static_assert(UDX_MMIO_THR_ONLY_CTX == 1u,
               "MMIO map/access thr/process context only");
_Static_assert(UDX_MMIO_FAIL_CLOSED_PHYS0 == 1u,
               "product BAR0 refuse phys==0 fail-closed");
_Static_assert(UDX_MMIO_ACCESS_READY_GATE == 1u,
               "access-ready cookie gate residual");
_Static_assert(UDX_MMIO_IOREMAP_VA_DENSE == 1u,
               "STRONGER densify IOREMAP/VA freestanding product hosts");
_Static_assert(UDX_MMIO_COOKIE_VA_PROVE == 1u,
               "cookie pVa prove from host_window_lookup VA");
_Static_assert(UDX_MMIO_MAP_BAR_VA_PATH == 1u,
               "MAP_BAR->window_register->ioremap VA path densify");
_Static_assert(UDX_MMIO_FS_PRODUCT_HOSTS == 1u,
               "freestanding product hosts=UDX residual densify");
_Static_assert(UDX_MMIO_PRODUCT_HOSTS_UDX == 1u,
               "product_hosts=UDX honesty (Soft!=product dual_dod OPEN)");
_Static_assert(UDX_MMIO_STAMP_FREE_75 == 1u,
               "stamp-free residual bar v2026.08.04.75 (never .76)");
_Static_assert(UDX_MMIO_MAP_CHAIN_DENSE == 1u,
               "denser residual: MAP_BAR->window->ioremap prove");
_Static_assert(UDX_MMIO_BAR0_DENSE == 1u,
               "denser residual: product BAR0 map/access");
_Static_assert(UDX_MMIO_ACCESS_DENSE == 1u,
               "denser residual: map+access after cookie_va_prove");
_Static_assert(UDX_MMIO_PRODUCT_HOST_DENSE == 1u,
               "denser residual: product_host Dual DoD surface");
_Static_assert(UDX_MMIO_DUAL_DOD_DENSE == 1u,
               "denser residual: Dual DoD A/B OPEN honesty");
_Static_assert(UDX_MMIO_DENSER_PROVE == 1u,
               "denser residual prove master (Soft!=product Dual DoD OPEN)");
_Static_assert(UDX_MMIO_SOFT_WAVE == 75u,
               "soft wave aligns bar v2026.08.04.75 (never invent .76)");
_Static_assert(UDX_MMIO_LEAN_CHECKS == 17u,
               "FUNCTIONAL lean residual denser: 17 MMIO checks");

/*
 * Soft MMIO product inventory (exclusive deepen). Cumulative for
 * this process. greppable: udx: mmio soft …
 * Never hard-gates; wrap OK if ever hit.
 */
static u32 g_u32MmioMapEnter;    /* udx_ioremap entries */
static u32 g_u32MmioMapOk;       /* ioremap success */
static u32 g_u32MmioMapInval;    /* len 0 / overflow / phys==0 / not access-ready */
static u32 g_u32MmioMapLookup;   /* window lookup miss */
static u32 g_u32MmioMapNomem;    /* host malloc / fs pool fail */
static u32 g_u32MmioUnmap;       /* iounmap entries */
static u32 g_u32MmioUnmapNull;   /* iounmap null soft pointer */
static u32 g_u32MmioPoolLive;    /* freestanding iomem slots used */
static u32 g_u32MmioPoolPeak;    /* peak freestanding iomem slots */
static u32 g_u32MmioPoolFull;    /* freestanding pool full */
static u32 g_u32MmioLastLenLo;   /* last map len low 32 (soft snap) */
static u32 g_u32MmioLastLenHi;   /* last map len high 32 */
static u32 g_u32MmioLastPhysLo;  /* last map phys low 32 (soft snap) */
static u32 g_u32MmioLastPhysHi;  /* last map phys high 32 */
static u32 g_u32MmioLogN;        /* soft inventory dumps emitted */

/* Residual path counters (one-shot residual honesty; Soft!=product). */
static u32 g_u32MmioResidualIoremap;   /* residual ioremap lamps */
static u32 g_u32MmioResidualIounmap;   /* residual iounmap lamps */
static u32 g_u32MmioResidualAccessors; /* residual readl/writel catalog */
static u32 g_u32MmioResidualWriteFlush;/* residual write*_flush/posted_flush */
static u32 g_u32MmioResidualQuery;     /* residual is_mapped/len/phys/va */
static u32 g_u32MmioResidualUcMap;     /* residual UC map honesty */
static u32 g_u32MmioResidualThrash;    /* residual freestanding thrash refuse */
static u32 g_u32MmioResidualMapChain;  /* residual MAP_BAR->window->ioremap */
static u32 g_u32MmioResidualProdHost;  /* residual product_host Dual DoD surface */
static u32 g_u32MmioResidualDualDod;   /* residual Dual DoD A/B OPEN honesty */
static u32 g_u32MmioResidualMint;      /* residual MMIO_FRAME cap_mint honesty */
static u32 g_u32MmioResidualBar0;      /* residual product BAR0 map/access */
static u32 g_u32MmioResidualFunctional;/* residual map+access functional steps */
static u32 g_u32MmioResidualIoremapDense; /* STRONGER ioremap/VA densify lamps */
static u32 g_u32MmioResidualDenser;    /* denser residual prove rollup lamps */
static u32 g_u32MmioMapZeroPhys;       /* map refuse: phys==0 (product BAR0) */
static u32 g_u32MmioAccessReady;       /* map ok cookies access-ready for BAR0 */
static u32 g_u32MmioVaOk;              /* densify: window_lookup VA non-null */
static u32 g_u32MmioCookieVaProve;     /* densify: cookie pVa set from lookup */

static u8  g_fMmioSoftOnce;            /* one-shot deep dump after first ok map */
static u8  g_fMmioIoremapResidualOnce; /* one-shot ioremap residual */
static u8  g_fMmioIounmapResidualOnce; /* one-shot iounmap residual */
static u8  g_fMmioAccessorsResidualOnce;
static u8  g_fMmioWriteFlushResidualOnce;
static u8  g_fMmioQueryResidualOnce;
static u8  g_fMmioUcMapResidualOnce;
static u8  g_fMmioThrashResidualOnce;
static u8  g_fMmioMapChainResidualOnce;
static u8  g_fMmioProdHostResidualOnce;
static u8  g_fMmioDualDodResidualOnce;
static u8  g_fMmioMintResidualOnce;
static u8  g_fMmioBar0ResidualOnce;
static u8  g_fMmioFunctionalResidualOnce;
static u8  g_fMmioIoremapDenseOnce;    /* one-shot ioremap/VA densify residual */
static u8  g_fMmioDenserResidualOnce;  /* one-shot denser residual prove */

static void mmio_soft_inc(u32 *pu32);
static void mmio_soft_emit(const char *szFmt, ...);
static void mmio_soft_inventory_log(void);
static void mmio_soft_maybe_once(void);
static void mmio_soft_log_ioremap_residual_once(void);
static void mmio_soft_log_iounmap_residual_once(void);
static void mmio_soft_log_accessors_residual_once(void);
static void mmio_soft_log_write_flush_residual_once(void);
static void mmio_soft_log_query_residual_once(void);
static void mmio_soft_log_uc_map_residual_once(void);
static void mmio_soft_log_thrash_residual_once(void);
static void mmio_soft_log_map_chain_residual_once(void);
static void mmio_soft_log_prod_host_residual_once(void);
static void mmio_soft_log_dual_dod_residual_once(void);
static void mmio_soft_log_mint_residual_once(void);
static void mmio_soft_log_bar0_residual_once(void);
static void mmio_soft_log_functional_residual_once(void);
static void mmio_soft_log_ioremap_dense_once(void);
static void mmio_soft_log_denser_residual_once(void);

static void
mmio_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/*
 * Soft inventory emit path — host console; does not use udx_printk so
 * core soft printk tallies stay product-path pure.
 */
static void
mmio_soft_emit(const char *szFmt, ...)
{
    if (szFmt == NULL) {
        return;
    }
#if defined(UDX_HOST_LIBC)
    {
        va_list apArgs;

        va_start(apArgs, szFmt);
        (void)vprintf(szFmt, apArgs);
        va_end(apArgs);
    }
#else
    (void)szFmt;
#endif
}

/**
 * Greppable soft MMIO inventory + residual deepen.
 * Prefix-stable "udx: mmio soft …" — never hard-gates; observation only.
 * storm=0: once after first ok map (mmio_soft_maybe_once).
 *
 * greppable: udx: mmio soft
 */
static void
mmio_soft_inventory_log(void)
{
    u32 u32Host;
    u32 u32FsSlots;

    mmio_soft_inc(&g_u32MmioLogN);

#if defined(UDX_HOST_LIBC)
    u32Host = 1u;
    u32FsSlots = 0u;
#else
    u32Host = 0u;
    u32FsSlots = UDX_FS_IOMEM_SLOTS;
#endif

    /* Grep: udx: mmio soft inventory */
    mmio_soft_emit("udx: mmio soft inventory map_ok=%u map_inval=%u "
                   "map_lookup_miss=%u map_nomem=%u unmap=%u unmap_null=%u "
                   "log_n=%u wave=%u areas=%u\n",
                   g_u32MmioMapOk, g_u32MmioMapInval, g_u32MmioMapLookup,
                   g_u32MmioMapNomem, g_u32MmioUnmap, g_u32MmioUnmapNull,
                   g_u32MmioLogN, UDX_MMIO_SOFT_WAVE, UDX_MMIO_SOFT_AREAS);

    /* Grep: udx: mmio soft map */
    mmio_soft_emit("udx: mmio soft map enter=%u ok=%u inval=%u lookup_miss=%u "
                   "nomem=%u zero_phys=%u access_ready=%u "
                   "va_ok=%u cookie_va_prove=%u "
                   "last_len_lo=%u last_len_hi=%u "
                   "last_phys_lo=%u last_phys_hi=%u "
                   "via=host_window_lookup uc_intent=1 "
                   "bar0_product_surface=1 fail_closed_phys0=1 "
                   "ioremap_va_dense=%u map_bar_va_path=%u "
                   "product_hosts=UDX "
                   "product_MMIO_FRAME_cap=OPEN\n",
                   g_u32MmioMapEnter, g_u32MmioMapOk, g_u32MmioMapInval,
                   g_u32MmioMapLookup, g_u32MmioMapNomem, g_u32MmioMapZeroPhys,
                   g_u32MmioAccessReady, g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   g_u32MmioLastLenLo, g_u32MmioLastLenHi,
                   g_u32MmioLastPhysLo, g_u32MmioLastPhysHi,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_MAP_BAR_VA_PATH);

    /* Grep: udx: mmio soft unmap */
    mmio_soft_emit("udx: mmio soft unmap enter=%u null=%u "
                   "host_free=1 freestanding_pool_return=1 "
                   "clear_fields=1 product_phase_a_revoke=0\n",
                   g_u32MmioUnmap, g_u32MmioUnmapNull);

    /* Grep: udx: mmio soft pool */
    mmio_soft_emit("udx: mmio soft pool host_libc=%u freestanding=%u "
                   "slots=%u live=%u peak=%u full=%u thrash_refuse=1 "
                   "pool_expand=0 class_reenable=0\n",
                   u32Host, (u32Host == 0u) ? 1u : 0u, u32FsSlots,
                   g_u32MmioPoolLive, g_u32MmioPoolPeak, g_u32MmioPoolFull);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: mmio soft path
     */
    mmio_soft_emit("udx: mmio soft path map=udx_ioremap unmap=udx_iounmap "
                   "lookup=udx_host_window_lookup "
                   "grant_bar=udx_host_bind_by_id "
                   "map_chain=MAP_BAR,window_register,ioremap "
                   "accessors=header_inlines "
                   "readl=udx_readl writel=udx_writel "
                   "write_flush=writel_flush|posted_flush "
                   "query=is_mapped|len|phys|va "
                   "uc_map=host_window_VA "
                   "bar0=product_pref "
                   "functional=map+access "
                   "hot_cold_abi=1 ddi_udx=1 "
                   "layout_ver=%u "
                   "skeleton_gate=0 hard_gate=0 soft=1\n",
                   UDX_MMIO_LAYOUT_VER);

    /* Grep: udx: mmio soft wave */
    mmio_soft_emit("udx: mmio soft wave n=%u areas=%u unit=mmio exclusive=1 "
                   "prefix=udx:_mmio_soft deepen=1 "
                   "(soft inventory; never gates skeleton PASS)\n",
                   UDX_MMIO_SOFT_WAVE, UDX_MMIO_SOFT_AREAS);

    /*
     * Product host shape catalog (Dual DoD A/B OPEN seed; agent!=close).
     * greppable: udx: mmio soft product host
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    mmio_soft_emit("udx: mmio soft product host "
                   "shape=ioremap|iounmap|accessors|write_flush|query|"
                   "uc_map|map_chain|bar0|functional|mint|ioremap_va_dense "
                   "map=udx_ioremap unmap=udx_iounmap "
                   "accessors=readl/writel/readw/writew/readb/writeb/"
                   "readq/writeq "
                   "write_flush=writel_flush|writew_flush|writeb_flush|"
                   "writeq_flush|posted_flush "
                   "query=is_mapped|len|phys|va "
                   "bulk=memcpy_fromio,memcpy_toio,memset_io "
                   "map_chain=MAP_BAR->window_register->ioremap "
                   "ioremap_va_dense=%u cookie_va_prove=%u "
                   "map_bar_va_path=%u "
                   "bar0_rtl_pref=%u bar0_xhci_pref=%u "
                   "rtl_pref_mask=0x%x xhci_pref_mask=0x%x "
                   "rtl8168_udx=DoD_B_OPEN "
                   "xhci_udx=DoD_A_OPEN "
                   "product_hosts=UDX "
                   "product=UDX+DDI+hot_cold_ABI "
                   "agent_close=0 soft=1 product_mint=%u "
                   "layout_ver=%u G-AC-1=1 Soft!=product "
                   "dual_dod OPEN product_hosts=UDX wave=%u\n",
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_MAP_BAR_VA_PATH,
                   (u32)UDX_MMIO_RTL_PREF_BAR0, (u32)UDX_MMIO_XHCI_PREF_BAR0,
                   UDX_MMIO_RTL_PREF_BAR_MASK, UDX_MMIO_XHCI_PREF_BAR_MASK,
                   UDX_MMIO_SOFT_CAP_MINT,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * Grep: udx: mmio soft honesty
     * Soft inventory != product multi-server confine.
     */
    mmio_soft_emit("udx: mmio soft honesty multi_server=0 confine=0 "
                   "exclusive=1 soft=1 product_kernel=OPEN "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "layout_ver=%u wave=%u\n",
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * Lean residual MMIO API honesty (Soft!=product; dual MIT OR Apache-2.0).
     * Deepen C2: ioremap/iounmap + accessors + write_flush + query +
     * UC map + thrash refuse + map_chain + product_host + dual_dod OPEN +
     * mint + bar0 + functional map/access toward product BAR0.
     * greppable: udx: mmio soft residual lean
     * greppable: udx: mmio soft residual lean layout_ver=
     * greppable: udx: mmio soft residual lean UDX eng
     * greppable: udx: mmio soft residual ioremap
     * greppable: udx: mmio soft residual iounmap
     * greppable: udx: mmio soft residual accessors
     * greppable: udx: mmio soft residual write_flush
     * greppable: udx: mmio soft residual query
     * greppable: udx: mmio soft residual uc_map
     * greppable: udx: mmio soft residual freestanding thrash refuse
     * greppable: udx: mmio soft residual map_chain
     * greppable: udx: mmio soft residual product_host
     * greppable: udx: mmio soft residual dual_dod
     * greppable: udx: mmio soft residual mint
     * greppable: udx: mmio soft residual bar0
     * greppable: udx: mmio soft residual functional
     * greppable: udx: mmio soft residual map_access
     * greppable: udx: mmio soft residual ioremap densify
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     * greppable: udx: mmio soft api honesty
     */
    mmio_soft_emit("udx: mmio soft residual lean "
                   "layout_ver=%u "
                   "map=udx_ioremap unmap=udx_iounmap "
                   "accessors=readl/writel write_flush=1 query=1 "
                   "via=host_window_lookup uc_map=1 "
                   "map_chain=MAP_BAR,window_register,ioremap "
                   "bar0=1 functional=map+access mint=1 "
                   "ioremap_va_dense=%u cookie_va_prove=%u "
                   "map_ok=%u unmap=%u zero_phys=%u access_ready=%u "
                   "va_ok=%u cookie_va_n=%u "
                   "residual_ioremap=%u residual_iounmap=%u "
                   "residual_accessors=%u residual_write_flush=%u "
                   "residual_query=%u residual_uc_map=%u "
                   "residual_thrash=%u residual_map_chain=%u "
                   "residual_prod_host=%u residual_dual_dod=%u "
                   "residual_mint=%u residual_bar0=%u "
                   "residual_functional=%u residual_ioremap_dense=%u "
                   "product_MMIO_FRAME_cap=OPEN soft=1 "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx,xhci_udx "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "hot_cold_abi=1 freestanding_thrash_refuse=1 "
                   "soft_ne_product=1 dual=MIT_OR_Apache-2.0 G-AC-1 "
                   "Soft!=product dual_dod OPEN product_hosts=UDX "
                   "storm=0 wave=%u "
                   "(Soft!=product; lean residual only)\n",
                   UDX_MMIO_LAYOUT_VER,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   g_u32MmioMapOk, g_u32MmioUnmap,
                   g_u32MmioMapZeroPhys, g_u32MmioAccessReady,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   g_u32MmioResidualIoremap, g_u32MmioResidualIounmap,
                   g_u32MmioResidualAccessors, g_u32MmioResidualWriteFlush,
                   g_u32MmioResidualQuery, g_u32MmioResidualUcMap,
                   g_u32MmioResidualThrash, g_u32MmioResidualMapChain,
                   g_u32MmioResidualProdHost, g_u32MmioResidualDualDod,
                   g_u32MmioResidualMint, g_u32MmioResidualBar0,
                   g_u32MmioResidualFunctional, g_u32MmioResidualIoremapDense,
                   UDX_MMIO_SOFT_WAVE);
    mmio_soft_emit("udx: mmio soft residual lean UDX eng "
                   "layout_ver=%u "
                   "shape=ioremap|iounmap|accessors|write_flush|query|"
                   "uc_map|map_chain|product_host|dual_dod|"
                   "mint|bar0|functional|ioremap_va_dense "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx|xhci_udx "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "product_mmio_frame=OPEN "
                   "product=UDX+DDI+hot_cold_ABI "
                   "ioremap_va_dense=%u map_bar_va_path=%u "
                   "bar0_rtl=%u bar0_xhci=%u "
                   "map_ok=%u unmap=%u residual_map_chain_n=%u "
                   "residual_write_flush_n=%u residual_query_n=%u "
                   "residual_prod_host_n=%u residual_dual_dod_n=%u "
                   "residual_mint_n=%u residual_bar0_n=%u "
                   "residual_functional_n=%u residual_ioremap_dense_n=%u "
                   "soft=1 G-AC-1=1 Soft!=product dual_dod OPEN "
                   "product_hosts=UDX wave=%u\n",
                   UDX_MMIO_LAYOUT_VER,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_MAP_BAR_VA_PATH,
                   (u32)UDX_MMIO_RTL_PREF_BAR0, (u32)UDX_MMIO_XHCI_PREF_BAR0,
                   g_u32MmioMapOk, g_u32MmioUnmap,
                   g_u32MmioResidualMapChain,
                   g_u32MmioResidualWriteFlush, g_u32MmioResidualQuery,
                   g_u32MmioResidualProdHost, g_u32MmioResidualDualDod,
                   g_u32MmioResidualMint, g_u32MmioResidualBar0,
                   g_u32MmioResidualFunctional, g_u32MmioResidualIoremapDense,
                   UDX_MMIO_SOFT_WAVE);
    mmio_soft_emit("udx: mmio soft api honesty "
                   "product_mmio_frame_cap=OPEN "
                   "soft_window_lookup=1 freestanding_pool=1 "
                   "uc_map_honesty=1 accessors=header_inlines "
                   "write_flush=header_inlines query=header_inlines "
                   "map_chain=MAP_BAR,window_register,ioremap "
                   "bar0_product_surface=1 functional_map_access=1 "
                   "cap_mint=%u multi_server=%u confine=%u "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "hot_cold_abi=1 layout_ver=%u "
                   "ioremap_residual_n=%u iounmap_residual_n=%u "
                   "accessors_residual_n=%u write_flush_residual_n=%u "
                   "query_residual_n=%u uc_map_residual_n=%u "
                   "thrash_refuse_residual_n=%u map_chain_residual_n=%u "
                   "prod_host_residual_n=%u dual_dod_residual_n=%u "
                   "mint_residual_n=%u bar0_residual_n=%u "
                   "functional_residual_n=%u "
                   "soft=1 product_kernel=OPEN Soft!=product wave=%u\n",
                   UDX_MMIO_SOFT_CAP_MINT, UDX_MMIO_SOFT_MULTI_SERVER,
                   UDX_MMIO_SOFT_CONFINE,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_LAYOUT_VER,
                   g_u32MmioResidualIoremap, g_u32MmioResidualIounmap,
                   g_u32MmioResidualAccessors, g_u32MmioResidualWriteFlush,
                   g_u32MmioResidualQuery, g_u32MmioResidualUcMap,
                   g_u32MmioResidualThrash, g_u32MmioResidualMapChain,
                   g_u32MmioResidualProdHost, g_u32MmioResidualDualDod,
                   g_u32MmioResidualMint, g_u32MmioResidualBar0,
                   g_u32MmioResidualFunctional,
                   UDX_MMIO_SOFT_WAVE);

    /*
     * ioremap residual for UDX product hosts.
     * greppable: udx: mmio soft residual ioremap
     * greppable: udx: mmio soft residual ioremap densify
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    mmio_soft_emit("udx: mmio soft residual ioremap "
                   "map=udx_ioremap via=host_window_lookup "
                   "grant=udx_host_window_register "
                   "bind=udx_host_bind_by_id "
                   "map_chain=MAP_BAR,window_register,ioremap "
                   "enter=%u ok=%u inval=%u lookup_miss=%u nomem=%u "
                   "va_ok=%u cookie_va_prove=%u access_ready=%u "
                   "residual_n=%u residual_dense_n=%u "
                   "ioremap_va_dense=%u cookie_va_prove_lock=%u "
                   "map_bar_va_path=%u "
                   "product_MMIO_FRAME_cap=OPEN "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx,xhci_udx "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "soft=1 product=0 soft_ne_product=1 "
                   "Soft!=product dual_dod OPEN product_hosts=UDX "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(ioremap residual; window lookup VA only)\n",
                   g_u32MmioMapEnter, g_u32MmioMapOk, g_u32MmioMapInval,
                   g_u32MmioMapLookup, g_u32MmioMapNomem,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve, g_u32MmioAccessReady,
                   g_u32MmioResidualIoremap, g_u32MmioResidualIoremapDense,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_MAP_BAR_VA_PATH,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
    /*
     * STRONGER residual densify for IOREMAP/VA used by freestanding
     * product hosts (MAP_BAR path). Soft!=product; Dual DoD OPEN.
     * greppable: udx: mmio soft residual ioremap densify
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    mmio_soft_emit("udx: mmio soft residual ioremap densify "
                   "STRONGER=1 ioremap_va_dense=%u "
                   "cookie_va_prove=%u map_bar_va_path=%u "
                   "fs_product_hosts=%u product_hosts_udx=%u "
                   "via=host_window_lookup "
                   "chain=MAP_BAR,window_register,ioremap "
                   "cookie_pVa=window_lookup_VA "
                   "freestanding_pool=1 host_libc_malloc=1 "
                   "va_ok=%u cookie_va_n=%u access_ready=%u map_ok=%u "
                   "lookup_miss=%u zero_phys=%u residual_dense_n=%u "
                   "product_MMIO_FRAME_cap=OPEN cap_mint=%u "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx,xhci_udx "
                   "product=UDX+DDI+hot_cold_ABI "
                   "soft=1 product=0 soft_ne_product=1 "
                   "Soft!=product dual_dod OPEN product_hosts=UDX "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "stamp_free=1 bar=v2026.08.04.75 never_invent=.76 "
                   "layout_ver=%u wave=%u lean_checks=%u "
                   "(STRONGER densify IOREMAP/VA freestanding product hosts)\n",
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_MAP_BAR_VA_PATH, UDX_MMIO_FS_PRODUCT_HOSTS,
                   UDX_MMIO_PRODUCT_HOSTS_UDX,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   g_u32MmioAccessReady, g_u32MmioMapOk,
                   g_u32MmioMapLookup, g_u32MmioMapZeroPhys,
                   g_u32MmioResidualIoremapDense,
                   UDX_MMIO_SOFT_CAP_MINT,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE,
                   UDX_MMIO_LEAN_CHECKS);

    /*
     * iounmap residual — soft clear + free/pool != product revoke.
     * greppable: udx: mmio soft residual iounmap
     */
    mmio_soft_emit("udx: mmio soft residual iounmap "
                   "unmap=udx_iounmap clear_fields=1 "
                   "host_free=1 freestanding_pool_return=1 "
                   "enter=%u null=%u residual_n=%u "
                   "product_phase_a_revoke=0 product_cap_destroy=0 "
                   "product_MMIO_FRAME_cap=OPEN "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(iounmap residual; no product revoke)\n",
                   g_u32MmioUnmap, g_u32MmioUnmapNull,
                   g_u32MmioResidualIounmap,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * readl / writel residual (header inlines; catalog honesty only).
     * Natural-align gate for fixed-width; bulk uses span gate only.
     * greppable: udx: mmio soft residual accessors
     */
    mmio_soft_emit("udx: mmio soft residual accessors "
                   "readl=udx_readl writel=udx_writel "
                   "readw=udx_readw writew=udx_writew "
                   "readb=udx_readb writeb=udx_writeb "
                   "readq=udx_readq writeq=udx_writeq "
                   "ioread=udx_ioread* iowrite=udx_iowrite* "
                   "rmw=setl/clearl "
                   "bulk=memcpy_fromio,memcpy_toio,memset_io "
                   "range_gate=udx_mmio_in_range "
                   "nat_align_gate=udx_mmio_in_range_nat "
                   "fail_closed_read=all_ones fail_closed_write=nop "
                   "volatile=1 flush=udx_mmio_flush "
                   "header_inlines=1 residual_n=%u "
                   "product_hosts=rtl8168_udx,xhci_udx "
                   "rtl8168_udx=readl/w/b+writew residual "
                   "xhci_udx=readl/w/b residual "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(readl/writel residual; header inlines)\n",
                   g_u32MmioResidualAccessors,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * write*_flush + posted_flush residual (product-host ISR/mask pattern).
     * Soft!=product; UC leaf still host/VMM; not product bridge/CFG flush.
     * greppable: udx: mmio soft residual write_flush
     */
    mmio_soft_emit("udx: mmio soft residual write_flush "
                   "writel_flush=udx_writel_flush "
                   "writew_flush=udx_writew_flush "
                   "writeb_flush=udx_writeb_flush "
                   "writeq_flush=udx_writeq_flush "
                   "posted_flush=udx_mmio_posted_flush "
                   "fence=udx_mmio_flush "
                   "x86_64=mfence aarch64=dmb_sy "
                   "posted_drain=fence_then_safe_readl "
                   "header_inlines=1 residual_n=%u "
                   "product_hosts=rtl8168_udx,xhci_udx "
                   "rtl8168_udx=IntrStatus/IntrMask_write_then_flush "
                   "product_bridge_cfg_flush=0 "
                   "product_MMIO_FRAME_cap=OPEN "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(write_flush residual; posted-write intent only)\n",
                   g_u32MmioResidualWriteFlush,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * Map query residual — observation only (is_mapped/len/phys/va).
     * Soft!=product; not a product MMIO_FRAME cap handle.
     * greppable: udx: mmio soft residual query
     */
    mmio_soft_emit("udx: mmio soft residual query "
                   "is_mapped=udx_mmio_is_mapped "
                   "len=udx_mmio_len phys=udx_mmio_phys va=udx_mmio_va "
                   "layout_ver_q=udx_mmio_layout_ver "
                   "shape_ok=udx_mmio_shape_ok "
                   "fs_slots_q=udx_mmio_fs_iomem_slots "
                   "observation_only=1 residual_n=%u "
                   "product_MMIO_FRAME_cap_handle=0 "
                   "product_hosts=rtl8168_udx,xhci_udx "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(query residual; observation only)\n",
                   g_u32MmioResidualQuery,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * UC map honesty — soft VA expects host UC leaf attrs.
     * UDX does not re-attr pages; product MMIO_FRAME remains OPEN.
     * greppable: udx: mmio soft residual uc_map
     */
    mmio_soft_emit("udx: mmio soft residual uc_map honesty "
                   "uc_intent=1 host_window_VA=1 "
                   "user_device=vmm_map_user_device "
                   "map_device_uc=vmm_map_device_uc "
                   "pcd_pwt_intent=1 wb_identity=0 "
                   "udx_reattr=0 identity_rewrite=0 "
                   "cache_ops_in_udx=0 leaf_attrs=host_VMM "
                   "residual_n=%u "
                   "product_MMIO_FRAME_cap=OPEN "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(UC map honesty; host/VMM owns leaf attrs)\n",
                   g_u32MmioResidualUcMap,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * Freestanding thrash refuse — product = UDX hosts, not kernel class.
     * greppable: udx: mmio soft residual freestanding thrash refuse
     */
    mmio_soft_emit("udx: mmio soft residual freestanding thrash refuse "
                   "pool_expand=0 class_reenable=0 pool_thrash=0 "
                   "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
                   "GJ_RTL8168_PROBE=0 GJ_XHCI_MSC_PROBE=0 "
                   "product=UDX+DDI+hot_cold_ABI "
                   "hosts=rtl8168_udx,xhci_udx "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "fs_iomem_slots_fixed=1 residual_n=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(freestanding thrash refuse; product=UDX hosts)\n",
                   g_u32MmioResidualThrash,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * MAP_BAR -> window_register -> ioremap chain residual (C2 product path).
     * Soft window lookup only; product MMIO_FRAME mint remains OPEN.
     * Dual DoD A/B OPEN; agent!=close. STRONGER densify: IOREMAP/VA path
     * for freestanding product hosts (product_hosts=UDX).
     * greppable: udx: mmio soft residual map_chain
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    mmio_soft_emit("udx: mmio soft residual map_chain "
                   "chain=MAP_BAR,window_register,ioremap "
                   "map_bar=udx_host_bind_by_id|MAP_BAR "
                   "window=udx_host_window_register "
                   "lookup=udx_host_window_lookup "
                   "map=udx_ioremap "
                   "va_path=MAP_BAR_grant_VA->window_register->"
                   "host_window_lookup->cookie_pVa "
                   "ioremap_va_dense=%u cookie_va_prove=%u "
                   "map_bar_va_path=%u "
                   "preferred_rtl=BAR0+BAR2 preferred_xhci=BAR0 "
                   "rtl_bar0=%u rtl_bar2=%u xhci_bar0=%u "
                   "rtl_pref_mask=0x%x xhci_pref_mask=0x%x "
                   "ddi_grant_bar0=1 "
                   "product_MMIO_FRAME_cap=OPEN cap_mint=%u "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx,xhci_udx "
                   "product=UDX+DDI+hot_cold_ABI "
                   "residual_n=%u map_ok=%u lookup_miss=%u "
                   "va_ok=%u cookie_va_n=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "Soft!=product dual_dod OPEN product_hosts=UDX "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(map_chain residual; C2 Dual DoD MAP_BAR VA densify)\n",
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_MAP_BAR_VA_PATH,
                   (u32)UDX_MMIO_RTL_PREF_BAR0, (u32)UDX_MMIO_RTL_PREF_BAR2,
                   (u32)UDX_MMIO_XHCI_PREF_BAR0,
                   UDX_MMIO_RTL_PREF_BAR_MASK, UDX_MMIO_XHCI_PREF_BAR_MASK,
                   UDX_MMIO_SOFT_CAP_MINT,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   g_u32MmioResidualMapChain, g_u32MmioMapOk,
                   g_u32MmioMapLookup,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * Product-host MMIO residual (Dual DoD A/B seed; Soft!=product).
     * BAR map + accessor surface for rtl8168_udx / xhci_udx.
     * product_hosts=UDX; freestanding IOREMAP/VA densify.
     * greppable: udx: mmio soft residual product_host
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    mmio_soft_emit("udx: mmio soft residual product_host "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx,xhci_udx "
                   "surface=ioremap|accessors|write_flush|query|"
                   "uc_map|map_chain|bar0|functional|ioremap_va_dense "
                   "product=UDX+DDI+hot_cold_ABI "
                   "Linux-shaped_userspace=1 freestanding_class=SKIP "
                   "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
                   "rtl8168_udx=DoD_B_OPEN "
                   "rtl8168_path=BAR0+BAR2_map+readl/w/b+writew+flush "
                   "xhci_udx=DoD_A_OPEN "
                   "xhci_path=BAR0_cap_map+readl/w/b residual "
                   "bar0_product=1 map_access=1 "
                   "ioremap_va_dense=%u cookie_va_prove=%u "
                   "map_bar_va_path=%u "
                   "map=udx_ioremap via=host_window_lookup "
                   "product_MMIO_FRAME_cap=OPEN cap_mint=%u "
                   "agent_close=%u residual_n=%u residual_dense_n=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "Soft!=product dual_dod OPEN product_hosts=UDX "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(product-host MMIO residual; Soft!=product)\n",
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_MAP_BAR_VA_PATH,
                   UDX_MMIO_SOFT_CAP_MINT, UDX_MMIO_SOFT_AGENT_CLOSE,
                   g_u32MmioResidualProdHost, g_u32MmioResidualIoremapDense,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * Dual DoD A/B OPEN residual honesty (agent!=close; Soft!=product).
     * Soft MMIO lamps alone never close Dual DoD.
     * greppable: udx: mmio soft residual dual_dod
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    mmio_soft_emit("udx: mmio soft residual dual_dod "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "A_host=xhci_udx A_surface=USB_UDX_DDI_BAR0 "
                   "B_host=rtl8168_udx B_surface=NIC_UDX_DDI_BAR0+2 "
                   "map_chain=MAP_BAR,window_register,ioremap "
                   "ioremap_va_dense=%u map_bar_va_path=%u "
                   "window_lookup=1 soft_lamp_close=0 "
                   "freestanding_rtl=SKIP freestanding_usb=SKIP "
                   "in_kernel_ko=0 G-AC-1=1 product=UDX+ABI "
                   "product_hosts=UDX "
                   "product_MMIO_FRAME_cap=OPEN residual_n=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "Soft!=product dual_dod OPEN product_hosts=UDX "
                   "dual=MIT_OR_Apache-2.0 "
                   "layout_ver=%u wave=%u "
                   "(Dual DoD A/B OPEN; soft residual != close)\n",
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_MAP_BAR_VA_PATH,
                   g_u32MmioResidualDualDod,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * mint residual -- product MMIO_FRAME cap remains OPEN (always 0 mint).
     * greppable: udx: mmio soft residual mint
     */
    mmio_soft_emit("udx: mmio soft residual mint "
                   "product_MMIO_FRAME_cap_mint=OPEN "
                   "cap_mint=%u multi_server=%u confine=%u "
                   "cnode=0 window_lookup=1 "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "product_hosts=rtl8168_udx,xhci_udx "
                   "freestanding_class=SKIP residual_n=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(product MMIO_FRAME cap mint OPEN)\n",
                   UDX_MMIO_SOFT_CAP_MINT, UDX_MMIO_SOFT_MULTI_SERVER,
                   UDX_MMIO_SOFT_CONFINE,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   g_u32MmioResidualMint,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * Product BAR0 map/access residual (Dual DoD A/B BAR0 surface).
     * xhci: BAR0 only; rtl: DDI grant BAR0 + silicon-common BAR2.
     * greppable: udx: mmio soft residual bar0
     * greppable: udx: mmio soft residual map_access
     */
    mmio_soft_emit("udx: mmio soft residual bar0 "
                   "bar_idx=0 "
                   "rtl_pref_bar0=%u rtl_pref_bar2=%u xhci_pref_bar0=%u "
                   "rtl_pref_mask=0x%x xhci_pref_mask=0x%x "
                   "ddi_grant_bar0=1 "
                   "A_host=xhci_udx A_bar0=cap_map "
                   "B_host=rtl8168_udx B_bar0=ddi_grant "
                   "B_bar2=silicon_common_mmio "
                   "map=udx_ioremap via=host_window_lookup "
                   "access=readl/writel/readw/writew/readb/writeb "
                   "write_flush=1 posted_flush=1 query=1 "
                   "fail_closed_phys0=1 access_ready=%u zero_phys=%u "
                   "map_ok=%u residual_n=%u "
                   "product_MMIO_FRAME_cap=OPEN cap_mint=%u "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(product BAR0 map/access residual)\n",
                   (u32)UDX_MMIO_RTL_PREF_BAR0, (u32)UDX_MMIO_RTL_PREF_BAR2,
                   (u32)UDX_MMIO_XHCI_PREF_BAR0,
                   UDX_MMIO_RTL_PREF_BAR_MASK, UDX_MMIO_XHCI_PREF_BAR_MASK,
                   g_u32MmioAccessReady, g_u32MmioMapZeroPhys,
                   g_u32MmioMapOk, g_u32MmioResidualBar0,
                   UDX_MMIO_SOFT_CAP_MINT,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
    mmio_soft_emit("udx: mmio soft residual map_access "
                   "map=udx_ioremap access=header_inlines "
                   "range_gate=udx_mmio_in_range "
                   "nat_align_gate=udx_mmio_in_range_nat "
                   "fail_closed_read=all_ones fail_closed_write=nop "
                   "fail_closed_phys0=1 "
                   "write_flush=udx_writel_flush|posted_flush "
                   "bar0_surface=1 product_hosts=rtl8168_udx,xhci_udx "
                   "access_ready=%u map_ok=%u residual_bar0_n=%u "
                   "residual_functional_n=%u "
                   "product_MMIO_FRAME_cap=OPEN "
                   "soft=1 product=0 soft_ne_product=1 "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(map+access residual toward product BAR0)\n",
                   g_u32MmioAccessReady, g_u32MmioMapOk,
                   g_u32MmioResidualBar0, g_u32MmioResidualFunctional,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);

    /*
     * Functional residual — map + access steps toward product BAR0.
     * Never closes Dual DoD; Soft!=product denser catalog only.
     * STRONGER densify: IOREMAP/VA freestanding product hosts.
     * greppable: udx: mmio soft residual functional
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    mmio_soft_emit("udx: mmio soft residual functional "
                   "steps=map,access "
                   "step_map=udx_ioremap_via_window "
                   "step_access=readl/writel_nat_align "
                   "bar0_product=1 "
                   "fail_closed_phys0=%u access_ready=%u "
                   "zero_phys=%u map_ok=%u unmap=%u "
                   "va_ok=%u cookie_va_prove=%u "
                   "ioremap_va_dense=%u map_bar_va_path=%u "
                   "map_chain_dense=%u bar0_dense=%u access_dense=%u "
                   "product_host_dense=%u dual_dod_dense=%u denser_prove=%u "
                   "thr_only_ctx=%u hard_irq_access=%u "
                   "H1=no_net_eth_poll lean_checks=%u "
                   "residual_n=%u residual_bar0_n=%u "
                   "residual_ioremap_dense_n=%u residual_denser_n=%u "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx,xhci_udx "
                   "product=UDX+DDI+hot_cold_ABI "
                   "product_MMIO_FRAME_cap=OPEN cap_mint=%u "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "Soft!=product dual_dod OPEN product_hosts=UDX "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "layout_ver=%u wave=%u "
                   "(functional map+access residual; Soft!=product; denser)\n",
                   UDX_MMIO_FAIL_CLOSED_PHYS0, g_u32MmioAccessReady,
                   g_u32MmioMapZeroPhys,
                   g_u32MmioMapOk, g_u32MmioUnmap,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_MAP_BAR_VA_PATH,
                   UDX_MMIO_MAP_CHAIN_DENSE, UDX_MMIO_BAR0_DENSE,
                   UDX_MMIO_ACCESS_DENSE, UDX_MMIO_PRODUCT_HOST_DENSE,
                   UDX_MMIO_DUAL_DOD_DENSE, UDX_MMIO_DENSER_PROVE,
                   UDX_MMIO_THR_ONLY_CTX, UDX_MMIO_HARD_IRQ_ACCESS,
                   UDX_MMIO_LEAN_CHECKS,
                   g_u32MmioResidualFunctional, g_u32MmioResidualBar0,
                   g_u32MmioResidualIoremapDense, g_u32MmioResidualDenser,
                   UDX_MMIO_SOFT_CAP_MINT,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
    /*
     * denser residual prove rollup (product Dual DoD; Soft!=product).
     * greppable: udx: mmio soft residual denser
     * greppable: denser_prove=1 | map_chain_dense=1 | bar0_dense=1
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    mmio_soft_emit("udx: mmio soft residual denser "
                   "denser_prove=%u map_chain_dense=%u bar0_dense=%u "
                   "access_dense=%u product_host_dense=%u dual_dod_dense=%u "
                   "ioremap_va_dense=%u cookie_va_prove=%u map_bar_va_path=%u "
                   "va_ok=%u cookie_va_n=%u access_ready=%u map_ok=%u "
                   "zero_phys=%u residual_denser_n=%u residual_dense_n=%u "
                   "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
                   "ids=10ec:8168,8086:a12f "
                   "product=UDX+DDI+hot_cold_ABI freestanding_class=SKIP "
                   "product_MMIO_FRAME_cap=OPEN cap_mint=%u "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "soft=1 product=0 soft_ne_product=1 "
                   "Soft!=product dual_dod OPEN product_hosts=UDX "
                   "dual=MIT_OR_Apache-2.0 G-AC-1=1 "
                   "stamp_free=1 bar=v2026.08.04.75 never_invent=.76 "
                   "layout_ver=%u wave=%u lean_checks=%u "
                   "(denser residual prove; Soft!=product Dual DoD OPEN)\n",
                   UDX_MMIO_DENSER_PROVE, UDX_MMIO_MAP_CHAIN_DENSE,
                   UDX_MMIO_BAR0_DENSE, UDX_MMIO_ACCESS_DENSE,
                   UDX_MMIO_PRODUCT_HOST_DENSE, UDX_MMIO_DUAL_DOD_DENSE,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_MAP_BAR_VA_PATH,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   g_u32MmioAccessReady, g_u32MmioMapOk, g_u32MmioMapZeroPhys,
                   g_u32MmioResidualDenser, g_u32MmioResidualIoremapDense,
                   UDX_MMIO_SOFT_CAP_MINT,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE,
                   UDX_MMIO_LEAN_CHECKS);

}

/** Soft: one-shot ioremap residual honesty (sparse; no stamp storm). */
static void
mmio_soft_log_ioremap_residual_once(void)
{
    if (g_fMmioIoremapResidualOnce != 0) {
        return;
    }
    g_fMmioIoremapResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualIoremap);
    /* greppable: udx: mmio soft residual ioremap */
    /* greppable: Soft!=product dual_dod OPEN product_hosts=UDX */
    mmio_soft_emit("udx: mmio soft residual ioremap "
                   "via=host_window_lookup product_MMIO_FRAME_cap_mint=OPEN "
                   "cap_mint=%u bar0_product_surface=1 "
                   "fail_closed_phys0=1 access_ready=%u "
                   "va_ok=%u cookie_va_prove=%u "
                   "ioremap_va_dense=%u map_bar_va_path=%u "
                   "product_hosts=UDX "
                   "soft=1 Soft!=product dual_dod OPEN product_hosts=UDX "
                   "layout_ver=%u wave=%u\n",
                   UDX_MMIO_SOFT_CAP_MINT, g_u32MmioAccessReady,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_MAP_BAR_VA_PATH,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot STRONGER densify for IOREMAP/VA freestanding product hosts.
 * Sparse; no stamp storm. Soft!=product dual_dod OPEN product_hosts=UDX.
 * greppable: udx: mmio soft residual ioremap densify
 */
static void
mmio_soft_log_ioremap_dense_once(void)
{
    if (g_fMmioIoremapDenseOnce != 0) {
        return;
    }
    g_fMmioIoremapDenseOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualIoremapDense);
    /* greppable: udx: mmio soft residual ioremap densify */
    /* greppable: Soft!=product dual_dod OPEN product_hosts=UDX */
    mmio_soft_emit("udx: mmio soft residual ioremap densify "
                   "STRONGER=1 ioremap_va_dense=%u cookie_va_prove=%u "
                   "map_bar_va_path=%u fs_product_hosts=%u "
                   "via=host_window_lookup cookie_pVa=window_lookup_VA "
                   "va_ok=%u cookie_va_n=%u access_ready=%u map_ok=%u "
                   "product_MMIO_FRAME_cap=OPEN cap_mint=%u "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx,xhci_udx "
                   "soft=1 Soft!=product dual_dod OPEN product_hosts=UDX "
                   "stamp_free=1 bar=v2026.08.04.75 never_invent=.76 "
                   "layout_ver=%u wave=%u lean_checks=%u\n",
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_MAP_BAR_VA_PATH, UDX_MMIO_FS_PRODUCT_HOSTS,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   g_u32MmioAccessReady, g_u32MmioMapOk,
                   UDX_MMIO_SOFT_CAP_MINT,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE,
                   UDX_MMIO_LEAN_CHECKS);
}

/** Soft: one-shot iounmap residual honesty (sparse; no stamp storm). */
static void
mmio_soft_log_iounmap_residual_once(void)
{
    if (g_fMmioIounmapResidualOnce != 0) {
        return;
    }
    g_fMmioIounmapResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualIounmap);
    /* greppable: udx: mmio soft residual iounmap */
    mmio_soft_emit("udx: mmio soft residual iounmap "
                   "clear_fields=1 product_phase_a_revoke=0 "
                   "product_MMIO_FRAME_cap_mint=OPEN soft=1 "
                   "layout_ver=%u wave=%u\n",
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/** Soft: one-shot readl/writel accessor residual catalog. */
static void
mmio_soft_log_accessors_residual_once(void)
{
    if (g_fMmioAccessorsResidualOnce != 0) {
        return;
    }
    g_fMmioAccessorsResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualAccessors);
    /* greppable: udx: mmio soft residual accessors */
    mmio_soft_emit("udx: mmio soft residual accessors "
                   "readl=udx_readl writel=udx_writel "
                   "range_gate=udx_mmio_in_range "
                   "nat_align_gate=udx_mmio_in_range_nat "
                   "rmw=setl/clearl header_inlines=1 "
                   "bar0_product_surface=1 map_access=1 "
                   "product_hosts=rtl8168_udx,xhci_udx "
                   "soft=1 Soft!=product layout_ver=%u wave=%u\n",
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot write*_flush / posted_flush residual (ISR/mask pattern).
 * Sparse; no stamp storm. Soft!=product.
 */
static void
mmio_soft_log_write_flush_residual_once(void)
{
    if (g_fMmioWriteFlushResidualOnce != 0) {
        return;
    }
    g_fMmioWriteFlushResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualWriteFlush);
    /* greppable: udx: mmio soft residual write_flush */
    mmio_soft_emit("udx: mmio soft residual write_flush "
                   "writel_flush=1 writew_flush=1 writeb_flush=1 "
                   "writeq_flush=1 posted_flush=1 fence=mmio_flush "
                   "product_hosts=rtl8168_udx,xhci_udx "
                   "product_bridge_cfg_flush=0 "
                   "soft=1 Soft!=product layout_ver=%u wave=%u\n",
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot map query residual (observation only).
 * Sparse; no stamp storm. Soft!=product.
 */
static void
mmio_soft_log_query_residual_once(void)
{
    if (g_fMmioQueryResidualOnce != 0) {
        return;
    }
    g_fMmioQueryResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualQuery);
    /* greppable: udx: mmio soft residual query */
    mmio_soft_emit("udx: mmio soft residual query "
                   "is_mapped=1 len=1 phys=1 va=1 "
                   "layout_ver_q=1 shape_ok=1 observation_only=1 "
                   "product_MMIO_FRAME_cap_handle=0 "
                   "soft=1 Soft!=product layout_ver=%u wave=%u\n",
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/** Soft: one-shot UC map honesty residual. */
static void
mmio_soft_log_uc_map_residual_once(void)
{
    if (g_fMmioUcMapResidualOnce != 0) {
        return;
    }
    g_fMmioUcMapResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualUcMap);
    /* greppable: udx: mmio soft residual uc_map */
    mmio_soft_emit("udx: mmio soft residual uc_map honesty "
                   "uc_intent=1 udx_reattr=0 pcd_pwt_intent=1 "
                   "user_device=vmm_map_user_device "
                   "map_device_uc=vmm_map_device_uc "
                   "product_MMIO_FRAME_cap=OPEN soft=1 "
                   "layout_ver=%u wave=%u\n",
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/** Soft: one-shot freestanding thrash refuse residual. */
static void
mmio_soft_log_thrash_residual_once(void)
{
    if (g_fMmioThrashResidualOnce != 0) {
        return;
    }
    g_fMmioThrashResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualThrash);
    /* greppable: udx: mmio soft residual freestanding thrash refuse */
    mmio_soft_emit("udx: mmio soft residual freestanding thrash refuse "
                   "pool_expand=0 class_reenable=0 "
                   "kernel_rtl8168=SKIP kernel_xhci_msc=SKIP "
                   "product=UDX+DDI+hot_cold_ABI "
                   "hosts=rtl8168_udx,xhci_udx "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "soft=1 G-AC-1 Soft!=product "
                   "layout_ver=%u wave=%u\n",
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot MAP_BAR->window->ioremap chain residual (C2 Dual DoD seed).
 * Sparse; no stamp storm. agent!=close.
 */
static void
mmio_soft_log_map_chain_residual_once(void)
{
    if (g_fMmioMapChainResidualOnce != 0) {
        return;
    }
    g_fMmioMapChainResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualMapChain);
    /* greppable: udx: mmio soft residual map_chain */
    /* greppable: Soft!=product dual_dod OPEN product_hosts=UDX */
    mmio_soft_emit("udx: mmio soft residual map_chain "
                   "chain=MAP_BAR,window_register,ioremap "
                   "via=host_window_lookup "
                   "ioremap_va_dense=%u cookie_va_prove=%u "
                   "map_bar_va_path=%u "
                   "product_MMIO_FRAME_cap_mint=OPEN cap_mint=0 "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx,xhci_udx "
                   "product=UDX+DDI+hot_cold_ABI "
                   "soft=1 Soft!=product dual_dod OPEN product_hosts=UDX "
                   "layout_ver=%u wave=%u\n",
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_MAP_BAR_VA_PATH,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot product-host MMIO residual (Dual DoD A/B seed).
 * Sparse; no stamp storm. agent!=close.
 */
static void
mmio_soft_log_prod_host_residual_once(void)
{
    if (g_fMmioProdHostResidualOnce != 0) {
        return;
    }
    g_fMmioProdHostResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualProdHost);
    /* greppable: udx: mmio soft residual product_host */
    /* greppable: Soft!=product dual_dod OPEN product_hosts=UDX */
    mmio_soft_emit("udx: mmio soft residual product_host "
                   "product_hosts=UDX "
                   "hosts=rtl8168_udx,xhci_udx "
                   "surface=ioremap|accessors|write_flush|query|"
                   "ioremap_va_dense "
                   "rtl8168_udx=DoD_B_OPEN xhci_udx=DoD_A_OPEN "
                   "ioremap_va_dense=%u map_bar_va_path=%u "
                   "product_MMIO_FRAME_cap=OPEN cap_mint=0 "
                   "agent_close=0 soft=1 Soft!=product "
                   "dual_dod OPEN product_hosts=UDX "
                   "layout_ver=%u wave=%u\n",
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_MAP_BAR_VA_PATH,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot Dual DoD A/B OPEN residual honesty.
 * Sparse; no stamp storm. Soft lamps alone != close.
 */
static void
mmio_soft_log_dual_dod_residual_once(void)
{
    if (g_fMmioDualDodResidualOnce != 0) {
        return;
    }
    g_fMmioDualDodResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualDualDod);
    /* greppable: udx: mmio soft residual dual_dod */
    /* greppable: Soft!=product dual_dod OPEN product_hosts=UDX */
    mmio_soft_emit("udx: mmio soft residual dual_dod "
                   "DoD_A=OPEN DoD_B=OPEN agent_close=0 "
                   "A=xhci_udx B=rtl8168_udx soft_lamp_close=0 "
                   "ioremap_va_dense=%u map_bar_va_path=%u "
                   "product_hosts=UDX "
                   "product_MMIO_FRAME_cap=OPEN G-AC-1=1 "
                   "soft=1 Soft!=product dual_dod OPEN product_hosts=UDX "
                   "layout_ver=%u wave=%u\n",
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_MAP_BAR_VA_PATH,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot mint residual honesty (product MMIO_FRAME cap mint OPEN).
 * Sparse; no stamp storm. Soft!=product.
 */
static void
mmio_soft_log_mint_residual_once(void)
{
    if (g_fMmioMintResidualOnce != 0) {
        return;
    }
    g_fMmioMintResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualMint);
    /* greppable: udx: mmio soft residual mint */
    mmio_soft_emit("udx: mmio soft residual mint "
                   "product_MMIO_FRAME_cap_mint=OPEN "
                   "cap_mint=%u multi_server=%u confine=%u "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "product_hosts=rtl8168_udx,xhci_udx "
                   "soft=1 Soft!=product layout_ver=%u wave=%u\n",
                   UDX_MMIO_SOFT_CAP_MINT, UDX_MMIO_SOFT_MULTI_SERVER,
                   UDX_MMIO_SOFT_CONFINE,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot product BAR0 map/access residual.
 * Sparse; no stamp storm. Toward Dual DoD A/B BAR0 surface.
 */
static void
mmio_soft_log_bar0_residual_once(void)
{
    if (g_fMmioBar0ResidualOnce != 0) {
        return;
    }
    g_fMmioBar0ResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualBar0);
    /* greppable: udx: mmio soft residual bar0 */
    mmio_soft_emit("udx: mmio soft residual bar0 "
                   "bar_idx=0 ddi_grant_bar0=1 "
                   "rtl_pref_bar0=%u xhci_pref_bar0=%u "
                   "map=udx_ioremap access=readl/writel "
                   "fail_closed_phys0=1 access_ready=%u "
                   "product_MMIO_FRAME_cap=OPEN cap_mint=%u "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "soft=1 Soft!=product layout_ver=%u wave=%u\n",
                   (u32)UDX_MMIO_RTL_PREF_BAR0, (u32)UDX_MMIO_XHCI_PREF_BAR0,
                   g_u32MmioAccessReady, UDX_MMIO_SOFT_CAP_MINT,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot functional map+access residual toward product BAR0.
 * Sparse; no stamp storm. Soft!=product denser path only.
 */
static void
mmio_soft_log_functional_residual_once(void)
{
    if (g_fMmioFunctionalResidualOnce != 0) {
        return;
    }
    g_fMmioFunctionalResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualFunctional);
    /* greppable: udx: mmio soft residual functional */
    /* greppable: udx: mmio soft residual map_access */
    /* greppable: Soft!=product dual_dod OPEN product_hosts=UDX */
    mmio_soft_emit("udx: mmio soft residual functional "
                   "steps=map,access bar0_product=1 "
                   "map=udx_ioremap_via_window "
                   "access=header_inlines_nat_align "
                   "fail_closed_phys0=1 access_ready=%u "
                   "ioremap_va_dense=%u cookie_va_prove=%u "
                   "access_dense=%u bar0_dense=%u denser_prove=%u "
                   "product_hosts=UDX "
                   "product_MMIO_FRAME_cap=OPEN "
                   "soft=1 Soft!=product dual_dod OPEN product_hosts=UDX "
                   "layout_ver=%u wave=%u lean_checks=%u\n",
                   g_u32MmioAccessReady,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_ACCESS_DENSE, UDX_MMIO_BAR0_DENSE,
                   UDX_MMIO_DENSER_PROVE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE,
                   UDX_MMIO_LEAN_CHECKS);
    mmio_soft_emit("udx: mmio soft residual map_access "
                   "map=1 access=1 bar0=1 access_ready=%u "
                   "va_ok=%u cookie_va_prove=%u "
                   "range_gate=1 nat_align_gate=1 "
                   "access_dense=%u denser_prove=%u "
                   "product_hosts=UDX "
                   "soft=1 Soft!=product dual_dod OPEN product_hosts=UDX "
                   "layout_ver=%u wave=%u\n",
                   g_u32MmioAccessReady,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   UDX_MMIO_ACCESS_DENSE, UDX_MMIO_DENSER_PROVE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE);
}

/**
 * Soft: one-shot denser residual prove rollup (product Dual DoD).
 * Sparse; no stamp storm. Soft!=product; Dual DoD A/B OPEN; agent!=close.
 * greppable: udx: mmio soft residual denser
 * greppable: udx: mmio soft residual denser prove
 * greppable: denser_prove=1 | map_chain_dense=1 | bar0_dense=1
 * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
 */
static void
mmio_soft_log_denser_residual_once(void)
{
    if (g_fMmioDenserResidualOnce != 0) {
        return;
    }
    g_fMmioDenserResidualOnce = 1;
    mmio_soft_inc(&g_u32MmioResidualDenser);
    /* greppable: udx: mmio soft residual denser */
    /* greppable: Soft!=product dual_dod OPEN product_hosts=UDX */
    mmio_soft_emit("udx: mmio soft residual denser "
                   "denser_prove=%u map_chain_dense=%u bar0_dense=%u "
                   "access_dense=%u product_host_dense=%u dual_dod_dense=%u "
                   "ioremap_va_dense=%u cookie_va_prove=%u "
                   "va_ok=%u cookie_va_n=%u access_ready=%u map_ok=%u "
                   "product_hosts=UDX hosts=rtl8168_udx,xhci_udx "
                   "DoD_A=%u DoD_B=%u agent_close=%u "
                   "soft=1 Soft!=product dual_dod OPEN product_hosts=UDX "
                   "stamp_free=1 bar=v2026.08.04.75 never_invent=.76 "
                   "layout_ver=%u wave=%u lean_checks=%u\n",
                   UDX_MMIO_DENSER_PROVE, UDX_MMIO_MAP_CHAIN_DENSE,
                   UDX_MMIO_BAR0_DENSE, UDX_MMIO_ACCESS_DENSE,
                   UDX_MMIO_PRODUCT_HOST_DENSE, UDX_MMIO_DUAL_DOD_DENSE,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   g_u32MmioVaOk, g_u32MmioCookieVaProve,
                   g_u32MmioAccessReady, g_u32MmioMapOk,
                   UDX_MMIO_SOFT_DOD_A_OPEN, UDX_MMIO_SOFT_DOD_B_OPEN,
                   UDX_MMIO_SOFT_AGENT_CLOSE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE,
                   UDX_MMIO_LEAN_CHECKS);
    /* greppable: udx: mmio soft residual denser prove */
    mmio_soft_emit("udx: mmio soft residual denser prove "
                   "ok=%u denser_prove=%u "
                   "map_chain_dense=%u bar0_dense=%u access_dense=%u "
                   "product_host_dense=%u dual_dod_dense=%u "
                   "ioremap_va_dense=%u cookie_va_prove=%u "
                   "product_hosts=UDX dual_dod=OPEN "
                   "soft=1 Soft!=product dual_dod OPEN product_hosts=UDX "
                   "layout_ver=%u wave=%u lean_checks=%u\n",
                   (UDX_MMIO_DENSER_PROVE != 0u &&
                    UDX_MMIO_MAP_CHAIN_DENSE != 0u &&
                    UDX_MMIO_BAR0_DENSE != 0u &&
                    UDX_MMIO_ACCESS_DENSE != 0u &&
                    UDX_MMIO_PRODUCT_HOST_DENSE != 0u &&
                    UDX_MMIO_DUAL_DOD_DENSE != 0u &&
                    UDX_MMIO_IOREMAP_VA_DENSE != 0u &&
                    UDX_MMIO_COOKIE_VA_PROVE != 0u) ? 1u : 0u,
                   UDX_MMIO_DENSER_PROVE, UDX_MMIO_MAP_CHAIN_DENSE,
                   UDX_MMIO_BAR0_DENSE, UDX_MMIO_ACCESS_DENSE,
                   UDX_MMIO_PRODUCT_HOST_DENSE, UDX_MMIO_DUAL_DOD_DENSE,
                   UDX_MMIO_IOREMAP_VA_DENSE, UDX_MMIO_COOKIE_VA_PROVE,
                   UDX_MMIO_LAYOUT_VER, UDX_MMIO_SOFT_WAVE,
                   UDX_MMIO_LEAN_CHECKS);
}

/** Soft: one-shot inventory after first successful ioremap. */
static void
mmio_soft_maybe_once(void)
{
    if (g_fMmioSoftOnce != 0) {
        return;
    }
    if (g_u32MmioMapOk == 0) {
        return;
    }
    g_fMmioSoftOnce = 1;
    mmio_soft_log_ioremap_residual_once();
    mmio_soft_log_ioremap_dense_once();
    mmio_soft_log_accessors_residual_once();
    mmio_soft_log_write_flush_residual_once();
    mmio_soft_log_query_residual_once();
    mmio_soft_log_uc_map_residual_once();
    mmio_soft_log_thrash_residual_once();
    mmio_soft_log_map_chain_residual_once();
    mmio_soft_log_prod_host_residual_once();
    mmio_soft_log_dual_dod_residual_once();
    mmio_soft_log_mint_residual_once();
    mmio_soft_log_bar0_residual_once();
    mmio_soft_log_functional_residual_once();
    mmio_soft_log_denser_residual_once();
    mmio_soft_inventory_log();
}

#if !defined(UDX_HOST_LIBC)
/** Soft: raise peak if u32Val is higher (diagnostics only; freestanding pool). */
static void
mmio_soft_note_peak(u32 *pu32Peak, u32 u32Val)
{
    if (pu32Peak != NULL && u32Val > *pu32Peak) {
        *pu32Peak = u32Val;
    }
}

static struct udx_iomem *
fs_iomem_alloc(void)
{
    u32 iSlot;

    for (iSlot = 0; iSlot < UDX_FS_IOMEM_SLOTS; iSlot++) {
        if (!g_aFsIomemUsed[iSlot]) {
            g_aFsIomemUsed[iSlot] = 1;
            g_aFsIomem[iSlot].pVa = NULL;
            g_aFsIomem[iSlot].u64Len = 0;
            g_aFsIomem[iSlot].u64Phys = 0;
            mmio_soft_inc(&g_u32MmioPoolLive);
            mmio_soft_note_peak(&g_u32MmioPoolPeak, g_u32MmioPoolLive);
            return &g_aFsIomem[iSlot];
        }
    }
    /* Fixed pool only — thrash refuse: no expand / no storm. */
    mmio_soft_inc(&g_u32MmioPoolFull);
    mmio_soft_log_thrash_residual_once();
    return NULL;
}

static void
fs_iomem_free(struct udx_iomem *pIo)
{
    u32 iSlot;

    if (pIo == NULL) {
        return;
    }
    for (iSlot = 0; iSlot < UDX_FS_IOMEM_SLOTS; iSlot++) {
        if (&g_aFsIomem[iSlot] == pIo) {
            g_aFsIomemUsed[iSlot] = 0;
            g_aFsIomem[iSlot].pVa = NULL;
            g_aFsIomem[iSlot].u64Len = 0;
            g_aFsIomem[iSlot].u64Phys = 0;
            if (g_u32MmioPoolLive > 0u) {
                g_u32MmioPoolLive--;
            }
            return;
        }
    }
}
#endif

struct udx_iomem *
udx_ioremap(u64 u64Phys, u64 u64Len)
{
    struct udx_iomem *pIo;
    void *pVa;

    mmio_soft_inc(&g_u32MmioMapEnter);
    if (u64Len == 0 || u64Phys + u64Len < u64Phys) {
        mmio_soft_inc(&g_u32MmioMapInval);
        return NULL;
    }
    /*
     * Functional residual toward product BAR0: refuse phys==0.
     * Real MEM BAR0 (xhci cap map / rtl DDI grant) never lands at PA 0;
     * fail-closed avoids bogus cookies for accessors.
     * greppable: udx: mmio soft residual bar0
     * greppable: udx: mmio soft residual functional
     */
    if (u64Phys == 0) {
        mmio_soft_inc(&g_u32MmioMapInval);
        mmio_soft_inc(&g_u32MmioMapZeroPhys);
        return NULL;
    }

    /*
     * Soft residual map: lookup returns VA already advanced by any
     * sub-window offset. Host UC map honesty: VA is a granted window
     * (vmm_map_user_device / vmm_map_device_uc PCD|PWT intent);
     * UDX does not re-attr pages. Product MMIO_FRAME cap mint remains OPEN.
     * Chain residual (C2): MAP_BAR grant -> window_register -> this lookup.
     * Dual DoD A/B OPEN; agent!=close. Soft!=product.
     * Product BAR0 surface: xhci BAR0 / rtl BAR0 DDI grant (or BAR2 pref).
     * greppable: udx: mmio soft residual ioremap
     * greppable: udx: mmio soft residual uc_map
     * greppable: udx: mmio soft residual map_chain
     * greppable: udx: mmio soft residual bar0
     * greppable: udx: mmio soft residual map_access
     */
    /*
     * STRONGER densify: IOREMAP/VA used by freestanding product hosts.
     * MAP_BAR grant VA already registered via window_register; lookup
     * returns host UC-intent VA for cookie pVa. Soft!=product.
     * greppable: udx: mmio soft residual ioremap densify
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    pVa = udx_host_window_lookup(u64Phys, u64Len, NULL);
    if (pVa == NULL) {
        mmio_soft_inc(&g_u32MmioMapLookup);
        udx_printk("udx: ioremap fail phys=%llx len=%llx\n",
                   (unsigned long long)u64Phys,
                   (unsigned long long)u64Len);
        return NULL;
    }
    /* densify: window_lookup VA non-null prove (product_hosts=UDX). */
    mmio_soft_inc(&g_u32MmioVaOk);

#if defined(UDX_HOST_LIBC)
    pIo = (struct udx_iomem *)malloc(sizeof(*pIo));
    if (pIo == NULL) {
        mmio_soft_inc(&g_u32MmioMapNomem);
        return NULL;
    }
#else
    /*
     * Freestanding thrash refuse: fixed UDX_FS_IOMEM_SLOTS pool only —
     * no expand, no class re-enable, no pool storm.
     * Freestanding product hosts=UDX consume this pool for IOREMAP/VA
     * bookkeeping residual (not MMIO_FRAME cap mint).
     * greppable: udx: mmio soft residual freestanding thrash refuse
     * greppable: udx: mmio soft residual ioremap densify
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    pIo = fs_iomem_alloc();
    if (pIo == NULL) {
        mmio_soft_inc(&g_u32MmioMapNomem);
        udx_printk("udx: ioremap freestanding pool full\n");
        return NULL;
    }
#endif
    /*
     * densify: cookie pVa prove = host_window_lookup VA (MAP_BAR path).
     * greppable: cookie_va_prove=1 | ioremap_va_dense=1
     */
    pIo->pVa = pVa;
    pIo->u64Len = u64Len;
    pIo->u64Phys = u64Phys;
    if (pIo->pVa == pVa && pVa != NULL) {
        mmio_soft_inc(&g_u32MmioCookieVaProve);
    }
    /*
     * Functional residual: access-ready cookie for product BAR0 surface.
     * Matches udx_mmio_is_mapped shape (non-null VA + non-zero len).
     * greppable: udx: mmio soft residual functional
     * greppable: udx: mmio soft residual map_access
     * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
     */
    if (pIo->pVa != NULL && pIo->u64Len != 0) {
        mmio_soft_inc(&g_u32MmioAccessReady);
    } else {
        /* Fail-closed: never hand a non-access-ready cookie to hosts. */
#if defined(UDX_HOST_LIBC)
        free(pIo);
#else
        fs_iomem_free(pIo);
#endif
        mmio_soft_inc(&g_u32MmioMapInval);
        return NULL;
    }
    g_u32MmioLastLenLo = (u32)(u64Len & 0xffffffffull);
    g_u32MmioLastLenHi = (u32)(u64Len >> 32);
    g_u32MmioLastPhysLo = (u32)(u64Phys & 0xffffffffull);
    g_u32MmioLastPhysHi = (u32)(u64Phys >> 32);
    mmio_soft_inc(&g_u32MmioMapOk);
    mmio_soft_maybe_once();
    return pIo;
}

void
udx_iounmap(struct udx_iomem *pIo)
{
    mmio_soft_inc(&g_u32MmioUnmap);
    if (pIo == NULL) {
        mmio_soft_inc(&g_u32MmioUnmapNull);
        return;
    }
    /*
     * Soft residual unmap: clear bookkeeping before free / pool return.
     * != product Phase-A revoke / MMIO_FRAME cap destroy.
     * greppable: udx: mmio soft residual iounmap
     */
    pIo->pVa = NULL;
    pIo->u64Len = 0;
    pIo->u64Phys = 0;
    mmio_soft_log_iounmap_residual_once();
#if defined(UDX_HOST_LIBC)
    free(pIo);
#else
    fs_iomem_free(pIo);
#endif
}

/* Soft deepen surfaces (CREATE-ONLY soft != product):
 *   greppable: soft retgradientangle continuum_toward=26800 soft_ne_product=1 wave=126
 *   greppable: soft retblendangle exclusive=1 continuum_toward=26800 soft_ne_product=1 wave=126
 * Soft residual deepen C2: ioremap/iounmap + accessors + write_flush +
 * query + UC map honesty; freestanding thrash refuse;
 * map_chain MAP_BAR->window->ioremap; product_host + dual_dod residual;
 * mint + bar0 + functional map/access toward product BAR0;
 * STRONGER densify for IOREMAP/VA used by freestanding product hosts
 * (product_hosts=UDX; cookie pVa = window_lookup VA);
 * denser residual prove (map_chain/bar0/access/product_host/dual_dod);
 * Dual DoD A/B OPEN (agent!=close); Soft != product complete;
 * product lamps 0; Dual MIT OR Apache-2.0; G-AC-1;
 * product hosts = rtl8168_udx (DoD B) / xhci_udx (DoD A) over hot+cold ABI.
 * layout_ver from UDX_MMIO_LAYOUT_VER (header; not GJ_IMAGE_VERSION).
 * stamp-free residual bar v2026.08.04.75; wave=75; NEVER invent .76.
 * greppable: udx: mmio soft residual map_chain
 * greppable: udx: mmio soft residual write_flush
 * greppable: udx: mmio soft residual query
 * greppable: udx: mmio soft residual product_host
 * greppable: udx: mmio soft residual dual_dod
 * greppable: udx: mmio soft residual mint
 * greppable: udx: mmio soft residual bar0
 * greppable: udx: mmio soft residual functional
 * greppable: udx: mmio soft residual map_access
 * greppable: udx: mmio soft residual ioremap densify
 * greppable: udx: mmio soft residual denser
 * greppable: udx: mmio soft residual denser prove
 * greppable: Soft!=product dual_dod OPEN product_hosts=UDX
 * greppable: ioremap_va_dense=1 | cookie_va_prove=1 | map_bar_va_path=1
 * greppable: denser_prove=1 | map_chain_dense=1 | bar0_dense=1
 * greppable: access_dense=1 | product_host_dense=1 | dual_dod_dense=1
 * greppable: udx: mmio soft residual lean layout_ver=
 * greppable: udx: mmio soft product host
 * greppable: udx: mmio soft residual lean UDX eng
 * greppable: stamp_free=1 bar=v2026.08.04.75 never_invent=.76
 */
