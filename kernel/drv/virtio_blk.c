/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-blk: modern PCI + request virtqueue (OASIS virtio-blk).
 * Pure C, dual license (MIT OR Apache-2.0). No GPL. No Linux virtio source.
 *
 * Queues:
 *   q0 request — hdr (device-R) + data (R or W) + status (device-W)
 *
 * Product markers (serial): "virtio-blk: ready …", export/map via store_door.
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
#define VIRTIO_BLK_T_FLUSH 4u /* reserved for barrier path; not yet exported */

#define VIRTIO_BLK_S_OK    0u
#define VIRTIO_BLK_S_IOERR 1u
#define VIRTIO_BLK_S_UNSUP 2u

/* Bring-up queue geometry (identity-mapped BSS + single outstanding xfer). */
#define VIRTIO_BLK_Q_SIZE     64u
#define VIRTIO_BLK_POLL_SPINS 10000000u

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
static u32                   g_u32IoCount;

/* One outstanding request buffers (identity-mapped BSS; PA == VA on this path). */
static struct virtio_blk_req g_Req __attribute__((aligned(16)));
static u8                    g_aData[GJ_VIRTIO_BLK_SECTOR] __attribute__((aligned(16)));
static u8                    g_u8Status __attribute__((aligned(16)));

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

/*
 * Sector-at-a-time xfer on request q0.
 * Descriptor chain (OASIS virtio-blk):
 *   desc0 device-read  — request header (type + sector)
 *   desc1 R or W       — data (device writes on IN, reads on OUT)
 *   desc2 device-write — 1-byte status
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
        memset(&g_Req, 0, sizeof(g_Req));
        g_Req.u32Type = u32Type;
        g_Req.u64Sector = u64Sector + (u64)(u32Off / GJ_VIRTIO_BLK_SECTOR);
        g_u8Status = 0xff; /* poison; device overwrites */

        if (u32Type == VIRTIO_BLK_T_OUT) {
            memcpy(g_aData, (const u8 *)pBuf + u32Off, GJ_VIRTIO_BLK_SECTOR);
            fDataWrite = 0; /* device reads guest data */
        } else {
            memset(g_aData, 0, sizeof(g_aData));
            fDataWrite = 1; /* device writes guest data */
        }

        if (virtio_q_add3(&g_qReq,
                          (gj_paddr_t)(gj_vaddr_t)&g_Req, (u32)sizeof(g_Req), 0,
                          (gj_paddr_t)(gj_vaddr_t)g_aData, GJ_VIRTIO_BLK_SECTOR,
                          fDataWrite,
                          (gj_paddr_t)(gj_vaddr_t)&g_u8Status, 1, 1) < 0) {
            kprintf("virtio-blk: q_add3 failed\n");
            return -1;
        }
        virtio_q_kick(&g_qReq);
        i32Len = virtio_q_poll(&g_qReq, VIRTIO_BLK_POLL_SPINS);
        if (i32Len < 0) {
            kprintf("virtio-blk: timeout type=%u sector=%lu\n",
                    u32Type, (unsigned long)g_Req.u64Sector);
            return -1;
        }
        if (g_u8Status != VIRTIO_BLK_S_OK) {
            kprintf("virtio-blk: status=%u type=%u sector=%lu\n",
                    (unsigned)g_u8Status, u32Type,
                    (unsigned long)g_Req.u64Sector);
            return -1;
        }
        if (u32Type == VIRTIO_BLK_T_IN) {
            memcpy((u8 *)pBuf + u32Off, g_aData, GJ_VIRTIO_BLK_SECTOR);
        }
        u32Off += GJ_VIRTIO_BLK_SECTOR;
        g_u32IoCount++;
    }
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
        return -1;
    }

    st = virtio_pci_setup(g_pBlk);
    if (st != GJ_OK || g_pBlk->pCommon == NULL) {
        kprintf("virtio-blk: pci setup failed %d\n", (int)st);
        g_pBlk = NULL;
        return -1;
    }

    /* Prefer V1; fall back to transitional (empty want mask) */
    st = virtio_negotiate(g_pBlk, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        st = virtio_negotiate(g_pBlk, 0);
        if (st != GJ_OK) {
            kprintf("virtio-blk: negotiate failed %d\n", (int)st);
            g_pBlk = NULL;
            return -1;
        }
    }

    g_u64Capacity = devcfg_capacity(g_pBlk);

    st = virtio_q_setup(g_pBlk, &g_qReq, (u16)GJ_VIRTIO_BLK_Q_REQUEST,
                        (u16)VIRTIO_BLK_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-blk: request queue failed %d\n", (int)st);
        g_pBlk = NULL;
        return -1;
    }

    virtio_set_status(g_pBlk,
                      (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                           GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
    /* Product marker: greppable ready line with capacity */
    kprintf("virtio-blk: ready bus=%x slot=%x capacity=%lu sectors (%lu KiB)\n",
            (unsigned)g_pBlk->u8Bus, (unsigned)g_pBlk->u8Slot,
            (unsigned long)g_u64Capacity,
            (unsigned long)((g_u64Capacity * GJ_VIRTIO_BLK_SECTOR) / 1024ull));
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

u32
virtio_blk_io_count(void)
{
    return g_u32IoCount;
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
    return 0;
}

int
virtio_blk_kick_q(void)
{
    if (!g_fReady) {
        return -1;
    }
    virtio_q_kick(&g_qReq);
    return 0;
}

u16
virtio_blk_q_free(void)
{
    if (!g_fReady) {
        return 0;
    }
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
    kprintf("virtio-blk: map_q va=0x%lx size=%u\n", (unsigned long)u64VaBase,
            ex.u16Size);
    return 0;
}
