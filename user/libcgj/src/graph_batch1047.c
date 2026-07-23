/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1047: Fisher–Yates shuffle of uint32_t[] with
 * SplitMix64 RNG state.
 *
 * Surface (unique symbols):
 *   void gj_u32_shuffle_fisher(uint32_t *a, size_t n, uint64_t *rng_state);
 *     — Modern Fisher–Yates (Knuth) in-place shuffle of a[0..n). PRNG is
 *       a file-local SplitMix64 step (not gj_splitmix64 / gj_splitmix64_next
 *       — those remain elsewhere; avoid multi-def). *rng_state is advanced.
 *       No-op if a or rng_state is NULL or n < 2.
 *   void gj_u32_shuffle_yates(uint32_t *a, size_t n, uint64_t *rng_state);
 *     — Unique alternate name for the same Fisher–Yates entry (alias).
 *   void __gj_u32_shuffle_fisher  (alias)
 *   void __gj_u32_shuffle_yates   (alias)
 *   __libcgj_batch1047_marker = "libcgj-batch1047"
 *
 * Distinct from gj_shuffle_u32 (batch212 LCG). Clean-room freestanding
 * pure C (integer/pointer only). Compiles with -ffreestanding -msse2
 * -Wall -Wextra -Werror. No malloc, no errno, no libc. No third-party
 * source copied. SplitMix64 constants match the public Steele/Vigna
 * algorithm (file-local step only).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1047_marker[] = "libcgj-batch1047";

/* SplitMix64 public algorithm constants (file-local step). */
#define B1047_GAMMA  0x9e3779b97f4a7c15ULL
#define B1047_M1     0xbf58476d1ce4e5b9ULL
#define B1047_M2     0x94d049bb133111ebULL

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * One SplitMix64 step; advances *pState and returns mixed 64-bit output.
 * File-static — does not export gj_splitmix64 / gj_splitmix64_next.
 */
static uint64_t
b1047_splitmix_step(uint64_t *pState)
{
	uint64_t u64Z;

	u64Z = (*pState += B1047_GAMMA);
	u64Z = (u64Z ^ (u64Z >> 30)) * B1047_M1;
	u64Z = (u64Z ^ (u64Z >> 27)) * B1047_M2;
	return u64Z ^ (u64Z >> 31);
}

/*
 * Uniform index in [0, bound). Uses high 32 bits of SplitMix output
 * scaled by Lemire-style multiply-high (bound as 32-bit range).
 */
static size_t
b1047_rand_index(uint64_t *pState, size_t bound)
{
	uint32_t uR;
	uint64_t u64Prod;

	if (bound <= 1u) {
		return 0u;
	}
	uR = (uint32_t)(b1047_splitmix_step(pState) >> 32);
	u64Prod = (uint64_t)uR * (uint64_t)bound;
	return (size_t)(u64Prod >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_shuffle_fisher — Fisher–Yates shuffle of a[0..n).
 *
 * For i = n-1 .. 1:
 *   j = uniform in [0, i]
 *   swap a[i], a[j]
 */
void
gj_u32_shuffle_fisher(uint32_t *a, size_t n, uint64_t *rng_state)
{
	size_t i;

	if (a == NULL || rng_state == NULL || n < 2u) {
		return;
	}

	for (i = n - 1u; i > 0u; i--) {
		size_t j;
		uint32_t uT;

		j = b1047_rand_index(rng_state, i + 1u);
		uT = a[i];
		a[i] = a[j];
		a[j] = uT;
	}
}

/* Unique alternate entry name (same body). */
void
gj_u32_shuffle_yates(uint32_t *a, size_t n, uint64_t *rng_state)
    __attribute__((alias("gj_u32_shuffle_fisher")));

/* ---- underscored aliases ----------------------------------------------- */

void __gj_u32_shuffle_fisher(uint32_t *a, size_t n, uint64_t *rng_state)
    __attribute__((alias("gj_u32_shuffle_fisher")));

void __gj_u32_shuffle_yates(uint32_t *a, size_t n, uint64_t *rng_state)
    __attribute__((alias("gj_u32_shuffle_fisher")));
