/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room virtio-gpu 2D present (OASIS virtio-gpu).
 * No Linux virtio source. Dual MIT OR Apache-2.0 only.
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

/* virtio-gpu control types (public OASIS) */
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

#define VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM       2u
#define VIRTIO_GPU_MAX_SCANOUTS                16u

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

struct virtio_gpu_ctrl_hdr_resp {
    struct virtio_gpu_ctrl_hdr hdr;
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

/* Request/response buffers (identity-mapped DMA) */
static u8 g_aReq[512] __attribute__((aligned(16)));
static u8 g_aResp[1024] __attribute__((aligned(16)));

static struct gj_virtio_dev *g_pGpu;
static struct gj_virtq       g_qCtrl;
static u32                   g_u32PresentCount;
static u32                   g_u32ResId = 1; /* active host resource when fHaveRes */
static int                   g_fReady;
static int                   g_fHaveRes;
static u32                   g_u32Width;
static u32                   g_u32Height;
static u32                   g_u32Stride;
static gj_paddr_t            g_paFb;

static int
gpu_cmd(void *pReq, u32 cbReq)
{
    i32 i32Len;

    memset(g_aResp, 0, sizeof(g_aResp));
    if (virtio_q_add2(&g_qCtrl,
                      (gj_paddr_t)(gj_vaddr_t)pReq, cbReq, 0,
                      (gj_paddr_t)(gj_vaddr_t)g_aResp, sizeof(g_aResp), 1) < 0) {
        return -1;
    }
    virtio_q_kick(&g_qCtrl);
    i32Len = virtio_q_poll(&g_qCtrl, 10000000u);
    if (i32Len < 0) {
        kprintf("virtio-gpu: cmd timeout type=%u\n",
                ((struct virtio_gpu_ctrl_hdr *)pReq)->u32Type);
        return -1;
    }
    {
        struct virtio_gpu_ctrl_hdr *pRh =
            (struct virtio_gpu_ctrl_hdr *)(void *)g_aResp;

        if (pRh->u32Type < VIRTIO_GPU_RESP_OK_NODATA ||
            pRh->u32Type > 0x11ffu) {
            kprintf("virtio-gpu: bad resp type=0x%x for cmd=0x%x\n",
                    pRh->u32Type,
                    ((struct virtio_gpu_ctrl_hdr *)pReq)->u32Type);
            return -1;
        }
    }
    return 0;
}

static void
gpu_unref(u32 u32Res)
{
    struct virtio_gpu_resource_unref *pUnref;

    if (u32Res == 0) {
        return;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pUnref = (struct virtio_gpu_resource_unref *)(void *)g_aReq;
    pUnref->hdr.u32Type = VIRTIO_GPU_CMD_RESOURCE_UNREF;
    pUnref->u32ResourceId = u32Res;
    (void)gpu_cmd(pUnref, sizeof(*pUnref));
}

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
    st = virtio_q_setup(g_pGpu, &g_qCtrl, 0, 64);
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
    struct virtio_gpu_ctrl_hdr *pReq;
    struct virtio_gpu_resp_display_info *pInfo;

    if (!g_fReady) {
        return -1;
    }
    memset(g_aReq, 0, sizeof(g_aReq));
    pReq = (struct virtio_gpu_ctrl_hdr *)(void *)g_aReq;
    pReq->u32Type = VIRTIO_GPU_CMD_GET_DISPLAY_INFO;
    if (gpu_cmd(pReq, sizeof(*pReq)) != 0) {
        return -1;
    }
    pInfo = (struct virtio_gpu_resp_display_info *)(void *)g_aResp;
    if (pInfo->hdr.u32Type != VIRTIO_GPU_RESP_OK_DISPLAY_INFO) {
        /* Some hosts return NODATA — keep defaults */
        if (pOutW) {
            *pOutW = 1024;
        }
        if (pOutH) {
            *pOutH = 768;
        }
        return 0;
    }
    if (pInfo->aPmodes[0].u32Enabled) {
        if (pOutW) {
            *pOutW = pInfo->aPmodes[0].r.u32Width;
        }
        if (pOutH) {
            *pOutH = pInfo->aPmodes[0].r.u32Height;
        }
        kprintf("virtio-gpu: display0 %ux%u enabled=%u\n",
                pInfo->aPmodes[0].r.u32Width, pInfo->aPmodes[0].r.u32Height,
                pInfo->aPmodes[0].u32Enabled);
    } else {
        if (pOutW) {
            *pOutW = 1024;
        }
        if (pOutH) {
            *pOutH = 768;
        }
        kprintf("virtio-gpu: display0 not enabled (default 1024x768)\n");
    }
    return 0;
}

int
virtio_gpu_present(u32 u32Width, u32 u32Height, void *pFb, u32 u32Stride)
{
    struct virtio_gpu_resource_create_2d *pCreate;
    struct virtio_gpu_resource_attach_backing *pAttach;
    struct virtio_gpu_mem_entry *pEnt;
    struct virtio_gpu_set_scanout *pScan;
    struct virtio_gpu_transfer_to_host_2d *pXfer;
    struct virtio_gpu_resource_flush *pFlush;
    gj_paddr_t paFb;
    u32 u32Bytes;
    u32 u32Res;
    int fReuse;

    if (!g_fReady || pFb == NULL || u32Width == 0 || u32Height == 0) {
        return -1;
    }
    if (u32Stride == 0) {
        u32Stride = u32Width * 4u;
    }
    /* Prefer page-table translate (user VA / HHDM); fallback identity. */
    paFb = vmm_virt_to_phys((gj_vaddr_t)pFb);
    if (paFb == 0) {
        paFb = (gj_paddr_t)(gj_vaddr_t)pFb;
    }
    u32Bytes = u32Stride * u32Height;
    fReuse = (g_fHaveRes && g_u32Width == u32Width && g_u32Height == u32Height &&
              g_u32Stride == u32Stride && g_paFb == paFb) ? 1 : 0;

    if (!fReuse) {
        /* Drop prior host resource when geometry or backing changes */
        if (g_fHaveRes) {
            gpu_unref(g_u32ResId);
            g_u32ResId++;
            if (g_u32ResId == 0) {
                g_u32ResId = 1;
            }
            g_fHaveRes = 0;
        }
        u32Res = g_u32ResId;

        /* Create 2D resource */
        memset(g_aReq, 0, sizeof(g_aReq));
        pCreate = (struct virtio_gpu_resource_create_2d *)(void *)g_aReq;
        pCreate->hdr.u32Type = VIRTIO_GPU_CMD_RESOURCE_CREATE_2D;
        pCreate->u32ResourceId = u32Res;
        pCreate->u32Format = VIRTIO_GPU_FORMAT_B8G8R8X8_UNORM;
        pCreate->u32Width = u32Width;
        pCreate->u32Height = u32Height;
        if (gpu_cmd(pCreate, sizeof(*pCreate)) != 0) {
            return -1;
        }

        /* Attach backing (single contiguous guest buffer) */
        memset(g_aReq, 0, sizeof(g_aReq));
        pAttach = (struct virtio_gpu_resource_attach_backing *)(void *)g_aReq;
        pAttach->hdr.u32Type = VIRTIO_GPU_CMD_RESOURCE_ATTACH_BACKING;
        pAttach->u32ResourceId = u32Res;
        pAttach->u32NrEntries = 1;
        pEnt = (struct virtio_gpu_mem_entry *)(void *)(g_aReq + sizeof(*pAttach));
        pEnt->u64Addr = (u64)paFb;
        pEnt->u32Length = u32Bytes;
        if (gpu_cmd(pAttach, sizeof(*pAttach) + sizeof(*pEnt)) != 0) {
            return -1;
        }

        /* Set scanout 0 */
        memset(g_aReq, 0, sizeof(g_aReq));
        pScan = (struct virtio_gpu_set_scanout *)(void *)g_aReq;
        pScan->hdr.u32Type = VIRTIO_GPU_CMD_SET_SCANOUT;
        pScan->r.u32Width = u32Width;
        pScan->r.u32Height = u32Height;
        pScan->u32ScanoutId = 0;
        pScan->u32ResourceId = u32Res;
        if (gpu_cmd(pScan, sizeof(*pScan)) != 0) {
            return -1;
        }
    } else {
        u32Res = g_u32ResId;
    }

    /* Transfer dirty rect to host */
    memset(g_aReq, 0, sizeof(g_aReq));
    pXfer = (struct virtio_gpu_transfer_to_host_2d *)(void *)g_aReq;
    pXfer->hdr.u32Type = VIRTIO_GPU_CMD_TRANSFER_TO_HOST_2D;
    pXfer->r.u32Width = u32Width;
    pXfer->r.u32Height = u32Height;
    pXfer->u64Offset = 0;
    pXfer->u32ResourceId = u32Res;
    if (gpu_cmd(pXfer, sizeof(*pXfer)) != 0) {
        return -1;
    }

    /* Flush */
    memset(g_aReq, 0, sizeof(g_aReq));
    pFlush = (struct virtio_gpu_resource_flush *)(void *)g_aReq;
    pFlush->hdr.u32Type = VIRTIO_GPU_CMD_RESOURCE_FLUSH;
    pFlush->r.u32Width = u32Width;
    pFlush->r.u32Height = u32Height;
    pFlush->u32ResourceId = u32Res;
    if (gpu_cmd(pFlush, sizeof(*pFlush)) != 0) {
        return -1;
    }

    g_u32PresentCount++;
    g_u32Width = u32Width;
    g_u32Height = u32Height;
    g_u32Stride = u32Stride;
    g_paFb = paFb;
    g_u32ResId = u32Res;
    g_fHaveRes = 1;
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
    (void)i;
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
