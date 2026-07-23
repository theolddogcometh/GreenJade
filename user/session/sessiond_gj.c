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
 *   sessiond-gj: STATS soft …
 *   sessiond-gj: soft health PASS | soft health soft-skip
 *   sessiond-gj: soft PRESENT_FB PASS | soft PRESENT soft-skip
 *
 * Door opcodes (do not renumber — match kernel/include/gj/session_door.h):
 *   1 PRESENT  2 DISPLAY_INFO  3 INPUT_POLL  4 INPUT_POP
 *   5 STATS    6 PRESENT_FB    7 CLAIM       8 RELEASE  9 MAP_SCANOUT
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

/* STATS flags: bit0 ready, bit1 input ready, bit2 owned (door contract). */
#define SESS_STAT_F_READY  1u
#define SESS_STAT_F_INPUT  2u
#define SESS_STAT_F_OWNED  4u

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
    /* hex low nibble dump (token is small) */
    {
        unsigned u = aSt[4];
        unsigned i;
        char aHex[8];
        unsigned n = 0;

        if (u == 0) {
            aHex[n++] = '0';
        } else {
            while (u > 0 && n < sizeof(aHex)) {
                unsigned d = u & 0xfu;
                aHex[n++] = (char)(d < 10u ? ('0' + d) : ('a' + (d - 10u)));
                u >>= 4;
            }
        }
        for (i = n; i > 0 && o + 1 < sizeof(aLine); i--) {
            aLine[o++] = aHex[i - 1u];
        }
    }
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
 * Soft health: MAP_SCANOUT, INPUT_POLL/POP drain, STATS ownership check,
 * optional interim PRESENT, second PRESENT_FB tint.
 * Never aborts the hard ownership path.
 */
static void
soft_health(unsigned char *pFb)
{
    static unsigned aStats[5];
    static struct sess_input_ev ev;
    unsigned long u64Va = 0;
    unsigned aInfo[3];
    long n;
    unsigned cPop = 0;
    unsigned cSoftOk = 0;

    msg("sessiond-gj: soft health start\n");

    /* MAP_SCANOUT — va hint + w/h/stride (soft if NODEV) */
    u64Va = 0;
    aInfo[0] = aInfo[1] = aInfo[2] = 0;
    n = gj_session(GJ_SESS_OP_MAP_SCANOUT, (long)(uintptr_t)&u64Va,
                   (long)(uintptr_t)aInfo, 0);
    if (n == 0) {
        msg("sessiond-gj: MAP_SCANOUT soft\n");
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
            if (cPop > 64u) {
                break; /* defensive cap */
            }
        }
        msg("sessiond-gj: INPUT soft\n");
        cSoftOk++;
    }
    (void)ev;
    (void)cPop;

    /* STATS — verify owned bit + token when available */
    aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
    n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (n == 0) {
        msg_stats_soft(aStats);
        if ((aStats[3] & SESS_STAT_F_OWNED) != 0 && aStats[4] == g_uToken) {
            msg("sessiond-gj: ownership soft\n");
            cSoftOk++;
        }
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

    /* Second PRESENT_FB with alternate tint (multi-frame soft) */
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
    } else {
        msg("sessiond-gj: STATS soft-skip\n");
    }

    /* Soft health suite (map / input / ownership / multi-present) */
    soft_health(aFb);

    n = gj_session_release(g_uToken);
    if (n != 0) {
        msg_fail_ret("RELEASE", n);
        g_uToken = 0u;
        fail_exit("sessiond-gj: RELEASE fail\n");
    }
    g_uToken = 0u;
    msg("sessiond-gj: RELEASE PASS\n");

    /* Required by scripts/smoke-all.sh — do not rename */
    msg("sessiond-gj: ownership path PASS\n");
    gj_exit(0);
}
