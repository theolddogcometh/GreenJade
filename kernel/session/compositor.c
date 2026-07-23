/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Kernel-side session compositor bridge (interim until userspace sessiond).
 *
 * Owns one BGRA scanout buffer and presents via virtio-gpu (Proton A1 T0).
 * Invariants while ready:
 *   - g_pScanout is a valid HHDM pointer to g_u32W * g_u32H * 4 bytes
 *   - g_u32W/H are clamped to a small tile (≤ GJ_COMP_MAX_DIM) for bring-up
 * session_door / sessiond claim ownership of *policy*; this module still
 * holds the physical scanout until a full userspace compositor lands.
 */
#include <gj/compositor.h>
#include <gj/config.h>
#include <gj/klog.h>
#include <gj/pmm.h>
#include <gj/string.h>
#include <gj/virtio_gpu.h>
#include <gj/vmm.h>

/* Soft upper bound for interim scanout (matches session door PRESENT_FB). */
#define GJ_COMP_MAX_DIM    256u
#define GJ_COMP_FALLBACK_W 64u
#define GJ_COMP_FALLBACK_H 64u
#define GJ_COMP_MIN_W      32u
#define GJ_COMP_MIN_H      32u

static gj_paddr_t g_paScanout;
static void      *g_pScanout;
static u32        g_u32W;
static u32        g_u32H;
static u32        g_u32Stride;
static u32        g_u32Presents;
static int        g_fReady;
static int        g_fLoggedPresent; /* quiet hot path after first success */

/**
 * Allocate scanout, fill a jade-ish gradient, mark ready.
 * Idempotent when already ready. Returns 0 on success, -1 if virtio-gpu
 * or PMM cannot back a tile.
 */
int
session_compositor_init(void)
{
    u32 u32W = GJ_COMP_FALLBACK_W;
    u32 u32H = GJ_COMP_FALLBACK_H;
    u32 u32Pages;
    u32 u32Bytes;
    u32 iPix;
    u8 *p;

    if (g_fReady && g_pScanout != NULL && g_u32W != 0 && g_u32H != 0) {
        return 0;
    }

    g_fReady = 0;
    g_fLoggedPresent = 0;
    g_u32Presents = 0;
    g_paScanout = 0;
    g_pScanout = NULL;
    g_u32W = 0;
    g_u32H = 0;
    g_u32Stride = 0;

    if (!virtio_gpu_ready()) {
        kprintf("compositor: no virtio-gpu\n");
        return -1;
    }

    (void)virtio_gpu_get_display(&u32W, &u32H);
    /* Clamp: zero/absurd display info must not drive multi-megabyte allocs. */
    if (u32W == 0 || u32H == 0 || u32W > GJ_COMP_MAX_DIM ||
        u32H > GJ_COMP_MAX_DIM) {
        u32W = GJ_COMP_FALLBACK_W;
        u32H = GJ_COMP_FALLBACK_H;
    }

    g_u32W = u32W;
    g_u32H = u32H;
    g_u32Stride = u32W * 4u;
    /* u32W/H ≤ 256 ⇒ stride*height fits comfortably in u32. */
    u32Bytes = g_u32Stride * g_u32H;
    u32Pages = (u32Bytes + GJ_PAGE_SIZE - 1u) / GJ_PAGE_SIZE;
    if (u32Pages == 0) {
        return -1;
    }

    g_paScanout = pmm_alloc_pages(u32Pages);
    if (g_paScanout == 0) {
        /* Last-ditch single page: shrink tile so one frame still presents. */
        g_paScanout = pmm_alloc();
        if (g_paScanout == 0) {
            g_u32W = 0;
            g_u32H = 0;
            g_u32Stride = 0;
            return -1;
        }
        g_u32W = GJ_COMP_MIN_W;
        g_u32H = GJ_COMP_MIN_H;
        g_u32Stride = g_u32W * 4u;
        u32Pages = 1;
        u32Bytes = g_u32Stride * g_u32H;
    }

    g_pScanout = (void *)hhdm_to_virt(g_paScanout);
    if (g_pScanout == NULL) {
        /* HHDM must cover PMM frames; treat as hard failure. */
        g_paScanout = 0;
        g_u32W = 0;
        g_u32H = 0;
        g_u32Stride = 0;
        return -1;
    }

    p = (u8 *)g_pScanout;
    for (iPix = 0; iPix < g_u32W * g_u32H; iPix++) {
        /* GreenJade green-ish gradient (BGRA) — visual bring-up only. */
        p[iPix * 4u + 0] = 0x20;
        p[iPix * 4u + 1] = (u8)(0x80u + (iPix & 0x3fu));
        p[iPix * 4u + 2] = 0x10;
        p[iPix * 4u + 3] = 0xff;
    }
    (void)u32Bytes;
    (void)u32Pages;

    g_fReady = 1;
    kprintf("compositor: scanout %ux%u pa=0x%lx ready\n", g_u32W, g_u32H,
            (unsigned long)g_paScanout);
    return 0;
}

/**
 * Flip current scanout to virtio-gpu. Returns 0 on success, -1 if not ready
 * or the backend rejects the frame. Hot path is quiet after the first ok.
 */
int
session_compositor_present(void)
{
    int st;

    if (!g_fReady || g_pScanout == NULL || g_u32W == 0 || g_u32H == 0 ||
        g_u32Stride == 0) {
        return -1;
    }
    st = virtio_gpu_present(g_u32W, g_u32H, g_pScanout, g_u32Stride);
    if (st == 0) {
        g_u32Presents++;
        if (!g_fLoggedPresent) {
            g_fLoggedPresent = 1;
            kprintf("compositor: present ok (quiet after first)\n");
        }
    }
    return st;
}

u32
session_compositor_present_count(void)
{
    return g_u32Presents;
}

int
session_compositor_ready(void)
{
    return g_fReady != 0;
}

/** Scanout VA, or NULL if not ready (never a dangling interim pointer). */
void *
session_compositor_fb(void)
{
    if (!g_fReady) {
        return NULL;
    }
    return g_pScanout;
}

/**
 * Write current width/height (0,0 if not ready). Null out-params are ignored.
 */
void
session_compositor_size(u32 *pW, u32 *pH)
{
    u32 u32W = g_fReady ? g_u32W : 0u;
    u32 u32H = g_fReady ? g_u32H : 0u;

    if (pW != NULL) {
        *pW = u32W;
    }
    if (pH != NULL) {
        *pH = u32H;
    }
}

u32
session_compositor_stride(void)
{
    if (!g_fReady) {
        return 0;
    }
    return g_u32Stride;
}
