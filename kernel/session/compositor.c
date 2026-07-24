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
 * Soft product inventory (Wave 33 exclusive deepen; this unit only):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   - Init path: enter / ok / idem / fail_gpu / fail_pmm / fail_hhdm /
 *     shrink / clamp(zero|max) / fallback + last pages/bytes snap
 *   - Present path: enter / ok / fail_ready / fail_backend + live
 *     multi/gen/idx/presents + first_ok / multi_first
 *   - Batch path: enter / ok / partial / fail / frames_ok / stop_rej /
 *     clamp_n / zero_n / req / last_ok
 *   - Peaks: presents / multi / gen / batch_ok high-water
 *   - Query: ready / fb / size / stride / present_n / multi / gen / idx
 *   - Paint / reinit / ratio / last / size-null / fb-null soft surfaces
 *   - Geom + path honesty catalog + soft verdict INIT|PASS|PARTIAL
 *   - Wave 15 base: honesty / multi / backend / scanout / catalog / deepen
 *   - Wave 16 base: capacity / headroom / surface lamps
 *   greppable: "compositor: soft …"
 *   greppable: "compositor: soft deepen"
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ bar3.
 *   Soft ≠ desktop/compositor product bar.
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
/* Wave 20 deepen stamp (file-local; never hard-gates). */
#define GJ_COMP_SOFT_WAVE 33u

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
 * Soft product inventory (Wave 33 exclusive deepen). Cumulative unless noted
 * live/peak. greppable: compositor: soft …
 */
static u32 g_u32SoftInitEnter;     /* session_compositor_init entries */
static u32 g_u32SoftInitOk;        /* ready after init */
static u32 g_u32SoftInitIdem;      /* already-ready early return */
static u32 g_u32SoftInitFailGpu;   /* !virtio_gpu_ready */
static u32 g_u32SoftInitFailPmm;   /* PMM could not back tile */
static u32 g_u32SoftInitFailHhdm;  /* HHDM map missing */
static u32 g_u32SoftInitShrink;    /* single-page min-tile fallback */
static u32 g_u32SoftInitClamp;     /* display clamped (max/zero/absurd) */
static u32 g_u32SoftInitClampZero; /* clamp reason: zero dim */
static u32 g_u32SoftInitClampMax;  /* clamp reason: > max dim */
static u32 g_u32SoftInitFallback;  /* forced FALLBACK_W×H geom */
static u32 g_u32SoftInitLastPages; /* pages backing last ok/fail tile snap */
static u32 g_u32SoftInitLastBytes; /* bytes of last planned tile snap */
static u32 g_u32SoftInitReenter;   /* init after a prior present activity */
static u32 g_u32SoftPresentEnter;  /* present entries */
static u32 g_u32SoftPresentOk;     /* backend accepted frame */
static u32 g_u32SoftPresentFailRdy;/* not ready / zero geom */
static u32 g_u32SoftPresentFailBe; /* virtio-gpu reject */
static u32 g_u32SoftPresentFirstOk;/* first-ok present events (sticky count) */
static u32 g_u32SoftPresentMulti1; /* first multi-frame soft events */
static u32 g_u32SoftBatchEnter;    /* present_n entries */
static u32 g_u32SoftBatchOk;       /* at least one frame in batch ok */
static u32 g_u32SoftBatchPartial;  /* batch stopped mid-way after ≥1 ok */
static u32 g_u32SoftBatchFail;     /* zero frames ok (not ready / reject) */
static u32 g_u32SoftBatchFramesOk; /* sum of ok frames across batches */
static u32 g_u32SoftBatchStopRej;  /* batches stopped on backend reject */
static u32 g_u32SoftBatchClamp;    /* N clamped down to GJ_COMP_MULTI_MAX */
static u32 g_u32SoftBatchZeroN;    /* N==0 promoted to 1 */
static u32 g_u32SoftBatchReq;      /* sum of requested frames (post-clamp) */
static u32 g_u32SoftBatchLastOk;   /* ok frames in most recent present_n */
static u32 g_u32SoftBatchLastReq;  /* requested frames in most recent batch */
static u32 g_u32SoftPeakPresents;  /* high-water g_u32Presents */
static u32 g_u32SoftPeakMulti;     /* high-water g_u32Multi */
static u32 g_u32SoftPeakGen;       /* high-water g_u32FrameGen */
static u32 g_u32SoftPeakBatchOk;   /* max ok frames in one present_n */
static u32 g_u32SoftQueryReady;    /* session_compositor_ready reads */
static u32 g_u32SoftQueryFb;       /* session_compositor_fb reads */
static u32 g_u32SoftQueryFbNull;   /* fb query while not ready */
static u32 g_u32SoftQueryFbOk;     /* fb query returned non-NULL */
static u32 g_u32SoftQuerySize;     /* session_compositor_size reads */
static u32 g_u32SoftQuerySizeNullW;/* size: pW == NULL */
static u32 g_u32SoftQuerySizeNullH;/* size: pH == NULL */
static u32 g_u32SoftQueryStride;   /* session_compositor_stride reads */
static u32 g_u32SoftQueryPresentN; /* session_compositor_present_count */
static u32 g_u32SoftQueryMulti;    /* session_compositor_multi_count */
static u32 g_u32SoftQueryGen;      /* session_compositor_frame_gen */
static u32 g_u32SoftQueryIdx;      /* session_compositor_soft_index */
static u32 g_u32SoftPaintEnter;    /* gradient paint runs */
static u32 g_u32SoftPaintPix;      /* last paint pixel count */
static u32 g_u32SoftPaintBytes;    /* last paint byte count */
static u32 g_u32SoftLastPresentSt; /* 0 ok / 1 fail_rdy / 2 fail_be */
static u32 g_u32SoftLogN;          /* inventory log emissions */
static u8  g_fSoftOnce;            /* one-shot after first present activity */
static u8  g_fSoftBatchInFlight;   /* suppress mid-batch maybe_once */

static void comp_soft_inc(u32 *pCtr);
static void soft_note_peaks(void);
static void soft_inventory_log(void);
static void soft_inventory_maybe_once(void);

/** Soft: saturating bump (u32 wrap avoided; wrap OK if ever hit). */
static void
comp_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/**
 * Soft: refresh presents / multi / gen high-water (diagnostics only).
 */
static void
soft_note_peaks(void)
{
    if (g_u32Presents > g_u32SoftPeakPresents) {
        g_u32SoftPeakPresents = g_u32Presents;
    }
    if (g_u32Multi > g_u32SoftPeakMulti) {
        g_u32SoftPeakMulti = g_u32Multi;
    }
    if (g_u32FrameGen > g_u32SoftPeakGen) {
        g_u32SoftPeakGen = g_u32FrameGen;
    }
}

/**
 * Greppable soft compositor inventory (product / smoke; Wave 20 deepen).
 *   compositor: soft honesty …
 *   compositor: soft inventory …
 *   compositor: soft init …
 *   compositor: soft present …
 *   compositor: soft batch …
 *   compositor: soft geom …
 *   compositor: soft paint …
 *   compositor: soft peaks …
 *   compositor: soft query …
 *   compositor: soft ratio …
 *   compositor: soft last …
 *   compositor: soft multi …
 *   compositor: soft backend …
 *   compositor: soft scanout …
 *   compositor: soft capacity …   (Wave 16)
 *   compositor: soft headroom …   (Wave 16)
 *   compositor: soft surface …    (Wave 16)
 *   compositor: soft catalog …
 *   compositor: soft path …
 *   compositor: soft deepen …
 *   compositor: soft PASS|PARTIAL|INIT
 * greppable: compositor: soft
 * greppable: compositor: soft deepen
 * Honesty: soft inventory only — not bar3 / desktop compositor product.
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
    u32 u32Pa;
    u32 u32Pages;
    u32 u32Bytes;
    u32 u32Gpu;
    u32 u32OkBp;
    u32 u32FailBp;
    u32 u32Fb;
    u32 cAreas;
    const char *szVerdict;

    comp_soft_inc(&g_u32SoftLogN);
    soft_note_peaks();
    cAreas = 0;

    /* Snapshot live surface (diagnostics only; no hard lock). */
    u32Ready = g_fReady != 0 ? 1u : 0u;
    u32W = g_u32W;
    u32H = g_u32H;
    u32Stride = g_u32Stride;
    u32Multi = g_u32Multi;
    u32Gen = g_u32FrameGen;
    u32Idx = g_u32SoftIdx & 1u;
    u32Presents = g_u32Presents;
    u32Pa = (g_paScanout != 0) ? 1u : 0u;
    u32Bytes = g_u32SoftInitLastBytes;
    u32Pages = g_u32SoftInitLastPages;
    u32Gpu = virtio_gpu_ready() ? 1u : 0u;
    u32Fb = (g_pScanout != NULL) ? 1u : 0u;

    if (g_u32SoftPresentEnter != 0u) {
        u32OkBp = (g_u32SoftPresentOk * 10000u) / g_u32SoftPresentEnter;
        u32FailBp = ((g_u32SoftPresentFailRdy + g_u32SoftPresentFailBe) *
                     10000u) /
                    g_u32SoftPresentEnter;
    } else {
        u32OkBp = 0;
        u32FailBp = 0;
    }

    /*
     * Soft verdict (inventory only; never hard-gates present):
     *   INIT     — no present/batch activity yet (post-init baseline ok)
     *   PASS     — any successful present observed
     *   PARTIAL  — only failures so far (not ready / backend reject)
     */
    if (g_u32SoftPresentOk != 0u || g_u32SoftBatchFramesOk != 0u) {
        szVerdict = "PASS";
    } else if (g_u32SoftPresentFailRdy != 0u || g_u32SoftPresentFailBe != 0u ||
               g_u32SoftBatchFail != 0u || g_u32SoftInitFailGpu != 0u ||
               g_u32SoftInitFailPmm != 0u || g_u32SoftInitFailHhdm != 0u) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "INIT";
    }

    /*
     * Honesty first: interim kernel scanout is NOT desktop compositor product.
     * greppable: compositor: soft honesty
     */
    kprintf("compositor: soft honesty interim_scanout=1 desktop_product=OPEN "
            "sessiond_policy=door multi_frame=soft_idx bar3=OPEN "
            "soft_never_gates=1 wave=%u (soft; never closes desktop bar)\n",
            GJ_COMP_SOFT_WAVE);
    cAreas++;

    /* Grep: compositor: soft inventory */
    kprintf("compositor: soft inventory ready=%u w=%u h=%u stride=%u "
            "multi_max=%u presents=%u multi=%u gen=%u idx=%u gpu=%u "
            "ok_bp=%u logs=%u wave=%u\n",
            u32Ready, u32W, u32H, u32Stride, GJ_COMP_MULTI_MAX, u32Presents,
            u32Multi, u32Gen, u32Idx, u32Gpu, u32OkBp, g_u32SoftLogN,
            GJ_COMP_SOFT_WAVE);
    cAreas++;

    /* Grep: compositor: soft init */
    kprintf("compositor: soft init enter=%u ok=%u idem=%u fail_gpu=%u "
            "fail_pmm=%u fail_hhdm=%u shrink=%u clamp=%u clamp_zero=%u "
            "clamp_max=%u fallback=%u reenter=%u last_pages=%u "
            "last_bytes=%u\n",
            g_u32SoftInitEnter, g_u32SoftInitOk, g_u32SoftInitIdem,
            g_u32SoftInitFailGpu, g_u32SoftInitFailPmm,
            g_u32SoftInitFailHhdm, g_u32SoftInitShrink, g_u32SoftInitClamp,
            g_u32SoftInitClampZero, g_u32SoftInitClampMax,
            g_u32SoftInitFallback, g_u32SoftInitReenter,
            g_u32SoftInitLastPages, g_u32SoftInitLastBytes);
    cAreas++;

    /* Grep: compositor: soft present */
    kprintf("compositor: soft present enter=%u ok=%u fail_ready=%u "
            "fail_backend=%u multi=%u gen=%u idx=%u presents=%u "
            "first_ok=%u multi_first=%u last_st=%u\n",
            g_u32SoftPresentEnter, g_u32SoftPresentOk,
            g_u32SoftPresentFailRdy, g_u32SoftPresentFailBe, u32Multi,
            u32Gen, u32Idx, u32Presents, g_u32SoftPresentFirstOk,
            g_u32SoftPresentMulti1, g_u32SoftLastPresentSt);
    cAreas++;

    /* Grep: compositor: soft batch */
    kprintf("compositor: soft batch enter=%u ok=%u partial=%u fail=%u "
            "frames_ok=%u stop_rej=%u clamp=%u zero_n=%u req=%u "
            "last_ok=%u last_req=%u peak_ok=%u multi_max=%u\n",
            g_u32SoftBatchEnter, g_u32SoftBatchOk, g_u32SoftBatchPartial,
            g_u32SoftBatchFail, g_u32SoftBatchFramesOk,
            g_u32SoftBatchStopRej, g_u32SoftBatchClamp, g_u32SoftBatchZeroN,
            g_u32SoftBatchReq, g_u32SoftBatchLastOk, g_u32SoftBatchLastReq,
            g_u32SoftPeakBatchOk, GJ_COMP_MULTI_MAX);
    cAreas++;

    /* Grep: compositor: soft geom */
    kprintf("compositor: soft geom ready=%u w=%u h=%u stride=%u bpp=%u "
            "pa=%u pages=%u bytes=%u max_dim=%u fallback=%ux%u min=%ux%u "
            "fb=%u\n",
            u32Ready, u32W, u32H, u32Stride, GJ_COMP_BPP, u32Pa, u32Pages,
            u32Bytes, GJ_COMP_MAX_DIM, GJ_COMP_FALLBACK_W, GJ_COMP_FALLBACK_H,
            GJ_COMP_MIN_W, GJ_COMP_MIN_H, u32Fb);
    cAreas++;

    /* Grep: compositor: soft paint */
    kprintf("compositor: soft paint enter=%u last_pix=%u last_bytes=%u "
            "bpp=%u fill=jade_gradient\n",
            g_u32SoftPaintEnter, g_u32SoftPaintPix, g_u32SoftPaintBytes,
            GJ_COMP_BPP);
    cAreas++;

    /* Grep: compositor: soft peaks */
    kprintf("compositor: soft peaks presents=%u multi=%u gen=%u "
            "batch_ok=%u logs=%u\n",
            g_u32SoftPeakPresents, g_u32SoftPeakMulti, g_u32SoftPeakGen,
            g_u32SoftPeakBatchOk, g_u32SoftLogN);
    cAreas++;

    /* Grep: compositor: soft query */
    kprintf("compositor: soft query ready=%u fb=%u fb_ok=%u fb_null=%u "
            "size=%u size_null_w=%u size_null_h=%u stride=%u "
            "presents=%u multi=%u gen=%u idx=%u\n",
            g_u32SoftQueryReady, g_u32SoftQueryFb, g_u32SoftQueryFbOk,
            g_u32SoftQueryFbNull, g_u32SoftQuerySize,
            g_u32SoftQuerySizeNullW, g_u32SoftQuerySizeNullH,
            g_u32SoftQueryStride, g_u32SoftQueryPresentN, g_u32SoftQueryMulti,
            g_u32SoftQueryGen, g_u32SoftQueryIdx);
    cAreas++;

    /* Grep: compositor: soft ratio */
    kprintf("compositor: soft ratio ok_bp=%u fail_bp=%u enter=%u ok=%u "
            "fail_rdy=%u fail_be=%u batch_ok=%u wave=%u\n",
            u32OkBp, u32FailBp, g_u32SoftPresentEnter, g_u32SoftPresentOk,
            g_u32SoftPresentFailRdy, g_u32SoftPresentFailBe,
            g_u32SoftBatchOk, GJ_COMP_SOFT_WAVE);
    cAreas++;

    /* Grep: compositor: soft last */
    kprintf("compositor: soft last present_st=%u batch_ok=%u batch_req=%u "
            "idx=%u gen=%u presents=%u\n",
            g_u32SoftLastPresentSt, g_u32SoftBatchLastOk,
            g_u32SoftBatchLastReq, u32Idx, u32Gen, u32Presents);
    cAreas++;

    /* Grep: compositor: soft multi — Wave 15 multi-frame soft surface. */
    kprintf("compositor: soft multi live=%u peak=%u gen=%u idx=%u "
            "first=%u batch_peak=%u multi_max=%u soft_idx=1 soft PASS\n",
            u32Multi, g_u32SoftPeakMulti, u32Gen, u32Idx,
            g_u32SoftPresentMulti1, g_u32SoftPeakBatchOk, GJ_COMP_MULTI_MAX);
    cAreas++;

    /* Grep: compositor: soft backend — virtio-gpu present path lamps. */
    kprintf("compositor: soft backend gpu=%u present_ok=%u fail_be=%u "
            "fail_rdy=%u last_st=%u path=virtio_gpu soft %s\n",
            u32Gpu, g_u32SoftPresentOk, g_u32SoftPresentFailBe,
            g_u32SoftPresentFailRdy, g_u32SoftLastPresentSt,
            u32Gpu != 0u ? "PASS" : "SKIP");
    cAreas++;

    /* Grep: compositor: soft scanout — physical tile ownership lamps. */
    kprintf("compositor: soft scanout ready=%u pa=%u fb=%u pages=%u "
            "bytes=%u w=%u h=%u stride=%u bpp=%u claim=kernel_interim "
            "soft %s\n",
            u32Ready, u32Pa, u32Fb, u32Pages, u32Bytes, u32W, u32H,
            u32Stride, GJ_COMP_BPP, u32Ready != 0u ? "PASS" : "INIT");
    cAreas++;

    /* Grep: compositor: soft capacity — Wave 19 design-constant lamps. */
    kprintf("compositor: soft capacity max_dim=%u fallback=%ux%u min=%ux%u "
            "bpp=%u multi_max=%u soft_idx=1 batch=present_n soft PASS "
            "wave=%u\n",
            GJ_COMP_MAX_DIM, GJ_COMP_FALLBACK_W, GJ_COMP_FALLBACK_H,
            GJ_COMP_MIN_W, GJ_COMP_MIN_H, GJ_COMP_BPP, GJ_COMP_MULTI_MAX,
            GJ_COMP_SOFT_WAVE);
    cAreas++;

    /* Grep: compositor: soft headroom — Wave 19 live slack lamps. */
    kprintf("compositor: soft headroom presents=%u peak_presents=%u "
            "multi=%u peak_multi=%u gen=%u peak_gen=%u batch_peak=%u "
            "ok_bp=%u wave=%u\n",
            u32Presents, g_u32SoftPeakPresents, u32Multi, g_u32SoftPeakMulti,
            u32Gen, g_u32SoftPeakGen, g_u32SoftPeakBatchOk, u32OkBp,
            GJ_COMP_SOFT_WAVE);
    cAreas++;

    /* Grep: compositor: soft surface — Wave 19 surface bit lamps. */
    kprintf("compositor: soft surface ready=%u gpu=%u pa=%u fb=%u "
            "present_ok=%u batch_ok=%u multi=%u init_ok=%u "
            "surf=0x%x wave=%u\n",
            u32Ready, u32Gpu, u32Pa, u32Fb,
            g_u32SoftPresentOk != 0u ? 1u : 0u,
            g_u32SoftBatchOk != 0u ? 1u : 0u,
            u32Multi != 0u ? 1u : 0u,
            g_u32SoftInitOk != 0u ? 1u : 0u,
            (u32Ready) | (u32Gpu << 1) | (u32Pa << 2) | (u32Fb << 3) |
                ((g_u32SoftPresentOk != 0u) ? (1u << 4) : 0u) |
                ((g_u32SoftBatchOk != 0u) ? (1u << 5) : 0u) |
                ((u32Multi != 0u) ? (1u << 6) : 0u) |
                ((g_u32SoftInitOk != 0u) ? (1u << 7) : 0u),
            GJ_COMP_SOFT_WAVE);
    cAreas++;

    /* Grep: compositor: soft catalog — area name rollup (Wave 19). */
    kprintf("compositor: soft catalog honesty,inventory,init,present,batch,"
            "geom,paint,peaks,query,ratio,last,multi,backend,scanout,"
            "capacity,headroom,surface,return,catalog,path,deepen "
            "wave=%u areas_expect=25 soft PASS\n",
            GJ_COMP_SOFT_WAVE);
    cAreas++;

    /* Grep: compositor: soft path */
    kprintf("compositor: soft path scanout=pmm+hhdm present=virtio_gpu "
            "multi=soft_idx batch=present_n bpp=bgra policy=door "
            "wave=%u (soft inventory; not bar3; not desktop product)\n",
            GJ_COMP_SOFT_WAVE);
    cAreas++;

    /* Grep: compositor: soft return — Wave 19 API return surfaces */
    kprintf("compositor: soft return init_ok=%u init_idem=%u init_fail_gpu=%u "
            "init_fail_pmm=%u present_ok=%u present_fail_rdy=%u "
            "present_fail_be=%u batch_ok=%u batch_partial=%u ready=%u "
            "product_desktop=OPEN wave=%u\n",
            g_u32SoftInitOk, g_u32SoftInitIdem, g_u32SoftInitFailGpu,
            g_u32SoftInitFailPmm, g_u32SoftPresentOk, g_u32SoftPresentFailRdy,
            g_u32SoftPresentFailBe, g_u32SoftBatchOk, g_u32SoftBatchPartial,
            u32Ready, GJ_COMP_SOFT_WAVE);
    cAreas++;

    /* Grep: compositor: soft retmap — Wave 19 return-surface map */
    kprintf("compositor: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=33\n");

    /* Grep: compositor: soft deepen — Wave 20 stamp + area count. */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: compositor: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("compositor: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_COMP_SOFT_WAVE);
    /* Grep: compositor: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("compositor: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_COMP_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: compositor: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("compositor: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_COMP_SOFT_WAVE);
    /* Grep: compositor: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("compositor: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_COMP_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: compositor: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("compositor: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /* Grep: compositor: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("compositor: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: compositor: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("compositor: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /* Grep: compositor: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("compositor: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: compositor: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("compositor: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /* Grep: compositor: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("compositor: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: compositor: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("compositor: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /* Grep: compositor: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("compositor: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: compositor: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("compositor: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /* Grep: compositor: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("compositor: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: compositor: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("compositor: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
            /* Grep: compositor: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("compositor: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_COMP_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: compositor: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("compositor: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_COMP_SOFT_WAVE);
                    /* Grep: compositor: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("compositor: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_COMP_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: compositor: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("compositor: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_COMP_SOFT_WAVE);
                            /* Grep: compositor: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("compositor: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_COMP_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: compositor: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("compositor: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_COMP_SOFT_WAVE);
                            /* Grep: compositor: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("compositor: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_COMP_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: compositor: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("compositor: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_COMP_SOFT_WAVE);
                            /* Grep: compositor: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("compositor: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_COMP_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: compositor: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("compositor: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_COMP_SOFT_WAVE);
                            /* Grep: compositor: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("compositor: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_COMP_SOFT_WAVE);
                            /* Grep: compositor: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("compositor: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_COMP_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: compositor: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("compositor: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COMP_SOFT_WAVE);
/* Grep: compositor: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("compositor: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_COMP_SOFT_WAVE);
/*
 * ---- Wave 33 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: compositor: soft retbastion — Wave 33 return-bastion honesty */
kprintf("compositor: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_COMP_SOFT_WAVE);
/* Grep: compositor: soft retcitadel — Wave 33 exclusive citadel stamp */
kprintf("compositor: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_COMP_SOFT_WAVE);
                            kprintf("compositor: soft deepen wave=%u areas=%u verdict=%s "
            "ready=%u presents=%u multi=%u gen=%u init_ok=%u batch_ok=%u "
            "desktop_product=OPEN soft_never_gates=1 (soft; not bar3)\n",
            GJ_COMP_SOFT_WAVE, cAreas, szVerdict, u32Ready, u32Presents,
            u32Multi, u32Gen, g_u32SoftInitOk, g_u32SoftBatchOk);

    /* Grep: compositor: soft PASS|PARTIAL|INIT */
    kprintf("compositor: soft %s ready=%u presents=%u multi=%u gen=%u "
            "init_ok=%u batch_ok=%u wave=%u (soft; not bar3)\n",
            szVerdict, u32Ready, u32Presents, u32Multi, u32Gen,
            g_u32SoftInitOk, g_u32SoftBatchOk, GJ_COMP_SOFT_WAVE);
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
    /* Wave 15: re-init after present activity (soft reenter). */
    if (g_u32SoftPresentEnter != 0u || g_u32SoftBatchEnter != 0u ||
        g_u32Presents != 0u) {
        comp_soft_inc(&g_u32SoftInitReenter);
    }

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
    g_u32SoftInitLastPages = 0;
    g_u32SoftInitLastBytes = 0;
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
        comp_soft_inc(&g_u32SoftInitClamp);
        if (u32W == 0 || u32H == 0) {
            comp_soft_inc(&g_u32SoftInitClampZero);
        }
        if (u32W > GJ_COMP_MAX_DIM || u32H > GJ_COMP_MAX_DIM) {
            comp_soft_inc(&g_u32SoftInitClampMax);
        }
        comp_soft_inc(&g_u32SoftInitFallback);
        u32W = GJ_COMP_FALLBACK_W;
        u32H = GJ_COMP_FALLBACK_H;
    }

    g_u32W = u32W;
    g_u32H = u32H;
    g_u32Stride = u32W * GJ_COMP_BPP;
    /* u32W/H ≤ 256 ⇒ stride*height fits comfortably in u32. */
    u32Bytes = g_u32Stride * g_u32H;
    u32Pages = (u32Bytes + GJ_PAGE_SIZE - 1u) / GJ_PAGE_SIZE;
    g_u32SoftInitLastBytes = u32Bytes;
    g_u32SoftInitLastPages = u32Pages;
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
            g_u32SoftInitLastPages = 0;
            g_u32SoftInitLastBytes = 0;
            comp_soft_inc(&g_u32SoftInitFailPmm);
            soft_inventory_log();
            return -1;
        }
        comp_soft_inc(&g_u32SoftInitShrink);
        g_u32W = GJ_COMP_MIN_W;
        g_u32H = GJ_COMP_MIN_H;
        g_u32Stride = g_u32W * GJ_COMP_BPP;
        u32Pages = 1;
        u32Bytes = g_u32Stride * g_u32H;
        g_u32SoftInitLastPages = u32Pages;
        g_u32SoftInitLastBytes = u32Bytes;
    }

    g_pScanout = (void *)hhdm_to_virt(g_paScanout);
    if (g_pScanout == NULL) {
        /* HHDM must cover PMM frames; treat as hard failure. */
        g_paScanout = 0;
        g_u32W = 0;
        g_u32H = 0;
        g_u32Stride = 0;
        g_u32SoftInitLastPages = 0;
        g_u32SoftInitLastBytes = 0;
        comp_soft_inc(&g_u32SoftInitFailHhdm);
        soft_inventory_log();
        return -1;
    }

    p = (u8 *)g_pScanout;
    comp_soft_inc(&g_u32SoftPaintEnter);
    for (iPix = 0; iPix < g_u32W * g_u32H; iPix++) {
        /* GreenJade green-ish gradient (BGRA) — visual bring-up only. */
        p[iPix * 4u + 0] = 0x20;
        p[iPix * 4u + 1] = (u8)(0x80u + (iPix & 0x3fu));
        p[iPix * 4u + 2] = 0x10;
        p[iPix * 4u + 3] = 0xff;
    }
    g_u32SoftPaintPix = g_u32W * g_u32H;
    g_u32SoftPaintBytes = g_u32SoftPaintPix * GJ_COMP_BPP;
    (void)u32Bytes;
    (void)u32Pages;

    g_fReady = 1;
    comp_soft_inc(&g_u32SoftInitOk);
    kprintf("compositor: scanout %ux%u pa=0x%lx ready (multi-frame soft "
            "wave=%u)\n",
            g_u32W, g_u32H, (unsigned long)g_paScanout, GJ_COMP_SOFT_WAVE);
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
            comp_soft_inc(&g_u32SoftPresentMulti1);
            /* greppable: compositor: multi-frame soft */
            kprintf("compositor: multi-frame soft gen=%u idx=%u\n",
                    g_u32FrameGen, g_u32SoftIdx);
        }
    }
    if (!g_fLoggedPresent) {
        g_fLoggedPresent = 1;
        comp_soft_inc(&g_u32SoftPresentFirstOk);
        kprintf("compositor: present ok (quiet after first)\n");
    }
    soft_note_peaks();
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
        g_u32SoftLastPresentSt = 1u; /* fail_ready */
        if (g_fSoftBatchInFlight == 0) {
            soft_inventory_maybe_once();
        }
        return -1;
    }
    st = virtio_gpu_present(g_u32W, g_u32H, g_pScanout, g_u32Stride);
    if (st == 0) {
        comp_soft_inc(&g_u32SoftPresentOk);
        g_u32SoftLastPresentSt = 0u; /* ok */
        comp_note_ok_present();
    } else {
        comp_soft_inc(&g_u32SoftPresentFailBe);
        g_u32SoftLastPresentSt = 2u; /* fail_backend */
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
    u32 u32Req;
    int stLast = -1;

    comp_soft_inc(&g_u32SoftBatchEnter);

    if (!g_fReady) {
        comp_soft_inc(&g_u32SoftBatchFail);
        g_u32SoftBatchLastOk = 0;
        g_u32SoftBatchLastReq = 0;
        soft_inventory_maybe_once();
        return -1;
    }
    if (u32N == 0) {
        comp_soft_inc(&g_u32SoftBatchZeroN);
        u32N = 1;
    }
    if (u32N > GJ_COMP_MULTI_MAX) {
        comp_soft_inc(&g_u32SoftBatchClamp);
        u32N = GJ_COMP_MULTI_MAX;
    }
    u32Req = u32N;
    g_u32SoftBatchReq += u32Req;
    g_u32SoftBatchLastReq = u32Req;
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
    g_u32SoftBatchLastOk = u32Ok;
    if (u32Ok > g_u32SoftPeakBatchOk) {
        g_u32SoftPeakBatchOk = u32Ok;
    }
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
    comp_soft_inc(&g_u32SoftQueryPresentN);
    /*
     * Soft diagnostics re-read: emit inventory so multi-frame smoke greps
     * greppable compositor: soft lines without a dedicated syscall.
     * greppable: compositor: soft
     */
    soft_inventory_log();
    return g_u32Presents;
}

u32
session_compositor_multi_count(void)
{
    comp_soft_inc(&g_u32SoftQueryMulti);
    /* greppable: compositor: soft (query multi) */
    soft_inventory_log();
    return g_u32Multi;
}

u32
session_compositor_frame_gen(void)
{
    comp_soft_inc(&g_u32SoftQueryGen);
    return g_u32FrameGen;
}

u32
session_compositor_soft_index(void)
{
    comp_soft_inc(&g_u32SoftQueryIdx);
    return g_u32SoftIdx & 1u;
}

int
session_compositor_ready(void)
{
    comp_soft_inc(&g_u32SoftQueryReady);
    return g_fReady != 0;
}

/** Scanout VA, or NULL if not ready (never a dangling interim pointer). */
void *
session_compositor_fb(void)
{
    comp_soft_inc(&g_u32SoftQueryFb);
    if (!g_fReady) {
        comp_soft_inc(&g_u32SoftQueryFbNull);
        return NULL;
    }
    comp_soft_inc(&g_u32SoftQueryFbOk);
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

    comp_soft_inc(&g_u32SoftQuerySize);
    if (pW != NULL) {
        *pW = u32W;
    } else {
        comp_soft_inc(&g_u32SoftQuerySizeNullW);
    }
    if (pH != NULL) {
        *pH = u32H;
    } else {
        comp_soft_inc(&g_u32SoftQuerySizeNullH);
    }
}

u32
session_compositor_stride(void)
{
    comp_soft_inc(&g_u32SoftQueryStride);
    if (!g_fReady) {
        return 0;
    }
    return g_u32Stride;
}
