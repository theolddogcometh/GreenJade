/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7148: shm_open flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_shm_flags_pack_u_7148(uint32_t want_rdwr,
 *                                     uint32_t want_creat,
 *                                     uint32_t want_excl);
 *     - Pack soft O_* wants into a flags mask (0x2|0x40|0x80).
 *   uint32_t __gj_shm_flags_pack_u_7148  (alias)
 *   __libcgj_batch7148_marker = "libcgj-batch7148"
 *
 * Exclusive continuum CREATE-ONLY (7141-7150: shm_open flag stubs —
 * o_rdwr_id, o_creat_id, o_excl_id, has_rdwr, has_creat, has_excl,
 * flags_ok, flags_pack, flags_errorish, batch_id_7150).
 * Unique gj_shm_flags_pack_u_7148 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7148_marker[] = "libcgj-batch7148";

/* Soft pack bits matching Linux/POSIX O_* layout for shm_open. */
#define B7148_O_RDWR  ((uint32_t)0x2u)
#define B7148_O_CREAT ((uint32_t)0x40u)
#define B7148_O_EXCL  ((uint32_t)0x80u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7148_shm_flags_pack(uint32_t u32WantRdwr,
                     uint32_t u32WantCreat,
                     uint32_t u32WantExcl)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantRdwr != 0u)
		u32Pack |= B7148_O_RDWR;
	if (u32WantCreat != 0u)
		u32Pack |= B7148_O_CREAT;
	if (u32WantExcl != 0u)
		u32Pack |= B7148_O_EXCL;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_shm_flags_pack_u_7148 - soft-pack shm_open flag wants.
 *
 * want_rdwr:  non-zero to set O_RDWR (0x2)
 * want_creat: non-zero to set O_CREAT (0x40)
 * want_excl:  non-zero to set O_EXCL (0x80)
 *
 * Returns bitmask of requested O_* bits. Pure integer pack; does not
 * call shm_open. No parent wires.
 */
uint32_t
gj_shm_flags_pack_u_7148(uint32_t u32WantRdwr,
                         uint32_t u32WantCreat,
                         uint32_t u32WantExcl)
{
	(void)NULL;
	return b7148_shm_flags_pack(u32WantRdwr, u32WantCreat, u32WantExcl);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_shm_flags_pack_u_7148(uint32_t u32WantRdwr,
                                    uint32_t u32WantCreat,
                                    uint32_t u32WantExcl)
    __attribute__((alias("gj_shm_flags_pack_u_7148")));
