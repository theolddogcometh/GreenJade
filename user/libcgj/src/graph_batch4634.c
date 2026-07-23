/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4634: bytes to kibibytes (u-suffix, floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_to_kib_u(uint64_t b);
 *     - Return floor(b / 1024) (bytes → binary KiB).
 *   uint64_t __gj_bytes_to_kib_u  (alias)
 *   __libcgj_batch4634_marker = "libcgj-batch4634"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, kb_to_bytes_u SI, mb_to_bytes_u SI, bytes_round_page_u,
 * batch_id_4640). Distinct from gj_bytes_to_kib_floor (batch1584) —
 * unique gj_bytes_to_kib_u surface only; no multi-def. Inverse of
 * gj_kib_to_bytes_u (batch4631); does not redefine it. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4634_marker[] = "libcgj-batch4634";

/* Bytes per kibibyte (2^10). */
#define B4634_BYTES_PER_KIB  1024ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide bytes by KiB size. */
static uint64_t
b4634_div_kib(uint64_t u64B)
{
	return u64B / B4634_BYTES_PER_KIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_kib_u - convert bytes to kibibytes (floor).
 *
 * b: size in bytes.
 *
 * Returns floor(b / 1024). Examples: 0 → 0, 1023 → 0, 1024 → 1.
 * No parent wires.
 */
uint64_t
gj_bytes_to_kib_u(uint64_t u64B)
{
	(void)NULL;
	return b4634_div_kib(u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_to_kib_u(uint64_t u64B)
    __attribute__((alias("gj_bytes_to_kib_u")));
