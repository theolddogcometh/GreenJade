/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch173: xorshift128+ PRNG.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party PRNG source was copied.
 *
 * Existing surface NOT redefined (avoid multi-def):
 *   rand / srand / random / srandom          → stdlib / rand_div.c
 *   drand48 / lrand48 / srand48 / seed48     → rand48.c
 *   arc4random* / __arc4random*              → graph_batch106.c
 *   cookie xorshift64* (kernel only)         → kernel/mm/fault.c
 *
 * This TU adds only unique symbols:
 *   void     gj_xorshift128_seed(uint64_t s0, uint64_t s1);
 *   uint64_t gj_xorshift128_next(void);
 *   void     gj_xorshift128_fill(void *buf, size_t n);
 *   __gj_xorshift128_seed / __gj_xorshift128_next / __gj_xorshift128_fill
 *   __libcgj_batch173_marker = "libcgj-batch173"
 *
 * Algorithm (xorshift128+):
 *   state = (s0, s1); output = s0 + s1; then the classic (23, 18, 5)
 *   shift triple on the two 64-bit words. All-zero state is a fixed
 *   point — seed replaces (0,0) with a non-zero default pair.
 *
 * Notes:
 *   - Not cryptographically secure; use for non-crypto randomness only.
 *   - Process-global state (not thread-local); no locking.
 *   - fill writes host-endian 64-bit chunks then a trailing partial word.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch173_marker[] = "libcgj-batch173";

/* Default non-zero seed pair (SplitMix-style constants; not secret). */
#define B173_DEF_S0  0x9E3779B97F4A7C15ULL
#define B173_DEF_S1  0xBF58476D1CE4E5B9ULL

/* xorshift128+ state: must not be (0, 0). */
static uint64_t g_b173_s0 = B173_DEF_S0;
static uint64_t g_b173_s1 = B173_DEF_S1;

/* ---- gj_xorshift128_seed ----------------------------------------------- */

/*
 * Set PRNG state to (s0, s1). If both words are zero, install the
 * non-zero default pair so the generator does not stick at zero.
 */
void
gj_xorshift128_seed(uint64_t s0, uint64_t s1)
{
	if (s0 == 0ULL && s1 == 0ULL) {
		g_b173_s0 = B173_DEF_S0;
		g_b173_s1 = B173_DEF_S1;
		return;
	}
	g_b173_s0 = s0;
	g_b173_s1 = s1;
}

void __gj_xorshift128_seed(uint64_t s0, uint64_t s1)
    __attribute__((alias("gj_xorshift128_seed")));

/* ---- gj_xorshift128_next ----------------------------------------------- */

/*
 * Advance state and return one 64-bit sample (xorshift128+).
 * Shifts: a=23, b=18, c=5 on the two-word state.
 */
uint64_t
gj_xorshift128_next(void)
{
	uint64_t s1 = g_b173_s0;
	uint64_t s0 = g_b173_s1;
	uint64_t result = s0 + s1;

	g_b173_s0 = s0;
	s1 ^= s1 << 23; /* a */
	g_b173_s1 = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5); /* b, c */
	return result;
}

uint64_t __gj_xorshift128_next(void)
    __attribute__((alias("gj_xorshift128_next")));

/* ---- gj_xorshift128_fill ----------------------------------------------- */

/*
 * Fill buf[0..n) with PRNG bytes. Full uint64_t words first (host endian),
 * then any remaining 1..7 bytes from one extra sample. NULL buf or n==0
 * is a no-op.
 */
void
gj_xorshift128_fill(void *buf, size_t n)
{
	unsigned char *p;
	size_t i;

	if (buf == NULL || n == 0u) {
		return;
	}
	p = (unsigned char *)buf;

	while (n >= 8u) {
		uint64_t v = gj_xorshift128_next();
		p[0] = (unsigned char)(v);
		p[1] = (unsigned char)(v >> 8);
		p[2] = (unsigned char)(v >> 16);
		p[3] = (unsigned char)(v >> 24);
		p[4] = (unsigned char)(v >> 32);
		p[5] = (unsigned char)(v >> 40);
		p[6] = (unsigned char)(v >> 48);
		p[7] = (unsigned char)(v >> 56);
		p += 8u;
		n -= 8u;
	}

	if (n > 0u) {
		uint64_t v = gj_xorshift128_next();
		for (i = 0u; i < n; i++) {
			p[i] = (unsigned char)(v >> (i * 8u));
		}
	}
}

void __gj_xorshift128_fill(void *buf, size_t n)
    __attribute__((alias("gj_xorshift128_fill")));
