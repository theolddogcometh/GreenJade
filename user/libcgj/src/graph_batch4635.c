/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4635: bytes to mebibytes (u-suffix, floor).
 *
 * Surface (unique symbols):
 *   uint64_t gj_bytes_to_mib_u(uint64_t b);
 *     - Return floor(b / 1048576) (bytes → binary MiB).
 *   uint64_t __gj_bytes_to_mib_u  (alias)
 *   __libcgj_batch4635_marker = "libcgj-batch4635"
 *
 * Exclusive continuum CREATE-ONLY (4631-4640: kib_to_bytes_u,
 * mib_to_bytes_u, gib_to_bytes_u, bytes_to_kib_u, bytes_to_mib_u,
 * bytes_to_gib_u, kb_to_bytes_u SI, mb_to_bytes_u SI, bytes_round_page_u,
 * batch_id_4640). Distinct from gj_bytes_to_mib_floor (batch1585) —
 * unique gj_bytes_to_mib_u surface only; no multi-def. Inverse of
 * gj_mib_to_bytes_u (batch4632); does not redefine it. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4635_marker[] = "libcgj-batch4635";

/* Bytes per mebibyte (2^20). */
#define B4635_BYTES_PER_MIB  1048576ull

/* ---- freestanding helpers ---------------------------------------------- */

/* Floor-divide bytes by MiB size. */
static uint64_t
b4635_div_mib(uint64_t u64B)
{
	return u64B / B4635_BYTES_PER_MIB;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bytes_to_mib_u - convert bytes to mebibytes (floor).
 *
 * b: size in bytes.
 *
 * Returns floor(b / 1048576). Examples: 0 → 0, 1048575 → 0, 1048576 → 1.
 * No parent wires.
 */
uint64_t
gj_bytes_to_mib_u(uint64_t u64B)
{
	(void)NULL;
	return b4635_div_mib(u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_bytes_to_mib_u(uint64_t u64B)
    __attribute__((alias("gj_bytes_to_mib_u")));
