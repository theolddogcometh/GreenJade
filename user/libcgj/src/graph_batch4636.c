/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4636: bytes to gibibytes (u-suffix, floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_to_gib_u(uint64_t b);
 *     - Return floor(b / 1073741824) (bytes → binary GiB).
 *   uint64_t __gj_bytes_to_gib_u  (alias)
 *   __libcgj_batch4636_marker = "libcgj-batch4636"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, kb_to_bytes_u SI, mb_to_bytes_u SI, bytes_round_page_u,
 * batch_id_4640). Inverse of gj_gib_to_bytes_u (batch4633); does not
 * redefine it. Unique gj_bytes_to_gib_u surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4636_marker[] = "libcgj-batch4636";

/* Bytes per gibibyte (2^30). */
#define B4636_BYTES_PER_GIB  1073741824ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide bytes by GiB size. */
static uint64_t
b4636_div_gib(uint64_t u64B)
{
	return u64B / B4636_BYTES_PER_GIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_gib_u - convert bytes to gibibytes (floor).
 *
 * b: size in bytes.
 *
 * Returns floor(b / 1073741824). Examples: 0 → 0, 1073741823 → 0,
 * 1073741824 → 1. No parent wires.
 */
uint64_t
gj_bytes_to_gib_u(uint64_t u64B)
{
	(void)NULL;
	return b4636_div_gib(u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_to_gib_u(uint64_t u64B)
    __attribute__((alias("gj_bytes_to_gib_u")));
