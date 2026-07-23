/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7338: flock op validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flock_ops_ok_u_7338(uint32_t op);
 *     - Return 1 if op uses only known flock bits
 *       (LOCK_SH|LOCK_EX|LOCK_NB|LOCK_UN = 0xF), else 0.
 *   uint32_t __gj_flock_ops_ok_u_7338  (alias)
 *   __libcgj_batch7338_marker = "libcgj-batch7338"
 *
 * Exclusive continuum CREATE-ONLY (7331-7340: flock op stubs —
 * lock_sh_id, lock_ex_id, lock_nb_id, lock_un_id, has_sh, has_ex,
 * has_nb, ops_ok, ops_pack, batch_id_7340).
 * Unique gj_flock_ops_ok_u_7338 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7338_marker[] = "libcgj-batch7338";

/*
 * Known flock(2) operation mask:
 *   LOCK_SH (1) | LOCK_EX (2) | LOCK_NB (4) | LOCK_UN (8) = 0xF
 */
#define B7338_FLOCK_KNOWN_MASK ((uint32_t)0xFu)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7338_flock_ops_ok(uint32_t u32Op)
{
	return ((u32Op & ~B7338_FLOCK_KNOWN_MASK) == 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flock_ops_ok_u_7338 - 1 if op has no unknown bits.
 *
 * op: flock(2) operation argument
 *
 * Returns 1 when only bits in {1, 2, 4, 8} are set (including 0),
 * else 0. Soft catalog check; does not call flock. No parent wires.
 */
uint32_t
gj_flock_ops_ok_u_7338(uint32_t u32Op)
{
	(void)NULL;
	return b7338_flock_ops_ok(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flock_ops_ok_u_7338(uint32_t u32Op)
    __attribute__((alias("gj_flock_ops_ok_u_7338")));
