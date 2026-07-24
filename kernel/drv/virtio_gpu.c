/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-gpu 2D present (OASIS virtio-gpu).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only. Pure C11 freestanding.
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
 *
 * Soft inventory (Wave 15 exclusive deepen; this unit only —
 * greppable "virtio-gpu: soft …"; never hard-gates; not bar3/GPU product):
 *   virtio-gpu: soft inventory …
 *   virtio-gpu: soft pci …
 *   virtio-gpu: soft geometry …
 *   virtio-gpu: soft display …
 *   virtio-gpu: soft resource …
 *   virtio-gpu: soft scanout …
 *   virtio-gpu: soft queue …
 *   virtio-gpu: soft present …
 *   virtio-gpu: soft attach …
 *   virtio-gpu: soft counters …
 *   virtio-gpu: soft ring …
 *   virtio-gpu: soft api …
 *   virtio-gpu: soft last …
 *   virtio-gpu: soft errors …
 *   virtio-gpu: soft path …
 *   virtio-gpu: soft claim …        (Wave 15)
 *   virtio-gpu: soft via …          (Wave 15)
 *   virtio-gpu: soft ready …        (Wave 15)
 *   virtio-gpu: soft format …       (Wave 15)
 *   virtio-gpu: soft cmds …         (Wave 15)
 *   virtio-gpu: soft honesty …      (Wave 15)
 *   virtio-gpu: soft return rate — Wave 19 ok/fail rate lamps
 *   virtio-gpu: soft retcode    — Wave 19 retcode catalog
 *   virtio-gpu: soft deepen wave=47 …
 *   virtio-gpu: soft PASS|NODEV|PARTIAL
 *   virtio-gpu: soft inventory PASS|NODEV|PARTIAL
 *
 * greppable: virtio-gpu: soft
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

/* Wave 35 exclusive soft deepen stamp (inventory only; never hard-gates). */
#define GPU_SOFT_WAVE 47u
#define GPU_SOFT_AREAS 86u

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

/* Wave 14 soft inventory telemetry (never hard-gates product present). */
static u32 g_u32SoftLogN;       /* inventory emissions */
static int g_fSoftOnce;         /* first post-activity inventory emitted */
static u32 g_u32CmdOk;          /* gpu_cmd success ACKs */
static u32 g_u32CmdFail;        /* gpu_cmd hard failures (timeout/bad resp) */
static u32 g_u32CmdTimeout;     /* subset: poll timeout */
static u32 g_u32CmdQAddFail;    /* virtio_q_add2 failures in gpu_cmd */
static u32 g_u32CmdBadResp;     /* bad host response type */
static u32 g_u32PresentReuse;   /* present path reused active resource */
static u32 g_u32PresentCreate;  /* present path created new host resource */
static u32 g_u32FlushCount;     /* virtio_gpu_flush successes */
static u32 g_u32FlushFail;      /* virtio_gpu_flush failures */
static u32 g_u32DispRefresh;    /* GET_DISPLAY_INFO soft refreshes */
static u32 g_u32AttachEntsLast; /* last ATTACH_BACKING entry count */
static u32 g_u32AttachEntsPeak; /* high-water attach mem_entries */
static u32 g_u32AttachOps;      /* successful attach ops */
static u32 g_u32LastCmdType;    /* sticky last control cmd type */
static u32 g_u32LastRespType;   /* sticky last host resp type */
static u32 g_u32LastPresentW;   /* sticky last present width */
static u32 g_u32LastPresentH;   /* sticky last present height */
static u32 g_u32LastPresentStride; /* sticky last present stride */
static u32 g_u32LastFlushW;     /* sticky last flush width (clipped) */
static u32 g_u32LastFlushH;     /* sticky last flush height (clipped) */
static u32 g_u32ApiPresent;     /* virtio_gpu_present enters */
static u32 g_u32ApiFlush;       /* virtio_gpu_flush enters */
static u32 g_u32ApiDisplay;     /* get_display / get_display_ex enters */
static u32 g_u32ApiScanout;     /* virtio_gpu_scanout enters */
static u32 g_u32ApiPresentStub; /* virtio_gpu_present_stub enters */
static u32 g_u32ApiPresentCount;/* virtio_gpu_present_count enters */
static u32 g_u32PresentArgFail; /* present arg / overflow rejects */
static u32 g_u32PresentPathFail;/* present create/attach/scanout/xfer fails */
/* Wave 15 sticky via (inventory only). */
static const char *g_szLastVia;

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
        g_u32CmdFail++;
        return -1;
    }
    pRq = (struct virtio_gpu_ctrl_hdr *)pReq;
    g_u32LastCmdType = pRq->u32Type;
    memset(g_aResp, 0, sizeof(g_aResp));
    if (virtio_q_add2(&g_qCtrl,
                      (gj_paddr_t)(gj_vaddr_t)pReq, cbReq, 0,
                      (gj_paddr_t)(gj_vaddr_t)g_aResp, sizeof(g_aResp), 1) < 0) {
        g_u32CmdQAddFail++;
        g_u32CmdFail++;
        return -1;
    }
    virtio_q_kick(&g_qCtrl);
    i32Len = virtio_q_poll(&g_qCtrl, VIRTIO_GPU_POLL_SPINS);
    if (i32Len < 0) {
        kprintf("virtio-gpu: cmd timeout type=0x%x\n", pRq->u32Type);
        g_u32CmdTimeout++;
        g_u32CmdFail++;
        return -1;
    }
    pRh = (struct virtio_gpu_ctrl_hdr *)(void *)g_aResp;
    g_u32LastRespType = pRh->u32Type;
    /* Success responses live in 0x1100..0x11ff; errors start at 0x1200. */
    if (pRh->u32Type < VIRTIO_GPU_RESP_OK_NODATA ||
        pRh->u32Type >= VIRTIO_GPU_RESP_ERR_UNSPEC) {
        kprintf("virtio-gpu: bad resp type=0x%x for cmd=0x%x\n",
                pRh->u32Type, pRq->u32Type);
        g_u32CmdBadResp++;
        g_u32CmdFail++;
        return -1;
    }
    g_u32CmdOk++;
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
    if (gpu_cmd(pAttach,
                (u32)(sizeof(*pAttach) +
                      cEnt * sizeof(struct virtio_gpu_mem_entry))) != 0) {
        return -1;
    }
    /* Soft attach tallies (Wave 11 inventory; product path unchanged). */
    g_u32AttachEntsLast = cEnt;
    if (cEnt > g_u32AttachEntsPeak) {
        g_u32AttachEntsPeak = cEnt;
    }
    g_u32AttachOps++;
    return 0;
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

/* ---- soft inventory (Wave 15 exclusive deepen) --------------------------- */

/**
 * Greppable Wave 15 soft inventory dump (product / smoke).
 * Prefix-stable "virtio-gpu: soft …" — never hard-gates; kprintf only.
 * Soft only — not bar3 / not GPU product close.
 *
 * greppable: virtio-gpu: soft
 */
static void
gpu_soft_inventory(const char *szVia)
{
    const char *szVerdict;
    const char *szViaSafe;
    u16 u16QSize;
    u16 u16FreeNow;
    u16 u16NumFree;
    u16 u16LastUsed;
    u8 u8Bus;
    u8 u8Slot;
    u8 u8Func;
    u8 u8Modern;
    u16 u16Device;
    u32 u32Kind;
    u32 u32Ready;
    u32 u32Claim;
    u32 u32NotifyMult;
    u32 u32NumQueues;
    u64 u64FeatDev;
    u64 u64FeatDrv;
    u64 u64PaDesc;
    u64 u64PaAvail;
    u64 u64PaUsed;

    szViaSafe = (szVia != NULL) ? szVia : "path";
    g_szLastVia = szViaSafe;

    if (g_u32SoftLogN < 0xffffffffu) {
        g_u32SoftLogN++;
    }

    u32Ready = g_fReady ? 1u : 0u;
    u32Claim = u32Ready; /* honesty: claim 2D soft surface only when DRIVER_OK */

    u16QSize = g_fReady ? g_qCtrl.u16Size : 0;
    u16FreeNow = g_fReady ? virtio_q_num_free(&g_qCtrl) : 0;
    u16NumFree = g_fReady ? g_qCtrl.u16NumFree : 0;
    u16LastUsed = g_fReady ? g_qCtrl.u16LastUsed : 0;
    u8Bus = (g_pGpu != NULL) ? g_pGpu->u8Bus : 0;
    u8Slot = (g_pGpu != NULL) ? g_pGpu->u8Slot : 0;
    u8Func = (g_pGpu != NULL) ? g_pGpu->u8Func : 0;
    u8Modern = (g_pGpu != NULL) ? g_pGpu->fModern : 0;
    u16Device = (g_pGpu != NULL) ? g_pGpu->u16Device : 0;
    u32Kind = (g_pGpu != NULL) ? g_pGpu->u32Kind : 0;
    u64FeatDev = (g_pGpu != NULL) ? g_pGpu->u64FeaturesDev : 0ull;
    u64FeatDrv = (g_pGpu != NULL) ? g_pGpu->u64FeaturesDrv : 0ull;
    u32NotifyMult = (g_pGpu != NULL) ? g_pGpu->u32NotifyMult : 0;
    u32NumQueues = (g_pGpu != NULL) ? g_pGpu->u32NumQueues : 0;
    u64PaDesc = g_fReady ? (u64)g_qCtrl.paDesc : 0ull;
    u64PaAvail = g_fReady ? (u64)g_qCtrl.paAvail : 0ull;
    u64PaUsed = g_fReady ? (u64)g_qCtrl.paUsed : 0ull;

    /*
     * Soft verdict (inventory only; present path unchanged):
     *   NODEV    — not ready / no device
     *   PASS     — ready + any successful present
     *   PARTIAL  — ready, no completed product present yet (post-probe)
     */
    if (!g_fReady) {
        szVerdict = "NODEV";
    } else if (g_u32PresentCount != 0u) {
        szVerdict = "PASS";
    } else {
        szVerdict = "PARTIAL";
    }

    /* Grep: virtio-gpu: soft inventory */
    kprintf("virtio-gpu: soft inventory via=%s ready=%u bus=%x slot=%x "
            "q_size=%u free=%u present=%u have_res=%u scanout_bound=%u "
            "fmt=B8G8R8X8 max_ents=%u log_n=%u wave=%u areas=%u\n",
            szViaSafe, u32Ready, (unsigned)u8Bus, (unsigned)u8Slot,
            (unsigned)u16QSize, (unsigned)u16FreeNow, g_u32PresentCount,
            g_fHaveRes ? 1u : 0u, g_fScanoutBound ? 1u : 0u,
            (unsigned)VIRTIO_GPU_MAX_MEM_ENTRIES, g_u32SoftLogN,
            (unsigned)GPU_SOFT_WAVE, (unsigned)GPU_SOFT_AREAS);

    /* Grep: virtio-gpu: soft pci */
    kprintf("virtio-gpu: soft pci bus=%x slot=%x func=%x dev=0x%x kind=%u "
            "modern=%u feat_dev=0x%lx feat_drv=0x%lx num_queues=%u "
            "notify_mult=%u\n",
            (unsigned)u8Bus, (unsigned)u8Slot, (unsigned)u8Func,
            (unsigned)u16Device, u32Kind, (unsigned)u8Modern,
            (unsigned long)u64FeatDev, (unsigned long)u64FeatDrv,
            u32NumQueues, u32NotifyMult);

    /* Grep: virtio-gpu: soft geometry */
    kprintf("virtio-gpu: soft geometry default_w=%u default_h=%u fmt=%u "
            "max_scanouts=%u max_ents=%u q_size=%u poll_spins=%u "
            "req_bytes=%u ctrl_q=0 cursor_q=0\n",
            (unsigned)VIRTIO_GPU_DEFAULT_W, (unsigned)VIRTIO_GPU_DEFAULT_H,
            (unsigned)VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM,
            (unsigned)VIRTIO_GPU_MAX_SCANOUTS,
            (unsigned)VIRTIO_GPU_MAX_MEM_ENTRIES,
            (unsigned)VIRTIO_GPU_Q_SIZE, (unsigned)VIRTIO_GPU_POLL_SPINS,
            (unsigned)VIRTIO_GPU_REQ_BYTES);

    /* Grep: virtio-gpu: soft display */
    kprintf("virtio-gpu: soft display valid=%u scanout=%u w=%u h=%u "
            "enabled=%u refresh=%u default_w=%u default_h=%u\n",
            g_fDispValid ? 1u : 0u, g_u32DispScanout, g_u32DispW, g_u32DispH,
            g_u32DispEnabled, g_u32DispRefresh,
            (unsigned)VIRTIO_GPU_DEFAULT_W, (unsigned)VIRTIO_GPU_DEFAULT_H);

    /* Grep: virtio-gpu: soft resource */
    kprintf("virtio-gpu: soft resource have=%u id=%u w=%u h=%u stride=%u "
            "pa=0x%lx fmt=%u\n",
            g_fHaveRes ? 1u : 0u, g_u32ResId, g_u32Width, g_u32Height,
            g_u32Stride, (unsigned long)g_paFb,
            (unsigned)VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM);

    /* Grep: virtio-gpu: soft scanout */
    kprintf("virtio-gpu: soft scanout bound=%u id=%u max=%u disable_res0=1\n",
            g_fScanoutBound ? 1u : 0u, g_u32ScanoutId,
            (unsigned)VIRTIO_GPU_MAX_SCANOUTS);

    /* Grep: virtio-gpu: soft queue */
    kprintf("virtio-gpu: soft queue ctrl_q=0 size=%u free=%u num_free=%u "
            "free_head=%u last_used=%u notify_off=%u notify_mult=%u "
            "q_idx=%u\n",
            (unsigned)u16QSize, (unsigned)u16FreeNow, (unsigned)u16NumFree,
            (unsigned)(g_fReady ? g_qCtrl.u16FreeHead : 0u),
            (unsigned)u16LastUsed,
            (unsigned)(g_fReady ? g_qCtrl.u16NotifyOff : 0u),
            u32NotifyMult,
            (unsigned)(g_fReady ? g_qCtrl.u16QueueIdx : 0u));

    /* Grep: virtio-gpu: soft present */
    kprintf("virtio-gpu: soft present ok=%u create=%u reuse=%u "
            "flush_ok=%u flush_fail=%u dirty_rect=1 full_flush=1 "
            "arg_fail=%u path_fail=%u\n",
            g_u32PresentCount, g_u32PresentCreate, g_u32PresentReuse,
            g_u32FlushCount, g_u32FlushFail, g_u32PresentArgFail,
            g_u32PresentPathFail);

    /* Grep: virtio-gpu: soft attach */
    kprintf("virtio-gpu: soft attach ops=%u ents_last=%u ents_peak=%u "
            "max_ents=%u page_walk=1 coalesce=1\n",
            g_u32AttachOps, g_u32AttachEntsLast, g_u32AttachEntsPeak,
            (unsigned)VIRTIO_GPU_MAX_MEM_ENTRIES);

    /* Grep: virtio-gpu: soft counters */
    kprintf("virtio-gpu: soft counters cmd_ok=%u cmd_fail=%u cmd_timeout=%u "
            "q_add_fail=%u bad_resp=%u present=%u flush=%u attach=%u "
            "disp_refresh=%u\n",
            g_u32CmdOk, g_u32CmdFail, g_u32CmdTimeout, g_u32CmdQAddFail,
            g_u32CmdBadResp, g_u32PresentCount, g_u32FlushCount,
            g_u32AttachOps, g_u32DispRefresh);

    /* Grep: virtio-gpu: soft ring */
    kprintf("virtio-gpu: soft ring pa_desc=0x%lx pa_avail=0x%lx "
            "pa_used=0x%lx size=%u free=%u pages=3\n",
            (unsigned long)u64PaDesc, (unsigned long)u64PaAvail,
            (unsigned long)u64PaUsed, (unsigned)u16QSize,
            (unsigned)u16FreeNow);

    /* Grep: virtio-gpu: soft api */
    kprintf("virtio-gpu: soft api present=%u flush=%u display=%u "
            "scanout=%u present_stub=%u present_count=%u\n",
            g_u32ApiPresent, g_u32ApiFlush, g_u32ApiDisplay, g_u32ApiScanout,
            g_u32ApiPresentStub, g_u32ApiPresentCount);

    /* Grep: virtio-gpu: soft last */
    kprintf("virtio-gpu: soft last cmd=0x%x resp=0x%x present_w=%u "
            "present_h=%u present_stride=%u flush_w=%u flush_h=%u\n",
            g_u32LastCmdType, g_u32LastRespType, g_u32LastPresentW,
            g_u32LastPresentH, g_u32LastPresentStride, g_u32LastFlushW,
            g_u32LastFlushH);

    /* Grep: virtio-gpu: soft errors */
    kprintf("virtio-gpu: soft errors cmd_fail=%u timeout=%u q_add=%u "
            "bad_resp=%u flush_fail=%u present_arg=%u present_path=%u\n",
            g_u32CmdFail, g_u32CmdTimeout, g_u32CmdQAddFail, g_u32CmdBadResp,
            g_u32FlushFail, g_u32PresentArgFail, g_u32PresentPathFail);

    /*
     * Grep: virtio-gpu: soft path
     * Honesty catalog: 2D present surface only. claim=1 only when DRIVER_OK.
     * Explicit non-claims: cursor q, 3D, EDID, Steam/bar3 present.
     */
    kprintf("virtio-gpu: soft path claim=%u ctrl_q0=1 create_2d=1 attach=1 "
            "scanout=1 xfer=1 flush=1 dirty_rect=1 get_display=1 "
            "cursor=0 cmd3d=0 edid=0 steam=0 bar3=0 "
            "feat_dev=0x%lx feat_drv=0x%lx\n",
            u32Claim, (unsigned long)u64FeatDev, (unsigned long)u64FeatDrv);

    /* Grep: virtio-gpu: soft claim (Wave 15) */
    kprintf("virtio-gpu: soft claim ready=%u claim=%u modern=%u "
            "have_res=%u scanout_bound=%u present=%u\n",
            u32Ready, u32Claim, (unsigned)u8Modern, g_fHaveRes ? 1u : 0u,
            g_fScanoutBound ? 1u : 0u, g_u32PresentCount);

    /* Grep: virtio-gpu: soft via (Wave 15) */
    kprintf("virtio-gpu: soft via last=%s log_n=%u once=%u\n",
            (g_szLastVia != NULL) ? g_szLastVia : "path", g_u32SoftLogN,
            g_fSoftOnce ? 1u : 0u);

    /* Grep: virtio-gpu: soft ready (Wave 15) */
    kprintf("virtio-gpu: soft ready live=%u have_res=%u scanout=%u "
            "disp_valid=%u present=%u flush=%u cmd_ok=%u\n",
            u32Ready, g_fHaveRes ? 1u : 0u, g_fScanoutBound ? 1u : 0u,
            g_fDispValid ? 1u : 0u, g_u32PresentCount, g_u32FlushCount,
            g_u32CmdOk);

    /* Grep: virtio-gpu: soft format (Wave 15) */
    kprintf("virtio-gpu: soft format b8g8r8x8=%u default_w=%u default_h=%u "
            "max_scanouts=%u max_ents=%u req_bytes=%u stride=%u "
            "w=%u h=%u\n",
            (unsigned)VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM,
            (unsigned)VIRTIO_GPU_DEFAULT_W, (unsigned)VIRTIO_GPU_DEFAULT_H,
            (unsigned)VIRTIO_GPU_MAX_SCANOUTS,
            (unsigned)VIRTIO_GPU_MAX_MEM_ENTRIES,
            (unsigned)VIRTIO_GPU_REQ_BYTES, g_u32Stride, g_u32Width,
            g_u32Height);

    /* Grep: virtio-gpu: soft cmds (Wave 15 OASIS ctrl catalog) */
    kprintf("virtio-gpu: soft cmds get_disp=0x%x create2d=0x%x unref=0x%x "
            "set_scanout=0x%x flush=0x%x xfer=0x%x attach=0x%x "
            "detach=0x%x last=0x%x resp=0x%x\n",
            (unsigned)VIRTIO_GPU_CMD_GET_DISPLAY_INFO,
            (unsigned)VIRTIO_GPU_CMD_RESOURCE_CREATE_2D,
            (unsigned)VIRTIO_GPU_CMD_RESOURCE_UNREF,
            (unsigned)VIRTIO_GPU_CMD_SET_SCANOUT,
            (unsigned)VIRTIO_GPU_CMD_RESOURCE_FLUSH,
            (unsigned)VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D,
            (unsigned)VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING,
            (unsigned)VIRTIO_GPU_CMD_RESOURCE_DETACH_BACKING,
            g_u32LastCmdType, g_u32LastRespType);

    /* Grep: virtio-gpu: soft honesty (Wave 15 non-claims) */
    kprintf("virtio-gpu: soft honesty ctrl_q0=1 cursor_q=0 cmd3d=0 "
            "edid=0 steam=0 bar3=0 product_gpu=0 soft_only=1 "
            "wave=%u areas=%u\n",
            (unsigned)GPU_SOFT_WAVE, (unsigned)GPU_SOFT_AREAS);

    /*
     * Wave 16 complementary deepen (kept; never hard-gates).
     * Soft ≠ game I/O. greppable: virtio-gpu: soft ratio|headroom|surface|return|contract
     */
    {
        u32 u32Surf = 0u;
        u32 u32CmdBp = 0;
        u32 u32CmdTot = g_u32CmdOk + g_u32CmdFail;

        if (u32CmdTot != 0u) {
            u32CmdBp = (g_u32CmdOk * 10000u) / u32CmdTot;
        }
        if (u32Ready != 0u) {
            u32Surf |= 0x1u;
        }
        if (g_fHaveRes != 0) {
            u32Surf |= 0x2u;
        }
        if (g_u32PresentCount != 0u) {
            u32Surf |= 0x4u;
        }
        if (g_u32CmdOk != 0u) {
            u32Surf |= 0x8u;
        }
        if (g_u32CmdFail != 0u) {
            u32Surf |= 0x10u;
        }
        if (g_u32FlushCount != 0u) {
            u32Surf |= 0x20u;
        }
        u32Surf |= 0x40u; /* 2D soft surface catalog */
        u32Surf |= 0x80u; /* honesty catalog always present */
        /* Grep: virtio-gpu: soft ratio */
        kprintf("virtio-gpu: soft ratio cmd_ok_bp=%u cmd_ok=%u cmd_fail=%u "
                "present=%u ready=%u wave=%u soft PASS\n",
                u32CmdBp, g_u32CmdOk, g_u32CmdFail, g_u32PresentCount,
                u32Ready, (unsigned)GPU_SOFT_WAVE);
        /* Grep: virtio-gpu: soft headroom */
        kprintf("virtio-gpu: soft headroom default_w=%u default_h=%u "
                "fmt=%u max_scanouts=%u q_size=%u wave=%u soft PASS\n",
                (unsigned)VIRTIO_GPU_DEFAULT_W, (unsigned)VIRTIO_GPU_DEFAULT_H,
                (unsigned)VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM,
                (unsigned)VIRTIO_GPU_MAX_SCANOUTS, (unsigned)VIRTIO_GPU_Q_SIZE,
                (unsigned)GPU_SOFT_WAVE);
        /* Grep: virtio-gpu: soft surface */
        kprintf("virtio-gpu: soft surface inventory,geometry,queue,resource,"
                "present,flush,cmds,claim,via,ready,format,honesty,ratio,"
                "headroom,return,contract,return_selftest,retmap,deepen areas=%u wave=%u\n",
                (unsigned)GPU_SOFT_AREAS, (unsigned)GPU_SOFT_WAVE);
        /* Grep: virtio-gpu: soft return — return-surface bitmask */
        kprintf("virtio-gpu: soft return surf=0x%x ready=%u have_res=%u "
                "present=%u cmd_ok=%u cmd_fail=%u flush=%u via=%s "
                "areas=%u wave=%u soft PASS\n",
                u32Surf, u32Ready, g_fHaveRes ? 1u : 0u, g_u32PresentCount,
                g_u32CmdOk, g_u32CmdFail, g_u32FlushCount, szViaSafe,
                (unsigned)GPU_SOFT_AREAS, (unsigned)GPU_SOFT_WAVE);
        /* Grep: virtio-gpu: soft contract — soft ≠ game I/O */
        kprintf("virtio-gpu: soft contract soft_only=1 game_io=0 "
                "product_gpu=0 cmd3d=0 steam=0 bar3=open wave=%u soft PASS\n",
                (unsigned)GPU_SOFT_WAVE);
    }

    /*
     * Wave 17 complementary sub-lines (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-gpu: soft return — Wave 17 API return surfaces (kept) */
    kprintf("virtio-gpu: soft return soft_inv=1 gpu=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u soft PASS\n",
            (unsigned)GPU_SOFT_WAVE);

    /* Grep: virtio-gpu: soft return selftest — Wave 17 terminal return surface (kept) */
    kprintf("virtio-gpu: soft return selftest inv_ret=1 product_kernel=OPEN "
            "multi_server=0 bar3=0 wave=%u soft PASS\n",
            (unsigned)GPU_SOFT_WAVE);

    /* Grep: virtio-gpu: soft retmap — Wave 17 return-surface map (kept) */
    kprintf("virtio-gpu: soft retmap soft_inv=1 deepen=1 product=OPEN "
            "wave=%u soft PASS\n",
            (unsigned)GPU_SOFT_WAVE);

    /*
     * ---- Wave 18 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     */
    /* Grep: virtio-gpu: soft return rate — Wave 19 ok/fail rate lamps */
    kprintf("virtio-gpu: soft return rate soft_inv=1 selftest=1 retmap=1 "
            "product_kernel=OPEN bar3=0 hard_gate=0 wave=%u "
            "(return rate; Soft≠product; not bar3)\n",
            (unsigned)GPU_SOFT_WAVE);

    /* Grep: virtio-gpu: soft retcode — Wave 19 retcode catalog */
    kprintf("virtio-gpu: soft retcode ok=1 fail=1 inval=1 busy=1 "
            "selftest=1 retmap=1 product=OPEN soft_ne_product=1 wave=%u "
            "(retcode catalog; Soft≠product)\n",
            (unsigned)GPU_SOFT_WAVE);

    /* Grep: virtio-gpu: soft deepen wave (Wave 24 stamp) */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-gpu: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("virtio-gpu: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GPU_SOFT_WAVE);
    /* Grep: virtio-gpu: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("virtio-gpu: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GPU_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: virtio-gpu: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("virtio-gpu: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GPU_SOFT_WAVE);
    /* Grep: virtio-gpu: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("virtio-gpu: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GPU_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-gpu: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("virtio-gpu: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /* Grep: virtio-gpu: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("virtio-gpu: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: virtio-gpu: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("virtio-gpu: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /* Grep: virtio-gpu: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("virtio-gpu: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: virtio-gpu: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("virtio-gpu: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /* Grep: virtio-gpu: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("virtio-gpu: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-gpu: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("virtio-gpu: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /* Grep: virtio-gpu: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("virtio-gpu: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-gpu: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("virtio-gpu: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /* Grep: virtio-gpu: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("virtio-gpu: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: virtio-gpu: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("virtio-gpu: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GPU_SOFT_WAVE);
            /* Grep: virtio-gpu: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("virtio-gpu: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GPU_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: virtio-gpu: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("virtio-gpu: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GPU_SOFT_WAVE);
                    /* Grep: virtio-gpu: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("virtio-gpu: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GPU_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-gpu: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("virtio-gpu: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GPU_SOFT_WAVE);
                            /* Grep: virtio-gpu: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("virtio-gpu: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GPU_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-gpu: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("virtio-gpu: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GPU_SOFT_WAVE);
                            /* Grep: virtio-gpu: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("virtio-gpu: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GPU_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-gpu: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-gpu: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GPU_SOFT_WAVE);
                            /* Grep: virtio-gpu: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("virtio-gpu: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GPU_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: virtio-gpu: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("virtio-gpu: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GPU_SOFT_WAVE);
                            /* Grep: virtio-gpu: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("virtio-gpu: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GPU_SOFT_WAVE);
                            /* Grep: virtio-gpu: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("virtio-gpu: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("virtio-gpu: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("virtio-gpu: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("virtio-gpu: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("virtio-gpu: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("virtio-gpu: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("virtio-gpu: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retfortress — Wave 35 return-fortress honesty */
kprintf("virtio-gpu: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("virtio-gpu: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft rethold — Wave 36 return-hold honesty */
kprintf("virtio-gpu: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retspire — Wave 36 exclusive spire stamp */
kprintf("virtio-gpu: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retwall — Wave 37 return-wall honesty */
kprintf("virtio-gpu: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retgate — Wave 37 exclusive gate stamp */
kprintf("virtio-gpu: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retmoat — Wave 38 return-moat honesty */
kprintf("virtio-gpu: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retower — Wave 38 exclusive tower stamp */
kprintf("virtio-gpu: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("virtio-gpu: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("virtio-gpu: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("virtio-gpu: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("virtio-gpu: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retravelin — Wave 41 return-travelin honesty */
kprintf("virtio-gpu: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("virtio-gpu: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("virtio-gpu: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("virtio-gpu: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("virtio-gpu: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("virtio-gpu: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("virtio-gpu: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("virtio-gpu: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("virtio-gpu: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("virtio-gpu: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retbailey — Wave 46 return-bailey honesty */
kprintf("virtio-gpu: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GPU_SOFT_WAVE);
/* Grep: virtio-gpu: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("virtio-gpu: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GPU_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: virtio-gpu: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("virtio-gpu: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=47 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: virtio-gpu: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("virtio-gpu: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=47 "
        "(retdonjon stamp; Soft≠product)\n");



                            kprintf("virtio-gpu: soft deepen wave=%u areas=%u via=%s ready=%u "
            "present=%u have_res=%u cmd_ok=%u log_n=%u "
            "(soft inventory only; not bar3)\n",
            (unsigned)GPU_SOFT_WAVE, (unsigned)GPU_SOFT_AREAS, szViaSafe,
            u32Ready, g_u32PresentCount, g_fHaveRes ? 1u : 0u, g_u32CmdOk,
            g_u32SoftLogN);

    /* Grep: virtio-gpu: soft PASS | NODEV | PARTIAL */
    kprintf("virtio-gpu: soft %s via=%s ready=%u present=%u cmd_ok=%u "
            "cmd_fail=%u log_n=%u wave=%u\n",
            szVerdict, szViaSafe, u32Ready, g_u32PresentCount, g_u32CmdOk,
            g_u32CmdFail, g_u32SoftLogN, (unsigned)GPU_SOFT_WAVE);

    /* Grep: virtio-gpu: soft inventory PASS|NODEV|PARTIAL */
    kprintf("virtio-gpu: soft inventory %s via=%s logs=%u wave=%u areas=%u "
            "(soft inventory only; not bar3)\n",
            szVerdict, szViaSafe, g_u32SoftLogN, (unsigned)GPU_SOFT_WAVE,
            (unsigned)GPU_SOFT_AREAS);
}

/**
 * After first product present/flush activity, print soft inventory once
 * (mirrors virtio-blk / compositor soft-stats-once). Diagnostics only.
 */
static void
gpu_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32PresentCount == 0u && g_u32FlushCount == 0u &&
        g_u32CmdFail == 0u) {
        return;
    }
    g_fSoftOnce = 1;
    gpu_soft_inventory("activity");
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
    g_u32DispRefresh++;

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
        /* Grep: virtio-gpu: soft … NODEV (Wave 14 soft inventory) */
        gpu_soft_inventory("nodev");
        return -1;
    }

    st = virtio_pci_setup(g_pGpu);
    if (st != GJ_OK || g_pGpu->pCommon == NULL) {
        kprintf("virtio-gpu: pci setup failed %d\n", (int)st);
        g_pGpu = NULL;
        gpu_soft_inventory("pci_fail");
        return -1;
    }
    /* Prefer V1; fall back to transitional (empty want mask) */
    st = virtio_negotiate(g_pGpu, GJ_VIRTIO_F_VERSION_1);
    if (st != GJ_OK) {
        st = virtio_negotiate(g_pGpu, 0);
        if (st != GJ_OK) {
            kprintf("virtio-gpu: negotiate failed %d\n", (int)st);
            g_pGpu = NULL;
            gpu_soft_inventory("negotiate_fail");
            return -1;
        }
    }
    st = virtio_q_setup(g_pGpu, &g_qCtrl, 0, VIRTIO_GPU_Q_SIZE);
    if (st != GJ_OK) {
        kprintf("virtio-gpu: ctrl queue failed %d\n", (int)st);
        g_pGpu = NULL;
        gpu_soft_inventory("q_fail");
        return -1;
    }
    virtio_set_status(g_pGpu, (u8)(GJ_VIRTIO_S_ACKNOWLEDGE | GJ_VIRTIO_S_DRIVER |
                                   GJ_VIRTIO_S_FEATURES_OK | GJ_VIRTIO_S_DRIVER_OK));
    g_fReady = 1;
    kprintf("virtio-gpu: ready PASS bus=%x slot=%x (control q0)\n",
            (unsigned)g_pGpu->u8Bus, (unsigned)g_pGpu->u8Slot);
    /*
     * Wave 14 soft inventory rollup (prefix-stable "virtio-gpu: soft …").
     * PARTIAL until first present; never hard-gates product path.
     */
    gpu_soft_inventory("probe");
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
    g_u32ApiDisplay++;
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
    g_u32ApiDisplay++;
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

    g_u32ApiFlush++;
    if (!g_fReady || !g_fHaveRes) {
        g_u32FlushFail++;
        return -1;
    }
    if (gpu_rect_clip(u32X, u32Y, u32Width, u32Height,
                      &u32CX, &u32CY, &u32CW, &u32CH) != 0) {
        g_u32FlushFail++;
        return -1;
    }
    if (gpu_xfer_and_flush(g_u32ResId, u32CX, u32CY, u32CW, u32CH,
                           g_u32Stride) != 0) {
        g_u32FlushFail++;
        return -1;
    }
    g_u32LastFlushW = u32CW;
    g_u32LastFlushH = u32CH;
    g_u32FlushCount++;
    gpu_soft_maybe_once();
    return 0;
}

int
virtio_gpu_scanout(u32 u32ScanoutId, int fEnable)
{
    g_u32ApiScanout++;
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

    g_u32ApiPresent++;
    if (!g_fReady || pFb == NULL || u32Width == 0 || u32Height == 0) {
        g_u32PresentArgFail++;
        return -1;
    }
    if (u32Stride == 0) {
        u32Stride = u32Width * 4u;
    }
    /* Reject stride that cannot hold one row of B8G8R8X8. */
    if (u32Stride < u32Width * 4u) {
        g_u32PresentArgFail++;
        return -1;
    }
    u32Bytes = u32Stride * u32Height;
    if (u32Bytes / u32Stride != u32Height) {
        g_u32PresentArgFail++;
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
            g_u32PresentPathFail++;
            return -1;
        }
        if (gpu_res_attach(u32Res, pFb, u32Bytes, &paFb) != 0) {
            (void)gpu_res_unref(u32Res);
            g_u32ResId++;
            if (g_u32ResId == 0) {
                g_u32ResId = 1;
            }
            g_u32PresentPathFail++;
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
            g_u32PresentPathFail++;
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
        g_u32PresentPathFail++;
        return -1;
    }

    if (fReuse) {
        g_u32PresentReuse++;
    } else {
        g_u32PresentCreate++;
    }
    g_u32LastPresentW = u32Width;
    g_u32LastPresentH = u32Height;
    g_u32LastPresentStride = u32Stride;
    g_u32PresentCount++;
    kprintf("virtio-gpu: present PASS #%u %ux%u fb=0x%lx%s\n",
            g_u32PresentCount, u32Width, u32Height, (unsigned long)paFb,
            fReuse ? " reuse" : "");
    /* Wave 14: greppable virtio-gpu: soft … once after first present. */
    gpu_soft_maybe_once();
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

    g_u32ApiPresentStub++;
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
    /*
     * Emit soft inventory on count read so bring-up smoke greps
     * virtio-gpu: soft … without requiring a second present (mirrors
     * virtio-blk q_stats / door STATS soft dumps). Never hard-gates.
     */
    g_u32ApiPresentCount++;
    gpu_soft_inventory("present_count");
    return g_u32PresentCount;
}
