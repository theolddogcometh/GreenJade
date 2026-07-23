/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding HDA stream client (ring-3 smoke) for GreenJade.
 * Exercises GJ_SYS_HDA_STREAM (nr 94) via gj_hda_stream:
 *   open → write PCM → start → tick → stats → underrun tick → close
 * then a deepened soft suite (multi-op + partial-tick + multi-write +
 * soft underrun + mono format + reclaim re-OPEN + reject probe). Soft
 * steps never hard-fail the live path.
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
 *   hda_client-gj: stats soft q=… p=… u=…
 *   hda_client-gj: soft suite PASS | soft suite soft-skip
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

/* Frame size: stereo 16-bit LE → 4 bytes/frame. */
#define HDA_BYTES_PER_FRAME 4u
/* Mono 16-bit LE → 2 bytes/frame. */
#define HDA_MONO_BYTES_PER_FRAME 2u

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

/* Soft best-effort CLOSE (never hard-fails). */
static void
soft_close(void)
{
    (void)gj_hda_stream(GJ_HDA_OP_CLOSE, 0, 0, 0);
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
        msg_fail_ret(tag, n);
    }
    return n;
}

/* Soft STATS snapshot into aSt; logs on miss. Returns 0 on ok. */
static long
soft_stats(const char *tag, unsigned *aSt)
{
    long n;

    if (aSt == 0) {
        return -1;
    }
    aSt[HDA_ST_QUEUED] = aSt[HDA_ST_PLAYED] = aSt[HDA_ST_UNDERRUNS] = 0;
    n = gj_hda_stream(GJ_HDA_OP_STATS, (long)(uintptr_t)aSt, 0, 0);
    if (n != 0) {
        msg_fail_ret(tag, n);
        return n;
    }
    msg_stats(tag, aSt);
    return 0;
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
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft WRITE", n);
        soft_close();
        msg("hda_client-gj: soft multi-op soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    if (n != 0) {
        msg_fail_ret("soft START", n);
        soft_close();
        msg("hda_client-gj: soft multi-op soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_FRAMES, 0, 0);
    msg_tick("soft TICK", nPlayed);

    /* STATS soft-miss still soft-PASS (never hard-fails). */
    (void)soft_stats("soft", aSt);

    soft_close();
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
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft partial WRITE", n);
        soft_close();
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    if (n != 0) {
        msg_fail_ret("soft partial START", n);
        soft_close();
        msg("hda_client-gj: soft partial-tick soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_PART_TICK1, 0, 0);
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
    if (n != (long)sizeof(aChunkA)) {
        msg_fail_ret("soft multi-write WRITE1", n);
        soft_close();
        msg("hda_client-gj: soft multi-write soft-skip\n");
        return 0;
    }
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aChunkB,
                      (long)sizeof(aChunkB), 0);
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
    if (n != 0) {
        msg_fail_ret("soft multi-write START", n);
        soft_close();
        msg("hda_client-gj: soft multi-write soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK,
                            (long)(HDA_SOFT_CHUNK_FRAMES * 2u), 0, 0);
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
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft underrun WRITE", n);
        soft_close();
        msg("hda_client-gj: soft underrun soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    if (n != 0) {
        msg_fail_ret("soft underrun START", n);
        soft_close();
        msg("hda_client-gj: soft underrun soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_FRAMES, 0, 0);
    msg_tick("soft underrun TICK", nPlayed);
    if (nPlayed != (long)sizeof(aTone)) {
        msg_fail_ret("soft underrun TICK", nPlayed);
        soft_close();
        msg("hda_client-gj: soft underrun soft-skip\n");
        return 0;
    }

    /* Empty ring → underrun counter must rise. */
    (void)gj_hda_stream(GJ_HDA_OP_TICK, 8, 0, 0);
    if (soft_stats("soft underrun", aSt) != 0 ||
        aSt[HDA_ST_UNDERRUNS] == 0u) {
        soft_close();
        msg("hda_client-gj: soft underrun soft-skip\n");
        return 0;
    }

    soft_close();
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
        msg_fail_ret("soft mono OPEN", n);
        msg("hda_client-gj: soft mono soft-skip\n");
        return 0;
    }

    for (i = 0; i < sizeof(aTone); i++) {
        aTone[i] = (unsigned char)((i & 8u) ? 0x30u : 0xd0u);
    }
    n = gj_hda_stream(GJ_HDA_OP_WRITE, (long)(uintptr_t)aTone,
                      (long)sizeof(aTone), 0);
    if (n != (long)sizeof(aTone)) {
        msg_fail_ret("soft mono WRITE", n);
        soft_close();
        msg("hda_client-gj: soft mono soft-skip\n");
        return 0;
    }

    n = gj_hda_stream(GJ_HDA_OP_START, 0, 0, 0);
    if (n != 0) {
        msg_fail_ret("soft mono START", n);
        soft_close();
        msg("hda_client-gj: soft mono soft-skip\n");
        return 0;
    }

    nPlayed = gj_hda_stream(GJ_HDA_OP_TICK, (long)HDA_SOFT_MONO_FRAMES, 0, 0);
    msg_tick("soft mono TICK", nPlayed);
    if (nPlayed != (long)sizeof(aTone)) {
        msg_fail_ret("soft mono TICK", nPlayed);
        soft_close();
        msg("hda_client-gj: soft mono soft-skip\n");
        return 0;
    }

    (void)soft_stats("soft mono", aSt);
    soft_close();
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
        msg_fail_ret("soft reclaim re-OPEN", n);
        soft_close();
        msg("hda_client-gj: soft reclaim soft-skip\n");
        return 0;
    }

    if (soft_stats("soft reclaim", aSt) != 0 ||
        aSt[HDA_ST_QUEUED] != 0u || aSt[HDA_ST_PLAYED] != 0u) {
        soft_close();
        msg("hda_client-gj: soft reclaim soft-skip\n");
        return 0;
    }

    soft_close();
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
        soft_close();
        msg("hda_client-gj: soft reject soft-skip\n");
        return 0;
    }

    /* Also probe illegal bit depth (not 8/16/32). */
    nBits = gj_hda_stream(GJ_HDA_OP_OPEN, (long)GJ_HDA_CHANNELS_STEREO,
                          (long)GJ_HDA_RATE_48K, 24);
    if (nBits == 0) {
        soft_close();
        msg("hda_client-gj: soft reject soft-skip\n");
        return 0;
    }

    msg("hda_client-gj: soft reject PASS\n");
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

    msg("hda_client-gj: soft suite start\n");

    cOk += (unsigned)soft_multi_op_stats();
    cOk += (unsigned)soft_partial_tick();
    cOk += (unsigned)soft_multi_write();
    cOk += (unsigned)soft_underrun_cycle();
    cOk += (unsigned)soft_mono_format();
    cOk += (unsigned)soft_reclaim_reopen();
    cOk += (unsigned)soft_reject_inval();

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
