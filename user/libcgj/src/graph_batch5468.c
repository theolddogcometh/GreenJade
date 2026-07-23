/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5468: Steam CDN chunk size alignment hint.
 *
 * Surface (unique symbols):
 *   int gj_steam_cdn_chunk_align_ok_5468(uint32_t size);
 *     — Return 1 if size is a power of two in [1024, 1048576]
 *       (typical CDN depot chunk sizes), else 0. Soft network hint.
 *   int __gj_steam_cdn_chunk_align_ok_5468  (alias)
 *   __libcgj_batch5468_marker = "libcgj-batch5468"
 *
 * Steam network/filesystem exclusive pure-data wave (5461–5470).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5468_marker[] = "libcgj-batch5468";

#define B5468_CHUNK_MIN  1024u
#define B5468_CHUNK_MAX  1048576u

/* ---- freestanding helpers ---------------------------------------------- */

/* Power-of-two test; 0 is not a power of two. */
static int
b5468_is_pow2(uint32_t u)
{
	if (u == 0u) {
		return 0;
	}
	return ((u & (u - 1u)) == 0u) ? 1 : 0;
}

static int
b5468_chunk_ok(uint32_t uSize)
{
	if (uSize < B5468_CHUNK_MIN) {
		return 0;
	}
	if (uSize > B5468_CHUNK_MAX) {
		return 0;
	}
	return b5468_is_pow2(uSize);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_cdn_chunk_align_ok_5468 — 1 iff size is aligned CDN chunk size.
 *
 * size: proposed depot/CDN transfer chunk length in octets.
 * Returns 1 for power-of-two sizes in [1024, 1MiB], else 0.
 */
int
gj_steam_cdn_chunk_align_ok_5468(uint32_t uSize)
{
	(void)NULL;
	return b5468_chunk_ok(uSize);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_steam_cdn_chunk_align_ok_5468(uint32_t uSize)
    __attribute__((alias("gj_steam_cdn_chunk_align_ok_5468")));
