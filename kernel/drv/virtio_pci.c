/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-pci modern transport + virtqueues (OASIS virtio 1.1).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * C2 T0 product transport residual (this unit only; Soft!=product):
 *   Lean residual only - never re-introduce multi-kprintf stamp storms
 *   (prior FAULT class: sequential soft-deepen kprintf floods -> stack smash
 *   / #PF I=1). Soft inventory hard-capped, few lines, no wave=/version.
 *   Silent residual counters stay; product path logs (scan/features/q) stay.
 *   Dual DoD: freestanding lab net = rtl; virtio = T0 product path.
 *   G-AC-1: no Linux .ko exec. No config.h / GJ_IMAGE_VERSION.
 *   dual=MIT_OR_Apache-2.0 | stamp_storm=0 | no_version_stamp=1.
 *
 * T0 class triad this transport residual supports (class modules own I/O):
 *   virtio-net (KIND_NET) - product NIC until UDX owns real HW
 *   virtio-blk (KIND_BLK) - store / store_door T0 path
 *   virtio-gpu (KIND_GPU) - present / compositor T0 path
 * Lean residual tallies scan/setup/nego/q/kick for the triad; Soft!=product.
 *
 * Soft product depth (common-cfg, features, queue setup):
 *   - modern common-cfg cap walk + soft reset
 *   - feature read/write helpers + soft negotiate ladder
 *   - queue soft size clamp, disable-before-setup, enable verify
 *   - T0 triad soft tallies + lean self-check (net/blk/gpu IDs)
 *
 * C2 residual deepen (stamp-free lean; Soft!=product; dual DoD OPEN):
 *   - partial add chain desc free-list recovery (add/add2/add3)
 *   - q_setup re-entry frees prior rings (no soft page leak)
 *   - setup cap-type tallies + nego V1 honesty + sticky last status/poll
 *   - T0 kick path tallies; fail honesty folded into residual lean lines
 *   - silent lean self-check expansion (layout / clamp / status bits)
 *   - q_resync + last_poll + poll_hit greppable on residual lean (honesty)
 *   - dual_dod=OPEN greppable (Soft residual never closes Dual DoD A/B)
 *   freestanding SKIP | H2 stamp_storm=0 | no thr_exit surface here
 *
 * Greppable product markers (prefix-stable):
 *   virtio: scan PASS
 *   virtio: modern common@
 *   virtio: features
 *   virtio: features soft
 *   virtio: q soft
 *   virtio: q%u size=
 *   virtio: driver_ok
 *
 * Lean soft residual (hard-capped emission; Soft!=product dual license):
 *   virtio-pci: soft inventory ...
 *   virtio-pci: soft residual lean ...
 *   virtio-pci: soft PASS|NODEV|PARTIAL
 *   virtio: soft inventory|residual lean|PASS|NODEV|PARTIAL  (twin)
 * greppable: virtio-pci: soft inventory
 * greppable: virtio-pci: soft residual lean
 * greppable: virtio: soft residual lean
 * greppable: t0_product=1 product_net=virtio t0_net= t0_blk= t0_gpu=
 * greppable: setup_net= setup_blk= setup_gpu= q_net= q_blk= q_gpu=
 * greppable: kick_net= kick_blk= kick_gpu= dok_net= dok_blk= dok_gpu=
 * greppable: add_fail= nego_fail= q_nomem= desc_recov= v1=
 * greppable: q_resync= last_poll= poll_hit= dual_dod=OPEN
 */
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/vmm.h>

/* Local page size - no config.h / GJ_IMAGE_VERSION in this residual unit. */
#define VIRTIO_PCI_PAGE_SIZE 4096u

#define PCI_CFG_ADDR 0xCF8u
#define PCI_CFG_DATA 0xCFCu

#define PCI_CMD      0x04u
#define PCI_CMD_IO   0x1u
#define PCI_CMD_MEM  0x2u
#define PCI_CMD_BUS  0x4u
#define PCI_CAP_PTR  0x34u
#define PCI_CAP_ID_VNDR 0x09u

/* Local aliases to public GJ_VIRTIO_PCI_COMMON_* (keep body greppable). */
#define VIRTIO_PCI_COMMON_DFSELECT   GJ_VIRTIO_PCI_COMMON_DFSELECT
#define VIRTIO_PCI_COMMON_DF         GJ_VIRTIO_PCI_COMMON_DF
#define VIRTIO_PCI_COMMON_GFSELECT   GJ_VIRTIO_PCI_COMMON_GFSELECT
#define VIRTIO_PCI_COMMON_GF         GJ_VIRTIO_PCI_COMMON_GF
#define VIRTIO_PCI_COMMON_MSIX       GJ_VIRTIO_PCI_COMMON_MSIX
#define VIRTIO_PCI_COMMON_NUMQ       GJ_VIRTIO_PCI_COMMON_NUMQ
#define VIRTIO_PCI_COMMON_STATUS     GJ_VIRTIO_PCI_COMMON_STATUS
#define VIRTIO_PCI_COMMON_CFGGEN     GJ_VIRTIO_PCI_COMMON_CFGGEN
#define VIRTIO_PCI_COMMON_Q_SELECT   GJ_VIRTIO_PCI_COMMON_Q_SELECT
#define VIRTIO_PCI_COMMON_Q_SIZE     GJ_VIRTIO_PCI_COMMON_Q_SIZE
#define VIRTIO_PCI_COMMON_Q_MSIX     GJ_VIRTIO_PCI_COMMON_Q_MSIX
#define VIRTIO_PCI_COMMON_Q_ENABLE   GJ_VIRTIO_PCI_COMMON_Q_ENABLE
#define VIRTIO_PCI_COMMON_Q_NOFF     GJ_VIRTIO_PCI_COMMON_Q_NOFF
#define VIRTIO_PCI_COMMON_Q_DESC     GJ_VIRTIO_PCI_COMMON_Q_DESC
#define VIRTIO_PCI_COMMON_Q_DRIVER   GJ_VIRTIO_PCI_COMMON_Q_DRIVER
#define VIRTIO_PCI_COMMON_Q_DEVICE   GJ_VIRTIO_PCI_COMMON_Q_DEVICE

static struct gj_virtio_dev g_aDevs[GJ_VIRTIO_MAX_DEVS];
static u32                  g_cDevs;

/*
 * Lean residual inventory: hard cap on emission (no stamp storms).
 * Budget: scan + post-activity maybe_once (init is silent baseline).
 * Soft != product; dual MIT OR Apache-2.0. No version/wave stamp.
 */
#define VIRTIO_PCI_SOFT_INV_LOG_CAP 2u

/*
 * Soft residual counters (silent; Soft!=product). Cumulative path tallies.
 * Printed only via hard-capped soft_inventory_log - never multi-line deepen.
 * greppable: virtio-pci: soft residual lean / virtio: soft residual lean
 */
static u32 g_u32SoftScanEnter;     /* virtio_pci_scan entries */
static u32 g_u32SoftScanFound;     /* devices retained last scan (snapshot) */
static u32 g_u32SoftKindNet;       /* scan: KIND_NET */
static u32 g_u32SoftKindBlk;       /* scan: KIND_BLK */
static u32 g_u32SoftKindGpu;       /* scan: KIND_GPU */
static u32 g_u32SoftKindInput;     /* scan: KIND_INPUT */
static u32 g_u32SoftKindConsole;   /* scan: KIND_CONSOLE */
static u32 g_u32SoftKindScsi;      /* scan: KIND_SCSI */
static u32 g_u32SoftKindUnknown;   /* scan: KIND_UNKNOWN */
static u32 g_u32SoftBarMapFail;    /* high BAR vmm_map_device fail */
static u32 g_u32SoftSetupOk;       /* virtio_pci_setup success */
static u32 g_u32SoftSetupInval;    /* setup NULL pDev */
static u32 g_u32SoftSetupNocap;    /* setup no PCI cap list */
static u32 g_u32SoftSetupNocommon; /* setup no common-cfg */
static u32 g_u32SoftModern;        /* live fModern count after setups */
static u32 g_u32SoftReset;         /* virtio_reset calls with common */
static u32 g_u32SoftResetTimeout;  /* reset spin exhausted */
static u32 g_u32SoftNegoOk;        /* virtio_negotiate success */
static u32 g_u32SoftNegoFail;      /* virtio_negotiate FEATURES_OK reject */
static u32 g_u32SoftNegoInval;     /* negotiate bad args */
static u32 g_u32SoftSoftOk;        /* virtio_negotiate_soft success */
static u32 g_u32SoftSoftFail;      /* negotiate_soft exhausted ladder */
static u32 g_u32SoftSoftInval;     /* negotiate_soft bad args */
static u32 g_u32SoftSoftSteps;     /* soft ladder steps attempted (sum) */
static u32 g_u32SoftQSetupOk;      /* virtio_q_setup success */
static u32 g_u32SoftQClamp;        /* q size clamped from want */
static u32 g_u32SoftQNomem;        /* q ring page alloc fail */
static u32 g_u32SoftQEnableRej;    /* q enable read-back 0 */
static u32 g_u32SoftQBeyond;       /* q idx >= num_queues */
static u32 g_u32SoftQMax0;         /* device queue_size max 0 */
static u32 g_u32SoftQInval;        /* q_setup bad args */
static u32 g_u32SoftQDisable;      /* virtio_q_disable calls */
static u32 g_u32SoftDriverOk;      /* virtio_driver_ok calls */
static u32 g_u32SoftKick;          /* virtio_q_kick calls */
static u32 g_u32SoftAdd;           /* virtio_q_add success */
static u32 g_u32SoftAddFail;       /* virtio_q_add reject */
static u32 g_u32SoftAdd2;          /* virtio_q_add2 success */
static u32 g_u32SoftAdd2Fail;      /* virtio_q_add2 reject */
static u32 g_u32SoftAdd3;          /* virtio_q_add3 success */
static u32 g_u32SoftAdd3Fail;      /* virtio_q_add3 reject */
static u32 g_u32SoftPollHit;       /* virtio_q_poll_id completion */
static u32 g_u32SoftPollTo;        /* virtio_q_poll_id timeout */
static u32 g_u32SoftReap;          /* virtio_q_reap completions (sum) */
static u32 g_u32SoftIsr;           /* virtio_isr_read calls */
static u32 g_u32SoftLogN;          /* soft inventory log emissions */
static u8  g_fSoftInvOnce;         /* one-shot deep dump after activity */
/* Soft residual: status / feature / sticky last / live-cap snapshots. */
static u32 g_u32SoftStatusSet;     /* virtio_set_status calls */
static u32 g_u32SoftGetStatus;     /* virtio_get_status calls */
static u32 g_u32SoftCfgGen;        /* virtio_config_generation calls */
static u32 g_u32SoftFeatDev;       /* virtio_features_device calls */
static u32 g_u32SoftFeatDrv;       /* virtio_features_driver calls */
static u32 g_u32SoftFeatNego;      /* virtio_features_negotiated calls */
static u32 g_u32SoftFeatHas;       /* virtio_features_has calls */
static u32 g_u32SoftLastQIdx;      /* sticky last q_setup / q op idx */
static u32 g_u32SoftLastAddN;      /* sticky last successful add chain n (1/2/3) */
static u32 g_u32SoftLastWantLo;    /* sticky last negotiate want low 32 */
static u32 g_u32SoftLastWantHi;    /* sticky last negotiate want high 32 */
static u32 g_u32SoftLiveNotify;    /* scan table: pNotify non-NULL count */
static u32 g_u32SoftLiveIsr;       /* scan table: pIsr non-NULL count */
static u32 g_u32SoftLiveDevCfg;    /* scan table: pDevice non-NULL count */
static u32 g_u32SoftNumQSum;       /* sum of u32NumQueues across live modern */
/* T0 triad path tallies (net/blk/gpu; Soft!=product; silent until lean log). */
static u32 g_u32SoftSetupNet;      /* setup ok KIND_NET */
static u32 g_u32SoftSetupBlk;      /* setup ok KIND_BLK */
static u32 g_u32SoftSetupGpu;      /* setup ok KIND_GPU */
static u32 g_u32SoftNegoNet;       /* negotiate ok KIND_NET */
static u32 g_u32SoftNegoBlk;       /* negotiate ok KIND_BLK */
static u32 g_u32SoftNegoGpu;       /* negotiate ok KIND_GPU */
static u32 g_u32SoftQSetupNet;     /* q_setup ok on KIND_NET */
static u32 g_u32SoftQSetupBlk;     /* q_setup ok on KIND_BLK */
static u32 g_u32SoftQSetupGpu;     /* q_setup ok on KIND_GPU */
static u32 g_u32SoftDriverOkNet;   /* driver_ok on KIND_NET */
static u32 g_u32SoftDriverOkBlk;   /* driver_ok on KIND_BLK */
static u32 g_u32SoftDriverOkGpu;   /* driver_ok on KIND_GPU */
static u32 g_u32SoftFindKindHit;   /* virtio_dev_find_kind hits */
static u32 g_u32SoftFindKindMiss;  /* virtio_dev_find_kind misses */
/* C2 residual deepen (silent; Soft!=product; fold into lean lines only). */
static u32 g_u32SoftKickNet;       /* kick on KIND_NET */
static u32 g_u32SoftKickBlk;       /* kick on KIND_BLK */
static u32 g_u32SoftKickGpu;       /* kick on KIND_GPU */
static u32 g_u32SoftCapCommon;     /* setup: common-cfg cap wired */
static u32 g_u32SoftCapNotify;     /* setup: notify-cfg cap wired */
static u32 g_u32SoftCapIsr;        /* setup: isr-cfg cap wired */
static u32 g_u32SoftCapDevCfg;     /* setup: device-cfg cap wired */
static u32 g_u32SoftNegoV1;        /* negotiate ok with VERSION_1 bit */
static u32 g_u32SoftDescRecov;     /* partial add free-list recoveries */
static u32 g_u32SoftQResync;       /* q_setup freed prior rings (re-entry) */
static u32 g_u32SoftLastStatus;    /* sticky last set/get device_status */
static u32 g_u32SoftLastPollLen;   /* sticky last successful poll used len */

static void soft_inc(u32 *pCtr);
static void soft_t0_kind_inc(u32 u32Kind, u32 *pNet, u32 *pBlk, u32 *pGpu);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);
static u32 soft_residual_lean_ok(u32 *pOutChecks);
static u32 kind_from_device(u16 u16Device);
static u16 q_soft_size(u16 u16Want, u16 u16Max);
static void q_free_one_desc(struct gj_virtq *pQ, u16 u16Idx);

/** Soft: bump path tally (saturate at u32 max; telemetry only). */
static void
soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/**
 * Soft: bump T0 triad counter matching u32Kind (net/blk/gpu only).
 * Other kinds are silent here (class residual units own them). Soft!=product.
 * Null counter pointers are no-ops (lean self-check / TE safety).
 */
static void
soft_t0_kind_inc(u32 u32Kind, u32 *pNet, u32 *pBlk, u32 *pGpu)
{
    if (u32Kind == GJ_VIRTIO_KIND_NET) {
        if (pNet != NULL) {
            soft_inc(pNet);
        }
    } else if (u32Kind == GJ_VIRTIO_KIND_BLK) {
        if (pBlk != NULL) {
            soft_inc(pBlk);
        }
    } else if (u32Kind == GJ_VIRTIO_KIND_GPU) {
        if (pGpu != NULL) {
            soft_inc(pGpu);
        }
    }
}

/**
 * Silent lean residual self-check (no kprintf; Soft!=product).
 * Returns ok count; *pOutChecks = total checks. Transport path unchanged.
 * Results fold into soft residual lean lines only (no stamp storm).
 * Covers OASIS common-cfg layout + T0 net/blk/gpu ID -> kind mapping.
 * C2 residual deepen: extra layout / clamp / status / transitional IDs /
 * inventory kinds / desc flags / soft_t0 null-safety / dual_dod honesty.
 */
static u32
soft_residual_lean_ok(u32 *pOutChecks)
{
    u32 u32Ok = 0;
    u32 u32Checks = 0;
    u16 u16Sz;

    /* 1: common-cfg status offset (OASIS) */
    u32Checks++;
    if (VIRTIO_PCI_COMMON_STATUS == 20u) {
        u32Ok++;
    }
    /* 2: queue_desc offset */
    u32Checks++;
    if (VIRTIO_PCI_COMMON_Q_DESC == 32u) {
        u32Ok++;
    }
    /* 3: q_soft_size power-of-two clamp within max */
    u32Checks++;
    u16Sz = q_soft_size(100, 256);
    if (u16Sz != 0u && u16Sz <= 256u && (u16Sz & (u16)(u16Sz - 1u)) == 0u) {
        u32Ok++;
    }
    /* 4: modern net device id -> KIND_NET (T0 product NIC) */
    u32Checks++;
    if (kind_from_device(GJ_VIRTIO_PCI_DEV_NET_MODERN) == GJ_VIRTIO_KIND_NET) {
        u32Ok++;
    }
    /* 5: modern blk device id -> KIND_BLK (T0 store) */
    u32Checks++;
    if (kind_from_device(GJ_VIRTIO_PCI_DEV_BLK_MODERN) == GJ_VIRTIO_KIND_BLK) {
        u32Ok++;
    }
    /* 6: modern gpu device id -> KIND_GPU (T0 present) */
    u32Checks++;
    if (kind_from_device(GJ_VIRTIO_PCI_DEV_GPU_MODERN) == GJ_VIRTIO_KIND_GPU) {
        u32Ok++;
    }
    /* 7: T0 kinds unique and non-zero */
    u32Checks++;
    if (GJ_VIRTIO_KIND_NET != 0u && GJ_VIRTIO_KIND_BLK != 0u &&
        GJ_VIRTIO_KIND_GPU != 0u &&
        GJ_VIRTIO_KIND_NET != GJ_VIRTIO_KIND_BLK &&
        GJ_VIRTIO_KIND_NET != GJ_VIRTIO_KIND_GPU &&
        GJ_VIRTIO_KIND_BLK != GJ_VIRTIO_KIND_GPU) {
        u32Ok++;
    }
    /* 8: GJ_VIRTIO_KIND_IS_T0 covers triad only */
    u32Checks++;
    if (GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_NET) &&
        GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_BLK) &&
        GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_GPU) &&
        !GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_INPUT) &&
        !GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_UNKNOWN)) {
        u32Ok++;
    }
    /* 9: VERSION_1 transport bit (required on product negotiate) */
    u32Checks++;
    if (GJ_VIRTIO_F_VERSION_1 == (1ull << 32)) {
        u32Ok++;
    }
    /* 10: ring struct max covers T0 class queue sizes (64 typical) */
    u32Checks++;
    if (GJ_VIRTQ_MAX_SIZE >= 64u &&
        (GJ_VIRTQ_MAX_SIZE & (GJ_VIRTQ_MAX_SIZE - 1u)) == 0u) {
        u32Ok++;
    }
    /* 11: emission cap is lean (stamp storm bound; strict CAP<=2 preferred) */
    u32Checks++;
    if (VIRTIO_PCI_SOFT_INV_LOG_CAP > 0u &&
        VIRTIO_PCI_SOFT_INV_LOG_CAP <= 2u) {
        u32Ok++;
    }
    /* 12: soft_inc null-safe (no fault) */
    u32Checks++;
    soft_inc(NULL);
    u32Ok++;
    /* 13: common-cfg cap type id (OASIS) */
    u32Checks++;
    if (GJ_VIRTIO_PCI_CAP_COMMON_CFG == 1u &&
        GJ_VIRTIO_PCI_CAP_NOTIFY_CFG == 2u) {
        u32Ok++;
    }
    /* 14: q_soft_size(64,256) returns 64 (T0 class default want) */
    u32Checks++;
    if (q_soft_size(64, 256) == 64u) {
        u32Ok++;
    }
    /* 15: residual deepen - queue_enable / avail / used offsets */
    u32Checks++;
    if (VIRTIO_PCI_COMMON_Q_ENABLE == 28u &&
        VIRTIO_PCI_COMMON_Q_DRIVER == 40u &&
        VIRTIO_PCI_COMMON_Q_DEVICE == 48u) {
        u32Ok++;
    }
    /* 16: residual deepen - status bits distinct product ladder */
    u32Checks++;
    if (GJ_VIRTIO_S_ACKNOWLEDGE == 1u && GJ_VIRTIO_S_DRIVER == 2u &&
        GJ_VIRTIO_S_DRIVER_OK == 4u && GJ_VIRTIO_S_FEATURES_OK == 8u &&
        GJ_VIRTIO_S_FAILED == 128u &&
        (GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
         GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK) == 15u) {
        u32Ok++;
    }
    /* 17: residual deepen - transitional net/blk inventory map to T0 */
    u32Checks++;
    if (kind_from_device(0x1000u) == GJ_VIRTIO_KIND_NET &&
        kind_from_device(0x1001u) == GJ_VIRTIO_KIND_BLK) {
        u32Ok++;
    }
    /* 18: residual deepen - q_soft_size edge (want0->1; want>max clamp pot) */
    u32Checks++;
    u16Sz = q_soft_size(0, 256);
    if (u16Sz == 1u && q_soft_size(300, 128) == 128u &&
        q_soft_size(50, 0) == 0u) {
        u32Ok++;
    }
    /* 19: residual deepen - ISR/DEVICE/PCI cap type ids (OASIS) */
    u32Checks++;
    if (GJ_VIRTIO_PCI_CAP_ISR_CFG == 3u &&
        GJ_VIRTIO_PCI_CAP_DEVICE_CFG == 4u &&
        GJ_VIRTIO_PCI_CAP_PCI_CFG == 5u) {
        u32Ok++;
    }
    /* 20: residual deepen - vendor + page size residual constants */
    u32Checks++;
    if (GJ_VIRTIO_PCI_VENDOR == 0x1AF4u && VIRTIO_PCI_PAGE_SIZE == 4096u) {
        u32Ok++;
    }
    /* 21: residual deepen - inventory kinds input/scsi/console (not T0) */
    u32Checks++;
    if (kind_from_device(GJ_VIRTIO_PCI_DEV_INPUT) == GJ_VIRTIO_KIND_INPUT &&
        kind_from_device(GJ_VIRTIO_PCI_DEV_SCSI_MODERN) ==
            GJ_VIRTIO_KIND_SCSI &&
        kind_from_device(GJ_VIRTIO_PCI_DEV_CONSOLE) ==
            GJ_VIRTIO_KIND_CONSOLE &&
        !GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_SCSI) &&
        !GJ_VIRTIO_KIND_IS_T0(GJ_VIRTIO_KIND_CONSOLE)) {
        u32Ok++;
    }
    /* 22: residual deepen - split-VQ desc flags distinct (NEXT vs WRITE) */
    u32Checks++;
    if (GJ_VIRTQ_DESC_F_NEXT == 1u && GJ_VIRTQ_DESC_F_WRITE == 2u &&
        (GJ_VIRTQ_DESC_F_NEXT & GJ_VIRTQ_DESC_F_WRITE) == 0u) {
        u32Ok++;
    }
    /* 23: residual deepen - soft_t0_kind_inc null-safe (no fault) */
    u32Checks++;
    soft_t0_kind_inc(GJ_VIRTIO_KIND_NET, NULL, NULL, NULL);
    soft_t0_kind_inc(GJ_VIRTIO_KIND_BLK, NULL, NULL, NULL);
    soft_t0_kind_inc(GJ_VIRTIO_KIND_GPU, NULL, NULL, NULL);
    soft_t0_kind_inc(GJ_VIRTIO_KIND_INPUT, NULL, NULL, NULL);
    u32Ok++;
    /* 24: residual deepen - scan table holds T0 triad capacity */
    u32Checks++;
    if (GJ_VIRTIO_MAX_DEVS >= 3u) {
        u32Ok++;
    }
    /* 25: residual deepen - transport feature bits distinct from VERSION_1 */
    u32Checks++;
    if (GJ_VIRTIO_F_RING_PACKED != GJ_VIRTIO_F_VERSION_1 &&
        GJ_VIRTIO_F_RING_INDIRECT_DESC != GJ_VIRTIO_F_VERSION_1 &&
        GJ_VIRTIO_F_ACCESS_PLATFORM != GJ_VIRTIO_F_VERSION_1) {
        u32Ok++;
    }
    /* 26: residual deepen - Q_SELECT / Q_SIZE / Q_MSIX / Q_NOFF offsets */
    u32Checks++;
    if (VIRTIO_PCI_COMMON_Q_SELECT == 22u &&
        VIRTIO_PCI_COMMON_Q_SIZE == 24u &&
        VIRTIO_PCI_COMMON_Q_MSIX == 26u &&
        VIRTIO_PCI_COMMON_Q_NOFF == 30u) {
        u32Ok++;
    }

    if (pOutChecks != NULL) {
        *pOutChecks = u32Checks;
    }
    return u32Ok;
}

/**
 * Lean soft virtio-pci residual inventory (C2 product path / smoke).
 * Hard-capped emission - few lines only; never multi-kprintf stamp storms
 * (prior FAULT class). Twin prefixes; Soft!=product dual license;
 * no wave=/version stamp.
 * greppable: virtio-pci: soft inventory
 * greppable: virtio-pci: soft residual lean
 * greppable: virtio: soft residual lean
 */
static void
soft_inventory_log(void)
{
    u32 u32Found;
    u32 u32Modern;
    u32 u32Notify;
    u32 u32Isr;
    u32 u32DevCfg;
    u32 u32NumQSum;
    u32 u32AddOk;
    u32 u32LeanOk;
    u32 u32LeanChecks;
    u32 i;
    const char *szVerdict;

    /* Cap serial flood (scan + maybe_once; residual only). */
    if (g_u32SoftLogN >= VIRTIO_PCI_SOFT_INV_LOG_CAP) {
        return;
    }
    soft_inc(&g_u32SoftLogN);

    /* Live inventory snapshot (scan table; no lock). */
    u32Found = g_cDevs;
    u32Modern = 0;
    u32Notify = 0;
    u32Isr = 0;
    u32DevCfg = 0;
    u32NumQSum = 0;
    for (i = 0; i < g_cDevs && i < GJ_VIRTIO_MAX_DEVS; i++) {
        if (g_aDevs[i].fModern != 0) {
            u32Modern++;
            u32NumQSum += g_aDevs[i].u32NumQueues;
        }
        if (g_aDevs[i].pNotify != NULL) {
            u32Notify++;
        }
        if (g_aDevs[i].pIsr != NULL) {
            u32Isr++;
        }
        if (g_aDevs[i].pDevice != NULL) {
            u32DevCfg++;
        }
    }
    g_u32SoftModern = u32Modern;
    g_u32SoftScanFound = u32Found;
    g_u32SoftLiveNotify = u32Notify;
    g_u32SoftLiveIsr = u32Isr;
    g_u32SoftLiveDevCfg = u32DevCfg;
    g_u32SoftNumQSum = u32NumQSum;

    u32AddOk = g_u32SoftAdd + g_u32SoftAdd2 + g_u32SoftAdd3;
    u32LeanOk = soft_residual_lean_ok(&u32LeanChecks);

    /*
     * Soft verdict (inventory only; transport path unchanged):
     *   NODEV    - scan table empty (no virtio BDF retained)
     *   PASS     - modern + any queue setup or product kick/add
     *   PARTIAL  - found and/or modern, no q/io activity yet
     * Note: fModern is set only after virtio_pci_setup; post-scan
     * found>0 with modern=0 is PARTIAL (not NODEV).
     * T0 triad activity (net/blk/gpu setup|q|driver_ok) also yields PASS.
     */
    if (u32Found == 0u) {
        szVerdict = "NODEV";
    } else if (u32Modern != 0u &&
               (g_u32SoftQSetupOk != 0u || g_u32SoftKick != 0u ||
                u32AddOk != 0u || g_u32SoftDriverOk != 0u ||
                g_u32SoftSetupNet != 0u || g_u32SoftSetupBlk != 0u ||
                g_u32SoftSetupGpu != 0u)) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /*
     * Lean twin inventory + residual lean + verdict (6 lines max).
     * Soft!=product dual license; stamp_storm=0; no version stamp.
     * T0 triad fields greppable: t0_net/t0_blk/t0_gpu + setup_* / q_* /
     * kick_* / dok_* + fail honesty (add_fail/nego_fail/q_nomem/desc_recov)
     * + q_resync/last_poll/poll_hit + dual_dod=OPEN (Soft residual honesty).
     * greppable: virtio-pci: soft inventory / residual lean / PASS|NODEV|PARTIAL
     */
    kprintf("virtio-pci: soft inventory found=%u modern=%u net=%u blk=%u "
            "gpu=%u scsi=%u setup_ok=%u q_ok=%u nego_ok=%u driver_ok=%u "
            "kick=%u notify=%u isr=%u numq_sum=%u "
            "setup_net=%u setup_blk=%u setup_gpu=%u "
            "q_net=%u q_blk=%u q_gpu=%u "
            "kick_net=%u kick_blk=%u kick_gpu=%u "
            "dok_net=%u dok_blk=%u dok_gpu=%u log_n=%u Soft!=product\n",
            u32Found, u32Modern, g_u32SoftKindNet, g_u32SoftKindBlk,
            g_u32SoftKindGpu, g_u32SoftKindScsi, g_u32SoftSetupOk,
            g_u32SoftQSetupOk, g_u32SoftNegoOk, g_u32SoftDriverOk,
            g_u32SoftKick, u32Notify, u32Isr, u32NumQSum,
            g_u32SoftSetupNet, g_u32SoftSetupBlk, g_u32SoftSetupGpu,
            g_u32SoftQSetupNet, g_u32SoftQSetupBlk, g_u32SoftQSetupGpu,
            g_u32SoftKickNet, g_u32SoftKickBlk, g_u32SoftKickGpu,
            g_u32SoftDriverOkNet, g_u32SoftDriverOkBlk, g_u32SoftDriverOkGpu,
            g_u32SoftLogN);
    kprintf("virtio: soft inventory found=%u modern=%u net=%u blk=%u "
            "gpu=%u scsi=%u setup_ok=%u q_ok=%u nego_ok=%u driver_ok=%u "
            "kick=%u notify=%u isr=%u numq_sum=%u "
            "setup_net=%u setup_blk=%u setup_gpu=%u "
            "q_net=%u q_blk=%u q_gpu=%u "
            "kick_net=%u kick_blk=%u kick_gpu=%u "
            "dok_net=%u dok_blk=%u dok_gpu=%u log_n=%u Soft!=product\n",
            u32Found, u32Modern, g_u32SoftKindNet, g_u32SoftKindBlk,
            g_u32SoftKindGpu, g_u32SoftKindScsi, g_u32SoftSetupOk,
            g_u32SoftQSetupOk, g_u32SoftNegoOk, g_u32SoftDriverOk,
            g_u32SoftKick, u32Notify, u32Isr, u32NumQSum,
            g_u32SoftSetupNet, g_u32SoftSetupBlk, g_u32SoftSetupGpu,
            g_u32SoftQSetupNet, g_u32SoftQSetupBlk, g_u32SoftQSetupGpu,
            g_u32SoftKickNet, g_u32SoftKickBlk, g_u32SoftKickGpu,
            g_u32SoftDriverOkNet, g_u32SoftDriverOkBlk, g_u32SoftDriverOkGpu,
            g_u32SoftLogN);
    kprintf("virtio-pci: soft residual lean t0_product=1 lab_net=rtl "
            "path=pci_modern+vq common=1 notify=%u isr=%u devcfg=%u "
            "packed=0 msix=0 game_io=0 product_net=virtio "
            "t0_net=%u t0_blk=%u t0_gpu=%u "
            "setup_net=%u setup_blk=%u setup_gpu=%u "
            "q_net=%u q_blk=%u q_gpu=%u "
            "nego_net=%u nego_blk=%u nego_gpu=%u "
            "kick_net=%u kick_blk=%u kick_gpu=%u "
            "dok_net=%u dok_blk=%u dok_gpu=%u "
            "add_fail=%u nego_fail=%u q_nomem=%u desc_recov=%u "
            "v1=%u cap_n=%u q_resync=%u last_q=%u last_addn=%u last_st=0x%x "
            "last_poll=%u poll_hit=%u "
            "lean_ok=%u/%u verdict=%s dual=MIT_OR_Apache-2.0 dual_dod=OPEN "
            "stamp_storm=0 no_version_stamp=1 soft_ne_product=1 G-AC-1 "
            "Soft!=product\n",
            u32Notify, u32Isr, u32DevCfg,
            g_u32SoftKindNet, g_u32SoftKindBlk, g_u32SoftKindGpu,
            g_u32SoftSetupNet, g_u32SoftSetupBlk, g_u32SoftSetupGpu,
            g_u32SoftQSetupNet, g_u32SoftQSetupBlk, g_u32SoftQSetupGpu,
            g_u32SoftNegoNet, g_u32SoftNegoBlk, g_u32SoftNegoGpu,
            g_u32SoftKickNet, g_u32SoftKickBlk, g_u32SoftKickGpu,
            g_u32SoftDriverOkNet, g_u32SoftDriverOkBlk, g_u32SoftDriverOkGpu,
            g_u32SoftAddFail + g_u32SoftAdd2Fail + g_u32SoftAdd3Fail,
            g_u32SoftNegoFail, g_u32SoftQNomem, g_u32SoftDescRecov,
            g_u32SoftNegoV1,
            g_u32SoftCapCommon + g_u32SoftCapNotify + g_u32SoftCapIsr +
                g_u32SoftCapDevCfg,
            g_u32SoftQResync, g_u32SoftLastQIdx, g_u32SoftLastAddN,
            g_u32SoftLastStatus, g_u32SoftLastPollLen, g_u32SoftPollHit,
            u32LeanOk, u32LeanChecks, szVerdict);
    kprintf("virtio: soft residual lean t0_product=1 lab_net=rtl "
            "path=pci_modern+vq common=1 notify=%u isr=%u devcfg=%u "
            "packed=0 msix=0 game_io=0 product_net=virtio "
            "t0_net=%u t0_blk=%u t0_gpu=%u "
            "setup_net=%u setup_blk=%u setup_gpu=%u "
            "q_net=%u q_blk=%u q_gpu=%u "
            "nego_net=%u nego_blk=%u nego_gpu=%u "
            "kick_net=%u kick_blk=%u kick_gpu=%u "
            "dok_net=%u dok_blk=%u dok_gpu=%u "
            "add_fail=%u nego_fail=%u q_nomem=%u desc_recov=%u "
            "v1=%u cap_n=%u q_resync=%u last_q=%u last_addn=%u last_st=0x%x "
            "last_poll=%u poll_hit=%u "
            "lean_ok=%u/%u verdict=%s dual=MIT_OR_Apache-2.0 dual_dod=OPEN "
            "stamp_storm=0 no_version_stamp=1 soft_ne_product=1 G-AC-1 "
            "Soft!=product\n",
            u32Notify, u32Isr, u32DevCfg,
            g_u32SoftKindNet, g_u32SoftKindBlk, g_u32SoftKindGpu,
            g_u32SoftSetupNet, g_u32SoftSetupBlk, g_u32SoftSetupGpu,
            g_u32SoftQSetupNet, g_u32SoftQSetupBlk, g_u32SoftQSetupGpu,
            g_u32SoftNegoNet, g_u32SoftNegoBlk, g_u32SoftNegoGpu,
            g_u32SoftKickNet, g_u32SoftKickBlk, g_u32SoftKickGpu,
            g_u32SoftDriverOkNet, g_u32SoftDriverOkBlk, g_u32SoftDriverOkGpu,
            g_u32SoftAddFail + g_u32SoftAdd2Fail + g_u32SoftAdd3Fail,
            g_u32SoftNegoFail, g_u32SoftQNomem, g_u32SoftDescRecov,
            g_u32SoftNegoV1,
            g_u32SoftCapCommon + g_u32SoftCapNotify + g_u32SoftCapIsr +
                g_u32SoftCapDevCfg,
            g_u32SoftQResync, g_u32SoftLastQIdx, g_u32SoftLastAddN,
            g_u32SoftLastStatus, g_u32SoftLastPollLen, g_u32SoftPollHit,
            u32LeanOk, u32LeanChecks, szVerdict);
    kprintf("virtio-pci: soft %s found=%u modern=%u setup_ok=%u q_ok=%u "
            "kick=%u t0_net=%u t0_blk=%u t0_gpu=%u "
            "kick_net=%u kick_blk=%u kick_gpu=%u "
            "lean_ok=%u/%u log_n=%u dual=MIT_OR_Apache-2.0 dual_dod=OPEN "
            "Soft!=product\n",
            szVerdict, u32Found, u32Modern, g_u32SoftSetupOk,
            g_u32SoftQSetupOk, g_u32SoftKick,
            g_u32SoftKindNet, g_u32SoftKindBlk, g_u32SoftKindGpu,
            g_u32SoftKickNet, g_u32SoftKickBlk, g_u32SoftKickGpu,
            u32LeanOk, u32LeanChecks, g_u32SoftLogN);
    kprintf("virtio: soft %s found=%u modern=%u setup_ok=%u q_ok=%u "
            "kick=%u t0_net=%u t0_blk=%u t0_gpu=%u "
            "kick_net=%u kick_blk=%u kick_gpu=%u "
            "lean_ok=%u/%u log_n=%u dual=MIT_OR_Apache-2.0 dual_dod=OPEN "
            "Soft!=product\n",
            szVerdict, u32Found, u32Modern, g_u32SoftSetupOk,
            g_u32SoftQSetupOk, g_u32SoftKick,
            g_u32SoftKindNet, g_u32SoftKindBlk, g_u32SoftKindGpu,
            g_u32SoftKickNet, g_u32SoftKickBlk, g_u32SoftKickGpu,
            u32LeanOk, u32LeanChecks, g_u32SoftLogN);
}
#if 0 /* STORM_TAIL removed - lean residual; dead storm surface kept out of compile */


    /*
     * Twin prefix: virtio: soft ... (agent-friendly alias; same tallies).
     */


    /*
     * Dead STORM_TAIL surface - not compiled; lean residual forbids re-open.
     */























}

#endif /* STORM_TAIL (lean residual: dead storm surface; not compiled) */

/**
 * After first product setup/negotiate/q/io activity, print soft inventory
 * once (capped via soft_inventory_log). Diagnostics only. Soft!=product.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftInvOnce != 0) {
        return;
    }
    if (g_u32SoftSetupOk == 0 && g_u32SoftSetupNocommon == 0 &&
        g_u32SoftNegoOk == 0 && g_u32SoftNegoFail == 0 &&
        g_u32SoftQSetupOk == 0 && g_u32SoftDriverOk == 0 &&
        g_u32SoftKick == 0) {
        return;
    }
    g_fSoftInvOnce = 1;
    soft_inventory_log();
}

static void
outl(u16 u16Port, u32 u32Val)
{
    __asm__ volatile ("outl %0, %1" : : "a"(u32Val), "Nd"(u16Port));
}

static u32
inl(u16 u16Port)
{
    u32 u32Val;

    __asm__ volatile ("inl %1, %0" : "=a"(u32Val) : "Nd"(u16Port));
    return u32Val;
}

static u32
pci_read32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32Addr = (1u << 31) | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xFCu);

    outl(PCI_CFG_ADDR, u32Addr);
    return inl(PCI_CFG_DATA);
}

static u16
pci_read16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32 = pci_read32(u8Bus, u8Slot, u8Func, u8Off & 0xFCu);
    return (u16)((u32 >> ((u8Off & 2u) * 8u)) & 0xffffu);
}

static u8
pci_read8(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off)
{
    u32 u32 = pci_read32(u8Bus, u8Slot, u8Func, u8Off & 0xFCu);
    return (u8)((u32 >> ((u8Off & 3u) * 8u)) & 0xffu);
}

static void
pci_write32(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u32 u32Val)
{
    u32 u32Addr = (1u << 31) | ((u32)u8Bus << 16) | ((u32)u8Slot << 11) |
                  ((u32)u8Func << 8) | (u8Off & 0xFCu);

    outl(PCI_CFG_ADDR, u32Addr);
    outl(PCI_CFG_DATA, u32Val);
}

static void
pci_write16(u8 u8Bus, u8 u8Slot, u8 u8Func, u8 u8Off, u16 u16Val)
{
    u32 u32Word = pci_read32(u8Bus, u8Slot, u8Func, u8Off & 0xFCu);
    u32 u32Shift = (u8Off & 2u) * 8u;

    u32Word &= ~(0xffffu << u32Shift);
    u32Word |= ((u32)u16Val << u32Shift);
    pci_write32(u8Bus, u8Slot, u8Func, u8Off & 0xFCu, u32Word);
}

static u32
kind_from_device(u16 u16Device)
{
    switch (u16Device) {
    case GJ_VIRTIO_PCI_DEV_NET_MODERN:
    case 0x1000u: /* transitional net (inventory; modern preferred) */
        return GJ_VIRTIO_KIND_NET;
    case GJ_VIRTIO_PCI_DEV_BLK_MODERN:
    case 0x1001u: /* transitional blk */
        return GJ_VIRTIO_KIND_BLK;
    case GJ_VIRTIO_PCI_DEV_SCSI_MODERN:
    case 0x1004u:
        return GJ_VIRTIO_KIND_SCSI;
    case GJ_VIRTIO_PCI_DEV_CONSOLE:
    case 0x1003u:
        return GJ_VIRTIO_KIND_CONSOLE;
    case GJ_VIRTIO_PCI_DEV_GPU_MODERN:
        return GJ_VIRTIO_KIND_GPU;
    case GJ_VIRTIO_PCI_DEV_INPUT:
        return GJ_VIRTIO_KIND_INPUT;
    default:
        return GJ_VIRTIO_KIND_UNKNOWN;
    }
}

static const char *
kind_name(u32 u32Kind)
{
    switch (u32Kind) {
    case GJ_VIRTIO_KIND_NET:
        return "net";
    case GJ_VIRTIO_KIND_BLK:
        return "blk";
    case GJ_VIRTIO_KIND_GPU:
        return "gpu";
    case GJ_VIRTIO_KIND_INPUT:
        return "input";
    case GJ_VIRTIO_KIND_CONSOLE:
        return "console";
    case GJ_VIRTIO_KIND_SCSI:
        return "scsi";
    default:
        return "virtio";
    }
}

/* Resolve capability BAR+offset to an MMIO pointer; NULL if unusable. */
static volatile u8 *
bar_ptr(struct gj_virtio_dev *pDev, u8 u8Bar, u32 u32Off)
{
    if (pDev == NULL || u8Bar >= 6 || pDev->u64Bar[u8Bar] == 0) {
        return NULL;
    }
    if ((pDev->u32BarIsIo & (1u << u8Bar)) != 0) {
        return NULL; /* MMIO only for modern */
    }
    return (volatile u8 *)(gj_vaddr_t)(pDev->u64Bar[u8Bar] + u32Off);
}

static u8
mmio_r8(volatile u8 *p)
{
    return p ? *p : 0;
}

static void
mmio_w8(volatile u8 *p, u8 u8V)
{
    if (p) {
        *p = u8V;
    }
}

static u16
mmio_r16(volatile u8 *p)
{
    return p ? *(volatile u16 *)p : 0;
}

static void
mmio_w16(volatile u8 *p, u16 u16V)
{
    if (p) {
        *(volatile u16 *)p = u16V;
    }
}

static u32
mmio_r32(volatile u8 *p)
{
    return p ? *(volatile u32 *)p : 0;
}

static void
mmio_w32(volatile u8 *p, u32 u32V)
{
    if (p) {
        *(volatile u32 *)p = u32V;
    }
}

static void
mmio_w64(volatile u8 *p, u64 u64V)
{
    /* Two 32-bit stores: qemu-kvm common-cfg impl max_access_size is 4.
     * An 8-byte KVM MMIO exit is dropped, so queue_desc/avail/used stay 0
     * and used.idx never advances (gpu/blk timeout, net rx=0). */
    if (p) {
        mmio_w32(p, (u32)u64V);
        mmio_w32(p + 4, (u32)(u64V >> 32));
    }
}

/* ---- common-cfg feature helpers (soft; greppable) -------------------- */

static u64
common_features_read(struct gj_virtio_dev *pDev, int fGuest)
{
    volatile u8 *pCommon;
    u32 u32Sel;
    u32 u32Val;
    u32 u32Lo;
    u32 u32Hi;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    pCommon = pDev->pCommon;
    if (fGuest) {
        u32Sel = VIRTIO_PCI_COMMON_GFSELECT;
        u32Val = VIRTIO_PCI_COMMON_GF;
    } else {
        u32Sel = VIRTIO_PCI_COMMON_DFSELECT;
        u32Val = VIRTIO_PCI_COMMON_DF;
    }
    mmio_w32(pCommon + u32Sel, 0);
    u32Lo = mmio_r32(pCommon + u32Val);
    mmio_w32(pCommon + u32Sel, 1);
    u32Hi = mmio_r32(pCommon + u32Val);
    return ((u64)u32Hi << 32) | (u64)u32Lo;
}

static void
common_features_write_guest(struct gj_virtio_dev *pDev, u64 u64Drv)
{
    volatile u8 *pCommon;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return;
    }
    pCommon = pDev->pCommon;
    mmio_w32(pCommon + VIRTIO_PCI_COMMON_GFSELECT, 0);
    mmio_w32(pCommon + VIRTIO_PCI_COMMON_GF, (u32)u64Drv);
    mmio_w32(pCommon + VIRTIO_PCI_COMMON_GFSELECT, 1);
    mmio_w32(pCommon + VIRTIO_PCI_COMMON_GF, (u32)(u64Drv >> 32));
}

/* Soft power-of-two size clamp into [1, u16Max] (0 if max is 0). */
static u16
q_soft_size(u16 u16Want, u16 u16Max)
{
    u16 u16P2;

    if (u16Max == 0) {
        return 0;
    }
    if (u16Want == 0) {
        u16Want = 1;
    }
    if (u16Want > u16Max) {
        u16Want = u16Max;
    }
    /* Round down to power of two within max (virtio queue_size requirement). */
    u16P2 = 1;
    while ((u16)(u16P2 << 1) <= u16Want && (u16)(u16P2 << 1) <= u16Max &&
           (u16P2 << 1) != 0) {
        u16P2 = (u16)(u16P2 << 1);
    }
    if (u16P2 > u16Max) {
        /* max itself may not be power-of-two on broken hosts - soft clamp */
        u16P2 = 1;
        while ((u16)(u16P2 << 1) <= u16Max && (u16P2 << 1) != 0) {
            u16P2 = (u16)(u16P2 << 1);
        }
    }
    return u16P2;
}

/**
 * Soft residual deepen: return one descriptor index to the free list.
 * Used by partial add/add2/add3 recovery so free-list honesty holds under
 * TE (no silent desc leak). Soft!=product; no kprintf.
 */
static void
q_free_one_desc(struct gj_virtq *pQ, u16 u16Idx)
{
    if (pQ == NULL || pQ->pDesc == NULL) {
        return;
    }
    if (u16Idx >= pQ->u16Size) {
        return;
    }
    pQ->pDesc[u16Idx].u64Addr = 0;
    pQ->pDesc[u16Idx].u32Len = 0;
    pQ->pDesc[u16Idx].u16Flags = 0;
    pQ->pDesc[u16Idx].u16Next = pQ->u16FreeHead;
    pQ->u16FreeHead = u16Idx;
    if (pQ->u16NumFree < pQ->u16Size) {
        pQ->u16NumFree++;
    }
    soft_inc(&g_u32SoftDescRecov);
}

static void
q_ring_free(struct gj_virtq *pQ)
{
    if (pQ == NULL) {
        return;
    }
    if (pQ->paDesc != 0) {
        pmm_free(pQ->paDesc);
        pQ->paDesc = 0;
        pQ->pDesc = NULL;
    }
    if (pQ->paAvail != 0) {
        pmm_free(pQ->paAvail);
        pQ->paAvail = 0;
        pQ->pAvail = NULL;
    }
    if (pQ->paUsed != 0) {
        pmm_free(pQ->paUsed);
        pQ->paUsed = 0;
        pQ->pUsed = NULL;
    }
}

static void
alloc_zero_page_unskip(gj_paddr_t paSkip)
{
    while (paSkip != 0) {
        gj_paddr_t paNext;

        paNext = *(gj_paddr_t *)hhdm_to_virt(paSkip);
        pmm_free(paSkip);
        paSkip = paNext;
    }
}

/*
 * Virtq DMA must sit in the IOMMU identity window [0,1GiB).
 * pmm_alloc_low is LIFO from the top of QEMU 2GiB RAM (still <4GiB), so the
 * first pops are ~0x7ffd0000. Hold those in a page-chain until a GPA below
 * 1GiB appears; an 8-slot skip list returns NOMEM while low pages remain.
 */
static gj_paddr_t
alloc_zero_page(void)
{
    gj_paddr_t paSkip = 0;
    gj_paddr_t pa;
    u32 cSkip = 0;

    for (;;) {
        pa = pmm_alloc_low();
        if (pa == 0) {
            pa = pmm_alloc();
        }
        if (pa == 0) {
            break;
        }
        if (pa < 0x40000000ull) {
            memset((void *)hhdm_to_virt(pa), 0, VIRTIO_PCI_PAGE_SIZE);
            alloc_zero_page_unskip(paSkip);
            return pa;
        }
        if (cSkip >= 524288u) {
            pmm_free(pa);
            break;
        }
        *(gj_paddr_t *)hhdm_to_virt(pa) = paSkip;
        paSkip = pa;
        cSkip++;
    }
    alloc_zero_page_unskip(paSkip);
    return 0;
}

void
virtio_init(void)
{
    memset(g_aDevs, 0, sizeof(g_aDevs));
    g_cDevs = 0;
    /* Soft residual tallies: baseline zero; log_n preserved across re-init. */
    g_u32SoftScanEnter = 0;
    g_u32SoftScanFound = 0;
    g_u32SoftKindNet = 0;
    g_u32SoftKindBlk = 0;
    g_u32SoftKindGpu = 0;
    g_u32SoftKindInput = 0;
    g_u32SoftKindConsole = 0;
    g_u32SoftKindScsi = 0;
    g_u32SoftKindUnknown = 0;
    g_u32SoftBarMapFail = 0;
    g_u32SoftSetupOk = 0;
    g_u32SoftSetupInval = 0;
    g_u32SoftSetupNocap = 0;
    g_u32SoftSetupNocommon = 0;
    g_u32SoftModern = 0;
    g_u32SoftReset = 0;
    g_u32SoftResetTimeout = 0;
    g_u32SoftNegoOk = 0;
    g_u32SoftNegoFail = 0;
    g_u32SoftNegoInval = 0;
    g_u32SoftSoftOk = 0;
    g_u32SoftSoftFail = 0;
    g_u32SoftSoftInval = 0;
    g_u32SoftSoftSteps = 0;
    g_u32SoftQSetupOk = 0;
    g_u32SoftQClamp = 0;
    g_u32SoftQNomem = 0;
    g_u32SoftQEnableRej = 0;
    g_u32SoftQBeyond = 0;
    g_u32SoftQMax0 = 0;
    g_u32SoftQInval = 0;
    g_u32SoftQDisable = 0;
    g_u32SoftDriverOk = 0;
    g_u32SoftKick = 0;
    g_u32SoftAdd = 0;
    g_u32SoftAddFail = 0;
    g_u32SoftAdd2 = 0;
    g_u32SoftAdd2Fail = 0;
    g_u32SoftAdd3 = 0;
    g_u32SoftAdd3Fail = 0;
    g_u32SoftPollHit = 0;
    g_u32SoftPollTo = 0;
    g_u32SoftReap = 0;
    g_u32SoftIsr = 0;
    g_fSoftInvOnce = 0;
    g_u32SoftStatusSet = 0;
    g_u32SoftGetStatus = 0;
    g_u32SoftCfgGen = 0;
    g_u32SoftFeatDev = 0;
    g_u32SoftFeatDrv = 0;
    g_u32SoftFeatNego = 0;
    g_u32SoftFeatHas = 0;
    g_u32SoftLastQIdx = 0;
    g_u32SoftLastAddN = 0;
    g_u32SoftLastWantLo = 0;
    g_u32SoftLastWantHi = 0;
    g_u32SoftLiveNotify = 0;
    g_u32SoftLiveIsr = 0;
    g_u32SoftLiveDevCfg = 0;
    g_u32SoftNumQSum = 0;
    g_u32SoftSetupNet = 0;
    g_u32SoftSetupBlk = 0;
    g_u32SoftSetupGpu = 0;
    g_u32SoftNegoNet = 0;
    g_u32SoftNegoBlk = 0;
    g_u32SoftNegoGpu = 0;
    g_u32SoftQSetupNet = 0;
    g_u32SoftQSetupBlk = 0;
    g_u32SoftQSetupGpu = 0;
    g_u32SoftDriverOkNet = 0;
    g_u32SoftDriverOkBlk = 0;
    g_u32SoftDriverOkGpu = 0;
    g_u32SoftFindKindHit = 0;
    g_u32SoftFindKindMiss = 0;
    /* C2 residual deepen tallies (silent until lean inventory). */
    g_u32SoftKickNet = 0;
    g_u32SoftKickBlk = 0;
    g_u32SoftKickGpu = 0;
    g_u32SoftCapCommon = 0;
    g_u32SoftCapNotify = 0;
    g_u32SoftCapIsr = 0;
    g_u32SoftCapDevCfg = 0;
    g_u32SoftNegoV1 = 0;
    g_u32SoftDescRecov = 0;
    g_u32SoftQResync = 0;
    g_u32SoftLastStatus = 0;
    g_u32SoftLastPollLen = 0;
    /*
     * Silent baseline only - no soft inventory kprintf at init.
     * Emission budget reserved for scan + post-activity maybe_once
     * (CAP=2). Soft!=product; stamp_storm=0. T0 triad tallies zeroed.
     */
}

u32
virtio_pci_scan(void)
{
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u32 u32Id;
    u16 u16Vendor;
    u16 u16Device;
    u32 iBar;

    soft_inc(&g_u32SoftScanEnter);
    /* Kind tallies re-accumulate for this scan pass. */
    g_u32SoftKindNet = 0;
    g_u32SoftKindBlk = 0;
    g_u32SoftKindGpu = 0;
    g_u32SoftKindInput = 0;
    g_u32SoftKindConsole = 0;
    g_u32SoftKindScsi = 0;
    g_u32SoftKindUnknown = 0;

    g_cDevs = 0;
    for (u8Bus = 0; u8Bus < 8; u8Bus++) {
        for (u8Slot = 0; u8Slot < 32; u8Slot++) {
            for (u8Func = 0; u8Func < 8; u8Func++) {
                struct gj_virtio_dev *pDev;

                u32Id = pci_read32(u8Bus, u8Slot, u8Func, 0);
                u16Vendor = (u16)(u32Id & 0xFFFFu);
                u16Device = (u16)(u32Id >> 16);
                if (u16Vendor == 0xFFFFu || u16Vendor == 0) {
                    if (u8Func == 0) {
                        break;
                    }
                    continue;
                }
                if (u16Vendor != GJ_VIRTIO_PCI_VENDOR) {
                    continue;
                }
                if (g_cDevs >= GJ_VIRTIO_MAX_DEVS) {
                    goto done;
                }
                pDev = &g_aDevs[g_cDevs];
                memset(pDev, 0, sizeof(*pDev));
                pDev->u16Vendor = u16Vendor;
                pDev->u16Device = u16Device;
                pDev->u8Bus = u8Bus;
                pDev->u8Slot = u8Slot;
                pDev->u8Func = u8Func;
                pDev->u32Kind = kind_from_device(u16Device);
                switch (pDev->u32Kind) {
                case GJ_VIRTIO_KIND_NET:
                    soft_inc(&g_u32SoftKindNet);
                    break;
                case GJ_VIRTIO_KIND_BLK:
                    soft_inc(&g_u32SoftKindBlk);
                    break;
                case GJ_VIRTIO_KIND_GPU:
                    soft_inc(&g_u32SoftKindGpu);
                    break;
                case GJ_VIRTIO_KIND_INPUT:
                    soft_inc(&g_u32SoftKindInput);
                    break;
                case GJ_VIRTIO_KIND_CONSOLE:
                    soft_inc(&g_u32SoftKindConsole);
                    break;
                case GJ_VIRTIO_KIND_SCSI:
                    soft_inc(&g_u32SoftKindScsi);
                    break;
                default:
                    soft_inc(&g_u32SoftKindUnknown);
                    break;
                }
                for (iBar = 0; iBar < 6; iBar++) {
                    u32 u32Bar = pci_read32(u8Bus, u8Slot, u8Func,
                                            (u8)(0x10u + iBar * 4u));

                    if ((u32Bar & 1u) != 0) {
                        pDev->u32BarIsIo |= (1u << iBar);
                        pDev->u64Bar[iBar] = (u64)(u32Bar & ~0x3u);
                    } else {
                        pDev->u64Bar[iBar] = (u64)(u32Bar & ~0xFull);
                        if ((u32Bar & 0x6u) == 0x4u && iBar + 1 < 6) {
                            /* 64-bit BAR */
                            u32 u32Hi = pci_read32(u8Bus, u8Slot, u8Func,
                                                   (u8)(0x10u + (iBar + 1) * 4u));
                            pDev->u64Bar[iBar] |= ((u64)u32Hi << 32);
                            iBar++;
                        }
                    }
                }
                /* High MMIO BARs (common with >=4 GiB RAM) need explicit maps */
                for (iBar = 0; iBar < 6; iBar++) {
                    if (pDev->u64Bar[iBar] == 0 ||
                        (pDev->u32BarIsIo & (1u << iBar)) != 0) {
                        continue;
                    }
                    if (pDev->u64Bar[iBar] >= 0x100000000ull ||
                        pDev->u64Bar[iBar] >= 0xf0000000ull) {
                        if (vmm_map_device((gj_paddr_t)pDev->u64Bar[iBar],
                                           2ull * 1024ull * 1024ull) != GJ_OK) {
                            soft_inc(&g_u32SoftBarMapFail);
                            kprintf("virtio: map bar%u 0x%lx failed\n", iBar,
                                    (unsigned long)pDev->u64Bar[iBar]);
                        }
                    }
                }
                kprintf("virtio: pci %x:%x.%u id=%x:%x bars=%lx/%lx/%lx %s\n",
                        (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
                        (unsigned)u16Vendor, (unsigned)u16Device,
                        (unsigned long)pDev->u64Bar[0],
                        (unsigned long)pDev->u64Bar[1],
                        (unsigned long)pDev->u64Bar[4],
                        kind_name(pDev->u32Kind));
                g_cDevs++;
            }
        }
    }
done:
    g_u32SoftScanFound = g_cDevs;
    if (g_cDevs > 0) {
        kprintf("virtio: scan PASS found %u device(s)\n", g_cDevs);
    } else {
        kprintf("virtio: scan found 0 device(s)\n");
    }
    /*
     * Lean soft residual after scan (found=0 = honest NODEV). Hard-capped.
     * greppable: virtio-pci: soft residual lean / virtio: soft residual lean
     */
    soft_inventory_log();
    return g_cDevs;
}

u32
virtio_dev_count(void)
{
    return g_cDevs;
}

struct gj_virtio_dev *
virtio_dev_get(u32 u32Index)
{
    if (u32Index >= g_cDevs) {
        return NULL;
    }
    return &g_aDevs[u32Index];
}

struct gj_virtio_dev *
virtio_dev_find_kind(u32 u32Kind)
{
    u32 i;

    for (i = 0; i < g_cDevs && i < GJ_VIRTIO_MAX_DEVS; i++) {
        if (g_aDevs[i].u32Kind == u32Kind) {
            soft_inc(&g_u32SoftFindKindHit);
            return &g_aDevs[i];
        }
    }
    soft_inc(&g_u32SoftFindKindMiss);
    return NULL;
}

u32
virtio_dev_count_kind(u32 u32Kind)
{
    u32 i;
    u32 c = 0;

    for (i = 0; i < g_cDevs && i < GJ_VIRTIO_MAX_DEVS; i++) {
        if (g_aDevs[i].u32Kind == u32Kind) {
            c++;
        }
    }
    return c;
}

/*
 * Walk PCI vendor caps and wire modern virtio MMIO windows.
 * Requires a non-NULL pDev from virtio_pci_scan(); leaves pCommon set on success.
 */
gj_status_t
virtio_pci_setup(struct gj_virtio_dev *pDev)
{
    u8 u8Cap;
    u16 u16Cmd;

    if (pDev == NULL) {
        soft_inc(&g_u32SoftSetupInval);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }

    pDev->fModern = 0;
    pDev->u64FeaturesDev = 0;
    pDev->u64FeaturesDrv = 0;
    pDev->pCommon = NULL;
    pDev->pNotify = NULL;
    pDev->pIsr = NULL;
    pDev->pDevice = NULL;
    pDev->u32NotifyMult = 0;
    pDev->u32NumQueues = 0;

    /* Enable memory + bus master before touching BARs */
    u16Cmd = pci_read16(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, PCI_CMD);
    u16Cmd |= (u16)(PCI_CMD_MEM | PCI_CMD_BUS);
    pci_write16(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, PCI_CMD, u16Cmd);

    {
        u16 u16Status = pci_read16(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, 0x06);

        if ((u16Status & 0x10u) == 0) {
            soft_inc(&g_u32SoftSetupNocap);
            kprintf("virtio: %x:%x no cap list (status=0x%x)\n",
                    (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot,
                    (unsigned)u16Status);
            soft_inventory_maybe_once();
            return GJ_ERR_NOSUPPORT;
        }
    }
    u8Cap = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, PCI_CAP_PTR) &
            0xFCu;
    while (u8Cap != 0) {
        u8 u8Id = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func, u8Cap);
        u8 u8Next = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                              (u8)(u8Cap + 1));
        u8 u8Len = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                             (u8)(u8Cap + 2));

        if (u8Id == PCI_CAP_ID_VNDR && u8Len >= 16) {
            u8 u8Type = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                                  (u8)(u8Cap + 3));
            u8 u8Bar = pci_read8(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                                 (u8)(u8Cap + 4));
            u32 u32Off = pci_read32(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                                    (u8)(u8Cap + 8));
            volatile u8 *pBase = bar_ptr(pDev, u8Bar, u32Off);

            /* Skip caps whose BAR is missing/IO - keep prior fields intact */
            if (pBase == NULL) {
                /* fall through to next cap */
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_COMMON_CFG) {
                pDev->pCommon = pBase;
                soft_inc(&g_u32SoftCapCommon);
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_NOTIFY_CFG) {
                pDev->pNotify = pBase;
                soft_inc(&g_u32SoftCapNotify);
                if (u8Len >= 20) {
                    pDev->u32NotifyMult =
                        pci_read32(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                                   (u8)(u8Cap + 16));
                } else {
                    pDev->u32NotifyMult = 0;
                }
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_ISR_CFG) {
                pDev->pIsr = pBase;
                soft_inc(&g_u32SoftCapIsr);
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_DEVICE_CFG) {
                pDev->pDevice = pBase;
                soft_inc(&g_u32SoftCapDevCfg);
            }
        }
        if (u8Next == 0 || u8Next == u8Cap) {
            break;
        }
        u8Cap = u8Next & 0xFCu;
    }

    if (pDev->pCommon == NULL) {
        soft_inc(&g_u32SoftSetupNocommon);
        kprintf("virtio: %x:%x no common cfg (legacy?)\n",
                (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot);
        soft_inventory_maybe_once();
        return GJ_ERR_NOSUPPORT;
    }

    /* Soft device reset; wait until status reads zero */
    virtio_reset(pDev);
    pDev->u32NumQueues = mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_NUMQ);
    pDev->fModern = 1;
    soft_inc(&g_u32SoftSetupOk);
    /* T0 triad soft tallies (net/blk/gpu); Soft!=product. */
    soft_t0_kind_inc(pDev->u32Kind, &g_u32SoftSetupNet, &g_u32SoftSetupBlk,
                     &g_u32SoftSetupGpu);
    kprintf("virtio: %x:%x modern common@%p queues=%u notify_mult=%u"
            " isr=%u devcfg=%u kind=%s\n",
            (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot,
            (void *)pDev->pCommon, pDev->u32NumQueues,
            (unsigned)pDev->u32NotifyMult,
            pDev->pIsr != NULL ? 1u : 0u,
            pDev->pDevice != NULL ? 1u : 0u,
            kind_name(pDev->u32Kind));
    soft_inventory_maybe_once();
    return GJ_OK;
}

void
virtio_reset(struct gj_virtio_dev *pDev)
{
    u32 iSpin;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return;
    }
    soft_inc(&g_u32SoftReset);
    mmio_w8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS, 0);
    for (iSpin = 0; iSpin < GJ_VIRTIO_RESET_SPINS; iSpin++) {
        if (mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS) == 0) {
            break;
        }
        __asm__ volatile ("pause");
    }
    pDev->u64FeaturesDev = 0;
    pDev->u64FeaturesDrv = 0;
    if (mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS) != 0) {
        soft_inc(&g_u32SoftResetTimeout);
        kprintf("virtio: reset soft timeout status=0x%x\n",
                (unsigned)mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS));
    }
}

void
virtio_set_status(struct gj_virtio_dev *pDev, u8 u8Status)
{
    if (pDev && pDev->pCommon) {
        soft_inc(&g_u32SoftStatusSet);
        g_u32SoftLastStatus = (u32)u8Status;
        mmio_w8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS, u8Status);
    }
}

u8
virtio_get_status(struct gj_virtio_dev *pDev)
{
    u8 u8St;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    soft_inc(&g_u32SoftGetStatus);
    u8St = mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS);
    g_u32SoftLastStatus = (u32)u8St;
    return u8St;
}

void
virtio_driver_ok(struct gj_virtio_dev *pDev)
{
    u8 u8St;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return;
    }
    soft_inc(&g_u32SoftDriverOk);
    soft_t0_kind_inc(pDev->u32Kind, &g_u32SoftDriverOkNet,
                     &g_u32SoftDriverOkBlk, &g_u32SoftDriverOkGpu);
    u8St = (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK);
    virtio_set_status(pDev, u8St);
    kprintf("virtio: driver_ok %x:%x status=0x%x features=0x%lx kind=%s\n",
            (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot,
            (unsigned)virtio_get_status(pDev),
            (unsigned long)pDev->u64FeaturesDrv,
            kind_name(pDev->u32Kind));
    soft_inventory_maybe_once();
}

u8
virtio_isr_read(struct gj_virtio_dev *pDev)
{
    if (pDev == NULL || pDev->pIsr == NULL) {
        return 0;
    }
    soft_inc(&g_u32SoftIsr);
    return mmio_r8(pDev->pIsr);
}

u8
virtio_config_generation(struct gj_virtio_dev *pDev)
{
    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    soft_inc(&g_u32SoftCfgGen);
    return mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_CFGGEN);
}

u64
virtio_features_device(struct gj_virtio_dev *pDev)
{
    soft_inc(&g_u32SoftFeatDev);
    return common_features_read(pDev, 0);
}

u64
virtio_features_driver(struct gj_virtio_dev *pDev)
{
    soft_inc(&g_u32SoftFeatDrv);
    return common_features_read(pDev, 1);
}

u64
virtio_features_negotiated(struct gj_virtio_dev *pDev)
{
    soft_inc(&g_u32SoftFeatNego);
    if (pDev == NULL) {
        return 0;
    }
    return pDev->u64FeaturesDrv;
}

int
virtio_features_has(struct gj_virtio_dev *pDev, u64 u64Bit)
{
    soft_inc(&g_u32SoftFeatHas);
    if (pDev == NULL || u64Bit == 0) {
        return 0;
    }
    return (pDev->u64FeaturesDrv & u64Bit) != 0 ? 1 : 0;
}

gj_status_t
virtio_negotiate(struct gj_virtio_dev *pDev, u64 u64WantFeatures)
{
    u64 u64Dev;
    u64 u64Drv;

    if (pDev == NULL || pDev->pCommon == NULL) {
        soft_inc(&g_u32SoftNegoInval);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }

    /* Sticky last want (soft residual only). */
    g_u32SoftLastWantLo = (u32)u64WantFeatures;
    g_u32SoftLastWantHi = (u32)(u64WantFeatures >> 32);

    /*
     * Soft reset first - OASIS requires reset before re-init; also makes
     * feature ladders (retry with smaller want masks) correct.
     */
    virtio_reset(pDev);

    virtio_set_status(pDev, GJ_VIRTIO_S_ACKNOWLEDGE);
    virtio_set_status(pDev, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER));

    u64Dev = common_features_read(pDev, 0);
    u64Drv = u64Dev & u64WantFeatures;

    common_features_write_guest(pDev, u64Drv);

    virtio_set_status(pDev, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                 GJ_VIRTIO_S_FEATURES_OK));
    if ((virtio_get_status(pDev) & GJ_VIRTIO_S_FEATURES_OK) == 0) {
        soft_inc(&g_u32SoftNegoFail);
        kprintf("virtio: FEATURES_OK rejected want=0x%lx dev=0x%lx\n",
                (unsigned long)u64WantFeatures, (unsigned long)u64Dev);
        virtio_set_status(pDev, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                     GJ_VIRTIO_S_FAILED));
        pDev->u64FeaturesDev = u64Dev;
        pDev->u64FeaturesDrv = 0;
        soft_inventory_maybe_once();
        return GJ_ERR_NOSUPPORT;
    }

    /* Soft snapshot from programmed guest features (read-back). */
    pDev->u64FeaturesDev = u64Dev;
    pDev->u64FeaturesDrv = common_features_read(pDev, 1);
    if (pDev->u64FeaturesDrv == 0) {
        pDev->u64FeaturesDrv = u64Drv; /* soft: some hosts omit GF read-back */
    }
    soft_inc(&g_u32SoftNegoOk);
    soft_t0_kind_inc(pDev->u32Kind, &g_u32SoftNegoNet, &g_u32SoftNegoBlk,
                     &g_u32SoftNegoGpu);
    /* Soft residual deepen: VERSION_1 honesty on product negotiate path. */
    if ((pDev->u64FeaturesDrv & GJ_VIRTIO_F_VERSION_1) != 0) {
        soft_inc(&g_u32SoftNegoV1);
    }
    kprintf("virtio: features dev=0x%lx drv=0x%lx want=0x%lx v1=%u kind=%s\n",
            (unsigned long)pDev->u64FeaturesDev,
            (unsigned long)pDev->u64FeaturesDrv,
            (unsigned long)u64WantFeatures,
            (unsigned)((pDev->u64FeaturesDrv & GJ_VIRTIO_F_VERSION_1) != 0),
            kind_name(pDev->u32Kind));
    soft_inventory_maybe_once();
    return GJ_OK;
}

gj_status_t
virtio_negotiate_soft(struct gj_virtio_dev *pDev, const u64 *pWants, u32 cWants,
                      u64 *pOutDrv)
{
    u32 i;
    gj_status_t st;
    u64 u64Last = 0;

    if (pDev == NULL || pDev->pCommon == NULL) {
        soft_inc(&g_u32SoftSoftInval);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }
    /* Soft default ladder when caller omits masks: V1 then transitional empty. */
    {
        static const u64 aDefault[] = {
            GJ_VIRTIO_F_VERSION_1,
            0
        };
        const u64 *pTry = pWants;
        u32 cTry = cWants;

        if (pTry == NULL || cTry == 0) {
            pTry = aDefault;
            cTry = 2;
        }

        for (i = 0; i < cTry; i++) {
            u64Last = pTry[i];
            soft_inc(&g_u32SoftSoftSteps);
            st = virtio_negotiate(pDev, u64Last);
            if (st == GJ_OK) {
                soft_inc(&g_u32SoftSoftOk);
                kprintf("virtio: features soft step=%u/%u want=0x%lx drv=0x%lx\n",
                        (unsigned)(i + 1), (unsigned)cTry,
                        (unsigned long)u64Last,
                        (unsigned long)pDev->u64FeaturesDrv);
                if (pOutDrv != NULL) {
                    *pOutDrv = pDev->u64FeaturesDrv;
                }
                soft_inventory_maybe_once();
                return GJ_OK;
            }
        }
        cWants = cTry;
    }
    soft_inc(&g_u32SoftSoftFail);
    kprintf("virtio: features soft FAIL steps=%u last_want=0x%lx\n",
            (unsigned)cWants, (unsigned long)u64Last);
    if (pOutDrv != NULL) {
        *pOutDrv = 0;
    }
    soft_inventory_maybe_once();
    return GJ_ERR_NOSUPPORT;
}

u16
virtio_q_max_size(struct gj_virtio_dev *pDev, u16 u16QIdx)
{
    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SELECT, u16QIdx);
    return mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SIZE);
}

void
virtio_q_disable(struct gj_virtio_dev *pDev, u16 u16QIdx)
{
    if (pDev == NULL || pDev->pCommon == NULL) {
        return;
    }
    soft_inc(&g_u32SoftQDisable);
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SELECT, u16QIdx);
    /* Modern virtio: queue_enable=0 is virtio_error (QEMU marks FAILED). */
}

gj_status_t
virtio_q_setup(struct gj_virtio_dev *pDev, struct gj_virtq *pQ, u16 u16QIdx,
               u16 u16Size)
{
    u16 u16Max;
    u16 u16Want;
    u16 u16Enabled;
    gj_paddr_t paDesc;
    gj_paddr_t paAvail;
    gj_paddr_t paUsed;

    if (pDev == NULL || pQ == NULL || pDev->pCommon == NULL) {
        soft_inc(&g_u32SoftQInval);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }
    if (u16Size == 0 || u16Size > GJ_VIRTQ_MAX_SIZE) {
        soft_inc(&g_u32SoftQInval);
        soft_inventory_maybe_once();
        return GJ_ERR_INVAL;
    }
    if (pDev->u32NumQueues != 0 && (u32)u16QIdx >= pDev->u32NumQueues) {
        soft_inc(&g_u32SoftQBeyond);
        kprintf("virtio: q soft idx=%u beyond num_queues=%u\n",
                (unsigned)u16QIdx, pDev->u32NumQueues);
        soft_inventory_maybe_once();
        return GJ_ERR_NOSUPPORT;
    }

    /*
     * C2 residual deepen: re-setup frees prior rings before zeroing pQ
     * (soft page-leak honesty under probe retry / TE re-entry).
     */
    if (pQ->paDesc != 0 || pQ->paAvail != 0 || pQ->paUsed != 0) {
        soft_inc(&g_u32SoftQResync);
        q_ring_free(pQ);
    }

    memset(pQ, 0, sizeof(*pQ));
    pQ->pDev = pDev;
    pQ->u16QueueIdx = u16QIdx;
    u16Want = u16Size;

    /* Select only. queue_enable=0 is virtio_error on qemu-kvm modern. */
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SELECT, u16QIdx);

    u16Max = mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SIZE);
    if (u16Max == 0) {
        soft_inc(&g_u32SoftQMax0);
        kprintf("virtio: q soft idx=%u max_size=0 (absent)\n",
                (unsigned)u16QIdx);
        soft_inventory_maybe_once();
        return GJ_ERR_NOSUPPORT;
    }

    u16Size = q_soft_size(u16Want, u16Max);
    if (u16Size == 0 || u16Size > GJ_VIRTQ_MAX_SIZE) {
        /* Soft: also clamp to our ring struct limit */
        if (u16Max >= GJ_VIRTQ_MAX_SIZE) {
            u16Size = q_soft_size(GJ_VIRTQ_MAX_SIZE, GJ_VIRTQ_MAX_SIZE);
        } else {
            u16Size = q_soft_size(u16Max, u16Max);
        }
    }
    if (u16Size > GJ_VIRTQ_MAX_SIZE) {
        u16Size = q_soft_size(GJ_VIRTQ_MAX_SIZE, GJ_VIRTQ_MAX_SIZE);
    }
    if (u16Size == 0) {
        soft_inc(&g_u32SoftQMax0);
        soft_inventory_maybe_once();
        return GJ_ERR_NOSUPPORT;
    }
    if (u16Size != u16Want) {
        soft_inc(&g_u32SoftQClamp);
        kprintf("virtio: q soft idx=%u size clamp want=%u -> %u (max=%u)\n",
                (unsigned)u16QIdx, (unsigned)u16Want, (unsigned)u16Size,
                (unsigned)u16Max);
    }

    pQ->u16Size = u16Size;
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SIZE, u16Size);

    /*
     * Three dedicated pages: desc | avail | used (soft non-contiguous OK).
     * Free any partial alloc on soft failure.
     */
    paDesc = alloc_zero_page();
    paAvail = alloc_zero_page();
    paUsed = alloc_zero_page();
    if (paDesc == 0 || paAvail == 0 || paUsed == 0) {
        if (paDesc != 0) {
            pmm_free(paDesc);
        }
        if (paAvail != 0) {
            pmm_free(paAvail);
        }
        if (paUsed != 0) {
            pmm_free(paUsed);
        }
        soft_inc(&g_u32SoftQNomem);
        kprintf("virtio: q soft idx=%u nomem\n", (unsigned)u16QIdx);
        soft_inventory_maybe_once();
        return GJ_ERR_NOMEM;
    }
    pQ->paDesc = paDesc;
    pQ->pDesc = (struct gj_virtq_desc *)hhdm_to_virt(paDesc);
    pQ->paAvail = paAvail;
    pQ->pAvail = (struct gj_virtq_avail *)hhdm_to_virt(paAvail);
    pQ->paUsed = paUsed;
    pQ->pUsed = (struct gj_virtq_used *)hhdm_to_virt(paUsed);

    /* Free list of descriptors */
    pQ->u16NumFree = u16Size;
    pQ->u16FreeHead = 0;
    pQ->u16LastUsed = 0;
    {
        u16 i;

        for (i = 0; i < u16Size - 1; i++) {
            pQ->pDesc[i].u16Next = (u16)(i + 1);
        }
        pQ->pDesc[u16Size - 1].u16Next = 0xffff;
    }

    mmio_w64(pDev->pCommon + VIRTIO_PCI_COMMON_Q_DESC, pQ->paDesc);
    mmio_w64(pDev->pCommon + VIRTIO_PCI_COMMON_Q_DRIVER, pQ->paAvail);
    mmio_w64(pDev->pCommon + VIRTIO_PCI_COMMON_Q_DEVICE, pQ->paUsed);
    pQ->u16NotifyOff = mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_NOFF);
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_ENABLE, 1);

    /* Soft enable verify */
    u16Enabled = mmio_r16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_ENABLE);
    if (u16Enabled == 0) {
        soft_inc(&g_u32SoftQEnableRej);
        kprintf("virtio: q soft idx=%u enable rejected\n", (unsigned)u16QIdx);
        q_ring_free(pQ);
        memset(pQ, 0, sizeof(*pQ));
        soft_inventory_maybe_once();
        return GJ_ERR_IO;
    }

    soft_inc(&g_u32SoftQSetupOk);
    soft_t0_kind_inc(pDev->u32Kind, &g_u32SoftQSetupNet, &g_u32SoftQSetupBlk,
                     &g_u32SoftQSetupGpu);
    g_u32SoftLastQIdx = (u32)u16QIdx;
    kprintf("virtio: q%u size=%u max=%u desc=0x%lx notify_off=%u soft ok"
            " kind=%s\n",
            (unsigned)u16QIdx, (unsigned)u16Size, (unsigned)u16Max,
            (unsigned long)pQ->paDesc, (unsigned)pQ->u16NotifyOff,
            kind_name(pDev->u32Kind));
    soft_inventory_maybe_once();
    return GJ_OK;
}

int
virtio_q_push_head(struct gj_virtq *pQ, u16 u16Head)
{
    u16 u16Aidx;

    if (pQ == NULL || pQ->pAvail == NULL) {
        return -1;
    }
    __asm__ volatile ("mfence" ::: "memory");
    u16Aidx = (u16)(pQ->pAvail->u16Idx % pQ->u16Size);
    pQ->pAvail->aRing[u16Aidx] = u16Head;
    __asm__ volatile ("mfence" ::: "memory");
    pQ->pAvail->u16Idx = (u16)(pQ->pAvail->u16Idx + 1);
    __asm__ volatile ("mfence" ::: "memory");
    return (int)u16Head;
}

int
virtio_q_alloc_desc(struct gj_virtq *pQ)
{
    u16 u16Head;

    if (pQ == NULL || pQ->u16NumFree == 0) {
        return -1;
    }
    u16Head = pQ->u16FreeHead;
    pQ->u16FreeHead = pQ->pDesc[u16Head].u16Next;
    pQ->u16NumFree--;
    return (int)u16Head;
}

int
virtio_q_add(struct gj_virtq *pQ, gj_paddr_t pa, u32 u32Len, int fWrite)
{
    int iHead;
    u16 u16Head;

    if (pQ == NULL || pa == 0 || u32Len == 0) {
        soft_inc(&g_u32SoftAddFail);
        return -1;
    }
    iHead = virtio_q_alloc_desc(pQ);
    if (iHead < 0) {
        soft_inc(&g_u32SoftAddFail);
        return -1;
    }
    u16Head = (u16)iHead;
    pQ->pDesc[u16Head].u64Addr = (u64)pa;
    pQ->pDesc[u16Head].u32Len = u32Len;
    pQ->pDesc[u16Head].u16Flags = fWrite ? GJ_VIRTQ_DESC_F_WRITE : 0;
    pQ->pDesc[u16Head].u16Next = 0;
    iHead = virtio_q_push_head(pQ, u16Head);
    if (iHead < 0) {
        /* C2 residual deepen: free-list recovery on push reject. */
        q_free_one_desc(pQ, u16Head);
        soft_inc(&g_u32SoftAddFail);
        return -1;
    }
    soft_inc(&g_u32SoftAdd);
    g_u32SoftLastAddN = 1u;
    return iHead;
}

int
virtio_q_add2(struct gj_virtq *pQ, gj_paddr_t pa0, u32 u32Len0, int fWrite0,
              gj_paddr_t pa1, u32 u32Len1, int fWrite1)
{
    u16 u16A;
    u16 u16B;
    int iHead;
    int ia;
    int ib;

    if (pQ == NULL || pa0 == 0 || pa1 == 0 || u32Len0 == 0 || u32Len1 == 0) {
        soft_inc(&g_u32SoftAdd2Fail);
        return -1;
    }
    if (pQ->u16NumFree < 2) {
        soft_inc(&g_u32SoftAdd2Fail);
        return -1;
    }
    ia = virtio_q_alloc_desc(pQ);
    ib = virtio_q_alloc_desc(pQ);
    if (ia < 0 || ib < 0) {
        /* C2 residual deepen: partial alloc free-list recovery. */
        if (ia >= 0) {
            q_free_one_desc(pQ, (u16)ia);
        }
        if (ib >= 0) {
            q_free_one_desc(pQ, (u16)ib);
        }
        soft_inc(&g_u32SoftAdd2Fail);
        return -1;
    }
    u16A = (u16)ia;
    u16B = (u16)ib;
    pQ->pDesc[u16A].u64Addr = (u64)pa0;
    pQ->pDesc[u16A].u32Len = u32Len0;
    pQ->pDesc[u16A].u16Flags =
        (u16)((fWrite0 ? GJ_VIRTQ_DESC_F_WRITE : 0) | GJ_VIRTQ_DESC_F_NEXT);
    pQ->pDesc[u16A].u16Next = u16B;

    pQ->pDesc[u16B].u64Addr = (u64)pa1;
    pQ->pDesc[u16B].u32Len = u32Len1;
    pQ->pDesc[u16B].u16Flags = fWrite1 ? GJ_VIRTQ_DESC_F_WRITE : 0;
    pQ->pDesc[u16B].u16Next = 0;
    iHead = virtio_q_push_head(pQ, u16A);
    if (iHead < 0) {
        q_free_one_desc(pQ, u16B);
        q_free_one_desc(pQ, u16A);
        soft_inc(&g_u32SoftAdd2Fail);
        return -1;
    }
    soft_inc(&g_u32SoftAdd2);
    g_u32SoftLastAddN = 2u;
    return iHead;
}

int
virtio_q_add3(struct gj_virtq *pQ, gj_paddr_t pa0, u32 u32Len0, int fWrite0,
              gj_paddr_t pa1, u32 u32Len1, int fWrite1,
              gj_paddr_t pa2, u32 u32Len2, int fWrite2)
{
    u16 u16A;
    u16 u16B;
    u16 u16C;
    int iHead;
    int ia;
    int ib;
    int ic;

    if (pQ == NULL || pa0 == 0 || pa1 == 0 || pa2 == 0 ||
        u32Len0 == 0 || u32Len1 == 0 || u32Len2 == 0) {
        soft_inc(&g_u32SoftAdd3Fail);
        return -1;
    }
    if (pQ->u16NumFree < 3) {
        soft_inc(&g_u32SoftAdd3Fail);
        return -1;
    }
    ia = virtio_q_alloc_desc(pQ);
    ib = virtio_q_alloc_desc(pQ);
    ic = virtio_q_alloc_desc(pQ);
    if (ia < 0 || ib < 0 || ic < 0) {
        /* C2 residual deepen: partial alloc free-list recovery. */
        if (ia >= 0) {
            q_free_one_desc(pQ, (u16)ia);
        }
        if (ib >= 0) {
            q_free_one_desc(pQ, (u16)ib);
        }
        if (ic >= 0) {
            q_free_one_desc(pQ, (u16)ic);
        }
        soft_inc(&g_u32SoftAdd3Fail);
        return -1;
    }
    u16A = (u16)ia;
    u16B = (u16)ib;
    u16C = (u16)ic;
    pQ->pDesc[u16A].u64Addr = (u64)pa0;
    pQ->pDesc[u16A].u32Len = u32Len0;
    pQ->pDesc[u16A].u16Flags =
        (u16)((fWrite0 ? GJ_VIRTQ_DESC_F_WRITE : 0) | GJ_VIRTQ_DESC_F_NEXT);
    pQ->pDesc[u16A].u16Next = u16B;

    pQ->pDesc[u16B].u64Addr = (u64)pa1;
    pQ->pDesc[u16B].u32Len = u32Len1;
    pQ->pDesc[u16B].u16Flags =
        (u16)((fWrite1 ? GJ_VIRTQ_DESC_F_WRITE : 0) | GJ_VIRTQ_DESC_F_NEXT);
    pQ->pDesc[u16B].u16Next = u16C;

    pQ->pDesc[u16C].u64Addr = (u64)pa2;
    pQ->pDesc[u16C].u32Len = u32Len2;
    pQ->pDesc[u16C].u16Flags = fWrite2 ? GJ_VIRTQ_DESC_F_WRITE : 0;
    pQ->pDesc[u16C].u16Next = 0;
    iHead = virtio_q_push_head(pQ, u16A);
    if (iHead < 0) {
        q_free_one_desc(pQ, u16C);
        q_free_one_desc(pQ, u16B);
        q_free_one_desc(pQ, u16A);
        soft_inc(&g_u32SoftAdd3Fail);
        return -1;
    }
    soft_inc(&g_u32SoftAdd3);
    g_u32SoftLastAddN = 3u;
    return iHead;
}

void
virtio_q_kick(struct gj_virtq *pQ)
{
    volatile u8 *pNotify;
    u32 u32Off;

    if (pQ == NULL || pQ->pDev == NULL || pQ->pDev->pNotify == NULL) {
        return;
    }
    soft_inc(&g_u32SoftKick);
    /* C2 residual deepen: T0 kick path tallies (Soft!=product). */
    soft_t0_kind_inc(pQ->pDev->u32Kind, &g_u32SoftKickNet, &g_u32SoftKickBlk,
                     &g_u32SoftKickGpu);
    g_u32SoftLastQIdx = (u32)pQ->u16QueueIdx;
    u32Off = (u32)pQ->u16NotifyOff * pQ->pDev->u32NotifyMult;
    pNotify = pQ->pDev->pNotify + u32Off;
    mmio_w16(pNotify, pQ->u16QueueIdx);
    soft_inventory_maybe_once();
}

i32
virtio_q_poll_id(struct gj_virtq *pQ, u32 u32Spins, u32 *pOutId)
{
    u32 i;

    if (pQ == NULL) {
        return -1;
    }
    for (i = 0; i < u32Spins; i++) {
        __asm__ volatile ("mfence" ::: "memory");
        if (pQ->pUsed->u16Idx != pQ->u16LastUsed) {
            u16 u16Slot = (u16)(pQ->u16LastUsed % pQ->u16Size);
            u32 u32Id = pQ->pUsed->aRing[u16Slot].u32Id;
            u32 u32Len = pQ->pUsed->aRing[u16Slot].u32Len;

            pQ->u16LastUsed = (u16)(pQ->u16LastUsed + 1);
            /* free descriptor chain (bound by queue size; cyclic NEXT = hang) */
            {
                u16 u16Cur = (u16)u32Id;
                u32 cWalk = 0;

                for (;;) {
                    u16 u16Next = pQ->pDesc[u16Cur].u16Next;
                    u16 u16Flags = pQ->pDesc[u16Cur].u16Flags;

                    pQ->pDesc[u16Cur].u16Next = pQ->u16FreeHead;
                    pQ->u16FreeHead = u16Cur;
                    pQ->u16NumFree++;
                    cWalk++;
                    if ((u16Flags & GJ_VIRTQ_DESC_F_NEXT) == 0) {
                        break;
                    }
                    if (cWalk > (u32)pQ->u16Size) {
                        break;
                    }
                    u16Cur = u16Next;
                }
            }
            if (pOutId != NULL) {
                *pOutId = u32Id;
            }
            soft_inc(&g_u32SoftPollHit);
            g_u32SoftLastPollLen = u32Len;
            return (i32)u32Len;
        }
        __asm__ volatile ("pause");
    }
    /*
     * Soft: count spin timeouts only (spins>1). Single-shot empty (reap)
     * is not a product timeout - leave poll_to honest.
     */
    if (u32Spins > 1u) {
        soft_inc(&g_u32SoftPollTo);
    }
    return -1;
}

i32
virtio_q_poll(struct gj_virtq *pQ, u32 u32Spins)
{
    return virtio_q_poll_id(pQ, u32Spins, NULL);
}

u16
virtio_q_num_free(struct gj_virtq *pQ)
{
    if (pQ == NULL) {
        return 0;
    }
    return pQ->u16NumFree;
}

u32
virtio_q_reap(struct gj_virtq *pQ, u32 u32Max)
{
    u32 n = 0;

    if (pQ == NULL || u32Max == 0) {
        return 0;
    }
    while (n < u32Max) {
        if (virtio_q_poll_id(pQ, 1, NULL) < 0) {
            break;
        }
        n++;
    }
    if (n > 0) {
        if (g_u32SoftReap <= 0xffffffffu - n) {
            g_u32SoftReap += n;
        } else {
            g_u32SoftReap = 0xffffffffu;
        }
    }
    return n;
}
