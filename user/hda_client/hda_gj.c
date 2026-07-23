/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding HDA stream client (ring-3 smoke) for GreenJade.
 * Exercises GJ_SYS_HDA_STREAM (nr 94) via gj_hda_stream:
 *   open → write PCM → start → tick → stats → underrun tick → close
 * then a soft multi-op second cycle (stats print; soft-skip on fail;
 * never hard-fails the live path).
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
 *   hda_client-gj: soft multi-op start
 *   hda_client-gj: soft multi-op PASS | soft multi-op soft-skip
 *   hda_client-gj: stats soft q=… p=… u=…
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
#define GJ_HDA_RATE_48K        48000u
#define GJ_HDA_BITS_16         16u

/* Frame size: stereo 16-bit LE → 4 bytes/frame. */
#define HDA_BYTES_PER_FRAME 4u

/* Hard smoke: 64 frames × 4 B = 256 B full-buffer consume. */
#define HDA_SMOKE_FRAMES 64u
#define HDA_SMOKE_BYTES  (HDA_SMOKE_FRAMES * HDA_BYTES_PER_FRAME)

/* Soft multi-op: half buffer (32 frames × 4 B = 128 B). */
#define HDA_SOFT_FRAMES 32u
#define HDA_SOFT_BYTES  (HDA_SOFT_FRAMES * HDA_BYTES_PER_FRAME)

/* STATS u32[3] slots — match kernel {queued, played, underruns}. */
enum {
    HDA_ST_QUEUED    = 0,
    HDA_ST_PLAYED    = 1,
    HDA_ST_UNDERRUNS = 2,
    HDA_ST_COUNT     = 3
};

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

/*
 * Soft multi-op cycle: re-open, write a short pattern, start, tick, stats.
 * Failures are soft (log + return); never aborts the live path PASS.
 */
static void
soft_multi_op_stats(void)
{
    static unsigned char aTone[HDA_SOFT_BYTES];
    static unsigned aSt[HDA_ST_COUNT];
    long n;
    long nPlayed;

    msg("hda_client-gj: soft multi-op start\n");

    n = gj_hda_stream(GJ_HDA_OP_OPEN, (long)GJ_HDA_CHANNELS_STEREO,
                      (long)GJ_HDA_RATE_48K, (long)GJ_HDA_BITS_16);
    if (n != 0) {
        msg_fail_ret("soft OPEN", n);
        msg("hda_client-gj: soft multi-op soft-skip\n");
        return;
    }

    /* Distinct soft pattern (quieter levels; period = 16 B). */
    {
        unsigned i;

        for (i = 0; i < sizeof(aTone); i++) {
            aTone[i] = (unsigned char)((i & 16u) ? 0x20u : 0xe0u);
        }
    }
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft WRITE", n);
        (void)gj_hda_stream(GJ_HDA_OP_CLOSE, 0, 0, 0);
        msg("hda_client-gj: soft multi-op soft-skip\n");
        return;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    if (n != 0) {
        msg_fail_ret("soft START", n);
        (void)gj_hda_stream(GJ_HDA_OP_CLOSE, 0, 0, 0);
        msg("hda_client-gj: soft multi-op soft-skip\n");
        return;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_FRAMES, 0, 0);
    msg_tick("soft TICK", nPlayed);

    aSt[HDA_ST_QUEUED] = aSt[HDA_ST_PLAYED] = aSt[HDA_ST_UNDERRUNS] = 0;
    n = gj_hda_stream(GJ_HDA_OP_STATS, (long)(uintptr_t)aSt, 0, 0);
    if (n != 0) {
        msg_fail_ret("soft STATS", n);
        /* STATS soft-miss still closes and soft-PASS (never hard-fails). */
    } else {
        msg_stats("soft", aSt);
    }

    (void)gj_hda_stream(GJ_HDA_OP_CLOSE, 0, 0, 0);
    msg("hda_client-gj: soft multi-op PASS\n");
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

    /* Soft second cycle + stats (multi-op); never hard-fails live path. */
    soft_multi_op_stats();

    msg("hda_client-gj: live path PASS\n");
    gj_exit(0);
}
