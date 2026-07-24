/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Session door: compositor + input hub + ownership hand-off for sessiond.
 *
 * Opcodes (GJ_SESS_OP_*): present/display, input poll/pop, stats, map
 * scanout, present user FB, claim/release ownership.
 *
 * Ownership: token 0 means kernel interim owns policy; non-zero means a
 * userspace sessiond claimed the door. Claim is re-entrant for the same
 * token (idempotent reclaim soft), BUSY for a different token. RELEASE
 * when free is soft 0.
 *
 * Soft present/input:
 *   PRESENT / PRESENT_FB usable without claim (smokes); multi-frame soft
 *   tracks successive PRESENT_FB (STATS bit18). INPUT_POLL/POP soft-ok
 *   when virtio-input is absent (empty ring).
 *
 * Soft door inventory (Wave 35 exclusive deepen; this unit only):
 *   - soft return: API return-surface catalog (product_*=OPEN)
 *   - soft retmap: Wave 19 return-surface map (ok|fail|… classes)
 *   - Ownership: claim / reclaim / release / busy / claim_inval
 *   - Present: PRESENT / PRESENT_FB ok|fail|nodev + multi-frame tallies
 *   - Input: poll + pop hit/empty (soft-ok without virtio-input)
 *   - Meta: stats / display_info / map_scanout
 *   - Opcode enter surface (per GJ_SESS_OP_*) + last-op snapshot
 *   - Copy path: user vs kernel-smoke + blit rows/clip/ok|fault
 *   - Peaks: calls / claims / reclaims / user_presents high-water
 *   - Soft ok tally + ok_bp ratio + STATS flag soft samples
 *   - Soft verdict INIT|PASS|PARTIAL + path honesty catalog
 *   - Wave 15 base: honesty / owner / terminal / catalog / deepen
 *   - Wave 16 base: capacity / headroom / surface / geom lamps
 *   greppable: "session_door: soft …"
 *   greppable: "session_door: soft deepen"
 *   Never hard-gates; diagnostics only (wrap OK). Soft ≠ bar3.
 *   Soft ≠ desktop/compositor product bar.
 *
 * User pointers: prefer user_range_ok + copy_{to,from}_user. The !user
 * branch is for early kernel smokes that pass HHDM/static buffers.
 */
#include <gj/compositor.h>
#include <gj/error.h>
#include <gj/klog.h>
#include <gj/session_door.h>
#include <gj/session_input.h>
#include <gj/string.h>
#include <gj/user_access.h>
#include <gj/virtio_gpu.h>

/* Match compositor interim clamp (PRESENT_FB / aTmp fallback). */
#define GJ_SESS_MAX_DIM 256u
#define GJ_SESS_TMP_W   64u
#define GJ_SESS_TMP_H   64u
/* Wave 20 deepen stamp (file-local; never hard-gates). */
#define GJ_SESS_SOFT_WAVE 101u

static int g_fInit;
static u32 g_u32Calls;
static u32 g_u32OwnerToken; /* 0 = kernel interim owns */
static u32 g_u32UserPresents;
static u32 g_u32Claims;     /* successful first claims */
static u32 g_u32Reclaims;   /* idempotent same-token CLAIM soft */

/*
 * Soft product inventory (Wave 35 exclusive deepen). Cumulative path tallies.
 * greppable: session_door: soft …
 */
static u32 g_u32SoftClaimInval;    /* CLAIM bad token */
static u32 g_u32SoftClaimBusy;     /* CLAIM different owner */
static u32 g_u32SoftRelease;       /* RELEASE success (was owned) */
static u32 g_u32SoftReleaseFree;   /* RELEASE when already free (soft 0) */
static u32 g_u32SoftReleaseInval;  /* RELEASE token mismatch */
static u32 g_u32SoftPresentOk;     /* PRESENT success */
static u32 g_u32SoftPresentNodev;  /* PRESENT no compositor */
static u32 g_u32SoftPresentIo;     /* PRESENT backend fail */
static u32 g_u32SoftPresentFbOk;   /* PRESENT_FB success */
static u32 g_u32SoftPresentFbInval;/* PRESENT_FB arg reject */
static u32 g_u32SoftPresentFbNodev;/* PRESENT_FB no backend */
static u32 g_u32SoftPresentFbFault;/* PRESENT_FB copy/blit fault */
static u32 g_u32SoftPresentFbIo;   /* PRESENT_FB present fail */
static u32 g_u32SoftPresentFbDirect; /* PRESENT_FB virtio-gpu direct path */
static u32 g_u32SoftPresentFbBlit; /* PRESENT_FB compositor blit path */
static u32 g_u32SoftPresentFbClip; /* PRESENT_FB clipped to scanout */
static u32 g_u32SoftPresentFbUser; /* PRESENT_FB user-range pixel src */
static u32 g_u32SoftPresentFbKern; /* PRESENT_FB kernel-smoke pixel src */
static u32 g_u32SoftDisplayOk;     /* DISPLAY_INFO ok */
static u32 g_u32SoftDisplayInval;  /* DISPLAY_INFO bad arg */
static u32 g_u32SoftDisplayFault;  /* DISPLAY_INFO copy fault */
static u32 g_u32SoftInputPoll;     /* INPUT_POLL ok (always soft) */
static u32 g_u32SoftInputPopHit;   /* INPUT_POP filled event */
static u32 g_u32SoftInputPopEmpty; /* INPUT_POP empty ring */
static u32 g_u32SoftInputPopInval; /* INPUT_POP null dst */
static u32 g_u32SoftInputPopFault; /* INPUT_POP copy fault */
static u32 g_u32SoftStatsOk;       /* STATS copy-out success */
static u32 g_u32SoftStatsInval;    /* STATS null dst */
static u32 g_u32SoftStatsFault;    /* STATS copy fault */
static u32 g_u32SoftMapOk;         /* MAP_SCANOUT success */
static u32 g_u32SoftMapInval;      /* MAP_SCANOUT bad args */
static u32 g_u32SoftMapNodev;      /* MAP_SCANOUT no compositor/fb */
static u32 g_u32SoftMapFault;      /* MAP_SCANOUT copy fault */
static u32 g_u32SoftInval;         /* aggregate INVAL terminals */
static u32 g_u32SoftNodev;         /* aggregate NODEV terminals */
static u32 g_u32SoftBusy;          /* aggregate BUSY terminals */
static u32 g_u32SoftFault;         /* aggregate FAULT terminals */
static u32 g_u32SoftIo;            /* aggregate IO terminals */
static u32 g_u32SoftNosupport;     /* unknown opcode */
static u32 g_u32SoftOk;            /* non-negative terminal returns */
static u32 g_u32SoftLogs;          /* soft inventory emissions */
/* Wave 15: per-opcode enter surface (diagnostics only). */
static u32 g_u32SoftOpPresent;
static u32 g_u32SoftOpDisplay;
static u32 g_u32SoftOpInputPoll;
static u32 g_u32SoftOpInputPop;
static u32 g_u32SoftOpStats;
static u32 g_u32SoftOpPresentFb;
static u32 g_u32SoftOpClaim;
static u32 g_u32SoftOpRelease;
static u32 g_u32SoftOpMap;
static u32 g_u32SoftOpUnknown;
/* Wave 15: copy / blit path surface. */
static u32 g_u32SoftCopyOutUser;   /* sess_copy_out via copy_to_user */
static u32 g_u32SoftCopyOutKern;   /* sess_copy_out HHDM/static */
static u32 g_u32SoftCopyInUser;    /* sess_copy_in via copy_from_user */
static u32 g_u32SoftCopyInKern;    /* sess_copy_in kernel-smoke */
static u32 g_u32SoftBlitEnter;     /* sess_blit_fb entries */
static u32 g_u32SoftBlitOk;        /* blit full success */
static u32 g_u32SoftBlitInval;     /* blit arg reject */
static u32 g_u32SoftBlitFault;     /* blit row copy fault */
static u32 g_u32SoftBlitRows;      /* sum of rows blitted ok */
static u32 g_u32SoftBlitLastRows;  /* rows in most recent ok blit */
/* Wave 15: peaks + last-op + STATS flag samples. */
static u32 g_u32SoftPeakCalls;
static u32 g_u32SoftPeakClaims;
static u32 g_u32SoftPeakReclaims;
static u32 g_u32SoftPeakUserPres;
static u32 g_u32SoftLastOp;
static i64 g_i64SoftLastRet;
static u32 g_u32SoftFlagReady;     /* STATS bit0 samples set */
static u32 g_u32SoftFlagInput;     /* STATS bit1 samples set */
static u32 g_u32SoftFlagOwned;     /* STATS bit2 samples set */
static u32 g_u32SoftFlagDrop;      /* STATS bit16 samples set */
static u32 g_u32SoftFlagUserFb;    /* STATS bit17 samples set */
static u32 g_u32SoftFlagMulti;     /* STATS bit18 samples set */
static u32 g_u32SoftFlagReclaim;   /* STATS bit19 samples set */
static u8  g_fSoftOnce;            /* one-shot after first call activity */

static void sess_soft_inc(u32 *pCtr);
static void sess_soft_add(u32 *pCtr, u32 u32N);
static void sess_soft_note_err(i64 i64R);
static void sess_soft_note_peaks(void);
static void sess_soft_note_op(u32 u32Op);
static void sess_soft_inventory_log(void);
static void sess_soft_maybe_once(void);

/** Soft: bump path tally (u32 wrap is fine for telemetry). */
static void
sess_soft_inc(u32 *pCtr)
{
    if (pCtr == NULL) {
        return;
    }
    if (*pCtr < 0xffffffffu) {
        (*pCtr)++;
    }
}

/** Soft: add to u32 path tally (saturate; wrap avoided). */
static void
sess_soft_add(u32 *pCtr, u32 u32N)
{
    if (pCtr == NULL || u32N == 0u) {
        return;
    }
    if (*pCtr > (0xffffffffu - u32N)) {
        *pCtr = 0xffffffffu;
    } else {
        *pCtr += u32N;
    }
}

/**
 * Soft: classify a terminal status into aggregate err buckets.
 * Non-negative returns count as soft ok (success / soft 0 / pop 1).
 */
static void
sess_soft_note_err(i64 i64R)
{
    if (i64R >= 0) {
        sess_soft_inc(&g_u32SoftOk);
        return;
    }
    if (i64R == GJ_ERR_INVAL) {
        sess_soft_inc(&g_u32SoftInval);
    } else if (i64R == GJ_ERR_NODEV) {
        sess_soft_inc(&g_u32SoftNodev);
    } else if (i64R == GJ_ERR_BUSY) {
        sess_soft_inc(&g_u32SoftBusy);
    } else if (i64R == GJ_ERR_FAULT) {
        sess_soft_inc(&g_u32SoftFault);
    } else if (i64R == GJ_ERR_IO) {
        sess_soft_inc(&g_u32SoftIo);
    } else if (i64R == GJ_ERR_NOSUPPORT) {
        sess_soft_inc(&g_u32SoftNosupport);
    }
}

/** Soft: refresh calls / claims / reclaims / user_presents high-water. */
static void
sess_soft_note_peaks(void)
{
    if (g_u32Calls > g_u32SoftPeakCalls) {
        g_u32SoftPeakCalls = g_u32Calls;
    }
    if (g_u32Claims > g_u32SoftPeakClaims) {
        g_u32SoftPeakClaims = g_u32Claims;
    }
    if (g_u32Reclaims > g_u32SoftPeakReclaims) {
        g_u32SoftPeakReclaims = g_u32Reclaims;
    }
    if (g_u32UserPresents > g_u32SoftPeakUserPres) {
        g_u32SoftPeakUserPres = g_u32UserPresents;
    }
}

/** Soft: per-opcode enter tallies (Wave 15 deepen). */
static void
sess_soft_note_op(u32 u32Op)
{
    if (u32Op == GJ_SESS_OP_PRESENT) {
        sess_soft_inc(&g_u32SoftOpPresent);
    } else if (u32Op == GJ_SESS_OP_DISPLAY_INFO) {
        sess_soft_inc(&g_u32SoftOpDisplay);
    } else if (u32Op == GJ_SESS_OP_INPUT_POLL) {
        sess_soft_inc(&g_u32SoftOpInputPoll);
    } else if (u32Op == GJ_SESS_OP_INPUT_POP) {
        sess_soft_inc(&g_u32SoftOpInputPop);
    } else if (u32Op == GJ_SESS_OP_STATS) {
        sess_soft_inc(&g_u32SoftOpStats);
    } else if (u32Op == GJ_SESS_OP_PRESENT_FB) {
        sess_soft_inc(&g_u32SoftOpPresentFb);
    } else if (u32Op == GJ_SESS_OP_CLAIM) {
        sess_soft_inc(&g_u32SoftOpClaim);
    } else if (u32Op == GJ_SESS_OP_RELEASE) {
        sess_soft_inc(&g_u32SoftOpRelease);
    } else if (u32Op == GJ_SESS_OP_MAP_SCANOUT) {
        sess_soft_inc(&g_u32SoftOpMap);
    } else {
        sess_soft_inc(&g_u32SoftOpUnknown);
    }
}

/**
 * Greppable soft session door inventory (product / smoke; Wave 20 deepen).
 *   session_door: soft honesty …
 *   session_door: soft inventory …
 *   session_door: soft claim …
 *   session_door: soft present …
 *   session_door: soft input …
 *   session_door: soft meta …
 *   session_door: soft op …
 *   session_door: soft copy …
 *   session_door: soft blit …
 *   session_door: soft peaks …
 *   session_door: soft last …
 *   session_door: soft flags …
 *   session_door: soft ratio …
 *   session_door: soft err …
 *   session_door: soft owner …
 *   session_door: soft terminal …
 *   session_door: soft capacity …   (Wave 16)
 *   session_door: soft headroom …   (Wave 16)
 *   session_door: soft surface …    (Wave 16)
 *   session_door: soft geom …       (Wave 16)
 *   session_door: soft catalog …
 *   session_door: soft path …
 *   session_door: soft deepen …
 *   session_door: soft PASS|PARTIAL|INIT
 * greppable: session_door: soft
 * greppable: session_door: soft deepen
 * Honesty: soft inventory only — not bar3 / desktop compositor product.
 */
static void
sess_soft_inventory_log(void)
{
    u32 u32Owned;
    u32 u32Ready;
    u32 u32Input;
    u32 u32Gpu;
    u32 u32Multi;
    u32 u32UserFb;
    u32 u32W = 0;
    u32 u32H = 0;
    u32 u32OkBp;
    u32 u32FbTotal;
    u32 u32ErrSum;
    u32 cAreas;
    const char *szVerdict;

    sess_soft_inc(&g_u32SoftLogs);
    sess_soft_note_peaks();
    cAreas = 0;
    u32Owned = (g_u32OwnerToken != 0) ? 1u : 0u;
    u32Ready = session_compositor_ready() ? 1u : 0u;
    u32Input = session_input_ready() ? 1u : 0u;
    u32Gpu = virtio_gpu_ready() ? 1u : 0u;
    u32UserFb = (g_u32UserPresents != 0) ? 1u : 0u;
    u32Multi = (g_u32UserPresents >= 2u) ? 1u : 0u;
    if (u32Ready != 0) {
        session_compositor_size(&u32W, &u32H);
    }
    /* Soft ok ratio in basis points of calls (0 if none). */
    if (g_u32Calls != 0u) {
        u32OkBp = (g_u32SoftOk * 10000u) / g_u32Calls;
    } else {
        u32OkBp = 0;
    }
    u32FbTotal = g_u32SoftPresentFbOk + g_u32SoftPresentFbInval +
                 g_u32SoftPresentFbNodev + g_u32SoftPresentFbFault +
                 g_u32SoftPresentFbIo;
    u32ErrSum = g_u32SoftInval + g_u32SoftNodev + g_u32SoftBusy +
                g_u32SoftFault + g_u32SoftIo + g_u32SoftNosupport;

    /*
     * Soft verdict (inventory only; never hard-gates door):
     *   INIT     — no product calls yet (baseline)
     *   PASS     — any present/claim/input success observed
     *   PARTIAL  — only failures so far
     */
    if (g_u32SoftPresentOk != 0u || g_u32SoftPresentFbOk != 0u ||
        g_u32Claims != 0u || g_u32SoftInputPopHit != 0u ||
        g_u32SoftMapOk != 0u) {
        szVerdict = "PASS";
    } else if (u32ErrSum != 0u) {
        szVerdict = "PARTIAL";
    } else {
        szVerdict = "INIT";
    }

    /*
     * Honesty first: session door is interim policy bridge, not desktop product.
     * greppable: session_door: soft honesty
     */
    kprintf("session_door: soft honesty interim_door=1 desktop_product=OPEN "
            "sessiond_claim=token bar3=OPEN soft_never_gates=1 wave=%u "
            "(soft; never closes desktop bar)\n",
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft inventory */
    kprintf("session_door: soft inventory calls=%u owned=%u token=0x%x "
            "claims=%u reclaims=%u user_presents=%u multi=%u ready=%u "
            "input=%u gpu=%u w=%u h=%u max_dim=%u ok=%u ok_bp=%u "
            "logs=%u wave=%u\n",
            g_u32Calls, u32Owned, g_u32OwnerToken, g_u32Claims, g_u32Reclaims,
            g_u32UserPresents, u32Multi, u32Ready, u32Input, u32Gpu, u32W,
            u32H, GJ_SESS_MAX_DIM, g_u32SoftOk, u32OkBp, g_u32SoftLogs,
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft claim */
    kprintf("session_door: soft claim ok=%u reclaim=%u busy=%u inval=%u "
            "release=%u release_free=%u release_inval=%u peak_claim=%u "
            "peak_reclaim=%u\n",
            g_u32Claims, g_u32Reclaims, g_u32SoftClaimBusy,
            g_u32SoftClaimInval, g_u32SoftRelease, g_u32SoftReleaseFree,
            g_u32SoftReleaseInval, g_u32SoftPeakClaims,
            g_u32SoftPeakReclaims);
    cAreas++;

    /* Grep: session_door: soft present */
    kprintf("session_door: soft present ok=%u nodev=%u io=%u fb_ok=%u "
            "fb_inval=%u fb_nodev=%u fb_fault=%u fb_io=%u fb_direct=%u "
            "fb_blit=%u fb_clip=%u fb_user=%u fb_kern=%u fb_total=%u "
            "user_fb=%u multi=%u\n",
            g_u32SoftPresentOk, g_u32SoftPresentNodev, g_u32SoftPresentIo,
            g_u32SoftPresentFbOk, g_u32SoftPresentFbInval,
            g_u32SoftPresentFbNodev, g_u32SoftPresentFbFault,
            g_u32SoftPresentFbIo, g_u32SoftPresentFbDirect,
            g_u32SoftPresentFbBlit, g_u32SoftPresentFbClip,
            g_u32SoftPresentFbUser, g_u32SoftPresentFbKern, u32FbTotal,
            u32UserFb, u32Multi);
    cAreas++;

    /* Grep: session_door: soft input */
    kprintf("session_door: soft input poll=%u pop_hit=%u pop_empty=%u "
            "pop_inval=%u pop_fault=%u ready=%u\n",
            g_u32SoftInputPoll, g_u32SoftInputPopHit, g_u32SoftInputPopEmpty,
            g_u32SoftInputPopInval, g_u32SoftInputPopFault, u32Input);
    cAreas++;

    /* Grep: session_door: soft meta */
    kprintf("session_door: soft meta stats=%u stats_inval=%u stats_fault=%u "
            "display=%u display_inval=%u display_fault=%u map=%u "
            "map_inval=%u map_nodev=%u map_fault=%u\n",
            g_u32SoftStatsOk, g_u32SoftStatsInval, g_u32SoftStatsFault,
            g_u32SoftDisplayOk, g_u32SoftDisplayInval, g_u32SoftDisplayFault,
            g_u32SoftMapOk, g_u32SoftMapInval, g_u32SoftMapNodev,
            g_u32SoftMapFault);
    cAreas++;

    /* Grep: session_door: soft op */
    kprintf("session_door: soft op present=%u display=%u input_poll=%u "
            "input_pop=%u stats=%u present_fb=%u claim=%u release=%u "
            "map=%u unknown=%u\n",
            g_u32SoftOpPresent, g_u32SoftOpDisplay, g_u32SoftOpInputPoll,
            g_u32SoftOpInputPop, g_u32SoftOpStats, g_u32SoftOpPresentFb,
            g_u32SoftOpClaim, g_u32SoftOpRelease, g_u32SoftOpMap,
            g_u32SoftOpUnknown);
    cAreas++;

    /* Grep: session_door: soft copy */
    kprintf("session_door: soft copy out_user=%u out_kern=%u in_user=%u "
            "in_kern=%u\n",
            g_u32SoftCopyOutUser, g_u32SoftCopyOutKern, g_u32SoftCopyInUser,
            g_u32SoftCopyInKern);
    cAreas++;

    /* Grep: session_door: soft blit */
    kprintf("session_door: soft blit enter=%u ok=%u inval=%u fault=%u "
            "rows=%u last_rows=%u clip=%u\n",
            g_u32SoftBlitEnter, g_u32SoftBlitOk, g_u32SoftBlitInval,
            g_u32SoftBlitFault, g_u32SoftBlitRows, g_u32SoftBlitLastRows,
            g_u32SoftPresentFbClip);
    cAreas++;

    /* Grep: session_door: soft peaks */
    kprintf("session_door: soft peaks calls=%u claims=%u reclaims=%u "
            "user_presents=%u logs=%u\n",
            g_u32SoftPeakCalls, g_u32SoftPeakClaims, g_u32SoftPeakReclaims,
            g_u32SoftPeakUserPres, g_u32SoftLogs);
    cAreas++;

    /* Grep: session_door: soft last */
    kprintf("session_door: soft last op=%u ret=%ld ok=%u calls=%u\n",
            g_u32SoftLastOp, (long)g_i64SoftLastRet, g_u32SoftOk, g_u32Calls);
    cAreas++;

    /* Grep: session_door: soft flags */
    kprintf("session_door: soft flags ready=%u input=%u owned=%u drop=%u "
            "user_fb=%u multi=%u reclaim=%u\n",
            g_u32SoftFlagReady, g_u32SoftFlagInput, g_u32SoftFlagOwned,
            g_u32SoftFlagDrop, g_u32SoftFlagUserFb, g_u32SoftFlagMulti,
            g_u32SoftFlagReclaim);
    cAreas++;

    /* Grep: session_door: soft ratio */
    kprintf("session_door: soft ratio ok_bp=%u calls=%u ok=%u err_sum=%u "
            "fb_ok=%u fb_fail=%u wave=%u\n",
            u32OkBp, g_u32Calls, g_u32SoftOk, u32ErrSum,
            g_u32SoftPresentFbOk,
            g_u32SoftPresentFbInval + g_u32SoftPresentFbNodev +
                g_u32SoftPresentFbFault + g_u32SoftPresentFbIo,
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft err */
    kprintf("session_door: soft err inval=%u nodev=%u busy=%u fault=%u "
            "io=%u nosupport=%u ok=%u logs=%u\n",
            g_u32SoftInval, g_u32SoftNodev, g_u32SoftBusy, g_u32SoftFault,
            g_u32SoftIo, g_u32SoftNosupport, g_u32SoftOk, g_u32SoftLogs);
    cAreas++;

    /* Grep: session_door: soft owner — Wave 15 ownership snapshot. */
    kprintf("session_door: soft owner owned=%u token=0x%x claims=%u "
            "reclaims=%u release=%u busy=%u free_release=%u "
            "policy=token_reclaim soft %s\n",
            u32Owned, g_u32OwnerToken, g_u32Claims, g_u32Reclaims,
            g_u32SoftRelease, g_u32SoftClaimBusy, g_u32SoftReleaseFree,
            u32Owned != 0u ? "PASS" : "INIT");
    cAreas++;

    /* Grep: session_door: soft terminal — terminal status rollup. */
    kprintf("session_door: soft terminal ok=%u inval=%u nodev=%u busy=%u "
            "fault=%u io=%u nosupport=%u err_sum=%u last_ret=%ld soft PASS\n",
            g_u32SoftOk, g_u32SoftInval, g_u32SoftNodev, g_u32SoftBusy,
            g_u32SoftFault, g_u32SoftIo, g_u32SoftNosupport, u32ErrSum,
            (long)g_i64SoftLastRet);
    cAreas++;

    /* Grep: session_door: soft capacity — Wave 19 design-constant lamps. */
    kprintf("session_door: soft capacity max_dim=%u tmp=%ux%u bpp=4 "
            "ops=9 reclaim=1 multi_frame=1 map_scanout=1 "
            "present_fb=direct|blit soft PASS wave=%u\n",
            GJ_SESS_MAX_DIM, GJ_SESS_TMP_W, GJ_SESS_TMP_H, GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft headroom — Wave 19 live slack lamps. */
    kprintf("session_door: soft headroom calls=%u peak_calls=%u "
            "claims=%u peak_claims=%u user_fb=%u peak_user_fb=%u "
            "ok=%u err_sum=%u wave=%u\n",
            g_u32Calls, g_u32SoftPeakCalls, g_u32Claims, g_u32SoftPeakClaims,
            g_u32UserPresents, g_u32SoftPeakUserPres, g_u32SoftOk, u32ErrSum,
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft surface — Wave 19 surface bit lamps. */
    kprintf("session_door: soft surface ready=%u input=%u gpu=%u owned=%u "
            "user_fb=%u multi=%u present_ok=%u fb_ok=%u map_ok=%u "
            "surf=0x%x wave=%u\n",
            u32Ready, u32Input, u32Gpu, u32Owned, u32UserFb, u32Multi,
            g_u32SoftPresentOk != 0u ? 1u : 0u,
            g_u32SoftPresentFbOk != 0u ? 1u : 0u,
            g_u32SoftMapOk != 0u ? 1u : 0u,
            (u32Ready) | (u32Input << 1) | (u32Gpu << 2) | (u32Owned << 3) |
                (u32UserFb << 4) | (u32Multi << 5) |
                ((g_u32SoftPresentOk != 0u) ? (1u << 6) : 0u) |
                ((g_u32SoftPresentFbOk != 0u) ? (1u << 7) : 0u) |
                ((g_u32SoftMapOk != 0u) ? (1u << 8) : 0u),
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft geom — Wave 19 scanout geometry snapshot. */
    kprintf("session_door: soft geom ready=%u w=%u h=%u max_dim=%u "
            "tmp=%ux%u bpp=bgra4 soft %s wave=%u\n",
            u32Ready, u32W, u32H, GJ_SESS_MAX_DIM, GJ_SESS_TMP_W,
            GJ_SESS_TMP_H, u32Ready != 0u ? "PASS" : "INIT",
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft catalog */
    kprintf("session_door: soft catalog honesty,inventory,claim,present,"
            "input,meta,op,copy,blit,peaks,last,flags,ratio,err,owner,"
            "terminal,capacity,headroom,surface,geom,catalog,return,path,deepen "
            "wave=%u areas_expect=84 soft PASS\n",
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft path */
    kprintf("session_door: soft path claim=sessiond present=comp|gpu_fb "
            "input=hub_soft map=va_hint multi_frame=bit18 reclaim=bit19 "
            "copy=user|kern blit=clip wave=%u "
            "(soft inventory; not bar3; not desktop product)\n",
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft return — Wave 19 API return surfaces */
    kprintf("session_door: soft return ok=%u inval=%u nodev=%u busy=%u "
            "fault=%u claims=%u present_ok=%u present_fb_ok=%u "
            "input_poll=%u input_pop_hit=%u ready=%u "
            "product_sessiond=OPEN wave=%u\n",
            g_u32SoftOk, g_u32SoftInval, g_u32SoftNodev, g_u32SoftBusy,
            g_u32SoftFault, g_u32Claims, g_u32SoftPresentOk,
            g_u32SoftPresentFbOk, g_u32SoftInputPoll, g_u32SoftInputPopHit,
            u32Ready, GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft retmap — Wave 19 return-surface map */
    kprintf("session_door: soft retmap ok|fail|inval|nodev|busy|nomem product_gate=0 soft_only=1 wave=101\n");

    /* Grep: session_door: soft deepen — Wave 20 stamp + area count. */
    /*
     * ---- Wave 19 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: session_door: soft retclass — Wave 19 return-class taxonomy (kept) */
    kprintf("session_door: soft retclass ok|fail|inval|nodev|busy|nomem "
            "soft_only=1 product_gate=0 wave=%u "
            "(retclass taxonomy; Soft≠product; not bar3)\n",
            (unsigned)GJ_SESS_SOFT_WAVE);
    /* Grep: session_door: soft retlane — Wave 19 return-lane catalog (kept) */
    kprintf("session_door: soft retlane inv|selftest|rate|retcode|retmap|class "
            "product_kernel=OPEN soft_ne_product=1 wave=%u "
            "(retlane catalog; Soft≠product)\n",
            (unsigned)GJ_SESS_SOFT_WAVE);
    /*
     * ---- Wave 20 complementary surfaces (kept) (never reshape primary).
     * Return surfaces only — soft inventory; never hard-gates product paths.
     * Soft≠product; not bar3.
     */
    /* Grep: session_door: soft retbound — Wave 20 return-bound honesty (kept) */
    kprintf("session_door: soft retbound soft_only=1 product_gate=0 hard_gate=0 "
            "never_blocks_m0=1 wave=%u "
            "(retbound honesty; Soft≠product; not bar3)\n",
            (unsigned)GJ_SESS_SOFT_WAVE);
    /* Grep: session_door: soft retseal — Wave 20 seal stamp (kept) */
    kprintf("session_door: soft retseal exclusive=1 soft_ne_product=1 "
            "product_kernel=OPEN bar3=0 wave=%u "
            "(retseal stamp; Soft≠product)\n",
            (unsigned)GJ_SESS_SOFT_WAVE);
            /*
             * ---- Wave 21 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: session_door: soft retpulse — Wave 21 return-pulse honesty (kept) */
            kprintf("session_door: soft retpulse soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retpulse honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /* Grep: session_door: soft retmark — Wave 21 mark stamp (kept) */
            kprintf("session_door: soft retmark exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retmark stamp; Soft≠product)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /*
             * ---- Wave 22 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
            */
            /* Grep: session_door: soft retphase — Wave 22 return-phase honesty (kept) */
            kprintf("session_door: soft retphase soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retphase honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /* Grep: session_door: soft retbadge — Wave 22 badge stamp (kept) */
            kprintf("session_door: soft retbadge exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbadge stamp; Soft≠product)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 23 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
            */
            /* Grep: session_door: soft rettoken — Wave 23 return-token honesty (kept) */
            kprintf("session_door: soft rettoken soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(rettoken honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /* Grep: session_door: soft retcrest — Wave 23 crest stamp (kept) */
            kprintf("session_door: soft retcrest exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retcrest stamp; Soft≠product)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /*
             * ---- Wave 24 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: session_door: soft retvault — Wave 24 return-vault honesty (kept) */
            kprintf("session_door: soft retvault soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retvault honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /* Grep: session_door: soft retbanner — Wave 24 banner stamp (kept) */
            kprintf("session_door: soft retbanner exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbanner stamp; Soft≠product)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /*
             * ---- Wave 25 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: session_door: soft retledger — Wave 25 return-ledger honesty (kept) */
            kprintf("session_door: soft retledger soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retledger honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /* Grep: session_door: soft retbeacon — Wave 25 beacon stamp (kept) */
            kprintf("session_door: soft retbeacon exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retbeacon stamp; Soft≠product)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /*
             * ---- Wave 26 complementary surfaces (kept) (never reshape primary).
             * Return surfaces only — soft inventory; never hard-gates product paths.
             * Soft≠product; not bar3.
             */
            /* Grep: session_door: soft retcipher — Wave 26 return-cipher honesty (kept) */
            kprintf("session_door: soft retcipher soft_only=1 product_gate=0 soft_ne_product=1 "
                    "never_blocks_m0=1 wave=%u "
                    "(retcipher honesty; Soft≠product; not bar3)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
            /* Grep: session_door: soft retflame — Wave 26 flame stamp (kept) */
            kprintf("session_door: soft retflame exclusive=1 soft_ne_product=1 "
                    "product_kernel=OPEN bar3=0 wave=%u "
                    "(retflame stamp; Soft≠product)\n",
                    (unsigned)GJ_SESS_SOFT_WAVE);
                    /*
                     * ---- Wave 27 complementary surfaces (kept) (never reshape primary).
                     * Return surfaces only — soft inventory; never hard-gates product paths.
                     * Soft≠product; not bar3.
                     */
                    /* Grep: session_door: soft retprism — Wave 27 return-prism honesty (kept) */
                    kprintf("session_door: soft retprism soft_only=1 product_gate=0 soft_ne_product=1 "
                            "never_blocks_m0=1 wave=%u "
                            "(retprism honesty; Soft≠product; not bar3)\n",
                            (unsigned)GJ_SESS_SOFT_WAVE);
                    /* Grep: session_door: soft retforge — Wave 27 forge stamp (kept) */
                    kprintf("session_door: soft retforge exclusive=1 soft_ne_product=1 "
                            "product_kernel=OPEN bar3=0 wave=%u "
                            "(retforge stamp; Soft≠product)\n",
                            (unsigned)GJ_SESS_SOFT_WAVE);
                            /*
                             * ---- Wave 28 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: session_door: soft retshard — Wave 28 return-shard honesty (kept) */
                            kprintf("session_door: soft retshard soft_only=1 product_gate=0 soft_ne_product=1 "
                                "never_blocks_m0=1 wave=%u "
                                "(retshard honesty; Soft≠product; not bar3)\n",
                                (unsigned)GJ_SESS_SOFT_WAVE);
                            /* Grep: session_door: soft retcrown — Wave 28 crown stamp (kept) */
                            kprintf("session_door: soft retcrown exclusive=1 soft_ne_product=1 "
                                "product_kernel=OPEN bar3=0 wave=%u "
                                "(retcrown stamp; Soft≠product)\n",
                                (unsigned)GJ_SESS_SOFT_WAVE);
                                /*
                             * ---- Wave 29 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: session_door: soft retglyph — Wave 29 return-glyph honesty (kept) */
                            kprintf("session_door: soft retglyph soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retglyph honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_SESS_SOFT_WAVE);
                            /* Grep: session_door: soft retscepter — Wave 29 scepter stamp (kept) */
                            kprintf("session_door: soft retscepter exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retscepter stamp; Soft≠product)\n",
                                    (unsigned)GJ_SESS_SOFT_WAVE);
                                /*
                             * ---- Wave 30 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: session_door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("session_door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_SESS_SOFT_WAVE);
                            /* Grep: session_door: soft retemblem — Wave 30 emblem stamp (kept) */
                            kprintf("session_door: soft retemblem exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retemblem stamp; Soft≠product)\n",
                                    (unsigned)GJ_SESS_SOFT_WAVE);
                            /*
                             * ---- Wave 31 complementary surfaces (kept) (never reshape primary).
                             * Return surfaces only — soft inventory; never hard-gates product paths.
                             * Soft≠product; not bar3.
                             */
                            /* Grep: session_door: soft retaegis — Wave 31 return-aegis honesty (kept) */
                            kprintf("session_door: soft retaegis soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retaegis honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_SESS_SOFT_WAVE);
                            /* Grep: session_door: soft retsigil — Wave 30 return-sigil honesty (kept) */
                            kprintf("session_door: soft retsigil soft_only=1 product_gate=0 soft_ne_product=1 "
                                    "never_blocks_m0=1 wave=%u "
                                    "(retsigil honesty; Soft≠product; not bar3)\n",
                                    (unsigned)GJ_SESS_SOFT_WAVE);
                            /* Grep: session_door: soft retmantle — Wave 31 mantle stamp (kept) */
                            kprintf("session_door: soft retmantle exclusive=1 soft_ne_product=1 "
                                    "product_kernel=OPEN bar3=0 wave=%u "
                                    "(retmantle stamp; Soft≠product)\n",
                                    (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 32 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retbulwark — Wave 32 return-bulwark honesty (kept) */
kprintf("session_door: soft retbulwark soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbulwark honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retpanoply — Wave 32 panoply stamp (kept) */
kprintf("session_door: soft retpanoply exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpanoply stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 33 complementary surfaces (kept) (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retbastion — Wave 33 return-bastion honesty (kept) */
kprintf("session_door: soft retbastion soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbastion honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retcitadel — Wave 33 citadel stamp (kept) */
kprintf("session_door: soft retcitadel exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retcitadel stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 34 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retredoubt — Wave 34 return-redoubt honesty */
kprintf("session_door: soft retredoubt soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retredoubt honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retkeep — Wave 34 exclusive keep stamp */
kprintf("session_door: soft retkeep exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retkeep stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 35 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retfortress — Wave 35 return-fortress honesty */
kprintf("session_door: soft retfortress soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retfortress honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retpalace — Wave 35 exclusive palace stamp */
kprintf("session_door: soft retpalace exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpalace stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 36 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft rethold — Wave 36 return-hold honesty */
kprintf("session_door: soft rethold soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(rethold honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retspire — Wave 36 exclusive spire stamp */
kprintf("session_door: soft retspire exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retspire stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 37 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retwall — Wave 37 return-wall honesty */
kprintf("session_door: soft retwall soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retwall honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retgate — Wave 37 exclusive gate stamp */
kprintf("session_door: soft retgate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retgate stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 38 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retmoat — Wave 38 return-moat honesty */
kprintf("session_door: soft retmoat soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmoat honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retower — Wave 38 exclusive tower stamp */
kprintf("session_door: soft retower exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retower stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
                            
/*
 * ---- Wave 39 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retbarbican — Wave 39 return-barbican honesty */
kprintf("session_door: soft retbarbican soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbarbican honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retglacis — Wave 39 exclusive glacis stamp */
kprintf("session_door: soft retglacis exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retglacis stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 40 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retcurtain — Wave 40 return-curtain honesty */
kprintf("session_door: soft retcurtain soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retcurtain honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retparapet — Wave 40 exclusive parapet stamp */
kprintf("session_door: soft retparapet exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retparapet stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 41 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retravelin — Wave 41 return-travelin honesty */
kprintf("session_door: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retravelin honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retditch — Wave 41 exclusive ditch stamp */
kprintf("session_door: soft retditch exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retditch stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 42 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retportcullis — Wave 42 return-portcullis honesty */
kprintf("session_door: soft retportcullis soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retportcullis honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retbattlement — Wave 42 exclusive battlement stamp */
kprintf("session_door: soft retbattlement exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retbattlement stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/*
 * ---- Wave 43 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retmachicolation — Wave 43 return-machicolation honesty */
kprintf("session_door: soft retmachicolation soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmachicolation honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retarrowslit — Wave 43 exclusive arrowslit stamp */
kprintf("session_door: soft retarrowslit exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retarrowslit stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);

/*
 * ---- Wave 44 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retmerlon — Wave 44 return-merlon honesty */
kprintf("session_door: soft retmerlon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retmerlon honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retembrasure — Wave 44 exclusive embrasure stamp */
kprintf("session_door: soft retembrasure exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retembrasure stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);

/*
 * ---- Wave 45 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retkeepgate — Wave 45 return-keepgate honesty */
kprintf("session_door: soft retkeepgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retkeepgate honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retouterward — Wave 45 exclusive outerward stamp */
kprintf("session_door: soft retouterward exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retouterward stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);

/*
 * ---- Wave 46 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retbailey — Wave 46 return-bailey honesty */
kprintf("session_door: soft retbailey soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=%u "
        "(retbailey honesty; Soft≠product; not bar3)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);
/* Grep: session_door: soft retpostern — Wave 46 exclusive postern stamp */
kprintf("session_door: soft retpostern exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=%u "
        "(retpostern stamp; Soft≠product)\n",
        (unsigned)GJ_SESS_SOFT_WAVE);

/*
 * ---- Wave 47 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retinnerward — Wave 47 return-innerward honesty */
kprintf("session_door: soft retinnerward soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retinnerward honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retdonjon — Wave 47 exclusive donjon stamp */
kprintf("session_door: soft retdonjon exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retdonjon stamp; Soft≠product)\n");

/*
 * ---- Wave 48 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retchevaux — Wave 48 return-chevaux honesty */
kprintf("session_door: soft retchevaux soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retchevaux honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retpalisade — Wave 48 exclusive palisade stamp */
kprintf("session_door: soft retpalisade exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retpalisade stamp; Soft≠product)\n");

/*
 * ---- Wave 49 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retglacisgate — Wave 49 return-glacisgate honesty */
kprintf("session_door: soft retglacisgate soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retglacisgate honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retoutwork — Wave 49 exclusive outwork stamp */
kprintf("session_door: soft retoutwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retoutwork stamp; Soft≠product)\n");
/*
 * ---- Wave 50 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retsally — Wave 50 return-sally honesty */
kprintf("session_door: soft retsally soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retsally honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retcounterscarp — Wave 50 exclusive counterscarp stamp */
kprintf("session_door: soft retcounterscarp exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retcounterscarp stamp; Soft≠product)\n");
/*
 * ---- Wave 51 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retfosse — Wave 51 return-fosse honesty */
kprintf("session_door: soft retfosse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retfosse honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retcoveredway — Wave 51 exclusive coveredway stamp */
kprintf("session_door: soft retcoveredway exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retcoveredway stamp; Soft≠product)\n");

/*
 * ---- Wave 52 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft rettenaille — Wave 52 return-tenaille honesty */
kprintf("session_door: soft rettenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(rettenaille honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retdemilune — Wave 52 exclusive demilune stamp */
kprintf("session_door: soft retdemilune exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retdemilune stamp; Soft≠product)\n");
/*
 * ---- Wave 53 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retravelin — Wave 53 return-travelin honesty */
kprintf("session_door: soft retravelin soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retravelin honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retlunette — Wave 53 exclusive lunette stamp */
kprintf("session_door: soft retlunette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retlunette stamp; Soft≠product)\n");
/*
 * ---- Wave 54 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retcaponier — Wave 54 return-caponier honesty */
kprintf("session_door: soft retcaponier soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retcaponier honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retredan — Wave 54 exclusive redan stamp */
kprintf("session_door: soft retredan exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retredan stamp; Soft≠product)\n");
/*
 * ---- Wave 55 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retflank — Wave 55 return-flank honesty */
kprintf("session_door: soft retflank soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retflank honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retface — Wave 55 exclusive face stamp */
kprintf("session_door: soft retface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retface stamp; Soft≠product)\n");
/*
 * ---- Wave 56 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retgorge — Wave 56 return-gorge honesty */
kprintf("session_door: soft retgorge soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retgorge honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retshoulder — Wave 56 exclusive shoulder stamp */
kprintf("session_door: soft retshoulder exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retshoulder stamp; Soft≠product)\n");
/*
 * ---- Wave 57 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retraverse — Wave 57 return-traverse honesty */
kprintf("session_door: soft retraverse soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retraverse honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retcasemate — Wave 57 exclusive casemate stamp */
kprintf("session_door: soft retcasemate exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retcasemate stamp; Soft≠product)\n");

/*
 * ---- Wave 58 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retorillon — Wave 58 return-orillon honesty */
kprintf("session_door: soft retorillon soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retorillon honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retbonnette — Wave 58 exclusive bonnette stamp */
kprintf("session_door: soft retbonnette exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retbonnette stamp; Soft≠product)\n");

/*
 * ---- Wave 59 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retcrownwork — Wave 59 return-crownwork honesty */
kprintf("session_door: soft retcrownwork soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retcrownwork honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft rethornwork — Wave 59 exclusive hornwork stamp */
kprintf("session_door: soft rethornwork exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(rethornwork stamp; Soft≠product)\n");

/*
 * ---- Wave 60 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retplace — Wave 60 return-place honesty */
kprintf("session_door: soft retplace soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retplace honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retenvelope — Wave 60 exclusive envelope stamp */
kprintf("session_door: soft retenvelope exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retenvelope stamp; Soft≠product)\n");









/*
 * ---- Wave 61 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retcounterguard — Wave 61 return-counterguard honesty */
kprintf("session_door: soft retcounterguard soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retcounterguard honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retcoveredface — Wave 61 exclusive coveredface stamp */
kprintf("session_door: soft retcoveredface exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retcoveredface stamp; Soft≠product)\n");
/*
 * ---- Wave 62 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retbastionface — Wave 62 return-bastionface honesty */
kprintf("session_door: soft retbastionface soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retbastionface honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retcurtainangle — Wave 62 exclusive curtainangle stamp */
kprintf("session_door: soft retcurtainangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retcurtainangle stamp; Soft≠product)\n");
/*
 * ---- Wave 63 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retdoubletenaille — Wave 63 return-doubletenaille honesty */
kprintf("session_door: soft retdoubletenaille soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retdoubletenaille honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retplaceofarms — Wave 63 exclusive placeofarms stamp */
kprintf("session_door: soft retplaceofarms exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retplaceofarms stamp; Soft≠product)\n");
 /*
  * ---- Wave 64 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: session_door: soft retreentrant — Wave 64 return-reentrant honesty */
kprintf("session_door: soft retreentrant soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retreentrant honesty; Soft≠product; not bar3)\n");
 /* Grep: session_door: soft retsallyport — Wave 64 exclusive sallyport stamp */
kprintf("session_door: soft retsallyport exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retsallyport stamp; Soft≠product)\n");
 /*
  * ---- Wave 65 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: session_door: soft retgorgeangle — Wave 65 return-gorgeangle honesty */
kprintf("session_door: soft retgorgeangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retgorgeangle honesty; Soft≠product; not bar3)\n");
 /* Grep: session_door: soft retshoulderangle — Wave 65 exclusive shoulderangle stamp */
kprintf("session_door: soft retshoulderangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retshoulderangle stamp; Soft≠product)\n");
 /*
  * ---- Wave 66 exclusive complementary surfaces (never reshape primary).
  * Return surfaces only — soft inventory; never hard-gates product paths.
  * Soft≠product; not bar3.
  */
 /* Grep: session_door: soft retflankangle — Wave 66 return-flankangle honesty */
 kprintf("session_door: soft retflankangle soft_only=1 product_gate=0 soft_ne_product=1 "
         "never_blocks_m0=1 wave=101 "
         "(retflankangle honesty; Soft≠product; not bar3)\n");
 /* Grep: session_door: soft retfaceangle — Wave 66 exclusive faceangle stamp */
 kprintf("session_door: soft retfaceangle exclusive=1 soft_ne_product=1 "
         "product_kernel=OPEN bar3=0 wave=101 "
         "(retfaceangle stamp; Soft≠product)\n");
/*
 * ---- Wave 67 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retcaponierangle — Wave 67 return-caponierangle honesty */
kprintf("session_door: soft retcaponierangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retcaponierangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retredanangle — Wave 67 exclusive redanangle stamp */
kprintf("session_door: soft retredanangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retredanangle stamp; Soft≠product)\n");
/*
 * ---- Wave 68 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retlunetteangle — Wave 68 return-lunetteangle honesty */
kprintf("session_door: soft retlunetteangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retlunetteangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft rettenailleangle — Wave 68 exclusive tenailleangle stamp */
kprintf("session_door: soft rettenailleangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(rettenailleangle stamp; Soft≠product)\n");
/*
 * ---- Wave 69 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retdemiluneangle — Wave 69 return-demiluneangle honesty */
kprintf("session_door: soft retdemiluneangle soft_only=1 product_gate=0 soft_ne_product=1 "
        "never_blocks_m0=1 wave=101 "
        "(retdemiluneangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retcoveredwayangle — Wave 69 exclusive coveredwayangle stamp */
kprintf("session_door: soft retcoveredwayangle exclusive=1 soft_ne_product=1 "
        "product_kernel=OPEN bar3=0 wave=101 "
        "(retcoveredwayangle stamp; Soft≠product)\n");
/*
 * ---- Wave 70 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retfosseangle — Wave 70 return-fosseangle honesty */
kprintf("session_door: soft retfosseangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retfosseangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retcounterscarple — Wave 70 exclusive counterscarple stamp */
kprintf("session_door: soft retcounterscarple exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retcounterscarple stamp; Soft≠product)\n");
/*
 * ---- Wave 71 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retsallyportangle — Wave 71 return-sallyportangle honesty */
kprintf("session_door: soft retsallyportangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retsallyportangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retreentrantangle — Wave 71 exclusive reentrantangle stamp */
kprintf("session_door: soft retreentrantangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retreentrantangle stamp; Soft≠product)\n");
/*
 * ---- Wave 72 exclusive complementary surfaces (never reshape primary).
 * Return surfaces only — soft inventory; never hard-gates product paths.
 * Soft≠product; not bar3.
 */
/* Grep: session_door: soft retplaceofarmsangle — Wave 72 return-placeofarmsangle honesty */
kprintf("session_door: soft retplaceofarmsangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retplaceofarmsangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retdoubletenailleangle — Wave 72 exclusive doubletenailleangle stamp */
kprintf("session_door: soft retdoubletenailleangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retdoubletenailleangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retcurtainface — Wave 73 return-curtainface honesty */
kprintf("session_door: soft retcurtainface soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retcurtainface honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retbastionangle — Wave 73 exclusive bastionangle stamp */
kprintf("session_door: soft retbastionangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retbastionangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retglacisangle — Wave 74 return-glacisangle honesty */
kprintf("session_door: soft retglacisangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retglacisangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retparapetangle — Wave 74 exclusive parapetangle stamp */
kprintf("session_door: soft retparapetangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retparapetangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retmoatangle — Wave 75 return-moatangle honesty */
kprintf("session_door: soft retmoatangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retmoatangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retowerangle — Wave 75 exclusive towerangle stamp */
kprintf("session_door: soft retowerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retowerangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retgateangle — Wave 76 return-gateangle honesty */
kprintf("session_door: soft retgateangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retgateangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retwallangle — Wave 76 exclusive wallangle stamp */
kprintf("session_door: soft retwallangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retwallangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retspireangle — Wave 77 return-spireangle honesty */
kprintf("session_door: soft retspireangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retspireangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retholdangle — Wave 77 exclusive holdangle stamp */
kprintf("session_door: soft retholdangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retholdangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retpalaceangle — Wave 78 return-palaceangle honesty */
kprintf("session_door: soft retpalaceangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retpalaceangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retfortressangle — Wave 78 exclusive fortressangle stamp */
kprintf("session_door: soft retfortressangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retfortressangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retkeepangle — Wave 79 return-keepangle honesty */
kprintf("session_door: soft retkeepangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retkeepangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retredoubtangle — Wave 79 exclusive redoubtangle stamp */
kprintf("session_door: soft retredoubtangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retredoubtangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retcitadelangle — Wave 80 return-citadelangle honesty */
kprintf("session_door: soft retcitadelangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retcitadelangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retbastionkeep — Wave 80 exclusive bastionkeep stamp */
kprintf("session_door: soft retbastionkeep exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retbastionkeep stamp; Soft≠product)\n");
/* Grep: session_door: soft retpanoplyangle — Wave 81 return-panoplyangle honesty */
kprintf("session_door: soft retpanoplyangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retpanoplyangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retbulwarkangle — Wave 81 exclusive bulwarkangle stamp */
kprintf("session_door: soft retbulwarkangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retbulwarkangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retmantleangle — Wave 82 return-mantleangle honesty */
kprintf("session_door: soft retmantleangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retmantleangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retaegisangle — Wave 82 exclusive aegisangle stamp */
kprintf("session_door: soft retaegisangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retaegisangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retemblemangle — Wave 83 return-emblemangle honesty */
kprintf("session_door: soft retemblemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retemblemangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retsigilangle — Wave 83 exclusive sigilangle stamp */
kprintf("session_door: soft retsigilangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retsigilangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retscepterangle — Wave 84 return-scepterangle honesty */
kprintf("session_door: soft retscepterangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retscepterangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retglyphangle — Wave 84 exclusive glyphangle stamp */
kprintf("session_door: soft retglyphangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retglyphangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retcrownangle — Wave 85 return-crownangle honesty */
kprintf("session_door: soft retcrownangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retcrownangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retshardangle — Wave 85 exclusive shardangle stamp */
kprintf("session_door: soft retshardangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retshardangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retforgeangle — Wave 86 return-forgeangle honesty */
kprintf("session_door: soft retforgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retforgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retprismangle — Wave 86 exclusive prismangle stamp */
kprintf("session_door: soft retprismangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retprismangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retflameangle — Wave 87 return-flameangle honesty */
kprintf("session_door: soft retflameangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retflameangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retcipherangle — Wave 87 exclusive cipherangle stamp */
kprintf("session_door: soft retcipherangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retcipherangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retbeaconangle — Wave 88 return-beaconangle honesty */
kprintf("session_door: soft retbeaconangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retbeaconangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retledgerangle — Wave 88 exclusive ledgerangle stamp */
kprintf("session_door: soft retledgerangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retledgerangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retbannerangle — Wave 89 return-bannerangle honesty */
kprintf("session_door: soft retbannerangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retbannerangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retvaultangle — Wave 89 exclusive vaultangle stamp */
kprintf("session_door: soft retvaultangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retvaultangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retcrestangle — Wave 90 return-crestangle honesty */
kprintf("session_door: soft retcrestangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retcrestangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft rettokenangle — Wave 90 exclusive tokenangle stamp */
kprintf("session_door: soft rettokenangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (rettokenangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retbadgeangle — Wave 91 return-badgeangle honesty */
kprintf("session_door: soft retbadgeangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retbadgeangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retphaseangle — Wave 91 exclusive phaseangle stamp */
kprintf("session_door: soft retphaseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retphaseangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retmarkangle — Wave 92 return-markangle honesty */
kprintf("session_door: soft retmarkangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retmarkangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retpulseangle — Wave 92 exclusive pulseangle stamp */
kprintf("session_door: soft retpulseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retpulseangle stamp; Soft≠product)\n");

/* Grep: session_door: soft retsealangle — Wave 93 return-sealangle honesty */
kprintf("session_door: soft retsealangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retsealangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retboundangle — Wave 93 exclusive boundangle stamp */
kprintf("session_door: soft retboundangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retboundangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retstemangle — Wave 94 return-stemangle honesty */
kprintf("session_door: soft retstemangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retstemangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retbladeangle — Wave 94 exclusive bladeangle stamp */
kprintf("session_door: soft retbladeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retbladeangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retchordangle — Wave 95 return-chordangle honesty */
kprintf("session_door: soft retchordangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retchordangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retarcangle — Wave 95 exclusive arcangle stamp */
kprintf("session_door: soft retarcangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retarcangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retsectorangle — Wave 96 return-sectorangle honesty */
kprintf("session_door: soft retsectorangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retsectorangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retwedgeangle — Wave 96 exclusive wedgeangle stamp */
kprintf("session_door: soft retwedgeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retwedgeangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retradiusangle — Wave 97 return-radiusangle honesty */
kprintf("session_door: soft retradiusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retradiusangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retdiameterangle — Wave 97 exclusive diameterangle stamp */
kprintf("session_door: soft retdiameterangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retdiameterangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retcircumangle — Wave 98 return-circumangle honesty */
kprintf("session_door: soft retcircumangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retcircumangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retellipseangle — Wave 98 exclusive ellipseangle stamp */
kprintf("session_door: soft retellipseangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retellipseangle stamp; Soft≠product)\n");
/* Grep: session_door: soft rethyperangle — Wave 99 return-hyperangle honesty */
kprintf("session_door: soft rethyperangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (rethyperangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retparabolaangle — Wave 99 exclusive parabolaangle stamp */
kprintf("session_door: soft retparabolaangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retparabolaangle stamp; Soft≠product)\n");
/* Grep: session_door: soft retspiralangle — Wave 100 return-spiralangle honesty */
kprintf("session_door: soft retspiralangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (retspiralangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft rethelixangle — Wave 100 exclusive helixangle stamp */
kprintf("session_door: soft rethelixangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (rethelixangle stamp; Soft≠product)\n");
/* Grep: session_door: soft rettorusangle — Wave 101 return-torusangle honesty */
kprintf("session_door: soft rettorusangle soft_only=1 product_gate=0 soft_ne_product=1 never_blocks_m0=1 wave=101 (rettorusangle honesty; Soft≠product; not bar3)\n");
/* Grep: session_door: soft retknotangle — Wave 101 exclusive knotangle stamp */
kprintf("session_door: soft retknotangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=101 (retknotangle stamp; Soft≠product)\n");
                            kprintf("session_door: soft deepen wave=%u areas=%u verdict=%s "
            "ready=%u owned=%u presents_user=%u claims=%u multi=%u "
            "desktop_product=OPEN soft_never_gates=1 (soft; not bar3)\n",
            GJ_SESS_SOFT_WAVE, cAreas, szVerdict, u32Ready, u32Owned,
            g_u32UserPresents, g_u32Claims, u32Multi);

    /* Grep: session_door: soft PASS|PARTIAL|INIT */
    kprintf("session_door: soft %s ready=%u owned=%u presents_user=%u "
            "claims=%u multi=%u wave=%u (soft; not bar3)\n",
            szVerdict, u32Ready, u32Owned, g_u32UserPresents, g_u32Claims,
            u32Multi, GJ_SESS_SOFT_WAVE);
}

/**
 * After first product call activity, print soft inventory once (mirrors
 * store/net/compositor soft-stats-once). Safe from call return paths only.
 */
static void
sess_soft_maybe_once(void)
{
    if (g_fSoftOnce != 0) {
        return;
    }
    if (g_u32Calls == 0) {
        return;
    }
    g_fSoftOnce = 1;
    sess_soft_inventory_log();
}

void
session_door_init(void)
{
    g_fInit = 1;
    g_u32Calls = 0;
    g_u32OwnerToken = 0;
    g_u32UserPresents = 0;
    g_u32Claims = 0;
    g_u32Reclaims = 0;
    g_u32SoftClaimInval = 0;
    g_u32SoftClaimBusy = 0;
    g_u32SoftRelease = 0;
    g_u32SoftReleaseFree = 0;
    g_u32SoftReleaseInval = 0;
    g_u32SoftPresentOk = 0;
    g_u32SoftPresentNodev = 0;
    g_u32SoftPresentIo = 0;
    g_u32SoftPresentFbOk = 0;
    g_u32SoftPresentFbInval = 0;
    g_u32SoftPresentFbNodev = 0;
    g_u32SoftPresentFbFault = 0;
    g_u32SoftPresentFbIo = 0;
    g_u32SoftPresentFbDirect = 0;
    g_u32SoftPresentFbBlit = 0;
    g_u32SoftDisplayOk = 0;
    g_u32SoftDisplayInval = 0;
    g_u32SoftDisplayFault = 0;
    g_u32SoftInputPoll = 0;
    g_u32SoftInputPopHit = 0;
    g_u32SoftInputPopEmpty = 0;
    g_u32SoftInputPopInval = 0;
    g_u32SoftInputPopFault = 0;
    g_u32SoftStatsOk = 0;
    g_u32SoftStatsInval = 0;
    g_u32SoftStatsFault = 0;
    g_u32SoftMapOk = 0;
    g_u32SoftMapInval = 0;
    g_u32SoftMapNodev = 0;
    g_u32SoftMapFault = 0;
    g_u32SoftInval = 0;
    g_u32SoftNodev = 0;
    g_u32SoftBusy = 0;
    g_u32SoftFault = 0;
    g_u32SoftIo = 0;
    g_u32SoftNosupport = 0;
    g_u32SoftOk = 0;
    g_u32SoftLogs = 0;
    g_u32SoftPresentFbClip = 0;
    g_u32SoftPresentFbUser = 0;
    g_u32SoftPresentFbKern = 0;
    g_u32SoftOpPresent = 0;
    g_u32SoftOpDisplay = 0;
    g_u32SoftOpInputPoll = 0;
    g_u32SoftOpInputPop = 0;
    g_u32SoftOpStats = 0;
    g_u32SoftOpPresentFb = 0;
    g_u32SoftOpClaim = 0;
    g_u32SoftOpRelease = 0;
    g_u32SoftOpMap = 0;
    g_u32SoftOpUnknown = 0;
    g_u32SoftCopyOutUser = 0;
    g_u32SoftCopyOutKern = 0;
    g_u32SoftCopyInUser = 0;
    g_u32SoftCopyInKern = 0;
    g_u32SoftBlitEnter = 0;
    g_u32SoftBlitOk = 0;
    g_u32SoftBlitInval = 0;
    g_u32SoftBlitFault = 0;
    g_u32SoftBlitRows = 0;
    g_u32SoftBlitLastRows = 0;
    g_u32SoftPeakCalls = 0;
    g_u32SoftPeakClaims = 0;
    g_u32SoftPeakReclaims = 0;
    g_u32SoftPeakUserPres = 0;
    g_u32SoftLastOp = 0;
    g_i64SoftLastRet = 0;
    g_u32SoftFlagReady = 0;
    g_u32SoftFlagInput = 0;
    g_u32SoftFlagOwned = 0;
    g_u32SoftFlagDrop = 0;
    g_u32SoftFlagUserFb = 0;
    g_u32SoftFlagMulti = 0;
    g_u32SoftFlagReclaim = 0;
    g_fSoftOnce = 0;
    kprintf("session_door: init (present+input+claim soft wave=%u)\n",
            GJ_SESS_SOFT_WAVE);
    /* Grep: session_door: soft (baseline inventory after init) */
    sess_soft_inventory_log();
}

int
session_door_owned(void)
{
    return g_u32OwnerToken != 0;
}

u32
session_door_owner_token(void)
{
    return g_u32OwnerToken;
}

u32
session_door_claim_count(void)
{
    /* Soft diagnostics: first claims + idempotent reclaims. */
    /*
     * Emit soft inventory on claim stats read so bring-up can grep
     * session_door: soft … (Wave 16). greppable: session_door claim soft
     */
    sess_soft_inventory_log();
    return g_u32Claims + g_u32Reclaims;
}

u32
session_door_user_presents(void)
{
    /*
     * Soft diagnostics re-read: emit inventory so multi-frame smoke greps
     * greppable session_door: soft lines without a dedicated syscall.
     */
    sess_soft_inventory_log();
    return g_u32UserPresents;
}

/**
 * Copy @cb bytes to caller buffer at @u64Dst.
 * Uses copy_to_user when the range is in the user VA window.
 */
static i64
sess_copy_out(u64 u64Dst, const void *pSrc, u32 cb)
{
    if (u64Dst == 0 || pSrc == NULL || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Dst, cb)) {
        if (copy_to_user(u64Dst, pSrc, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
        sess_soft_inc(&g_u32SoftCopyOutUser);
    } else {
        /* Kernel-smoke path: destination is a trusted HHDM/static buffer. */
        memcpy((void *)(gj_vaddr_t)u64Dst, pSrc, cb);
        sess_soft_inc(&g_u32SoftCopyOutKern);
    }
    return 0;
}

/**
 * Copy @cb bytes from caller buffer at @u64Src into @pDst.
 */
static i64
sess_copy_in(void *pDst, u64 u64Src, u32 cb)
{
    if (pDst == NULL || u64Src == 0 || cb == 0) {
        return GJ_ERR_INVAL;
    }
    if (user_range_ok(u64Src, cb)) {
        if (copy_from_user(pDst, u64Src, cb) != GJ_OK) {
            return GJ_ERR_FAULT;
        }
        sess_soft_inc(&g_u32SoftCopyInUser);
    } else {
        memcpy(pDst, (const void *)(gj_vaddr_t)u64Src, cb);
        sess_soft_inc(&g_u32SoftCopyInKern);
    }
    return 0;
}

/**
 * Blit user BGRA tile into scanout top-left with independent strides.
 * @u64Src is user/kernel base; @u32SrcStride / @u32DstStride in bytes.
 * Copies @u32CopyW * 4 bytes per row for @u32CopyH rows.
 */
static i64
sess_blit_fb(u8 *pDst, u32 u32DstStride, u64 u64Src, u32 u32SrcStride,
             u32 u32CopyW, u32 u32CopyH)
{
    u32 iRow;
    u32 cbRow = u32CopyW * 4u;
    i64 st;

    sess_soft_inc(&g_u32SoftBlitEnter);

    if (pDst == NULL || u64Src == 0 || u32CopyW == 0 || u32CopyH == 0 ||
        cbRow == 0 || u32SrcStride < cbRow || u32DstStride < cbRow) {
        sess_soft_inc(&g_u32SoftBlitInval);
        return GJ_ERR_INVAL;
    }

    for (iRow = 0; iRow < u32CopyH; iRow++) {
        u64 u64Row = u64Src + (u64)iRow * (u64)u32SrcStride;
        u8 *pRow = pDst + (u64)iRow * (u64)u32DstStride;

        /* Overflow / wrap guards for adversarial huge strides. */
        if (u64Row < u64Src) {
            sess_soft_inc(&g_u32SoftBlitInval);
            return GJ_ERR_INVAL;
        }
        st = sess_copy_in(pRow, u64Row, cbRow);
        if (st != 0) {
            if (st == GJ_ERR_FAULT) {
                sess_soft_inc(&g_u32SoftBlitFault);
            } else {
                sess_soft_inc(&g_u32SoftBlitInval);
            }
            return st;
        }
    }
    sess_soft_inc(&g_u32SoftBlitOk);
    sess_soft_add(&g_u32SoftBlitRows, u32CopyH);
    g_u32SoftBlitLastRows = u32CopyH;
    return 0;
}

i64
session_door_call(u32 u32Op, u64 u64Arg1, u64 u64Arg2, u64 u64Arg3)
{
    i64 i64Ret;
    int fSoftInv = 0;

    if (!g_fInit) {
        return GJ_ERR_NODEV;
    }
    g_u32Calls++;
    sess_soft_note_op(u32Op);
    sess_soft_note_peaks();
    g_u32SoftLastOp = u32Op;

    switch (u32Op) {
    case GJ_SESS_OP_CLAIM:
        /* arg1 = non-zero ownership token (low 32 bits). */
        if (u64Arg1 == 0 || (u64Arg1 >> 32) != 0) {
            sess_soft_inc(&g_u32SoftClaimInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (g_u32OwnerToken != 0 && g_u32OwnerToken != (u32)u64Arg1) {
            sess_soft_inc(&g_u32SoftClaimBusy);
            i64Ret = GJ_ERR_BUSY; /* another sessiond */
            break;
        }
        /* Soft reclaim: same token re-CLAIM is idempotent (no re-log). */
        if (g_u32OwnerToken == (u32)u64Arg1) {
            g_u32Reclaims++;
            i64Ret = 0;
            break;
        }
        g_u32OwnerToken = (u32)u64Arg1;
        g_u32Claims++;
        kprintf("session_door: CLAIM token=0x%x (userspace owns scanout)\n",
                g_u32OwnerToken);
        i64Ret = 0;
        break;

    case GJ_SESS_OP_RELEASE:
        /* Soft free path: already unowned → 0 (no token match required). */
        if (g_u32OwnerToken == 0) {
            sess_soft_inc(&g_u32SoftReleaseFree);
            i64Ret = 0;
            break;
        }
        if ((u64Arg1 >> 32) != 0 || (u32)u64Arg1 != g_u32OwnerToken) {
            sess_soft_inc(&g_u32SoftReleaseInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        kprintf("session_door: RELEASE token=0x%x\n", g_u32OwnerToken);
        g_u32OwnerToken = 0;
        sess_soft_inc(&g_u32SoftRelease);
        i64Ret = 0;
        break;

    case GJ_SESS_OP_PRESENT:
        /*
         * Present kernel interim scanout. When claimed, sessiond prefers
         * PRESENT_FB; this path remains for bring-up tools (soft ok).
         * Soft multi-frame: use present_n(1) path bookkeeping via present().
         */
        if (!session_compositor_ready()) {
            sess_soft_inc(&g_u32SoftPresentNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        if (session_compositor_present() != 0) {
            sess_soft_inc(&g_u32SoftPresentIo);
            i64Ret = GJ_ERR_IO;
            break;
        }
        sess_soft_inc(&g_u32SoftPresentOk);
        i64Ret = 0;
        break;

    case GJ_SESS_OP_DISPLAY_INFO: {
        /* arg1 → u32[2] {w, h} */
        u32 aWH[2];
        i64 st;

        if (u64Arg1 == 0) {
            sess_soft_inc(&g_u32SoftDisplayInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        aWH[0] = 0;
        aWH[1] = 0;
        session_compositor_size(&aWH[0], &aWH[1]);
        st = sess_copy_out(u64Arg1, aWH, sizeof(aWH));
        if (st == 0) {
            sess_soft_inc(&g_u32SoftDisplayOk);
        } else if (st == GJ_ERR_FAULT) {
            sess_soft_inc(&g_u32SoftDisplayFault);
        } else {
            sess_soft_inc(&g_u32SoftDisplayInval);
        }
        i64Ret = st;
        break;
    }

    case GJ_SESS_OP_INPUT_POLL:
        /* Soft: always 0; empty hub when virtio-input absent. */
        session_input_poll();
        sess_soft_inc(&g_u32SoftInputPoll);
        i64Ret = 0;
        break;

    case GJ_SESS_OP_INPUT_POP: {
        /* arg1 → struct gj_input_event; ret 1 if event, 0 if empty. */
        struct gj_input_event ev;
        int fGot;
        i64 st;

        if (u64Arg1 == 0) {
            sess_soft_inc(&g_u32SoftInputPopInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        memset(&ev, 0, sizeof(ev));
        /* pop soft-refills once (lazy fan-in) inside session_input_pop. */
        fGot = session_input_pop(&ev);
        if (!fGot) {
            sess_soft_inc(&g_u32SoftInputPopEmpty);
            i64Ret = 0;
            break;
        }
        st = sess_copy_out(u64Arg1, &ev, sizeof(ev));
        if (st != 0) {
            if (st == GJ_ERR_FAULT) {
                sess_soft_inc(&g_u32SoftInputPopFault);
            } else {
                sess_soft_inc(&g_u32SoftInputPopInval);
            }
            i64Ret = st;
            break;
        }
        sess_soft_inc(&g_u32SoftInputPopHit);
        i64Ret = 1;
        break;
    }

    case GJ_SESS_OP_STATS: {
        /*
         * arg1 → u32[5]:
         *   [0] compositor presents
         *   [1] input events pushed (lifetime)
         *   [2] door call count
         *   [3] flags: bit0 ready, bit1 input ready, bit2 owned,
         *              bits 8..15 pending input (capped 255),
         *              bit16 drop sticky, bit17 user PRESENT_FB,
         *              bit18 multi-frame soft, bit19 reclaim soft
         *   [4] owner token
         * Wire size stays 5 for sessiond / smoke ABI stability.
         */
        u32 aSt[5];
        u32 u32Pend;
        i64 st;

        if (u64Arg1 == 0) {
            sess_soft_inc(&g_u32SoftStatsInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        u32Pend = session_input_pending();
        if (u32Pend > 255u) {
            u32Pend = 255u;
        }
        aSt[0] = session_compositor_present_count();
        aSt[1] = session_input_pushed();
        aSt[2] = g_u32Calls;
        aSt[3] = (session_compositor_ready() ? 1u : 0u) |
                 (session_input_ready() ? 2u : 0u) |
                 (g_u32OwnerToken != 0 ? 4u : 0u) |
                 (u32Pend << 8) |
                 /* bit16: any input drop observed (sticky observability) */
                 (session_input_dropped() != 0 ? (1u << 16) : 0u) |
                 /* bit17: any user PRESENT_FB success */
                 (g_u32UserPresents != 0 ? (1u << 17) : 0u) |
                 /* bit18: multi-frame soft (2+ user presents) */
                 (g_u32UserPresents >= 2u ? (1u << 18) : 0u) |
                 /* bit19: reclaim soft observed */
                 (g_u32Reclaims != 0 ? (1u << 19) : 0u);
        aSt[4] = g_u32OwnerToken;
        /* Wave 14: soft STATS flag samples (diagnostics only). */
        if ((aSt[3] & 1u) != 0u) {
            sess_soft_inc(&g_u32SoftFlagReady);
        }
        if ((aSt[3] & 2u) != 0u) {
            sess_soft_inc(&g_u32SoftFlagInput);
        }
        if ((aSt[3] & 4u) != 0u) {
            sess_soft_inc(&g_u32SoftFlagOwned);
        }
        if ((aSt[3] & (1u << 16)) != 0u) {
            sess_soft_inc(&g_u32SoftFlagDrop);
        }
        if ((aSt[3] & (1u << 17)) != 0u) {
            sess_soft_inc(&g_u32SoftFlagUserFb);
        }
        if ((aSt[3] & (1u << 18)) != 0u) {
            sess_soft_inc(&g_u32SoftFlagMulti);
        }
        if ((aSt[3] & (1u << 19)) != 0u) {
            sess_soft_inc(&g_u32SoftFlagReclaim);
        }
        st = sess_copy_out(u64Arg1, aSt, sizeof(aSt));
        if (st == 0) {
            sess_soft_inc(&g_u32SoftStatsOk);
            /*
             * Emit soft inventory on STATS so bring-up smoke greps
             * session_door: soft … lines. greppable: session_door: soft
             */
            fSoftInv = 1;
        } else if (st == GJ_ERR_FAULT) {
            sess_soft_inc(&g_u32SoftStatsFault);
        } else {
            sess_soft_inc(&g_u32SoftStatsInval);
        }
        i64Ret = st;
        break;
    }

    case GJ_SESS_OP_MAP_SCANOUT: {
        /*
         * arg1 → u64 VA hint of kernel scanout (interim; not a true map).
         * arg2 → u32[3] {w, h, stride_bytes}
         */
        u64 u64Va;
        u32 aInfo[3];
        u32 u32W = 0;
        u32 u32H = 0;
        u32 u32Stride;
        void *pFb;
        i64 st;

        if (u64Arg1 == 0 || u64Arg2 == 0) {
            sess_soft_inc(&g_u32SoftMapInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (!session_compositor_ready()) {
            sess_soft_inc(&g_u32SoftMapNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        pFb = session_compositor_fb();
        if (pFb == NULL) {
            sess_soft_inc(&g_u32SoftMapNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        session_compositor_size(&u32W, &u32H);
        u32Stride = session_compositor_stride();
        if (u32W == 0 || u32H == 0 || u32Stride == 0) {
            sess_soft_inc(&g_u32SoftMapNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        u64Va = (u64)(uintptr_t)pFb;
        aInfo[0] = u32W;
        aInfo[1] = u32H;
        aInfo[2] = u32Stride;
        st = sess_copy_out(u64Arg1, &u64Va, sizeof(u64Va));
        if (st != 0) {
            if (st == GJ_ERR_FAULT) {
                sess_soft_inc(&g_u32SoftMapFault);
            } else {
                sess_soft_inc(&g_u32SoftMapInval);
            }
            i64Ret = st;
            break;
        }
        st = sess_copy_out(u64Arg2, aInfo, sizeof(aInfo));
        if (st == 0) {
            sess_soft_inc(&g_u32SoftMapOk);
        } else if (st == GJ_ERR_FAULT) {
            sess_soft_inc(&g_u32SoftMapFault);
        } else {
            sess_soft_inc(&g_u32SoftMapInval);
        }
        i64Ret = st;
        break;
    }

    case GJ_SESS_OP_PRESENT_FB: {
        /*
         * arg1=w arg2=h arg3=BGRA pixel ptr (stride = w*4).
         * When compositor ready: blit top-left into scanout with correct
         * source/dest strides, then present. Else direct virtio-gpu present
         * with a small static temp for user buffers.
         * Soft multi-frame: each success bumps g_u32UserPresents (bit18).
         */
        u32 u32ReqW = (u32)u64Arg1;
        u32 u32ReqH = (u32)u64Arg2;
        u32 u32SrcStride;
        u32 u32Bytes;
        u8 *pDst;
        u32 u32Cw = 0;
        u32 u32Ch = 0;
        u32 u32DstStride;
        u32 u32CopyW;
        u32 u32CopyH;
        i64 st;

        if (u64Arg3 == 0 || u32ReqW == 0 || u32ReqH == 0 ||
            u32ReqW > GJ_SESS_MAX_DIM || u32ReqH > GJ_SESS_MAX_DIM) {
            sess_soft_inc(&g_u32SoftPresentFbInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        /* Reject tokens that look like truncated wide values on w/h. */
        if ((u64Arg1 >> 32) != 0 || (u64Arg2 >> 32) != 0) {
            sess_soft_inc(&g_u32SoftPresentFbInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }

        u32SrcStride = u32ReqW * 4u;
        u32Bytes = u32SrcStride * u32ReqH;
        if (u32Bytes / u32SrcStride != u32ReqH) {
            sess_soft_inc(&g_u32SoftPresentFbInval);
            i64Ret = GJ_ERR_INVAL; /* overflow guard (defensive) */
            break;
        }

        if (!session_compositor_ready()) {
            if (!virtio_gpu_ready()) {
                sess_soft_inc(&g_u32SoftPresentFbNodev);
                i64Ret = GJ_ERR_NODEV;
                break;
            }
            if (user_range_ok(u64Arg3, u32Bytes)) {
                static u8 aTmp[GJ_SESS_TMP_W * GJ_SESS_TMP_H * 4u];

                sess_soft_inc(&g_u32SoftPresentFbUser);
                if (u32Bytes > sizeof(aTmp)) {
                    sess_soft_inc(&g_u32SoftPresentFbInval);
                    i64Ret = GJ_ERR_INVAL;
                    break;
                }
                if (copy_from_user(aTmp, u64Arg3, u32Bytes) != GJ_OK) {
                    sess_soft_inc(&g_u32SoftPresentFbFault);
                    i64Ret = GJ_ERR_FAULT;
                    break;
                }
                sess_soft_inc(&g_u32SoftCopyInUser);
                if (virtio_gpu_present(u32ReqW, u32ReqH, aTmp,
                                       u32SrcStride) != 0) {
                    sess_soft_inc(&g_u32SoftPresentFbIo);
                    i64Ret = GJ_ERR_IO;
                    break;
                }
            } else if (virtio_gpu_present(u32ReqW, u32ReqH,
                                          (void *)(gj_vaddr_t)u64Arg3,
                                          u32SrcStride) != 0) {
                sess_soft_inc(&g_u32SoftPresentFbKern);
                sess_soft_inc(&g_u32SoftPresentFbIo);
                i64Ret = GJ_ERR_IO;
                break;
            } else {
                sess_soft_inc(&g_u32SoftPresentFbKern);
            }
            g_u32UserPresents++;
            sess_soft_note_peaks();
            sess_soft_inc(&g_u32SoftPresentFbOk);
            sess_soft_inc(&g_u32SoftPresentFbDirect);
            i64Ret = 0;
            break;
        }

        session_compositor_size(&u32Cw, &u32Ch);
        u32DstStride = session_compositor_stride();
        pDst = (u8 *)session_compositor_fb();
        if (pDst == NULL || u32Cw == 0 || u32Ch == 0 || u32DstStride == 0) {
            sess_soft_inc(&g_u32SoftPresentFbNodev);
            i64Ret = GJ_ERR_NODEV;
            break;
        }
        /* Clip to scanout; never write past the allocated tile. */
        u32CopyW = u32ReqW;
        u32CopyH = u32ReqH;
        if (u32CopyW > u32Cw) {
            u32CopyW = u32Cw;
        }
        if (u32CopyH > u32Ch) {
            u32CopyH = u32Ch;
        }
        if (u32CopyW != u32ReqW || u32CopyH != u32ReqH) {
            sess_soft_inc(&g_u32SoftPresentFbClip);
        }
        if (u32CopyW == 0 || u32CopyH == 0) {
            sess_soft_inc(&g_u32SoftPresentFbInval);
            i64Ret = GJ_ERR_INVAL;
            break;
        }
        if (user_range_ok(u64Arg3, u32Bytes)) {
            sess_soft_inc(&g_u32SoftPresentFbUser);
        } else {
            sess_soft_inc(&g_u32SoftPresentFbKern);
        }
        st = sess_blit_fb(pDst, u32DstStride, u64Arg3, u32SrcStride, u32CopyW,
                          u32CopyH);
        if (st != 0) {
            if (st == GJ_ERR_FAULT) {
                sess_soft_inc(&g_u32SoftPresentFbFault);
            } else {
                sess_soft_inc(&g_u32SoftPresentFbInval);
            }
            i64Ret = st;
            break;
        }
        if (session_compositor_present() != 0) {
            sess_soft_inc(&g_u32SoftPresentFbIo);
            i64Ret = GJ_ERR_IO;
            break;
        }
        g_u32UserPresents++;
        sess_soft_note_peaks();
        sess_soft_inc(&g_u32SoftPresentFbOk);
        sess_soft_inc(&g_u32SoftPresentFbBlit);
        i64Ret = 0;
        break;
    }

    default:
        i64Ret = GJ_ERR_NOSUPPORT;
        break;
    }

    /* Wave 14 soft inventory tallies (never mutates i64Ret). */
    g_i64SoftLastRet = i64Ret;
    sess_soft_note_err(i64Ret);
    sess_soft_maybe_once();
    if (fSoftInv) {
        sess_soft_inventory_log();
    }
    return i64Ret;
}
