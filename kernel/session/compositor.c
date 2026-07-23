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
 *
 * Soft multi-frame: single physical buffer + soft 0/1 index + frame gen;
 * present_n batches up to GJ_COMP_MULTI_MAX flips for multi-frame smokes.
 *
 * Soft product inventory (Wave 10 exclusive deepen):
 *   - Init path: enter / ok / fail_gpu / fail_pmm / fail_hhdm / shrink
 *   - Present path: enter / ok / fail_ready / fail_backend + live multi/gen/idx
 *   - Batch path: enter / ok / partial / frames_ok / stop_rej
 *   greppable: "compositor: soft …"
 *   Never hard-gates; diagnostics only (wrap OK).
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
static u32        g_u32Multi;     /* soft: presents after first ok */
static u32        g_u32FrameGen;  /* soft: bumps each ok present */
static u32        g_u32SoftIdx;   /* soft double-buffer index 0/1 */
static int        g_fReady;
static int        g_fLoggedPresent; /* quiet hot path after first success */
static int        g_fLoggedMulti;   /* quiet multi-frame soft once */

/*
 * Soft product inventory (Wave 10). Cumulative unless noted live.
 * greppable: compositor: soft …
 */
static u32 g_u32SoftInitEnter;     /* session_compositor_init entries */
static u32 g_u32SoftInitOk;        /* ready after init */
static u32 g_u32SoftInitIdem;      /* already-ready early return */
static u32 g_u32SoftInitFailGpu;   /* !virtio_gpu_ready */
static u32 g_u32SoftInitFailPmm;   /* PMM could not back tile */
static u32 g_u32SoftInitFailHhdm;  /* HHDM map missing */
static u32 g_u32SoftInitShrink;    /* single-page min-tile fallback */
static u32 g_u32SoftPresentEnter;  /* present entries */
static u32 g_u32SoftPresentOk;     /* backend accepted frame */
static u32 g_u32SoftPresentFailRdy;/* not ready / zero geom */
static u32 g_u32SoftPresentFailBe; /* virtio-gpu reject */
static u32 g_u32SoftBatchEnter;    /* present_n entries */
static u32 g_u32SoftBatchOk;       /* at least one frame in batch ok */
static u32 g_u32SoftBatchPartial;  /* batch stopped mid-way after ≥1 ok */
static u32 g_u32SoftBatchFail;     /* zero frames ok (not ready / reject) */
static u32 g_u32SoftBatchFramesOk; /* sum of ok frames across batches */
static u32 g_u32SoftBatchStopRej;  /* batches stopped on backend reject */
static u32 g_u32SoftLogN;          /* inventory log emissions */
static u8  g_fSoftOnce;            /* one-shot after first present activity */
static u8  g_fSoftBatchInFlight;   /* suppress mid-batch maybe_once */

static void comp_soft_inc(u32 *pCtr);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
comp_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    (*pCtr)++;
}

/**
 * Greppable soft compositor inventory (product / smoke).
 *   compositor: soft inventory ready=… w=… h=… stride=… multi_max=… logs=…
 *   compositor: soft init enter=… ok=… idem=… fail_gpu=… fail_pmm=…
 *        fail_hhdm=… shrink=…
 *   compositor: soft present enter=… ok=… fail_ready=… fail_backend=…
 *        multi=… gen=… idx=… presents=…
 *   compositor: soft batch enter=… ok=… partial=… fail=… frames_ok=…
 *        stop_rej=…
 * greppable: compositor: soft
 */
static void
soft_inventory_log(void)
{
    u32 u32Ready;
    u32 u32W;
    u32 u32H;
    u32 u32Stride;
    u32 u32Multi;
    u32 u32Gen;
    u32 u32Idx;
    u32 u32Presents;

    comp_soft_inc(&g_u32SoftLogN);

    /* Snapshot live surface (diagnostics only; no hard lock). */
    u32Ready = g_fReady != 0 ? 1u : 0u;
    u32W = g_u32W;
    u32H = g_u32H;
    u32Stride = g_u32Stride;
    u32Multi = g_u32Multi;
    u32Gen = g_u32FrameGen;
    u32Idx = g_u32SoftIdx & 1u;
    u32Presents = g_u32Presents;

    /* Grep: compositor: soft inventory */
    kprintf("compositor: soft inventory ready=%u w=%u h=%u stride=%u "
            "multi_max=%u logs=%u\n",
            u32Ready, u32W, u32H, u32Stride, GJ_COMP_MULTI_MAX,
            g_u32SoftLogN);

    /* Grep: compositor: soft init */
    kprintf("compositor: soft init enter=%u ok=%u idem=%u fail_gpu=%u "
            "fail_pmm=%u fail_hhdm=%u shrink=%u\n",
            g_u32SoftInitEnter, g_u32SoftInitOk, g_u32SoftInitIdem,
            g_u32SoftInitFailGpu, g_u32SoftInitFailPmm,
            g_u32SoftInitFailHhdm, g_u32SoftInitShrink);

    /* Grep: compositor: soft present */
    kprintf("compositor: soft present enter=%u ok=%u fail_ready=%u "
            "fail_backend=%u multi=%u gen=%u idx=%u presents=%u\n",
            g_u32SoftPresentEnter, g_u32SoftPresentOk,
            g_u32SoftPresentFailRdy, g_u32SoftPresentFailBe, u32Multi,
            u32Gen, u32Idx, u32Presents);

    /* Grep: compositor: soft batch */
    kprintf("compositor: soft batch enter=%u ok=%u partial=%u fail=%u "
            "frames_ok=%u stop_rej=%u\n",
            g_u32SoftBatchEnter, g_u32SoftBatchOk, g_u32SoftBatchPartial,
            g_u32SoftBatchFail, g_u32SoftBatchFramesOk,
            g_u32SoftBatchStopRej);
}

/**
 * After first product present activity, print soft inventory once
 * (mirrors door/memobj soft-stats-once). Diagnostics only.
 */
static void
soft_inventory_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32SoftPresentEnter == 0 && g_u32SoftBatchEnter == 0) {
        return;
    }
    g_fSoftOnce = 1;
    soft_inventory_log();
}

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

    comp_soft_inc(&g_u32SoftInitEnter);

    if (g_fReady && g_pScanout != NULL && g_u32W != 0 && g_u32H != 0) {
        comp_soft_inc(&g_u32SoftInitIdem);
        return 0;
    }

    g_fReady = 0;
    g_fLoggedPresent = 0;
    g_fLoggedMulti = 0;
    g_u32Presents = 0;
    g_u32Multi = 0;
    g_u32FrameGen = 0;
    g_u32SoftIdx = 0;
    g_paScanout = 0;
    g_pScanout = NULL;
    g_u32W = 0;
    g_u32H = 0;
    g_u32Stride = 0;
    /* Soft once re-arms so a re-init after present can dump again. */
    g_fSoftOnce = 0;
    g_fSoftBatchInFlight = 0;

    if (!virtio_gpu_ready()) {
        comp_soft_inc(&g_u32SoftInitFailGpu);
        kprintf("compositor: no virtio-gpu\n");
        /* Grep: compositor: soft (fail baseline) */
        soft_inventory_log();
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
        comp_soft_inc(&g_u32SoftInitFailPmm);
        soft_inventory_log();
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
            comp_soft_inc(&g_u32SoftInitFailPmm);
            soft_inventory_log();
            return -1;
        }
        comp_soft_inc(&g_u32SoftInitShrink);
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
        comp_soft_inc(&g_u32SoftInitFailHhdm);
        soft_inventory_log();
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
    comp_soft_inc(&g_u32SoftInitOk);
    kprintf("compositor: scanout %ux%u pa=0x%lx ready (multi-frame soft)\n",
            g_u32W, g_u32H, (unsigned long)g_paScanout);
    /* Grep: compositor: soft (baseline inventory after ready) */
    soft_inventory_log();
    return 0;
}

/**
 * Bookkeep a successful flip: present count, frame gen, soft index, multi.
 */
static void
comp_note_ok_present(void)
{
    g_u32Presents++;
    g_u32FrameGen++;
    g_u32SoftIdx ^= 1u;
    if (g_u32Presents > 1u) {
        g_u32Multi++;
        if (!g_fLoggedMulti) {
            g_fLoggedMulti = 1;
            /* greppable: compositor: multi-frame soft */
            kprintf("compositor: multi-frame soft gen=%u idx=%u\n",
                    g_u32FrameGen, g_u32SoftIdx);
        }
    }
    if (!g_fLoggedPresent) {
        g_fLoggedPresent = 1;
        kprintf("compositor: present ok (quiet after first)\n");
    }
}

/**
 * Flip current scanout to virtio-gpu. Returns 0 on success, -1 if not ready
 * or the backend rejects the frame. Hot path is quiet after the first ok.
 */
int
session_compositor_present(void)
{
    int st;

    comp_soft_inc(&g_u32SoftPresentEnter);

    if (!g_fReady || g_pScanout == NULL || g_u32W == 0 || g_u32H == 0 ||
        g_u32Stride == 0) {
        comp_soft_inc(&g_u32SoftPresentFailRdy);
        if (g_fSoftBatchInFlight == 0) {
            soft_inventory_maybe_once();
        }
        return -1;
    }
    st = virtio_gpu_present(g_u32W, g_u32H, g_pScanout, g_u32Stride);
    if (st == 0) {
        comp_soft_inc(&g_u32SoftPresentOk);
        comp_note_ok_present();
    } else {
        comp_soft_inc(&g_u32SoftPresentFailBe);
    }
    /* Batch path dumps once after batch counters update. */
    if (g_fSoftBatchInFlight == 0) {
        soft_inventory_maybe_once();
    }
    return st;
}

int
session_compositor_present_n(u32 u32N)
{
    u32 i;
    u32 u32Ok = 0;
    int stLast = -1;

    comp_soft_inc(&g_u32SoftBatchEnter);

    if (!g_fReady) {
        comp_soft_inc(&g_u32SoftBatchFail);
        soft_inventory_maybe_once();
        return -1;
    }
    if (u32N == 0) {
        u32N = 1;
    }
    if (u32N > GJ_COMP_MULTI_MAX) {
        u32N = GJ_COMP_MULTI_MAX;
    }
    g_fSoftBatchInFlight = 1;
    for (i = 0; i < u32N; i++) {
        stLast = session_compositor_present();
        if (stLast == 0) {
            u32Ok++;
        } else {
            /* stop soft batch on first backend reject */
            if (u32Ok > 0u) {
                comp_soft_inc(&g_u32SoftBatchStopRej);
            }
            break;
        }
    }
    g_fSoftBatchInFlight = 0;
    if (u32Ok != 0u) {
        g_u32SoftBatchFramesOk += u32Ok;
        comp_soft_inc(&g_u32SoftBatchOk);
        if (u32Ok < u32N) {
            comp_soft_inc(&g_u32SoftBatchPartial);
        }
    } else {
        comp_soft_inc(&g_u32SoftBatchFail);
    }
    soft_inventory_maybe_once();
    return u32Ok != 0 ? 0 : stLast;
}

u32
session_compositor_present_count(void)
{
    return g_u32Presents;
}

u32
session_compositor_multi_count(void)
{
    return g_u32Multi;
}

u32
session_compositor_frame_gen(void)
{
    return g_u32FrameGen;
}

u32
session_compositor_soft_index(void)
{
    return g_u32SoftIdx & 1u;
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
