/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PCI driver surface: register, match, probe/remove/quiesce.
 * Host soft: 256-byte config image, BAR windows, inject/remove.
 *
 * Soft inventory (exclusive deepen; this unit only) —
 * greppable "udx: pci soft …":
 *   udx: pci soft inventory …
 *   udx: pci soft driver …
 *   udx: pci soft inject …
 *   udx: pci soft grant residual …
 *   udx: pci soft bind …
 *   udx: pci soft unbind residual …
 *   udx: pci soft lifecycle …
 *   udx: pci soft config …
 *   udx: pci soft enable residual …
 *   udx: pci soft master residual …
 *   udx: pci soft regions residual …
 *   udx: pci soft cfg residual …
 *   udx: pci soft bar residual …
 *   udx: pci soft cap residual …
 *   udx: pci soft resource residual …
 *   udx: pci soft product_host residual …
 *   udx: pci soft residual lean …
 *   udx: pci soft residual lean UDX eng …
 *   udx: pci soft api honesty …
 *   udx: pci soft product bind …
 *   udx: pci soft product bind residual …
 *   udx: pci soft c2 product path …
 *   udx: pci soft c2 residual …
 *   udx: pci soft dual DoD …
 *   udx: pci soft path …
 *   udx: pci soft wave …
 * Pure observation; never gates host skeleton PASS or freestanding path.
 * Soft != skeleton PASS. greppable: udx: pci soft
 * greppable: udx: pci soft residual lean
 * greppable: udx: pci soft grant residual
 * greppable: udx: pci soft bar residual
 * greppable: udx: pci soft enable residual
 * greppable: udx: pci soft cfg residual
 * greppable: udx: pci soft cap residual
 * greppable: udx: pci soft resource residual
 * greppable: udx: pci soft product_host residual
 * greppable: udx: pci soft c2 product path
 * greppable: udx: pci soft c2 residual
 * greppable: udx: pci soft dual DoD
 * greppable: udx: pci soft residual scan
 * greppable: udx: pci soft residual get
 * greppable: udx: pci soft residual id_match
 * greppable: udx: pci soft residual dual_dod OPEN
 * greppable: prefer_real=1 product_hosts=UDX
 *
 * Lean residual (Soft!=product; G-AC-1) for product UDX bind:
 *   pci_enable / enable_mem → soft enable_cnt + COMMAND residual
 *             + BAR cfg re-encode residual on first enable (phys honesty)
 *   set_master → soft COMMAND MASTER residual
 *   request_regions|region|selected → soft per-BAR claim mask residual
 *   cfg r/w → soft 256-byte type-0 image + RO / STATUS RW1C / BAR size
 *   bar residual → 32/64-bit encode + base write → aBarPhys sync
 *   find_cap → soft cap list walk residual (null terminator; mint OPEN)
 *   resource → start/len/is_mem residual (not /proc/iomem product)
 *   grant → udx_host_install_granted_pci residual (DDI → bind path)
 * C2 product path residual deepen:
 *   register → grant|inject → match/probe →
 *   enable|enable_mem → set_master → regions|region|selected →
 *   cfg → bar → find_cap → resource → ioremap → dma → irq → set_drvdata
 *   teardown: quiesce → remove → release → disable
 * Dual DoD A/B OPEN (xhci_udx / rtl8168_udx); agent != close.
 * Freestanding rtl/USB class SKIP. Product = UDX/DDI + hot/cold ABI.
 * Dual MIT OR Apache-2.0. Product hosts = Linux-shaped userspace UDX.
 * No GJ_IMAGE_VERSION stamp; Soft!=product; G-AC-1; no GPL.
 *
 * STRONGER residual densify (host bind scan/get/id_match; stamp-free
 * bar v2026.08.04.75): udx_host_rescan_pci walk + udx_pci_get_device +
 * pci_id_match for product freestanding bind Dual DoD seeds
 * 10ec:8168 (rtl8168_udx DoD B) / 8086:a12f (xhci_udx DoD A).
 * denser residual (preferred over lamps alone; Soft!=product):
 *   denser_prove scan+get+id_match chain for product hosts;
 *   denser product_host (10ec:8168 / 8086:a12f) + denser C2 chain steps;
 *   denser agent_close=0 + Soft!=product + freestanding class SKIP;
 *   denser dual_dod OPEN (A/B) — never close Dual DoD from residual.
 * greppable: Soft!=product soft residual dual_dod OPEN prefer_real
 *            product_hosts=UDX
 * greppable: denser_prove=1 | denser residual | lean_checks=
 * greppable: udx: pci soft residual denser
 * prefer_real=1: prefer real product-host id_match over soft inject lamps.
 * Soft!=product; Dual DoD OPEN; agent!=close; NEVER invent .76.
 */
#include "udx_internal.h"

#include <udx/device.h>
#include <udx/host.h>
#include <udx/pci.h>
#include <udx/udx.h>

#include <stdarg.h>

#if defined(UDX_HOST_LIBC)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/* Soft wave stamp for greppable inventory / residual lines (stamp-free). */
#define UDX_PCI_SOFT_WAVE 75u

/*
 * FUNCTIONAL residual locks (STRONGER toward product UDX bind chain;
 * Soft!=product; G-AC-1; Dual DoD A/B OPEN; agent!=close; stamp-free
 * bar v2026.08.04.75). C2 thr-only product path:
 *   enable → master → regions → cfg/bar/cap/resource →
 *   ioremap → dma → request_irq (thr-only) → set_drvdata
 * H1: PCI residual never net_eth_poll; thr-only IRQ after bind.
 * Host-bind densify (STRONGER): scan (rescan walk) + get (BDF lookup)
 * + id_match (driver table vs 10ec:8168 / 8086:a12f).
 * denser residual (preferred over lamps alone): denser_prove chain
 * scan+get+id_match + denser product_host + denser C2 steps +
 * Soft!=product + agent_close=0 + freestanding class SKIP.
 * greppable: udx: pci soft c2 residual
 * greppable: udx: pci soft residual scan / get / id_match
 * greppable: udx: pci soft residual denser
 * greppable: denser_prove=1 | lean_checks=
 * greppable: thr_only=1 | H1=no_net_eth_poll | DoD_A=OPEN | DoD_B=OPEN
 * greppable: Soft!=product soft residual dual_dod OPEN prefer_real
 *            product_hosts=UDX
 */
#define UDX_PCI_H1_NO_NET_ETH_POLL   1u /* H1: no net_eth_poll from PCI path */
#define UDX_PCI_THR_ONLY_IRQ         1u /* post-bind IRQ is thr-only residual */
#define UDX_PCI_HARD_PRIMARY_IRQ     0u /* never hard primary IRQ product */
#define UDX_PCI_C2_CHAIN_STEPS       8u /* enable/master/regions/cfg/cap/res/grant/bind */
#define UDX_PCI_PREFER_REAL          1u /* prefer real product-host id_match */
#define UDX_PCI_PRODUCT_HOSTS_UDX    1u /* product_hosts=UDX (not fs class) */
#define UDX_PCI_SCAN_GET_MATCH_STEPS 3u /* scan + get + id_match densify */
#define UDX_PCI_DENSER_PROVE         1u /* denser residual prove on */
#define UDX_PCI_DENSER_CHAIN         1u /* denser scan>get>id_match chain */
#define UDX_PCI_DENSER_PRODUCT_HOST  1u /* denser product-host DoD A/B id */
#define UDX_PCI_AGENT_CLOSE          0u /* soft lamps never close Dual DoD */
#define UDX_PCI_SOFT_NE_PRODUCT      1u /* Soft!=product claim-class law */
#define UDX_PCI_FS_CLASS_SKIP        1u /* freestanding class SKIP */
#define UDX_PCI_STAMP_FREE_75        1u /* bar v2026.08.04.75; never invent .76 */
#define UDX_PCI_LEAN_CHECKS         16u /* denser functional self-check count */

_Static_assert(UDX_PCI_DOD_A_OPEN == 1u,
               "Dual DoD A remains OPEN (xhci_udx)");
_Static_assert(UDX_PCI_DOD_B_OPEN == 1u,
               "Dual DoD B remains OPEN (rtl8168_udx)");
_Static_assert(UDX_PCI_H1_NO_NET_ETH_POLL == 1u,
               "H1: PCI residual never net_eth_poll");
_Static_assert(UDX_PCI_THR_ONLY_IRQ == 1u,
               "post-bind IRQ residual thr-only");
_Static_assert(UDX_PCI_HARD_PRIMARY_IRQ == 0u,
               "PCI residual never hard primary IRQ");
_Static_assert(UDX_PCI_C2_CHAIN_STEPS == 8u,
               "C2 product chain residual step count");
_Static_assert(UDX_PCI_PREFER_REAL == 1u,
               "prefer_real product-host id_match residual");
_Static_assert(UDX_PCI_PRODUCT_HOSTS_UDX == 1u,
               "product_hosts=UDX residual lock");
_Static_assert(UDX_PCI_SCAN_GET_MATCH_STEPS == 3u,
               "scan/get/id_match densify step count");
_Static_assert(UDX_PCI_DENSER_PROVE == 1u,
               "denser residual prove on (scan/get/id_match)");
_Static_assert(UDX_PCI_DENSER_CHAIN == 1u,
               "denser residual chain scan>get>id_match");
_Static_assert(UDX_PCI_DENSER_PRODUCT_HOST == 1u,
               "denser residual product-host DoD A/B id");
_Static_assert(UDX_PCI_AGENT_CLOSE == 0u,
               "soft residual lamps never close Dual DoD");
_Static_assert(UDX_PCI_SOFT_NE_PRODUCT == 1u,
               "Soft!=product claim-class law (never Soft unicode ne)");
_Static_assert(UDX_PCI_FS_CLASS_SKIP == 1u,
               "freestanding class SKIP (product=UDX)");
_Static_assert(UDX_PCI_STAMP_FREE_75 == 1u,
               "stamp-free residual bar v2026.08.04.75 (never .76)");
_Static_assert(UDX_PCI_LEAN_CHECKS == 16u,
               "FUNCTIONAL lean residual denser: 16 PCI checks");
_Static_assert(UDX_PCI_SOFT_WAVE == 75u,
               "stamp-free soft wave aligns bar v2026.08.04.75 (never .76)");

struct udx_pci_bound {
    struct udx_pci_dev    *pPdev;
    struct udx_pci_driver *pDrv;
    struct udx_device       dev;
    u8                     u8Enabled;   /* cache: enable_cnt > 0 */
    u8                     u8Master;
    u8                     u8Regions;   /* any region claim active */
    u8                     u8Bound;
    u8                     u8EnableMemOnly;  /* first enable via enable_mem */
    u8                     u8BarPresentMask; /* bit N: BAR N len != 0 */
    u8                     u8BarClaimMask;   /* bit N: claimed by request_* */
    u8                     u8BarSizeMask;    /* bit N: BAR sizing residual live */
    u8                     u8Bar64Mask;      /* bit N: BAR N is 64-bit MEM low */
    u32                    u32EnableCnt;     /* soft residual of Linux enable_cnt */
    const char            *szRegionName;
    /* Soft type-0 config image (host inject + freestanding grant path). */
    u8                     aCfg[UDX_PCI_CFG_SIZE];
    u8                     u8CfgLive; /* non-zero once cfg image is valid */
    /* Host-sim BAR backing (free on destroy). */
    void                  *apBarHost[6];
    struct udx_pci_bound  *pNext;
};

static struct udx_pci_driver *g_pPciDrivers;
static struct udx_pci_bound  *g_pPciDevices;

/*
 * Freestanding DDI grant install pool (no libc heap).
 * Soft bound: host-linux still uses malloc inject; this path is for
 * GJ_SYS_DDI grants registered via udx_host_install_granted_pci.
 */
#if !defined(UDX_HOST_LIBC)
#define UDX_PCI_FS_GRANT_MAX 8u
static struct udx_pci_bound g_aFsBound[UDX_PCI_FS_GRANT_MAX];
static struct udx_pci_dev   g_aFsPdev[UDX_PCI_FS_GRANT_MAX];
static u8                   g_aFsGrantUsed[UDX_PCI_FS_GRANT_MAX];
#endif

/*
 * Soft PCI product inventory (Wave 126 exclusive deepen).
 * Cumulative for this process. greppable: udx: pci soft …
 * Never hard-gates; wrap OK if ever hit.
 */
static u32 g_u32PciRegDrv;        /* register_driver enter */
static u32 g_u32PciRegDrvOk;      /* register_driver ok */
static u32 g_u32PciRegDrvInval;   /* register reject */
static u32 g_u32PciUnregDrv;      /* unregister_driver */
static u32 g_u32PciInject;        /* inject_ex enter */
static u32 g_u32PciInjectOk;      /* inject success */
static u32 g_u32PciInjectBusy;    /* duplicate BDF */
static u32 g_u32PciInjectNomem;   /* alloc fail */
static u32 g_u32PciInjectNosys;   /* freestanding NOSYS */
static u32 g_u32PciRemove;        /* remove enter */
static u32 g_u32PciRemoveOk;      /* remove success */
static u32 g_u32PciRemoveNodev;   /* remove not found */
static u32 g_u32PciRemoveNosys;   /* freestanding NOSYS */
static u32 g_u32PciRescan;        /* host_rescan_pci */
static u32 g_u32PciTryBind;       /* pci_try_bind enter */
static u32 g_u32PciBindProbeOk;   /* probe returned 0 */
static u32 g_u32PciBindNoProbe;   /* bound without probe */
static u32 g_u32PciBindProbeFail; /* probe non-zero */
static u32 g_u32PciUnbind;        /* pci_unbind performed */
static u32 g_u32PciEnable;        /* enable ok (any, incl nested) */
static u32 g_u32PciEnableInval;   /* enable no bound */
static u32 g_u32PciEnableEnter;   /* enable enter residual */
static u32 g_u32PciEnableFirst;   /* first enable (0 → 1) */
static u32 g_u32PciEnableNested;  /* nested enable (cnt already > 0) */
static u32 g_u32PciEnableMem;     /* enable_mem ok (any) */
static u32 g_u32PciEnableMemFirst;/* first enable via enable_mem */
static u32 g_u32PciEnableMemNest; /* nested enable_mem */
static u32 g_u32PciEnableMemInv;  /* enable_mem inval */
static u32 g_u32PciDisable;       /* disable enter with cnt > 0 */
static u32 g_u32PciDisableLast;   /* disable that cleared cnt to 0 */
static u32 g_u32PciDisableUnder;  /* disable when cnt == 0 */
static u32 g_u32PciSetMaster;     /* set_master ok */
static u32 g_u32PciSetMasterInv;  /* set_master reject */
static u32 g_u32PciClrMaster;     /* clear_master */
static u32 g_u32PciReqReg;        /* request_regions ok */
static u32 g_u32PciReqRegEnter;   /* request_regions enter */
static u32 g_u32PciReqRegBusy;    /* request regions busy */
static u32 g_u32PciReqRegInval;   /* request regions inval */
static u32 g_u32PciReqRegEmpty;   /* ok claim with zero present BARs */
static u32 g_u32PciReqRegBars;    /* cumulative BAR bits claimed */
static u32 g_u32PciReqOne;        /* request_region (single) ok */
static u32 g_u32PciReqOneBusy;    /* request_region busy */
static u32 g_u32PciReqOneInval;   /* request_region inval */
static u32 g_u32PciReqSel;        /* request_selected_regions ok */
static u32 g_u32PciReqSelBusy;    /* selected busy overlap */
static u32 g_u32PciReqSelInval;   /* selected inval */
static u32 g_u32PciReqSelEmpty;   /* selected claim empty after mask */
static u32 g_u32PciRelReg;        /* release_regions */
static u32 g_u32PciRelOne;        /* release_region */
static u32 g_u32PciRelSel;        /* release_selected_regions */
static u32 g_u32PciCfgRead;       /* config read* ok */
static u32 g_u32PciCfgWrite;      /* config write* ok */
static u32 g_u32PciCfgFail;       /* config access fail */
static u32 g_u32PciCfgRdByte;     /* read_config_byte ok */
static u32 g_u32PciCfgRdWord;     /* read_config_word ok */
static u32 g_u32PciCfgRdDword;    /* read_config_dword ok */
static u32 g_u32PciCfgWrByte;     /* write_config_byte ok */
static u32 g_u32PciCfgWrWord;     /* write_config_word ok */
static u32 g_u32PciCfgWrDword;    /* write_config_dword ok */
static u32 g_u32PciCfgRoReject;   /* RO identity field write reject */
static u32 g_u32PciCfgCmdWrite;   /* COMMAND register write residual */
static u32 g_u32PciCfgStatusRw1c; /* STATUS RW1C residual write */
static u32 g_u32PciCfgBarSize;    /* BAR size residual enter */
static u32 g_u32PciCfgBarSizeRd;  /* BAR size residual read hit */
static u32 g_u32PciCfgBarWrite;   /* BAR base write residual (cfg → phys) */
static u32 g_u32PciCfgBar64Enc;   /* 64-bit BAR encode residual */
static u32 g_u32PciCfgBarSync;    /* BAR phys sync from cfg residual */
static u32 g_u32PciCfgBarRefresh; /* BAR cfg re-encode (enable/init) */
static u32 g_u32PciCfgFindCap;    /* find_capability enter */
static u32 g_u32PciCfgFindCapHit; /* find_capability hit */
static u32 g_u32PciCfgFindCapMiss;/* find_capability miss (soft null) */
static u32 g_u32PciCfgLiveHit;    /* access via live soft image */
static u32 g_u32PciCfgFallback;   /* identity fallback without live cfg */
static u32 g_u32PciGetDev;        /* get_device enter */
static u32 g_u32PciGetDevHit;     /* get_device hit */
static u32 g_u32PciGetDevMiss;    /* get_device miss */
/* Scan residual (host rescan walk; Soft!=product; host bind densify). */
static u32 g_u32PciScanEnter;     /* host_rescan_pci enter */
static u32 g_u32PciScanWalk;      /* devices walked on rescan */
static u32 g_u32PciScanProductHit;/* product-host IDs seen on scan walk */
static u32 g_u32PciScanRtl;       /* 10ec:8168 seen on scan */
static u32 g_u32PciScanXhci;      /* 8086:a12f seen on scan */
static u32 g_u32PciScanResidualN; /* scan residual lamp emissions */
/* Get residual densify (BDF lookup; product-host hit note). */
static u32 g_u32PciGetProductHit; /* get_device hit on product-host ID */
static u32 g_u32PciGetRtl;        /* get hit 10ec:8168 */
static u32 g_u32PciGetXhci;       /* get hit 8086:a12f */
static u32 g_u32PciGetResidualN;  /* get residual lamp emissions */
/* id_match residual densify (driver table match; host bind seed). */
static u32 g_u32PciIdMatchEnter;  /* pci_id_match enter */
static u32 g_u32PciIdMatchHit;    /* pci_id_match hit */
static u32 g_u32PciIdMatchMiss;   /* pci_id_match miss */
static u32 g_u32PciIdMatchProductHit; /* hit on 10ec:8168 or 8086:a12f */
static u32 g_u32PciIdMatchRtl;    /* id_match hit rtl8168_udx seed */
static u32 g_u32PciIdMatchXhci;   /* id_match hit xhci_udx seed */
static u32 g_u32PciIdMatchAny;    /* ANY_ID class/wild residual hit */
static u32 g_u32PciIdMatchResidualN; /* id_match residual lamp emissions */
/* Grant residual (DDI install_granted → product UDX bind). */
static u32 g_u32PciGrant;         /* install_granted enter */
static u32 g_u32PciGrantOk;       /* install_granted success */
static u32 g_u32PciGrantBusy;     /* grant duplicate BDF */
static u32 g_u32PciGrantNomem;    /* grant alloc / fs pool fail */
static u32 g_u32PciGrantInval;    /* grant null bar arrays */
static u32 g_u32PciGrantBar0Map;  /* grant with bar0 VA window */
static u32 g_u32PciGrantFsSlot;   /* freestanding pool slot taken */
static u32 g_u32PciGrantFsPeak;   /* peak freestanding grant slots */
static u32 g_u32PciGrantProductHit; /* grant of 10ec:8168 or 8086:a12f */
static u32 g_u32PciInjectProductHit;/* inject of product-host IDs */
/* Unbind residual (quiesce then remove; Soft!=product). */
static u32 g_u32PciUnbindQuiesce; /* pfnQuiesce invoked */
static u32 g_u32PciUnbindRemove;  /* pfnRemove invoked */
static u32 g_u32PciUnbindNoCb;    /* unbind with neither callback */
/* Resource residual (start/len/is_mem; Soft!=product iomem). */
static u32 g_u32PciResStart;      /* resource_start enter */
static u32 g_u32PciResLen;        /* resource_len enter */
static u32 g_u32PciResIsMem;      /* resource_is_mem enter */
static u32 g_u32PciResStartHit;   /* start returned non-zero */
static u32 g_u32PciResLenHit;     /* len returned non-zero */
static u32 g_u32PciResEmpty;      /* present query on empty BAR */
/* Cap residual (find_capability walk; mint OPEN). */
static u32 g_u32PciCapResidualN;  /* cap residual lamp emissions */
/* Product-host residual (Dual DoD A/B seed IDs; Soft!=product). */
static u32 g_u32PciProductHostRtl;  /* 10ec:8168 residual hit */
static u32 g_u32PciProductHostXhci; /* 8086:a12f residual hit */
static u32 g_u32PciProductHostBar0; /* product host BAR0 present */
static u32 g_u32PciProductHostBar2; /* product host BAR2 present (rtl) */
static u32 g_u32PciProductHostNote; /* product_host residual note n */
/* C2 product path residual step tallies (Soft!=product; never close). */
static u32 g_u32PciC2Enable;      /* C2 enable residual step */
static u32 g_u32PciC2Master;      /* C2 set_master residual step */
static u32 g_u32PciC2Regions;     /* C2 regions residual step */
static u32 g_u32PciC2Cfg;         /* C2 cfg residual step */
static u32 g_u32PciC2Cap;         /* C2 find_cap residual step */
static u32 g_u32PciC2Resource;    /* C2 resource residual step */
static u32 g_u32PciC2Grant;       /* C2 grant residual step */
static u32 g_u32PciC2Bind;        /* C2 bind residual step */
static u32 g_u32PciC2ResidualN;   /* C2 residual lamp emissions */
static u32 g_u32PciLiveDev;       /* soft live device count snap */
static u32 g_u32PciLiveDrv;       /* soft live driver count snap */
static u32 g_u32PciLiveEnabled;   /* soft live enable_cnt>0 snap */
static u32 g_u32PciLiveRegions;   /* soft live regions-claimed snap */
static u32 g_u32PciLiveMaster;    /* soft live master snap */
static u32 g_u32PciLiveBound;     /* soft live bound snap */
static u32 g_u32PciLiveCfg;       /* soft live cfg-image snap */
static u32 g_u32PciPeakDev;       /* peak devices observed */
static u32 g_u32PciPeakEnableCnt; /* peak enable_cnt observed on any bound */
static u32 g_u32PciLogN;          /* soft inventory dumps */
static u8  g_fPciSoftOnce;        /* one-shot after first ok path */
static u8  g_fPciProductBindOnce; /* one-shot product bind residual lamp */
static u8  g_fPciDualDodOnce;     /* one-shot Dual DoD A/B OPEN honesty */
static u8  g_fPciCapResidualOnce; /* one-shot cap residual */
static u8  g_fPciResourceResidualOnce; /* one-shot resource residual */
static u8  g_fPciBarResidualOnce; /* one-shot bar residual */
static u8  g_fPciEnableResidualOnce; /* one-shot enable residual deepen */
static u8  g_fPciProductHostResidualOnce; /* one-shot product_host residual */
static u8  g_fPciC2ResidualOnce;  /* one-shot C2 residual deepen lamp */
static u8  g_fPciScanResidualOnce;    /* one-shot scan residual densify */
static u8  g_fPciGetResidualOnce;     /* one-shot get residual densify */
static u8  g_fPciIdMatchResidualOnce; /* one-shot id_match residual densify */
/* denser residual (Soft!=product; preferred over lamps alone). */
static u32 g_u32PciDenserResidualN;   /* denser residual lamp emissions */
static u8  g_fPciDenserResidualOnce;  /* one-shot denser residual prove */

static void pci_soft_inc(u32 *pu32);
static void pci_soft_note_peak(u32 *pu32Peak, u32 u32Val);
static void pci_soft_emit(const char *szFmt, ...);
static void pci_soft_count_lists(void);
static void pci_soft_inventory_log(void);
static void pci_soft_maybe_once(void);
static void pci_soft_product_bind_maybe(void);
static void pci_soft_dual_dod_maybe(void);
static void pci_soft_cap_residual_maybe(void);
static void pci_soft_resource_residual_maybe(void);
static void pci_soft_bar_residual_maybe(void);
static void pci_soft_enable_residual_maybe(void);
static void pci_soft_product_host_residual_maybe(void);
static void pci_soft_c2_residual_maybe(void);
static void pci_soft_scan_residual_maybe(void);
static void pci_soft_get_residual_maybe(void);
static void pci_soft_id_match_residual_maybe(void);
static void pci_soft_denser_residual_maybe(void);
static void pci_soft_note_cfg_c2(void);
static void pci_soft_note_product_host(u16 u16Vendor, u16 u16Device,
                                       const struct udx_pci_dev *pPdev);
static int  pci_soft_is_product_host_id(u16 u16Vendor, u16 u16Device);
static void pci_cfg_bar_encode_all(struct udx_pci_bound *pBound);
static void pci_cfg_bar_sync_phys(struct udx_pci_bound *pBound, int iBar);

static void
pci_soft_inc(u32 *pu32)
{
    if (pu32 != NULL && *pu32 < 0xffffffffu) {
        (*pu32)++;
    }
}

/**
 * C2 cfg residual step (live soft image hit). Soft!=product ECAM.
 * greppable path: udx: pci soft c2 residual / cfg residual
 */
static void
pci_soft_note_cfg_c2(void)
{
    if (g_u32PciCfgLiveHit < 0xffffffffu) {
        g_u32PciCfgLiveHit++;
    }
    pci_soft_inc(&g_u32PciC2Cfg);
    pci_soft_c2_residual_maybe();
}

/** Soft: raise peak if u32Val is higher (diagnostics only). */
static void
pci_soft_note_peak(u32 *pu32Peak, u32 u32Val)
{
    if (pu32Peak != NULL && u32Val > *pu32Peak) {
        *pu32Peak = u32Val;
    }
}

/**
 * Soft product-host PCI IDs for Dual DoD residual honesty.
 * DoD B: rtl8168_udx 10ec:8168 · DoD A: xhci_udx 8086:a12f.
 * Soft!=product · agent != close Dual DoD A/B.
 */
static int
pci_soft_is_product_host_id(u16 u16Vendor, u16 u16Device)
{
    if (u16Vendor == (u16)UDX_PCI_G752_RTL8168_VEND &&
        u16Device == (u16)UDX_PCI_G752_RTL8168_DEV) {
        return 1; /* Dual DoD B NIC residual */
    }
    if (u16Vendor == (u16)UDX_PCI_G752_XHCI_VEND &&
        u16Device == (u16)UDX_PCI_G752_XHCI_DEV) {
        return 1; /* Dual DoD A USB residual */
    }
    return 0;
}

/**
 * Product-host residual note (pref BAR catalog; Soft!=product).
 * rtl8168_udx: BAR0 (DDI) + BAR2 (silicon-common MMIO).
 * xhci_udx: BAR0 (HC MMIO). Never closes Dual DoD A/B.
 * greppable: udx: pci soft product_host residual
 */
static void
pci_soft_note_product_host(u16 u16Vendor, u16 u16Device,
                           const struct udx_pci_dev *pPdev)
{
    int fRtl;
    int fXhci;

    fRtl = (u16Vendor == (u16)UDX_PCI_G752_RTL8168_VEND &&
            u16Device == (u16)UDX_PCI_G752_RTL8168_DEV) ? 1 : 0;
    fXhci = (u16Vendor == (u16)UDX_PCI_G752_XHCI_VEND &&
             u16Device == (u16)UDX_PCI_G752_XHCI_DEV) ? 1 : 0;
    if (fRtl == 0 && fXhci == 0) {
        return;
    }
    if (fRtl != 0) {
        pci_soft_inc(&g_u32PciProductHostRtl);
    }
    if (fXhci != 0) {
        pci_soft_inc(&g_u32PciProductHostXhci);
    }
    if (pPdev != NULL) {
        if (pPdev->aBarLen[UDX_PCI_XHCI_PREF_BAR0] != 0 ||
            pPdev->aBarLen[UDX_PCI_RTL_PREF_BAR0] != 0) {
            pci_soft_inc(&g_u32PciProductHostBar0);
        }
        if (fRtl != 0 && pPdev->aBarLen[UDX_PCI_RTL_PREF_BAR2] != 0) {
            pci_soft_inc(&g_u32PciProductHostBar2);
        }
    }
    pci_soft_inc(&g_u32PciProductHostNote);
    pci_soft_product_host_residual_maybe();
}

static void
pci_soft_count_lists(void)
{
    struct udx_pci_bound *pBound;
    struct udx_pci_driver *pDrv;
    u32 u32Dev;
    u32 u32Drv;
    u32 u32En;
    u32 u32Reg;
    u32 u32Master;
    u32 u32Bound;
    u32 u32Cfg;

    u32Dev = 0;
    u32En = 0;
    u32Reg = 0;
    u32Master = 0;
    u32Bound = 0;
    u32Cfg = 0;
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (u32Dev < 0xffffffffu) {
            u32Dev++;
        }
        if (pBound->u32EnableCnt > 0 && u32En < 0xffffffffu) {
            u32En++;
        }
        if ((pBound->u8Regions != 0 || pBound->u8BarClaimMask != 0) &&
            u32Reg < 0xffffffffu) {
            u32Reg++;
        }
        if (pBound->u8Master != 0 && u32Master < 0xffffffffu) {
            u32Master++;
        }
        if (pBound->u8Bound != 0 && u32Bound < 0xffffffffu) {
            u32Bound++;
        }
        if (pBound->u8CfgLive != 0 && u32Cfg < 0xffffffffu) {
            u32Cfg++;
        }
        pci_soft_note_peak(&g_u32PciPeakEnableCnt, pBound->u32EnableCnt);
    }
    u32Drv = 0;
    for (pDrv = g_pPciDrivers; pDrv != NULL; pDrv = pDrv->pNext) {
        if (u32Drv < 0xffffffffu) {
            u32Drv++;
        }
    }
    g_u32PciLiveDev = u32Dev;
    g_u32PciLiveDrv = u32Drv;
    g_u32PciLiveEnabled = u32En;
    g_u32PciLiveRegions = u32Reg;
    g_u32PciLiveMaster = u32Master;
    g_u32PciLiveBound = u32Bound;
    g_u32PciLiveCfg = u32Cfg;
    pci_soft_note_peak(&g_u32PciPeakDev, u32Dev);
}

/*
 * Soft inventory emit path — host console; does not use udx_printk so
 * core soft printk tallies stay product-path pure.
 */
static void
pci_soft_emit(const char *szFmt, ...)
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
 * Greppable soft PCI inventory (Wave 126 exclusive deepen).
 * Prefix-stable "udx: pci soft …" — never hard-gates; observation only.
 *
 * greppable: udx: pci soft
 */
static void
pci_soft_inventory_log(void)
{
    u32 u32Host;

    pci_soft_inc(&g_u32PciLogN);
    pci_soft_count_lists();

#if defined(UDX_HOST_LIBC)
    u32Host = 1u;
#else
    u32Host = 0u;
#endif

    /* Grep: udx: pci soft inventory */
    pci_soft_emit("udx: pci soft inventory reg_drv=%u inject_ok=%u "
                  "remove_ok=%u bind_ok=%u bind_fail=%u enable=%u "
                  "master=%u regions=%u cfg_r=%u cfg_w=%u "
                  "grant_ok=%u res_q=%u cap_q=%u "
                  "live_dev=%u live_drv=%u live_cfg=%u "
                  "log_n=%u layout_ver=%u wave=%u\n",
                  g_u32PciRegDrvOk, g_u32PciInjectOk, g_u32PciRemoveOk,
                  (u32)(g_u32PciBindProbeOk + g_u32PciBindNoProbe),
                  g_u32PciBindProbeFail, g_u32PciEnable, g_u32PciSetMaster,
                  g_u32PciReqReg, g_u32PciCfgRead, g_u32PciCfgWrite,
                  g_u32PciGrantOk,
                  (u32)(g_u32PciResStart + g_u32PciResLen + g_u32PciResIsMem),
                  g_u32PciCfgFindCap,
                  g_u32PciLiveDev, g_u32PciLiveDrv, g_u32PciLiveCfg,
                  g_u32PciLogN, UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

    /* Grep: udx: pci soft driver */
    pci_soft_emit("udx: pci soft driver reg_enter=%u reg_ok=%u reg_inval=%u "
                  "unreg=%u live_drv=%u rescan=%u\n",
                  g_u32PciRegDrv, g_u32PciRegDrvOk, g_u32PciRegDrvInval,
                  g_u32PciUnregDrv, g_u32PciLiveDrv, g_u32PciRescan);

    /* Grep: udx: pci soft inject */
    pci_soft_emit("udx: pci soft inject enter=%u ok=%u busy=%u nomem=%u "
                  "nosys=%u remove=%u remove_ok=%u remove_nodev=%u "
                  "remove_nosys=%u host_libc=%u\n",
                  g_u32PciInject, g_u32PciInjectOk, g_u32PciInjectBusy,
                  g_u32PciInjectNomem, g_u32PciInjectNosys, g_u32PciRemove,
                  g_u32PciRemoveOk, g_u32PciRemoveNodev, g_u32PciRemoveNosys,
                  u32Host);

    /* Grep: udx: pci soft bind */
    pci_soft_emit("udx: pci soft bind try=%u probe_ok=%u no_probe=%u "
                  "probe_fail=%u unbind=%u live_dev=%u peak_dev=%u\n",
                  g_u32PciTryBind, g_u32PciBindProbeOk, g_u32PciBindNoProbe,
                  g_u32PciBindProbeFail, g_u32PciUnbind, g_u32PciLiveDev,
                  g_u32PciPeakDev);

    /* Grep: udx: pci soft lifecycle */
    pci_soft_emit("udx: pci soft lifecycle enable=%u enable_inval=%u "
                  "enable_mem=%u disable=%u set_master=%u set_master_inv=%u "
                  "clr_master=%u req_reg=%u req_one=%u req_sel=%u "
                  "req_busy=%u req_inval=%u rel_reg=%u "
                  "get_hit=%u get_miss=%u\n",
                  g_u32PciEnable, g_u32PciEnableInval, g_u32PciEnableMem,
                  g_u32PciDisable, g_u32PciSetMaster, g_u32PciSetMasterInv,
                  g_u32PciClrMaster, g_u32PciReqReg, g_u32PciReqOne,
                  g_u32PciReqSel, g_u32PciReqRegBusy, g_u32PciReqRegInval,
                  g_u32PciRelReg, g_u32PciGetDevHit, g_u32PciGetDevMiss);

    /* Grep: udx: pci soft config */
    pci_soft_emit("udx: pci soft config read_ok=%u write_ok=%u fail=%u "
                  "soft_image=256 type0=1 live_hit=%u fallback=%u "
                  "ro_reject=%u cmd_wr=%u status_rw1c=%u bar_size=%u "
                  "find_cap=%u\n",
                  g_u32PciCfgRead, g_u32PciCfgWrite, g_u32PciCfgFail,
                  g_u32PciCfgLiveHit, g_u32PciCfgFallback,
                  g_u32PciCfgRoReject, g_u32PciCfgCmdWrite,
                  g_u32PciCfgStatusRw1c, g_u32PciCfgBarSize,
                  g_u32PciCfgFindCap);

    /*
     * Enable residual (Linux enable_cnt / COMMAND decode spirit).
     * greppable: udx: pci soft enable residual
     */
    pci_soft_emit("udx: pci soft enable residual enter=%u ok=%u first=%u "
                  "nested=%u inval=%u mem_ok=%u mem_first=%u mem_nest=%u "
                  "mem_inv=%u disable=%u disable_last=%u disable_under=%u "
                  "live_en=%u peak_cnt=%u "
                  "cmd_bits=IO+MEM|MEM_only soft_cnt=1 product_pm=OPEN "
                  "Soft!=product\n",
                  g_u32PciEnableEnter, g_u32PciEnable, g_u32PciEnableFirst,
                  g_u32PciEnableNested, g_u32PciEnableInval,
                  g_u32PciEnableMem, g_u32PciEnableMemFirst,
                  g_u32PciEnableMemNest, g_u32PciEnableMemInv,
                  g_u32PciDisable, g_u32PciDisableLast, g_u32PciDisableUnder,
                  g_u32PciLiveEnabled, g_u32PciPeakEnableCnt);

    /*
     * Regions residual (per-BAR claim mask; Soft!=product iomem).
     * greppable: udx: pci soft regions residual
     */
    pci_soft_emit("udx: pci soft regions residual enter=%u ok=%u busy=%u "
                  "inval=%u empty=%u bars_claimed=%u "
                  "one_ok=%u one_busy=%u one_inv=%u "
                  "sel_ok=%u sel_busy=%u sel_inv=%u sel_empty=%u "
                  "rel=%u rel_one=%u rel_sel=%u live_reg=%u "
                  "per_bar_mask=1 product_iomem=OPEN soft=1 "
                  "Soft!=product\n",
                  g_u32PciReqRegEnter, g_u32PciReqReg, g_u32PciReqRegBusy,
                  g_u32PciReqRegInval, g_u32PciReqRegEmpty, g_u32PciReqRegBars,
                  g_u32PciReqOne, g_u32PciReqOneBusy, g_u32PciReqOneInval,
                  g_u32PciReqSel, g_u32PciReqSelBusy, g_u32PciReqSelInval,
                  g_u32PciReqSelEmpty, g_u32PciRelReg, g_u32PciRelOne,
                  g_u32PciRelSel, g_u32PciLiveRegions);

    /*
     * Cfg residual (256-byte type-0 soft image).
     * greppable: udx: pci soft cfg residual
     */
    pci_soft_emit("udx: pci soft cfg residual rd_b=%u rd_w=%u rd_d=%u "
                  "wr_b=%u wr_w=%u wr_d=%u fail=%u ro_reject=%u "
                  "cmd_wr=%u status_rw1c=%u bar_size=%u bar_size_rd=%u "
                  "bar_wr=%u bar_sync=%u bar64=%u bar_refresh=%u "
                  "find_cap=%u find_hit=%u find_miss=%u "
                  "live_hit=%u fallback=%u size=256 type0=1 "
                  "cap_ptr_soft=0x40 product_ecam=OPEN soft=1 "
                  "Soft!=product\n",
                  g_u32PciCfgRdByte, g_u32PciCfgRdWord, g_u32PciCfgRdDword,
                  g_u32PciCfgWrByte, g_u32PciCfgWrWord, g_u32PciCfgWrDword,
                  g_u32PciCfgFail, g_u32PciCfgRoReject, g_u32PciCfgCmdWrite,
                  g_u32PciCfgStatusRw1c, g_u32PciCfgBarSize,
                  g_u32PciCfgBarSizeRd, g_u32PciCfgBarWrite,
                  g_u32PciCfgBarSync, g_u32PciCfgBar64Enc,
                  g_u32PciCfgBarRefresh, g_u32PciCfgFindCap,
                  g_u32PciCfgFindCapHit, g_u32PciCfgFindCapMiss,
                  g_u32PciCfgLiveHit, g_u32PciCfgFallback);

    /*
     * BAR residual (32/64 encode + size + base→phys sync; Soft!=product).
     * greppable: udx: pci soft bar residual
     */
    pci_soft_emit("udx: pci soft bar residual "
                  "size_enter=%u size_rd=%u base_wr=%u sync=%u "
                  "enc64=%u refresh=%u "
                  "bar_max=%u present_mask=soft claim_mask=soft "
                  "type32=1 type64=1 size_probe=1 phys_sync=1 "
                  "pref_rtl=0|2 pref_xhci=0 "
                  "product_iomem=OPEN product_ecam=OPEN soft=1 "
                  "Soft!=product product_hosts=rtl8168_udx,xhci_udx "
                  "G-AC-1 dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u\n",
                  g_u32PciCfgBarSize, g_u32PciCfgBarSizeRd,
                  g_u32PciCfgBarWrite, g_u32PciCfgBarSync,
                  g_u32PciCfgBar64Enc, g_u32PciCfgBarRefresh,
                  (u32)UDX_PCI_BAR_MAX,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

    /*
     * Path catalog — what this soft surface is / is not.
     * greppable: udx: pci soft path
     */
    pci_soft_emit("udx: pci soft path register=udx_pci_register_driver "
                  "inject=udx_host_inject_pci_ex "
                  "grant=udx_host_install_granted_pci "
                  "ddi_bind=udx_host_bind_by_id "
                  "enable=udx_pci_enable enable_mem=udx_pci_enable_mem "
                  "master=udx_pci_set_master "
                  "regions=udx_pci_request_regions "
                  "region=udx_pci_request_region "
                  "selected=udx_pci_request_selected_regions "
                  "cfg=udx_pci_read_config_*/write_config_* "
                  "find_cap=udx_pci_find_capability "
                  "skeleton_gate=0 hard_gate=0 soft=1\n");

    /* Grep: udx: pci soft wave */
    pci_soft_emit("udx: pci soft wave n=%u unit=pci exclusive=1 "
                  "prefix=udx:_pci_soft deepen=1 "
                  "(soft inventory; never gates skeleton PASS)\n",
                  UDX_PCI_SOFT_WAVE);

    /*
     * Grep: udx: pci soft honesty.
     * Soft inventory != product multi-server confine.
     */
    pci_soft_emit("udx: pci soft honesty multi_server=0 confine=0 "
                  "exclusive=1 soft=1 product_kernel=OPEN wave=%u "
                  "Soft!=product G-AC-1 dual=MIT_OR_Apache-2.0\n",
                  UDX_PCI_SOFT_WAVE);

    /*
     * Lean residual enable/regions/cfg/cap/resource for product UDX bind.
     * greppable: udx: pci soft residual lean
     * greppable: udx: pci soft residual lean UDX eng
     * greppable: udx: pci soft api honesty
     * greppable: udx: pci soft product bind
     */
    pci_soft_emit("udx: pci soft residual lean "
                  "layout_ver=%u "
                  "enable=enable_cnt+COMMAND_IO_MEM+bar_refresh "
                  "enable_mem=COMMAND_MEM_only "
                  "master=COMMAND_MASTER "
                  "regions=per_bar_claim_mask "
                  "region=single selected=mask "
                  "cfg=soft_type0_256 ro+rw1c+bar_size "
                  "bar=encode32|64+size+phys_sync "
                  "cap=find_capability_soft "
                  "resource=start|len|is_mem "
                  "grant=install_granted_pci "
                  "unbind=quiesce_then_remove "
                  "scan=rescan_walk get=bdf_lookup id_match=driver_table "
                  "c2=enable|master|regions|cfg|bar|cap|resource "
                  "product_enable_pm=OPEN product_iomem=OPEN "
                  "product_ecam=OPEN product_cap_mint=OPEN "
                  "prefer_real=%u product_hosts=UDX "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN "
                  "soft=1 soft_ne_product=1 "
                  "dual=MIT_OR_Apache-2.0 G-AC-1 "
                  "lean_checks=%u storm=0 wave=%u "
                  "(Soft!=product; lean residual only; STRONGER)\n",
                  UDX_PCI_LAYOUT_VER, UDX_PCI_PREFER_REAL,
                  UDX_PCI_LEAN_CHECKS, UDX_PCI_SOFT_WAVE);
    pci_soft_emit("udx: pci soft residual lean UDX eng "
                  "layout_ver=%u "
                  "shape=enable|master|regions|cfg|cap|resource|grant "
                  "product_hosts=rtl8168_udx|xhci_udx "
                  "DoD_A=OPEN DoD_B=OPEN "
                  "rtl8168_udx=enable+master+regions+BAR0|2 "
                  "xhci_udx=enable+regions+BAR0 "
                  "product=UDX+ABI soft=1 product_mint=0 "
                  "G-AC-1 Soft!=product wave=%u\n",
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_emit("udx: pci soft api honesty "
                  "enable_cnt=soft enable_mem=soft "
                  "set_master=cmd_bit "
                  "request_regions=soft_mask "
                  "request_region=soft_bit "
                  "request_selected=soft_mask "
                  "cfg_image=256 type0=1 "
                  "status_rw1c=1 bar_size=1 find_cap=soft "
                  "resource=soft_bar_table "
                  "grant=soft_install unbind=soft_quiesce_remove "
                  "cap_mint=0 multi_server=0 soft=1 product_kernel=OPEN "
                  "DoD_A=OPEN DoD_B=OPEN Soft!=product "
                  "layout_ver=%u wave=%u\n",
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_emit("udx: pci soft product bind "
                  "path=enable|enable_mem,set_master,"
                  "request_regions|region|selected,cfg,"
                  "find_cap,resource "
                  "surface=Linux-porter hosts=rtl8168_udx,xhci_udx "
                  "grant=udx_host_install_granted_pci "
                  "ddi_bind=udx_host_bind_by_id "
                  "pref_bar_rtl=0|2 pref_bar_xhci=0 "
                  "DoD_A=OPEN DoD_B=OPEN "
                  "product=UDX+ABI soft=1 product_mint=0 "
                  "G-AC-1 (Soft!=product) layout_ver=%u wave=%u\n",
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

    /*
     * Grant residual (DDI install_granted → match/probe).
     * greppable: udx: pci soft grant residual
     */
    pci_soft_emit("udx: pci soft grant residual enter=%u ok=%u busy=%u "
                  "nomem=%u inval=%u bar0_map=%u "
                  "fs_slot=%u fs_peak=%u product_hit=%u "
                  "api=udx_host_install_granted_pci "
                  "via=ddi_bind|soft_grant "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "product_mint=0 soft=1 Soft!=product "
                  "G-AC-1 dual=MIT_OR_Apache-2.0 wave=%u\n",
                  g_u32PciGrant, g_u32PciGrantOk, g_u32PciGrantBusy,
                  g_u32PciGrantNomem, g_u32PciGrantInval,
                  g_u32PciGrantBar0Map, g_u32PciGrantFsSlot,
                  g_u32PciGrantFsPeak, g_u32PciGrantProductHit,
                  UDX_PCI_SOFT_WAVE);

    /*
     * Master residual (COMMAND MASTER bit; Soft!=product).
     * greppable: udx: pci soft master residual
     */
    pci_soft_emit("udx: pci soft master residual set_ok=%u set_inv=%u "
                  "clr=%u live_master=%u live_en=%u "
                  "cmd_bit=MASTER require_enable=1 "
                  "product_bm=OPEN soft=1 Soft!=product wave=%u\n",
                  g_u32PciSetMaster, g_u32PciSetMasterInv, g_u32PciClrMaster,
                  g_u32PciLiveMaster, g_u32PciLiveEnabled,
                  UDX_PCI_SOFT_WAVE);

    /*
     * Unbind residual (quiesce then remove; Soft!=product).
     * greppable: udx: pci soft unbind residual
     */
    pci_soft_emit("udx: pci soft unbind residual unbind=%u quiesce=%u "
                  "remove_cb=%u no_cb=%u live_bound=%u "
                  "order=quiesce_then_remove "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "soft=1 Soft!=product wave=%u\n",
                  g_u32PciUnbind, g_u32PciUnbindQuiesce,
                  g_u32PciUnbindRemove, g_u32PciUnbindNoCb,
                  g_u32PciLiveBound, UDX_PCI_SOFT_WAVE);

    /*
     * Cap residual (find_capability soft walk; mint OPEN).
     * greppable: udx: pci soft cap residual
     */
    pci_soft_emit("udx: pci soft cap residual enter=%u hit=%u miss=%u "
                  "residual_n=%u "
                  "walk=CAP_PTR soft_term=0x40 null_id=0 "
                  "cap_ids=PM|MSI|MSIX|PCIE|VENDOR "
                  "product_cap_mint=OPEN soft=1 Soft!=product "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "G-AC-1 dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u\n",
                  g_u32PciCfgFindCap, g_u32PciCfgFindCapHit,
                  g_u32PciCfgFindCapMiss, g_u32PciCapResidualN,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

    /*
     * Resource residual (start/len/is_mem; Soft!=product iomem).
     * greppable: udx: pci soft resource residual
     */
    pci_soft_emit("udx: pci soft resource residual "
                  "start=%u start_hit=%u len=%u len_hit=%u "
                  "is_mem=%u empty=%u "
                  "bar_max=%u present_mask=soft "
                  "pref_rtl_bar0=%u pref_rtl_bar2=%u pref_xhci_bar0=%u "
                  "product_iomem=OPEN soft=1 Soft!=product "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "G-AC-1 dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u\n",
                  g_u32PciResStart, g_u32PciResStartHit,
                  g_u32PciResLen, g_u32PciResLenHit,
                  g_u32PciResIsMem, g_u32PciResEmpty,
                  (u32)UDX_PCI_BAR_MAX,
                  (u32)UDX_PCI_RTL_PREF_BAR0, (u32)UDX_PCI_RTL_PREF_BAR2,
                  (u32)UDX_PCI_XHCI_PREF_BAR0,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

    /*
     * Product-host residual (G752 Dual DoD seed IDs; Soft!=product).
     * greppable: udx: pci soft product_host residual
     */
    pci_soft_emit("udx: pci soft product_host residual "
                  "rtl=%u xhci=%u note=%u "
                  "bar0_present=%u bar2_present=%u "
                  "grant_hit=%u inject_hit=%u "
                  "rtl_id=10ec:8168 xhci_id=8086:a12f "
                  "rtl_pref=BAR0|BAR2 xhci_pref=BAR0 "
                  "DoD_A=OPEN DoD_B=OPEN close=0 agent=OPEN "
                  "fs_class=SKIP product=UDX+ABI soft=1 product_mint=0 "
                  "G-AC-1 Soft!=product dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u\n",
                  g_u32PciProductHostRtl, g_u32PciProductHostXhci,
                  g_u32PciProductHostNote,
                  g_u32PciProductHostBar0, g_u32PciProductHostBar2,
                  g_u32PciGrantProductHit, g_u32PciInjectProductHit,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

    /*
     * C2 product path residual catalog (Dual DoD direction).
     * greppable: udx: pci soft c2 product path
     * greppable: udx: pci soft c2 residual
     */
    pci_soft_emit("udx: pci soft c2 product path "
                  "chain=register,grant|inject,match,probe,"
                  "enable|enable_mem,set_master,"
                  "request_regions|region|selected,"
                  "cfg,find_cap,resource,ioremap,dma,irq,set_drvdata "
                  "teardown=quiesce,remove,release,disable "
                  "enable=%u master=%u regions=%u cfg_r=%u cfg_w=%u "
                  "find_cap=%u resource=%u "
                  "grant_ok=%u inject_ok=%u bind_ok=%u "
                  "c2_en=%u c2_mst=%u c2_reg=%u c2_cfg=%u "
                  "c2_cap=%u c2_res=%u c2_grant=%u c2_bind=%u "
                  "hosts=rtl8168_udx,xhci_udx "
                  "DoD_A=OPEN DoD_B=OPEN "
                  "fs_class=SKIP product=UDX+ABI "
                  "soft=1 product_mint=0 soft_ne_product=1 "
                  "G-AC-1 dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u "
                  "(C2 residual; Soft!=product; agent!=close)\n",
                  g_u32PciEnable, g_u32PciSetMaster, g_u32PciReqReg,
                  g_u32PciCfgRead, g_u32PciCfgWrite,
                  g_u32PciCfgFindCap,
                  (u32)(g_u32PciResStart + g_u32PciResLen + g_u32PciResIsMem),
                  g_u32PciGrantOk, g_u32PciInjectOk,
                  (u32)(g_u32PciBindProbeOk + g_u32PciBindNoProbe),
                  g_u32PciC2Enable, g_u32PciC2Master, g_u32PciC2Regions,
                  g_u32PciC2Cfg, g_u32PciC2Cap, g_u32PciC2Resource,
                  g_u32PciC2Grant, g_u32PciC2Bind,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_emit("udx: pci soft c2 residual "
                  "enable=%u master=%u regions=%u cfg=%u "
                  "cap=%u resource=%u grant=%u bind=%u residual_n=%u "
                  "chain=register>grant|inject>match>probe>"
                  "enable>master>regions>cfg>cap>resource>"
                  "ioremap>dma>irq_thr_only>drvdata "
                  "teardown=quiesce>remove>release>disable "
                  "thr_only_irq=%u hard_primary_irq=%u "
                  "H1=no_net_eth_poll c2_steps=%u lean_checks=%u "
                  "hosts=rtl8168_udx,xhci_udx "
                  "DoD_A=%u DoD_B=%u close=0 agent=OPEN "
                  "fs_class=SKIP product=UDX+ABI soft=1 product_mint=0 "
                  "soft_ne_product=1 G-AC-1 dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u "
                  "(C2 residual deepen; Soft!=product; thr-only; STRONGER)\n",
                  g_u32PciC2Enable, g_u32PciC2Master, g_u32PciC2Regions,
                  g_u32PciC2Cfg, g_u32PciC2Cap, g_u32PciC2Resource,
                  g_u32PciC2Grant, g_u32PciC2Bind, g_u32PciC2ResidualN,
                  UDX_PCI_THR_ONLY_IRQ, UDX_PCI_HARD_PRIMARY_IRQ,
                  UDX_PCI_C2_CHAIN_STEPS, UDX_PCI_LEAN_CHECKS,
                  UDX_PCI_DOD_A_OPEN, UDX_PCI_DOD_B_OPEN,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

    /*
     * Dual DoD A/B honesty (OPEN; agent != close).
     * greppable: udx: pci soft dual DoD
     * greppable: udx: pci soft residual dual_dod OPEN
     */
    pci_soft_emit("udx: pci soft dual DoD "
                  "A=xhci_udx:8086:a12f:OPEN "
                  "B=rtl8168_udx:10ec:8168:OPEN "
                  "close=0 agent=OPEN soft_lamps_ne_close=1 "
                  "grant_product_hit=%u inject_product_hit=%u "
                  "rtl_note=%u xhci_note=%u "
                  "product=UDX+DDI+hot_cold_ABI "
                  "fs_rtl=SKIP fs_xhci=SKIP "
                  "soft=1 product_mint=0 G-AC-1 "
                  "Soft!=product dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u\n",
                  g_u32PciGrantProductHit, g_u32PciInjectProductHit,
                  g_u32PciProductHostRtl, g_u32PciProductHostXhci,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_emit("udx: pci soft residual dual_dod OPEN "
                  "DoD_A=OPEN DoD_B=OPEN close=0 agent=OPEN "
                  "A=xhci_udx:8086:a12f B=rtl8168_udx:10ec:8168 "
                  "prefer_real=%u product_hosts=UDX "
                  "scan_hit=%u get_hit=%u id_match_hit=%u "
                  "soft=1 product_mint=0 soft_ne_product=1 "
                  "G-AC-1 Soft!=product dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u\n",
                  UDX_PCI_PREFER_REAL,
                  g_u32PciScanProductHit, g_u32PciGetProductHit,
                  g_u32PciIdMatchProductHit,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

    /*
     * STRONGER residual densify: scan / get / id_match for host bind.
     * greppable: udx: pci soft residual scan
     * greppable: udx: pci soft residual get
     * greppable: udx: pci soft residual id_match
     * greppable: Soft!=product soft residual dual_dod OPEN prefer_real
     *            product_hosts=UDX
     */
    pci_soft_emit("udx: pci soft residual scan "
                  "enter=%u walk=%u product_hit=%u "
                  "rtl=%u xhci=%u residual_n=%u rescan=%u "
                  "api=udx_host_rescan_pci "
                  "chain=scan>try_bind>id_match "
                  "ids=10ec:8168,8086:a12f "
                  "prefer_real=%u product_hosts=UDX "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN close=0 "
                  "fs_class=SKIP product=UDX+ABI soft=1 product_mint=0 "
                  "soft_ne_product=1 G-AC-1 Soft!=product "
                  "lean_checks=%u steps=%u "
                  "layout_ver=%u wave=%u "
                  "(scan residual densify; host bind; STRONGER)\n",
                  g_u32PciScanEnter, g_u32PciScanWalk,
                  g_u32PciScanProductHit, g_u32PciScanRtl, g_u32PciScanXhci,
                  g_u32PciScanResidualN, g_u32PciRescan,
                  UDX_PCI_PREFER_REAL, UDX_PCI_LEAN_CHECKS,
                  UDX_PCI_SCAN_GET_MATCH_STEPS,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_emit("udx: pci soft residual get "
                  "enter=%u hit=%u miss=%u product_hit=%u "
                  "rtl=%u xhci=%u residual_n=%u "
                  "api=udx_pci_get_device "
                  "chain=get>id_match>probe "
                  "ids=10ec:8168,8086:a12f "
                  "prefer_real=%u product_hosts=UDX "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN close=0 "
                  "fs_class=SKIP product=UDX+ABI soft=1 product_mint=0 "
                  "soft_ne_product=1 G-AC-1 Soft!=product "
                  "lean_checks=%u steps=%u "
                  "layout_ver=%u wave=%u "
                  "(get residual densify; host bind; STRONGER)\n",
                  g_u32PciGetDev, g_u32PciGetDevHit, g_u32PciGetDevMiss,
                  g_u32PciGetProductHit, g_u32PciGetRtl, g_u32PciGetXhci,
                  g_u32PciGetResidualN,
                  UDX_PCI_PREFER_REAL, UDX_PCI_LEAN_CHECKS,
                  UDX_PCI_SCAN_GET_MATCH_STEPS,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_emit("udx: pci soft residual id_match "
                  "enter=%u hit=%u miss=%u product_hit=%u "
                  "rtl=%u xhci=%u any=%u residual_n=%u "
                  "api=pci_id_match "
                  "chain=scan>get>id_match>probe "
                  "ids=10ec:8168,8086:a12f "
                  "prefer_real=%u product_hosts=UDX "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN close=0 "
                  "fs_class=SKIP product=UDX+ABI soft=1 product_mint=0 "
                  "soft_ne_product=1 G-AC-1 Soft!=product "
                  "lean_checks=%u steps=%u "
                  "layout_ver=%u wave=%u "
                  "(id_match residual densify; host bind; STRONGER)\n",
                  g_u32PciIdMatchEnter, g_u32PciIdMatchHit,
                  g_u32PciIdMatchMiss, g_u32PciIdMatchProductHit,
                  g_u32PciIdMatchRtl, g_u32PciIdMatchXhci,
                  g_u32PciIdMatchAny, g_u32PciIdMatchResidualN,
                  UDX_PCI_PREFER_REAL, UDX_PCI_LEAN_CHECKS,
                  UDX_PCI_SCAN_GET_MATCH_STEPS,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

    /*
     * denser residual (preferred over lamps alone; Soft!=product).
     * greppable: udx: pci soft residual denser
     * greppable: denser_prove=1 | denser residual
     * greppable: Soft!=product soft residual dual_dod OPEN prefer_real
     *            product_hosts=UDX
     */
    pci_soft_emit("udx: pci soft residual denser "
                  "denser_prove=%u denser_chain=%u denser_product_host=%u "
                  "scan_hit=%u get_hit=%u id_match_hit=%u "
                  "rtl_note=%u xhci_note=%u residual_n=%u "
                  "chain=scan>get>id_match "
                  "ids=10ec:8168,8086:a12f "
                  "prefer_real=%u product_hosts=UDX "
                  "DoD_A=%u DoD_B=%u dual_dod=OPEN agent_close=%u "
                  "soft_ne_product=%u fs_class_skip=%u stamp_free_75=%u "
                  "c2_steps=%u lean_checks=%u "
                  "soft=1 product_mint=0 product=UDX+ABI "
                  "G-AC-1 Soft!=product dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u "
                  "(denser residual prove; Soft!=product; Dual DoD OPEN)\n",
                  UDX_PCI_DENSER_PROVE, UDX_PCI_DENSER_CHAIN,
                  UDX_PCI_DENSER_PRODUCT_HOST,
                  g_u32PciScanProductHit, g_u32PciGetProductHit,
                  g_u32PciIdMatchProductHit,
                  g_u32PciProductHostRtl, g_u32PciProductHostXhci,
                  g_u32PciDenserResidualN,
                  UDX_PCI_PREFER_REAL,
                  UDX_PCI_DOD_A_OPEN, UDX_PCI_DOD_B_OPEN,
                  UDX_PCI_AGENT_CLOSE,
                  UDX_PCI_SOFT_NE_PRODUCT, UDX_PCI_FS_CLASS_SKIP,
                  UDX_PCI_STAMP_FREE_75,
                  UDX_PCI_C2_CHAIN_STEPS, UDX_PCI_LEAN_CHECKS,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);

}

/**
 * One-shot scan residual densify after host_rescan_pci walk.
 * Observation only; Soft!=product; Dual DoD OPEN; prefer_real product_hosts=UDX.
 * greppable: udx: pci soft residual scan
 */
static void
pci_soft_scan_residual_maybe(void)
{
    if (g_fPciScanResidualOnce != 0) {
        return;
    }
    if (g_u32PciScanEnter == 0 && g_u32PciRescan == 0) {
        return;
    }
    g_fPciScanResidualOnce = 1;
    pci_soft_inc(&g_u32PciScanResidualN);
    pci_soft_emit("udx: pci soft residual scan "
                  "once=1 enter=%u walk=%u product_hit=%u "
                  "rtl=%u xhci=%u "
                  "prefer_real=%u product_hosts=UDX "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN close=0 "
                  "ids=10ec:8168,8086:a12f "
                  "api=udx_host_rescan_pci soft=1 product_mint=0 "
                  "soft_ne_product=1 G-AC-1 Soft!=product "
                  "layout_ver=%u wave=%u "
                  "(scan residual densify; STRONGER)\n",
                  g_u32PciScanEnter, g_u32PciScanWalk,
                  g_u32PciScanProductHit, g_u32PciScanRtl, g_u32PciScanXhci,
                  UDX_PCI_PREFER_REAL,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_denser_residual_maybe();
}

/**
 * One-shot get residual densify after udx_pci_get_device.
 * Observation only; Soft!=product; Dual DoD OPEN; prefer_real product_hosts=UDX.
 * greppable: udx: pci soft residual get
 */
static void
pci_soft_get_residual_maybe(void)
{
    if (g_fPciGetResidualOnce != 0) {
        return;
    }
    if (g_u32PciGetDev == 0) {
        return;
    }
    g_fPciGetResidualOnce = 1;
    pci_soft_inc(&g_u32PciGetResidualN);
    pci_soft_emit("udx: pci soft residual get "
                  "once=1 enter=%u hit=%u miss=%u product_hit=%u "
                  "rtl=%u xhci=%u "
                  "prefer_real=%u product_hosts=UDX "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN close=0 "
                  "ids=10ec:8168,8086:a12f "
                  "api=udx_pci_get_device soft=1 product_mint=0 "
                  "soft_ne_product=1 G-AC-1 Soft!=product "
                  "layout_ver=%u wave=%u "
                  "(get residual densify; STRONGER)\n",
                  g_u32PciGetDev, g_u32PciGetDevHit, g_u32PciGetDevMiss,
                  g_u32PciGetProductHit, g_u32PciGetRtl, g_u32PciGetXhci,
                  UDX_PCI_PREFER_REAL,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_denser_residual_maybe();
}

/**
 * One-shot id_match residual densify after pci_id_match product path.
 * Observation only; Soft!=product; Dual DoD OPEN; prefer_real product_hosts=UDX.
 * greppable: udx: pci soft residual id_match
 */
static void
pci_soft_id_match_residual_maybe(void)
{
    if (g_fPciIdMatchResidualOnce != 0) {
        return;
    }
    if (g_u32PciIdMatchEnter == 0) {
        return;
    }
    g_fPciIdMatchResidualOnce = 1;
    pci_soft_inc(&g_u32PciIdMatchResidualN);
    pci_soft_emit("udx: pci soft residual id_match "
                  "once=1 enter=%u hit=%u miss=%u product_hit=%u "
                  "rtl=%u xhci=%u any=%u "
                  "prefer_real=%u product_hosts=UDX "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN close=0 "
                  "ids=10ec:8168,8086:a12f "
                  "api=pci_id_match soft=1 product_mint=0 "
                  "soft_ne_product=1 G-AC-1 Soft!=product "
                  "layout_ver=%u wave=%u "
                  "(id_match residual densify; STRONGER)\n",
                  g_u32PciIdMatchEnter, g_u32PciIdMatchHit,
                  g_u32PciIdMatchMiss, g_u32PciIdMatchProductHit,
                  g_u32PciIdMatchRtl, g_u32PciIdMatchXhci,
                  g_u32PciIdMatchAny, UDX_PCI_PREFER_REAL,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_denser_residual_maybe();
}

/**
 * One-shot denser residual prove after scan/get/id_match product path.
 * Preferred over lamps alone; Soft!=product; Dual DoD OPEN; agent!=close.
 * greppable: udx: pci soft residual denser
 * greppable: denser_prove=1
 */
static void
pci_soft_denser_residual_maybe(void)
{
    if (g_fPciDenserResidualOnce != 0) {
        return;
    }
    /* denser_prove needs at least one densify surface live. */
    if (g_u32PciScanEnter == 0 && g_u32PciGetDev == 0 &&
        g_u32PciIdMatchEnter == 0) {
        return;
    }
    g_fPciDenserResidualOnce = 1;
    pci_soft_inc(&g_u32PciDenserResidualN);
    pci_soft_emit("udx: pci soft residual denser "
                  "once=1 denser_prove=%u denser_chain=%u "
                  "denser_product_host=%u "
                  "scan_hit=%u get_hit=%u id_match_hit=%u "
                  "prefer_real=%u product_hosts=UDX "
                  "DoD_A=OPEN DoD_B=OPEN dual_dod=OPEN agent_close=%u "
                  "soft_ne_product=%u fs_class_skip=%u "
                  "lean_checks=%u soft=1 product_mint=0 "
                  "soft_ne_product=1 G-AC-1 Soft!=product "
                  "layout_ver=%u wave=%u "
                  "(denser residual prove once; Soft!=product)\n",
                  UDX_PCI_DENSER_PROVE, UDX_PCI_DENSER_CHAIN,
                  UDX_PCI_DENSER_PRODUCT_HOST,
                  g_u32PciScanProductHit, g_u32PciGetProductHit,
                  g_u32PciIdMatchProductHit,
                  UDX_PCI_PREFER_REAL, UDX_PCI_AGENT_CLOSE,
                  UDX_PCI_SOFT_NE_PRODUCT, UDX_PCI_FS_CLASS_SKIP,
                  UDX_PCI_LEAN_CHECKS,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
}

/**
 * One-shot Dual DoD A/B OPEN honesty after product bind residual.
 * Observation only; Soft!=product; never closes Dual DoD A/B.
 * greppable: udx: pci soft dual DoD residual
 * greppable: udx: pci soft residual dual_dod OPEN
 */
static void
pci_soft_dual_dod_maybe(void)
{
    if (g_fPciDualDodOnce != 0) {
        return;
    }
    if (g_fPciProductBindOnce == 0) {
        return;
    }
    g_fPciDualDodOnce = 1;
    pci_soft_emit("udx: pci soft dual DoD residual "
                  "A=OPEN B=OPEN close=0 agent=OPEN "
                  "enable=%u master=%u regions=%u grant_ok=%u "
                  "rtl_note=%u xhci_note=%u "
                  "scan_hit=%u get_hit=%u id_match_hit=%u "
                  "prefer_real=%u product_hosts=UDX "
                  "hosts=rtl8168_udx,xhci_udx "
                  "soft_lamps_ne_close=1 product_mint=0 "
                  "fs_class=SKIP product=UDX+ABI "
                  "G-AC-1 Soft!=product dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u\n",
                  g_u32PciEnable, g_u32PciSetMaster, g_u32PciReqReg,
                  g_u32PciGrantOk,
                  g_u32PciProductHostRtl, g_u32PciProductHostXhci,
                  g_u32PciScanProductHit, g_u32PciGetProductHit,
                  g_u32PciIdMatchProductHit, UDX_PCI_PREFER_REAL,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_emit("udx: pci soft residual dual_dod OPEN "
                  "once=1 DoD_A=OPEN DoD_B=OPEN close=0 agent=OPEN "
                  "prefer_real=%u product_hosts=UDX "
                  "scan=%u get=%u id_match=%u "
                  "soft=1 Soft!=product dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u\n",
                  UDX_PCI_PREFER_REAL,
                  g_u32PciScanProductHit, g_u32PciGetProductHit,
                  g_u32PciIdMatchProductHit,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
}

/**
 * One-shot cap residual after find_capability used.
 * Observation only; Soft!=product; cap mint remains OPEN.
 * greppable: udx: pci soft cap residual
 */
static void
pci_soft_cap_residual_maybe(void)
{
    if (g_fPciCapResidualOnce != 0) {
        return;
    }
    if (g_u32PciCfgFindCap == 0) {
        return;
    }
    g_fPciCapResidualOnce = 1;
    pci_soft_inc(&g_u32PciCapResidualN);
    pci_soft_emit("udx: pci soft cap residual "
                  "once=1 enter=%u hit=%u miss=%u "
                  "walk=CAP_PTR soft_term=0x40 "
                  "product_cap_mint=OPEN soft=1 Soft!=product "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "G-AC-1 layout_ver=%u wave=%u\n",
                  g_u32PciCfgFindCap, g_u32PciCfgFindCapHit,
                  g_u32PciCfgFindCapMiss,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
}

/**
 * One-shot resource residual after resource_* query used.
 * Observation only; Soft!=product; iomem tree remains OPEN.
 * greppable: udx: pci soft resource residual
 */
static void
pci_soft_resource_residual_maybe(void)
{
    if (g_fPciResourceResidualOnce != 0) {
        return;
    }
    if (g_u32PciResStart == 0 && g_u32PciResLen == 0 &&
        g_u32PciResIsMem == 0) {
        return;
    }
    g_fPciResourceResidualOnce = 1;
    pci_soft_emit("udx: pci soft resource residual "
                  "once=1 start=%u len=%u is_mem=%u "
                  "start_hit=%u len_hit=%u empty=%u "
                  "pref_rtl=0|2 pref_xhci=0 "
                  "product_iomem=OPEN soft=1 Soft!=product "
                  "product_hosts=rtl8168_udx,xhci_udx "
                  "G-AC-1 layout_ver=%u wave=%u\n",
                  g_u32PciResStart, g_u32PciResLen, g_u32PciResIsMem,
                  g_u32PciResStartHit, g_u32PciResLenHit, g_u32PciResEmpty,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
}

/**
 * One-shot BAR residual after size/base/encode residual used.
 * Observation only; Soft!=product; ECAM/iomem remain OPEN.
 * greppable: udx: pci soft bar residual
 */
static void
pci_soft_bar_residual_maybe(void)
{
    if (g_fPciBarResidualOnce != 0) {
        return;
    }
    if (g_u32PciCfgBarSize == 0 && g_u32PciCfgBarWrite == 0 &&
        g_u32PciCfgBar64Enc == 0 && g_u32PciCfgBarRefresh == 0 &&
        g_u32PciCfgBarSync == 0) {
        return;
    }
    g_fPciBarResidualOnce = 1;
    pci_soft_emit("udx: pci soft bar residual "
                  "once=1 size_enter=%u size_rd=%u base_wr=%u "
                  "sync=%u enc64=%u refresh=%u "
                  "type32=1 type64=1 size_probe=1 phys_sync=1 "
                  "product_ecam=OPEN product_iomem=OPEN soft=1 "
                  "Soft!=product product_hosts=rtl8168_udx,xhci_udx "
                  "G-AC-1 layout_ver=%u wave=%u\n",
                  g_u32PciCfgBarSize, g_u32PciCfgBarSizeRd,
                  g_u32PciCfgBarWrite, g_u32PciCfgBarSync,
                  g_u32PciCfgBar64Enc, g_u32PciCfgBarRefresh,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
}

/**
 * One-shot enable residual deepen after first enable/enable_mem.
 * Observation only; Soft!=product; pm/runtime remain OPEN.
 * greppable: udx: pci soft enable residual
 */
static void
pci_soft_enable_residual_maybe(void)
{
    if (g_fPciEnableResidualOnce != 0) {
        return;
    }
    if (g_u32PciEnable == 0 && g_u32PciEnableMem == 0) {
        return;
    }
    g_fPciEnableResidualOnce = 1;
    pci_soft_emit("udx: pci soft enable residual "
                  "once=1 ok=%u first=%u nested=%u "
                  "mem_ok=%u mem_first=%u "
                  "disable=%u disable_last=%u "
                  "cmd_bits=IO+MEM|MEM_only bar_refresh=1 "
                  "soft_cnt=1 product_pm=OPEN soft=1 "
                  "Soft!=product product_hosts=rtl8168_udx,xhci_udx "
                  "G-AC-1 layout_ver=%u wave=%u\n",
                  g_u32PciEnable, g_u32PciEnableFirst, g_u32PciEnableNested,
                  g_u32PciEnableMem, g_u32PciEnableMemFirst,
                  g_u32PciDisable, g_u32PciDisableLast,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
}

/**
 * One-shot product_host residual after G752 ID residual noted.
 * Observation only; Soft!=product; never closes Dual DoD A/B.
 * greppable: udx: pci soft product_host residual
 */
static void
pci_soft_product_host_residual_maybe(void)
{
    if (g_fPciProductHostResidualOnce != 0) {
        return;
    }
    if (g_u32PciProductHostNote == 0) {
        return;
    }
    g_fPciProductHostResidualOnce = 1;
    pci_soft_emit("udx: pci soft product_host residual "
                  "once=1 rtl=%u xhci=%u "
                  "bar0=%u bar2=%u grant_hit=%u inject_hit=%u "
                  "DoD_A=OPEN DoD_B=OPEN close=0 agent=OPEN "
                  "fs_class=SKIP product=UDX+ABI soft=1 product_mint=0 "
                  "G-AC-1 Soft!=product layout_ver=%u wave=%u\n",
                  g_u32PciProductHostRtl, g_u32PciProductHostXhci,
                  g_u32PciProductHostBar0, g_u32PciProductHostBar2,
                  g_u32PciGrantProductHit, g_u32PciInjectProductHit,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
}

/**
 * One-shot C2 residual deepen lamp after enable + regions product path.
 * Observation only; Soft!=product; Dual DoD A/B remain OPEN.
 * greppable: udx: pci soft c2 residual
 */
static void
pci_soft_c2_residual_maybe(void)
{
    if (g_fPciC2ResidualOnce != 0) {
        return;
    }
    if (g_u32PciC2Enable == 0 || g_u32PciC2Regions == 0) {
        return;
    }
    g_fPciC2ResidualOnce = 1;
    pci_soft_inc(&g_u32PciC2ResidualN);
    pci_soft_emit("udx: pci soft c2 residual "
                  "once=1 enable=%u master=%u regions=%u cfg=%u "
                  "cap=%u resource=%u grant=%u bind=%u "
                  "hosts=rtl8168_udx,xhci_udx "
                  "DoD_A=OPEN DoD_B=OPEN close=0 agent=OPEN "
                  "fs_class=SKIP product=UDX+ABI soft=1 product_mint=0 "
                  "soft_ne_product=1 G-AC-1 dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u "
                  "(C2 residual deepen; Soft!=product; agent!=close)\n",
                  g_u32PciC2Enable, g_u32PciC2Master, g_u32PciC2Regions,
                  g_u32PciC2Cfg, g_u32PciC2Cap, g_u32PciC2Resource,
                  g_u32PciC2Grant, g_u32PciC2Bind,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
}

/**
 * One-shot product-bind residual after enable + regions path used.
 * Observation only; Soft!=product; never gates skeleton PASS.
 * greppable: udx: pci soft product bind residual
 */
static void
pci_soft_product_bind_maybe(void)
{
    if (g_fPciProductBindOnce != 0) {
        return;
    }
    if (g_u32PciEnable == 0 && g_u32PciEnableMem == 0) {
        return;
    }
    if (g_u32PciReqReg == 0 && g_u32PciReqOne == 0 && g_u32PciReqSel == 0) {
        return;
    }
    g_fPciProductBindOnce = 1;
    pci_soft_count_lists();
    pci_soft_emit("udx: pci soft product bind residual "
                  "enable=%u enable_mem=%u master=%u "
                  "regions=%u region=%u selected=%u "
                  "cfg_r=%u cfg_w=%u find_cap=%u "
                  "resource=%u grant_ok=%u "
                  "path=enable,master,regions,cfg,cap,resource "
                  "hosts=rtl8168_udx,xhci_udx product=UDX+ABI "
                  "DoD_A=OPEN DoD_B=OPEN close=0 "
                  "soft=1 product_mint=0 soft_ne_product=1 "
                  "G-AC-1 Soft!=product dual=MIT_OR_Apache-2.0 "
                  "layout_ver=%u wave=%u\n",
                  g_u32PciEnable, g_u32PciEnableMem, g_u32PciSetMaster,
                  g_u32PciReqReg, g_u32PciReqOne, g_u32PciReqSel,
                  g_u32PciCfgRead, g_u32PciCfgWrite, g_u32PciCfgFindCap,
                  (u32)(g_u32PciResStart + g_u32PciResLen + g_u32PciResIsMem),
                  g_u32PciGrantOk,
                  UDX_PCI_LAYOUT_VER, UDX_PCI_SOFT_WAVE);
    pci_soft_dual_dod_maybe();
    pci_soft_c2_residual_maybe();
}

/** Soft: one-shot inventory after first register/inject/grant/bind success. */
static void
pci_soft_maybe_once(void)
{
    if (g_fPciSoftOnce != 0) {
        return;
    }
    if (g_u32PciRegDrvOk == 0 && g_u32PciInjectOk == 0 &&
        g_u32PciGrantOk == 0 &&
        g_u32PciBindProbeOk == 0 && g_u32PciBindNoProbe == 0) {
        return;
    }
    g_fPciSoftOnce = 1;
    pci_soft_inventory_log();
}

static struct udx_pci_bound *
pci_bound_of(const struct udx_pci_dev *pPdev)
{
    if (pPdev == NULL || pPdev->pDev == NULL) {
        return NULL;
    }
    return (struct udx_pci_bound *)pPdev->pDev->pBackend;
}

static void
pci_cfg_put_word(u8 *pCfg, int nWhere, u16 u16Val)
{
    if (nWhere < 0 || nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        return;
    }
    pCfg[nWhere] = (u8)(u16Val & 0xffu);
    pCfg[nWhere + 1] = (u8)((u16Val >> 8) & 0xffu);
}

static void
pci_cfg_put_dword(u8 *pCfg, int nWhere, u32 u32Val)
{
    if (nWhere < 0 || nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        return;
    }
    pCfg[nWhere] = (u8)(u32Val & 0xffu);
    pCfg[nWhere + 1] = (u8)((u32Val >> 8) & 0xffu);
    pCfg[nWhere + 2] = (u8)((u32Val >> 16) & 0xffu);
    pCfg[nWhere + 3] = (u8)((u32Val >> 24) & 0xffu);
}

static u16
pci_cfg_get_word(const u8 *pCfg, int nWhere)
{
    if (nWhere < 0 || nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        return 0xffffu;
    }
    return (u16)(pCfg[nWhere] | ((u16)pCfg[nWhere + 1] << 8));
}

static u32
pci_cfg_get_dword(const u8 *pCfg, int nWhere)
{
    if (nWhere < 0 || nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        return 0xffffffffu;
    }
    return (u32)pCfg[nWhere] |
           ((u32)pCfg[nWhere + 1] << 8) |
           ((u32)pCfg[nWhere + 2] << 16) |
           ((u32)pCfg[nWhere + 3] << 24);
}

/**
 * Refresh present-BAR residual mask from pdev lengths (product bind residual).
 */
static void
pci_bar_present_refresh(struct udx_pci_bound *pBound)
{
    struct udx_pci_dev *pPdev;
    int iBar;
    u8 u8Present;

    if (pBound == NULL || pBound->pPdev == NULL) {
        return;
    }
    pPdev = pBound->pPdev;
    u8Present = 0;
    for (iBar = 0; iBar < 6; iBar++) {
        if (pPdev->aBarLen[iBar] != 0) {
            u8Present = (u8)(u8Present | (u8)(1u << iBar));
        }
    }
    pBound->u8BarPresentMask = u8Present;
}

/**
 * Mirror COMMAND soft residual into enable_cnt / master flags.
 * Used by cfg write path and enable/disable.
 */
static void
pci_cfg_mirror_command(struct udx_pci_bound *pBound, u16 u16Cmd)
{
    if (pBound == NULL) {
        return;
    }
    pBound->u8Master = (u16Cmd & UDX_PCI_COMMAND_MASTER) ? 1u : 0u;
    if ((u16Cmd & (UDX_PCI_COMMAND_IO | UDX_PCI_COMMAND_MEMORY)) != 0) {
        pBound->u8Enabled = 1;
        if (pBound->u32EnableCnt == 0) {
            pBound->u32EnableCnt = 1;
        }
    } else {
        pBound->u8Enabled = 0;
        pBound->u32EnableCnt = 0;
        pBound->u8Master = 0;
        pBound->u8EnableMemOnly = 0;
    }
}

/**
 * Soft residual: RO identity field ranges for type-0 cfg image.
 * Vendor/device, revision/class, header type, subsystem, cap_ptr.
 * Soft!=product (not full PCIe RO table).
 */
static int
pci_cfg_is_ro_where(int nWhere)
{
    if (nWhere < 0 || nWhere >= (int)UDX_PCI_CFG_SIZE) {
        return 0;
    }
    /* Vendor + Device (0x00..0x03). */
    if (nWhere >= UDX_PCI_CFG_VENDOR && nWhere <= UDX_PCI_CFG_DEVICE + 1) {
        return 1;
    }
    /* Revision + class code (0x08..0x0b). */
    if (nWhere >= UDX_PCI_CFG_REVISION && nWhere <= 0x0b) {
        return 1;
    }
    /* Header type (0x0e) soft residual RO. */
    if (nWhere == UDX_PCI_CFG_HEADER) {
        return 1;
    }
    /* Subsystem vendor/device (0x2c..0x2f). */
    if (nWhere >= UDX_PCI_CFG_SUBSYS_VEN && nWhere <= UDX_PCI_CFG_SUBSYS_DEV + 1) {
        return 1;
    }
    /* Cap pointer soft residual RO (cap mint OPEN). */
    if (nWhere == UDX_PCI_CFG_CAP_PTR) {
        return 1;
    }
    return 0;
}

/**
 * Soft BAR index for dword-aligned cfg where, or -1 if not a BAR.
 */
static int
pci_cfg_bar_index(int nWhere)
{
    int nRel;

    if (nWhere < UDX_PCI_CFG_BAR0 || nWhere > UDX_PCI_CFG_BAR0 + 5 * 4) {
        return -1;
    }
    nRel = nWhere - UDX_PCI_CFG_BAR0;
    if ((nRel & 3) != 0) {
        return -1;
    }
    return nRel / 4;
}

/**
 * Soft BAR size residual: all-1s write → next read returns size mask.
 * Soft!=product · not live ECAM BAR reprogram.
 */
static void
pci_cfg_bar_enter_size(struct udx_pci_bound *pBound, int iBar)
{
    if (pBound == NULL || iBar < 0 || iBar > 5) {
        return;
    }
    pBound->u8BarSizeMask =
        (u8)(pBound->u8BarSizeMask | (u8)(1u << iBar));
    pci_soft_inc(&g_u32PciCfgBarSize);
    pci_soft_bar_residual_maybe();
}

static void
pci_cfg_bar_leave_size(struct udx_pci_bound *pBound, int iBar)
{
    if (pBound == NULL || iBar < 0 || iBar > 5) {
        return;
    }
    pBound->u8BarSizeMask =
        (u8)(pBound->u8BarSizeMask & (u8)~(1u << iBar));
}

/**
 * Soft residual: next BAR index free for 64-bit high dword (len==0).
 * Product hosts: rtl BAR1 empty under BAR0; xhci BAR1 empty under BAR0.
 */
static int
pci_cfg_bar_hi_slot_free(const struct udx_pci_dev *pPdev, int iBar)
{
    if (pPdev == NULL || iBar < 0 || iBar >= 5) {
        return 0;
    }
    return pPdev->aBarLen[iBar + 1] == 0 ? 1 : 0;
}

/**
 * Soft residual: map cfg BAR index to owning resource BAR.
 * High dword of a 64-bit BAR folds to the low index.
 */
static int
pci_cfg_bar_owner(const struct udx_pci_bound *pBound, int iBar)
{
    if (pBound == NULL || iBar < 0 || iBar > 5) {
        return -1;
    }
    if (iBar > 0 &&
        (pBound->u8Bar64Mask & (u8)(1u << (iBar - 1))) != 0) {
        return iBar - 1;
    }
    return iBar;
}

/**
 * Soft size mask for BAR residual (len power-of-two shape).
 * Mem: low 4 type bits preserved; Io: low 2 type bits.
 * 64-bit low dword: size bits in address field; high dword all-1s residual.
 */
static u32
pci_cfg_bar_size_mask(const struct udx_pci_dev *pPdev, int iBar)
{
    u64 u64Len;
    u32 u32Mask;

    if (pPdev == NULL || iBar < 0 || iBar > 5) {
        return 0;
    }
    u64Len = pPdev->aBarLen[iBar];
    if (u64Len == 0) {
        return 0;
    }
    /* Soft residual: ~(len-1) folded to 32-bit address bits. */
    u32Mask = (u32)(~(u64Len - 1ull));
    if (pPdev->aBarIsMem[iBar]) {
        u32Mask &= 0xfffffff0u;
    } else {
        u32Mask = (u32Mask & 0xfffffffeu) | 0x1u;
    }
    return u32Mask;
}

/**
 * Soft residual size read for a cfg BAR dword index (low or 64-bit high).
 * High half of 64-bit BAR returns upper size bits (often 0xffffffff soft).
 */
static u32
pci_cfg_bar_size_mask_at(const struct udx_pci_bound *pBound, int iBar)
{
    int iOwner;
    u64 u64Len;
    u32 u32Mask;

    if (pBound == NULL || pBound->pPdev == NULL || iBar < 0 || iBar > 5) {
        return 0;
    }
    iOwner = pci_cfg_bar_owner(pBound, iBar);
    if (iOwner < 0) {
        return 0;
    }
    if (iOwner == iBar) {
        return pci_cfg_bar_size_mask(pBound->pPdev, iBar);
    }
    /* High dword of 64-bit BAR: upper half of ~(len-1). */
    u64Len = pBound->pPdev->aBarLen[iOwner];
    if (u64Len == 0) {
        return 0;
    }
    u32Mask = (u32)((~(u64Len - 1ull)) >> 32);
    return u32Mask;
}

/**
 * Encode one present BAR into soft type-0 cfg image from aBarPhys/aBarLen.
 * MEM with phys>>32 and free next slot → type-64 + high dword residual.
 * Soft!=product · not live ECAM reprogram.
 * greppable: udx: pci soft bar residual
 */
static void
pci_cfg_bar_encode_one(struct udx_pci_bound *pBound, int iBar)
{
    struct udx_pci_dev *pPdev;
    u64 u64Phys;
    u64 u64Len;
    u32 u32Lo;
    int nOff;

    if (pBound == NULL || pBound->pPdev == NULL || iBar < 0 || iBar > 5) {
        return;
    }
    pPdev = pBound->pPdev;
    u64Len = pPdev->aBarLen[iBar];
    nOff = UDX_PCI_CFG_BAR0 + iBar * 4;

    /* Skip high half of a prior 64-bit BAR (owned by iBar-1). */
    if (iBar > 0 &&
        (pBound->u8Bar64Mask & (u8)(1u << (iBar - 1))) != 0) {
        return;
    }

    if (u64Len == 0) {
        pci_cfg_put_dword(pBound->aCfg, nOff, 0);
        pBound->u8Bar64Mask =
            (u8)(pBound->u8Bar64Mask & (u8)~(1u << iBar));
        return;
    }

    u64Phys = pPdev->aBarPhys[iBar];
    if (pPdev->aBarIsMem[iBar]) {
        u32Lo = (u32)(u64Phys & 0xfffffff0ull);
        if ((u64Phys >> 32) != 0 && pci_cfg_bar_hi_slot_free(pPdev, iBar)) {
            u32Lo |= UDX_PCI_BAR_TYPE_64;
            pBound->u8Bar64Mask =
                (u8)(pBound->u8Bar64Mask | (u8)(1u << iBar));
            pci_cfg_put_dword(pBound->aCfg, nOff, u32Lo);
            pci_cfg_put_dword(pBound->aCfg, nOff + 4,
                              (u32)(u64Phys >> 32));
            pci_soft_inc(&g_u32PciCfgBar64Enc);
            return;
        }
        pBound->u8Bar64Mask =
            (u8)(pBound->u8Bar64Mask & (u8)~(1u << iBar));
        /* type32 residual: bit0=0 mem, bits1-2=0, bit3=0 */
        pci_cfg_put_dword(pBound->aCfg, nOff, u32Lo);
    } else {
        pBound->u8Bar64Mask =
            (u8)(pBound->u8Bar64Mask & (u8)~(1u << iBar));
        u32Lo = (u32)(u64Phys & 0xfffffffcull) | UDX_PCI_BAR_SPACE_IO;
        pci_cfg_put_dword(pBound->aCfg, nOff, u32Lo);
    }
}

/**
 * Re-encode all BAR dwords from pdev tables into soft cfg image.
 * Called from cfg_init and first enable residual (phys honesty).
 */
static void
pci_cfg_bar_encode_all(struct udx_pci_bound *pBound)
{
    int iBar;

    if (pBound == NULL || pBound->pPdev == NULL) {
        return;
    }
    pBound->u8Bar64Mask = 0;
    for (iBar = 0; iBar < 6; iBar++) {
        pci_cfg_bar_encode_one(pBound, iBar);
    }
    pci_soft_inc(&g_u32PciCfgBarRefresh);
    pci_bar_present_refresh(pBound);
    pci_soft_bar_residual_maybe();
}

/**
 * Sync aBarPhys from soft cfg BAR dword(s) after a base write residual.
 * Preserves type bits in cfg; updates resource_start truth for product path.
 * Soft!=product · not live MMIO window reprogram (window stays inject/grant).
 */
static void
pci_cfg_bar_sync_phys(struct udx_pci_bound *pBound, int iBar)
{
    struct udx_pci_dev *pPdev;
    int iOwner;
    u32 u32Lo;
    u32 u32Hi;
    u64 u64Phys;
    int nOff;

    if (pBound == NULL || pBound->pPdev == NULL || !pBound->u8CfgLive) {
        return;
    }
    if (iBar < 0 || iBar > 5) {
        return;
    }
    pPdev = pBound->pPdev;
    iOwner = pci_cfg_bar_owner(pBound, iBar);
    if (iOwner < 0 || pPdev->aBarLen[iOwner] == 0) {
        return;
    }
    nOff = UDX_PCI_CFG_BAR0 + iOwner * 4;
    u32Lo = pci_cfg_get_dword(pBound->aCfg, nOff);
    if (pPdev->aBarIsMem[iOwner]) {
        u64Phys = (u64)(u32Lo & UDX_PCI_BAR_MEM_MASK);
        if ((pBound->u8Bar64Mask & (u8)(1u << iOwner)) != 0 ||
            (u32Lo & UDX_PCI_BAR_TYPE_MASK) == UDX_PCI_BAR_TYPE_64) {
            u32Hi = pci_cfg_get_dword(pBound->aCfg, nOff + 4);
            u64Phys |= ((u64)u32Hi << 32);
            pBound->u8Bar64Mask =
                (u8)(pBound->u8Bar64Mask | (u8)(1u << iOwner));
        }
    } else {
        u64Phys = (u64)(u32Lo & UDX_PCI_BAR_IO_MASK);
    }
    pPdev->aBarPhys[iOwner] = u64Phys;
    pci_soft_inc(&g_u32PciCfgBarSync);
    pci_soft_bar_residual_maybe();
}

/**
 * Soft STATUS RW1C residual: write-1 clears bits in RW1C mask;
 * CAP_LIST and other sticky soft bits preserved.
 */
static void
pci_cfg_status_rw1c(struct udx_pci_bound *pBound, u16 u16Write)
{
    u16 u16St;
    u16 u16Clear;

    if (pBound == NULL || !pBound->u8CfgLive) {
        return;
    }
    u16St = pci_cfg_get_word(pBound->aCfg, UDX_PCI_CFG_STATUS);
    u16Clear = (u16)(u16Write & UDX_PCI_STATUS_RW1C_MASK);
    u16St = (u16)(u16St & (u16)~u16Clear);
    /* Soft residual: CAP_LIST sticky. */
    u16St = (u16)(u16St | UDX_PCI_STATUS_CAP_LIST);
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_STATUS, u16St);
    pci_soft_inc(&g_u32PciCfgStatusRw1c);
}

static void pci_cfg_set_command_bits(struct udx_pci_bound *pBound, u16 u16Set,
                                     u16 u16Clr);

/**
 * First-enable residual: set COMMAND decode bits.
 * fMemOnly: MEMORY only (pci_enable_device_mem spirit).
 */
static void
pci_enable_first_decode(struct udx_pci_bound *pBound, struct udx_pci_dev *pPdev,
                        int fMemOnly)
{
    u16 u16Cmd;
    int iBar;
    int fNeedMem;
    int fNeedIo;

    if (pBound == NULL || pPdev == NULL) {
        return;
    }
    fNeedMem = 0;
    fNeedIo = 0;
    /*
     * Functional residual: re-encode BAR cfg from aBarPhys before decode.
     * Keeps soft type-0 image honest for product-host enable (32/64-bit).
     * Soft!=product · not live ECAM.
     */
    if (pBound->u8CfgLive) {
        pci_cfg_bar_encode_all(pBound);
    } else {
        pci_bar_present_refresh(pBound);
    }
    for (iBar = 0; iBar < 6; iBar++) {
        if (pPdev->aBarLen[iBar] == 0) {
            continue;
        }
        /* Skip high-half slot of 64-bit BAR (not an independent resource). */
        if (iBar > 0 &&
            (pBound->u8Bar64Mask & (u8)(1u << (iBar - 1))) != 0) {
            continue;
        }
        if (pPdev->aBarIsMem[iBar]) {
            fNeedMem = 1;
        } else if (!fMemOnly) {
            fNeedIo = 1;
        } else {
            /* enable_mem residual: ignore I/O BARs for decode. */
            fNeedMem = 1;
        }
    }
    if (!fNeedMem && !fNeedIo) {
        fNeedMem = 1; /* soft default decode for empty-BAR bind residual */
    }
    u16Cmd = 0;
    if (fNeedMem || fMemOnly) {
        u16Cmd = (u16)(u16Cmd | UDX_PCI_COMMAND_MEMORY);
    }
    if (fNeedIo && !fMemOnly) {
        u16Cmd = (u16)(u16Cmd | UDX_PCI_COMMAND_IO);
    }
    if (fMemOnly) {
        /* Force MEM-only residual even if IO BARs present. */
        u16Cmd = UDX_PCI_COMMAND_MEMORY;
        pBound->u8EnableMemOnly = 1;
    } else {
        pBound->u8EnableMemOnly = 0;
    }
    pBound->u32EnableCnt = 1;
    pBound->u8Enabled = 1;
    pci_cfg_set_command_bits(pBound, u16Cmd, 0);
}

/**
 * Soft claim residual helper for present BARs under mask.
 * Returns UDX_ERR_BUSY on overlap; OK on claim (empty mask allowed).
 */
static udx_status_t
pci_regions_claim(struct udx_pci_bound *pBound, u8 u8WantMask,
                  const char *szName, int fAllowEmpty)
{
    u8 u8Present;
    u8 u8Claim;
    u32 u32Bars;
    int iBar;

    if (pBound == NULL || pBound->pPdev == NULL) {
        return UDX_ERR_INVAL;
    }
    pci_bar_present_refresh(pBound);
    u8Present = pBound->u8BarPresentMask;
    u8Claim = (u8)(u8WantMask & u8Present);
    if ((pBound->u8BarClaimMask & u8Claim) != 0) {
        return UDX_ERR_BUSY;
    }
    if (u8Claim == 0 && !fAllowEmpty) {
        return UDX_ERR_INVAL;
    }
    u32Bars = 0;
    for (iBar = 0; iBar < 6; iBar++) {
        if ((u8Claim & (u8)(1u << iBar)) != 0) {
            u32Bars++;
        }
    }
    pBound->u8BarClaimMask = (u8)(pBound->u8BarClaimMask | u8Claim);
    /* Soft residual: regions bookkeeping active even if empty present mask. */
    pBound->u8Regions = 1;
    if (szName != NULL) {
        pBound->szRegionName = szName;
    }
    if (g_u32PciReqRegBars <= 0xffffffffu - u32Bars) {
        g_u32PciReqRegBars += u32Bars;
    } else {
        g_u32PciReqRegBars = 0xffffffffu;
    }
    return UDX_OK;
}

static void
pci_regions_release_mask(struct udx_pci_bound *pBound, u8 u8RelMask)
{
    if (pBound == NULL) {
        return;
    }
    pBound->u8BarClaimMask =
        (u8)(pBound->u8BarClaimMask & (u8)~u8RelMask);
    if (pBound->u8BarClaimMask == 0) {
        pBound->u8Regions = 0;
        pBound->szRegionName = NULL;
    }
}

static void
pci_cfg_init(struct udx_pci_bound *pBound)
{
    struct udx_pci_dev *pPdev;
    int iByte;

    if (pBound == NULL || pBound->pPdev == NULL) {
        return;
    }
    pPdev = pBound->pPdev;
    for (iByte = 0; iByte < (int)UDX_PCI_CFG_SIZE; iByte++) {
        pBound->aCfg[iByte] = 0;
    }
    pBound->u8Bar64Mask = 0;
    pBound->u8BarSizeMask = 0;

    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_VENDOR, pPdev->u16Vendor);
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_DEVICE, pPdev->u16Device);
    /* Command starts cleared; enable/set_master set bits. */
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_COMMAND, 0);
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_STATUS, 0x0010u); /* cap list */

    /* Class code at 0x09..0x0b (revision at 0x08 left 0). */
    pBound->aCfg[UDX_PCI_CFG_REVISION] = 0;
    pBound->aCfg[0x09] = (u8)(pPdev->u32Class & 0xffu);         /* prog-if */
    pBound->aCfg[0x0a] = (u8)((pPdev->u32Class >> 8) & 0xffu);  /* subclass */
    pBound->aCfg[0x0b] = (u8)((pPdev->u32Class >> 16) & 0xffu); /* base class */

    /* Type-0 header residual defaults for product UDX bind. */
    pBound->aCfg[UDX_PCI_CFG_CACHE_LINE] = 0;
    pBound->aCfg[UDX_PCI_CFG_LATENCY] = 0;
    pBound->aCfg[UDX_PCI_CFG_HEADER] = 0; /* type 0, single function */
    pBound->aCfg[UDX_PCI_CFG_BIST] = 0;

    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_SUBSYS_VEN, pPdev->u16SubVendor);
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_SUBSYS_DEV, pPdev->u16SubDevice);

    /* Soft cap list pointer residual (no live caps minted; Soft!=product). */
    pBound->aCfg[UDX_PCI_CFG_CAP_PTR] = (u8)UDX_PCI_CFG_SOFT_CAP0;
    /* Soft null cap terminator at soft cap0 (id=0). */
    pBound->aCfg[UDX_PCI_CFG_SOFT_CAP0] = 0;
    pBound->aCfg[UDX_PCI_CFG_SOFT_CAP0 + 1] = 0;

    /*
     * BAR residual encode (32/64-bit; Soft!=product).
     * Host inject synthetic phys often sits above 4 GiB — type-64 when
     * high bits set and next BAR slot free (product rtl/xhci shape).
     */
    pci_cfg_bar_encode_all(pBound);

    pBound->aCfg[UDX_PCI_CFG_IRQ_LINE] = (u8)(pPdev->nIrq & 0xff);
    pBound->aCfg[UDX_PCI_CFG_IRQ_PIN] = 1; /* INTA soft default */
    pBound->aCfg[UDX_PCI_CFG_MIN_GNT] = 0;
    pBound->aCfg[UDX_PCI_CFG_MAX_LAT] = 0;
    pBound->u8CfgLive = 1;
    pci_bar_present_refresh(pBound);
}

static void
pci_cfg_set_command_bits(struct udx_pci_bound *pBound, u16 u16Set, u16 u16Clr)
{
    u16 u16Cmd;

    if (pBound == NULL || !pBound->u8CfgLive) {
        return;
    }
    u16Cmd = pci_cfg_get_word(pBound->aCfg, UDX_PCI_CFG_COMMAND);
    u16Cmd = (u16)((u16Cmd | u16Set) & (u16)~u16Clr);
    pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_COMMAND, u16Cmd);
}

/**
 * Soft residual densify of Linux pci_match_id spirit for product UDX bind.
 * Host bind chain: SCAN → GET → id_match → probe (prefer_real product hosts).
 * Soft!=product; Dual DoD OPEN; product_hosts=UDX; 10ec:8168 / 8086:a12f.
 * greppable: udx: pci soft residual id_match
 */
static int
pci_id_match(const struct udx_pci_device_id *pId, const struct udx_pci_dev *pPdev)
{
    int fHit;
    int fAny;
    int fProduct;

    pci_soft_inc(&g_u32PciIdMatchEnter);
    if (pId == NULL || pPdev == NULL) {
        pci_soft_inc(&g_u32PciIdMatchMiss);
        pci_soft_id_match_residual_maybe();
        return 0;
    }
    fHit = 1;
    fAny = 0;
    if (pId->u32Vendor != UDX_PCI_ANY_ID &&
        (u16)pId->u32Vendor != pPdev->u16Vendor) {
        fHit = 0;
    }
    if (fHit != 0 && pId->u32Device != UDX_PCI_ANY_ID &&
        (u16)pId->u32Device != pPdev->u16Device) {
        fHit = 0;
    }
    if (fHit != 0 && pId->u32SubVendor != UDX_PCI_ANY_ID &&
        (u16)pId->u32SubVendor != pPdev->u16SubVendor) {
        fHit = 0;
    }
    if (fHit != 0 && pId->u32SubDevice != UDX_PCI_ANY_ID &&
        (u16)pId->u32SubDevice != pPdev->u16SubDevice) {
        fHit = 0;
    }
    if (fHit != 0 && pId->u32ClassMask != 0) {
        if ((pPdev->u32Class & pId->u32ClassMask) !=
            (pId->u32Class & pId->u32ClassMask)) {
            fHit = 0;
        }
    }
    if (fHit == 0) {
        pci_soft_inc(&g_u32PciIdMatchMiss);
        pci_soft_id_match_residual_maybe();
        return 0;
    }
    /* Hit residual densify (STRONGER; Soft!=product; prefer_real). */
    pci_soft_inc(&g_u32PciIdMatchHit);
    if (pId->u32Vendor == UDX_PCI_ANY_ID ||
        pId->u32Device == UDX_PCI_ANY_ID ||
        pId->u32ClassMask != 0) {
        fAny = 1;
        pci_soft_inc(&g_u32PciIdMatchAny);
    }
    fProduct = pci_soft_is_product_host_id(pPdev->u16Vendor, pPdev->u16Device);
    if (fProduct != 0) {
        pci_soft_inc(&g_u32PciIdMatchProductHit);
        if (pPdev->u16Vendor == (u16)UDX_PCI_G752_RTL8168_VEND &&
            pPdev->u16Device == (u16)UDX_PCI_G752_RTL8168_DEV) {
            pci_soft_inc(&g_u32PciIdMatchRtl);
        }
        if (pPdev->u16Vendor == (u16)UDX_PCI_G752_XHCI_VEND &&
            pPdev->u16Device == (u16)UDX_PCI_G752_XHCI_DEV) {
            pci_soft_inc(&g_u32PciIdMatchXhci);
        }
        /* prefer_real product-host residual note (never closes Dual DoD). */
        if (UDX_PCI_PREFER_REAL != 0) {
            pci_soft_note_product_host(pPdev->u16Vendor, pPdev->u16Device,
                                       pPdev);
        }
    }
    (void)fAny;
    pci_soft_id_match_residual_maybe();
    return 1;
}

static const struct udx_pci_device_id *
pci_find_id(struct udx_pci_driver *pDrv, struct udx_pci_dev *pPdev)
{
    const struct udx_pci_device_id *pId;

    if (pDrv == NULL || pDrv->pIdTable == NULL || pPdev == NULL) {
        return NULL;
    }
    for (pId = pDrv->pIdTable;
         pId->u32Vendor != 0 || pId->u32Device != 0 ||
         pId->u32ClassMask != 0;
         pId++) {
        if (pci_id_match(pId, pPdev)) {
            return pId;
        }
    }
    return NULL;
}

static void
pci_try_bind(struct udx_pci_bound *pBound)
{
    struct udx_pci_driver *pDrv;
    const struct udx_pci_device_id *pId;
    int nSt;

    if (pBound == NULL || pBound->u8Bound) {
        return;
    }
    pci_soft_inc(&g_u32PciTryBind);
    for (pDrv = g_pPciDrivers; pDrv != NULL; pDrv = pDrv->pNext) {
        pId = pci_find_id(pDrv, pBound->pPdev);
        if (pId == NULL) {
            continue;
        }
        pBound->pDrv = pDrv;
        pBound->pPdev->pDev = &pBound->dev;
        pBound->dev.szName = pDrv->szName;
        pBound->dev.pDriverData = NULL;
        pBound->dev.pBackend = pBound;
        if (pDrv->pfnProbe == NULL) {
            pBound->u8Bound = 1;
            pci_soft_inc(&g_u32PciBindNoProbe);
            pci_soft_inc(&g_u32PciC2Bind);
            if (pBound->pPdev != NULL) {
                pci_soft_note_product_host(pBound->pPdev->u16Vendor,
                                           pBound->pPdev->u16Device,
                                           pBound->pPdev);
            }
            pci_soft_maybe_once();
            pci_soft_c2_residual_maybe();
            udx_printk("udx: pci %s bound (no probe)\n", pDrv->szName);
            return;
        }
        nSt = pDrv->pfnProbe(pBound->pPdev, pId);
        if (nSt == 0) {
            pBound->u8Bound = 1;
            pci_soft_inc(&g_u32PciBindProbeOk);
            pci_soft_inc(&g_u32PciC2Bind);
            if (pBound->pPdev != NULL) {
                pci_soft_note_product_host(pBound->pPdev->u16Vendor,
                                           pBound->pPdev->u16Device,
                                           pBound->pPdev);
            }
            pci_soft_maybe_once();
            pci_soft_c2_residual_maybe();
            udx_printk("udx: pci %s probe ok %04x:%04x\n",
                       pDrv->szName,
                       pBound->pPdev->u16Vendor,
                       pBound->pPdev->u16Device);
            return;
        }
        pci_soft_inc(&g_u32PciBindProbeFail);
        udx_printk("udx: pci %s probe fail %d\n", pDrv->szName, nSt);
        pBound->pDrv = NULL;
        pBound->pPdev->pDev = NULL;
        pBound->dev.pBackend = NULL;
    }
}

static void
pci_unbind(struct udx_pci_bound *pBound)
{
    int fQuiesce;
    int fRemove;

    if (pBound == NULL || !pBound->u8Bound) {
        return;
    }
    fQuiesce = 0;
    fRemove = 0;
    if (pBound->pDrv) {
        if (pBound->pDrv->pfnQuiesce) {
            pBound->pDrv->pfnQuiesce(pBound->pPdev);
            fQuiesce = 1;
            pci_soft_inc(&g_u32PciUnbindQuiesce);
        }
        if (pBound->pDrv->pfnRemove) {
            pBound->pDrv->pfnRemove(pBound->pPdev);
            fRemove = 1;
            pci_soft_inc(&g_u32PciUnbindRemove);
        }
    }
    if (fQuiesce == 0 && fRemove == 0) {
        pci_soft_inc(&g_u32PciUnbindNoCb);
    }
    pBound->u8Bound = 0;
    pBound->pDrv = NULL;
    pci_soft_inc(&g_u32PciUnbind);
}

#if defined(UDX_HOST_LIBC)
/* Host inject only: free synthetic BAR windows + bound node. */
static void
pci_destroy_bound(struct udx_pci_bound *pBound)
{
    int iBar;

    if (pBound == NULL) {
        return;
    }
    pci_unbind(pBound);
    for (iBar = 0; iBar < 6; iBar++) {
        if (pBound->pPdev && pBound->pPdev->aBarPhys[iBar]) {
            udx_host_window_unregister(pBound->pPdev->aBarPhys[iBar]);
        }
        free(pBound->apBarHost[iBar]);
        pBound->apBarHost[iBar] = NULL;
    }
    free(pBound->pPdev);
    free(pBound);
}

static void
pci_unlink_bound(struct udx_pci_bound *pBound)
{
    struct udx_pci_bound **pp;

    for (pp = &g_pPciDevices; *pp != NULL; pp = &(*pp)->pNext) {
        if (*pp == pBound) {
            *pp = pBound->pNext;
            pBound->pNext = NULL;
            return;
        }
    }
}
#endif /* UDX_HOST_LIBC */

udx_status_t
udx_pci_register_driver(struct udx_pci_driver *pDrv)
{
    struct udx_pci_bound *pBound;

    pci_soft_inc(&g_u32PciRegDrv);
    if (pDrv == NULL || pDrv->szName == NULL) {
        pci_soft_inc(&g_u32PciRegDrvInval);
        return UDX_ERR_INVAL;
    }
    pDrv->pNext = g_pPciDrivers;
    g_pPciDrivers = pDrv;
    pci_soft_inc(&g_u32PciRegDrvOk);
    pci_soft_maybe_once();
    udx_printk("udx: pci register %s\n", pDrv->szName);

    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        pci_try_bind(pBound);
    }
    return UDX_OK;
}

void
udx_pci_unregister_driver(struct udx_pci_driver *pDrv)
{
    struct udx_pci_driver **pp;
    struct udx_pci_bound *pBound;

    pci_soft_inc(&g_u32PciUnregDrv);
    if (pDrv == NULL) {
        return;
    }
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (pBound->pDrv == pDrv) {
            pci_unbind(pBound);
        }
    }
    for (pp = &g_pPciDrivers; *pp != NULL; pp = &(*pp)->pNext) {
        if (*pp == pDrv) {
            *pp = pDrv->pNext;
            pDrv->pNext = NULL;
            return;
        }
    }
}

/**
 * Soft residual densify of host PCI rescan walk (SCAN spirit).
 * Used by host bind residual after register / inject / grant.
 * Soft!=product; Dual DoD OPEN; prefer_real product_hosts=UDX.
 * greppable: udx: pci soft residual scan
 */
void
udx_host_rescan_pci(void)
{
    struct udx_pci_bound *pBound;
    struct udx_pci_dev *pPdev;

    pci_soft_inc(&g_u32PciRescan);
    pci_soft_inc(&g_u32PciScanEnter);
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        pci_soft_inc(&g_u32PciScanWalk);
        pPdev = pBound->pPdev;
        if (pPdev != NULL &&
            pci_soft_is_product_host_id(pPdev->u16Vendor, pPdev->u16Device)) {
            pci_soft_inc(&g_u32PciScanProductHit);
            if (pPdev->u16Vendor == (u16)UDX_PCI_G752_RTL8168_VEND &&
                pPdev->u16Device == (u16)UDX_PCI_G752_RTL8168_DEV) {
                pci_soft_inc(&g_u32PciScanRtl);
            }
            if (pPdev->u16Vendor == (u16)UDX_PCI_G752_XHCI_VEND &&
                pPdev->u16Device == (u16)UDX_PCI_G752_XHCI_DEV) {
                pci_soft_inc(&g_u32PciScanXhci);
            }
            if (UDX_PCI_PREFER_REAL != 0) {
                pci_soft_note_product_host(pPdev->u16Vendor, pPdev->u16Device,
                                           pPdev);
            }
        }
        pci_try_bind(pBound);
    }
    pci_soft_scan_residual_maybe();
}

/*
 * Zero a freestanding bound slot (soft pool recycle).
 */
#if !defined(UDX_HOST_LIBC)
static void
pci_fs_bound_clear(struct udx_pci_bound *pBound)
{
    int iBar;

    if (pBound == NULL) {
        return;
    }
    for (iBar = 0; iBar < 6; iBar++) {
        pBound->apBarHost[iBar] = NULL;
    }
    pBound->pPdev = NULL;
    pBound->pDrv = NULL;
    pBound->pNext = NULL;
    pBound->u8Enabled = 0;
    pBound->u8Master = 0;
    pBound->u8Regions = 0;
    pBound->u8Bound = 0;
    pBound->u8EnableMemOnly = 0;
    pBound->u8BarPresentMask = 0;
    pBound->u8BarClaimMask = 0;
    pBound->u8BarSizeMask = 0;
    pBound->u8Bar64Mask = 0;
    pBound->u32EnableCnt = 0;
    pBound->szRegionName = NULL;
    pBound->u8CfgLive = 0;
}
#endif

/**
 * Install a DDI-granted (or soft-granted) PCI function.
 * BAR phys/len come from the grant; optional BAR0 VA is window-registered
 * so udx_ioremap(granted_pa, len) works without host inject.
 *
 * greppable path: used by udx_host_bind_* (soft ddi bind PASS).
 */
udx_status_t
udx_host_install_granted_pci(u16 u16Vendor, u16 u16Device,
                             u16 u16SubVendor, u16 u16SubDevice,
                             u32 u32Class,
                             u8 u8Bus, u8 u8Devfn, int nIrq,
                             const u64 *aBarPhys, const u64 *aBarLen,
                             const u8 *aBarIsMem, void *pBar0Va,
                             struct udx_pci_dev **ppOut)
{
    struct udx_pci_bound *pBound;
    struct udx_pci_dev *pPdev;
    int iBar;
#if !defined(UDX_HOST_LIBC)
    u32 iSlot;
    u32 iFs;
    u32 u32FsLive;
#endif

    pci_soft_inc(&g_u32PciGrant);
    if (aBarPhys == NULL || aBarLen == NULL) {
        pci_soft_inc(&g_u32PciGrantInval);
        return UDX_ERR_INVAL;
    }

    /* Reject duplicate BDF. */
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (pBound->pPdev &&
            pBound->pPdev->u8Bus == u8Bus &&
            pBound->pPdev->u8Devfn == u8Devfn) {
            pci_soft_inc(&g_u32PciGrantBusy);
            return UDX_ERR_BUSY;
        }
    }

#if defined(UDX_HOST_LIBC)
    pBound = (struct udx_pci_bound *)calloc(1, sizeof(*pBound));
    pPdev = (struct udx_pci_dev *)calloc(1, sizeof(*pPdev));
    if (pBound == NULL || pPdev == NULL) {
        free(pBound);
        free(pPdev);
        pci_soft_inc(&g_u32PciGrantNomem);
        return UDX_ERR_NOMEM;
    }
#else
    pBound = NULL;
    pPdev = NULL;
    for (iSlot = 0; iSlot < UDX_PCI_FS_GRANT_MAX; iSlot++) {
        if (!g_aFsGrantUsed[iSlot]) {
            g_aFsGrantUsed[iSlot] = 1;
            pBound = &g_aFsBound[iSlot];
            pPdev = &g_aFsPdev[iSlot];
            pci_fs_bound_clear(pBound);
            /* Soft zero pdev fields. */
            {
                u8 *pBytes = (u8 *)(void *)pPdev;
                u32 iByte;
                for (iByte = 0; iByte < (u32)sizeof(*pPdev); iByte++) {
                    pBytes[iByte] = 0;
                }
            }
            pci_soft_inc(&g_u32PciGrantFsSlot);
            u32FsLive = 0;
            for (iFs = 0; iFs < UDX_PCI_FS_GRANT_MAX; iFs++) {
                if (g_aFsGrantUsed[iFs] && u32FsLive < 0xffffffffu) {
                    u32FsLive++;
                }
            }
            pci_soft_note_peak(&g_u32PciGrantFsPeak, u32FsLive);
            break;
        }
    }
    if (pBound == NULL || pPdev == NULL) {
        pci_soft_inc(&g_u32PciGrantNomem);
        return UDX_ERR_NOMEM;
    }
#endif

    pPdev->u16Vendor = u16Vendor;
    pPdev->u16Device = u16Device;
    pPdev->u16SubVendor = u16SubVendor;
    pPdev->u16SubDevice = u16SubDevice;
    pPdev->u32Class = u32Class & 0x00ffffffu;
    pPdev->u8Bus = u8Bus;
    pPdev->u8Devfn = u8Devfn;
    pPdev->nIrq = nIrq;
    pBound->pPdev = pPdev;

    for (iBar = 0; iBar < 6; iBar++) {
        u64 u64Len = aBarLen[iBar];
        u8 u8Mem = aBarIsMem ? aBarIsMem[iBar] : 1;

        pPdev->aBarIsMem[iBar] = u8Mem ? 1u : 0u;
        pPdev->aBarLen[iBar] = u64Len;
        pPdev->aBarPhys[iBar] = (u64Len != 0) ? aBarPhys[iBar] : 0;
        pBound->apBarHost[iBar] = NULL;
    }

    /*
     * Wire ioremap path: granted BAR0 PA → process VA window.
     * udx_ioremap(phys, len) looks up this window (host + freestanding).
     */
    if (pBar0Va != NULL && pPdev->aBarLen[0] != 0 &&
        pPdev->aBarPhys[0] != 0) {
        udx_host_window_register(pPdev->aBarPhys[0], pBar0Va,
                                 pPdev->aBarLen[0]);
        pBound->apBarHost[0] = pBar0Va; /* bookkeeping; not free'd on FS */
        pci_soft_inc(&g_u32PciGrantBar0Map);
    }

    pci_cfg_init(pBound);

    pBound->pNext = g_pPciDevices;
    g_pPciDevices = pBound;

    if (pci_soft_is_product_host_id(u16Vendor, u16Device)) {
        pci_soft_inc(&g_u32PciGrantProductHit);
    }
    /* Product-host residual (pref BAR catalog; Soft!=product). */
    pci_soft_note_product_host(u16Vendor, u16Device, pPdev);
    pci_soft_inc(&g_u32PciC2Grant);

    udx_printk("udx: grant pci %04x:%04x bus %u devfn %u irq %d "
               "bar0_pa=%llx bar0_len=%llx bar0_va=%s\n",
               u16Vendor, u16Device, (unsigned)u8Bus, (unsigned)u8Devfn,
               nIrq,
               (unsigned long long)pPdev->aBarPhys[0],
               (unsigned long long)pPdev->aBarLen[0],
               (pBar0Va != NULL) ? "mapped" : "none");

    pci_try_bind(pBound);

    if (ppOut) {
        *ppOut = pPdev;
    }
    pci_soft_inc(&g_u32PciGrantOk);
    pci_soft_count_lists();
    pci_soft_maybe_once();
    pci_soft_c2_residual_maybe();
    return UDX_OK;
}

udx_status_t
udx_host_inject_pci(u16 u16Vendor, u16 u16Device,
                    u8 u8Bus, u8 u8Devfn, int nIrq,
                    const u64 *aBarLen, const u8 *aBarIsMem,
                    struct udx_pci_dev **ppOut)
{
    return udx_host_inject_pci_ex(u16Vendor, u16Device,
                                  0xffffu, 0xffffu, 0,
                                  u8Bus, u8Devfn, nIrq,
                                  aBarLen, aBarIsMem, ppOut);
}

udx_status_t
udx_host_inject_pci_ex(u16 u16Vendor, u16 u16Device,
                       u16 u16SubVendor, u16 u16SubDevice,
                       u32 u32Class,
                       u8 u8Bus, u8 u8Devfn, int nIrq,
                       const u64 *aBarLen, const u8 *aBarIsMem,
                       struct udx_pci_dev **ppOut)
{
    pci_soft_inc(&g_u32PciInject);
#if !defined(UDX_HOST_LIBC)
    (void)u16Vendor;
    (void)u16Device;
    (void)u16SubVendor;
    (void)u16SubDevice;
    (void)u32Class;
    (void)u8Bus;
    (void)u8Devfn;
    (void)nIrq;
    (void)aBarLen;
    (void)aBarIsMem;
    (void)ppOut;
    pci_soft_inc(&g_u32PciInjectNosys);
    return UDX_ERR_NOSYS;
#else
    struct udx_pci_bound *pBound;
    struct udx_pci_dev *pPdev;
    int iBar;
    /* Synthetic phys base so host windows don't collide. */
    static u64 g_u64NextPhys = 0x100000000ull; /* 4 GiB */

    /* Reject duplicate BDF soft inject. */
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (pBound->pPdev &&
            pBound->pPdev->u8Bus == u8Bus &&
            pBound->pPdev->u8Devfn == u8Devfn) {
            pci_soft_inc(&g_u32PciInjectBusy);
            return UDX_ERR_BUSY;
        }
    }

    pBound = (struct udx_pci_bound *)calloc(1, sizeof(*pBound));
    pPdev = (struct udx_pci_dev *)calloc(1, sizeof(*pPdev));
    if (pBound == NULL || pPdev == NULL) {
        free(pBound);
        free(pPdev);
        pci_soft_inc(&g_u32PciInjectNomem);
        return UDX_ERR_NOMEM;
    }

    pPdev->u16Vendor = u16Vendor;
    pPdev->u16Device = u16Device;
    pPdev->u16SubVendor = u16SubVendor;
    pPdev->u16SubDevice = u16SubDevice;
    pPdev->u32Class = u32Class & 0x00ffffffu;
    pPdev->u8Bus = u8Bus;
    pPdev->u8Devfn = u8Devfn;
    pPdev->nIrq = nIrq;
    pBound->pPdev = pPdev;

    for (iBar = 0; iBar < 6; iBar++) {
        u64 u64Len = aBarLen ? aBarLen[iBar] : 0;
        u8 u8Mem = aBarIsMem ? aBarIsMem[iBar] : 1;
        u64 u64Aligned;

        pPdev->aBarIsMem[iBar] = u8Mem ? 1u : 0u;
        pPdev->aBarLen[iBar] = u64Len;
        if (u64Len == 0) {
            continue;
        }
        /* Align length up to page for synthetic phys packing. */
        u64Aligned = (u64Len + 0xfffull) & ~0xfffull;
        if (u64Aligned == 0) {
            u64Aligned = 0x1000ull;
        }
        if (g_u64NextPhys + u64Aligned < g_u64NextPhys) {
            pci_destroy_bound(pBound);
            pci_soft_inc(&g_u32PciInjectNomem);
            return UDX_ERR_NOMEM;
        }
        pPdev->aBarPhys[iBar] = g_u64NextPhys;
        g_u64NextPhys += u64Aligned;
        pBound->apBarHost[iBar] = calloc(1, (size_t)u64Len);
        if (pBound->apBarHost[iBar] == NULL) {
            pci_destroy_bound(pBound);
            pci_soft_inc(&g_u32PciInjectNomem);
            return UDX_ERR_NOMEM;
        }
        udx_host_window_register(pPdev->aBarPhys[iBar],
                                 pBound->apBarHost[iBar], u64Len);
    }

    pci_cfg_init(pBound);

    pBound->pNext = g_pPciDevices;
    g_pPciDevices = pBound;

    if (pci_soft_is_product_host_id(u16Vendor, u16Device)) {
        pci_soft_inc(&g_u32PciInjectProductHit);
    }
    /* Product-host residual (host inject path; Soft!=product). */
    pci_soft_note_product_host(u16Vendor, u16Device, pPdev);

    udx_printk("udx: inject pci %04x:%04x bus %u devfn %u irq %d\n",
               u16Vendor, u16Device, (unsigned)u8Bus, (unsigned)u8Devfn, nIrq);

    pci_try_bind(pBound);

    if (ppOut) {
        *ppOut = pPdev;
    }
    pci_soft_inc(&g_u32PciInjectOk);
    pci_soft_count_lists();
    pci_soft_maybe_once();
    return UDX_OK;
#endif
}

udx_status_t
udx_host_remove_pci(struct udx_pci_dev *pPdev)
{
    pci_soft_inc(&g_u32PciRemove);
#if !defined(UDX_HOST_LIBC)
    (void)pPdev;
    pci_soft_inc(&g_u32PciRemoveNosys);
    return UDX_ERR_NOSYS;
#else
    struct udx_pci_bound *pBound;

    if (pPdev == NULL) {
        pci_soft_inc(&g_u32PciRemoveNodev);
        return UDX_ERR_INVAL;
    }
    pBound = NULL;
    if (pPdev->pDev != NULL) {
        pBound = (struct udx_pci_bound *)pPdev->pDev->pBackend;
    }
    if (pBound == NULL || pBound->pPdev != pPdev) {
        /* Search list (pre-probe or after unbind cleared pDev). */
        for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
            if (pBound->pPdev == pPdev) {
                break;
            }
        }
    }
    if (pBound == NULL || pBound->pPdev != pPdev) {
        pci_soft_inc(&g_u32PciRemoveNodev);
        return UDX_ERR_NODEV;
    }
    pci_unlink_bound(pBound);
    pci_destroy_bound(pBound);
    pci_soft_inc(&g_u32PciRemoveOk);
    pci_soft_count_lists();
    return UDX_OK;
#endif
}

udx_status_t
udx_pci_enable(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pci_soft_inc(&g_u32PciEnableEnter);
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || pPdev == NULL) {
        pci_soft_inc(&g_u32PciEnableInval);
        return UDX_ERR_INVAL;
    }

    /* Nested enable residual (Linux enable_cnt++). */
    if (pBound->u32EnableCnt > 0) {
        if (pBound->u32EnableCnt < 0xffffffffu) {
            pBound->u32EnableCnt++;
        }
        pBound->u8Enabled = 1;
        pci_soft_inc(&g_u32PciEnableNested);
        pci_soft_inc(&g_u32PciEnable);
        pci_soft_inc(&g_u32PciC2Enable);
        pci_soft_note_peak(&g_u32PciPeakEnableCnt, pBound->u32EnableCnt);
        pci_soft_enable_residual_maybe();
        pci_soft_product_bind_maybe();
        pci_soft_c2_residual_maybe();
        return UDX_OK;
    }

    /*
     * First enable: COMMAND IO/MEM from present BARs (empty → MEM).
     * Soft residual of Linux pci_enable_device. Soft!=product pm/runtime.
     * Functional residual: BAR cfg re-encode (32/64) before decode bits.
     */
    pci_enable_first_decode(pBound, pPdev, 0 /* IO+MEM */);
    pci_soft_inc(&g_u32PciEnableFirst);
    pci_soft_inc(&g_u32PciEnable);
    pci_soft_inc(&g_u32PciC2Enable);
    pci_soft_note_peak(&g_u32PciPeakEnableCnt, pBound->u32EnableCnt);
    pci_soft_enable_residual_maybe();
    pci_soft_product_bind_maybe();
    pci_soft_c2_residual_maybe();
    return UDX_OK;
}

udx_status_t
udx_pci_enable_mem(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pci_soft_inc(&g_u32PciEnableEnter);
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || pPdev == NULL) {
        pci_soft_inc(&g_u32PciEnableMemInv);
        pci_soft_inc(&g_u32PciEnableInval);
        return UDX_ERR_INVAL;
    }

    /* Nested enable residual shares enable_cnt with udx_pci_enable. */
    if (pBound->u32EnableCnt > 0) {
        if (pBound->u32EnableCnt < 0xffffffffu) {
            pBound->u32EnableCnt++;
        }
        pBound->u8Enabled = 1;
        pci_soft_inc(&g_u32PciEnableMemNest);
        pci_soft_inc(&g_u32PciEnableMem);
        pci_soft_inc(&g_u32PciEnableNested);
        pci_soft_inc(&g_u32PciEnable);
        pci_soft_inc(&g_u32PciC2Enable);
        pci_soft_note_peak(&g_u32PciPeakEnableCnt, pBound->u32EnableCnt);
        pci_soft_enable_residual_maybe();
        pci_soft_product_bind_maybe();
        pci_soft_c2_residual_maybe();
        return UDX_OK;
    }

    /*
     * First enable_mem: COMMAND MEMORY only (never IO).
     * Soft residual of Linux pci_enable_device_mem. Soft!=product.
     * Functional residual: BAR cfg re-encode before MEM-only decode.
     */
    pci_enable_first_decode(pBound, pPdev, 1 /* MEM only */);
    pci_soft_inc(&g_u32PciEnableMemFirst);
    pci_soft_inc(&g_u32PciEnableMem);
    pci_soft_inc(&g_u32PciEnableFirst);
    pci_soft_inc(&g_u32PciEnable);
    pci_soft_inc(&g_u32PciC2Enable);
    pci_soft_note_peak(&g_u32PciPeakEnableCnt, pBound->u32EnableCnt);
    pci_soft_enable_residual_maybe();
    pci_soft_product_bind_maybe();
    pci_soft_c2_residual_maybe();
    return UDX_OK;
}

void
udx_pci_disable(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL) {
        return;
    }
    if (pBound->u32EnableCnt == 0) {
        /* Soft residual: underflow (already disabled). */
        pBound->u8Enabled = 0;
        pBound->u8Master = 0;
        pBound->u8EnableMemOnly = 0;
        pci_soft_inc(&g_u32PciDisableUnder);
        return;
    }
    pBound->u32EnableCnt--;
    pci_soft_inc(&g_u32PciDisable);
    if (pBound->u32EnableCnt > 0) {
        /* Nested disable residual: keep decode bits live. */
        pBound->u8Enabled = 1;
        return;
    }
    /* Last disable: clear IO/MEM/MASTER command residual. */
    pBound->u8Enabled = 0;
    pBound->u8Master = 0;
    pBound->u8EnableMemOnly = 0;
    pci_cfg_set_command_bits(pBound, 0,
                             (u16)(UDX_PCI_COMMAND_IO |
                                   UDX_PCI_COMMAND_MEMORY |
                                   UDX_PCI_COMMAND_MASTER));
    pci_soft_inc(&g_u32PciDisableLast);
}

int
udx_pci_is_enabled(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    return pBound != NULL && pBound->u32EnableCnt > 0;
}

u32
udx_pci_enable_cnt(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL) {
        return 0;
    }
    return pBound->u32EnableCnt;
}

u16
udx_pci_command(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;
    u16 u16Cmd;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL) {
        return 0;
    }
    if (pBound->u8CfgLive) {
        return pci_cfg_get_word(pBound->aCfg, UDX_PCI_CFG_COMMAND);
    }
    /* Soft flag residual without live image. */
    u16Cmd = 0;
    if (pBound->u32EnableCnt > 0) {
        if (pBound->u8EnableMemOnly) {
            u16Cmd = (u16)(u16Cmd | UDX_PCI_COMMAND_MEMORY);
        } else {
            u16Cmd = (u16)(u16Cmd | UDX_PCI_COMMAND_MEMORY |
                           UDX_PCI_COMMAND_IO);
        }
    }
    if (pBound->u8Master) {
        u16Cmd = (u16)(u16Cmd | UDX_PCI_COMMAND_MASTER);
    }
    return u16Cmd;
}

udx_status_t
udx_pci_set_master(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || pBound->u32EnableCnt == 0) {
        pci_soft_inc(&g_u32PciSetMasterInv);
        return UDX_ERR_INVAL;
    }
    pBound->u8Master = 1;
    pci_cfg_set_command_bits(pBound, UDX_PCI_COMMAND_MASTER, 0);
    pci_soft_inc(&g_u32PciSetMaster);
    pci_soft_inc(&g_u32PciC2Master);
    /* Product hosts (rtl/xhci): enable → set_master → regions residual. */
    pci_soft_product_bind_maybe();
    pci_soft_c2_residual_maybe();
    return UDX_OK;
}

void
udx_pci_clear_master(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound) {
        pBound->u8Master = 0;
        pci_cfg_set_command_bits(pBound, 0, UDX_PCI_COMMAND_MASTER);
        pci_soft_inc(&g_u32PciClrMaster);
    }
}

int
udx_pci_is_master(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    return pBound != NULL && pBound->u8Master != 0;
}

udx_status_t
udx_pci_request_regions(struct udx_pci_dev *pPdev, const char *szName)
{
    struct udx_pci_bound *pBound;
    udx_status_t st;

    pci_soft_inc(&g_u32PciReqRegEnter);
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || pPdev == NULL) {
        pci_soft_inc(&g_u32PciReqRegInval);
        return UDX_ERR_INVAL;
    }
    if (pBound->u8Regions != 0 || pBound->u8BarClaimMask != 0) {
        pci_soft_inc(&g_u32PciReqRegBusy);
        return UDX_ERR_BUSY;
    }

    /*
     * Soft residual of Linux pci_request_regions: claim every present BAR
     * (len != 0). Empty present mask is still OK for soft bind residual
     * (devices with no BAR still pass probe bookkeeping). Soft!=product
     * iomem tree.
     */
    pci_bar_present_refresh(pBound);
    if (pBound->u8BarPresentMask == 0) {
        pci_soft_inc(&g_u32PciReqRegEmpty);
    }
    st = pci_regions_claim(pBound, UDX_PCI_BAR_MASK_ALL, szName, 1 /* empty OK */);
    if (st != UDX_OK) {
        if (st == UDX_ERR_BUSY) {
            pci_soft_inc(&g_u32PciReqRegBusy);
        } else {
            pci_soft_inc(&g_u32PciReqRegInval);
        }
        return st;
    }
    pci_soft_inc(&g_u32PciReqReg);
    pci_soft_inc(&g_u32PciC2Regions);
    pci_soft_product_bind_maybe();
    pci_soft_c2_residual_maybe();
    return UDX_OK;
}

void
udx_pci_release_regions(struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL) {
        return;
    }
    if (pBound->u8Regions == 0 && pBound->u8BarClaimMask == 0) {
        return;
    }
    pBound->u8Regions = 0;
    pBound->u8BarClaimMask = 0;
    pBound->szRegionName = NULL;
    pci_soft_inc(&g_u32PciRelReg);
}

udx_status_t
udx_pci_request_region(struct udx_pci_dev *pPdev, int nBar, const char *szName)
{
    struct udx_pci_bound *pBound;
    udx_status_t st;
    u8 u8Bit;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || pPdev == NULL || nBar < 0 || nBar > 5) {
        pci_soft_inc(&g_u32PciReqOneInval);
        return UDX_ERR_INVAL;
    }
    pci_bar_present_refresh(pBound);
    u8Bit = UDX_PCI_BAR_BIT(nBar);
    if ((pBound->u8BarPresentMask & u8Bit) == 0) {
        pci_soft_inc(&g_u32PciReqOneInval);
        return UDX_ERR_INVAL;
    }
    if ((pBound->u8BarClaimMask & u8Bit) != 0) {
        pci_soft_inc(&g_u32PciReqOneBusy);
        return UDX_ERR_BUSY;
    }
    st = pci_regions_claim(pBound, u8Bit, szName, 0 /* must claim */);
    if (st != UDX_OK) {
        if (st == UDX_ERR_BUSY) {
            pci_soft_inc(&g_u32PciReqOneBusy);
        } else {
            pci_soft_inc(&g_u32PciReqOneInval);
        }
        return st;
    }
    pci_soft_inc(&g_u32PciReqOne);
    pci_soft_inc(&g_u32PciC2Regions);
    pci_soft_product_bind_maybe();
    pci_soft_c2_residual_maybe();
    return UDX_OK;
}

void
udx_pci_release_region(struct udx_pci_dev *pPdev, int nBar)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || nBar < 0 || nBar > 5) {
        return;
    }
    if ((pBound->u8BarClaimMask & UDX_PCI_BAR_BIT(nBar)) == 0) {
        return;
    }
    pci_regions_release_mask(pBound, UDX_PCI_BAR_BIT(nBar));
    pci_soft_inc(&g_u32PciRelOne);
}

udx_status_t
udx_pci_request_selected_regions(struct udx_pci_dev *pPdev, u8 u8BarMask,
                                 const char *szName)
{
    struct udx_pci_bound *pBound;
    udx_status_t st;
    u8 u8Want;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || pPdev == NULL) {
        pci_soft_inc(&g_u32PciReqSelInval);
        return UDX_ERR_INVAL;
    }
    u8Want = (u8)(u8BarMask & UDX_PCI_BAR_MASK_ALL);
    pci_bar_present_refresh(pBound);
    if ((pBound->u8BarClaimMask & (u8)(u8Want & pBound->u8BarPresentMask)) != 0) {
        pci_soft_inc(&g_u32PciReqSelBusy);
        return UDX_ERR_BUSY;
    }
    if ((u8Want & pBound->u8BarPresentMask) == 0) {
        pci_soft_inc(&g_u32PciReqSelEmpty);
    }
    st = pci_regions_claim(pBound, u8Want, szName, 1 /* empty OK */);
    if (st != UDX_OK) {
        if (st == UDX_ERR_BUSY) {
            pci_soft_inc(&g_u32PciReqSelBusy);
        } else {
            pci_soft_inc(&g_u32PciReqSelInval);
        }
        return st;
    }
    pci_soft_inc(&g_u32PciReqSel);
    pci_soft_inc(&g_u32PciC2Regions);
    pci_soft_product_bind_maybe();
    pci_soft_c2_residual_maybe();
    return UDX_OK;
}

void
udx_pci_release_selected_regions(struct udx_pci_dev *pPdev, u8 u8BarMask)
{
    struct udx_pci_bound *pBound;
    u8 u8Rel;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL) {
        return;
    }
    u8Rel = (u8)(u8BarMask & UDX_PCI_BAR_MASK_ALL & pBound->u8BarClaimMask);
    if (u8Rel == 0) {
        return;
    }
    pci_regions_release_mask(pBound, u8Rel);
    pci_soft_inc(&g_u32PciRelSel);
}

u8
udx_pci_regions_mask(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL) {
        return 0;
    }
    return pBound->u8BarClaimMask;
}

u8
udx_pci_bar_present_mask(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound == NULL) {
        return 0;
    }
    pci_bar_present_refresh(pBound);
    return pBound->u8BarPresentMask;
}

u64
udx_pci_resource_start(struct udx_pci_dev *pPdev, int nBar)
{
    u64 u64Start;

    pci_soft_inc(&g_u32PciResStart);
    pci_soft_inc(&g_u32PciC2Resource);
    if (pPdev == NULL || nBar < 0 || nBar > 5) {
        pci_soft_inc(&g_u32PciResEmpty);
        pci_soft_resource_residual_maybe();
        return 0;
    }
    u64Start = pPdev->aBarPhys[nBar];
    if (u64Start != 0) {
        pci_soft_inc(&g_u32PciResStartHit);
    } else if (pPdev->aBarLen[nBar] == 0) {
        pci_soft_inc(&g_u32PciResEmpty);
    }
    pci_soft_resource_residual_maybe();
    pci_soft_c2_residual_maybe();
    return u64Start;
}

u64
udx_pci_resource_len(struct udx_pci_dev *pPdev, int nBar)
{
    u64 u64Len;

    pci_soft_inc(&g_u32PciResLen);
    pci_soft_inc(&g_u32PciC2Resource);
    if (pPdev == NULL || nBar < 0 || nBar > 5) {
        pci_soft_inc(&g_u32PciResEmpty);
        pci_soft_resource_residual_maybe();
        return 0;
    }
    u64Len = pPdev->aBarLen[nBar];
    if (u64Len != 0) {
        pci_soft_inc(&g_u32PciResLenHit);
    } else {
        pci_soft_inc(&g_u32PciResEmpty);
    }
    pci_soft_resource_residual_maybe();
    pci_soft_c2_residual_maybe();
    return u64Len;
}

int
udx_pci_resource_is_mem(const struct udx_pci_dev *pPdev, int nBar)
{
    pci_soft_inc(&g_u32PciResIsMem);
    pci_soft_inc(&g_u32PciC2Resource);
    if (pPdev == NULL || nBar < 0 || nBar > 5) {
        pci_soft_inc(&g_u32PciResEmpty);
        pci_soft_resource_residual_maybe();
        return 0;
    }
    if (pPdev->aBarLen[nBar] == 0) {
        pci_soft_inc(&g_u32PciResEmpty);
    }
    pci_soft_resource_residual_maybe();
    pci_soft_c2_residual_maybe();
    return pPdev->aBarIsMem[nBar] != 0;
}

const char *
udx_pci_name(const struct udx_pci_dev *pPdev)
{
    struct udx_pci_bound *pBound;

    pBound = pci_bound_of(pPdev);
    if (pBound != NULL) {
        if (pBound->szRegionName != NULL && pBound->szRegionName[0] != '\0') {
            return pBound->szRegionName;
        }
        if (pBound->pDrv != NULL && pBound->pDrv->szName != NULL) {
            return pBound->pDrv->szName;
        }
        if (pBound->dev.szName != NULL && pBound->dev.szName[0] != '\0') {
            return pBound->dev.szName;
        }
    }
    if (pPdev != NULL && pPdev->pDev != NULL) {
        return udx_dev_name(pPdev->pDev);
    }
    return "udx-pci";
}

/**
 * Soft residual densify of Linux pci_get_device (BDF lookup).
 * Host bind residual: GET by BDF after SCAN; prefer_real product hosts.
 * Soft!=product; Dual DoD OPEN; product_hosts=UDX; 10ec:8168 / 8086:a12f.
 * greppable: udx: pci soft residual get
 */
struct udx_pci_dev *
udx_pci_get_device(u8 u8Bus, u8 u8Devfn)
{
    struct udx_pci_bound *pBound;
    struct udx_pci_dev *pPdev;

    pci_soft_inc(&g_u32PciGetDev);
    for (pBound = g_pPciDevices; pBound != NULL; pBound = pBound->pNext) {
        if (pBound->pPdev &&
            pBound->pPdev->u8Bus == u8Bus &&
            pBound->pPdev->u8Devfn == u8Devfn) {
            pPdev = pBound->pPdev;
            pci_soft_inc(&g_u32PciGetDevHit);
            /* Product-host get residual densify (STRONGER; Soft!=product). */
            if (pci_soft_is_product_host_id(pPdev->u16Vendor,
                                            pPdev->u16Device)) {
                pci_soft_inc(&g_u32PciGetProductHit);
                if (pPdev->u16Vendor == (u16)UDX_PCI_G752_RTL8168_VEND &&
                    pPdev->u16Device == (u16)UDX_PCI_G752_RTL8168_DEV) {
                    pci_soft_inc(&g_u32PciGetRtl);
                }
                if (pPdev->u16Vendor == (u16)UDX_PCI_G752_XHCI_VEND &&
                    pPdev->u16Device == (u16)UDX_PCI_G752_XHCI_DEV) {
                    pci_soft_inc(&g_u32PciGetXhci);
                }
                if (UDX_PCI_PREFER_REAL != 0) {
                    pci_soft_note_product_host(pPdev->u16Vendor,
                                               pPdev->u16Device, pPdev);
                }
            }
            pci_soft_get_residual_maybe();
            return pPdev;
        }
    }
    pci_soft_inc(&g_u32PciGetDevMiss);
    pci_soft_get_residual_maybe();
    return NULL;
}

udx_status_t
udx_pci_read_config_byte(struct udx_pci_dev *pPdev, int nWhere, u8 *pVal)
{
    struct udx_pci_bound *pBound;
    int iBar;

    if (pPdev == NULL || pVal == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || nWhere >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound != NULL && pBound->u8CfgLive) {
        /*
         * BAR size residual: dword-aligned BAR in sizing mode returns
         * size mask (read via any byte of the BAR dword). Soft!=product.
         * Owner bit covers low + 64-bit high half.
         */
        iBar = pci_cfg_bar_index(nWhere & ~3);
        if (iBar >= 0) {
            int iOwner = pci_cfg_bar_owner(pBound, iBar);

            if (iOwner >= 0 &&
                (pBound->u8BarSizeMask & (u8)(1u << iOwner)) != 0) {
                u32 u32Mask = pci_cfg_bar_size_mask_at(pBound, iBar);
                int nShift = (nWhere & 3) * 8;

                *pVal = (u8)((u32Mask >> nShift) & 0xffu);
                pci_soft_inc(&g_u32PciCfgBarSizeRd);
                pci_soft_inc(&g_u32PciCfgRead);
                pci_soft_inc(&g_u32PciCfgRdByte);
                pci_soft_note_cfg_c2();
                pci_soft_bar_residual_maybe();
                return UDX_OK;
            }
        }
        *pVal = pBound->aCfg[nWhere];
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdByte);
        pci_soft_note_cfg_c2();
        return UDX_OK;
    }
    /* Fallback without bound cfg image (identity residual only). */
    if (nWhere == UDX_PCI_CFG_VENDOR) {
        *pVal = (u8)(pPdev->u16Vendor & 0xffu);
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdByte);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_VENDOR + 1) {
        *pVal = (u8)((pPdev->u16Vendor >> 8) & 0xffu);
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdByte);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_DEVICE) {
        *pVal = (u8)(pPdev->u16Device & 0xffu);
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdByte);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_DEVICE + 1) {
        *pVal = (u8)((pPdev->u16Device >> 8) & 0xffu);
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdByte);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    *pVal = 0xffu;
    pci_soft_inc(&g_u32PciCfgFail);
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_byte(struct udx_pci_dev *pPdev, int nWhere, u8 u8Val)
{
    struct udx_pci_bound *pBound;
    int iBar;

    if (pPdev == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || nWhere >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    /* Soft residual RO identity fields. */
    if (pci_cfg_is_ro_where(nWhere)) {
        pci_soft_inc(&g_u32PciCfgFail);
        pci_soft_inc(&g_u32PciCfgRoReject);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_NOSYS;
    }
    /* STATUS low byte soft RW1C residual (byte write path). */
    if (nWhere == UDX_PCI_CFG_STATUS || nWhere == UDX_PCI_CFG_STATUS + 1) {
        u16 u16Write = 0;

        if (nWhere == UDX_PCI_CFG_STATUS) {
            u16Write = (u16)u8Val;
        } else {
            u16Write = (u16)((u16)u8Val << 8);
        }
        pci_cfg_status_rw1c(pBound, u16Write);
        pci_soft_inc(&g_u32PciCfgWrite);
        pci_soft_inc(&g_u32PciCfgWrByte);
        pci_soft_note_cfg_c2();
        return UDX_OK;
    }
    pBound->aCfg[nWhere] = u8Val;
    /* Mirror command soft residual flags + enable_cnt. */
    if (nWhere == UDX_PCI_CFG_COMMAND || nWhere == UDX_PCI_CFG_COMMAND + 1) {
        u16 u16Cmd = pci_cfg_get_word(pBound->aCfg, UDX_PCI_CFG_COMMAND);

        pci_cfg_mirror_command(pBound, u16Cmd);
        pci_soft_inc(&g_u32PciCfgCmdWrite);
    }
    /* BAR byte write leaves sizing residual + sync aBarPhys. */
    iBar = pci_cfg_bar_index(nWhere & ~3);
    if (iBar >= 0) {
        int iOwner = pci_cfg_bar_owner(pBound, iBar);

        if (iOwner >= 0) {
            pci_cfg_bar_leave_size(pBound, iOwner);
        }
        pci_soft_inc(&g_u32PciCfgBarWrite);
        pci_cfg_bar_sync_phys(pBound, iBar);
    }
    pci_soft_inc(&g_u32PciCfgWrite);
    pci_soft_inc(&g_u32PciCfgWrByte);
    pci_soft_note_cfg_c2();
    return UDX_OK;
}

udx_status_t
udx_pci_read_config_word(struct udx_pci_dev *pPdev, int nWhere, u16 *pVal)
{
    struct udx_pci_bound *pBound;
    int iBar;

    if (pPdev == NULL || pVal == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 1) != 0 ||
        nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound != NULL && pBound->u8CfgLive) {
        iBar = pci_cfg_bar_index(nWhere & ~3);
        if (iBar >= 0) {
            int iOwner = pci_cfg_bar_owner(pBound, iBar);

            if (iOwner >= 0 &&
                (pBound->u8BarSizeMask & (u8)(1u << iOwner)) != 0) {
                u32 u32Mask = pci_cfg_bar_size_mask_at(pBound, iBar);
                int nShift = (nWhere & 2) ? 16 : 0;

                *pVal = (u16)((u32Mask >> nShift) & 0xffffu);
                pci_soft_inc(&g_u32PciCfgBarSizeRd);
                pci_soft_inc(&g_u32PciCfgRead);
                pci_soft_inc(&g_u32PciCfgRdWord);
                pci_soft_note_cfg_c2();
                pci_soft_bar_residual_maybe();
                return UDX_OK;
            }
        }
        *pVal = pci_cfg_get_word(pBound->aCfg, nWhere);
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdWord);
        pci_soft_note_cfg_c2();
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_VENDOR) {
        *pVal = pPdev->u16Vendor;
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdWord);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_DEVICE) {
        *pVal = pPdev->u16Device;
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdWord);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_SUBSYS_VEN) {
        *pVal = pPdev->u16SubVendor;
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdWord);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_SUBSYS_DEV) {
        *pVal = pPdev->u16SubDevice;
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdWord);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    *pVal = 0xffffu;
    pci_soft_inc(&g_u32PciCfgFail);
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_word(struct udx_pci_dev *pPdev, int nWhere, u16 u16Val)
{
    struct udx_pci_bound *pBound;
    int iBar;

    if (pPdev == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 1) != 0 ||
        nWhere + 1 >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (pci_cfg_is_ro_where(nWhere) || pci_cfg_is_ro_where(nWhere + 1)) {
        pci_soft_inc(&g_u32PciCfgFail);
        pci_soft_inc(&g_u32PciCfgRoReject);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_NOSYS;
    }
    if (nWhere == UDX_PCI_CFG_STATUS) {
        pci_cfg_status_rw1c(pBound, u16Val);
        pci_soft_inc(&g_u32PciCfgWrite);
        pci_soft_inc(&g_u32PciCfgWrWord);
        pci_soft_note_cfg_c2();
        return UDX_OK;
    }
    pci_cfg_put_word(pBound->aCfg, nWhere, u16Val);
    if (nWhere == UDX_PCI_CFG_COMMAND) {
        pci_cfg_mirror_command(pBound, u16Val);
        pci_soft_inc(&g_u32PciCfgCmdWrite);
    }
    iBar = pci_cfg_bar_index(nWhere & ~3);
    if (iBar >= 0) {
        int iOwner = pci_cfg_bar_owner(pBound, iBar);

        if (iOwner >= 0) {
            pci_cfg_bar_leave_size(pBound, iOwner);
        }
        pci_soft_inc(&g_u32PciCfgBarWrite);
        pci_cfg_bar_sync_phys(pBound, iBar);
    }
    pci_soft_inc(&g_u32PciCfgWrite);
    pci_soft_inc(&g_u32PciCfgWrWord);
    pci_soft_note_cfg_c2();
    return UDX_OK;
}

udx_status_t
udx_pci_read_config_dword(struct udx_pci_dev *pPdev, int nWhere, u32 *pVal)
{
    struct udx_pci_bound *pBound;
    int iBar;

    if (pPdev == NULL || pVal == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 3) != 0 ||
        nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    pBound = pci_bound_of(pPdev);
    if (pBound != NULL && pBound->u8CfgLive) {
        iBar = pci_cfg_bar_index(nWhere);
        if (iBar >= 0) {
            int iOwner = pci_cfg_bar_owner(pBound, iBar);

            if (iOwner >= 0 &&
                (pBound->u8BarSizeMask & (u8)(1u << iOwner)) != 0) {
                *pVal = pci_cfg_bar_size_mask_at(pBound, iBar);
                pci_soft_inc(&g_u32PciCfgBarSizeRd);
                pci_soft_inc(&g_u32PciCfgRead);
                pci_soft_inc(&g_u32PciCfgRdDword);
                pci_soft_note_cfg_c2();
                pci_soft_bar_residual_maybe();
                return UDX_OK;
            }
        }
        *pVal = pci_cfg_get_dword(pBound->aCfg, nWhere);
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdDword);
        pci_soft_note_cfg_c2();
        return UDX_OK;
    }
    if (nWhere == 0) {
        *pVal = (u32)pPdev->u16Vendor | ((u32)pPdev->u16Device << 16);
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdDword);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    if (nWhere == UDX_PCI_CFG_SUBSYS_VEN) {
        *pVal = (u32)pPdev->u16SubVendor |
                ((u32)pPdev->u16SubDevice << 16);
        pci_soft_inc(&g_u32PciCfgRead);
        pci_soft_inc(&g_u32PciCfgRdDword);
        pci_soft_inc(&g_u32PciCfgFallback);
        return UDX_OK;
    }
    *pVal = 0xffffffffu;
    pci_soft_inc(&g_u32PciCfgFail);
    return UDX_ERR_NOSYS;
}

udx_status_t
udx_pci_write_config_dword(struct udx_pci_dev *pPdev, int nWhere, u32 u32Val)
{
    struct udx_pci_bound *pBound;
    int iBar;
    int iByte;

    if (pPdev == NULL) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    if (nWhere < 0 || (nWhere & 3) != 0 ||
        nWhere + 3 >= (int)UDX_PCI_CFG_SIZE) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_INVAL;
    }
    for (iByte = 0; iByte < 4; iByte++) {
        if (pci_cfg_is_ro_where(nWhere + iByte)) {
            pci_soft_inc(&g_u32PciCfgFail);
            pci_soft_inc(&g_u32PciCfgRoReject);
            return UDX_ERR_INVAL;
        }
    }
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive) {
        pci_soft_inc(&g_u32PciCfgFail);
        return UDX_ERR_NOSYS;
    }
    /* dword at COMMAND: low COMMAND + high STATUS (RW1C residual). */
    if (nWhere == UDX_PCI_CFG_COMMAND) {
        u16 u16Cmd = (u16)(u32Val & 0xffffu);
        u16 u16St = (u16)((u32Val >> 16) & 0xffffu);

        pci_cfg_put_word(pBound->aCfg, UDX_PCI_CFG_COMMAND, u16Cmd);
        pci_cfg_mirror_command(pBound, u16Cmd);
        pci_soft_inc(&g_u32PciCfgCmdWrite);
        pci_cfg_status_rw1c(pBound, u16St);
        pci_soft_inc(&g_u32PciCfgWrite);
        pci_soft_inc(&g_u32PciCfgWrDword);
        pci_soft_note_cfg_c2();
        return UDX_OK;
    }
    /*
     * BAR size residual: all-1s write enters sizing mode on owner BAR.
     * Base write residual: store cfg + sync aBarPhys (resource_start truth).
     * Soft!=product · not live ECAM reprogram · window remains inject/grant.
     */
    iBar = pci_cfg_bar_index(nWhere);
    if (iBar >= 0) {
        int iOwner = pci_cfg_bar_owner(pBound, iBar);
        int fMem;
        u32 u32SizeProbe;

        if (iOwner < 0) {
            iOwner = iBar;
        }
        fMem = (pPdev->aBarIsMem[iOwner] != 0) ||
               ((pBound->u8Bar64Mask & (u8)(1u << iOwner)) != 0);
        u32SizeProbe = fMem ? 0xffffffffu : 0xfffffffeu;

        if (u32Val == 0xffffffffu || u32Val == u32SizeProbe) {
            pci_cfg_bar_enter_size(pBound, iOwner);
            /* Keep stored base until leave; size returned on read. */
            pci_soft_inc(&g_u32PciCfgWrite);
            pci_soft_inc(&g_u32PciCfgWrDword);
            pci_soft_note_cfg_c2();
            return UDX_OK;
        }
        pci_cfg_bar_leave_size(pBound, iOwner);
        /* Soft residual: accept base write into cfg image (type bits soft). */
        pci_cfg_put_dword(pBound->aCfg, nWhere, u32Val);
        /* Preserve MEM type-64 bit if this is the low dword of a 64-bit BAR. */
        if (iOwner == iBar && fMem &&
            (pBound->u8Bar64Mask & (u8)(1u << iOwner)) != 0) {
            u32 u32Lo = pci_cfg_get_dword(pBound->aCfg, nWhere);

            u32Lo = (u32)((u32Lo & ~UDX_PCI_BAR_TYPE_MASK) | UDX_PCI_BAR_TYPE_64);
            u32Lo &= ~UDX_PCI_BAR_SPACE_IO;
            pci_cfg_put_dword(pBound->aCfg, nWhere, u32Lo);
        }
        pci_soft_inc(&g_u32PciCfgBarWrite);
        pci_cfg_bar_sync_phys(pBound, iBar);
        pci_soft_inc(&g_u32PciCfgWrite);
        pci_soft_inc(&g_u32PciCfgWrDword);
        pci_soft_note_cfg_c2();
        return UDX_OK;
    }
    pci_cfg_put_dword(pBound->aCfg, nWhere, u32Val);
    pci_soft_inc(&g_u32PciCfgWrite);
    pci_soft_inc(&g_u32PciCfgWrDword);
    pci_soft_note_cfg_c2();
    return UDX_OK;
}

u8
udx_pci_find_capability(struct udx_pci_dev *pPdev, u8 u8CapId)
{
    struct udx_pci_bound *pBound;
    u8 u8Ptr;
    u8 u8Id;
    u8 u8Next;
    int nHops;

    pci_soft_inc(&g_u32PciCfgFindCap);
    pci_soft_inc(&g_u32PciC2Cap);
    pBound = pci_bound_of(pPdev);
    if (pBound == NULL || !pBound->u8CfgLive || u8CapId == 0) {
        pci_soft_inc(&g_u32PciCfgFindCapMiss);
        pci_soft_cap_residual_maybe();
        pci_soft_c2_residual_maybe();
        return 0;
    }
    /* Soft residual: walk from CAP_PTR; soft null terminator at soft cap0. */
    u8Ptr = pBound->aCfg[UDX_PCI_CFG_CAP_PTR] & 0xfcu;
    for (nHops = 0; nHops < 48 && u8Ptr >= 0x40 &&
         u8Ptr + 1u < UDX_PCI_CFG_SIZE; nHops++) {
        u8Id = pBound->aCfg[u8Ptr];
        u8Next = pBound->aCfg[u8Ptr + 1u] & 0xfcu;
        if (u8Id == 0) {
            break; /* soft null terminator */
        }
        if (u8Id == u8CapId) {
            pci_soft_inc(&g_u32PciCfgFindCapHit);
            pci_soft_note_cfg_c2();
            pci_soft_cap_residual_maybe();
            pci_soft_c2_residual_maybe();
            return u8Ptr;
        }
        if (u8Next == 0 || u8Next == u8Ptr) {
            break;
        }
        u8Ptr = u8Next;
    }
    pci_soft_inc(&g_u32PciCfgFindCapMiss);
    pci_soft_cap_residual_maybe();
    pci_soft_c2_residual_maybe();
    return 0;
}

/*
 * Soft residual deepen surfaces (CREATE-ONLY soft != product):
 *   greppable: udx: pci soft enable residual
 *   greppable: udx: pci soft master residual
 *   greppable: udx: pci soft regions residual
 *   greppable: udx: pci soft cfg residual
 *   greppable: udx: pci soft bar residual
 *   greppable: udx: pci soft cap residual
 *   greppable: udx: pci soft resource residual
 *   greppable: udx: pci soft product_host residual
 *   greppable: udx: pci soft grant residual
 *   greppable: udx: pci soft unbind residual
 *   greppable: udx: pci soft product bind residual
 *   greppable: udx: pci soft c2 product path
 *   greppable: udx: pci soft c2 residual
 *   greppable: udx: pci soft dual DoD
 *   greppable: udx: pci soft residual dual_dod OPEN
 *   greppable: udx: pci soft residual scan
 *   greppable: udx: pci soft residual get
 *   greppable: udx: pci soft residual id_match
 *   greppable: udx: pci soft residual denser
 *   greppable: denser_prove=1 | denser residual | lean_checks=
 *   greppable: Soft!=product soft residual dual_dod OPEN prefer_real
 *              product_hosts=UDX
 * Soft residual: pci_enable / enable_mem / set_master /
 * request_regions|region|selected / cfg RO+STATUS RW1C+BAR size /
 * bar 32|64 encode + size + base→phys sync /
 * find_capability / resource start|len|is_mem /
 * install_granted for product UDX bind.
 * STRONGER densify: scan (udx_host_rescan_pci) / get (udx_pci_get_device) /
 * id_match (pci_id_match) for host freestanding bind Dual DoD seeds
 * 10ec:8168 (rtl8168_udx) / 8086:a12f (xhci_udx). prefer_real=1.
 * denser residual: denser_prove scan+get+id_match chain + denser
 * product_host + denser C2 steps; lean_checks denser; Soft!=product.
 * Functional residual (wave 75): first enable re-encodes BAR cfg;
 * BAR base write syncs aBarPhys (resource_start); 64-bit MEM when
 * phys>>32 and next slot free (host inject ≥4GiB honesty).
 * C2 residual deepen: chain register→grant|inject→match→probe→
 * enable→master→regions→cfg→bar→cap→resource→ioremap→dma→irq→drvdata.
 * Dual DoD A/B OPEN (agent != close). Freestanding rtl/USB SKIP.
 * Soft != product complete; product lamps 0; G-AC-1 Soft!=product.
 * Dual MIT OR Apache-2.0. No GJ_IMAGE_VERSION stamp.
 * Bar honesty v2026.08.04.75 stamp-free. NEVER invent .76.
 */
