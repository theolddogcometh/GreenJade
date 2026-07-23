/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-blk: modern PCI + request virtqueue (OASIS virtio-blk).
 * Pure C, dual license (MIT OR Apache-2.0). No GPL. No Linux virtio source.
 *
 * Queues:
 *   q0 request — hdr (device-R) + data (R or W) + status (device-W)
 *                FLUSH: hdr + status only (no data)
 *
 * Product soft depth (Wave 10 exclusive deepen in this file):
 *   queue stats, multi-segment soft bounce (GJ_VIRTIO_BLK_SOFT_SEGS),
 *   FLUSH/sync serial depth-1, greppable soft inventory rollup.
 *
 * Greppable product markers (serial; prefix-stable "virtio-blk: soft …"):
 *   "virtio-blk: ready …"
 *   "virtio-blk: soft inventory …"
 *   "virtio-blk: soft queue …"
 *   "virtio-blk: soft multi-seg …"
 *   "virtio-blk: soft flush …"
 *   "virtio-blk: soft counters …"
 *   "virtio-blk: soft path …"
 *   "virtio-blk: soft ring …"
 *   "virtio-blk: soft PASS|NODEV|PARTIAL"
 * Export/map via store_door.
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/virtio_blk.h>
#include <gj/vmm.h>

/* ---- OASIS request types / status (public virtio-blk) -------------------- */
#define VIRTIO_BLK_T_IN    0u
#define VIRTIO_BLK_T_OUT   1u
#define VIRTIO_BLK_T_FLUSH 4u

#define VIRTIO_BLK_S_OK    0u
#define VIRTIO_BLK_S_IOERR 1u
#define VIRTIO_BLK_S_UNSUP 2u

/* Bring-up queue geometry (identity-mapped BSS + single outstanding xfer). */
#define VIRTIO_BLK_Q_SIZE     64u
#define VIRTIO_BLK_POLL_SPINS 10000000u
/* Soft FLUSH serial depth (one outstanding barrier on product path). */
#define VIRTIO_BLK_FLUSH_DEPTH 1u

/* Feature: capacity is always present in device config (first 8 bytes). */
struct virtio_blk_config {
    u64 u64Capacity; /* sectors of 512 bytes */
} __attribute__((packed));

struct virtio_blk_req {
    u32 u32Type;
    u32 u32Reserved;
    u64 u64Sector;
} __attribute__((packed));

static struct gj_virtio_dev *g_pBlk;
static struct gj_virtq       g_qReq; /* GJ_VIRTIO_BLK_Q_REQUEST (q0) */
static int                   g_fReady;
static u64                   g_u64Capacity;

/* Soft product counters (request queue stats). */
static u32 g_u32IoCount;
static u32 g_u32Kicks;
static u32 g_u32MultiSegOps;
static u32 g_u32MultiSegs;
static u32 g_u32FlushCount;
static u32 g_u32FlushSoft;
static u32 g_u32Errors;
static u32 g_u32BytesIn;
static u32 g_u32BytesOut;
static u16 g_u16FreeMin;
static int g_fFlushBusy; /* soft depth-1: non-zero while FLUSH in flight */

/* Wave 10 soft inventory telemetry (never hard-gates product I/O). */
static u32 g_u32SoftLogN;    /* inventory emissions */
static int g_fSoftOnce;      /* first post-activity inventory emitted */
static u32 g_u32MapQ;        /* map_q_user successes */
static u32 g_u32ExportQ;     /* export_q successes */
static u32 g_u32KickApi;     /* kick_q API calls (distinct from desc kicks) */

/*
 * One outstanding request buffers (identity-mapped BSS; PA == VA on this path).
 * Soft multi-seg bounce: up to GJ_VIRTIO_BLK_SOFT_SEGS sectors per chain.
 */
static struct virtio_blk_req g_Req __attribute__((aligned(16)));
static u8 g_aData[GJ_VIRTIO_BLK_SECTOR * GJ_VIRTIO_BLK_SOFT_SEGS]
    __attribute__((aligned(16)));
static u8 g_u8Status __attribute__((aligned(16)));

static u64
devcfg_capacity(struct gj_virtio_dev *pDev)
{
    volatile struct virtio_blk_config *pCfg;

    if (pDev == NULL || pDev->pDevice == NULL) {
        return 0;
    }
    pCfg = (volatile struct virtio_blk_config *)(void *)pDev->pDevice;
    return pCfg->u64Capacity;
}

/* Snapshot free-desc watermark for queue stats soft path. */
static void
q_note_free(void)
{
    u16 u16Free;

    if (!g_fReady) {
        return;
    }
    u16Free = virtio_q_num_free(&g_qReq);
    if (u16Free < g_u16FreeMin) {
        g_u16FreeMin = u16Free;
    }
}

static void
q_kick_counted(void)
{
    virtio_q_kick(&g_qReq);
    g_u32Kicks++;
    q_note_free();
}

/**
 * Greppable Wave 10 soft inventory dump (product / smoke).
 * Prefix-stable "virtio-blk: soft …" — never hard-gates; kprintf only.
 *
 *   virtio-blk: soft inventory  — ready + PCI + capacity + queue geometry
 *   virtio-blk: soft queue      — size / free watermark / free now
 *   virtio-blk: soft multi-seg  — bounce ceiling + multi-seg tallies
 *   virtio-blk: soft flush      — depth-1 serial barrier + soft UNSUP
 *   virtio-blk: soft counters   — io / kicks / bytes / errors
 *   virtio-blk: soft path       — product surface catalog (claim honesty)
 *   virtio-blk: soft ring       — export/map/kick soft tallies
 *   virtio-blk: soft PASS|NODEV|PARTIAL
 *
 * greppable: virtio-blk: soft
 */
static void
blk_soft_inventory(const char *szVia)
{
    const char *szVerdict;
    u16 u16FreeNow;
    u16 u16QSize;
    u8 u8Bus;
    u8 u8Slot;
    u32 u32CapKiB;
    u32 u32BounceKiB;

    if (szVia == NULL) {
        szVia = "path";
    }

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    u16QSize = g_fReady ? g_qReq.u16Size : 0;
    u16FreeNow = g_fReady ? virtio_q_num_free(&g_qReq) : 0;
    if (g_fReady) {
        q_note_free();
    }
    u8Bus = (g_pBlk != NULL) ? g_pBlk->u8Bus : 0;
    u8Slot = (g_pBlk != NULL) ? g_pBlk->u8Slot : 0;
    u32CapKiB = (u32)((g_u64Capacity * (u64)GJ_VIRTIO_BLK_SECTOR) / 1024ull);
    u32BounceKiB =
        (u32)((GJ_VIRTIO_BLK_SOFT_SEGS * GJ_VIRTIO_BLK_SECTOR) / 1024u);

    /*
     * Soft verdict (inventory only; I/O path unchanged):
     *   NODEV    — not ready / no device
     *   PASS     — ready + any completed io/flush
     *   PARTIAL  — ready, no completed product I/O yet (post-probe)
     */
    if (!g_fReady) {
        szVerdict = "NODEV";
    } else if (g_u32IoCount != 0u || g_u32FlushCount != 0u ||
               g_u32FlushSoft != 0u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /* Grep: virtio-blk: soft inventory */
    kprintf("virtio-blk: soft inventory via=%s ready=%u bus=%x slot=%x "
            "cap_secs=%lu cap_kib=%u q_size=%u free=%u free_min=%u "
            "soft_segs=%u flush_depth=%u sector=%u log_n=%u\n",
            szVia, g_fReady ? 1u : 0u, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned long)g_u64Capacity, u32CapKiB, (unsigned)u16QSize,
            (unsigned)u16FreeNow, (unsigned)g_u16FreeMin,
            (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS,
            (unsigned)VIRTIO_BLK_FLUSH_DEPTH, (unsigned)GJ_VIRTIO_BLK_SECTOR,
            g_u32SoftLogN);

    /* Grep: virtio-blk: soft queue */
    kprintf("virtio-blk: soft queue stats size=%u free=%u free_min=%u "
            "q_idx=%u notify_off=%u free_head=%u\n",
            (unsigned)u16QSize, (unsigned)u16FreeNow, (unsigned)g_u16FreeMin,
            (unsigned)(g_fReady ? g_qReq.u16QueueIdx : 0u),
            (unsigned)(g_fReady ? g_qReq.u16NotifyOff : 0u),
            (unsigned)(g_fReady ? g_qReq.u16FreeHead : 0u));

    /* Grep: virtio-blk: soft multi-seg */
    kprintf("virtio-blk: soft multi-seg max=%u (%u KiB bounce) ops=%u "
            "segs=%u\n",
            (unsigned)GJ_VIRTIO_BLK_SOFT_SEGS, u32BounceKiB, g_u32MultiSegOps,
            g_u32MultiSegs);

    /* Grep: virtio-blk: soft flush */
    kprintf("virtio-blk: soft flush depth=%u busy=%u ok=%u soft_unsup=%u "
            "serial=1\n",
            (unsigned)VIRTIO_BLK_FLUSH_DEPTH, g_fFlushBusy ? 1u : 0u,
            g_u32FlushCount, g_u32FlushSoft);

    /* Grep: virtio-blk: soft counters */
    kprintf("virtio-blk: soft counters io=%u kicks=%u err=%u bin=%u bout=%u "
            "flush=%u flush_soft=%u multi_ops=%u multi_segs=%u\n",
            g_u32IoCount, g_u32Kicks, g_u32Errors, g_u32BytesIn, g_u32BytesOut,
            g_u32FlushCount, g_u32FlushSoft, g_u32MultiSegOps, g_u32MultiSegs);

    /*
     * Grep: virtio-blk: soft path
     * Honesty catalog: product surfaces this driver exposes (store_door /
     * storaged / vfsd). claim=1 only when DRIVER_OK + q0 live.
     */
    kprintf("virtio-blk: soft path claim=%u rw=1 flush=1 export_q=1 map_q=1 "
            "kick_q=1 q_stats=1 multi_seg=1 depth1_flush=1 "
            "t_in=%u t_out=%u t_flush=%u\n",
            g_fReady ? 1u : 0u, (unsigned)VIRTIO_BLK_T_IN,
            (unsigned)VIRTIO_BLK_T_OUT, (unsigned)VIRTIO_BLK_T_FLUSH);

    /* Grep: virtio-blk: soft ring */
    kprintf("virtio-blk: soft ring export=%u map=%u kick_api=%u "
            "desc_kicks=%u pa_desc=0x%lx\n",
            g_u32ExportQ, g_u32MapQ, g_u32KickApi, g_u32Kicks,
            (unsigned long)(g_fReady ? (u64)g_qReq.paDesc : 0ull));

    /* Grep: virtio-blk: soft PASS | NODEV | PARTIAL */
    kprintf("virtio-blk: soft %s via=%s ready=%u io=%u err=%u log_n=%u\n",
            szVerdict, szVia, g_fReady ? 1u : 0u, g_u32IoCount, g_u32Errors,
            g_u32SoftLogN);
}

/**
 * After first product R/W/FLUSH activity, print soft inventory once
 * (mirrors door/futex soft-stats-once). Safe from xfer/flush return paths.
 */
static void
blk_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32IoCount == 0u && g_u32FlushCount == 0u &&
        g_u32FlushSoft == 0u && g_u32Errors == 0u) {
        return;
    }
    g_fSoftOnce = 1;
    blk_soft_inventory("activity");
}

/*
 * Sector soft multi-segment xfer on request q0.
 * Descriptor chain (OASIS virtio-blk):
 *   desc0 device-read  — request header (type + sector)
 *   desc1 R or W       — data (1..SOFT_SEGS sectors, contiguous bounce)
 *   desc2 device-write — 1-byte status
 *
 * Longer transfers loop over soft segments; multi-seg ops are counted when
 * a single chain carries more than one sector.
 */
static int
blk_xfer(u32 u32Type, u64 u64Sector, void *pBuf, u32 cbLen)
{
    i32 i32Len;
    int fDataWrite;
    u32 u32Off;

    if (!g_fReady || pBuf == NULL || cbLen == 0) {
        return -1;
    }
    if ((cbLen % GJ_VIRTIO_BLK_SECTOR) != 0) {
        return -1;
    }

    u32Off = 0;
    while (u32Off < cbLen) {
        u32 cSegs;
        u32 cbChunk;

        cSegs = (cbLen - u32Off) / GJ_VIRTIO_BLK_SECTOR;
        if (cSegs > GJ_VIRTIO_BLK_SOFT_SEGS) {
            cSegs = GJ_VIRTIO_BLK_SOFT_SEGS;
        }
        cbChunk = cSegs * GJ_VIRTIO_BLK_SECTOR;

        memset(&g_Req, 0, sizeof(g_Req));
        g_Req.u32Type = u32Type;
        g_Req.u64Sector = u64Sector + (u64)(u32Off / GJ_VIRTIO_BLK_SECTOR);
        g_u8Status = 0xff; /* poison; device overwrites */

        if (u32Type == VIRTIO_BLK_T_OUT) {
            memcpy(g_aData, (const u8 *)pBuf + u32Off, cbChunk);
            fDataWrite = 0; /* device reads guest data */
        } else {
            memset(g_aData, 0, cbChunk);
            fDataWrite = 1; /* device writes guest data */
        }

        if (virtio_q_add3(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)g_aData, cbChunk,
                          fDataWrite,
                          (gj_paddr_t)(gj_vaddr_t)&g_u8Status, 1, 1) < 0) {
            kprintf("virtio-blk: q_add3 failed\n");
            g_u32Errors++;
            blk_soft_maybe_once();
            return -1;
        }
        q_kick_counted();
        i32Len = virtio_q_poll(&g_qReq, VIRTIO_BLK_POLL_SPINS);
        q_note_free();
        if (i32Len < 0) {
            kprintf("virtio-blk: timeout type=%u sector=%lu\n",
                    u32Type, (unsigned long)g_Req.u64Sector);
            g_u32Errors++;
            blk_soft_maybe_once();
            return -1;
        }
        if (g_u8Status != VIRTIO_BLK_S_OK) {
            kprintf("virtio-blk: status=%u type=%u sector=%lu\n",
                    (unsigned)g_u8Status, u32Type,
                    (unsigned long)g_Req.u64Sector);
            g_u32Errors++;
            blk_soft_maybe_once();
            return -1;
        }
        if (u32Type == VIRTIO_BLK_T_IN) {
            memcpy((u8 *)pBuf + u32Off, g_aData, cbChunk);
            g_u32BytesIn += cbChunk;
        } else {
            g_u32BytesOut += cbChunk;
        }
        if (cSegs > 1u) {
            g_u32MultiSegOps++;
            g_u32MultiSegs += cSegs;
        }
        u32Off += cbChunk;
        g_u32IoCount++;
    }
    blk_soft_maybe_once();
    return 0;
}

/*
 * Probe path: find first blk → modern PCI caps → features → request q0 →
 * DRIVER_OK. Leaves g_fReady=0 and g_pBlk=NULL on any failure (no partial ready).
 */
int
virtio_blk_probe(void)
{
    u32 i;
    u32 c;
    gj_status_t st;

    g_pBlk = NULL;
    g_fReady = 0;
    g_u64Capacity = 0;
    g_u32IoCount = 0;
    g_u32Kicks = 0;
    g_u32MultiSegOps = 0;
    g_u32MultiSegs = 0;
    g_u32FlushCount = 0;
    g_u32FlushSoft = 0;
    g_u32Errors = 0;
    g_u32BytesIn = 0;
    g_u32BytesOut = 0;
    g_u16FreeMin = 0xffffu;
    g_fFlushBusy = 0;
    g_u32SoftLogN = 0;
    g_fSoftOnce = 0;
    g_u32MapQ = 0;
    g_u32ExportQ = 0;
    g_u32KickApi = 0;

    c = virtio_dev_count();
    /* kind==2 (product), transitional 0x1001, or modern blk device ID */
    for (i = 0; i < c; i++) {
        struct gj_virtio_dev *pDev = virtio_dev_get(i);

        if (pDev == NULL) {
            continue;
        }
        if (pDev->u32Kind == 2 || pDev->u16Device == 0x1001u ||
            pDev->u16Device == GJ_VIRTIO_PCI_DEV_BLK_MODERN) {
            g_pBlk = pDev;
            break;
        }
    }
    if (g_pBlk == NULL) {
        kprintf("virtio-blk: no device\n");
        /* Grep: virtio-blk: soft … NODEV (Wave 10 soft inventory) */
        blk_soft_inventory("nodev");
        return -1;
    }

    st = virtio_pci_setup(g_pBlk);
    if (st != GJ_OK || g_pBlk->pCommon == NULL) {
        kprintf("virtio-blk: pci setup failed %d\n", (int)st);
        g_pBlk = NULL;
        blk_soft_inventory("pci_fail");
        return -1;
    }

    /* Prefer V1; fall back to transitional (empty want mask) */
    st = virtio_negotiate(g_pBlk, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        st = virtio_negotiate(g_pBlk, 0);
        if (st != GJ_OK) {
            kprintf("virtio-blk: negotiate failed %d\n", (int)st);
            g_pBlk = NULL;
            blk_soft_inventory("negotiate_fail");
            return -1;
        }
    }

    g_u64Capacity = devcfg_capacity(g_pBlk);

    st = virtio_q_setup(g_pBlk, &g_qReq, (u16)GJ_VIRTIO_BLK_Q_REQUEST,
                        (u16)VIRTIO_BLK_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-blk: request queue failed %d\n", (int)st);
        g_pBlk = NULL;
        blk_soft_inventory("q_fail");
        return -1;
    }

    virtio_set_status(g_pBlk,
                      (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                           GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
    g_u16FreeMin = virtio_q_num_free(&g_qReq);

    /* Product marker: greppable ready line with capacity (prefix-stable). */
    kprintf("virtio-blk: ready bus=%x slot=%x capacity=%lu sectors (%lu KiB)\n",
            (unsigned)g_pBlk->u8Bus, (unsigned)g_pBlk->u8Slot,
            (unsigned long)g_u64Capacity,
            (unsigned long)((g_u64Capacity * GJ_VIRTIO_BLK_SECTOR) / 1024ull));
    /*
     * Wave 10 soft inventory rollup (prefix-stable "virtio-blk: soft …").
     * Replaces the prior three one-liners with a greppable catalog + PASS.
     */
    blk_soft_inventory("probe");
    return 0;
}

int
virtio_blk_ready(void)
{
    return g_fReady;
}

u64
virtio_blk_capacity_sectors(void)
{
    return g_fReady ? g_u64Capacity : 0;
}

int
virtio_blk_read(u64 u64Sector, void *pBuf, u32 cbLen)
{
    if (g_fReady && g_u64Capacity != 0 &&
        u64Sector + (cbLen / GJ_VIRTIO_BLK_SECTOR) > g_u64Capacity) {
        return -1;
    }
    return blk_xfer(VIRTIO_BLK_T_IN, u64Sector, pBuf, cbLen);
}

int
virtio_blk_write(u64 u64Sector, const void *pBuf, u32 cbLen)
{
    if (g_fReady && g_u64Capacity != 0 &&
        u64Sector + (cbLen / GJ_VIRTIO_BLK_SECTOR) > g_u64Capacity) {
        return -1;
    }
    return blk_xfer(VIRTIO_BLK_T_OUT, u64Sector, (void *)(gj_vaddr_t)pBuf, cbLen);
}

/*
 * OASIS T_FLUSH: hdr (device-R) + status (device-W), no data.
 * Soft depth-1 serial barrier. Device UNSUP → fsync-shaped soft success.
 */
int
virtio_blk_flush(void)
{
    i32 i32Len;

    if (!g_fReady) {
        return -1;
    }
    if (g_fFlushBusy) {
        /* Soft depth exceeded: serial product path rejects concurrent FLUSH. */
        g_u32Errors++;
        blk_soft_maybe_once();
        return -1;
    }

    g_fFlushBusy = 1;
    memset(&g_Req, 0, sizeof(g_Req));
    g_Req.u32Type = VIRTIO_BLK_T_FLUSH;
    g_Req.u64Sector = 0;
    g_u8Status = 0xff;

    if (virtio_q_add2(&g_qReq,
                      (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                      (gj_paddr_t)(gj_vaddr_t)&g_u8Status, 1, 1) < 0) {
        kprintf("virtio-blk: flush q_add2 failed\n");
        g_u32Errors++;
        g_fFlushBusy = 0;
        blk_soft_maybe_once();
        return -1;
    }
    q_kick_counted();
    i32Len = virtio_q_poll(&g_qReq, VIRTIO_BLK_POLL_SPINS);
    q_note_free();
    g_fFlushBusy = 0;

    if (i32Len < 0) {
        kprintf("virtio-blk: flush timeout\n");
        g_u32Errors++;
        blk_soft_maybe_once();
        return -1;
    }
    if (g_u8Status == VIRTIO_BLK_S_OK) {
        g_u32FlushCount++;
        g_u32IoCount++;
        blk_soft_maybe_once();
        return 0;
    }
    if (g_u8Status == VIRTIO_BLK_S_UNSUP) {
        /* Soft fsync: device lacks FLUSH; barrier treated as success. */
        g_u32FlushSoft++;
        blk_soft_maybe_once();
        return 0;
    }
    kprintf("virtio-blk: flush status=%u\n", (unsigned)g_u8Status);
    g_u32Errors++;
    blk_soft_maybe_once();
    return -1;
}

u32
virtio_blk_io_count(void)
{
    return g_u32IoCount;
}

u32
virtio_blk_flush_count(void)
{
    return g_u32FlushCount;
}

u32
virtio_blk_flush_soft_count(void)
{
    return g_u32FlushSoft;
}

int
virtio_blk_q_stats(struct gj_virtio_blk_q_stats *pOut)
{
    if (pOut == NULL) {
        return -1;
    }
    memset(pOut, 0, sizeof(*pOut));
    if (!g_fReady) {
        /* Soft inventory on stats miss so smoke greps NODEV surface. */
        blk_soft_inventory("q_stats");
        return -1;
    }
    pOut->u32IoCount = g_u32IoCount;
    pOut->u32Kicks = g_u32Kicks;
    pOut->u32MultiSegOps = g_u32MultiSegOps;
    pOut->u32MultiSegs = g_u32MultiSegs;
    pOut->u32FlushCount = g_u32FlushCount;
    pOut->u32FlushSoft = g_u32FlushSoft;
    pOut->u32Errors = g_u32Errors;
    pOut->u32BytesIn = g_u32BytesIn;
    pOut->u32BytesOut = g_u32BytesOut;
    pOut->u16QSize = g_qReq.u16Size;
    pOut->u16FreeMin = g_u16FreeMin;
    pOut->u16FreeNow = virtio_q_num_free(&g_qReq);
    pOut->u16FlushDepth = (u16)VIRTIO_BLK_FLUSH_DEPTH;
    /*
     * Emit soft inventory on stats read so bring-up smoke also greps
     * virtio-blk: soft … lines (mirrors door_stats / file_lock_count).
     * greppable: virtio-blk: soft
     */
    blk_soft_inventory("q_stats");
    return 0;
}

int
virtio_blk_export_q(struct gj_virtq_export *pOut)
{
    if (!g_fReady || pOut == NULL || g_qReq.pDesc == NULL) {
        return -1;
    }
    memset(pOut, 0, sizeof(*pOut));
    pOut->u16Which = (u16)GJ_VIRTIO_BLK_Q_REQUEST;
    pOut->u16Size = g_qReq.u16Size;
    pOut->u16QueueIdx = g_qReq.u16QueueIdx;
    pOut->u16NotifyOff = g_qReq.u16NotifyOff;
    pOut->u64PaDesc = (u64)g_qReq.paDesc;
    pOut->u64PaAvail = (u64)g_qReq.paAvail;
    pOut->u64PaUsed = (u64)g_qReq.paUsed;
    pOut->u32NotifyMult = g_pBlk != NULL ? g_pBlk->u32NotifyMult : 0;
    pOut->u32Ready = 1;
    /* MAP_RING layout: one page each for desc / avail / used (see map_q_user) */
    pOut->u32OffDesc = 0;
    pOut->u32OffAvail = GJ_PAGE_SIZE;
    pOut->u32OffUsed = GJ_PAGE_SIZE * 2u;
    pOut->u16FreeHead = g_qReq.u16FreeHead;
    pOut->u16NumFree = g_qReq.u16NumFree;
    q_note_free();
    g_u32ExportQ++;
    return 0;
}

int
virtio_blk_kick_q(void)
{
    if (!g_fReady) {
        return -1;
    }
    g_u32KickApi++;
    q_kick_counted();
    return 0;
}

u16
virtio_blk_q_free(void)
{
    if (!g_fReady) {
        return 0;
    }
    q_note_free();
    return virtio_q_num_free(&g_qReq);
}

int
virtio_blk_map_q_user(u64 u64VaBase, struct gj_virtq_export *pOut)
{
    struct gj_virtq_export ex;

    if (virtio_blk_export_q(&ex) != 0) {
        return -1;
    }
    if (u64VaBase == 0 || (u64VaBase & 0xfffull) != 0) {
        return -1;
    }
    /* One page each: rings live on dedicated PMM pages (virtio_q_setup). */
    if (vmm_map_page((gj_vaddr_t)u64VaBase, (gj_paddr_t)ex.u64PaDesc,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaAvail,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        return -1;
    }
    if (vmm_map_page((gj_vaddr_t)(u64VaBase + 2u * GJ_PAGE_SIZE),
                     (gj_paddr_t)ex.u64PaUsed,
                     GJ_VMM_PROT_READ | GJ_VMM_PROT_WRITE | GJ_VMM_PROT_USER) !=
        GJ_OK) {
        return -1;
    }
    ex.u32OffDesc = 0;
    ex.u32OffAvail = GJ_PAGE_SIZE;
    ex.u32OffUsed = GJ_PAGE_SIZE * 2u;
    if (pOut != NULL) {
        *pOut = ex;
    }
    g_u32MapQ++;
    kprintf("virtio-blk: map_q va=0x%lx size=%u\n", (unsigned long)u64VaBase,
            ex.u16Size);
    return 0;
}
