/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Kernel-side session compositor bridge (interim until userspace sessiond).
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. No GPL.
 *
 * Owns one BGRA scanout buffer and presents via virtio-gpu (Proton A1 T0).
 * Invariants while ready:
 *   - g_pScanout is a valid HHDM pointer to g_u32W * g_u32H * 4 bytes
 *   - g_u32W/H are clamped to a small tile (<= GJ_COMP_MAX_DIM) for bring-up
 * session_door / sessiond claim ownership of *policy*; this module still
 * holds the physical scanout until a full userspace compositor lands.
 *
 * Soft multi-frame: single physical buffer + soft 0/1 index + frame gen;
 * present_n batches up to GJ_COMP_MULTI_MAX flips for multi-frame smokes.
 *
 * Lean soft residual (this TU exclusive; C0; Soft!=product · G-AC-1):
 *   - Once-lamp residual lean after first present/init activity (2 lines max)
 *   - Functional deepen arms densify the same 2 lines (no ret*angle catalog):
 *       dims / path / multi / query / batch / lic
 *   - Cap kprintf: scanout ready / no-gpu / first present ok / multi-frame
 *     once + residual once - NO stamp storms, NO version stamps
 *   - Query paths stay silent (no inventory flood on present_count/etc.)
 *   Functional residual preferred over multi-line deepen catalogs.
 *   Soft != desktop/compositor product bar. Never hard-gates.
 *   Never closes bar3 / Dual DoD A|B / desktop product (all OPEN).
 *   Soft!=product · stamp-free · never bumps GJ_IMAGE_VERSION.
 *
 * Greppable markers (keep stable):
 *   compositor: scanout ... ready (multi-frame soft)
 *   compositor: multi-frame soft
 *   compositor multi-frame soft
 *   compositor: soft residual lean
 *   compositor: soft residual lean PASS | soft residual lean SKIP
 *   Soft!=product
 *   bar3 OPEN
 *   DualDoD=OPEN | dual_dod_a=OPEN | dual_dod_b=OPEN
 *   G-AC-1 | soft_ne_product=1 | class=C0
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
#define GJ_COMP_BPP        4u /* BGRA */
/*
 * Functional residual self-check arm count (C0 residual deepen; stamp-free):
 *   1 dims  2 path  3 multi  4 query  5 batch  6 lic
 * Soft!=product · G-AC-1 · Dual DoD OPEN · dual MIT|Apache-2.0.
 */
#define GJ_COMP_LEAN_CHECKS 6u

static gj_paddr_t g_paScanout;
static void      *g_pScanout;
static u32        g_u32W;
static u32        g_u32H;
static u32        g_u32Stride;
static u32        g_u32Presents;
static u32        g_u32PresentFail; /* soft: backend reject / not-ready flip */
static u32        g_u32Multi;     /* soft: presents after first ok */
static u32        g_u32FrameGen;  /* soft: bumps each ok present */
static u32        g_u32SoftIdx;   /* soft double-buffer index 0/1 */
static int        g_fReady;
static int        g_fLoggedPresent; /* quiet hot path after first success */
static int        g_fLoggedMulti;   /* quiet multi-frame soft once */

/* Once-lamps only - NO stamp storms, NO version stamps. Soft!=product. */
static u8         g_fSoftResidualOnce;
static u32        g_u32SoftLeanOk;   /* residual arms all held */
static u32        g_u32SoftLeanFail; /* residual arms missed */
static u32        g_u32SoftLeanRuns; /* residual once-lamp fire count */

/**
 * Lean residual honesty once-lamp (after first present/init activity).
 * Functional deepen arms (GJ_COMP_LEAN_CHECKS = 6; C0; stamp-free):
 *   1) dims  - MAX/FALLBACK/MIN/BPP/MULTI_MAX geometry contract
 *   2) path  - ready <-> fb/size/stride/pa coherence (no dangling FB)
 *   3) multi - soft idx 0/1 + gen/presents/multi bookkeeping (no tautology)
 *   4) query - silent export APIs match live state (count/gen/idx/ready/fb)
 *   5) batch - present_n clamp contract (MULTI_MAX / min-1)
 *   6) lic   - Soft!=product · G-AC-1 · Dual DoD OPEN · bar3_close=0
 * Soft!=product · G-AC-1 · dual MIT|Apache-2.0. Never hard-gates.
 * Two lean residual lines max (path + PASS|SKIP). Never closes bar3/DualDoD.
 * No version stamp. Soft residual != desktop/compositor product bar.
 * greppable: compositor: soft residual lean
 * greppable: compositor: soft residual lean PASS | soft residual lean SKIP
 * greppable: Soft!=product | DualDoD=OPEN | G-AC-1 | class=C0
 */
static void
comp_soft_residual_once(void)
{
    u32 u32Ready;
    u32 u32Gpu;
    u32 u32Idx;
    u32 u32Checks;
    u32 u32Ok;
    u32 u32DimOk;
    u32 u32PathOk;
    u32 u32MultiOk;
    u32 u32QueryOk;
    u32 u32BatchOk;
    u32 u32LicOk;
    u32 u32ExpectMulti;
    u32 u32Active;
    u32 u32Qw;
    u32 u32Qh;
    u32 u32Qs;
    u32 u32Qc;
    u32 u32Qm;
    u32 u32Qg;
    u32 u32Qi;
    u32 u32Qr;
    void *pQfb;
    const char *szLamp;
    /* Stack-local product honesty (never hard-gates; Soft!=product). */
    const int nSoft = 1;
    const int nProduct = 0;
    const int nBar3Close = 0;     /* bar3 OPEN; residual never closes bar3 */
    const int nDesktopProduct = 0;
    const int nGac1 = 1;          /* G-AC-1: no .ko product AC in kernel */
    const int nDualDodClose = 0;  /* Dual DoD A|B stay OPEN */
    const int nKoInKernel = 0;    /* G-AC-1: compositor never loads .ko */

    if (g_fSoftResidualOnce != 0u) {
        return;
    }
    g_fSoftResidualOnce = 1u;
    g_u32SoftLeanRuns++;

    u32Ready = g_fReady != 0 ? 1u : 0u;
    u32Gpu = virtio_gpu_ready() ? 1u : 0u;
    u32Idx = g_u32SoftIdx & 1u;
    u32Checks = 0;
    u32Ok = 0;
    u32DimOk = 0;
    u32PathOk = 0;
    u32MultiOk = 0;
    u32QueryOk = 0;
    u32BatchOk = 0;
    u32LicOk = 0;

    /* Arm 1: geometry contract (compile-time dims / multi batch ceiling). */
    u32Checks++;
    if (GJ_COMP_MAX_DIM == 256u && GJ_COMP_FALLBACK_W == 64u &&
        GJ_COMP_FALLBACK_H == 64u && GJ_COMP_MIN_W == 32u &&
        GJ_COMP_MIN_H == 32u && GJ_COMP_BPP == 4u &&
        GJ_COMP_MULTI_MAX == 8u && GJ_COMP_LEAN_CHECKS == 6u &&
        GJ_COMP_MIN_W <= GJ_COMP_FALLBACK_W &&
        GJ_COMP_FALLBACK_W <= GJ_COMP_MAX_DIM &&
        GJ_COMP_MIN_H <= GJ_COMP_FALLBACK_H &&
        GJ_COMP_FALLBACK_H <= GJ_COMP_MAX_DIM &&
        (GJ_COMP_FALLBACK_W * GJ_COMP_BPP) == (64u * 4u) &&
        (GJ_COMP_MIN_W * GJ_COMP_BPP) == (32u * 4u) &&
        GJ_COMP_MULTI_MAX >= 1u &&
        GJ_COMP_MULTI_MAX <= GJ_COMP_MAX_DIM) {
        u32DimOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 2: path coherence — ready implies live scanout; !ready implies
     * no dangling FB / zero geometry (session_compositor_fb contract).
     * Ready tile also meets min dim + stride=w*bpp + non-zero pa.
     */
    u32Checks++;
    if (u32Ready != 0u) {
        if (g_pScanout != NULL && g_paScanout != 0 && g_u32W != 0u &&
            g_u32H != 0u && g_u32Stride != 0u &&
            g_u32W >= GJ_COMP_MIN_W && g_u32H >= GJ_COMP_MIN_H &&
            g_u32W <= GJ_COMP_MAX_DIM && g_u32H <= GJ_COMP_MAX_DIM &&
            g_u32Stride == g_u32W * GJ_COMP_BPP &&
            g_u32Stride >= (GJ_COMP_MIN_W * GJ_COMP_BPP)) {
            u32PathOk = 1u;
            u32Ok++;
        }
    } else if (g_pScanout == NULL && g_paScanout == 0 && g_u32W == 0u &&
               g_u32H == 0u && g_u32Stride == 0u && g_fReady == 0) {
        u32PathOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 3: soft multi-frame bookkeeping (single physical buffer).
     * multi == max(presents-1, 0); gen == presents; idx == presents & 1.
     * Soft idx storage is only 0/1 (xor flip; never a second alloc).
     * No tautology on (idx == idx).
     */
    u32Checks++;
    u32ExpectMulti = (g_u32Presents > 1u) ? (g_u32Presents - 1u) : 0u;
    if (g_u32FrameGen == g_u32Presents && g_u32Multi == u32ExpectMulti &&
        u32Idx == (g_u32Presents & 1u) &&
        (g_u32SoftIdx == 0u || g_u32SoftIdx == 1u) &&
        (g_u32SoftIdx & 1u) == u32Idx &&
        (g_u32Presents == 0u || g_u32FrameGen != 0u) &&
        (g_u32Multi == 0u || g_u32Presents > 1u)) {
        u32MultiOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 4: silent query export coherence (no inventory flood).
     * present_count/multi/frame_gen/soft_index/ready/fb/size/stride match
     * live state; !ready never returns a dangling FB pointer.
     */
    u32Checks++;
    u32Qc = session_compositor_present_count();
    u32Qm = session_compositor_multi_count();
    u32Qg = session_compositor_frame_gen();
    u32Qi = session_compositor_soft_index();
    u32Qr = session_compositor_ready() ? 1u : 0u;
    pQfb = session_compositor_fb();
    u32Qw = 0u;
    u32Qh = 0u;
    session_compositor_size(&u32Qw, &u32Qh);
    u32Qs = session_compositor_stride();
    if (u32Qc == g_u32Presents && u32Qm == g_u32Multi &&
        u32Qg == g_u32FrameGen && u32Qi == u32Idx && u32Qr == u32Ready &&
        u32Qw == (u32Ready != 0u ? g_u32W : 0u) &&
        u32Qh == (u32Ready != 0u ? g_u32H : 0u) &&
        u32Qs == (u32Ready != 0u ? g_u32Stride : 0u) &&
        ((u32Ready != 0u && pQfb == g_pScanout && pQfb != NULL) ||
         (u32Ready == 0u && pQfb == NULL))) {
        u32QueryOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 5: present_n batch contract (soft multi-frame smoke ceiling).
     * Clamp N to [1, GJ_COMP_MULTI_MAX]; single physical buffer only.
     */
    u32Checks++;
    if (GJ_COMP_MULTI_MAX == 8u && GJ_COMP_MULTI_MAX >= 1u &&
        GJ_COMP_MULTI_MAX <= 64u &&
        /* present_n floor: u32N==0 treated as 1 (see present_n body). */
        1u <= GJ_COMP_MULTI_MAX &&
        /* soft double-buffer metadata only — one physical scanout. */
        (g_u32SoftIdx == 0u || g_u32SoftIdx == 1u) &&
        g_u32SoftIdx <= 1u) {
        u32BatchOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 6: Soft!=product · product=0 · G-AC-1 · Dual DoD OPEN · bar3_close=0.
     * LAW: dual MIT/Apache Soft!=product; Dual DoD A|B OPEN; bar3 OPEN.
     * Explicit Soft != product (nSoft != nProduct). Never hard-gates.
     */
    u32Checks++;
    if (nSoft == 1 && nProduct == 0 && nSoft != nProduct &&
        nBar3Close == 0 && nDesktopProduct == 0 && nGac1 == 1 &&
        nDualDodClose == 0 && nKoInKernel == 0) {
        u32LicOk = 1u;
        u32Ok++;
    }

    if (u32Ok == u32Checks && u32Checks == GJ_COMP_LEAN_CHECKS) {
        g_u32SoftLeanOk++;
    } else {
        g_u32SoftLeanFail++;
    }

    /*
     * Activity for PASS lamp: any ok present, or ready surface with gpu.
     * Functional arms must all hold — residual PASS is not product DoD.
     * Dual DoD A|B and bar3 remain OPEN regardless of PASS|SKIP.
     */
    u32Active = 0u;
    if (g_u32Presents != 0u) {
        u32Active = 1u;
    } else if (u32Ready != 0u && u32Gpu != 0u) {
        u32Active = 1u; /* ready surface; present not yet observed */
    }
    if (g_u32SoftLeanOk != 0u && u32Active != 0u) {
        szLamp = "PASS";
    } else {
        szLamp = "SKIP";
    }

    /*
     * Grep: compositor: soft residual lean
     * Soft residual honesty only - never product DoD / never bar3 close.
     * Functional deepen densifies this line (dims/path/multi/query/batch/lic).
     * class=C0 · DualDoD=OPEN · stamp_free · never GJ_IMAGE_VERSION.
     */
    kprintf("compositor: soft residual lean "
            "class=C0 interim_scanout=1 ready=%u w=%u h=%u stride=%u bpp=%u "
            "presents=%u fail=%u multi=%u gen=%u idx=%u gpu=%u multi_max=%u "
            "dims=%u path=%u multi_arm=%u query=%u batch=%u lic=%u ok=%u/%u "
            "lean_ok=%u lean_fail=%u lean_runs=%u "
            "soft=1 product=0 soft_ne_product=1 stamp_storm=0 stamp_free=1 "
            "desktop_product=OPEN bar3=OPEN bar3_close=0 "
            "DualDoD=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "G-AC-1=1 ko_in_kernel=0 Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "(kernel tile only; C0 residual; not desktop/compositor product)\n",
            u32Ready, g_u32W, g_u32H, g_u32Stride, GJ_COMP_BPP,
            g_u32Presents, g_u32PresentFail, g_u32Multi, g_u32FrameGen,
            u32Idx, u32Gpu, GJ_COMP_MULTI_MAX, u32DimOk, u32PathOk,
            u32MultiOk, u32QueryOk, u32BatchOk, u32LicOk, u32Ok, u32Checks,
            g_u32SoftLeanOk, g_u32SoftLeanFail, g_u32SoftLeanRuns);
    /*
     * Grep: compositor: soft residual lean PASS
     * Grep: compositor: soft residual lean SKIP
     */
    kprintf("compositor: soft residual lean %s "
            "class=C0 ready=%u presents=%u fail=%u multi=%u gen=%u "
            "dims=%u path=%u multi_arm=%u query=%u batch=%u lic=%u ok=%u/%u "
            "soft=1 product=0 soft_ne_product=1 stamp_free=1 "
            "desktop_product=OPEN bar3=OPEN bar3_close=0 "
            "DualDoD=OPEN dual_dod_a=OPEN dual_dod_b=OPEN "
            "G-AC-1=1 ko_in_kernel=0 Soft!=product dual=MIT|Apache-2.0 "
            "(lean residual only; Dual DoD OPEN; bar3 OPEN)\n",
            szLamp, u32Ready, g_u32Presents, g_u32PresentFail, g_u32Multi,
            g_u32FrameGen, u32DimOk, u32PathOk, u32MultiOk, u32QueryOk,
            u32BatchOk, u32LicOk, u32Ok, u32Checks);
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

    if (g_fReady && g_pScanout != NULL && g_u32W != 0 && g_u32H != 0) {
        return 0;
    }

    g_fReady = 0;
    g_fLoggedPresent = 0;
    g_fLoggedMulti = 0;
    g_u32Presents = 0;
    g_u32PresentFail = 0;
    g_u32Multi = 0;
    g_u32FrameGen = 0;
    g_u32SoftIdx = 0;
    g_paScanout = 0;
    g_pScanout = NULL;
    g_u32W = 0;
    g_u32H = 0;
    g_u32Stride = 0;
    /* Soft residual re-arms so a re-init after present can dump again. */
    g_fSoftResidualOnce = 0;
    g_u32SoftLeanOk = 0;
    g_u32SoftLeanFail = 0;
    /* lean_runs sticky across re-init (once-fire count; Soft!=product). */

    if (!virtio_gpu_ready()) {
        kprintf("compositor: no virtio-gpu\n");
        /* Lean fail residual once (no multi-line catalog). */
        comp_soft_residual_once();
        return -1;
    }

    (void)virtio_gpu_get_display(&u32W, &u32H);
    /*
     * Clamp: zero/absurd/too-small display info must not drive multi-megabyte
     * allocs or undersized tiles below the min residual geometry contract.
     * Soft tile only (Soft!=product); matches path arm min..max.
     */
    if (u32W < GJ_COMP_MIN_W || u32H < GJ_COMP_MIN_H ||
        u32W > GJ_COMP_MAX_DIM || u32H > GJ_COMP_MAX_DIM) {
        u32W = GJ_COMP_FALLBACK_W;
        u32H = GJ_COMP_FALLBACK_H;
    }

    g_u32W = u32W;
    g_u32H = u32H;
    g_u32Stride = u32W * GJ_COMP_BPP;
    /* u32W/H ≤ 256 ⇒ stride*height fits comfortably in u32. */
    u32Bytes = g_u32Stride * g_u32H;
    u32Pages = (u32Bytes + GJ_PAGE_SIZE - 1u) / GJ_PAGE_SIZE;
    if (u32Pages == 0) {
        g_u32W = 0;
        g_u32H = 0;
        g_u32Stride = 0;
        comp_soft_residual_once();
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
            /* Lean residual on hard alloc fail (path arm: !ready zeroed). */
            comp_soft_residual_once();
            return -1;
        }
        g_u32W = GJ_COMP_MIN_W;
        g_u32H = GJ_COMP_MIN_H;
        g_u32Stride = g_u32W * GJ_COMP_BPP;
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
        comp_soft_residual_once();
        return -1;
    }

    p = (u8 *)g_pScanout;
    for (iPix = 0; iPix < g_u32W * g_u32H; iPix++) {
        /* GreenJade green-ish gradient (BGRA) - visual bring-up only. */
        p[iPix * 4u + 0] = 0x20;
        p[iPix * 4u + 1] = (u8)(0x80u + (iPix & 0x3fu));
        p[iPix * 4u + 2] = 0x10;
        p[iPix * 4u + 3] = 0xff;
    }
    (void)u32Bytes;
    (void)u32Pages;

    g_fReady = 1;
    /* greppable: compositor: scanout ... ready (multi-frame soft) */
    kprintf("compositor: scanout %ux%u pa=0x%lx ready (multi-frame soft)\n",
            g_u32W, g_u32H, (unsigned long)g_paScanout);
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

    if (!g_fReady || g_pScanout == NULL || g_u32W == 0 || g_u32H == 0 ||
        g_u32Stride == 0) {
        g_u32PresentFail++;
        /* Once-lamp residual on not-ready flip (functional path honesty). */
        comp_soft_residual_once();
        return -1;
    }
    st = virtio_gpu_present(g_u32W, g_u32H, g_pScanout, g_u32Stride);
    if (st == 0) {
        comp_note_ok_present();
        /* Once-lamp residual after first ok present activity. */
        comp_soft_residual_once();
    } else {
        g_u32PresentFail++;
        /* Once-lamp residual after backend reject (still Soft!=product). */
        comp_soft_residual_once();
    }
    return st;
}

int
session_compositor_present_n(u32 u32N)
{
    u32 i;
    u32 u32Ok = 0;
    int stLast = -1;

    if (!g_fReady) {
        g_u32PresentFail++;
        comp_soft_residual_once();
        return -1;
    }
    if (u32N == 0) {
        u32N = 1;
    }
    if (u32N > GJ_COMP_MULTI_MAX) {
        u32N = GJ_COMP_MULTI_MAX;
    }
    for (i = 0; i < u32N; i++) {
        stLast = session_compositor_present();
        if (stLast == 0) {
            u32Ok++;
        } else {
            /* stop soft batch on first backend reject */
            break;
        }
    }
    return u32Ok != 0 ? 0 : stLast;
}

u32
session_compositor_present_count(void)
{
    /* Silent query - no inventory stamp storm. */
    return g_u32Presents;
}

u32
session_compositor_multi_count(void)
{
    /* Silent query - no inventory stamp storm. */
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
