/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7158: sem_open oflag pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sem_flags_pack_u_7158(uint32_t want_creat,
 *                                     uint32_t want_excl);
 *     - Pack soft O_CREAT/O_EXCL wants into an oflag mask (0x40|0x80).
 *   uint32_t __gj_sem_flags_pack_u_7158  (alias)
 *   __libcgj_batch7158_marker = "libcgj-batch7158"
 *
 * Exclusive continuum CREATE-ONLY (7151-7160: sem_open flag stubs —
 * creat_id, excl_id, known_mask_id, has_creat, has_excl, needs_mode,
 * flags_ok, flags_pack, flags_errorish, batch_id_7160).
 * Unique gj_sem_flags_pack_u_7158 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7158_marker[] = "libcgj-batch7158";

/* Soft pack bits matching open-style O_CREAT / O_EXCL layout. */
#define B7158_O_CREAT ((uint32_t)0x40u)
#define B7158_O_EXCL  ((uint32_t)0x80u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7158_sem_flags_pack(uint32_t u32WantCreat, uint32_t u32WantExcl)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantCreat != 0u)
		u32Pack |= B7158_O_CREAT;
	if (u32WantExcl != 0u)
		u32Pack |= B7158_O_EXCL;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sem_flags_pack_u_7158 - soft-pack sem_open oflag wants.
 *
 * want_creat: non-zero to set O_CREAT (0x40)
 * want_excl:  non-zero to set O_EXCL (0x80)
 *
 * Returns bitmask of requested bits. Pure integer pack; does not call
 * sem_open. Does not force O_CREAT when only O_EXCL is requested (use
 * flags_ok / flags_errorish for that policy). No parent wires.
 */
uint32_t
gj_sem_flags_pack_u_7158(uint32_t u32WantCreat, uint32_t u32WantExcl)
{
	(void)NULL;
	return b7158_sem_flags_pack(u32WantCreat, u32WantExcl);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sem_flags_pack_u_7158(uint32_t u32WantCreat,
                                    uint32_t u32WantExcl)
    __attribute__((alias("gj_sem_flags_pack_u_7158")));
