/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7335: flock LOCK_SH presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flock_has_sh_u_7335(uint32_t op);
 *     - Return 1 if op has LOCK_SH (1) set, else 0.
 *   uint32_t __gj_flock_has_sh_u_7335  (alias)
 *   __libcgj_batch7335_marker = "libcgj-batch7335"
 *
 * Exclusive continuum CREATE-ONLY (7331-7340: flock op stubs —
 * lock_sh_id, lock_ex_id, lock_nb_id, lock_un_id, has_sh, has_ex,
 * has_nb, ops_ok, ops_pack, batch_id_7340).
 * Unique gj_flock_has_sh_u_7335 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7335_marker[] = "libcgj-batch7335";

/* LOCK_SH bit (1). */
#define B7335_LOCK_SH ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7335_flock_has_sh(uint32_t u32Op)
{
	return ((u32Op & B7335_LOCK_SH) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flock_has_sh_u_7335 - 1 if op includes LOCK_SH.
 *
 * op: flock(2) operation argument
 *
 * Returns 1 when bit 1 is set, else 0. Soft bit test only; does not
 * call flock. No parent wires.
 */
uint32_t
gj_flock_has_sh_u_7335(uint32_t u32Op)
{
	(void)NULL;
	return b7335_flock_has_sh(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flock_has_sh_u_7335(uint32_t u32Op)
    __attribute__((alias("gj_flock_has_sh_u_7335")));
