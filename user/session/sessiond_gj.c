/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding sessiond — session door ownership live path:
 *   CLAIM → DISPLAY_INFO → PRESENT_FB → soft health → RELEASE
 * Built as user ELF for embed/spawn (kernel/proc/sessiond_embed.S).
 * Host A1 smoke remains sessiond.c (libc).
 *
 * Hard smoke markers (must stay prefix-stable for scripts/smoke-all.sh):
 *   sessiond-gj: CLAIM PASS
 *   sessiond-gj: PRESENT_FB PASS
 *   sessiond-gj: RELEASE PASS
 *   sessiond-gj: ownership path PASS
 *
 * Soft (optional; never fails ownership path):
 *   sessiond-gj: DISPLAY_INFO PASS | DISPLAY_INFO soft
 *   sessiond-gj: MAP_SCANOUT soft | MAP_SCANOUT soft-skip
 *   sessiond-gj: INPUT soft | INPUT soft-skip
 *   sessiond-gj: reclaim soft | reclaim soft-skip
 *   sessiond-gj: ready soft | input-ready soft
 *   sessiond-gj: user-fb soft | multi-frame soft
 *   sessiond-gj: STATS soft …
 *   sessiond-gj: soft health PASS | soft health soft-skip
 *   sessiond-gj: soft PRESENT_FB PASS | soft PRESENT soft-skip
 *
 * Soft health deepens door surface beyond hard claim/present/release:
 *   DISPLAY_INFO recheck, MAP_SCANOUT geometry, INPUT_POLL/POP drain,
 *   same-token CLAIM reclaim (STATS bit19), STATS ownership/ready flags,
 *   interim PRESENT, second PRESENT_FB (multi-frame bit18 / user-fb bit17).
 *
 * Door opcodes (do not renumber — match kernel/include/gj/session_door.h):
 *   1 PRESENT  2 DISPLAY_INFO  3 INPUT_POLL  4 INPUT_POP
 *   5 STATS    6 PRESENT_FB    7 CLAIM       8 RELEASE  9 MAP_SCANOUT
 *
 * STATS flags (door contract; wire size stays u32[5]):
 *   bit0 ready  bit1 input ready  bit2 owned
 *   bits8..15 pending input (0..255)
 *   bit16 drop sticky  bit17 user PRESENT_FB  bit18 multi-frame  bit19 reclaim
 *
 *   make sessiond-gj → build/user/sessiond.elf
 */
#include <gj/string.h>
#include <gj/syscalls.h>

/* Prefer libgj GJ_SESS_OP_*; local aliases keep the file self-describing. */
#ifndef GJ_SESS_OP_PRESENT
#define GJ_SESS_OP_PRESENT      1u
#define GJ_SESS_OP_DISPLAY_INFO 2u
#define GJ_SESS_OP_INPUT_POLL   3u
#define GJ_SESS_OP_INPUT_POP    4u
#define GJ_SESS_OP_STATS        5u
#define GJ_SESS_OP_PRESENT_FB   6u
#define GJ_SESS_OP_CLAIM        7u
#define GJ_SESS_OP_RELEASE      8u
#define GJ_SESS_OP_MAP_SCANOUT  9u
#endif

/* Fixed BGRA tile presented through GJ_SESS_OP_PRESENT_FB (kernel max 256). */
#define GJ_SESS_FB_W 64u
#define GJ_SESS_FB_H 64u

/* Session door ownership token (host sim uses the same value). */
#define SESS_TOKEN 0xc0ffe1u

/* STATS flags — match kernel session_door.h / session_door_call. */
#define SESS_STAT_F_READY    1u
#define SESS_STAT_F_INPUT    2u
#define SESS_STAT_F_OWNED    4u
#define SESS_STAT_PEND_SHIFT 8u
#define SESS_STAT_PEND_MASK  0xffu
#define SESS_STAT_F_DROP     (1u << 16)
#define SESS_STAT_F_USER_FB  (1u << 17)
#define SESS_STAT_F_MULTI    (1u << 18)
#define SESS_STAT_F_RECLAIM  (1u << 19)

/* Soft INPUT_POP drain cap (defensive; empty ring is fine). */
#define SESS_SOFT_POP_CAP 64u

/* Matches kernel struct gj_input_event layout (type, code, value). */
struct sess_input_ev {
    unsigned short u16Type;
    unsigned short u16Code;
    int i32Value;
};

static unsigned g_uToken;

static void
msg(const char *sz)
{
    if (sz == 0) {
        return;
    }
    (void)gj_debug_log(sz, (long)gj_strlen(sz));
}

/* Append decimal digits of u into aLine at *po; leave room for NUL. */
static void
append_u(char *aLine, unsigned cb, unsigned *po, unsigned long u)
{
    char aDig[20];
    unsigned n = 0;
    unsigned i;

    if (aLine == 0 || po == 0 || cb == 0) {
        return;
    }
    if (u == 0) {
        aDig[n++] = '0';
    } else {
        while (u > 0 && n < sizeof(aDig)) {
            aDig[n++] = (char)('0' + (u % 10ul));
            u /= 10ul;
        }
    }
    for (i = n; i > 0 && *po + 1 < cb; i--) {
        aLine[(*po)++] = aDig[i - 1u];
    }
}

/* Append lowercase hex of u (no 0x prefix). */
static void
append_hex(char *aLine, unsigned cb, unsigned *po, unsigned long u)
{
    char aHex[16];
    unsigned n = 0;
    unsigned i;

    if (aLine == 0 || po == 0 || cb == 0) {
        return;
    }
    if (u == 0) {
        aHex[n++] = '0';
    } else {
        while (u > 0 && n < sizeof(aHex)) {
            unsigned d = (unsigned)(u & 0xful);

            aHex[n++] = (char)(d < 10u ? ('0' + d) : ('a' + (d - 10u)));
            u >>= 4;
        }
    }
    for (i = n; i > 0 && *po + 1 < cb; i--) {
        aLine[(*po)++] = aHex[i - 1u];
    }
}

static void
append_s(char *aLine, unsigned cb, unsigned *po, const char *sz)
{
    if (aLine == 0 || po == 0 || sz == 0 || cb == 0) {
        return;
    }
    while (*sz != '\0' && *po + 1 < cb) {
        aLine[(*po)++] = *sz++;
    }
}

/* "sessiond-gj: <tag> fail ret=<n>\n" */
static void
msg_fail_ret(const char *szTag, long nRet)
{
    char aLine[80];
    unsigned o = 0;

    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: ");
    append_s(aLine, sizeof(aLine), &o, szTag);
    append_s(aLine, sizeof(aLine), &o, " fail ret=");
    if (nRet < 0) {
        append_s(aLine, sizeof(aLine), &o, "-");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)(-nRet));
    } else {
        append_u(aLine, sizeof(aLine), &o, (unsigned long)nRet);
    }
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/*
 * "sessiond-gj: STATS soft p= in= c= f= own=\n"
 * aSt layout: [0] presents [1] in_push [2] calls [3] flags [4] owner
 */
static void
msg_stats_soft(const unsigned *aSt)
{
    char aLine[120];
    unsigned o = 0;

    if (aSt == 0) {
        return;
    }
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: STATS soft p=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[0]);
    append_s(aLine, sizeof(aLine), &o, " in=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[1]);
    append_s(aLine, sizeof(aLine), &o, " c=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[2]);
    append_s(aLine, sizeof(aLine), &o, " f=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[3]);
    append_s(aLine, sizeof(aLine), &o, " own=0x");
    append_hex(aLine, sizeof(aLine), &o, (unsigned long)aSt[4]);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/* "sessiond-gj: MAP_SCANOUT soft WxH stride=S\n" */
static void
msg_map_soft(unsigned u32W, unsigned u32H, unsigned u32Stride)
{
    char aLine[96];
    unsigned o = 0;

    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: MAP_SCANOUT soft ");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)u32W);
    append_s(aLine, sizeof(aLine), &o, "x");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)u32H);
    append_s(aLine, sizeof(aLine), &o, " stride=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)u32Stride);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/* "sessiond-gj: DISPLAY_INFO soft WxH\n" (soft-health recheck). */
static void
msg_display_soft(unsigned u32W, unsigned u32H)
{
    char aLine[80];
    unsigned o = 0;

    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: DISPLAY_INFO soft ");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)u32W);
    append_s(aLine, sizeof(aLine), &o, "x");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)u32H);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/* "sessiond-gj: INPUT soft drained=N pend=P\n" */
static void
msg_input_soft(unsigned cPop, unsigned cPend)
{
    char aLine[80];
    unsigned o = 0;

    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: INPUT soft drained=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cPop);
    append_s(aLine, sizeof(aLine), &o, " pend=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cPend);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

static void
fail_exit(const char *szWhy)
{
    msg(szWhy);
    if (g_uToken != 0u) {
        (void)gj_session_release(g_uToken);
        g_uToken = 0u;
    }
    gj_exit(1);
}

static void
fb_fill(unsigned char *pFb, unsigned char b, unsigned char g, unsigned char r)
{
    unsigned iPix;

    if (pFb == 0) {
        return;
    }
    for (iPix = 0; iPix < GJ_SESS_FB_W * GJ_SESS_FB_H; iPix++) {
        pFb[iPix * 4u + 0] = b;
        pFb[iPix * 4u + 1] = g;
        pFb[iPix * 4u + 2] = r;
        pFb[iPix * 4u + 3] = 0xff;
    }
}

/*
 * Soft-decode STATS flags after ownership / multi-frame work.
 * Emits ready / input-ready / ownership / user-fb / multi-frame / reclaim
 * markers when the corresponding door bits are set. Never hard-fails.
 * Returns number of soft markers that counted as success.
 */
static unsigned
soft_stats_flags(const unsigned *aSt)
{
    unsigned cOk = 0;
    unsigned uFlags;
    unsigned cPend;

    if (aSt == 0) {
        return 0;
    }
    uFlags = aSt[3];
    cPend = (uFlags >> SESS_STAT_PEND_SHIFT) & SESS_STAT_PEND_MASK;
    (void)cPend;

    if ((uFlags & SESS_STAT_F_READY) != 0u) {
        msg("sessiond-gj: ready soft\n");
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_INPUT) != 0u) {
        msg("sessiond-gj: input-ready soft\n");
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_OWNED) != 0u && aSt[4] == g_uToken) {
        msg("sessiond-gj: ownership soft\n");
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_USER_FB) != 0u) {
        msg("sessiond-gj: user-fb soft\n");
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_MULTI) != 0u) {
        msg("sessiond-gj: multi-frame soft\n");
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_RECLAIM) != 0u) {
        msg("sessiond-gj: reclaim flag soft\n");
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_DROP) != 0u) {
        /* Observability only — sticky drop is rare; not a soft success. */
        msg("sessiond-gj: drop sticky soft\n");
    }
    return cOk;
}

/*
 * Soft health suite (deepened):
 *   DISPLAY_INFO recheck, MAP_SCANOUT geometry, INPUT_POLL/POP drain,
 *   same-token CLAIM reclaim, STATS ownership/ready/user-fb/reclaim flags,
 *   interim PRESENT, second PRESENT_FB tint, multi-frame STATS recheck.
 * Never aborts the hard ownership path.
 */
static void
soft_health(unsigned char *pFb)
{
    static unsigned aStats[5];
    static struct sess_input_ev ev;
    unsigned long u64Va = 0;
    unsigned aInfo[3];
    unsigned aWh[2];
    long n;
    unsigned cPop = 0;
    unsigned cSoftOk = 0;
    unsigned cPend = 0;

    msg("sessiond-gj: soft health start\n");

    /* DISPLAY_INFO recheck while owned (soft — dimensions optional) */
    aWh[0] = aWh[1] = 0;
    n = gj_session(GJ_SESS_OP_DISPLAY_INFO, (long)(uintptr_t)aWh, 0, 0);
    if (n == 0 && aWh[0] != 0u && aWh[1] != 0u) {
        msg_display_soft(aWh[0], aWh[1]);
        cSoftOk++;
    } else {
        msg("sessiond-gj: DISPLAY_INFO soft-skip\n");
    }
    (void)aWh;

    /* MAP_SCANOUT — va hint + w/h/stride (soft if NODEV) */
    u64Va = 0;
    aInfo[0] = aInfo[1] = aInfo[2] = 0;
    n = gj_session(GJ_SESS_OP_MAP_SCANOUT, (long)(uintptr_t)&u64Va,
                   (long)(uintptr_t)aInfo, 0);
    if (n == 0 && aInfo[0] != 0u && aInfo[1] != 0u && aInfo[2] != 0u) {
        msg_map_soft(aInfo[0], aInfo[1], aInfo[2]);
        cSoftOk++;
    } else {
        msg("sessiond-gj: MAP_SCANOUT soft-skip\n");
    }
    (void)u64Va;
    (void)aInfo;

    /* INPUT_POLL then drain INPUT_POP (empty ring is fine) */
    n = gj_session(GJ_SESS_OP_INPUT_POLL, 0, 0, 0);
    if (n != 0) {
        msg("sessiond-gj: INPUT soft-skip\n");
    } else {
        for (;;) {
            n = gj_session(GJ_SESS_OP_INPUT_POP, (long)(uintptr_t)&ev, 0, 0);
            if (n <= 0) {
                break;
            }
            cPop++;
            if (cPop >= SESS_SOFT_POP_CAP) {
                break; /* defensive cap */
            }
        }
        /* Pending count from a cheap STATS peek (bits 8..15) */
        aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
        n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
        if (n == 0) {
            cPend = (aStats[3] >> SESS_STAT_PEND_SHIFT) & SESS_STAT_PEND_MASK;
        }
        msg_input_soft(cPop, cPend);
        cSoftOk++;
    }
    (void)ev;

    /*
     * Soft reclaim: same-token CLAIM is idempotent (door sets STATS bit19).
     * Different token would be BUSY — we never try that on the live path.
     */
    if (g_uToken != 0u) {
        n = gj_session_claim(g_uToken);
        if (n == 0) {
            msg("sessiond-gj: reclaim soft\n");
            cSoftOk++;
        } else {
            msg_fail_ret("reclaim soft", n);
            msg("sessiond-gj: reclaim soft-skip\n");
        }
    } else {
        msg("sessiond-gj: reclaim soft-skip\n");
    }

    /* STATS — ownership + ready + user-fb (from hard PRESENT_FB) + reclaim */
    aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
    n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (n == 0) {
        msg_stats_soft(aStats);
        cSoftOk += soft_stats_flags(aStats);
    } else {
        msg("sessiond-gj: STATS soft-skip\n");
    }

    /* Interim PRESENT of kernel scanout (soft — policy prefers PRESENT_FB) */
    n = gj_session(GJ_SESS_OP_PRESENT, 0, 0, 0);
    if (n == 0) {
        msg("sessiond-gj: soft PRESENT PASS\n");
        cSoftOk++;
    } else {
        msg("sessiond-gj: soft PRESENT soft-skip\n");
    }

    /* Second PRESENT_FB with alternate tint (multi-frame soft → bit18) */
    if (pFb != 0) {
        fb_fill(pFb, 0x18, 0xc0, 0x40);
        n = gj_session_present_fb(GJ_SESS_FB_W, GJ_SESS_FB_H, pFb);
        if (n == 0) {
            msg("sessiond-gj: soft PRESENT_FB PASS\n");
            cSoftOk++;
        } else {
            msg_fail_ret("soft PRESENT_FB", n);
            msg("sessiond-gj: soft PRESENT_FB soft-skip\n");
        }

        /* Third tint — deepen multi-frame bookkeeping without hard-fail */
        fb_fill(pFb, 0x10, 0xd0, 0x50);
        n = gj_session_present_fb(GJ_SESS_FB_W, GJ_SESS_FB_H, pFb);
        if (n == 0) {
            msg("sessiond-gj: soft PRESENT_FB2 PASS\n");
            cSoftOk++;
        } else {
            msg_fail_ret("soft PRESENT_FB2", n);
            msg("sessiond-gj: soft PRESENT_FB2 soft-skip\n");
        }
    }

    /*
     * Post multi-frame STATS recheck — expect user-fb + multi-frame bits
     * when the door tracked ≥2 successful PRESENT_FB calls.
     */
    aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
    n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (n == 0) {
        msg_stats_soft(aStats);
        if ((aStats[3] & SESS_STAT_F_USER_FB) != 0u) {
            msg("sessiond-gj: user-fb soft\n");
            cSoftOk++;
        }
        if ((aStats[3] & SESS_STAT_F_MULTI) != 0u) {
            msg("sessiond-gj: multi-frame soft\n");
            cSoftOk++;
        } else {
            msg("sessiond-gj: multi-frame soft-skip\n");
        }
        if ((aStats[3] & SESS_STAT_F_RECLAIM) != 0u) {
            msg("sessiond-gj: reclaim flag soft\n");
            cSoftOk++;
        }
        if ((aStats[3] & SESS_STAT_F_OWNED) != 0u && aStats[4] == g_uToken) {
            msg("sessiond-gj: ownership soft\n");
            cSoftOk++;
        }
    } else {
        msg("sessiond-gj: STATS soft-skip\n");
    }

    if (cSoftOk > 0u) {
        msg("sessiond-gj: soft health PASS\n");
    } else {
        msg("sessiond-gj: soft health soft-skip\n");
    }
}

void
_start(void)
{
    unsigned aWh[2];
    unsigned aStats[5];
    static unsigned char aFb[GJ_SESS_FB_W * GJ_SESS_FB_H * 4u];
    long n;

    g_uToken = 0u;
    msg("sessiond-gj: start\n");

    /* CLAIM — kernel defers interim auto-scanout while owned */
    n = gj_session_claim(SESS_TOKEN);
    if (n != 0) {
        msg_fail_ret("CLAIM", n);
        fail_exit("sessiond-gj: CLAIM fail\n");
    }
    g_uToken = SESS_TOKEN;
    msg("sessiond-gj: CLAIM PASS\n");

    /* DISPLAY_INFO via session door (soft — dimensions optional) */
    aWh[0] = 0;
    aWh[1] = 0;
    n = gj_session(GJ_SESS_OP_DISPLAY_INFO, (long)(uintptr_t)aWh, 0, 0);
    if (n != 0) {
        /* Fallback GPU query; still soft if absent */
        n = gj_gpu_display_info(aWh);
        (void)n;
        msg("sessiond-gj: DISPLAY_INFO soft\n");
    } else {
        msg("sessiond-gj: DISPLAY_INFO PASS\n");
    }
    (void)aWh;

    /* Fill a fixed BGRA tile (jade-ish) and PRESENT_FB */
    fb_fill(aFb, 0x20, 0xb0, 0x30);
    n = gj_session_present_fb(GJ_SESS_FB_W, GJ_SESS_FB_H, aFb);
    if (n != 0) {
        msg_fail_ret("PRESENT_FB", n);
        fail_exit("sessiond-gj: PRESENT_FB fail\n");
    }
    msg("sessiond-gj: PRESENT_FB PASS\n");

    /* STATS soft — presents / input / calls / ready / owner when available */
    aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
    n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (n == 0) {
        msg_stats_soft(aStats);
        /* Early user-fb soft after hard present (bit17) — optional */
        if ((aStats[3] & SESS_STAT_F_USER_FB) != 0u) {
            msg("sessiond-gj: user-fb soft\n");
        }
    } else {
        msg("sessiond-gj: STATS soft-skip\n");
    }

    /* Soft health suite (map / input / reclaim / multi-present / flags) */
    soft_health(aFb);

    n = gj_session_release(g_uToken);
    if (n != 0) {
        msg_fail_ret("RELEASE", n);
        g_uToken = 0u;
        fail_exit("sessiond-gj: RELEASE fail\n");
    }
    g_uToken = 0u;
    msg("sessiond-gj: RELEASE PASS\n");

    /*
     * Soft free path: RELEASE when already free is 0 (door contract).
     * Confirm STATS no longer owned — never fails ownership path PASS.
     */
    aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
    n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (n == 0) {
        msg_stats_soft(aStats);
        if ((aStats[3] & SESS_STAT_F_OWNED) == 0u && aStats[4] == 0u) {
            msg("sessiond-gj: free soft\n");
        } else {
            msg("sessiond-gj: free soft-skip\n");
        }
        /* Soft double-RELEASE when free → 0 */
        n = gj_session_release(SESS_TOKEN);
        if (n == 0) {
            msg("sessiond-gj: free RELEASE soft\n");
        } else {
            msg("sessiond-gj: free RELEASE soft-skip\n");
        }
    } else {
        msg("sessiond-gj: free soft-skip\n");
    }

    /* Required by scripts/smoke-all.sh — do not rename */
    msg("sessiond-gj: ownership path PASS\n");
    gj_exit(0);
}
