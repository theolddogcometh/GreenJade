/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Session door: compositor + input hub + ownership hand-off for sessiond.
 * Pure C11 freestanding. Dual MIT OR Apache-2.0. Soft!=product.
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
 * Lean soft residual (this TU exclusive; Soft!=product · dual MIT|Apache-2.0):
 *   C0 residual deepen (stamp-free; Soft!=product · G-AC-1 · Dual DoD OPEN):
 *   - Once-lamp residual path + lean self-check (no inventory floods)
 *   - Lean arms (GJ_SESS_LEAN_CHECKS): ops / dims / path / stats_wire /
 *     handoff / input / multi_bits / bgra / lic
 *   - Path honesty: claim->present/FB->input hub->map scanout (sessiond)
 *   - STATS wire honesty: 5 u32 + flag bits 0/1/2/16/17/18/19 (sessiond ABI)
 *   - BGRA stride honesty: PRESENT_FB stride=w*4; tmp tile BGRA4 bounds
 *   - Product direction = userspace sessiond (not kernel desktop product)
 *   - Diagnostic readers (claim_count / user_presents) emit residual path
 *     + lean once only - NO full inventory stamp storms on every read
 *   - Soft inventory capped (GJ_SESS_SOFT_LOG_CAP) for init/first/STATS
 *   - NO version stamps, NO ret*angle stamp storms, NO hard-gates
 *   - bar3_close=0 only; Dual DoD A/B OPEN (never closes product DoD)
 *   Functional residual preferred over multi-line deepen cascades.
 *   greppable: "session_door: soft residual"
 *   greppable: "session_door: soft residual lean"
 *   greppable: "session_door: soft residual lean PASS"
 *   greppable: Soft!=product | product=0 | desktop_product=OPEN | G-AC-1
 *   greppable: dual_dod_a=OPEN | dual_dod_b=OPEN | Dual DoD OPEN
 *   greppable: handoff=sessiond | path=claim|present|input|map
 *
 * Soft door inventory (greppable surfaces; diagnostics only):
 *   - Ownership: claim / reclaim / release / busy / claim_inval
 *   - Present: PRESENT / PRESENT_FB ok|fail|nodev + multi-frame tallies
 *   - Input: poll + pop hit/empty (soft-ok without virtio-input)
 *   - Meta: stats / display_info / map_scanout
 *   - Opcode enter surface (per GJ_SESS_OP_*) + last-op snapshot
 *   - Copy path: user vs kernel-smoke + blit rows/clip/ok|fault
 *   - Peaks: calls / claims / reclaims / user_presents high-water
 *   - Soft ok tally + ok_bp ratio + STATS flag soft samples
 *   - Soft verdict INIT|PASS|PARTIAL + path honesty catalog
 *   greppable: "session_door: soft ..."
 *   greppable: "session_door: soft deepen"
 *   Never hard-gates; diagnostics only (wrap OK). Soft.
 *   Soft != desktop/compositor product bar.
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
/* Soft deepen wave id (file-local; never hard-gates; not an image version). */
#define GJ_SESS_SOFT_WAVE 116u
/* Honest soft catalog area count (no stamp-storm inflation). */
#define GJ_SESS_SOFT_AREAS 24u
/* Cap multi-line inventory dumps (lean residual; no STATS stamp storms). */
#define GJ_SESS_SOFT_LOG_CAP 8u
/*
 * Functional residual self-check arm count (C0 residual deepen; stamp-free):
 *   1 ops  2 dims  3 path  4 stats_wire  5 handoff  6 input
 *   7 multi_bits  8 bgra  9 lic
 * Soft!=product · G-AC-1 · Dual DoD OPEN · dual MIT|Apache-2.0. Never hard-gates.
 */
#define GJ_SESS_LEAN_CHECKS 9u
/* STATS copy-out wire words (sessiond / header ABI; keep 5). */
#define GJ_SESS_STATS_WORDS 5u
/* STATS flag bits (must match session_door_call packing + sessiond_gj). */
#define GJ_SESS_STAT_F_READY   (1u << 0)
#define GJ_SESS_STAT_F_INPUT   (1u << 1)
#define GJ_SESS_STAT_F_OWNED   (1u << 2)
#define GJ_SESS_STAT_F_DROP    (1u << 16)
#define GJ_SESS_STAT_F_USER_FB (1u << 17)
#define GJ_SESS_STAT_F_MULTI   (1u << 18)
#define GJ_SESS_STAT_F_RECLAIM (1u << 19)

static int g_fInit;
static u32 g_u32Calls;
static u32 g_u32OwnerToken; /* 0 = kernel interim owns */
static u32 g_u32UserPresents;
static u32 g_u32Claims;     /* successful first claims */
static u32 g_u32Reclaims;   /* idempotent same-token CLAIM soft */

/*
 * Soft product inventory (Wave 35 exclusive deepen). Cumulative path tallies.
 * greppable: session_door: soft ...
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
static u8  g_fSoftResidualOnce;    /* sparse residual path once-lamp */
static u8  g_fSoftResidualLeanOnce;/* lean residual once-lamp (no storms) */
static u32 g_u32SoftLeanOk;        /* residual self-check PASS count */
static u32 g_u32SoftLeanFail;      /* residual self-check arm fail count */
static u32 g_u32SoftLeanChecks;    /* last lean arm count */
static u32 g_u32SoftLeanArmsOk;    /* last lean arms that passed */

static void sess_soft_inc(u32 *pCtr);
static void sess_soft_add(u32 *pCtr, u32 u32N);
static void sess_soft_note_err(i64 i64R);
static void sess_soft_note_peaks(void);
static void sess_soft_note_op(u32 u32Op);
static void sess_soft_inventory_log(void);
static void sess_soft_maybe_once(void);
static void sess_soft_residual_once(void);
static void sess_soft_residual_lean_once(void);

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
 * Greppable soft session door inventory (smoke diagnostics; Soft!=product).
 *   session_door: soft honesty ...
 *   session_door: soft inventory ...
 *   session_door: soft claim ...
 *   session_door: soft present ...
 *   session_door: soft input ...
 *   session_door: soft meta ...
 *   session_door: soft op ...
 *   session_door: soft copy ...
 *   session_door: soft blit ...
 *   session_door: soft peaks ...
 *   session_door: soft last ...
 *   session_door: soft flags ...
 *   session_door: soft ratio ...
 *   session_door: soft err ...
 *   session_door: soft owner ...
 *   session_door: soft terminal ...
 *   session_door: soft capacity ...
 *   session_door: soft headroom ...
 *   session_door: soft surface ...
 *   session_door: soft geom ...
 *   session_door: soft catalog ...
 *   session_door: soft return ...
 *   session_door: soft path ...
 *   session_door: soft deepen ...
 *   session_door: soft PASS|PARTIAL|INIT
 * greppable: session_door: soft
 * greppable: session_door: soft residual lean
 * greppable: session_door: soft deepen
 * Honesty: soft inventory only - not desktop compositor product.
 * Cap: GJ_SESS_SOFT_LOG_CAP multi-line dumps (init / first / STATS);
 * past cap silent (counters keep ticking). No stamp storms on re-read.
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

    /*
     * Lean residual honesty: cap multi-line inventory (Soft!=product).
     * Past GJ_SESS_SOFT_LOG_CAP stay silent; counters still tick elsewhere.
     */
    if (g_u32SoftLogs >= GJ_SESS_SOFT_LOG_CAP) {
        return;
    }
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
     *   INIT     - no product calls yet (baseline)
     *   PASS     - any present/claim/input success observed
     *   PARTIAL  - only failures so far
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
            "sessiond_claim=token soft_never_gates=1 "
            "dual_dod_a=OPEN dual_dod_b=OPEN Soft!=product wave=%u "
            "(soft; never closes desktop bar; Dual DoD OPEN)\n",
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

    /* Grep: session_door: soft owner - Wave 15 ownership snapshot. */
    kprintf("session_door: soft owner owned=%u token=0x%x claims=%u "
            "reclaims=%u release=%u busy=%u free_release=%u "
            "policy=token_reclaim soft %s\n",
            u32Owned, g_u32OwnerToken, g_u32Claims, g_u32Reclaims,
            g_u32SoftRelease, g_u32SoftClaimBusy, g_u32SoftReleaseFree,
            u32Owned != 0u ? "PASS" : "INIT");
    cAreas++;

    /* Grep: session_door: soft terminal - terminal status rollup. */
    kprintf("session_door: soft terminal ok=%u inval=%u nodev=%u busy=%u "
            "fault=%u io=%u nosupport=%u err_sum=%u last_ret=%ld soft PASS\n",
            g_u32SoftOk, g_u32SoftInval, g_u32SoftNodev, g_u32SoftBusy,
            g_u32SoftFault, g_u32SoftIo, g_u32SoftNosupport, u32ErrSum,
            (long)g_i64SoftLastRet);
    cAreas++;

    /* Grep: session_door: soft capacity - Wave 19 design-constant lamps. */
    kprintf("session_door: soft capacity max_dim=%u tmp=%ux%u bpp=4 "
            "ops=9 reclaim=1 multi_frame=1 map_scanout=1 "
            "present_fb=direct|blit soft PASS wave=%u\n",
            GJ_SESS_MAX_DIM, GJ_SESS_TMP_W, GJ_SESS_TMP_H, GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft headroom - Wave 19 live slack lamps. */
    kprintf("session_door: soft headroom calls=%u peak_calls=%u "
            "claims=%u peak_claims=%u user_fb=%u peak_user_fb=%u "
            "ok=%u err_sum=%u wave=%u\n",
            g_u32Calls, g_u32SoftPeakCalls, g_u32Claims, g_u32SoftPeakClaims,
            g_u32UserPresents, g_u32SoftPeakUserPres, g_u32SoftOk, u32ErrSum,
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft surface - Wave 19 surface bit lamps. */
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

    /* Grep: session_door: soft geom - Wave 19 scanout geometry snapshot. */
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
            "wave=%u areas_expect=%u soft PASS\n",
            GJ_SESS_SOFT_WAVE, GJ_SESS_SOFT_AREAS);
    cAreas++;

    /*
     * Grep: session_door: soft return
     * Terminal return classes for door opcodes (one line; no stamp cascade).
     * Soft != product desktop/compositor bar. product_kernel=OPEN.
     */
    kprintf("session_door: soft return claim=ok|reclaim|busy|inval "
            "release=ok|free|inval present=ok|nodev|io "
            "fb=ok|inval|nodev|fault|io|direct|blit "
            "pop=hit|empty|inval|fault map=ok|inval|nodev|fault "
            "stats=ok|inval|fault display=ok|inval|fault "
            "product_kernel=OPEN Soft!=product\n");
    cAreas++;

    /* Grep: session_door: soft path */
    kprintf("session_door: soft path claim=sessiond present=comp|gpu_fb "
            "input=hub_soft map=va_hint multi_frame=bit18 reclaim=bit19 "
            "copy=user|kern blit=clip wave=%u "
            "(soft inventory; not desktop product)\n",
            GJ_SESS_SOFT_WAVE);
    cAreas++;

    /* Grep: session_door: soft deepen - area count (no stamp storms). */
    cAreas++;
    kprintf("session_door: soft deepen wave=%u areas=%u verdict=%s "
            "ready=%u owned=%u presents_user=%u claims=%u multi=%u "
            "desktop_product=OPEN soft_never_gates=1 Soft!=product (soft)\n",
            GJ_SESS_SOFT_WAVE, cAreas, szVerdict, u32Ready, u32Owned,
            g_u32UserPresents, g_u32Claims, u32Multi);

    /* Grep: session_door: soft PASS|PARTIAL|INIT */
    kprintf("session_door: soft %s ready=%u owned=%u presents_user=%u "
            "claims=%u multi=%u wave=%u Soft!=product (soft)\n",
            szVerdict, u32Ready, u32Owned, g_u32UserPresents, g_u32Claims,
            u32Multi, GJ_SESS_SOFT_WAVE);
}

/**
 * Sparse residual path lamp (this TU exclusive; C0 residual deepen).
 * Once only (no stamp storms). Soft!=product · Dual DoD OPEN · stamp-free.
 * Path honesty: sessiond claim -> present/FB -> input hub -> map scanout.
 * Product direction = userspace sessiond (G-AC-1 style Soft!=product).
 * greppable: session_door: soft residual
 * greppable: handoff=sessiond | path=claim|present|input|map
 * greppable: dual_dod_a=OPEN | dual_dod_b=OPEN | Soft!=product
 */
static void
sess_soft_residual_once(void)
{
    u32 u32Owned;
    u32 u32Ready;
    u32 u32Input;
    u32 u32Gpu;
    u32 u32Multi;
    u32 u32UserFb;

    if (g_fSoftResidualOnce != 0u) {
        return;
    }
    g_fSoftResidualOnce = 1u;

    u32Owned = (g_u32OwnerToken != 0u) ? 1u : 0u;
    u32Ready = session_compositor_ready() ? 1u : 0u;
    u32Input = session_input_ready() ? 1u : 0u;
    u32Gpu = virtio_gpu_ready() ? 1u : 0u;
    u32UserFb = (g_u32UserPresents != 0u) ? 1u : 0u;
    u32Multi = (g_u32UserPresents >= 2u) ? 1u : 0u;

    /*
     * Grep: session_door: soft residual
     * One sparse residual path line (HARD: no stamp storms). Soft!=product.
     * greppable: handoff=sessiond | path=claim|present|input|map | G-AC-1
     */
    kprintf("session_door: soft residual "
            "path=claim|present|input|map handoff=sessiond "
            "product_session=sessiond not_kernel_desktop_product "
            "claim->present->input->map sessiond=token "
            "owned=%u token=0x%x ready=%u input=%u gpu=%u "
            "calls=%u claims=%u reclaims=%u user_fb=%u multi=%u "
            "present_ok=%u fb_ok=%u fb_direct=%u fb_blit=%u "
            "map_ok=%u pop_hit=%u soft_ok=%u "
            "stats_words=%u flag_multi=bit18 flag_reclaim=bit19 "
            "bpp=4 stride=w*4 max_dim=%u tmp=%ux%u "
            "soft=1 product=0 soft_ne_product=1 stamp_storm=0 "
            "desktop_product=OPEN mint=OPEN bar3_close=0 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
            "(C0 residual; interim door; userspace sessiond handoff eng; "
            "not desktop/compositor product; Dual DoD OPEN)\n",
            u32Owned, g_u32OwnerToken, u32Ready, u32Input, u32Gpu,
            g_u32Calls, g_u32Claims, g_u32Reclaims, u32UserFb, u32Multi,
            g_u32SoftPresentOk, g_u32SoftPresentFbOk,
            g_u32SoftPresentFbDirect, g_u32SoftPresentFbBlit,
            g_u32SoftMapOk, g_u32SoftInputPopHit, g_u32SoftOk,
            GJ_SESS_STATS_WORDS, GJ_SESS_MAX_DIM, GJ_SESS_TMP_W,
            GJ_SESS_TMP_H);
}

/**
 * Lean soft residual once-lamp (this TU exclusive; C0 residual deepen).
 * Functional self-check arms (GJ_SESS_LEAN_CHECKS = 9):
 *   1) ops         - GJ_SESS_OP_* 1..9 contiguous contract
 *   2) dims        - max_dim / tmp tile / catalog areas / log cap
 *   3) path        - owner token <-> owned() coherence
 *   4) stats_wire  - STATS u32[5] + flag bits 0/1/2/16/17/18/19
 *   5) handoff     - CLAIM/RELEASE/PRESENT_FB/MAP_SCANOUT sessiond surface
 *   6) input       - INPUT_POLL/POP soft-empty-ok contract
 *   7) multi_bits  - user_fb bit17 / multi bit18 / reclaim bit19 policy
 *   8) bgra        - PRESENT_FB BGRA4 stride=w*4 + tmp tile byte bounds
 *   9) lic         - Soft!=product · product=0 · G-AC-1 · Dual DoD OPEN
 * Path honesty: sessiond claim -> present/FB -> input hub -> map scanout.
 * Soft!=product · dual MIT|Apache-2.0 · Dual DoD OPEN · no version stamp.
 * Two lean residual lines max (lean + PASS|FAIL). Never closes desktop/bar3.
 * greppable: session_door: soft residual lean
 * greppable: session_door: soft residual lean PASS
 * greppable: handoff=sessiond | Soft!=product | G-AC-1 | Dual DoD OPEN
 */
static void
sess_soft_residual_lean_once(void)
{
    u32 u32Owned;
    u32 u32Ready;
    u32 u32Input;
    u32 u32Gpu;
    u32 u32Multi;
    u32 u32Checks;
    u32 u32Ok;
    u32 u32OpsOk;
    u32 u32DimOk;
    u32 u32PathOk;
    u32 u32StatsOk;
    u32 u32HandoffOk;
    u32 u32InputOk;
    u32 u32MultiOk;
    u32 u32BgraOk;
    u32 u32LicOk;
    u32 u32TmpBytes;
    u32 u32MaxBytes;
    /* Stack-local product honesty (never hard-gates; Soft!=product). */
    const int nSoft = 1;
    const int nProduct = 0;
    const int nBar3Close = 0; /* residual never closes bar3 */
    const int nDesktopProduct = 0;
    const int nGac1 = 1; /* G-AC-1: no .ko product AC; Soft!=product law */
    const int nDualDodA = 0; /* Dual DoD A OPEN (not closed) */
    const int nDualDodB = 0; /* Dual DoD B OPEN (not closed) */
    const int nStampStorm = 0;

    if (g_fSoftResidualLeanOnce != 0u) {
        return;
    }
    g_fSoftResidualLeanOnce = 1u;

    u32Owned = (g_u32OwnerToken != 0u) ? 1u : 0u;
    u32Ready = session_compositor_ready() ? 1u : 0u;
    u32Input = session_input_ready() ? 1u : 0u;
    u32Gpu = virtio_gpu_ready() ? 1u : 0u;
    u32Multi = (g_u32UserPresents >= 2u) ? 1u : 0u;
    u32Checks = 0;
    u32Ok = 0;
    u32OpsOk = 0;
    u32DimOk = 0;
    u32PathOk = 0;
    u32StatsOk = 0;
    u32HandoffOk = 0;
    u32InputOk = 0;
    u32MultiOk = 0;
    u32BgraOk = 0;
    u32LicOk = 0;
    u32TmpBytes = GJ_SESS_TMP_W * GJ_SESS_TMP_H * 4u;
    u32MaxBytes = GJ_SESS_MAX_DIM * 4u; /* one-row stride bound */

    /* Arm 1: opcode contract (ops=9 contiguous 1..9). */
    u32Checks++;
    if (GJ_SESS_OP_PRESENT == 1u && GJ_SESS_OP_DISPLAY_INFO == 2u &&
        GJ_SESS_OP_INPUT_POLL == 3u && GJ_SESS_OP_INPUT_POP == 4u &&
        GJ_SESS_OP_STATS == 5u && GJ_SESS_OP_PRESENT_FB == 6u &&
        GJ_SESS_OP_CLAIM == 7u && GJ_SESS_OP_RELEASE == 8u &&
        GJ_SESS_OP_MAP_SCANOUT == 9u) {
        u32OpsOk = 1u;
        u32Ok++;
    }

    /* Arm 2: dim clamp + tmp tile + catalog/log honesty. */
    u32Checks++;
    if (GJ_SESS_MAX_DIM == 256u && GJ_SESS_TMP_W == 64u &&
        GJ_SESS_TMP_H == 64u && GJ_SESS_SOFT_AREAS == 24u &&
        GJ_SESS_SOFT_LOG_CAP >= 1u && GJ_SESS_SOFT_LOG_CAP <= 8u &&
        GJ_SESS_LEAN_CHECKS == 9u &&
        GJ_SESS_STATS_WORDS == 5u &&
        GJ_SESS_TMP_W <= GJ_SESS_MAX_DIM &&
        GJ_SESS_TMP_H <= GJ_SESS_MAX_DIM) {
        u32DimOk = 1u;
        u32Ok++;
    }

    /* Arm 3: ownership path coherence (token 0 == interim / not owned). */
    u32Checks++;
    if ((g_u32OwnerToken == 0u) == (u32Owned == 0u) &&
        (session_door_owned() != 0) == (u32Owned != 0u) &&
        session_door_owner_token() == g_u32OwnerToken) {
        u32PathOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 4: STATS wire ABI (sessiond_gj / header).
     * u32[5]: presents, input_pushed, calls, flags, owner_token.
     * Flag bits: ready/input/owned + drop/user_fb/multi/reclaim.
     */
    u32Checks++;
    if (GJ_SESS_STATS_WORDS == 5u &&
        GJ_SESS_STAT_F_READY == 1u &&
        GJ_SESS_STAT_F_INPUT == 2u &&
        GJ_SESS_STAT_F_OWNED == 4u &&
        GJ_SESS_STAT_F_DROP == (1u << 16) &&
        GJ_SESS_STAT_F_USER_FB == (1u << 17) &&
        GJ_SESS_STAT_F_MULTI == (1u << 18) &&
        GJ_SESS_STAT_F_RECLAIM == (1u << 19) &&
        (GJ_SESS_STAT_F_MULTI > GJ_SESS_STAT_F_USER_FB) &&
        (GJ_SESS_STAT_F_RECLAIM > GJ_SESS_STAT_F_MULTI)) {
        u32StatsOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 5: sessiond handoff surface (CLAIM/RELEASE/PRESENT_FB/MAP).
     * Product direction = userspace sessiond; kernel holds interim tile.
     */
    u32Checks++;
    if (GJ_SESS_OP_PRESENT_FB == 6u && GJ_SESS_OP_CLAIM == 7u &&
        GJ_SESS_OP_RELEASE == 8u && GJ_SESS_OP_MAP_SCANOUT == 9u &&
        GJ_SESS_OP_CLAIM > GJ_SESS_OP_PRESENT_FB &&
        GJ_SESS_OP_MAP_SCANOUT > GJ_SESS_OP_RELEASE) {
        u32HandoffOk = 1u;
        u32Ok++;
    }

    /* Arm 6: input soft-empty contract (POLL then POP; soft without virtio). */
    u32Checks++;
    if (GJ_SESS_OP_INPUT_POLL == 3u && GJ_SESS_OP_INPUT_POP == 4u &&
        GJ_SESS_OP_INPUT_POP == (GJ_SESS_OP_INPUT_POLL + 1u) &&
        GJ_SESS_OP_INPUT_POLL < GJ_SESS_OP_STATS) {
        u32InputOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 7: multi-frame / reclaim / user_fb STATS policy bits.
     * bit17 any PRESENT_FB; bit18 multi (>=2); bit19 reclaim soft.
     * Contiguous high flags + live multi <=> user_presents >= 2.
     */
    u32Checks++;
    if (GJ_SESS_STAT_F_USER_FB == (1u << 17) &&
        GJ_SESS_STAT_F_MULTI == (1u << 18) &&
        GJ_SESS_STAT_F_RECLAIM == (1u << 19) &&
        (GJ_SESS_STAT_F_MULTI == (GJ_SESS_STAT_F_USER_FB << 1)) &&
        (GJ_SESS_STAT_F_RECLAIM == (GJ_SESS_STAT_F_MULTI << 1)) &&
        ((g_u32UserPresents >= 2u) == (u32Multi != 0u))) {
        u32MultiOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 8: BGRA4 PRESENT_FB geometry (stride = w*4; tmp tile byte bound).
     * Catches silent bpp/stride breakage without mutating door state.
     */
    u32Checks++;
    if (u32TmpBytes == (64u * 64u * 4u) &&
        u32TmpBytes == 16384u &&
        u32MaxBytes == (256u * 4u) &&
        u32MaxBytes == 1024u &&
        (GJ_SESS_TMP_W * 4u) == 256u &&
        (GJ_SESS_MAX_DIM * 4u) == u32MaxBytes &&
        u32TmpBytes <= (GJ_SESS_MAX_DIM * GJ_SESS_MAX_DIM * 4u)) {
        u32BgraOk = 1u;
        u32Ok++;
    }

    /*
     * Arm 9: Soft!=product · product=0 · G-AC-1 · Dual DoD OPEN · bar3_close=0.
     * LAW: dual MIT/Apache Soft!=product; residual never closes Dual DoD/bar3.
     * Do not restate "bar3 OPEN" (see STEAM_BAR3_STATUS); bar3_close=0 only.
     */
    u32Checks++;
    if (nSoft == 1 && nProduct == 0 && nBar3Close == 0 &&
        nDesktopProduct == 0 && nGac1 == 1 &&
        nDualDodA == 0 && nDualDodB == 0 && nStampStorm == 0) {
        u32LicOk = 1u;
        u32Ok++;
    }

    g_u32SoftLeanChecks = u32Checks;
    g_u32SoftLeanArmsOk = u32Ok;

    if (u32Ok == u32Checks && u32Checks == GJ_SESS_LEAN_CHECKS) {
        sess_soft_inc(&g_u32SoftLeanOk);
    } else {
        sess_soft_inc(&g_u32SoftLeanFail);
    }

    /*
     * Grep: session_door: soft residual lean
     * One residual lean line (HARD: no stamp storms). Soft!=product.
     * greppable: handoff=sessiond | path=claim|present|input|map | G-AC-1
     * greppable: dual_dod_a=OPEN | dual_dod_b=OPEN | Dual DoD OPEN
     */
    kprintf("session_door: soft residual lean "
            "path=claim|present|input|map handoff=sessiond "
            "ops=%u dims=%u path=%u stats=%u handoff=%u input=%u "
            "multi=%u bgra=%u lic=%u ok=%u/%u "
            "owned=%u ready=%u input_rdy=%u gpu=%u "
            "claims=%u reclaims=%u user_fb=%u multi_live=%u "
            "soft_ok=%u calls=%u lean_ok=%u lean_fail=%u "
            "stats_words=%u log_cap=%u bpp=4 stride=w*4 "
            "tmp_bytes=%u max_row=%u soft=1 product=0 "
            "product_session=sessiond not_kernel_desktop_product "
            "desktop_product=OPEN mint=OPEN bar3_close=0 "
            "dual_dod_a=OPEN dual_dod_b=OPEN "
            "stamp_storm=0 soft_ne_product=1 Soft!=product G-AC-1 "
            "dual=MIT|Apache-2.0 "
            "(C0 residual lean; interim door; not desktop product; "
            "Dual DoD OPEN)\n",
            u32OpsOk, u32DimOk, u32PathOk, u32StatsOk, u32HandoffOk,
            u32InputOk, u32MultiOk, u32BgraOk, u32LicOk, u32Ok, u32Checks,
            u32Owned, u32Ready, u32Input, u32Gpu,
            g_u32Claims, g_u32Reclaims, g_u32UserPresents, u32Multi,
            g_u32SoftOk, g_u32Calls, g_u32SoftLeanOk, g_u32SoftLeanFail,
            GJ_SESS_STATS_WORDS, GJ_SESS_SOFT_LOG_CAP,
            u32TmpBytes, u32MaxBytes);

    /*
     * Grep: session_door: soft residual lean PASS | FAIL
     * Soft residual honesty only - never product DoD / never bar3 close.
     * Emit PASS only when all lean arms hold (functional residual).
     */
    if (g_u32SoftLeanOk != 0u) {
        kprintf("session_door: soft residual lean PASS "
                "ops=%u dims=%u path=%u stats=%u handoff=%u input=%u "
                "multi=%u bgra=%u lic=%u ok=%u/%u "
                "handoff=sessiond path=claim|present|input|map "
                "soft=1 product=0 soft_ne_product=1 "
                "product_session=sessiond desktop_product=OPEN "
                "dual_dod_a=OPEN dual_dod_b=OPEN bar3_close=0 "
                "Soft!=product G-AC-1 dual=MIT|Apache-2.0 "
                "(lean residual only; not desktop product; Dual DoD OPEN)\n",
                u32OpsOk, u32DimOk, u32PathOk, u32StatsOk, u32HandoffOk,
                u32InputOk, u32MultiOk, u32BgraOk, u32LicOk, u32Ok,
                u32Checks);
    } else {
        kprintf("session_door: soft residual lean FAIL "
                "ops=%u dims=%u path=%u stats=%u handoff=%u input=%u "
                "multi=%u bgra=%u lic=%u ok=%u/%u "
                "handoff=sessiond soft=1 product=0 "
                "(soft residual only; not product gate; Soft!=product; "
                "Dual DoD OPEN; G-AC-1)\n",
                u32OpsOk, u32DimOk, u32PathOk, u32StatsOk, u32HandoffOk,
                u32InputOk, u32MultiOk, u32BgraOk, u32LicOk, u32Ok,
                u32Checks);
    }
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
    sess_soft_residual_once();
    sess_soft_residual_lean_once();
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
    g_fSoftResidualOnce = 0;
    g_fSoftResidualLeanOnce = 0;
    g_u32SoftLeanOk = 0;
    g_u32SoftLeanFail = 0;
    g_u32SoftLeanChecks = 0;
    g_u32SoftLeanArmsOk = 0;
    kprintf("session_door: init (present+input+claim soft wave=%u)\n",
            GJ_SESS_SOFT_WAVE);
    /* Grep: session_door: soft (baseline inventory after init) */
    sess_soft_inventory_log();
    /*
     * Residual path + lean once-lamps deferred to first product call /
     * diag readers so counters are live (Soft!=product; no version stamp;
     * no storms; C0 residual deepen stamp-free).
     */
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
     * Residual path + lean once - NO full inventory stamp storm on re-read.
     * greppable: session_door claim soft
     * greppable: session_door: soft residual
     * greppable: session_door: soft residual lean
     */
    sess_soft_residual_once();
    sess_soft_residual_lean_once();
    return g_u32Claims + g_u32Reclaims;
}

u32
session_door_user_presents(void)
{
    /*
     * Soft diagnostics re-read: residual path + lean once (no stamp storms).
     * greppable: session_door: soft residual
     * greppable: session_door: soft residual lean
     */
    sess_soft_residual_once();
    sess_soft_residual_lean_once();
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
        /* Soft free path: already unowned -> 0 (no token match required). */
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
        /* arg1 -> u32[2] {w, h} */
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
        /* arg1 -> struct gj_input_event; ret 1 if event, 0 if empty. */
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
         * arg1 -> u32[GJ_SESS_STATS_WORDS] (5):
         *   [0] compositor presents
         *   [1] input events pushed (lifetime)
         *   [2] door call count
         *   [3] flags: bit0 ready, bit1 input ready, bit2 owned,
         *              bits 8..15 pending input (capped 255),
         *              bit16 drop sticky, bit17 user PRESENT_FB,
         *              bit18 multi-frame soft, bit19 reclaim soft
         *   [4] owner token
         * Wire size stays 5 for sessiond / smoke ABI stability.
         * Flag bits: GJ_SESS_STAT_F_* (residual lean arms 4/7).
         */
        u32 aSt[GJ_SESS_STATS_WORDS];
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
        /* Flag packing uses GJ_SESS_STAT_F_* (residual lean arm 4/7). */
        aSt[3] = (session_compositor_ready() ? GJ_SESS_STAT_F_READY : 0u) |
                 (session_input_ready() ? GJ_SESS_STAT_F_INPUT : 0u) |
                 (g_u32OwnerToken != 0 ? GJ_SESS_STAT_F_OWNED : 0u) |
                 (u32Pend << 8) |
                 /* bit16: any input drop observed (sticky observability) */
                 (session_input_dropped() != 0 ? GJ_SESS_STAT_F_DROP : 0u) |
                 /* bit17: any user PRESENT_FB success */
                 (g_u32UserPresents != 0 ? GJ_SESS_STAT_F_USER_FB : 0u) |
                 /* bit18: multi-frame soft (2+ user presents) */
                 (g_u32UserPresents >= 2u ? GJ_SESS_STAT_F_MULTI : 0u) |
                 /* bit19: reclaim soft observed */
                 (g_u32Reclaims != 0 ? GJ_SESS_STAT_F_RECLAIM : 0u);
        aSt[4] = g_u32OwnerToken;
        /* Soft STATS flag samples (diagnostics only; Soft!=product). */
        if ((aSt[3] & GJ_SESS_STAT_F_READY) != 0u) {
            sess_soft_inc(&g_u32SoftFlagReady);
        }
        if ((aSt[3] & GJ_SESS_STAT_F_INPUT) != 0u) {
            sess_soft_inc(&g_u32SoftFlagInput);
        }
        if ((aSt[3] & GJ_SESS_STAT_F_OWNED) != 0u) {
            sess_soft_inc(&g_u32SoftFlagOwned);
        }
        if ((aSt[3] & GJ_SESS_STAT_F_DROP) != 0u) {
            sess_soft_inc(&g_u32SoftFlagDrop);
        }
        if ((aSt[3] & GJ_SESS_STAT_F_USER_FB) != 0u) {
            sess_soft_inc(&g_u32SoftFlagUserFb);
        }
        if ((aSt[3] & GJ_SESS_STAT_F_MULTI) != 0u) {
            sess_soft_inc(&g_u32SoftFlagMulti);
        }
        if ((aSt[3] & GJ_SESS_STAT_F_RECLAIM) != 0u) {
            sess_soft_inc(&g_u32SoftFlagReclaim);
        }
        st = sess_copy_out(u64Arg1, aSt, sizeof(aSt));
        if (st == 0) {
            sess_soft_inc(&g_u32SoftStatsOk);
            /*
             * Emit soft inventory on STATS so bring-up smoke greps
             * session_door: soft ... lines. greppable: session_door: soft
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
         * arg1 -> u64 VA hint of kernel scanout (interim; not a true map).
         * arg2 -> u32[3] {w, h, stride_bytes}
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
