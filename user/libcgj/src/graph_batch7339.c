/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7339: flock ops pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_flock_ops_pack_u_7339(uint32_t want_sh,
 *                                     uint32_t want_ex,
 *                                     uint32_t want_nb,
 *                                     uint32_t want_un);
 *     - Pack soft LOCK_* wants into an op mask (1|2|4|8).
 *   uint32_t __gj_flock_ops_pack_u_7339  (alias)
 *   __libcgj_batch7339_marker = "libcgj-batch7339"
 *
 * Exclusive continuum CREATE-ONLY (7331-7340: flock op stubs —
 * lock_sh_id, lock_ex_id, lock_nb_id, lock_un_id, has_sh, has_ex,
 * has_nb, ops_ok, ops_pack, batch_id_7340).
 * Unique gj_flock_ops_pack_u_7339 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7339_marker[] = "libcgj-batch7339";

/* Soft pack bits matching Linux flock(2) op layout. */
#define B7339_LOCK_SH ((uint32_t)1u)
#define B7339_LOCK_EX ((uint32_t)2u)
#define B7339_LOCK_NB ((uint32_t)4u)
#define B7339_LOCK_UN ((uint32_t)8u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7339_flock_ops_pack(uint32_t u32WantSh,
                     uint32_t u32WantEx,
                     uint32_t u32WantNb,
                     uint32_t u32WantUn)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantSh != 0u)
		u32Pack |= B7339_LOCK_SH;
	if (u32WantEx != 0u)
		u32Pack |= B7339_LOCK_EX;
	if (u32WantNb != 0u)
		u32Pack |= B7339_LOCK_NB;
	if (u32WantUn != 0u)
		u32Pack |= B7339_LOCK_UN;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flock_ops_pack_u_7339 - soft-pack flock operation wants.
 *
 * want_sh: non-zero to set LOCK_SH (1)
 * want_ex: non-zero to set LOCK_EX (2)
 * want_nb: non-zero to set LOCK_NB (4)
 * want_un: non-zero to set LOCK_UN (8)
 *
 * Returns bitmask of requested LOCK_* bits. Pure integer pack; does not
 * call flock. No parent wires.
 */
uint32_t
gj_flock_ops_pack_u_7339(uint32_t u32WantSh,
                         uint32_t u32WantEx,
                         uint32_t u32WantNb,
                         uint32_t u32WantUn)
{
	(void)NULL;
	return b7339_flock_ops_pack(u32WantSh, u32WantEx, u32WantNb, u32WantUn);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_flock_ops_pack_u_7339(uint32_t u32WantSh,
                                    uint32_t u32WantEx,
                                    uint32_t u32WantNb,
                                    uint32_t u32WantUn)
    __attribute__((alias("gj_flock_ops_pack_u_7339")));
