/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-gpu 2D present (OASIS virtio-gpu).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
 *
 * Soft path layout (control virtqueue 0 only):
 *   display  — gpu_display_refresh / pick first enabled scanout
 *   resource — create_2d, attach (page-walk+coalesce), detach, unref
 *   scanout  — set / disable
 *   flush    — transfer_to_host_2d + resource_flush (full or dirty rect)
 *
 * Greppable product markers (prefix-stable):
 *   virtio-gpu: ready PASS
 *   virtio-gpu: present PASS
 */
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/virtio.h>
#include <gj/virtio_gpu.h>
#include <gj/vmm.h>

/* ---- OASIS virtio-gpu control types (public) ----------------------------- */
#define VIRTIO_GPU_CMD_GET_DISPLAY_INFO        0x0100u
#define VIRTIO_GPU_CMD_RESOURCE_CREATE_2D      0x0101u
#define VIRTIO_GPU_CMD_RESOURCE_UNREF          0x0102u
#define VIRTIO_GPU_CMD_SET_SCANOUT             0x0103u
#define VIRTIO_GPU_CMD_RESOURCE_FLUSH          0x0104u
#define VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D     0x0105u
#define VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING 0x0106u
#define VIRTIO_GPU_CMD_RESOURCE_DETACH_BACKING 0x0107u

#define VIRTIO_GPU_RESP_OK_NODATA              0x1100u
#define VIRTIO_GPU_RESP_OK_DISPLAY_INFO        0x1101u
#define VIRTIO_GPU_RESP_ERR_UNSPEC             0x1200u

#define VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM       2u
#define VIRTIO_GPU_MAX_SCANOUTS                16u

/* Default soft-path geometry when host has no enabled scanout. */
#define VIRTIO_GPU_DEFAULT_W                   1024u
#define VIRTIO_GPU_DEFAULT_H                   768u

/* Bring-up queue + DMA buffers. */
#define VIRTIO_GPU_Q_SIZE                      64u
#define VIRTIO_GPU_POLL_SPINS                   10000000u

/*
 * Max coalesced mem_entries per ATTACH_BACKING.
 * 256 runs × 4 KiB covers a fully fragmented 1 MiB FB; contiguous pmm
 * allocations collapse to a single entry regardless of size.
 */
#define VIRTIO_GPU_MAX_MEM_ENTRIES             256u

/* ---- wire structs (packed, OASIS layout) --------------------------------- */
struct virtio_gpu_ctrl_hdr {
    u32 u32Type;
    u32 u32Flags;
    u64 u64FenceId;
    u32 u32CtxId;
    u32 u32Padding;
} __attribute__((packed));

struct virtio_gpu_rect {
    u32 u32X;
    u32 u32Y;
    u32 u32Width;
    u32 u32Height;
} __attribute__((packed));

struct virtio_gpu_resource_create_2d {
    struct virtio_gpu_ctrl_hdr hdr;
    u32 u32ResourceId;
    u32 u32Format;
    u32 u32Width;
    u32 u32Height;
} __attribute__((packed));

struct virtio_gpu_resource_unref {
    struct virtio_gpu_ctrl_hdr hdr;
    u32 u32ResourceId;
    u32 u32Padding;
} __attribute__((packed));

struct virtio_gpu_resource_detach_backing {
    struct virtio_gpu_ctrl_hdr hdr;
    u32 u32ResourceId;
    u32 u32Padding;
} __attribute__((packed));

struct virtio_gpu_mem_entry {
    u64 u64Addr;
    u32 u32Length;
    u32 u32Padding;
} __attribute__((packed));

struct virtio_gpu_resource_attach_backing {
    struct virtio_gpu_ctrl_hdr hdr;
    u32 u32ResourceId;
    u32 u32NrEntries;
} __attribute__((packed));

struct virtio_gpu_set_scanout {
    struct virtio_gpu_ctrl_hdr hdr;
    struct virtio_gpu_rect r;
    u32 u32ScanoutId;
    u32 u32ResourceId;
} __attribute__((packed));

struct virtio_gpu_transfer_to_host_2d {
    struct virtio_gpu_ctrl_hdr hdr;
    struct virtio_gpu_rect r;
    u64 u64Offset;
    u32 u32ResourceId;
    u32 u32Padding;
} __attribute__((packed));

struct virtio_gpu_resource_flush {
    struct virtio_gpu_ctrl_hdr hdr;
    struct virtio_gpu_rect r;
    u32 u32ResourceId;
    u32 u32Padding;
} __attribute__((packed));

struct virtio_gpu_display_one {
    struct virtio_gpu_rect r;
    u32 u32Enabled;
    u32 u32Flags;
} __attribute__((packed));

struct virtio_gpu_resp_display_info {
    struct virtio_gpu_ctrl_hdr hdr;
    struct virtio_gpu_display_one aPmodes[VIRTIO_GPU_MAX_SCANOUTS];
} __attribute__((packed));

/* Request blob must fit attach hdr + max mem_entries. */
#define VIRTIO_GPU_REQ_BYTES \
    (sizeof(struct virtio_gpu_resource_attach_backing) + \
     (VIRTIO_GPU_MAX_MEM_ENTRIES * sizeof(struct virtio_gpu_mem_entry)))

static u8 g_aReq[VIRTIO_GPU_REQ_BYTES] __attribute__((aligned(16)));
static u8 g_aResp[1024] __attribute__((aligned(16)));

static struct gj_virtio_dev *g_pGpu;
static struct gj_virtq       g_qCtrl;
static u32                   g_u32PresentCount;
static u32                   g_u32ResId = 1; /* active host resource when fHaveRes */
static int                   g_fReady;
static int                   g_fHaveRes;
static int                   g_fScanoutBound;
static u32                   g_u32Width;
static u32                   g_u32Height;
static u32                   g_u32Stride;
static gj_paddr_t            g_paFb;       /* first page PA of active backing */
static u32                   g_u32ScanoutId;

/* Display soft-path cache (refreshed by gpu_display_refresh). */
static int                   g_fDispValid;
static u32                   g_u32DispScanout;
static u32                   g_u32DispW;
static u32                   g_u32DispH;
static u32                   g_u32DispEnabled;

/* ---- control-queue transport --------------------------------------------- */

static void
gpu_hdr_init(struct virtio_gpu_ctrl_hdr *pH, u32 u32Type)
{
    if (pH == NULL) {
        return;
    }
    pH->u32Type = u32Type;
    pH->u32Flags = 0;
    pH->u64FenceId = 0;
    pH->u32CtxId = 0;
    pH->u32Padding = 0;
}

static void
gpu_rect_set(struct virtio_gpu_rect *pR, u32 u32X, u32 u32Y,
             u32 u32W, u32 u32H)
{
    if (pR == NULL) {
        return;
    }
    pR->u32X = u32X;
    pR->u32Y = u32Y;
    pR->u32Width = u32W;
    pR->u32Height = u32H;
}

/*
 * Clip dirty rect to active resource. Zero width or height → full resource.
 * Returns 0 and writes clipped coords, or -1 if empty / no resource.
 */
static int
gpu_rect_clip(u32 u32X, u32 u32Y, u32 u32W, u32 u32H,
              u32 *pOutX, u32 *pOutY, u32 *pOutW, u32 *pOutH)
{
    u32 u32X2;
    u32 u32Y2;

    if (!g_fHaveRes || g_u32Width == 0 || g_u32Height == 0) {
        return -1;
    }
    if (u32W == 0 || u32H == 0) {
        u32X = 0;
        u32Y = 0;
        u32W = g_u32Width;
        u32H = g_u32Height;
    }
    if (u32X >= g_u32Width || u32Y >= g_u32Height) {
        return -1;
    }
    u32X2 = u32X + u32W;
    u32Y2 = u32Y + u32H;
    if (u32X2 < u32X || u32X2 > g_u32Width) {
        u32X2 = g_u32Width;
    }
    if (u32Y2 < u32Y || u32Y2 > g_u32Height) {
        u32Y2 = g_u32Height;
    }
    if (u32X2 <= u32X || u32Y2 <= u32Y) {
        return -1;
    }
    if (pOutX) {
        *pOutX = u32X;
    }
    if (pOutY) {
        *pOutY = u32Y;
    }
    if (pOutW) {
        *pOutW = u32X2 - u32X;
    }
    if (pOutH) {
        *pOutH = u32Y2 - u32Y;
    }
    return 0;
}

static int
gpu_cmd(void *pReq, u32 cbReq)
{
    i32 i32Len;
    struct virtio_gpu_ctrl_hdr *pRh;
    struct virtio_gpu_ctrl_hdr *pRq;

    if (pReq == NULL || cbReq == 0 || cbReq > sizeof(g_aReq)) {
        return -1;
    }
    memset(g_aResp, 0, sizeof(g_aResp));
    if (virtio_q_add2(&g_qCtrl,
                      (gj_paddr_t)(gj_vaddr_t)pReq, cbReq, 0,
                      (gj_paddr_t)(gj_vaddr_t)g_aResp, sizeof(g_aResp), 1) < 0) {
        return -1;
    }
    virtio_q_kick(&g_qCtrl);
    i32Len = virtio_q_poll(&g_qCtrl, VIRTIO_GPU_POLL_SPINS);
    pRq = (struct virtio_gpu_ctrl_hdr *)pReq;
    if (i32Len < 0) {
        kprintf("virtio-gpu: cmd timeout type=0x%x\n", pRq->u32Type);
        return -1;
    }
    pRh = (struct virtio_gpu_ctrl_hdr *)(void *)g_aResp;
    /* Success responses live in 0x1100..0x11ff; errors start at 0x1200. */
    if (pRh->u32Type < VIRTIO_GPU_RESP_OK_NODATA ||
        pRh->u32Type >= VIRTIO_GPU_RESP_ERR_UNSPEC) {
        kprintf("virtio-gpu: bad resp type=0x%x for cmd=0x%x\n",
                pRh->u32Type, pRq->u32Type);
        return -1;
    }
    return 0;
}

/* ---- resource soft path -------------------------------------------------- */

static int
gpu_res_create_2d(u32 u32Res, u32 u32W, u32 u32H, u32 u32Fmt)
{
    struct virtio_gpu_resource_create_2d *pCreate;

    if (u32Res == 0 || u32W == 0 || u32H == 0) {
        return -1;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pCreate = (struct virtio_gpu_resource_create_2d *)(void *)g_aReq;
    gpu_hdr_init(&pCreate->hdr, VIRTIO_GPU_CMD_RESOURCE_CREATE_2D);
    pCreate->u32ResourceId = u32Res;
    pCreate->u32Format = u32Fmt;
    pCreate->u32Width = u32W;
    pCreate->u32Height = u32H;
    return gpu_cmd(pCreate, sizeof(*pCreate));
}

static int
gpu_res_detach(u32 u32Res)
{
    struct virtio_gpu_resource_detach_backing *pDet;

    if (u32Res == 0) {
        return 0;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pDet = (struct virtio_gpu_resource_detach_backing *)(void *)g_aReq;
    gpu_hdr_init(&pDet->hdr, VIRTIO_GPU_CMD_RESOURCE_DETACH_BACKING);
    pDet->u32ResourceId = u32Res;
    return gpu_cmd(pDet, sizeof(*pDet));
}

static int
gpu_res_unref(u32 u32Res)
{
    struct virtio_gpu_resource_unref *pUnref;

    if (u32Res == 0) {
        return 0;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pUnref = (struct virtio_gpu_resource_unref *)(void *)g_aReq;
    gpu_hdr_init(&pUnref->hdr, VIRTIO_GPU_CMD_RESOURCE_UNREF);
    pUnref->u32ResourceId = u32Res;
    return gpu_cmd(pUnref, sizeof(*pUnref));
}

/*
 * Drop host resource: detach backing (best-effort) then unref.
 * Bumps g_u32ResId so the next create uses a fresh id. Clears local flags.
 */
static void
gpu_res_drop(void)
{
    if (!g_fHaveRes) {
        return;
    }
    if (g_fScanoutBound) {
        /* Disable scanout before tearing down resource (soft cleanup). */
        struct virtio_gpu_set_scanout *pScan;

        memset(g_aReq, 0, sizeof(g_aReq));
        pScan = (struct virtio_gpu_set_scanout *)(void *)g_aReq;
        gpu_hdr_init(&pScan->hdr, VIRTIO_GPU_CMD_SET_SCANOUT);
        gpu_rect_set(&pScan->r, 0, 0, 0, 0);
        pScan->u32ScanoutId = g_u32ScanoutId;
        pScan->u32ResourceId = 0; /* OASIS: resource_id 0 disables */
        (void)gpu_cmd(pScan, sizeof(*pScan));
        g_fScanoutBound = 0;
    }
    (void)gpu_res_detach(g_u32ResId);
    (void)gpu_res_unref(g_u32ResId);
    g_u32ResId++;
    if (g_u32ResId == 0) {
        g_u32ResId = 1;
    }
    g_fHaveRes = 0;
    g_paFb = 0;
    g_u32Width = 0;
    g_u32Height = 0;
    g_u32Stride = 0;
}

/*
 * Walk guest VA page-by-page, coalesce contiguous PA runs into mem_entries,
 * then ATTACH_BACKING. Identity fallback when page-table translate yields 0.
 * Writes first-page PA to *pOutFirstPa when non-NULL.
 */
static int
gpu_res_attach(u32 u32Res, void *pFb, u32 cbBytes, gj_paddr_t *pOutFirstPa)
{
    struct virtio_gpu_resource_attach_backing *pAttach;
    struct virtio_gpu_mem_entry *pEnts;
    gj_vaddr_t va;
    u32 cbLeft;
    u32 cEnt;
    gj_paddr_t paFirst;

    if (u32Res == 0 || pFb == NULL || cbBytes == 0) {
        return -1;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pAttach = (struct virtio_gpu_resource_attach_backing *)(void *)g_aReq;
    pEnts = (struct virtio_gpu_mem_entry *)(void *)(g_aReq + sizeof(*pAttach));

    va = (gj_vaddr_t)pFb;
    cbLeft = cbBytes;
    cEnt = 0;
    paFirst = 0;

    while (cbLeft > 0) {
        gj_paddr_t pa;
        u32 u32Off;
        u32 u32Chunk;

        u32Off = (u32)(va & (GJ_PAGE_SIZE - 1u));
        u32Chunk = GJ_PAGE_SIZE - u32Off;
        if (u32Chunk > cbLeft) {
            u32Chunk = cbLeft;
        }

        pa = vmm_virt_to_phys(va);
        if (pa == 0) {
            /* Low identity map (Multiboot BSS / early kernel VA==PA). */
            pa = (gj_paddr_t)va;
        }
        if (paFirst == 0) {
            paFirst = pa;
        }

        /* Coalesce with previous run when physically contiguous. */
        if (cEnt > 0) {
            struct virtio_gpu_mem_entry *pPrev = &pEnts[cEnt - 1u];

            if (pPrev->u64Addr + (u64)pPrev->u32Length == (u64)pa) {
                pPrev->u32Length += u32Chunk;
                va += u32Chunk;
                cbLeft -= u32Chunk;
                continue;
            }
        }
        if (cEnt >= VIRTIO_GPU_MAX_MEM_ENTRIES) {
            kprintf("virtio-gpu: attach too fragmented cEnt=%u cb=%u\n",
                    cEnt, cbBytes);
            return -1;
        }
        pEnts[cEnt].u64Addr = (u64)pa;
        pEnts[cEnt].u32Length = u32Chunk;
        pEnts[cEnt].u32Padding = 0;
        cEnt++;
        va += u32Chunk;
        cbLeft -= u32Chunk;
    }

    gpu_hdr_init(&pAttach->hdr, VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING);
    pAttach->u32ResourceId = u32Res;
    pAttach->u32NrEntries = cEnt;
    if (pOutFirstPa != NULL) {
        *pOutFirstPa = paFirst;
    }
    return gpu_cmd(pAttach,
                   (u32)(sizeof(*pAttach) +
                         cEnt * sizeof(struct virtio_gpu_mem_entry)));
}

/* ---- scanout soft path --------------------------------------------------- */

static int
gpu_scanout_set(u32 u32ScanoutId, u32 u32Res,
                u32 u32X, u32 u32Y, u32 u32W, u32 u32H)
{
    struct virtio_gpu_set_scanout *pScan;

    if (u32ScanoutId >= VIRTIO_GPU_MAX_SCANOUTS) {
        return -1;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pScan = (struct virtio_gpu_set_scanout *)(void *)g_aReq;
    gpu_hdr_init(&pScan->hdr, VIRTIO_GPU_CMD_SET_SCANOUT);
    gpu_rect_set(&pScan->r, u32X, u32Y, u32W, u32H);
    pScan->u32ScanoutId = u32ScanoutId;
    pScan->u32ResourceId = u32Res;
    if (gpu_cmd(pScan, sizeof(*pScan)) != 0) {
        return -1;
    }
    if (u32Res == 0) {
        g_fScanoutBound = 0;
    } else {
        g_fScanoutBound = 1;
        g_u32ScanoutId = u32ScanoutId;
    }
    return 0;
}

static int
gpu_scanout_disable(u32 u32ScanoutId)
{
    return gpu_scanout_set(u32ScanoutId, 0, 0, 0, 0, 0);
}

/* ---- transfer + flush soft path ------------------------------------------ */

static int
gpu_xfer_to_host(u32 u32Res, u32 u32X, u32 u32Y, u32 u32W, u32 u32H,
                 u64 u64Offset)
{
    struct virtio_gpu_transfer_to_host_2d *pXfer;

    if (u32Res == 0 || u32W == 0 || u32H == 0) {
        return -1;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pXfer = (struct virtio_gpu_transfer_to_host_2d *)(void *)g_aReq;
    gpu_hdr_init(&pXfer->hdr, VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D);
    gpu_rect_set(&pXfer->r, u32X, u32Y, u32W, u32H);
    pXfer->u64Offset = u64Offset;
    pXfer->u32ResourceId = u32Res;
    return gpu_cmd(pXfer, sizeof(*pXfer));
}

static int
gpu_flush_rect(u32 u32Res, u32 u32X, u32 u32Y, u32 u32W, u32 u32H)
{
    struct virtio_gpu_resource_flush *pFlush;

    if (u32Res == 0 || u32W == 0 || u32H == 0) {
        return -1;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pFlush = (struct virtio_gpu_resource_flush *)(void *)g_aReq;
    gpu_hdr_init(&pFlush->hdr, VIRTIO_GPU_CMD_RESOURCE_FLUSH);
    gpu_rect_set(&pFlush->r, u32X, u32Y, u32W, u32H);
    pFlush->u32ResourceId = u32Res;
    return gpu_cmd(pFlush, sizeof(*pFlush));
}

/*
 * Transfer dirty pixels to host then flush scanout. Offset is byte offset
 * into the resource for the (x,y) origin (stride-aware for non-zero x).
 */
static int
gpu_xfer_and_flush(u32 u32Res, u32 u32X, u32 u32Y, u32 u32W, u32 u32H,
                   u32 u32Stride)
{
    u64 u64Off;

    if (u32Stride == 0) {
        return -1;
    }
    u64Off = (u64)u32Y * (u64)u32Stride + (u64)u32X * 4ull;
    if (gpu_xfer_to_host(u32Res, u32X, u32Y, u32W, u32H, u64Off) != 0) {
        return -1;
    }
    return gpu_flush_rect(u32Res, u32X, u32Y, u32W, u32H);
}

/* ---- display soft path --------------------------------------------------- */

/*
 * GET_DISPLAY_INFO → cache first enabled scanout (or soft default).
 * Hosts that ACK with NODATA keep the default geometry.
 */
static int
gpu_display_refresh(void)
{
    struct virtio_gpu_ctrl_hdr *pReq;
    struct virtio_gpu_resp_display_info *pInfo;
    u32 iScan;
    int fFound;

    if (!g_fReady) {
        return -1;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pReq = (struct virtio_gpu_ctrl_hdr *)(void *)g_aReq;
    gpu_hdr_init(pReq, VIRTIO_GPU_CMD_GET_DISPLAY_INFO);
    if (gpu_cmd(pReq, sizeof(*pReq)) != 0) {
        return -1;
    }

    g_u32DispScanout = 0;
    g_u32DispW = VIRTIO_GPU_DEFAULT_W;
    g_u32DispH = VIRTIO_GPU_DEFAULT_H;
    g_u32DispEnabled = 0;
    g_fDispValid = 1;

    pInfo = (struct virtio_gpu_resp_display_info *)(void *)g_aResp;
    if (pInfo->hdr.u32Type != VIRTIO_GPU_RESP_OK_DISPLAY_INFO) {
        /* NODATA or unexpected OK — soft default already loaded. */
        return 0;
    }

    fFound = 0;
    for (iScan = 0; iScan < VIRTIO_GPU_MAX_SCANOUTS; iScan++) {
        struct virtio_gpu_display_one *pOne = &pInfo->aPmodes[iScan];

        if (!pOne->u32Enabled) {
            continue;
        }
        if (pOne->r.u32Width == 0 || pOne->r.u32Height == 0) {
            continue;
        }
        g_u32DispScanout = iScan;
        g_u32DispW = pOne->r.u32Width;
        g_u32DispH = pOne->r.u32Height;
        g_u32DispEnabled = 1;
        fFound = 1;
        kprintf("virtio-gpu: display%u %ux%u enabled flags=0x%x\n",
                iScan, pOne->r.u32Width, pOne->r.u32Height, pOne->u32Flags);
        break;
    }
    if (!fFound) {
        kprintf("virtio-gpu: no enabled scanout (default %ux%u)\n",
                VIRTIO_GPU_DEFAULT_W, VIRTIO_GPU_DEFAULT_H);
    }
    return 0;
}

/* ---- public API ---------------------------------------------------------- */

/*
 * Probe path: find first gpu → modern PCI caps → features → ctrl q0.
 * Leaves g_fReady=0 and g_pGpu=NULL on any failure.
 */
int
virtio_gpu_probe(void)
{
    u32 i;
    u32 c;
    gj_status_t st;

    g_pGpu = NULL;
    g_fReady = 0;
    g_fHaveRes = 0;
    g_fScanoutBound = 0;
    g_fDispValid = 0;
    g_u32ScanoutId = 0;
    c = virtio_dev_count();
    /* kind==3 or modern GPU device ID (0x1050) */
    for (i = 0; i < c; i++) {
        struct gj_virtio_dev *pDev = virtio_dev_get(i);

        if (pDev == NULL) {
            continue;
        }
        if (pDev->u32Kind == 3 ||
            pDev->u16Device == GJ_VIRTIO_PCI_DEV_GPU_MODERN) {
            g_pGpu = pDev;
            break;
        }
    }
    if (g_pGpu == NULL) {
        kprintf("virtio-gpu: no device\n");
        return -1;
    }

    st = virtio_pci_setup(g_pGpu);
    if (st != GJ_OK || g_pGpu->pCommon == NULL) {
        kprintf("virtio-gpu: pci setup failed %d\n", (int)st);
        g_pGpu = NULL;
        return -1;
    }
    /* Prefer V1; fall back to transitional (empty want mask) */
    st = virtio_negotiate(g_pGpu, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        st = virtio_negotiate(g_pGpu, 0);
        if (st != GJ_OK) {
            kprintf("virtio-gpu: negotiate failed %d\n", (int)st);
            g_pGpu = NULL;
            return -1;
        }
    }
    st = virtio_q_setup(g_pGpu, &g_qCtrl, 0, VIRTIO_GPU_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-gpu: ctrl queue failed %d\n", (int)st);
        g_pGpu = NULL;
        return -1;
    }
    virtio_set_status(g_pGpu, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                   GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
    kprintf("virtio-gpu: ready PASS bus=%x slot=%x (control q0)\n",
            (unsigned)g_pGpu->u8Bus, (unsigned)g_pGpu->u8Slot);
    return 0;
}

int
virtio_gpu_ready(void)
{
    return g_fReady;
}

int
virtio_gpu_get_display(u32 *pOutW, u32 *pOutH)
{
    if (gpu_display_refresh() != 0) {
        return -1;
    }
    if (pOutW) {
        *pOutW = g_u32DispW;
    }
    if (pOutH) {
        *pOutH = g_u32DispH;
    }
    return 0;
}

int
virtio_gpu_get_display_ex(u32 *pOutScanoutId, u32 *pOutW, u32 *pOutH,
                          u32 *pOutEnabled)
{
    if (gpu_display_refresh() != 0) {
        return -1;
    }
    if (pOutScanoutId) {
        *pOutScanoutId = g_u32DispScanout;
    }
    if (pOutW) {
        *pOutW = g_u32DispW;
    }
    if (pOutH) {
        *pOutH = g_u32DispH;
    }
    if (pOutEnabled) {
        *pOutEnabled = g_u32DispEnabled;
    }
    return 0;
}

int
virtio_gpu_flush(u32 u32X, u32 u32Y, u32 u32Width, u32 u32Height)
{
    u32 u32CX;
    u32 u32CY;
    u32 u32CW;
    u32 u32CH;

    if (!g_fReady || !g_fHaveRes) {
        return -1;
    }
    if (gpu_rect_clip(u32X, u32Y, u32Width, u32Height,
                      &u32CX, &u32CY, &u32CW, &u32CH) != 0) {
        return -1;
    }
    return gpu_xfer_and_flush(g_u32ResId, u32CX, u32CY, u32CW, u32CH,
                              g_u32Stride);
}

int
virtio_gpu_scanout(u32 u32ScanoutId, int fEnable)
{
    if (!g_fReady) {
        return -1;
    }
    if (!fEnable) {
        return gpu_scanout_disable(u32ScanoutId);
    }
    if (!g_fHaveRes) {
        return -1;
    }
    return gpu_scanout_set(u32ScanoutId, g_u32ResId, 0, 0,
                           g_u32Width, g_u32Height);
}

int
virtio_gpu_present(u32 u32Width, u32 u32Height, void *pFb, u32 u32Stride)
{
    gj_paddr_t paFb;
    u32 u32Bytes;
    u32 u32Res;
    u32 u32Scan;
    int fReuse;

    if (!g_fReady || pFb == NULL || u32Width == 0 || u32Height == 0) {
        return -1;
    }
    if (u32Stride == 0) {
        u32Stride = u32Width * 4u;
    }
    /* Reject stride that cannot hold one row of B8G8R8X8. */
    if (u32Stride < u32Width * 4u) {
        return -1;
    }
    u32Bytes = u32Stride * u32Height;
    if (u32Bytes / u32Stride != u32Height) {
        return -1; /* overflow */
    }

    /*
     * Prefer page-table translate for first-page PA (reuse key). Full
     * multi-page walk happens inside gpu_res_attach on (re)create.
     */
    paFb = vmm_virt_to_phys((gj_vaddr_t)pFb);
    if (paFb == 0) {
        paFb = (gj_paddr_t)(gj_vaddr_t)pFb;
    }

    fReuse = (g_fHaveRes && g_u32Width == u32Width && g_u32Height == u32Height &&
              g_u32Stride == u32Stride && g_paFb == paFb) ? 1 : 0;

    if (!fReuse) {
        /* Tear down prior host resource when geometry or backing changes. */
        if (g_fHaveRes) {
            gpu_res_drop();
        }
        u32Res = g_u32ResId;

        if (gpu_res_create_2d(u32Res, u32Width, u32Height,
                              VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM) != 0) {
            return -1;
        }
        if (gpu_res_attach(u32Res, pFb, u32Bytes, &paFb) != 0) {
            (void)gpu_res_unref(u32Res);
            g_u32ResId++;
            if (g_u32ResId == 0) {
                g_u32ResId = 1;
            }
            return -1;
        }

        /* Prefer host-reported scanout id when display info is available. */
        u32Scan = 0;
        if (g_fDispValid) {
            u32Scan = g_u32DispScanout;
        } else if (gpu_display_refresh() == 0) {
            u32Scan = g_u32DispScanout;
        }
        if (gpu_scanout_set(u32Scan, u32Res, 0, 0, u32Width, u32Height) != 0) {
            (void)gpu_res_detach(u32Res);
            (void)gpu_res_unref(u32Res);
            g_u32ResId++;
            if (g_u32ResId == 0) {
                g_u32ResId = 1;
            }
            return -1;
        }

        g_u32Width = u32Width;
        g_u32Height = u32Height;
        g_u32Stride = u32Stride;
        g_paFb = paFb;
        g_u32ResId = u32Res;
        g_fHaveRes = 1;
    } else {
        u32Res = g_u32ResId;
    }

    if (gpu_xfer_and_flush(u32Res, 0, 0, u32Width, u32Height, u32Stride) != 0) {
        return -1;
    }

    g_u32PresentCount++;
    kprintf("virtio-gpu: present PASS #%u %ux%u fb=0x%lx%s\n",
            g_u32PresentCount, u32Width, u32Height, (unsigned long)paFb,
            fReuse ? " reuse" : "");
    return 0;
}

int
virtio_gpu_present_stub(u32 u32Width, u32 u32Height, u64 u64GuestAddr)
{
    void *pFb;
    u32 u32Pages;
    u32 u32Bytes;
    u32 i;
    gj_paddr_t pa;
    u8 *p;

    if (!g_fReady) {
        return -1;
    }
    if (u64GuestAddr != 0) {
        return virtio_gpu_present(u32Width, u32Height,
                                  (void *)(gj_vaddr_t)u64GuestAddr,
                                  u32Width * 4u);
    }
    /* Allocate a simple test pattern framebuffer (contiguous) */
    if (u32Width == 0) {
        u32Width = 64;
    }
    if (u32Height == 0) {
        u32Height = 64;
    }
    if (u32Width * u32Height * 4u > 16u * GJ_PAGE_SIZE) {
        u32Width = 64;
        u32Height = 64;
    }
    u32Bytes = u32Width * 4u * u32Height;
    u32Pages = (u32Bytes + GJ_PAGE_SIZE - 1) / GJ_PAGE_SIZE;
    pa = pmm_alloc_pages(u32Pages);
    if (pa == 0) {
        /* Fallback: one page */
        pa = pmm_alloc();
        if (pa == 0) {
            return -1;
        }
        u32Width = 32;
        u32Height = 32;
        u32Bytes = u32Width * 4u * u32Height;
        u32Pages = 1;
    }
    pFb = (void *)hhdm_to_virt(pa);
    p = (u8 *)pFb;
    for (i = 0; i < u32Width * u32Height; i++) {
        /* BGRA: blue gradient */
        p[i * 4u + 0] = (u8)(i & 0xffu);
        p[i * 4u + 1] = 0x40;
        p[i * 4u + 2] = 0x20;
        p[i * 4u + 3] = 0xff;
    }
    return virtio_gpu_present(u32Width, u32Height, pFb, u32Width * 4u);
}

u32
virtio_gpu_present_count(void)
{
    return g_u32PresentCount;
}
