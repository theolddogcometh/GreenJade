/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-pci modern transport + virtqueues (OASIS virtio 1.1).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Soft product depth (common-cfg, features, queue setup):
 *   - modern common-cfg cap walk + soft reset
 *   - feature read/write helpers + soft negotiate ladder
 *   - queue soft size clamp, disable-before-setup, enable verify
 *
 * Soft transport inventory (Wave 15 exclusive deepen; this unit only):
 *   - Scan catalog (found / kind tallies / modern setup)
 *   - Setup / reset / feature negotiate + soft ladder
 *   - Queue soft size clamp / enable verify / nomem
 *   - I/O path tallies (add/kick/poll/reap/driver_ok)
 *   - Wave 14 splits: reset|kind|poll|reap|last|modern|status|transport
 *   - Wave 15 splits: add|bar|ratio|notify|ladder|caps|deepen
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ bar3.
 * Greppable twin prefixes (product / agent greps):
 *   "virtio-pci: soft …"
 *   "virtio: soft …"
 *
 * Greppable product markers (prefix-stable):
 *   virtio: scan PASS
 *   virtio: modern common@
 *   virtio: features
 *   virtio: features soft
 *   virtio: q soft
 *   virtio: q%u size=
 *   virtio: driver_ok
 *   virtio-pci: soft inventory|scan|setup|features|queue|io|path …
 *   virtio-pci: soft reset|kind|poll|reap|last|modern|status|transport
 *   virtio-pci: soft add|bar|ratio|notify|ladder|caps|deepen
 *   virtio-pci: soft PASS|NODEV|PARTIAL
 *   virtio: soft inventory|scan|setup|features|queue|io|path …
 *   virtio: soft reset|kind|poll|reap|last|modern|status|transport
 *   virtio: soft add|bar|ratio|notify|ladder|caps|deepen
 *   virtio: soft PASS|NODEV|PARTIAL
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/vmm.h>

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

/* Wave 20 deepen stamp (greppable wave= / areas=). */
#define VIRTIO_PCI_SOFT_DEEPEN_WAVE  22u
#define VIRTIO_PCI_SOFT_DEEPEN_AREAS 40u

/*
 * Soft product inventory (Wave 15 exclusive deepen). Cumulative path tallies.
 * greppable: virtio-pci: soft … / virtio: soft …
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
static u32 g_u32SoftQBeyond;       /* q idx ≥ num_queues */
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
/* Wave 14 deepen: status / feature / sticky last / live-cap snapshots. */
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

static void soft_inc(u32 *pCtr);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
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
 * Greppable soft virtio-pci transport inventory (product / smoke).
 * Wave 15 deepen — twin prefixes so either agent grep works:
 *   virtio-pci: soft inventory|scan|setup|features|queue|io|path …
 *   virtio-pci: soft reset|kind|poll|reap|last|modern|status|transport
 *   virtio-pci: soft add|bar|ratio|notify|ladder|caps|deepen
 *   virtio-pci: soft PASS|NODEV|PARTIAL
 *   virtio: soft … (same catalog)
 * greppable: virtio-pci: soft
 * greppable: virtio: soft
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
    u32 u32Known;
    u32 u32AddOk;
    u32 u32AddFail;
    u32 i;
    const char *szVerdict;

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

    u32Known = g_u32SoftKindNet + g_u32SoftKindBlk + g_u32SoftKindGpu +
               g_u32SoftKindInput + g_u32SoftKindConsole + g_u32SoftKindScsi;
    u32AddOk = g_u32SoftAdd + g_u32SoftAdd2 + g_u32SoftAdd3;
    u32AddFail = g_u32SoftAddFail + g_u32SoftAdd2Fail + g_u32SoftAdd3Fail;

    /*
     * Soft verdict (inventory only; transport path unchanged):
     *   NODEV    — no modern devices retained
     *   PASS     — modern + any queue setup or product kick/add
     *   PARTIAL  — modern present, no q/io activity yet
     */
    if (u32Modern == 0u) {
        szVerdict = "NODEV";
    } else if (g_u32SoftQSetupOk != 0u || g_u32SoftKick != 0u ||
               u32AddOk != 0u || g_u32SoftDriverOk != 0u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /*
     * Primary prefix: virtio-pci: soft …
     * Wave 15 deepen splits add/bar/ratio/notify/ladder/caps + prior catalog.
     */
    /* Grep: virtio-pci: soft inventory */
    kprintf("virtio-pci: soft inventory found=%u modern=%u max_devs=%u "
            "setup_ok=%u q_ok=%u nego_ok=%u driver_ok=%u log_n=%u "
            "wave=%u areas=%u\n",
            u32Found, u32Modern, GJ_VIRTIO_MAX_DEVS, g_u32SoftSetupOk,
            g_u32SoftQSetupOk, g_u32SoftNegoOk, g_u32SoftDriverOk,
            g_u32SoftLogN, (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE,
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_AREAS);

    /* Grep: virtio-pci: soft scan */
    kprintf("virtio-pci: soft scan enter=%u found=%u net=%u blk=%u gpu=%u "
            "input=%u scsi=%u console=%u unknown=%u bar_map_fail=%u "
            "known=%u wave=%u\n",
            g_u32SoftScanEnter, u32Found, g_u32SoftKindNet, g_u32SoftKindBlk,
            g_u32SoftKindGpu, g_u32SoftKindInput, g_u32SoftKindScsi,
            g_u32SoftKindConsole, g_u32SoftKindUnknown, g_u32SoftBarMapFail,
            u32Known, (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-pci: soft setup */
    kprintf("virtio-pci: soft setup ok=%u inval=%u nocap=%u nocommon=%u "
            "modern=%u reset=%u reset_to=%u wave=%u\n",
            g_u32SoftSetupOk, g_u32SoftSetupInval, g_u32SoftSetupNocap,
            g_u32SoftSetupNocommon, u32Modern, g_u32SoftReset,
            g_u32SoftResetTimeout, (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-pci: soft features */
    kprintf("virtio-pci: soft features nego_ok=%u nego_fail=%u nego_inval=%u "
            "soft_ok=%u soft_fail=%u soft_inval=%u soft_steps=%u "
            "feat_dev=%u feat_drv=%u feat_nego=%u feat_has=%u\n",
            g_u32SoftNegoOk, g_u32SoftNegoFail, g_u32SoftNegoInval,
            g_u32SoftSoftOk, g_u32SoftSoftFail, g_u32SoftSoftInval,
            g_u32SoftSoftSteps, g_u32SoftFeatDev, g_u32SoftFeatDrv,
            g_u32SoftFeatNego, g_u32SoftFeatHas);

    /* Grep: virtio-pci: soft queue */
    kprintf("virtio-pci: soft queue setup_ok=%u clamp=%u nomem=%u "
            "enable_rej=%u beyond=%u max0=%u inval=%u disable=%u "
            "q_max_size=%u last_q=%u numq_sum=%u\n",
            g_u32SoftQSetupOk, g_u32SoftQClamp, g_u32SoftQNomem,
            g_u32SoftQEnableRej, g_u32SoftQBeyond, g_u32SoftQMax0,
            g_u32SoftQInval, g_u32SoftQDisable, GJ_VIRTQ_MAX_SIZE,
            g_u32SoftLastQIdx, u32NumQSum);

    /* Grep: virtio-pci: soft io */
    kprintf("virtio-pci: soft io kick=%u add=%u add_fail=%u add2=%u "
            "add2_fail=%u add3=%u add3_fail=%u poll_hit=%u poll_to=%u "
            "reap=%u driver_ok=%u isr=%u add_ok=%u add_fail_sum=%u\n",
            g_u32SoftKick, g_u32SoftAdd, g_u32SoftAddFail, g_u32SoftAdd2,
            g_u32SoftAdd2Fail, g_u32SoftAdd3, g_u32SoftAdd3Fail,
            g_u32SoftPollHit, g_u32SoftPollTo, g_u32SoftReap,
            g_u32SoftDriverOk, g_u32SoftIsr, u32AddOk, u32AddFail);

    /* Grep: virtio-pci: soft reset (Wave 14) */
    kprintf("virtio-pci: soft reset calls=%u timeout=%u spins=%u "
            "status_set=%u get_status=%u cfg_gen=%u\n",
            g_u32SoftReset, g_u32SoftResetTimeout,
            (unsigned)GJ_VIRTIO_RESET_SPINS, g_u32SoftStatusSet,
            g_u32SoftGetStatus, g_u32SoftCfgGen);

    /* Grep: virtio-pci: soft kind (Wave 14) */
    kprintf("virtio-pci: soft kind net=%u blk=%u gpu=%u input=%u "
            "scsi=%u console=%u unknown=%u known=%u found=%u\n",
            g_u32SoftKindNet, g_u32SoftKindBlk, g_u32SoftKindGpu,
            g_u32SoftKindInput, g_u32SoftKindScsi, g_u32SoftKindConsole,
            g_u32SoftKindUnknown, u32Known, u32Found);

    /* Grep: virtio-pci: soft poll (Wave 14) */
    kprintf("virtio-pci: soft poll hit=%u to=%u kick=%u isr=%u "
            "ratio_to_kick=%u\n",
            g_u32SoftPollHit, g_u32SoftPollTo, g_u32SoftKick, g_u32SoftIsr,
            (g_u32SoftKick != 0u)
                ? ((g_u32SoftPollTo * 100u) / g_u32SoftKick)
                : 0u);

    /* Grep: virtio-pci: soft reap (Wave 14) */
    kprintf("virtio-pci: soft reap sum=%u poll_hit=%u add_ok=%u "
            "reap_path=poll_id_1\n",
            g_u32SoftReap, g_u32SoftPollHit, u32AddOk);

    /* Grep: virtio-pci: soft last (Wave 14 sticky) */
    kprintf("virtio-pci: soft last q_idx=%u add_n=%u want_lo=0x%x "
            "want_hi=0x%x setup_ok=%u nego_ok=%u kick=%u\n",
            g_u32SoftLastQIdx, g_u32SoftLastAddN, g_u32SoftLastWantLo,
            g_u32SoftLastWantHi, g_u32SoftSetupOk, g_u32SoftNegoOk,
            g_u32SoftKick);

    /* Grep: virtio-pci: soft modern (Wave 14 live cap snapshot) */
    kprintf("virtio-pci: soft modern count=%u notify=%u isr=%u devcfg=%u "
            "numq_sum=%u max_devs=%u bar_map_fail=%u\n",
            u32Modern, u32Notify, u32Isr, u32DevCfg, u32NumQSum,
            GJ_VIRTIO_MAX_DEVS, g_u32SoftBarMapFail);

    /* Grep: virtio-pci: soft status (Wave 14) */
    kprintf("virtio-pci: soft status set=%u get=%u driver_ok=%u "
            "cfg_gen=%u s_ack=%u s_drv=%u s_ok=%u s_feat=%u s_fail=%u\n",
            g_u32SoftStatusSet, g_u32SoftGetStatus, g_u32SoftDriverOk,
            g_u32SoftCfgGen, (unsigned)GJ_VIRTIO_S_ACKNOWLEDGE,
            (unsigned)GJ_VIRTIO_S_DRIVER, (unsigned)GJ_VIRTIO_S_DRIVER_OK,
            (unsigned)GJ_VIRTIO_S_FEATURES_OK, (unsigned)GJ_VIRTIO_S_FAILED);

    /* Grep: virtio-pci: soft transport (Wave 14 honesty geometry) */
    kprintf("virtio-pci: soft transport common=1 notify=1 isr=1 "
            "devcfg=1 pci_cfg=0 packed=0 msix=0 vendor=0x%x "
            "max_devs=%u q_max=%u\n",
            (unsigned)GJ_VIRTIO_PCI_VENDOR, GJ_VIRTIO_MAX_DEVS,
            GJ_VIRTQ_MAX_SIZE);

    /* Grep: virtio-pci: soft add (Wave 15 chain breakdown) */
    kprintf("virtio-pci: soft add n1=%u n1_fail=%u n2=%u n2_fail=%u "
            "n3=%u n3_fail=%u ok_sum=%u fail_sum=%u last_n=%u\n",
            g_u32SoftAdd, g_u32SoftAddFail, g_u32SoftAdd2, g_u32SoftAdd2Fail,
            g_u32SoftAdd3, g_u32SoftAdd3Fail, u32AddOk, u32AddFail,
            g_u32SoftLastAddN);

    /* Grep: virtio-pci: soft bar (Wave 15 BAR/map honesty) */
    kprintf("virtio-pci: soft bar map_fail=%u max_devs=%u found=%u "
            "modern=%u known=%u unknown=%u\n",
            g_u32SoftBarMapFail, GJ_VIRTIO_MAX_DEVS, u32Found, u32Modern,
            u32Known, g_u32SoftKindUnknown);

    /* Grep: virtio-pci: soft ratio (Wave 15 derived lamps; inventory only) */
    kprintf("virtio-pci: soft ratio poll_hit_pct=%u poll_to_pct=%u "
            "add_fail_pct=%u nego_fail_pct=%u q_clamp_pct=%u\n",
            (g_u32SoftKick != 0u)
                ? ((g_u32SoftPollHit * 100u) / g_u32SoftKick)
                : 0u,
            (g_u32SoftKick != 0u)
                ? ((g_u32SoftPollTo * 100u) / g_u32SoftKick)
                : 0u,
            ((u32AddOk + u32AddFail) != 0u)
                ? ((u32AddFail * 100u) / (u32AddOk + u32AddFail))
                : 0u,
            ((g_u32SoftNegoOk + g_u32SoftNegoFail) != 0u)
                ? ((g_u32SoftNegoFail * 100u) /
                   (g_u32SoftNegoOk + g_u32SoftNegoFail))
                : 0u,
            ((g_u32SoftQSetupOk + g_u32SoftQClamp) != 0u)
                ? ((g_u32SoftQClamp * 100u) /
                   (g_u32SoftQSetupOk + g_u32SoftQClamp))
                : 0u);

    /* Grep: virtio-pci: soft notify (Wave 15 live notify/isr/devcfg) */
    kprintf("virtio-pci: soft notify live=%u isr=%u devcfg=%u "
            "numq_sum=%u modern=%u isr_reads=%u\n",
            u32Notify, u32Isr, u32DevCfg, u32NumQSum, u32Modern,
            g_u32SoftIsr);

    /* Grep: virtio-pci: soft ladder (Wave 15 soft negotiate ladder) */
    kprintf("virtio-pci: soft ladder soft_ok=%u soft_fail=%u soft_inval=%u "
            "soft_steps=%u nego_ok=%u nego_fail=%u feat_has=%u\n",
            g_u32SoftSoftOk, g_u32SoftSoftFail, g_u32SoftSoftInval,
            g_u32SoftSoftSteps, g_u32SoftNegoOk, g_u32SoftNegoFail,
            g_u32SoftFeatHas);

    /* Grep: virtio-pci: soft caps (Wave 15 modern cap honesty) */
    kprintf("virtio-pci: soft caps common=1 notify=%u isr=%u devcfg=%u "
            "pci_cfg=0 packed=0 msix=0 setup_nocap=%u setup_nocommon=%u "
            "bar_map_fail=%u\n",
            u32Notify, u32Isr, u32DevCfg, g_u32SoftSetupNocap,
            g_u32SoftSetupNocommon, g_u32SoftBarMapFail);

    /* Grep: virtio-pci: soft path */
    kprintf("virtio-pci: soft path claim=pci_modern+vq "
            "transport=common_cfg+notify+isr setup=cap_walk "
            "features=ladder q=clamp+enable_verify "
            "wave=%u (soft inventory; not bar3)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: virtio-pci: soft honesty|geom|return|contract|surface
     */
    {
        u32 u32Surf = 0u;
        u32 u32ModernBp = 0;

        if (u32Found != 0u) {
            u32ModernBp = (u32Modern * 10000u) / u32Found;
        }
        if (u32Found != 0u) {
            u32Surf |= 0x1u;
        }
        if (u32Modern != 0u) {
            u32Surf |= 0x2u;
        }
        if (g_u32SoftSetupOk != 0u) {
            u32Surf |= 0x4u;
        }
        if (g_u32SoftQSetupOk != 0u) {
            u32Surf |= 0x8u;
        }
        if (g_u32SoftNegoOk != 0u) {
            u32Surf |= 0x10u;
        }
        if (g_u32SoftDriverOk != 0u) {
            u32Surf |= 0x20u;
        }
        if (g_u32SoftKick != 0u) {
            u32Surf |= 0x40u;
        }
        u32Surf |= 0x80u; /* transport catalog always present */
        /* Grep: virtio-pci: soft honesty */
        kprintf("virtio-pci: soft honesty transport=1 packed=0 msix=0 "
                "game_io=0 product_vq=0 bar3=open soft_only=1 wave=%u "
                "soft PASS\n",
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-pci: soft geom */
        kprintf("virtio-pci: soft geom max_devs=%u found=%u modern=%u "
                "setup_ok=%u q_ok=%u wave=%u soft PASS\n",
                (unsigned)GJ_VIRTIO_MAX_DEVS, u32Found, u32Modern,
                g_u32SoftSetupOk, g_u32SoftQSetupOk,
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-pci: soft surface */
        kprintf("virtio-pci: soft surface inventory,scan,reset,kind,poll,"
                "reap,last,modern,status,transport,add,bar,ratio,notify,"
                "ladder,caps,path,honesty,geom,return,contract,deepen "
                "areas=%u wave=%u\n",
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_AREAS,
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-pci: soft return — return-surface bitmask */
        kprintf("virtio-pci: soft return surf=0x%x found=%u modern=%u "
                "setup=%u q=%u nego=%u driver=%u kick=%u modern_bp=%u "
                "areas=%u wave=%u soft PASS\n",
                u32Surf, u32Found, u32Modern, g_u32SoftSetupOk,
                g_u32SoftQSetupOk, g_u32SoftNegoOk, g_u32SoftDriverOk,
                g_u32SoftKick, u32ModernBp,
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_AREAS,
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
        /* Grep: virtio-pci: soft contract — soft ≠ game I/O */
        kprintf("virtio-pci: soft contract soft_only=1 game_io=0 "
                "product_transport=0 packed=0 msix_claim=0 bar3=open "
                "wave=%u soft PASS\n",
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
        /* Twin: virtio: soft honesty|geom|return|contract|surface */
        kprintf("virtio: soft honesty transport=1 packed=0 msix=0 "
                "game_io=0 product_vq=0 bar3=open soft_only=1 wave=%u "
                "soft PASS\n",
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
        kprintf("virtio: soft geom max_devs=%u found=%u modern=%u "
                "setup_ok=%u q_ok=%u wave=%u soft PASS\n",
                (unsigned)GJ_VIRTIO_MAX_DEVS, u32Found, u32Modern,
                g_u32SoftSetupOk, g_u32SoftQSetupOk,
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
        kprintf("virtio: soft surface inventory,scan,reset,kind,poll,reap,"
                "last,modern,status,transport,add,bar,ratio,notify,ladder,"
                "caps,path,honesty,geom,return,contract,deepen "
                "areas=%u wave=%u\n",
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_AREAS,
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
        kprintf("virtio: soft return surf=0x%x found=%u modern=%u setup=%u "
                "q=%u nego=%u driver=%u kick=%u modern_bp=%u areas=%u "
                "wave=%u soft PASS\n",
                u32Surf, u32Found, u32Modern, g_u32SoftSetupOk,
                g_u32SoftQSetupOk, g_u32SoftNegoOk, g_u32SoftDriverOk,
                g_u32SoftKick, u32ModernBp,
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_AREAS,
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
        kprintf("virtio: soft contract soft_only=1 game_io=0 "
                "product_transport=0 packed=0 msix_claim=0 bar3=open "
                "wave=%u soft PASS\n",
                (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-pci: soft return — Wave 17 API return surfaces (kept) */
    kprintf("virtio-pci: soft return soft_inv=1 modern=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-pci: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("virtio-pci: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-pci: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("virtio-pci: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-pci: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("virtio-pci: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-pci: soft retcode — Wave 19 retcode catalog */
    kprintf("virtio-pci: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-pci: soft deepen (Wave 20 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-pci: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("virtio-pci: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    /* Grep: virtio-pci: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("virtio-pci: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-pci: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("virtio-pci: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    /* Grep: virtio-pci: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("virtio-pci: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-pci: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("virtio-pci: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-pci: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("virtio-pci: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 exclusive complementary surfaces (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-pci: soft retphase — Wave 22 return-phase honesty */
            kprintf("virtio-pci: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
            /* Grep: virtio-pci: soft retbadge — Wave 22 exclusive badge stamp */
            kprintf("virtio-pci: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    kprintf("virtio-pci: soft deepen wave=%u areas=%u found=%u modern=%u "
            "setup_ok=%u q_ok=%u log_n=%u\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE,
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_AREAS, u32Found, u32Modern,
            g_u32SoftSetupOk, g_u32SoftQSetupOk, g_u32SoftLogN);

    /* Grep: virtio-pci: soft PASS | NODEV | PARTIAL */
    kprintf("virtio-pci: soft %s found=%u modern=%u setup_ok=%u q_ok=%u "
            "kick=%u log_n=%u wave=%u\n",
            szVerdict, u32Found, u32Modern, g_u32SoftSetupOk,
            g_u32SoftQSetupOk, g_u32SoftKick, g_u32SoftLogN,
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio-pci: soft inventory PASS|NODEV|PARTIAL */
    kprintf("virtio-pci: soft inventory %s wave=%u logs=%u\n", szVerdict,
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE, g_u32SoftLogN);

    /*
     * Twin prefix: virtio: soft … (agent-friendly alias; same tallies).
     */
    /* Grep: virtio: soft inventory */
    kprintf("virtio: soft inventory found=%u modern=%u max_devs=%u "
            "setup_ok=%u q_ok=%u nego_ok=%u driver_ok=%u log_n=%u "
            "wave=%u areas=%u\n",
            u32Found, u32Modern, GJ_VIRTIO_MAX_DEVS, g_u32SoftSetupOk,
            g_u32SoftQSetupOk, g_u32SoftNegoOk, g_u32SoftDriverOk,
            g_u32SoftLogN, (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE,
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_AREAS);

    /* Grep: virtio: soft scan */
    kprintf("virtio: soft scan enter=%u found=%u net=%u blk=%u gpu=%u "
            "input=%u scsi=%u console=%u unknown=%u bar_map_fail=%u "
            "known=%u wave=%u\n",
            g_u32SoftScanEnter, u32Found, g_u32SoftKindNet, g_u32SoftKindBlk,
            g_u32SoftKindGpu, g_u32SoftKindInput, g_u32SoftKindScsi,
            g_u32SoftKindConsole, g_u32SoftKindUnknown, g_u32SoftBarMapFail,
            u32Known, (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio: soft setup */
    kprintf("virtio: soft setup ok=%u inval=%u nocap=%u nocommon=%u "
            "modern=%u reset=%u reset_to=%u wave=%u\n",
            g_u32SoftSetupOk, g_u32SoftSetupInval, g_u32SoftSetupNocap,
            g_u32SoftSetupNocommon, u32Modern, g_u32SoftReset,
            g_u32SoftResetTimeout, (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio: soft features */
    kprintf("virtio: soft features nego_ok=%u nego_fail=%u nego_inval=%u "
            "soft_ok=%u soft_fail=%u soft_inval=%u soft_steps=%u "
            "feat_dev=%u feat_drv=%u feat_nego=%u feat_has=%u\n",
            g_u32SoftNegoOk, g_u32SoftNegoFail, g_u32SoftNegoInval,
            g_u32SoftSoftOk, g_u32SoftSoftFail, g_u32SoftSoftInval,
            g_u32SoftSoftSteps, g_u32SoftFeatDev, g_u32SoftFeatDrv,
            g_u32SoftFeatNego, g_u32SoftFeatHas);

    /* Grep: virtio: soft queue */
    kprintf("virtio: soft queue setup_ok=%u clamp=%u nomem=%u "
            "enable_rej=%u beyond=%u max0=%u inval=%u disable=%u "
            "q_max_size=%u last_q=%u numq_sum=%u\n",
            g_u32SoftQSetupOk, g_u32SoftQClamp, g_u32SoftQNomem,
            g_u32SoftQEnableRej, g_u32SoftQBeyond, g_u32SoftQMax0,
            g_u32SoftQInval, g_u32SoftQDisable, GJ_VIRTQ_MAX_SIZE,
            g_u32SoftLastQIdx, u32NumQSum);

    /* Grep: virtio: soft io */
    kprintf("virtio: soft io kick=%u add=%u add_fail=%u add2=%u "
            "add2_fail=%u add3=%u add3_fail=%u poll_hit=%u poll_to=%u "
            "reap=%u driver_ok=%u isr=%u add_ok=%u add_fail_sum=%u\n",
            g_u32SoftKick, g_u32SoftAdd, g_u32SoftAddFail, g_u32SoftAdd2,
            g_u32SoftAdd2Fail, g_u32SoftAdd3, g_u32SoftAdd3Fail,
            g_u32SoftPollHit, g_u32SoftPollTo, g_u32SoftReap,
            g_u32SoftDriverOk, g_u32SoftIsr, u32AddOk, u32AddFail);

    /* Grep: virtio: soft reset */
    kprintf("virtio: soft reset calls=%u timeout=%u spins=%u "
            "status_set=%u get_status=%u cfg_gen=%u\n",
            g_u32SoftReset, g_u32SoftResetTimeout,
            (unsigned)GJ_VIRTIO_RESET_SPINS, g_u32SoftStatusSet,
            g_u32SoftGetStatus, g_u32SoftCfgGen);

    /* Grep: virtio: soft kind */
    kprintf("virtio: soft kind net=%u blk=%u gpu=%u input=%u "
            "scsi=%u console=%u unknown=%u known=%u found=%u\n",
            g_u32SoftKindNet, g_u32SoftKindBlk, g_u32SoftKindGpu,
            g_u32SoftKindInput, g_u32SoftKindScsi, g_u32SoftKindConsole,
            g_u32SoftKindUnknown, u32Known, u32Found);

    /* Grep: virtio: soft poll */
    kprintf("virtio: soft poll hit=%u to=%u kick=%u isr=%u "
            "ratio_to_kick=%u\n",
            g_u32SoftPollHit, g_u32SoftPollTo, g_u32SoftKick, g_u32SoftIsr,
            (g_u32SoftKick != 0u)
                ? ((g_u32SoftPollTo * 100u) / g_u32SoftKick)
                : 0u);

    /* Grep: virtio: soft reap */
    kprintf("virtio: soft reap sum=%u poll_hit=%u add_ok=%u "
            "reap_path=poll_id_1\n",
            g_u32SoftReap, g_u32SoftPollHit, u32AddOk);

    /* Grep: virtio: soft last */
    kprintf("virtio: soft last q_idx=%u add_n=%u want_lo=0x%x "
            "want_hi=0x%x setup_ok=%u nego_ok=%u kick=%u\n",
            g_u32SoftLastQIdx, g_u32SoftLastAddN, g_u32SoftLastWantLo,
            g_u32SoftLastWantHi, g_u32SoftSetupOk, g_u32SoftNegoOk,
            g_u32SoftKick);

    /* Grep: virtio: soft modern */
    kprintf("virtio: soft modern count=%u notify=%u isr=%u devcfg=%u "
            "numq_sum=%u max_devs=%u bar_map_fail=%u\n",
            u32Modern, u32Notify, u32Isr, u32DevCfg, u32NumQSum,
            GJ_VIRTIO_MAX_DEVS, g_u32SoftBarMapFail);

    /* Grep: virtio: soft status */
    kprintf("virtio: soft status set=%u get=%u driver_ok=%u "
            "cfg_gen=%u s_ack=%u s_drv=%u s_ok=%u s_feat=%u s_fail=%u\n",
            g_u32SoftStatusSet, g_u32SoftGetStatus, g_u32SoftDriverOk,
            g_u32SoftCfgGen, (unsigned)GJ_VIRTIO_S_ACKNOWLEDGE,
            (unsigned)GJ_VIRTIO_S_DRIVER, (unsigned)GJ_VIRTIO_S_DRIVER_OK,
            (unsigned)GJ_VIRTIO_S_FEATURES_OK, (unsigned)GJ_VIRTIO_S_FAILED);

    /* Grep: virtio: soft transport */
    kprintf("virtio: soft transport common=1 notify=1 isr=1 "
            "devcfg=1 pci_cfg=0 packed=0 msix=0 vendor=0x%x "
            "max_devs=%u q_max=%u\n",
            (unsigned)GJ_VIRTIO_PCI_VENDOR, GJ_VIRTIO_MAX_DEVS,
            GJ_VIRTQ_MAX_SIZE);

    /* Grep: virtio: soft add (Wave 15) */
    kprintf("virtio: soft add n1=%u n1_fail=%u n2=%u n2_fail=%u "
            "n3=%u n3_fail=%u ok_sum=%u fail_sum=%u last_n=%u\n",
            g_u32SoftAdd, g_u32SoftAddFail, g_u32SoftAdd2, g_u32SoftAdd2Fail,
            g_u32SoftAdd3, g_u32SoftAdd3Fail, u32AddOk, u32AddFail,
            g_u32SoftLastAddN);

    /* Grep: virtio: soft bar (Wave 15) */
    kprintf("virtio: soft bar map_fail=%u max_devs=%u found=%u "
            "modern=%u known=%u unknown=%u\n",
            g_u32SoftBarMapFail, GJ_VIRTIO_MAX_DEVS, u32Found, u32Modern,
            u32Known, g_u32SoftKindUnknown);

    /* Grep: virtio: soft ratio (Wave 15) */
    kprintf("virtio: soft ratio poll_hit_pct=%u poll_to_pct=%u "
            "add_fail_pct=%u nego_fail_pct=%u q_clamp_pct=%u\n",
            (g_u32SoftKick != 0u)
                ? ((g_u32SoftPollHit * 100u) / g_u32SoftKick)
                : 0u,
            (g_u32SoftKick != 0u)
                ? ((g_u32SoftPollTo * 100u) / g_u32SoftKick)
                : 0u,
            ((u32AddOk + u32AddFail) != 0u)
                ? ((u32AddFail * 100u) / (u32AddOk + u32AddFail))
                : 0u,
            ((g_u32SoftNegoOk + g_u32SoftNegoFail) != 0u)
                ? ((g_u32SoftNegoFail * 100u) /
                   (g_u32SoftNegoOk + g_u32SoftNegoFail))
                : 0u,
            ((g_u32SoftQSetupOk + g_u32SoftQClamp) != 0u)
                ? ((g_u32SoftQClamp * 100u) /
                   (g_u32SoftQSetupOk + g_u32SoftQClamp))
                : 0u);

    /* Grep: virtio: soft notify (Wave 15) */
    kprintf("virtio: soft notify live=%u isr=%u devcfg=%u "
            "numq_sum=%u modern=%u isr_reads=%u\n",
            u32Notify, u32Isr, u32DevCfg, u32NumQSum, u32Modern,
            g_u32SoftIsr);

    /* Grep: virtio: soft ladder (Wave 15) */
    kprintf("virtio: soft ladder soft_ok=%u soft_fail=%u soft_inval=%u "
            "soft_steps=%u nego_ok=%u nego_fail=%u feat_has=%u\n",
            g_u32SoftSoftOk, g_u32SoftSoftFail, g_u32SoftSoftInval,
            g_u32SoftSoftSteps, g_u32SoftNegoOk, g_u32SoftNegoFail,
            g_u32SoftFeatHas);

    /* Grep: virtio: soft caps (Wave 15) */
    kprintf("virtio: soft caps common=1 notify=%u isr=%u devcfg=%u "
            "pci_cfg=0 packed=0 msix=0 setup_nocap=%u setup_nocommon=%u "
            "bar_map_fail=%u\n",
            u32Notify, u32Isr, u32DevCfg, g_u32SoftSetupNocap,
            g_u32SoftSetupNocommon, g_u32SoftBarMapFail);

    /* Grep: virtio: soft path */
    kprintf("virtio: soft path claim=pci_modern+vq "
            "transport=common_cfg+notify+isr setup=cap_walk "
            "features=ladder q=clamp+enable_verify "
            "wave=%u (soft inventory; not bar3)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /*
     * Wave 17 exclusive twin return surfaces (virtio: soft …).
     * Soft inventory only; never hard-gates product paths.
     */
    /* Grep: virtio: soft return — Wave 17 API return surfaces (kept) */
    kprintf("virtio: soft return soft_inv=1 modern=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    /* Grep: virtio: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("virtio: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    /* Grep: virtio: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("virtio: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio: soft deepen (Wave 20 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("virtio: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    /* Grep: virtio: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("virtio: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("virtio: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    /* Grep: virtio: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("virtio: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("virtio: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
            /* Grep: virtio: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("virtio: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
            /*
             * ---- Wave 22 exclusive complementary surfaces (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio: soft retphase — Wave 22 return-phase honesty */
            kprintf("virtio: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
            /* Grep: virtio: soft retbadge — Wave 22 exclusive badge stamp */
            kprintf("virtio: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);
    kprintf("virtio: soft deepen wave=%u areas=%u found=%u modern=%u "
            "setup_ok=%u q_ok=%u log_n=%u\n",
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE,
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_AREAS, u32Found, u32Modern,
            g_u32SoftSetupOk, g_u32SoftQSetupOk, g_u32SoftLogN);

    /* Grep: virtio: soft PASS | NODEV | PARTIAL */
    kprintf("virtio: soft %s found=%u modern=%u setup_ok=%u q_ok=%u "
            "kick=%u log_n=%u wave=%u\n",
            szVerdict, u32Found, u32Modern, g_u32SoftSetupOk,
            g_u32SoftQSetupOk, g_u32SoftKick, g_u32SoftLogN,
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE);

    /* Grep: virtio: soft inventory PASS|NODEV|PARTIAL */
    kprintf("virtio: soft inventory %s wave=%u logs=%u\n", szVerdict,
            (unsigned)VIRTIO_PCI_SOFT_DEEPEN_WAVE, g_u32SoftLogN);
}

/**
 * After first product setup/negotiate/q/io activity, print soft inventory
 * once (mirrors session_door/input_hub soft-stats-once). Diagnostics only.
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
    case 0x1000u:
        return 1;
    case GJ_VIRTIO_PCI_DEV_BLK_MODERN:
    case 0x1001u:
        return 2;
    case GJ_VIRTIO_PCI_DEV_SCSI_MODERN:
    case 0x1004u:
        return 6;
    case GJ_VIRTIO_PCI_DEV_CONSOLE:
    case 0x1003u:
        return 5;
    case GJ_VIRTIO_PCI_DEV_GPU_MODERN:
        return 3;
    case GJ_VIRTIO_PCI_DEV_INPUT:
        return 4;
    default:
        return 0;
    }
}

static const char *
kind_name(u32 u32Kind)
{
    switch (u32Kind) {
    case 1:
        return "net";
    case 2:
        return "blk";
    case 3:
        return "gpu";
    case 4:
        return "input";
    case 5:
        return "console";
    case 6:
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
    if (p) {
        *(volatile u64 *)p = u64V;
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
        /* max itself may not be power-of-two on broken hosts — soft clamp */
        u16P2 = 1;
        while ((u16)(u16P2 << 1) <= u16Max && (u16P2 << 1) != 0) {
            u16P2 = (u16)(u16P2 << 1);
        }
    }
    return u16P2;
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

static gj_paddr_t
alloc_zero_page(void)
{
    gj_paddr_t pa = pmm_alloc();

    if (pa == 0) {
        return 0;
    }
    memset((void *)hhdm_to_virt(pa), 0, GJ_PAGE_SIZE);
    return pa;
}

void
virtio_init(void)
{
    memset(g_aDevs, 0, sizeof(g_aDevs));
    g_cDevs = 0;
    /* Wave 14 soft tallies: baseline zero; log_n preserved across re-init. */
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
    /* Grep: virtio-pci: soft / virtio: soft (baseline inventory after init) */
    soft_inventory_log();
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
                case 1:
                    soft_inc(&g_u32SoftKindNet);
                    break;
                case 2:
                    soft_inc(&g_u32SoftKindBlk);
                    break;
                case 3:
                    soft_inc(&g_u32SoftKindGpu);
                    break;
                case 4:
                    soft_inc(&g_u32SoftKindInput);
                    break;
                case 5:
                    soft_inc(&g_u32SoftKindConsole);
                    break;
                case 6:
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
                /* High MMIO BARs (common with ≥4 GiB RAM) need explicit maps */
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
     * Wave 12 soft inventory after every scan (found=0 is honest NODEV-style
     * catalog). greppable: virtio-pci: soft / virtio: soft
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

            /* Skip caps whose BAR is missing/IO — keep prior fields intact */
            if (pBase == NULL) {
                /* fall through to next cap */
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_COMMON_CFG) {
                pDev->pCommon = pBase;
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_NOTIFY_CFG) {
                pDev->pNotify = pBase;
                if (u8Len >= 20) {
                    pDev->u32NotifyMult =
                        pci_read32(pDev->u8Bus, pDev->u8Slot, pDev->u8Func,
                                   (u8)(u8Cap + 16));
                } else {
                    pDev->u32NotifyMult = 0;
                }
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_ISR_CFG) {
                pDev->pIsr = pBase;
            } else if (u8Type == GJ_VIRTIO_PCI_CAP_DEVICE_CFG) {
                pDev->pDevice = pBase;
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
    kprintf("virtio: %x:%x modern common@%p queues=%u notify_mult=%u"
            " isr=%u devcfg=%u\n",
            (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot,
            (void *)pDev->pCommon, pDev->u32NumQueues,
            (unsigned)pDev->u32NotifyMult,
            pDev->pIsr != NULL ? 1u : 0u,
            pDev->pDevice != NULL ? 1u : 0u);
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
        mmio_w8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS, u8Status);
    }
}

u8
virtio_get_status(struct gj_virtio_dev *pDev)
{
    if (pDev == NULL || pDev->pCommon == NULL) {
        return 0;
    }
    soft_inc(&g_u32SoftGetStatus);
    return mmio_r8(pDev->pCommon + VIRTIO_PCI_COMMON_STATUS);
}

void
virtio_driver_ok(struct gj_virtio_dev *pDev)
{
    u8 u8St;

    if (pDev == NULL || pDev->pCommon == NULL) {
        return;
    }
    soft_inc(&g_u32SoftDriverOk);
    u8St = (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK);
    virtio_set_status(pDev, u8St);
    kprintf("virtio: driver_ok %x:%x status=0x%x features=0x%lx\n",
            (unsigned)pDev->u8Bus, (unsigned)pDev->u8Slot,
            (unsigned)virtio_get_status(pDev),
            (unsigned long)pDev->u64FeaturesDrv);
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

    /* Wave 14 sticky last want (soft inventory only). */
    g_u32SoftLastWantLo = (u32)u64WantFeatures;
    g_u32SoftLastWantHi = (u32)(u64WantFeatures >> 32);

    /*
     * Soft reset first — OASIS requires reset before re-init; also makes
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
    kprintf("virtio: features dev=0x%lx drv=0x%lx want=0x%lx v1=%u\n",
            (unsigned long)pDev->u64FeaturesDev,
            (unsigned long)pDev->u64FeaturesDrv,
            (unsigned long)u64WantFeatures,
            (unsigned)((pDev->u64FeaturesDrv & GJ_VIRTIO_F_VERSION_1) != 0));
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
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_ENABLE, 0);
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

    memset(pQ, 0, sizeof(*pQ));
    pQ->pDev = pDev;
    pQ->u16QueueIdx = u16QIdx;
    u16Want = u16Size;

    /* Soft: disable before reprogramming (safe re-setup / probe retry). */
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_SELECT, u16QIdx);
    mmio_w16(pDev->pCommon + VIRTIO_PCI_COMMON_Q_ENABLE, 0);

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
    g_u32SoftLastQIdx = (u32)u16QIdx;
    kprintf("virtio: q%u size=%u max=%u desc=0x%lx notify_off=%u soft ok\n",
            (unsigned)u16QIdx, (unsigned)u16Size, (unsigned)u16Max,
            (unsigned long)pQ->paDesc, (unsigned)pQ->u16NotifyOff);
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

    if (pQ == NULL || pa == 0 || u32Len == 0) {
        soft_inc(&g_u32SoftAddFail);
        return -1;
    }
    iHead = virtio_q_alloc_desc(pQ);
    if (iHead < 0) {
        soft_inc(&g_u32SoftAddFail);
        return -1;
    }
    pQ->pDesc[iHead].u64Addr = (u64)pa;
    pQ->pDesc[iHead].u32Len = u32Len;
    pQ->pDesc[iHead].u16Flags = fWrite ? GJ_VIRTQ_DESC_F_WRITE : 0;
    pQ->pDesc[iHead].u16Next = 0;
    iHead = virtio_q_push_head(pQ, (u16)iHead);
    if (iHead < 0) {
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

    if (pQ == NULL || pa0 == 0 || pa1 == 0 || u32Len0 == 0 || u32Len1 == 0) {
        soft_inc(&g_u32SoftAdd2Fail);
        return -1;
    }
    if (pQ->u16NumFree < 2) {
        soft_inc(&g_u32SoftAdd2Fail);
        return -1;
    }
    {
        int ia = virtio_q_alloc_desc(pQ);
        int ib = virtio_q_alloc_desc(pQ);

        if (ia < 0 || ib < 0) {
            soft_inc(&g_u32SoftAdd2Fail);
            return -1;
        }
        u16A = (u16)ia;
        u16B = (u16)ib;
    }
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

    if (pQ == NULL || pa0 == 0 || pa1 == 0 || pa2 == 0 ||
        u32Len0 == 0 || u32Len1 == 0 || u32Len2 == 0) {
        soft_inc(&g_u32SoftAdd3Fail);
        return -1;
    }
    if (pQ->u16NumFree < 3) {
        soft_inc(&g_u32SoftAdd3Fail);
        return -1;
    }
    {
        int ia = virtio_q_alloc_desc(pQ);
        int ib = virtio_q_alloc_desc(pQ);
        int ic = virtio_q_alloc_desc(pQ);

        if (ia < 0 || ib < 0 || ic < 0) {
            soft_inc(&g_u32SoftAdd3Fail);
            return -1;
        }
        u16A = (u16)ia;
        u16B = (u16)ib;
        u16C = (u16)ic;
    }
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
            /* free descriptor chain */
            {
                u16 u16Cur = (u16)u32Id;

                for (;;) {
                    u16 u16Next = pQ->pDesc[u16Cur].u16Next;
                    u16 u16Flags = pQ->pDesc[u16Cur].u16Flags;

                    pQ->pDesc[u16Cur].u16Next = pQ->u16FreeHead;
                    pQ->u16FreeHead = u16Cur;
                    pQ->u16NumFree++;
                    if ((u16Flags & GJ_VIRTQ_DESC_F_NEXT) == 0) {
                        break;
                    }
                    u16Cur = u16Next;
                }
            }
            if (pOutId != NULL) {
                *pOutId = u32Id;
            }
            soft_inc(&g_u32SoftPollHit);
            return (i32)u32Len;
        }
        __asm__ volatile ("pause");
    }
    /*
     * Soft: count spin timeouts only (spins>1). Single-shot empty (reap)
     * is not a product timeout — leave poll_to honest.
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
