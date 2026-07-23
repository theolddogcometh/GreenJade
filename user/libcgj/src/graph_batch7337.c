/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7337: flock LOCK_NB presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flock_has_nb_u_7337(uint32_t op);
 *     - Return 1 if op has LOCK_NB (4) set, else 0.
 *   uint32_t __gj_flock_has_nb_u_7337  (alias)
 *   __libcgj_batch7337_marker = "libcgj-batch7337"
 *
 * Exclusive continuum CREATE-ONLY (7331-7340: flock op stubs —
 * lock_sh_id, lock_ex_id, lock_nb_id, lock_un_id, has_sh, has_ex,
 * has_nb, ops_ok, ops_pack, batch_id_7340).
 * Unique gj_flock_has_nb_u_7337 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7337_marker[] = "libcgj-batch7337";

/* LOCK_NB bit (4). */
#define B7337_LOCK_NB ((uint32_t)4u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7337_flock_has_nb(uint32_t u32Op)
{
	return ((u32Op & B7337_LOCK_NB) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flock_has_nb_u_7337 - 1 if op includes LOCK_NB.
 *
 * op: flock(2) operation argument
 *
 * Returns 1 when bit 4 is set, else 0. Soft bit test only; does not
 * call flock. No parent wires.
 */
uint32_t
gj_flock_has_nb_u_7337(uint32_t u32Op)
{
	(void)NULL;
	return b7337_flock_has_nb(u32Op);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flock_has_nb_u_7337(uint32_t u32Op)
    __attribute__((alias("gj_flock_has_nb_u_7337")));
