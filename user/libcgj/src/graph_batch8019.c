/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8019: malloc alignment clamp / next-pow2 stubs.
 *
 * Surface (unique symbols):
 *   size_t   gj_malloc_align_next_pow2_8019(size_t align);
 *     - Smallest power of two >= align (align==0 → 1). Cap at soft max
 *       0x80000000u; values above max → 0.
 *   size_t   gj_malloc_align_clamp_8019(size_t align);
 *     - Normalize align into [8, 1<<20]: 0 → 16 (default); else next
 *       power of two clamped into range.
 *   uint32_t gj_malloc_align_max_8019(void);
 *     - Soft maximum alignment accepted by clamp (1 << 20 = 1048576).
 *   __gj_malloc_align_*_8019  (aliases)
 *   __libcgj_batch8019_marker = "libcgj-batch8019"
 *
 * Exclusive continuum CREATE-ONLY (8011-8020: malloc alignment stubs).
 * Unique gj_malloc_align_*_8019 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8019_marker[] = "libcgj-batch8019";

#define B8019_DEFAULT  16u
#define B8019_MIN      8u
#define B8019_MAX      (1u << 20) /* 1048576 */
#define B8019_POW2_CAP 0x80000000u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b8019_next_pow2(size_t uAlign)
{
	size_t uV;

	if (uAlign == 0u) {
		return 1u;
	}
	if (uAlign > (size_t)B8019_POW2_CAP) {
		return 0u;
	}
	/* Already power of two. */
	if ((uAlign & (uAlign - 1u)) == 0u) {
		return uAlign;
	}
	/* Soft bit-smear next-pow2 for 32-bit-ish size_t values. */
	uV = uAlign - 1u;
	uV |= uV >> 1;
	uV |= uV >> 2;
	uV |= uV >> 4;
	uV |= uV >> 8;
	uV |= uV >> 16;
#if UINTPTR_MAX > 0xffffffffu
	uV |= uV >> 32;
#endif
	uV = uV + 1u;
	if (uV == 0u) {
		return 0u;
	}
	return uV;
}

static size_t
b8019_clamp(size_t uAlign)
{
	size_t uN;

	if (uAlign == 0u) {
		return (size_t)B8019_DEFAULT;
	}
	uN = b8019_next_pow2(uAlign);
	if (uN == 0u) {
		return (size_t)B8019_MAX;
	}
	if (uN < (size_t)B8019_MIN) {
		return (size_t)B8019_MIN;
	}
	if (uN > (size_t)B8019_MAX) {
		return (size_t)B8019_MAX;
	}
	return uN;
}

static uint32_t
b8019_max(void)
{
	return B8019_MAX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_malloc_align_next_pow2_8019 - soft next power-of-two alignment.
 *
 * align: candidate (0 → 1)
 *
 * Returns next power of two, or 0 if above soft cap / overflow.
 * Soft pure-data math; does not allocate. No parent wires.
 */
size_t
gj_malloc_align_next_pow2_8019(size_t align)
{
	(void)NULL;
	return b8019_next_pow2(align);
}

/*
 * gj_malloc_align_clamp_8019 - soft normalize alignment into [8, 1MiB].
 *
 * Zero → default 16; otherwise next pow2 clamped into range.
 */
size_t
gj_malloc_align_clamp_8019(size_t align)
{
	return b8019_clamp(align);
}

/*
 * gj_malloc_align_max_8019 - soft maximum clamp alignment.
 *
 * Always returns 1048576 (1 << 20).
 */
uint32_t
gj_malloc_align_max_8019(void)
{
	return b8019_max();
}

/* ---- underscored aliases ----------------------------------------------- */

size_t __gj_malloc_align_next_pow2_8019(size_t align)
    __attribute__((alias("gj_malloc_align_next_pow2_8019")));

size_t __gj_malloc_align_clamp_8019(size_t align)
    __attribute__((alias("gj_malloc_align_clamp_8019")));

uint32_t __gj_malloc_align_max_8019(void)
    __attribute__((alias("gj_malloc_align_max_8019")));
