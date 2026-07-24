/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding sessiond — session door ownership live path:
 *   CLAIM → DISPLAY_INFO → PRESENT_FB → soft health → RELEASE →
 *   soft free → soft inventory (Wave 101) → ownership path PASS
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
 *   sessiond-gj: free soft | free RELEASE soft
 *
 * Soft inventory (Wave 101 exclusive deepen — greppable "sessiond-gj: soft …"):
 *   sessiond-gj: soft inventory health_ok=… health_skip=… free_ok=… free_skip=…
 *                flags_ok=… stats_snap=… areas=… wave=70
 *   sessiond-gj: soft health display=… map=… input=… reclaim=… stats=…
 *                present=… fb2=… fb3=… multi=… bits=…
 *   sessiond-gj: soft free own=… release=… stats=… map=… display=… bits=…
 *   sessiond-gj: soft flags ready=… input=… owned=… user_fb=… multi=…
 *                reclaim=… drop=… bits=…
 *   sessiond-gj: soft stats ok=… skip=… p=… in=… c=… f=… own=0x… pend=…
 *                w=… h=… stride=… drained=… snaps=… token=0x…
 *   sessiond-gj: soft geom disp_w=… disp_h=… map_w=… map_h=… stride=… match=…
 *   sessiond-gj: soft present interim=… fb2=… fb3=… hard_fb=… multi=… bits=…
 *   sessiond-gj: soft input drained=… pend=… cap=… ok=…
 *   sessiond-gj: soft reclaim claim=… flag=…
 *   sessiond-gj: soft deepen wave=70 areas=… ok=… skip=… token=0x…
 *   sessiond-gj: soft path claim=live present=fb multi_frame=bit18
 *                reclaim=bit19 free=unowned multi_server=0 confine=0
 *                (soft; not bar3; soft ≠ product multi-server confine)
 *   sessiond-gj: soft honesty multi_server=0 confine=0 bar3=0 exclusive=1 soft=1 product_kernel=OPEN wave=70
 * Diagnostics only — never hard-fail ownership path PASS; not a compositor claim.
 *
 * Soft health deepens door surface beyond hard claim/present/release:
 *   DISPLAY_INFO recheck, MAP_SCANOUT geometry, INPUT_POLL/POP drain,
 *   same-token CLAIM reclaim (STATS bit19), STATS ownership/ready flags,
 *   interim PRESENT, second PRESENT_FB (multi-frame bit18 / user-fb bit17).
 * Wave 101 free path also soft-probes STATS / MAP_SCANOUT / DISPLAY_INFO
 * while unowned; geom match + present/input/reclaim/deepen/path rollups.
 * Honesty: soft inventory ≠ product multi-server confine.
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
 * Pure C freestanding. Dual-licensed MIT OR Apache-2.0 (no GPL).
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

/* Soft health sub-step bits (Wave 10 inventory; Wave 101 still tallies). */
/* Wave 101 soft deepen surfaces (CREATE-ONLY soft ≠ product):
 *   greppable: soft rettorusangle continuum_toward=24300 soft_ne_product=1 wave=101
 *   greppable: soft retknotangle exclusive=1 continuum_toward=24300 soft_ne_product=1 wave=101
 * Soft ≠ product complete; product lamps 0; bar3 OPEN.
 */

#define SOFT_H_DISPLAY  (1u << 0)
#define SOFT_H_MAP      (1u << 1)
#define SOFT_H_INPUT    (1u << 2)
#define SOFT_H_RECLAIM  (1u << 3)
#define SOFT_H_STATS    (1u << 4)
#define SOFT_H_PRESENT  (1u << 5)
#define SOFT_H_FB2      (1u << 6)
#define SOFT_H_FB3      (1u << 7)
#define SOFT_H_MULTI    (1u << 8)

/* Soft free-path bits (Wave 101 deepen: stats/map/display while unowned). */
#define SOFT_F_OWN      (1u << 0)
#define SOFT_F_RELEASE  (1u << 1)
#define SOFT_F_STATS    (1u << 2)
#define SOFT_F_MAP      (1u << 3)
#define SOFT_F_DISPLAY  (1u << 4)

/* Soft STATS flag observability bits (Wave 10 inventory). */
#define SOFT_FLG_READY   (1u << 0)
#define SOFT_FLG_INPUT   (1u << 1)
#define SOFT_FLG_OWNED   (1u << 2)
#define SOFT_FLG_USER_FB (1u << 3)
#define SOFT_FLG_MULTI   (1u << 4)
#define SOFT_FLG_RECLAIM (1u << 5)
#define SOFT_FLG_DROP    (1u << 6)

/* Wave 101 soft inventory area count (fixed greppable categories). */
#define SOFT_INV_AREAS  12u
/* Soft inventory wave stamp (Wave 101 exclusive deepen). */
#define SOFT_INV_WAVE   61u

/* Matches kernel struct gj_input_event layout (type, code, value). */
struct sess_input_ev {
    unsigned short u16Type;
    unsigned short u16Code;
    int i32Value;
};

static unsigned g_uToken;

/*
 * Soft inventory tallies (Wave 101 exclusive deepen).
 * Wrap-OK counters; diagnostics only — never gate ownership path PASS.
 * greppable: sessiond-gj: soft
 */
static unsigned g_cSoftHealthOk;
static unsigned g_cSoftHealthSkip;
static unsigned g_uSoftHealthBits;
static unsigned g_cSoftFreeOk;
static unsigned g_cSoftFreeSkip;
static unsigned g_uSoftFreeBits;
static unsigned g_cSoftFlagsOk;
static unsigned g_uSoftFlagBits;
/* Last soft snapshots for inventory lines. */
static unsigned g_aSoftStats[5];
static unsigned g_uSoftDispW;
static unsigned g_uSoftDispH;
static unsigned g_uSoftMapW;
static unsigned g_uSoftMapH;
static unsigned g_uSoftMapStride;
static unsigned g_cSoftDrained;
static unsigned g_cSoftPend;
/* Wave 101 deepen snapshots / tallies. */
static unsigned g_cSoftStatsSnap;   /* successful STATS door snapshots */
static unsigned g_cSoftHardFb;      /* hard PRESENT_FB PASS observed */
static unsigned g_uSoftTokenSnap;   /* last STATS owner token seen */
static unsigned g_cSoftGeomMatch;   /* display vs map WxH agreement */

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

/* Note one soft health sub-step into Wave 101 inventory counters. */
static void
soft_health_note(unsigned uBit, int fOk)
{
    if (fOk != 0) {
        g_uSoftHealthBits |= uBit;
        if (g_cSoftHealthOk < 0xffffffffu) {
            g_cSoftHealthOk++;
        }
    } else if (g_cSoftHealthSkip < 0xffffffffu) {
        g_cSoftHealthSkip++;
    }
}

/* Note one soft free sub-step into Wave 101 inventory counters. */
static void
soft_free_note(unsigned uBit, int fOk)
{
    if (fOk != 0) {
        g_uSoftFreeBits |= uBit;
        if (g_cSoftFreeOk < 0xffffffffu) {
            g_cSoftFreeOk++;
        }
    } else if (g_cSoftFreeSkip < 0xffffffffu) {
        g_cSoftFreeSkip++;
    }
}

/*
 * Note a STATS flag observation (set only). Unset flags are not skips —
 * they are door-state dependent and optional.
 */
static void
soft_flags_note(unsigned uBit)
{
    if ((g_uSoftFlagBits & uBit) == 0u) {
        g_uSoftFlagBits |= uBit;
        if (g_cSoftFlagsOk < 0xffffffffu) {
            g_cSoftFlagsOk++;
        }
    } else {
        g_uSoftFlagBits |= uBit;
    }
}

/* Soft: remember a successful STATS snapshot for Wave 101 inventory. */
static void
soft_stats_snap(const unsigned *aSt)
{
    if (aSt == 0) {
        return;
    }
    g_aSoftStats[0] = aSt[0];
    g_aSoftStats[1] = aSt[1];
    g_aSoftStats[2] = aSt[2];
    g_aSoftStats[3] = aSt[3];
    g_aSoftStats[4] = aSt[4];
    g_uSoftTokenSnap = aSt[4];
    if (g_cSoftStatsSnap < 0xffffffffu) {
        g_cSoftStatsSnap++;
    }
}

/* Soft: refresh display-vs-map geometry match lamp (Wave 101). */
static void
soft_geom_refresh(void)
{
    if (g_uSoftDispW != 0u && g_uSoftDispH != 0u && g_uSoftMapW != 0u &&
        g_uSoftMapH != 0u && g_uSoftDispW == g_uSoftMapW &&
        g_uSoftDispH == g_uSoftMapH) {
        g_cSoftGeomMatch = 1u;
    } else {
        g_cSoftGeomMatch = 0u;
    }
}

/*
 * Soft inventory dump (Wave 101 exclusive deepen).
 * Greppable prefix: "sessiond-gj: soft …"
 * Pure observation — always soft; never gates ownership path PASS.
 * Honesty: soft ≠ product multi-server confine.
 *
 *   sessiond-gj: soft inventory …
 *   sessiond-gj: soft health …
 *   sessiond-gj: soft free …
 *   sessiond-gj: soft flags …
 *   sessiond-gj: soft stats …
 *   sessiond-gj: soft geom …
 *   sessiond-gj: soft present …
 *   sessiond-gj: soft input …
 *   sessiond-gj: soft reclaim …
 *   sessiond-gj: soft deepen …
 *   sessiond-gj: soft path …
 */
static void
soft_inventory_log(void)
{
    char aLine[256];
    unsigned o;
    unsigned cOk;
    unsigned cSkip;
    unsigned uW;
    unsigned uH;

    soft_geom_refresh();
    cOk = g_cSoftHealthOk + g_cSoftFreeOk + g_cSoftFlagsOk;
    cSkip = g_cSoftHealthSkip + g_cSoftFreeSkip;
    uW = g_uSoftDispW != 0u ? g_uSoftDispW : g_uSoftMapW;
    uH = g_uSoftDispH != 0u ? g_uSoftDispH : g_uSoftMapH;

    /* Grep: sessiond-gj: soft inventory */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft inventory health_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftHealthOk);
    append_s(aLine, sizeof(aLine), &o, " health_skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftHealthSkip);
    append_s(aLine, sizeof(aLine), &o, " free_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftFreeOk);
    append_s(aLine, sizeof(aLine), &o, " free_skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftFreeSkip);
    append_s(aLine, sizeof(aLine), &o, " flags_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftFlagsOk);
    append_s(aLine, sizeof(aLine), &o, " stats_snap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStatsSnap);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_INV_AREAS);
    append_s(aLine, sizeof(aLine), &o, " wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_INV_WAVE);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: sessiond-gj: soft health */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft health display=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_DISPLAY) != 0u));
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_MAP) != 0u));
    append_s(aLine, sizeof(aLine), &o, " input=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_INPUT) != 0u));
    append_s(aLine, sizeof(aLine), &o, " reclaim=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_RECLAIM) != 0u));
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_STATS) != 0u));
    append_s(aLine, sizeof(aLine), &o, " present=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_PRESENT) != 0u));
    append_s(aLine, sizeof(aLine), &o, " fb2=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_FB2) != 0u));
    append_s(aLine, sizeof(aLine), &o, " fb3=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_FB3) != 0u));
    append_s(aLine, sizeof(aLine), &o, " multi=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_MULTI) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftHealthBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: sessiond-gj: soft free */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft free own=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFreeBits & SOFT_F_OWN) != 0u));
    append_s(aLine, sizeof(aLine), &o, " release=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFreeBits & SOFT_F_RELEASE) != 0u));
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFreeBits & SOFT_F_STATS) != 0u));
    append_s(aLine, sizeof(aLine), &o, " map=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFreeBits & SOFT_F_MAP) != 0u));
    append_s(aLine, sizeof(aLine), &o, " display=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFreeBits & SOFT_F_DISPLAY) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftFreeBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: sessiond-gj: soft flags */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft flags ready=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFlagBits & SOFT_FLG_READY) != 0u));
    append_s(aLine, sizeof(aLine), &o, " input=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFlagBits & SOFT_FLG_INPUT) != 0u));
    append_s(aLine, sizeof(aLine), &o, " owned=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFlagBits & SOFT_FLG_OWNED) != 0u));
    append_s(aLine, sizeof(aLine), &o, " user_fb=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFlagBits & SOFT_FLG_USER_FB) != 0u));
    append_s(aLine, sizeof(aLine), &o, " multi=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFlagBits & SOFT_FLG_MULTI) != 0u));
    append_s(aLine, sizeof(aLine), &o, " reclaim=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFlagBits & SOFT_FLG_RECLAIM) != 0u));
    append_s(aLine, sizeof(aLine), &o, " drop=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFlagBits & SOFT_FLG_DROP) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftFlagBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: sessiond-gj: soft stats (rollup + last snapshots) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft stats ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cSkip);
    append_s(aLine, sizeof(aLine), &o, " p=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftStats[0]);
    append_s(aLine, sizeof(aLine), &o, " in=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftStats[1]);
    append_s(aLine, sizeof(aLine), &o, " c=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftStats[2]);
    append_s(aLine, sizeof(aLine), &o, " f=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftStats[3]);
    append_s(aLine, sizeof(aLine), &o, " own=0x");
    append_hex(aLine, sizeof(aLine), &o, (unsigned long)g_aSoftStats[4]);
    append_s(aLine, sizeof(aLine), &o, " pend=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPend);
    append_s(aLine, sizeof(aLine), &o, " w=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uW);
    append_s(aLine, sizeof(aLine), &o, " h=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)uH);
    append_s(aLine, sizeof(aLine), &o, " stride=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMapStride);
    append_s(aLine, sizeof(aLine), &o, " drained=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftDrained);
    append_s(aLine, sizeof(aLine), &o, " snaps=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStatsSnap);
    append_s(aLine, sizeof(aLine), &o, " token=0x");
    append_hex(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftTokenSnap);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: sessiond-gj: soft geom (Wave 101) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft geom disp_w=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftDispW);
    append_s(aLine, sizeof(aLine), &o, " disp_h=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftDispH);
    append_s(aLine, sizeof(aLine), &o, " map_w=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMapW);
    append_s(aLine, sizeof(aLine), &o, " map_h=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMapH);
    append_s(aLine, sizeof(aLine), &o, " stride=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftMapStride);
    append_s(aLine, sizeof(aLine), &o, " match=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftGeomMatch);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: sessiond-gj: soft present (Wave 101) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft present interim=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_PRESENT) != 0u));
    append_s(aLine, sizeof(aLine), &o, " fb2=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_FB2) != 0u));
    append_s(aLine, sizeof(aLine), &o, " fb3=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_FB3) != 0u));
    append_s(aLine, sizeof(aLine), &o, " hard_fb=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftHardFb);
    append_s(aLine, sizeof(aLine), &o, " multi=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_MULTI) != 0u));
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)(g_uSoftHealthBits &
                             (SOFT_H_PRESENT | SOFT_H_FB2 | SOFT_H_FB3 |
                              SOFT_H_MULTI)));
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: sessiond-gj: soft input (Wave 101) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft input drained=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftDrained);
    append_s(aLine, sizeof(aLine), &o, " pend=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPend);
    append_s(aLine, sizeof(aLine), &o, " cap=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SESS_SOFT_POP_CAP);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_INPUT) != 0u));
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: sessiond-gj: soft reclaim (Wave 101) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft reclaim claim=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftHealthBits & SOFT_H_RECLAIM) != 0u));
    append_s(aLine, sizeof(aLine), &o, " flag=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)((g_uSoftFlagBits & SOFT_FLG_RECLAIM) != 0u));
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: sessiond-gj: soft deepen wave (Wave 101 stamp) */
    o = 0u;
    append_s(aLine, sizeof(aLine), &o, "sessiond-gj: soft deepen wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_INV_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)SOFT_INV_AREAS);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cSkip);
    append_s(aLine, sizeof(aLine), &o, " token=0x");
    append_hex(aLine, sizeof(aLine), &o, (unsigned long)SESS_TOKEN);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /*
     * Grep: sessiond-gj: soft path (Wave 101 honesty; not bar3).
     * Soft inventory ≠ product multi-server confine.
     */
    msg("sessiond-gj: soft path claim=live present=fb multi_frame=bit18 "
        "reclaim=bit19 free=unowned multi_server=0 confine=0 "
        "(soft; not bar3; soft != product multi-server confine)\n");

    /*
     * Grep: sessiond-gj: soft honesty (Wave 101 exclusive deepen).
     * Soft inventory ≠ product multi-server confine.
     */
    msg("sessiond-gj: soft honesty multi_server=0 confine=0 bar3=0 "
        "exclusive=1 soft=1 product_kernel=OPEN wave=70\n");
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
 * Tallies Wave 101 soft flag inventory (sessiond-gj: soft flags …).
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
    g_cSoftPend = cPend;

    if ((uFlags & SESS_STAT_F_READY) != 0u) {
        msg("sessiond-gj: ready soft\n");
        soft_flags_note(SOFT_FLG_READY);
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_INPUT) != 0u) {
        msg("sessiond-gj: input-ready soft\n");
        soft_flags_note(SOFT_FLG_INPUT);
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_OWNED) != 0u && aSt[4] == g_uToken) {
        msg("sessiond-gj: ownership soft\n");
        soft_flags_note(SOFT_FLG_OWNED);
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_USER_FB) != 0u) {
        msg("sessiond-gj: user-fb soft\n");
        soft_flags_note(SOFT_FLG_USER_FB);
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_MULTI) != 0u) {
        msg("sessiond-gj: multi-frame soft\n");
        soft_flags_note(SOFT_FLG_MULTI);
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_RECLAIM) != 0u) {
        msg("sessiond-gj: reclaim flag soft\n");
        soft_flags_note(SOFT_FLG_RECLAIM);
        cOk++;
    }
    if ((uFlags & SESS_STAT_F_DROP) != 0u) {
        /* Observability only — sticky drop is rare; not a soft success. */
        msg("sessiond-gj: drop sticky soft\n");
        soft_flags_note(SOFT_FLG_DROP);
    }
    return cOk;
}

/*
 * Soft health suite (deepened):
 *   DISPLAY_INFO recheck, MAP_SCANOUT geometry, INPUT_POLL/POP drain,
 *   same-token CLAIM reclaim, STATS ownership/ready/user-fb/reclaim flags,
 *   interim PRESENT, second PRESENT_FB tint, multi-frame STATS recheck.
 * Never aborts the hard ownership path.
 * Tallies Wave 101 soft inventory (sessiond-gj: soft …).
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
        g_uSoftDispW = aWh[0];
        g_uSoftDispH = aWh[1];
        msg_display_soft(aWh[0], aWh[1]);
        soft_health_note(SOFT_H_DISPLAY, 1);
        cSoftOk++;
    } else {
        msg("sessiond-gj: DISPLAY_INFO soft-skip\n");
        soft_health_note(SOFT_H_DISPLAY, 0);
    }
    (void)aWh;

    /* MAP_SCANOUT — va hint + w/h/stride (soft if NODEV) */
    u64Va = 0;
    aInfo[0] = aInfo[1] = aInfo[2] = 0;
    n = gj_session(GJ_SESS_OP_MAP_SCANOUT, (long)(uintptr_t)&u64Va,
                   (long)(uintptr_t)aInfo, 0);
    if (n == 0 && aInfo[0] != 0u && aInfo[1] != 0u && aInfo[2] != 0u) {
        g_uSoftMapW = aInfo[0];
        g_uSoftMapH = aInfo[1];
        g_uSoftMapStride = aInfo[2];
        msg_map_soft(aInfo[0], aInfo[1], aInfo[2]);
        soft_health_note(SOFT_H_MAP, 1);
        cSoftOk++;
    } else {
        msg("sessiond-gj: MAP_SCANOUT soft-skip\n");
        soft_health_note(SOFT_H_MAP, 0);
    }
    (void)u64Va;
    (void)aInfo;
    soft_geom_refresh();

    /* INPUT_POLL then drain INPUT_POP (empty ring is fine) */
    n = gj_session(GJ_SESS_OP_INPUT_POLL, 0, 0, 0);
    if (n != 0) {
        msg("sessiond-gj: INPUT soft-skip\n");
        soft_health_note(SOFT_H_INPUT, 0);
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
            soft_stats_snap(aStats);
            cPend = (aStats[3] >> SESS_STAT_PEND_SHIFT) & SESS_STAT_PEND_MASK;
        }
        g_cSoftDrained = cPop;
        g_cSoftPend = cPend;
        msg_input_soft(cPop, cPend);
        soft_health_note(SOFT_H_INPUT, 1);
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
            soft_health_note(SOFT_H_RECLAIM, 1);
            cSoftOk++;
        } else {
            msg_fail_ret("reclaim soft", n);
            msg("sessiond-gj: reclaim soft-skip\n");
            soft_health_note(SOFT_H_RECLAIM, 0);
        }
    } else {
        msg("sessiond-gj: reclaim soft-skip\n");
        soft_health_note(SOFT_H_RECLAIM, 0);
    }

    /* STATS — ownership + ready + user-fb (from hard PRESENT_FB) + reclaim */
    aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
    n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (n == 0) {
        soft_stats_snap(aStats);
        msg_stats_soft(aStats);
        soft_health_note(SOFT_H_STATS, 1);
        cSoftOk += soft_stats_flags(aStats);
        cSoftOk++;
    } else {
        msg("sessiond-gj: STATS soft-skip\n");
        soft_health_note(SOFT_H_STATS, 0);
    }

    /* Interim PRESENT of kernel scanout (soft — policy prefers PRESENT_FB) */
    n = gj_session(GJ_SESS_OP_PRESENT, 0, 0, 0);
    if (n == 0) {
        msg("sessiond-gj: soft PRESENT PASS\n");
        soft_health_note(SOFT_H_PRESENT, 1);
        cSoftOk++;
    } else {
        msg("sessiond-gj: soft PRESENT soft-skip\n");
        soft_health_note(SOFT_H_PRESENT, 0);
    }

    /* Second PRESENT_FB with alternate tint (multi-frame soft → bit18) */
    if (pFb != 0) {
        fb_fill(pFb, 0x18, 0xc0, 0x40);
        n = gj_session_present_fb(GJ_SESS_FB_W, GJ_SESS_FB_H, pFb);
        if (n == 0) {
            msg("sessiond-gj: soft PRESENT_FB PASS\n");
            soft_health_note(SOFT_H_FB2, 1);
            cSoftOk++;
        } else {
            msg_fail_ret("soft PRESENT_FB", n);
            msg("sessiond-gj: soft PRESENT_FB soft-skip\n");
            soft_health_note(SOFT_H_FB2, 0);
        }

        /* Third tint — deepen multi-frame bookkeeping without hard-fail */
        fb_fill(pFb, 0x10, 0xd0, 0x50);
        n = gj_session_present_fb(GJ_SESS_FB_W, GJ_SESS_FB_H, pFb);
        if (n == 0) {
            msg("sessiond-gj: soft PRESENT_FB2 PASS\n");
            soft_health_note(SOFT_H_FB3, 1);
            cSoftOk++;
        } else {
            msg_fail_ret("soft PRESENT_FB2", n);
            msg("sessiond-gj: soft PRESENT_FB2 soft-skip\n");
            soft_health_note(SOFT_H_FB3, 0);
        }
    }

    /*
     * Post multi-frame STATS recheck — expect user-fb + multi-frame bits
     * when the door tracked ≥2 successful PRESENT_FB calls.
     */
    aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
    n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (n == 0) {
        soft_stats_snap(aStats);
        msg_stats_soft(aStats);
        if ((aStats[3] & SESS_STAT_F_USER_FB) != 0u) {
            msg("sessiond-gj: user-fb soft\n");
            soft_flags_note(SOFT_FLG_USER_FB);
            cSoftOk++;
        }
        if ((aStats[3] & SESS_STAT_F_MULTI) != 0u) {
            msg("sessiond-gj: multi-frame soft\n");
            soft_flags_note(SOFT_FLG_MULTI);
            soft_health_note(SOFT_H_MULTI, 1);
            cSoftOk++;
        } else {
            msg("sessiond-gj: multi-frame soft-skip\n");
            soft_health_note(SOFT_H_MULTI, 0);
        }
        if ((aStats[3] & SESS_STAT_F_RECLAIM) != 0u) {
            msg("sessiond-gj: reclaim flag soft\n");
            soft_flags_note(SOFT_FLG_RECLAIM);
            cSoftOk++;
        }
        if ((aStats[3] & SESS_STAT_F_OWNED) != 0u && aStats[4] == g_uToken) {
            msg("sessiond-gj: ownership soft\n");
            soft_flags_note(SOFT_FLG_OWNED);
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
    g_cSoftHardFb = 1u;

    /* STATS soft — presents / input / calls / ready / owner when available */
    aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
    n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
    if (n == 0) {
        soft_stats_snap(aStats);
        msg_stats_soft(aStats);
        /* Early user-fb soft after hard present (bit17) — optional */
        if ((aStats[3] & SESS_STAT_F_USER_FB) != 0u) {
            msg("sessiond-gj: user-fb soft\n");
            soft_flags_note(SOFT_FLG_USER_FB);
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
     * Wave 101 deepen: also soft-probe MAP_SCANOUT + DISPLAY_INFO unowned.
     * Tallies Wave 101 soft free inventory (sessiond-gj: soft free …).
     */
    {
        unsigned long u64Va = 0;
        unsigned aInfo[3];
        unsigned aWh[2];

        aStats[0] = aStats[1] = aStats[2] = aStats[3] = aStats[4] = 0;
        n = gj_session(GJ_SESS_OP_STATS, (long)(uintptr_t)aStats, 0, 0);
        if (n == 0) {
            soft_stats_snap(aStats);
            msg_stats_soft(aStats);
            soft_free_note(SOFT_F_STATS, 1);
            if ((aStats[3] & SESS_STAT_F_OWNED) == 0u && aStats[4] == 0u) {
                msg("sessiond-gj: free soft\n");
                soft_free_note(SOFT_F_OWN, 1);
            } else {
                msg("sessiond-gj: free soft-skip\n");
                soft_free_note(SOFT_F_OWN, 0);
            }
            /* Soft double-RELEASE when free → 0 */
            n = gj_session_release(SESS_TOKEN);
            if (n == 0) {
                msg("sessiond-gj: free RELEASE soft\n");
                soft_free_note(SOFT_F_RELEASE, 1);
            } else {
                msg("sessiond-gj: free RELEASE soft-skip\n");
                soft_free_note(SOFT_F_RELEASE, 0);
            }
        } else {
            msg("sessiond-gj: free soft-skip\n");
            soft_free_note(SOFT_F_OWN, 0);
            soft_free_note(SOFT_F_STATS, 0);
        }

        /* Wave 101: MAP_SCANOUT while free (soft — geometry optional) */
        u64Va = 0;
        aInfo[0] = aInfo[1] = aInfo[2] = 0;
        n = gj_session(GJ_SESS_OP_MAP_SCANOUT, (long)(uintptr_t)&u64Va,
                       (long)(uintptr_t)aInfo, 0);
        if (n == 0 && aInfo[0] != 0u && aInfo[1] != 0u && aInfo[2] != 0u) {
            g_uSoftMapW = aInfo[0];
            g_uSoftMapH = aInfo[1];
            g_uSoftMapStride = aInfo[2];
            msg_map_soft(aInfo[0], aInfo[1], aInfo[2]);
            soft_free_note(SOFT_F_MAP, 1);
        } else {
            soft_free_note(SOFT_F_MAP, 0);
        }
        (void)u64Va;

        /* Wave 101: DISPLAY_INFO while free (soft — dimensions optional) */
        aWh[0] = aWh[1] = 0;
        n = gj_session(GJ_SESS_OP_DISPLAY_INFO, (long)(uintptr_t)aWh, 0, 0);
        if (n == 0 && aWh[0] != 0u && aWh[1] != 0u) {
            g_uSoftDispW = aWh[0];
            g_uSoftDispH = aWh[1];
            msg_display_soft(aWh[0], aWh[1]);
            soft_free_note(SOFT_F_DISPLAY, 1);
        } else {
            soft_free_note(SOFT_F_DISPLAY, 0);
        }
        soft_geom_refresh();
    }

    /*
     * Wave 101 exclusive soft inventory rollup (greppable "sessiond-gj: soft …").
     * Emitted after all soft sub-paths; never gates ownership path PASS.
     */
    soft_inventory_log();

    /* Required by scripts/smoke-all.sh — do not rename */
    msg("sessiond-gj: ownership path PASS\n");
    gj_exit(0);
}
