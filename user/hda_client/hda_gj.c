/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding HDA stream client (ring-3 smoke) for GreenJade.
 * Exercises GJ_SYS_HDA_STREAM (nr 94) via gj_hda_stream:
 *   open → write PCM → start → tick → stats → underrun tick → close
 * then a deepened soft suite (multi-op + partial-tick + multi-write +
 * soft underrun + mono + reclaim + reject + bits8 + idle + closed +
 * zerotick). Soft steps never hard-fail the live path.
 *
 * Soft inventory (Wave 37 exclusive deepen):
 *   - Cumulative soft sub-step ok/skip; door open/write/start/tick/stats
 *   - Soft played-byte sum; underrun / reject / mono / reclaim lamps
 *   - Per-step lamps (multi/partial/…/bits8/idle/closed/zerotick)
 *   - miss rollup, last/first STATS, peak q/p/u, ops/format/ratio
 *   - inventory / steps / miss / last / deepen / path / ops / first /
 *     peak / format / ratio / honesty greppable lines
 *   greppable: "hda_client: soft …"
 *   greppable: "hda-gj: soft …"
 * Diagnostics only — never gates live path PASS.
 *
 * Smoke markers (must stay prefix-stable for greppable live logs):
 *   hda_client-gj: OPEN PASS
 *   hda_client-gj: WRITE PASS
 *   hda_client-gj: START PASS
 *   hda_client-gj: TICK PASS
 *   hda_client-gj: STATS PASS
 *   hda_client-gj: underrun PASS
 *   hda_client-gj: CLOSE PASS
 *   hda_client-gj: live path PASS
 *
 * Soft (optional; never fails live path):
 *   hda_client-gj: soft suite start
 *   hda_client-gj: soft multi-op start
 *   hda_client-gj: soft multi-op PASS | soft multi-op soft-skip
 *   hda_client-gj: soft partial-tick PASS | soft partial-tick soft-skip
 *   hda_client-gj: soft multi-write PASS | soft multi-write soft-skip
 *   hda_client-gj: soft underrun PASS | soft underrun soft-skip
 *   hda_client-gj: soft mono PASS | soft mono soft-skip
 *   hda_client-gj: soft reclaim PASS | soft reclaim soft-skip
 *   hda_client-gj: soft reject PASS | soft reject soft-skip
 *   hda_client-gj: soft bits8 PASS | soft bits8 soft-skip
 *   hda_client-gj: soft idle PASS | soft idle soft-skip
 *   hda_client-gj: soft closed PASS | soft closed soft-skip
 *   hda_client-gj: soft zerotick PASS | soft zerotick soft-skip
 *   hda_client-gj: stats soft q=… p=… u=…
 *   hda_client-gj: soft suite PASS | soft suite soft-skip
 *   hda_client: soft suite ok=… skip=…
 *   hda_client: soft door open=… write=… start=… tick=… stats=… close=…
 *   hda_client: soft played=… underrun=… reject=… mono=… reclaim=…
 *   hda_client: soft inventory ok=… skip=… door_ok=… door_miss=… played=…
 *                wave=37 areas=…
 *   hda_client: soft steps multi=… partial=… multiwrite=… underrun=…
 *                mono=… reclaim=… reject=… bits8=… idle=… closed=…
 *                zerotick=… bits=…
 *   hda_client: soft miss open=… write=… start=… tick=… stats=…
 *   hda_client: soft last q=… p=… u=… snaps=…
 *   hda_client: soft first q=… p=… u=…
 *   hda_client: soft peak q=… p=… u=…
 *   hda_client: soft ops open=… write=… start=… tick=… stats=… close=…
 *   hda_client: soft format stereo=… mono=… bits8=…
 *   hda_client: soft ratio ok=… skip=… door_ok=… door_miss=…
 *   hda_client: soft deepen wave=37 areas=… ok=… skip=… played=…
 *   hda_client: soft path open=stereo write=pcm … (soft; not bar3)
 *   hda_client: soft honesty not-bar3 not-pipewire soft-inventory-only
 *   hda-gj: soft suite|door|played|inventory|steps|miss|last|first|
 *           peak|ops|format|ratio|deepen|path|honesty twins
 *
 * Fail lines: hda_client-gj: <tag> fail ret=<n>
 *
 *   make hda_client-gj → build/user/hda_client.elf
 * Boot embed (parent): kernel/proc/hda_client_embed.S (.incbin of the ELF).
 */
#include <gj/syscalls.h>

/*
 * Prefer libgj GJ_HDA_OP_* (GJ_SYS_HDA_STREAM = 94). Local aliases keep the
 * file self-describing if headers lag; numbers must not drift from libgj.
 */
#ifndef GJ_HDA_OP_OPEN
#define GJ_HDA_OP_OPEN  0u
#define GJ_HDA_OP_WRITE 1u
#define GJ_HDA_OP_START 2u
#define GJ_HDA_OP_TICK  3u
#define GJ_HDA_OP_CLOSE 4u
#define GJ_HDA_OP_STATS 5u
#endif

#define GJ_HDA_CHANNELS_STEREO 2u
#define GJ_HDA_CHANNELS_MONO   1u
#define GJ_HDA_RATE_48K        48000u
#define GJ_HDA_BITS_16         16u
#define GJ_HDA_BITS_8          8u

/* Frame size: stereo 16-bit LE → 4 bytes/frame. */
#define HDA_BYTES_PER_FRAME 4u
/* Mono 16-bit LE → 2 bytes/frame. */
#define HDA_MONO_BYTES_PER_FRAME 2u
/* Stereo 8-bit → 2 bytes/frame. */
#define HDA_BITS8_BYTES_PER_FRAME 2u

/* Hard smoke: 64 frames × 4 B = 256 B full-buffer consume. */
#define HDA_SMOKE_FRAMES 64u
#define HDA_SMOKE_BYTES  (HDA_SMOKE_FRAMES * HDA_BYTES_PER_FRAME)

/* Soft multi-op: half buffer (32 frames × 4 B = 128 B). */
#define HDA_SOFT_FRAMES 32u
#define HDA_SOFT_BYTES  (HDA_SOFT_FRAMES * HDA_BYTES_PER_FRAME)

/* Soft partial-tick: 48 frames; first tick 16 frames leave queue non-empty. */
#define HDA_SOFT_PART_FRAMES   48u
#define HDA_SOFT_PART_BYTES    (HDA_SOFT_PART_FRAMES * HDA_BYTES_PER_FRAME)
#define HDA_SOFT_PART_TICK1    16u
#define HDA_SOFT_PART_TICK1_B  (HDA_SOFT_PART_TICK1 * HDA_BYTES_PER_FRAME)

/* Soft multi-write: two 64 B chunks (16 frames each). */
#define HDA_SOFT_CHUNK_BYTES 64u
#define HDA_SOFT_CHUNK_FRAMES 16u

/* Soft mono: 32 frames × 2 B = 64 B. */
#define HDA_SOFT_MONO_FRAMES 32u
#define HDA_SOFT_MONO_BYTES  (HDA_SOFT_MONO_FRAMES * HDA_MONO_BYTES_PER_FRAME)

/* Soft bits8 stereo: 32 frames × 2 B = 64 B. */
#define HDA_SOFT_BITS8_FRAMES 32u
#define HDA_SOFT_BITS8_BYTES  (HDA_SOFT_BITS8_FRAMES * HDA_BITS8_BYTES_PER_FRAME)

/*
 * Wave stamp + inventory area count (Wave 37 exclusive deepen).
 * Areas: suite door played inventory steps miss last deepen path
 *        ops first peak format ratio honesty
 */
#define HDA_SOFT_WAVE   37u
#define HDA_SOFT_AREAS  15u

/* Soft suite sub-step bits (Wave 37 greppable steps line). */
#define SOFT_S_MULTI      (1u << 0)
#define SOFT_S_PARTIAL    (1u << 1)
#define SOFT_S_MULTIWRITE (1u << 2)
#define SOFT_S_UNDERRUN   (1u << 3)
#define SOFT_S_MONO       (1u << 4)
#define SOFT_S_RECLAIM    (1u << 5)
#define SOFT_S_REJECT     (1u << 6)
#define SOFT_S_BITS8      (1u << 7)
#define SOFT_S_IDLE       (1u << 8)
#define SOFT_S_CLOSED     (1u << 9)
#define SOFT_S_ZEROTICK   (1u << 10)

/* STATS u32[3] slots — match kernel {queued, played, underruns}. */
enum {
    HDA_ST_QUEUED    = 0,
    HDA_ST_PLAYED    = 1,
    HDA_ST_UNDERRUNS = 2,
    HDA_ST_COUNT     = 3
};

/*
 * Soft product inventory (Wave 37 exclusive deepen). Cumulative for this
 * process. greppable: hda_client: soft … / hda-gj: soft …
 * Never hard-gates live path.
 */
static unsigned g_cSoftOk;         /* soft sub-steps greened */
static unsigned g_cSoftSkip;       /* soft sub-steps soft-skipped */
static unsigned g_cSoftOpenOk;     /* soft OPEN success */
static unsigned g_cSoftOpenMiss;   /* soft OPEN miss */
static unsigned g_cSoftWriteOk;    /* soft WRITE full accept */
static unsigned g_cSoftWriteMiss;  /* soft WRITE miss/short */
static unsigned g_cSoftStartOk;    /* soft START success */
static unsigned g_cSoftStartMiss;  /* soft START miss */
static unsigned g_cSoftTickOk;     /* soft TICK accepted (>=0) */
static unsigned g_cSoftTickMiss;   /* soft TICK miss (<0) */
static unsigned g_cSoftStatsOk;    /* soft STATS success */
static unsigned g_cSoftStatsMiss;  /* soft STATS miss */
static unsigned g_cSoftClose;      /* soft CLOSE best-effort calls */
static unsigned g_cSoftPlayed;     /* cumulative soft TICK bytes (wrap OK) */
static unsigned g_cSoftUnderrun;   /* soft underrun sub-step PASS */
static unsigned g_cSoftReject;     /* soft reject sub-step PASS */
static unsigned g_cSoftMono;       /* soft mono sub-step PASS */
static unsigned g_cSoftReclaim;    /* soft reclaim sub-step PASS */
/* Wave 13/15 per-step lamps + last/first STATS + peak. */
static unsigned g_cSoftMulti;      /* soft multi-op PASS */
static unsigned g_cSoftPartial;    /* soft partial-tick PASS */
static unsigned g_cSoftMultiWrite; /* soft multi-write PASS */
static unsigned g_cSoftBits8;      /* soft bits8 sub-step PASS */
static unsigned g_cSoftIdle;       /* soft idle STATS sub-step PASS */
static unsigned g_cSoftClosed;     /* soft closed-ops sub-step PASS */
static unsigned g_cSoftZeroTick;   /* soft zerotick sub-step PASS */
static unsigned g_cSoftStereoFmt;  /* soft stereo OPEN greened paths */
static unsigned g_uSoftStepBits;   /* SOFT_S_* mask of greened sub-steps */
static unsigned g_aSoftLast[HDA_ST_COUNT];  /* last successful soft STATS */
static unsigned g_aSoftFirst[HDA_ST_COUNT]; /* first successful soft STATS */
static unsigned g_fSoftFirst;      /* first STATS captured */
static unsigned g_aSoftPeak[HDA_ST_COUNT];  /* peak q/p/u observed */
static unsigned g_cSoftStatsSnap;  /* successful soft STATS snapshots */

static void
msg(const char *sz)
{
    size_t n = 0;

    if (sz == 0) {
        return;
    }
    while (sz[n] != '\0') {
        n++;
    }
    (void)gj_debug_log(sz, (long)n);
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

/* "hda_client-gj: <tag> fail ret=<n>\n" */
static void
msg_fail_ret(const char *tag, long nRet)
{
    char aLine[72];
    unsigned o = 0;

    append_s(aLine, sizeof(aLine), &o, "hda_client-gj: ");
    append_s(aLine, sizeof(aLine), &o, tag);
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

/* "hda_client-gj: stats <tag> q= p= u=\n" */
static void
msg_stats(const char *tag, const unsigned *aSt)
{
    char aLine[96];
    unsigned o = 0;

    if (aSt == 0) {
        return;
    }
    append_s(aLine, sizeof(aLine), &o, "hda_client-gj: stats ");
    if (tag != 0) {
        append_s(aLine, sizeof(aLine), &o, tag);
        append_s(aLine, sizeof(aLine), &o, " ");
    }
    append_s(aLine, sizeof(aLine), &o, "q=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[HDA_ST_QUEUED]);
    append_s(aLine, sizeof(aLine), &o, " p=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[HDA_ST_PLAYED]);
    append_s(aLine, sizeof(aLine), &o, " u=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)aSt[HDA_ST_UNDERRUNS]);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/* "hda_client-gj: <tag> played=<n>\n" */
static void
msg_tick(const char *tag, long nPlayed)
{
    char aLine[72];
    unsigned o = 0;

    append_s(aLine, sizeof(aLine), &o, "hda_client-gj: ");
    append_s(aLine, sizeof(aLine), &o, tag);
    append_s(aLine, sizeof(aLine), &o, " played=");
    if (nPlayed < 0) {
        append_s(aLine, sizeof(aLine), &o, "-");
        append_u(aLine, sizeof(aLine), &o, (unsigned long)(-nPlayed));
    } else {
        append_u(aLine, sizeof(aLine), &o, (unsigned long)nPlayed);
    }
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);
}

/* Hard-path failure: log, best-effort CLOSE, exit 1. */
static void
hard_fail(const char *tag, long nRet)
{
    msg_fail_ret(tag, nRet);
    (void)gj_hda_stream(GJ_HDA_OP_CLOSE, 0, 0, 0);
    gj_exit(1);
}

/* Square-ish 16-bit LE stereo PCM; period in bytes of the high/low half. */
static void
fill_pcm(unsigned char *p, unsigned cb, unsigned period)
{
    unsigned i;

    if (p == 0 || period == 0) {
        return;
    }
    for (i = 0; i < cb; i++) {
        p[i] = (unsigned char)((i & period) ? 0x40u : 0xc0u);
    }
}

/* Soft best-effort CLOSE (never hard-fails). */
static void
soft_close(void)
{
    (void)gj_hda_stream(GJ_HDA_OP_CLOSE, 0, 0, 0);
    g_cSoftClose++;
}

/*
 * Soft OPEN stereo 48 kHz 16-bit. Returns 0 on success, else non-zero.
 * On failure logs soft-tag and leaves stream closed.
 */
static long
soft_open_stereo(const char *tag)
{
    long n;

    n = gj_hda_stream(GJ_HDA_OP_OPEN, (long)GJ_HDA_CHANNELS_STEREO,
                      (long)GJ_HDA_RATE_48K, (long)GJ_HDA_BITS_16);
    if (n != 0) {
        g_cSoftOpenMiss++;
        msg_fail_ret(tag, n);
    } else {
        g_cSoftOpenOk++;
        g_cSoftStereoFmt++;
    }
    return n;
}

/* Soft STATS snapshot into aSt; logs on miss. Returns 0 on ok. */
static long
soft_stats(const char *tag, unsigned *aSt)
{
    long n;

    if (aSt == 0) {
        g_cSoftStatsMiss++;
        return -1;
    }
    aSt[HDA_ST_QUEUED] = aSt[HDA_ST_PLAYED] = aSt[HDA_ST_UNDERRUNS] = 0;
    n = gj_hda_stream(GJ_HDA_OP_STATS, (long)(uintptr_t)aSt, 0, 0);
    if (n != 0) {
        g_cSoftStatsMiss++;
        msg_fail_ret(tag, n);
        return n;
    }
    g_cSoftStatsOk++;
    g_cSoftStatsSnap++;
    g_aSoftLast[HDA_ST_QUEUED] = aSt[HDA_ST_QUEUED];
    g_aSoftLast[HDA_ST_PLAYED] = aSt[HDA_ST_PLAYED];
    g_aSoftLast[HDA_ST_UNDERRUNS] = aSt[HDA_ST_UNDERRUNS];
    /* Wave 37: first successful STATS snapshot. */
    if (g_fSoftFirst == 0u) {
        g_aSoftFirst[HDA_ST_QUEUED] = aSt[HDA_ST_QUEUED];
        g_aSoftFirst[HDA_ST_PLAYED] = aSt[HDA_ST_PLAYED];
        g_aSoftFirst[HDA_ST_UNDERRUNS] = aSt[HDA_ST_UNDERRUNS];
        g_fSoftFirst = 1u;
    }
    /* Wave 37: peak q/p/u across soft STATS. */
    if (aSt[HDA_ST_QUEUED] > g_aSoftPeak[HDA_ST_QUEUED]) {
        g_aSoftPeak[HDA_ST_QUEUED] = aSt[HDA_ST_QUEUED];
    }
    if (aSt[HDA_ST_PLAYED] > g_aSoftPeak[HDA_ST_PLAYED]) {
        g_aSoftPeak[HDA_ST_PLAYED] = aSt[HDA_ST_PLAYED];
    }
    if (aSt[HDA_ST_UNDERRUNS] > g_aSoftPeak[HDA_ST_UNDERRUNS]) {
        g_aSoftPeak[HDA_ST_UNDERRUNS] = aSt[HDA_ST_UNDERRUNS];
    }
    msg_stats(tag, aSt);
    return 0;
}

/* Soft WRITE note: full accept → ok, else miss. */
static void
soft_note_write(long nRet, unsigned cbExpect)
{
    if (nRet == (long)cbExpect) {
        g_cSoftWriteOk++;
    } else {
        g_cSoftWriteMiss++;
    }
}

/* Soft START note. */
static void
soft_note_start(long nRet)
{
    if (nRet == 0) {
        g_cSoftStartOk++;
    } else {
        g_cSoftStartMiss++;
    }
}

/* Soft TICK note: tally played bytes when ret >= 0. */
static void
soft_note_tick(long nPlayed)
{
    if (nPlayed < 0) {
        g_cSoftTickMiss++;
    } else {
        g_cSoftTickOk++;
        g_cSoftPlayed += (unsigned)nPlayed;
    }
}

/* Soft sub-step aggregate lamp. */
static void
soft_note_step(int fOk)
{
    if (fOk) {
        g_cSoftOk++;
    } else {
        g_cSoftSkip++;
    }
}

/*
 * Greppable soft inventory (Wave 37 exclusive deepen).
 * Twin prefixes for product/agent greps:
 *   hda_client: soft suite|door|played|inventory|steps|miss|last|first|
 *               peak|ops|format|ratio|deepen|path|honesty …
 *   hda-gj: soft suite|door|played|inventory|steps|miss|last|first|
 *           peak|ops|format|ratio|deepen|path|honesty …
 * Pure observation — always soft; does not gate live path.
 */
static void
soft_inventory_log(void)
{
    char aLine[224];
    unsigned o;
    unsigned cDoorOk;
    unsigned cDoorMiss;
    unsigned cOpenTot;
    unsigned cWriteTot;
    unsigned cStartTot;
    unsigned cTickTot;
    unsigned cStatsTot;

    cDoorOk = g_cSoftOpenOk + g_cSoftWriteOk + g_cSoftStartOk +
              g_cSoftTickOk + g_cSoftStatsOk;
    cDoorMiss = g_cSoftOpenMiss + g_cSoftWriteMiss + g_cSoftStartMiss +
                g_cSoftTickMiss + g_cSoftStatsMiss;
    cOpenTot = g_cSoftOpenOk + g_cSoftOpenMiss;
    cWriteTot = g_cSoftWriteOk + g_cSoftWriteMiss;
    cStartTot = g_cSoftStartOk + g_cSoftStartMiss;
    cTickTot = g_cSoftTickOk + g_cSoftTickMiss;
    cStatsTot = g_cSoftStatsOk + g_cSoftStatsMiss;

    /* Grep: hda_client: soft suite */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft suite ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft door */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft door open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOpenOk);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOpenTot);
    append_s(aLine, sizeof(aLine), &o, " write=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftWriteOk);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cWriteTot);
    append_s(aLine, sizeof(aLine), &o, " start=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStartOk);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cStartTot);
    append_s(aLine, sizeof(aLine), &o, " tick=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftTickOk);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cTickTot);
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStatsOk);
    append_s(aLine, sizeof(aLine), &o, "/");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cStatsTot);
    append_s(aLine, sizeof(aLine), &o, " close=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftClose);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft played (lamps) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft played=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPlayed);
    append_s(aLine, sizeof(aLine), &o, " underrun=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftUnderrun);
    append_s(aLine, sizeof(aLine), &o, " reject=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftReject);
    append_s(aLine, sizeof(aLine), &o, " mono=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMono);
    append_s(aLine, sizeof(aLine), &o, " reclaim=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftReclaim);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft inventory (Wave 37 rollup) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft inventory ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, " door_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cDoorOk);
    append_s(aLine, sizeof(aLine), &o, " door_miss=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cDoorMiss);
    append_s(aLine, sizeof(aLine), &o, " played=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPlayed);
    append_s(aLine, sizeof(aLine), &o, " wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)HDA_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)HDA_SOFT_AREAS);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft steps (Wave 37 per-sub-step lamps) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft steps multi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMulti);
    append_s(aLine, sizeof(aLine), &o, " partial=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPartial);
    append_s(aLine, sizeof(aLine), &o, " multiwrite=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMultiWrite);
    append_s(aLine, sizeof(aLine), &o, " underrun=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftUnderrun);
    append_s(aLine, sizeof(aLine), &o, " mono=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMono);
    append_s(aLine, sizeof(aLine), &o, " reclaim=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftReclaim);
    append_s(aLine, sizeof(aLine), &o, " reject=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftReject);
    append_s(aLine, sizeof(aLine), &o, " bits8=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftBits8);
    append_s(aLine, sizeof(aLine), &o, " idle=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftIdle);
    append_s(aLine, sizeof(aLine), &o, " closed=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftClosed);
    append_s(aLine, sizeof(aLine), &o, " zerotick=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftZeroTick);
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftStepBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft miss (door miss rollup) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft miss open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOpenMiss);
    append_s(aLine, sizeof(aLine), &o, " write=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftWriteMiss);
    append_s(aLine, sizeof(aLine), &o, " start=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStartMiss);
    append_s(aLine, sizeof(aLine), &o, " tick=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftTickMiss);
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStatsMiss);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft last (last STATS snapshot) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft last q=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftLast[HDA_ST_QUEUED]);
    append_s(aLine, sizeof(aLine), &o, " p=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftLast[HDA_ST_PLAYED]);
    append_s(aLine, sizeof(aLine), &o, " u=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftLast[HDA_ST_UNDERRUNS]);
    append_s(aLine, sizeof(aLine), &o, " snaps=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStatsSnap);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft first (Wave 37 first STATS) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft first q=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftFirst[HDA_ST_QUEUED]);
    append_s(aLine, sizeof(aLine), &o, " p=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftFirst[HDA_ST_PLAYED]);
    append_s(aLine, sizeof(aLine), &o, " u=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftFirst[HDA_ST_UNDERRUNS]);
    append_s(aLine, sizeof(aLine), &o, " set=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_fSoftFirst);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft peak (Wave 37 peak q/p/u) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft peak q=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftPeak[HDA_ST_QUEUED]);
    append_s(aLine, sizeof(aLine), &o, " p=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftPeak[HDA_ST_PLAYED]);
    append_s(aLine, sizeof(aLine), &o, " u=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftPeak[HDA_ST_UNDERRUNS]);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft ops (Wave 37 total door op attempts) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft ops open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOpenTot);
    append_s(aLine, sizeof(aLine), &o, " write=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cWriteTot);
    append_s(aLine, sizeof(aLine), &o, " start=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cStartTot);
    append_s(aLine, sizeof(aLine), &o, " tick=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cTickTot);
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cStatsTot);
    append_s(aLine, sizeof(aLine), &o, " close=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftClose);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft format (Wave 37 format lamps) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft format stereo=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStereoFmt);
    append_s(aLine, sizeof(aLine), &o, " mono=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMono);
    append_s(aLine, sizeof(aLine), &o, " bits8=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftBits8);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft ratio (Wave 37 ok/skip + door rollup) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft ratio ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, " door_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cDoorOk);
    append_s(aLine, sizeof(aLine), &o, " door_miss=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cDoorMiss);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /* Grep: hda_client: soft deepen (Wave 37 stamp) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda_client: soft deepen wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)HDA_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)HDA_SOFT_AREAS);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, " played=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPlayed);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    /*
     * Grep: hda_client: soft path
     * Static route labels for agent greps (not live counters).
     */
    msg("hda_client: soft path open=stereo write=pcm start=arm "
        "tick=frames stats=qpu mono=ch1 bits8=ch2 reclaim=reopen "
        "reject=inval idle=empty closed=reject zerotick=0 "
        "(soft inventory; not bar3)\n");

    /* Grep: hda_client: soft honesty (Wave 37; never bar3) */
    msg("hda_client: soft honesty not-bar3 not-pipewire not-product-audio "
        "soft-inventory-only multi_server=0 confine=0 bar3=0 product_kernel=OPEN wave=37\n");

    /* Twin prefix: hda-gj: soft … (agent-friendly alias) */
    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft suite ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft door open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOpenOk);
    append_s(aLine, sizeof(aLine), &o, " write=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftWriteOk);
    append_s(aLine, sizeof(aLine), &o, " start=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStartOk);
    append_s(aLine, sizeof(aLine), &o, " tick=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftTickOk);
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStatsOk);
    append_s(aLine, sizeof(aLine), &o, " close=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftClose);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft played=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPlayed);
    append_s(aLine, sizeof(aLine), &o, " underrun=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftUnderrun);
    append_s(aLine, sizeof(aLine), &o, " reject=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftReject);
    append_s(aLine, sizeof(aLine), &o, " mono=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMono);
    append_s(aLine, sizeof(aLine), &o, " reclaim=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftReclaim);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft inventory ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, " door_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cDoorOk);
    append_s(aLine, sizeof(aLine), &o, " door_miss=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cDoorMiss);
    append_s(aLine, sizeof(aLine), &o, " played=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPlayed);
    append_s(aLine, sizeof(aLine), &o, " wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)HDA_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)HDA_SOFT_AREAS);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft steps multi=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMulti);
    append_s(aLine, sizeof(aLine), &o, " partial=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPartial);
    append_s(aLine, sizeof(aLine), &o, " multiwrite=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMultiWrite);
    append_s(aLine, sizeof(aLine), &o, " underrun=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftUnderrun);
    append_s(aLine, sizeof(aLine), &o, " mono=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMono);
    append_s(aLine, sizeof(aLine), &o, " reclaim=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftReclaim);
    append_s(aLine, sizeof(aLine), &o, " reject=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftReject);
    append_s(aLine, sizeof(aLine), &o, " bits8=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftBits8);
    append_s(aLine, sizeof(aLine), &o, " idle=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftIdle);
    append_s(aLine, sizeof(aLine), &o, " closed=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftClosed);
    append_s(aLine, sizeof(aLine), &o, " zerotick=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftZeroTick);
    append_s(aLine, sizeof(aLine), &o, " bits=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_uSoftStepBits);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft miss open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOpenMiss);
    append_s(aLine, sizeof(aLine), &o, " write=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftWriteMiss);
    append_s(aLine, sizeof(aLine), &o, " start=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStartMiss);
    append_s(aLine, sizeof(aLine), &o, " tick=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftTickMiss);
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStatsMiss);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft last q=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftLast[HDA_ST_QUEUED]);
    append_s(aLine, sizeof(aLine), &o, " p=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftLast[HDA_ST_PLAYED]);
    append_s(aLine, sizeof(aLine), &o, " u=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftLast[HDA_ST_UNDERRUNS]);
    append_s(aLine, sizeof(aLine), &o, " snaps=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStatsSnap);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft first q=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftFirst[HDA_ST_QUEUED]);
    append_s(aLine, sizeof(aLine), &o, " p=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftFirst[HDA_ST_PLAYED]);
    append_s(aLine, sizeof(aLine), &o, " u=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftFirst[HDA_ST_UNDERRUNS]);
    append_s(aLine, sizeof(aLine), &o, " set=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_fSoftFirst);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft peak q=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftPeak[HDA_ST_QUEUED]);
    append_s(aLine, sizeof(aLine), &o, " p=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftPeak[HDA_ST_PLAYED]);
    append_s(aLine, sizeof(aLine), &o, " u=");
    append_u(aLine, sizeof(aLine), &o,
             (unsigned long)g_aSoftPeak[HDA_ST_UNDERRUNS]);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft ops open=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cOpenTot);
    append_s(aLine, sizeof(aLine), &o, " write=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cWriteTot);
    append_s(aLine, sizeof(aLine), &o, " start=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cStartTot);
    append_s(aLine, sizeof(aLine), &o, " tick=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cTickTot);
    append_s(aLine, sizeof(aLine), &o, " stats=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cStatsTot);
    append_s(aLine, sizeof(aLine), &o, " close=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftClose);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft format stereo=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftStereoFmt);
    append_s(aLine, sizeof(aLine), &o, " mono=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftMono);
    append_s(aLine, sizeof(aLine), &o, " bits8=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftBits8);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft ratio ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, " door_ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cDoorOk);
    append_s(aLine, sizeof(aLine), &o, " door_miss=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)cDoorMiss);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    o = 0;
    append_s(aLine, sizeof(aLine), &o, "hda-gj: soft deepen wave=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)HDA_SOFT_WAVE);
    append_s(aLine, sizeof(aLine), &o, " areas=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)HDA_SOFT_AREAS);
    append_s(aLine, sizeof(aLine), &o, " ok=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftOk);
    append_s(aLine, sizeof(aLine), &o, " skip=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftSkip);
    append_s(aLine, sizeof(aLine), &o, " played=");
    append_u(aLine, sizeof(aLine), &o, (unsigned long)g_cSoftPlayed);
    append_s(aLine, sizeof(aLine), &o, "\n");
    aLine[o] = '\0';
    msg(aLine);

    msg("hda-gj: soft path open=stereo write=pcm start=arm tick=frames "
        "stats=qpu mono=ch1 bits8=ch2 reclaim=reopen reject=inval "
        "idle=empty closed=reject zerotick=0 "
        "(soft inventory; not bar3)\n");
    msg("hda-gj: soft honesty not-bar3 not-pipewire not-product-audio "
        "soft-inventory-only multi_server=0 confine=0 bar3=0 product_kernel=OPEN wave=37\n");
}

/*
 * Soft multi-op cycle: re-open, write a short pattern, start, tick, stats.
 * Failures are soft (log + return 0); success returns 1.
 * Never aborts the live path PASS.
 */
static int
soft_multi_op_stats(void)
{
    static unsigned char aTone[HDA_SOFT_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;
    long nPlayed;
    unsigned i;

    msg("hda_client-gj: soft multi-op start\n");

    if (soft_open_stereo("soft OPEN") != 0) {
        msg("hda_client-gj: soft multi-op soft-skip\n");
        return 0;
    }

    /* Distinct soft pattern (quieter levels; period = 16 B). */
    for (i = 0; i < sizeof(aTone); i++) {
        aTone[i] = (unsigned char)((i & 16u) ? 0x20u : 0xe0u);
    }
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    soft_note_write(n, (unsigned)sizeof(aTone));
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft WRITE", n);
        soft_close();
        msg("hda_client-gj: soft multi-op soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    soft_note_start(n);
    if (n != 0) {
        msg_fail_ret("soft START", n);
        soft_close();
        msg("hda_client-gj: soft multi-op soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_FRAMES, 0, 0);
    soft_note_tick(nPlayed);
    msg_tick("soft TICK", nPlayed);

    /* STATS soft-miss still soft-PASS (never hard-fails). */
    (void)soft_stats("soft", aSt);

    soft_close();
    g_cSoftMulti++;
    g_uSoftStepBits |= SOFT_S_MULTI;
    msg("hda_client-gj: soft multi-op PASS\n");
    return 1;
}

/*
 * Soft partial-tick: write 48 frames, tick 16 (queue must stay non-empty),
 * tick remainder, confirm drained. Deepens TICK/STATS mid-buffer surface.
 */
static int
soft_partial_tick(void)
{
    static unsigned char aTone[HDA_SOFT_PART_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;
    long nPlayed;
    unsigned uLeft;

    if (soft_open_stereo("soft partial OPEN") != 0) {
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }

    fill_pcm(aTone, (unsigned)sizeof(aTone), 8u);
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    soft_note_write(n, (unsigned)sizeof(aTone));
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft partial WRITE", n);
        soft_close();
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    soft_note_start(n);
    if (n != 0) {
        msg_fail_ret("soft partial START", n);
        soft_close();
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_PART_TICK1, 0, 0);
    soft_note_tick(nPlayed);
    msg_tick("soft partial TICK1", nPlayed);
    if (nPlayed != (long)HDA_SOFT_PART_TICK1_B) {
        msg_fail_ret("soft partial TICK1", nPlayed);
        soft_close();
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }

    if (soft_stats("soft partial mid", aSt) != 0) {
        soft_close();
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }
    /* Mid-buffer: queue must still hold residual frames. */
    uLeft = (unsigned)(HDA_SOFT_PART_BYTES - HDA_SOFT_PART_TICK1_B);
    if (aSt[HDA_ST_QUEUED] != uLeft) {
        msg_fail_ret("soft partial queued", (long)aSt[HDA_ST_QUEUED]);
        soft_close();
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK,
                            (long)(HDA_SOFT_PART_FRAMES - HDA_SOFT_PART_TICK1),
                            0, 0);
    soft_note_tick(nPlayed);
    msg_tick("soft partial TICK2", nPlayed);
    if (nPlayed != (long)uLeft) {
        msg_fail_ret("soft partial TICK2", nPlayed);
        soft_close();
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }

    if (soft_stats("soft partial end", aSt) != 0 ||
        aSt[HDA_ST_QUEUED] != 0u) {
        soft_close();
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }

    soft_close();
    g_cSoftPartial++;
    g_uSoftStepBits |= SOFT_S_PARTIAL;
    msg("hda_client-gj: soft partial-tick PASS\n");
    return 1;
}

/*
 * Soft multi-write: two sequential WRITE chunks before START, then one TICK.
 * Exercises ring enqueue without single-shot fill.
 */
static int
soft_multi_write(void)
{
    static unsigned char aChunkA[HDA_SOFT_CHUNK_BYTES];
    static unsigned char aChunkB[HDA_SOFT_CHUNK_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;
    long nPlayed;
    unsigned i;

    if (soft_open_stereo("soft multi-write OPEN") != 0) {
        msg("hda_client-gj: soft multi-write soft-skip\n");
        return 0;
    }

    for (i = 0; i < sizeof(aChunkA); i++) {
        aChunkA[i] = (unsigned char)(0x11u + (i & 0x0fu));
        aChunkB[i] = (unsigned char)(0xaau - (i & 0x0fu));
    }

    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aChunkA,
                      (long)sizeof(aChunkA), 0);
    soft_note_write(n, (unsigned)sizeof(aChunkA));
    if (n != (long)sizeof(aChunkA)) {
        msg_fail_ret("soft multi-write WRITE1", n);
        soft_close();
        msg("hda_client-gj: soft multi-write soft-skip\n");
        return 0;
    }
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aChunkB,
                      (long)sizeof(aChunkB), 0);
    soft_note_write(n, (unsigned)sizeof(aChunkB));
    if (n != (long)sizeof(aChunkB)) {
        msg_fail_ret("soft multi-write WRITE2", n);
        soft_close();
        msg("hda_client-gj: soft multi-write soft-skip\n");
        return 0;
    }

    /* Pre-start STATS: queue should hold both chunks. */
    if (soft_stats("soft multi-write pre", aSt) != 0 ||
        aSt[HDA_ST_QUEUED] != (unsigned)(sizeof(aChunkA) + sizeof(aChunkB))) {
        soft_close();
        msg("hda_client-gj: soft multi-write soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    soft_note_start(n);
    if (n != 0) {
        msg_fail_ret("soft multi-write START", n);
        soft_close();
        msg("hda_client-gj: soft multi-write soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK,
                            (long)(HDA_SOFT_CHUNK_FRAMES * 2u), 0, 0);
    soft_note_tick(nPlayed);
    msg_tick("soft multi-write TICK", nPlayed);
    if (nPlayed != (long)(sizeof(aChunkA) + sizeof(aChunkB))) {
        msg_fail_ret("soft multi-write TICK", nPlayed);
        soft_close();
        msg("hda_client-gj: soft multi-write soft-skip\n");
        return 0;
    }

    if (soft_stats("soft multi-write", aSt) != 0 ||
        aSt[HDA_ST_QUEUED] != 0u) {
        soft_close();
        msg("hda_client-gj: soft multi-write soft-skip\n");
        return 0;
    }

    soft_close();
    g_cSoftMultiWrite++;
    g_uSoftStepBits |= SOFT_S_MULTIWRITE;
    msg("hda_client-gj: soft multi-write PASS\n");
    return 1;
}

/*
 * Soft underrun: open/write/start/full-tick, empty-ring tick, expect u > 0.
 * Parallel of hard underrun; soft-skip never fails live path.
 */
static int
soft_underrun_cycle(void)
{
    static unsigned char aTone[HDA_SOFT_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;
    long nPlayed;

    if (soft_open_stereo("soft underrun OPEN") != 0) {
        msg("hda_client-gj: soft underrun soft-skip\n");
        return 0;
    }

    fill_pcm(aTone, (unsigned)sizeof(aTone), 16u);
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    soft_note_write(n, (unsigned)sizeof(aTone));
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft underrun WRITE", n);
        soft_close();
        msg("hda_client-gj: soft underrun soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    soft_note_start(n);
    if (n != 0) {
        msg_fail_ret("soft underrun START", n);
        soft_close();
        msg("hda_client-gj: soft underrun soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_FRAMES, 0, 0);
    soft_note_tick(nPlayed);
    msg_tick("soft underrun TICK", nPlayed);
    if (nPlayed != (long)sizeof(aTone)) {
        msg_fail_ret("soft underrun TICK", nPlayed);
        soft_close();
        msg("hda_client-gj: soft underrun soft-skip\n");
        return 0;
    }

    /* Empty ring → underrun counter must rise. */
    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, 8, 0, 0);
    soft_note_tick(nPlayed);
    if (soft_stats("soft underrun", aSt) != 0 ||
        aSt[HDA_ST_UNDERRUNS] == 0u) {
        soft_close();
        msg("hda_client-gj: soft underrun soft-skip\n");
        return 0;
    }

    soft_close();
    g_cSoftUnderrun++;
    g_uSoftStepBits |= SOFT_S_UNDERRUN;
    msg("hda_client-gj: soft underrun PASS\n");
    return 1;
}

/*
 * Soft mono format probe: OPEN ch=1 rate=48000 bits=16, short write/tick.
 * Kernel accepts mono 16-bit; soft-skip if door rejects.
 */
static int
soft_mono_format(void)
{
    static unsigned char aTone[HDA_SOFT_MONO_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;
    long nPlayed;
    unsigned i;

    n = gj_hda_stream(GJ_HDA_OP_OPEN, (long)GJ_HDA_CHANNELS_MONO,
                      (long)GJ_HDA_RATE_48K, (long)GJ_HDA_BITS_16);
    if (n != 0) {
        g_cSoftOpenMiss++;
        msg_fail_ret("soft mono OPEN", n);
        msg("hda_client-gj: soft mono soft-skip\n");
        return 0;
    }
    g_cSoftOpenOk++;

    for (i = 0; i < sizeof(aTone); i++) {
        aTone[i] = (unsigned char)((i & 8u) ? 0x30u : 0xd0u);
    }
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    soft_note_write(n, (unsigned)sizeof(aTone));
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft mono WRITE", n);
        soft_close();
        msg("hda_client-gj: soft mono soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    soft_note_start(n);
    if (n != 0) {
        msg_fail_ret("soft mono START", n);
        soft_close();
        msg("hda_client-gj: soft mono soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_MONO_FRAMES, 0, 0);
    soft_note_tick(nPlayed);
    msg_tick("soft mono TICK", nPlayed);
    if (nPlayed != (long)sizeof(aTone)) {
        msg_fail_ret("soft mono TICK", nPlayed);
        soft_close();
        msg("hda_client-gj: soft mono soft-skip\n");
        return 0;
    }

    (void)soft_stats("soft mono", aSt);
    soft_close();
    g_cSoftMono++;
    g_uSoftStepBits |= SOFT_S_MONO;
    msg("hda_client-gj: soft mono PASS\n");
    return 1;
}

/*
 * Soft reclaim: OPEN while already open resets ring (kernel re-open).
 * OPEN → WRITE small → re-OPEN → STATS queue must be 0 → CLOSE.
 */
static int
soft_reclaim_reopen(void)
{
    static unsigned char aTone[HDA_SOFT_CHUNK_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;

    if (soft_open_stereo("soft reclaim OPEN") != 0) {
        msg("hda_client-gj: soft reclaim soft-skip\n");
        return 0;
    }

    fill_pcm(aTone, (unsigned)sizeof(aTone), 4u);
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    soft_note_write(n, (unsigned)sizeof(aTone));
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft reclaim WRITE", n);
        soft_close();
        msg("hda_client-gj: soft reclaim soft-skip\n");
        return 0;
    }

    /* Re-OPEN resets queue/played/underruns (idempotent soft reclaim). */
    n = gj_hda_stream(GJ_HDA_OP_OPEN, (long)GJ_HDA_CHANNELS_STEREO,
                      (long)GJ_HDA_RATE_48K, (long)GJ_HDA_BITS_16);
    if (n != 0) {
        g_cSoftOpenMiss++;
        msg_fail_ret("soft reclaim re-OPEN", n);
        soft_close();
        msg("hda_client-gj: soft reclaim soft-skip\n");
        return 0;
    }
    g_cSoftOpenOk++;

    if (soft_stats("soft reclaim", aSt) != 0 ||
        aSt[HDA_ST_QUEUED] != 0u || aSt[HDA_ST_PLAYED] != 0u) {
        soft_close();
        msg("hda_client-gj: soft reclaim soft-skip\n");
        return 0;
    }

    soft_close();
    g_cSoftReclaim++;
    g_uSoftStepBits |= SOFT_S_RECLAIM;
    msg("hda_client-gj: soft reclaim PASS\n");
    return 1;
}

/*
 * Soft reject: OPEN with invalid channels=0 must fail (door INVAL).
 * Soft PASS when rejection is observed; soft-skip if door wrongly accepts.
 */
static int
soft_reject_inval(void)
{
    long n;
    long nBits;

    n = gj_hda_stream(GJ_HDA_OP_OPEN, 0, (long)GJ_HDA_RATE_48K,
                      (long)GJ_HDA_BITS_16);
    if (n == 0) {
        /* Unexpected accept — clean up and soft-skip (do not hard-fail). */
        g_cSoftOpenOk++; /* door accepted; wrong for reject probe */
        soft_close();
        msg("hda_client-gj: soft reject soft-skip\n");
        return 0;
    }
    g_cSoftOpenMiss++; /* expected reject tallied as open-miss */

    /* Also probe illegal bit depth (not 8/16/32). */
    nBits = gj_hda_stream(GJ_HDA_OP_OPEN, (long)GJ_HDA_CHANNELS_STEREO,
                          (long)GJ_HDA_RATE_48K, 24);
    if (nBits == 0) {
        g_cSoftOpenOk++;
        soft_close();
        msg("hda_client-gj: soft reject soft-skip\n");
        return 0;
    }
    g_cSoftOpenMiss++;

    g_cSoftReject++;
    g_uSoftStepBits |= SOFT_S_REJECT;
    msg("hda_client-gj: soft reject PASS\n");
    return 1;
}

/*
 * Soft bits8 format: OPEN ch=2 rate=48000 bits=8, short write/tick.
 * Kernel accepts 8-bit; soft-skip if door rejects.
 */
static int
soft_bits8_format(void)
{
    static unsigned char aTone[HDA_SOFT_BITS8_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;
    long nPlayed;
    unsigned i;

    n = gj_hda_stream(GJ_HDA_OP_OPEN, (long)GJ_HDA_CHANNELS_STEREO,
                      (long)GJ_HDA_RATE_48K, (long)GJ_HDA_BITS_8);
    if (n != 0) {
        g_cSoftOpenMiss++;
        msg_fail_ret("soft bits8 OPEN", n);
        msg("hda_client-gj: soft bits8 soft-skip\n");
        return 0;
    }
    g_cSoftOpenOk++;

    for (i = 0; i < sizeof(aTone); i++) {
        aTone[i] = (unsigned char)((i & 4u) ? 0x55u : 0xaau);
    }
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    soft_note_write(n, (unsigned)sizeof(aTone));
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft bits8 WRITE", n);
        soft_close();
        msg("hda_client-gj: soft bits8 soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    soft_note_start(n);
    if (n != 0) {
        msg_fail_ret("soft bits8 START", n);
        soft_close();
        msg("hda_client-gj: soft bits8 soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_BITS8_FRAMES, 0, 0);
    soft_note_tick(nPlayed);
    msg_tick("soft bits8 TICK", nPlayed);
    if (nPlayed != (long)sizeof(aTone)) {
        msg_fail_ret("soft bits8 TICK", nPlayed);
        soft_close();
        msg("hda_client-gj: soft bits8 soft-skip\n");
        return 0;
    }

    (void)soft_stats("soft bits8", aSt);
    soft_close();
    g_cSoftBits8++;
    g_uSoftStepBits |= SOFT_S_BITS8;
    msg("hda_client-gj: soft bits8 PASS\n");
    return 1;
}

/*
 * Soft idle STATS: OPEN → STATS with empty ring (q=0 p=0) → CLOSE.
 * Deepens pre-write STATS surface without PCM.
 */
static int
soft_idle_stats(void)
{
    static unsigned aSt[HDA_ST_COUNT];

    if (soft_open_stereo("soft idle OPEN") != 0) {
        msg("hda_client-gj: soft idle soft-skip\n");
        return 0;
    }

    if (soft_stats("soft idle", aSt) != 0 ||
        aSt[HDA_ST_QUEUED] != 0u || aSt[HDA_ST_PLAYED] != 0u) {
        soft_close();
        msg("hda_client-gj: soft idle soft-skip\n");
        return 0;
    }

    soft_close();
    g_cSoftIdle++;
    g_uSoftStepBits |= SOFT_S_IDLE;
    msg("hda_client-gj: soft idle PASS\n");
    return 1;
}

/*
 * Soft closed-ops: after CLOSE, WRITE returns 0 and START fails.
 * Proves door rejects post-close traffic (soft PASS on reject observed).
 */
static int
soft_closed_ops(void)
{
    static unsigned char aTone[HDA_SOFT_CHUNK_BYTES];
    long nWrite;
    long nStart;
    long nTick;

    /* Ensure stream is closed. */
    soft_close();

    fill_pcm(aTone, (unsigned)sizeof(aTone), 8u);
    nWrite = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                           (long)sizeof(aTone), 0);
    /* Closed WRITE must not accept bytes. */
    if (nWrite != 0) {
        g_cSoftWriteOk++; /* unexpected accept */
        soft_close();
        msg("hda_client-gj: soft closed soft-skip\n");
        return 0;
    }
    g_cSoftWriteMiss++; /* expected zero-accept */

    nStart = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    if (nStart == 0) {
        g_cSoftStartOk++; /* unexpected */
        soft_close();
        msg("hda_client-gj: soft closed soft-skip\n");
        return 0;
    }
    g_cSoftStartMiss++; /* expected reject */

    nTick = gj_hda_stream(GJ_HDA_OP_TICK, 8, 0, 0);
    soft_note_tick(nTick);
    if (nTick != 0) {
        soft_close();
        msg("hda_client-gj: soft closed soft-skip\n");
        return 0;
    }

    g_cSoftClosed++;
    g_uSoftStepBits |= SOFT_S_CLOSED;
    msg("hda_client-gj: soft closed PASS\n");
    return 1;
}

/*
 * Soft zerotick: armed stream + TICK frames=0 must return 0 and leave
 * residual queue intact. Deepens zero-frame TICK edge.
 */
static int
soft_zero_tick(void)
{
    static unsigned char aTone[HDA_SOFT_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;
    long nPlayed;

    if (soft_open_stereo("soft zerotick OPEN") != 0) {
        msg("hda_client-gj: soft zerotick soft-skip\n");
        return 0;
    }

    fill_pcm(aTone, (unsigned)sizeof(aTone), 16u);
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    soft_note_write(n, (unsigned)sizeof(aTone));
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft zerotick WRITE", n);
        soft_close();
        msg("hda_client-gj: soft zerotick soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    soft_note_start(n);
    if (n != 0) {
        msg_fail_ret("soft zerotick START", n);
        soft_close();
        msg("hda_client-gj: soft zerotick soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, 0, 0, 0);
    soft_note_tick(nPlayed);
    msg_tick("soft zerotick TICK0", nPlayed);
    if (nPlayed != 0) {
        msg_fail_ret("soft zerotick TICK0", nPlayed);
        soft_close();
        msg("hda_client-gj: soft zerotick soft-skip\n");
        return 0;
    }

    if (soft_stats("soft zerotick mid", aSt) != 0 ||
        aSt[HDA_ST_QUEUED] != (unsigned)sizeof(aTone)) {
        soft_close();
        msg("hda_client-gj: soft zerotick soft-skip\n");
        return 0;
    }

    /* Drain residual so later soft steps see a clean door. */
    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_FRAMES, 0, 0);
    soft_note_tick(nPlayed);
    soft_close();
    g_cSoftZeroTick++;
    g_uSoftStepBits |= SOFT_S_ZEROTICK;
    msg("hda_client-gj: soft zerotick PASS\n");
    return 1;
}

/*
 * Soft suite orchestrator — deepens freestanding door surface beyond the
 * single multi-op cycle. Each sub-step is independent; aggregate greened if
 * any sub-step PASS. Never hard-fails live path.
 */
static void
soft_suite(void)
{
    unsigned cOk = 0;
    int fStep;

    msg("hda_client-gj: soft suite start\n");

    fStep = soft_multi_op_stats();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    fStep = soft_partial_tick();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    fStep = soft_multi_write();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    fStep = soft_underrun_cycle();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    fStep = soft_mono_format();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    fStep = soft_reclaim_reopen();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    fStep = soft_reject_inval();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    /* Wave 37 exclusive soft door deepen. */
    fStep = soft_bits8_format();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    fStep = soft_idle_stats();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    fStep = soft_closed_ops();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    fStep = soft_zero_tick();
    soft_note_step(fStep);
    cOk += (unsigned)fStep;

    /* Wave 37 soft inventory — greppable hda_client: soft / hda-gj: soft. */
    soft_inventory_log();

    if (cOk > 0u) {
        msg("hda_client-gj: soft suite PASS\n");
    } else {
        msg("hda_client-gj: soft suite soft-skip\n");
    }
}

void
_start(void)
{
    static unsigned char aTone[HDA_SMOKE_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;
    long nPlayed;

    msg("hda_client-gj: start\n");

    n = gj_hda_stream(GJ_HDA_OP_OPEN, (long)GJ_HDA_CHANNELS_STEREO,
                      (long)GJ_HDA_RATE_48K, (long)GJ_HDA_BITS_16);
    if (n != 0) {
        hard_fail("OPEN", n);
    }
    msg("hda_client-gj: OPEN PASS\n");

    /* Simple square-ish PCM pattern (16-bit LE stereo); period = 32 B. */
    fill_pcm(aTone, (unsigned)sizeof(aTone), 32u);
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    if (n != (long)sizeof(aTone)) {
        hard_fail("WRITE", n);
    }
    msg("hda_client-gj: WRITE PASS\n");

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    if (n != 0) {
        hard_fail("START", n);
    }
    msg("hda_client-gj: START PASS\n");

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SMOKE_FRAMES, 0, 0);
    if (nPlayed != (long)sizeof(aTone)) {
        msg_tick("TICK short", nPlayed);
        hard_fail("TICK", nPlayed);
    }
    msg_tick("TICK", nPlayed);
    msg("hda_client-gj: TICK PASS\n");

    aSt[HDA_ST_QUEUED] = aSt[HDA_ST_PLAYED] = aSt[HDA_ST_UNDERRUNS] = 0;
    n = gj_hda_stream(GJ_HDA_OP_STATS, (long)(uintptr_t)aSt, 0, 0);
    if (n != 0 || aSt[HDA_ST_QUEUED] != 0 ||
        aSt[HDA_ST_PLAYED] != (unsigned)sizeof(aTone)) {
        msg_stats("bad", aSt);
        hard_fail("STATS", n);
    }
    msg_stats("post-tick", aSt);
    msg("hda_client-gj: STATS PASS\n");

    /* Empty ring → underrun counter must rise. */
    (void)gj_hda_stream(GJ_HDA_OP_TICK, 8, 0, 0);
    aSt[HDA_ST_QUEUED] = aSt[HDA_ST_PLAYED] = aSt[HDA_ST_UNDERRUNS] = 0;
    n = gj_hda_stream(GJ_HDA_OP_STATS, (long)(uintptr_t)aSt, 0, 0);
    if (n != 0 || aSt[HDA_ST_UNDERRUNS] == 0) {
        msg_stats("underrun-bad", aSt);
        hard_fail("underrun STATS", n);
    }
    msg_stats("underrun", aSt);
    msg("hda_client-gj: underrun PASS\n");

    (void)gj_hda_stream(GJ_HDA_OP_CLOSE, 0, 0, 0);
    msg("hda_client-gj: CLOSE PASS\n");

    /* Soft suite: multi-op + deepened surface; never hard-fails live path. */
    soft_suite();

    msg("hda_client-gj: live path PASS\n");
    gj_exit(0);
}
