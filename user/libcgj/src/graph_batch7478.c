/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7478: mkdirat mode pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mkdirat_mode_pack_u_7478(uint32_t want_u,
 *                                        uint32_t want_g,
 *                                        uint32_t want_o);
 *     - Pack soft owner/group/other rwx wants into a mode mask
 *       (S_IRWXU|S_IRWXG|S_IRWXO).
 *   uint32_t __gj_mkdirat_mode_pack_u_7478  (alias)
 *   __libcgj_batch7478_marker = "libcgj-batch7478"
 *
 * Exclusive continuum CREATE-ONLY (7471-7480: mkdirat mode stubs —
 * def_mode_id, priv_mode_id, world_mode_id, allperms_id,
 * has_irwxu, has_irwxo, mode_ok, mode_pack, mode_errorish,
 * batch_id_7480).
 * Unique gj_mkdirat_mode_pack_u_7478 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7478_marker[] = "libcgj-batch7478";

/* Soft pack bits matching POSIX S_IRWX{U,G,O}. */
#define B7478_S_IRWXU ((uint32_t)0700u)
#define B7478_S_IRWXG ((uint32_t)0070u)
#define B7478_S_IRWXO ((uint32_t)0007u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7478_mkdirat_mode_pack(uint32_t u32WantU,
                        uint32_t u32WantG,
                        uint32_t u32WantO)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantU != 0u)
		u32Pack |= B7478_S_IRWXU;
	if (u32WantG != 0u)
		u32Pack |= B7478_S_IRWXG;
	if (u32WantO != 0u)
		u32Pack |= B7478_S_IRWXO;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mkdirat_mode_pack_u_7478 - soft-pack mkdirat permission wants.
 *
 * want_u: non-zero to set S_IRWXU (0700)
 * want_g: non-zero to set S_IRWXG (0070)
 * want_o: non-zero to set S_IRWXO (0007)
 *
 * Returns bitmask of requested permission class bits. Pure integer
 * pack; does not call mkdirat. No parent wires.
 */
uint32_t
gj_mkdirat_mode_pack_u_7478(uint32_t u32WantU,
                            uint32_t u32WantG,
                            uint32_t u32WantO)
{
	(void)NULL;
	return b7478_mkdirat_mode_pack(u32WantU, u32WantG, u32WantO);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mkdirat_mode_pack_u_7478(uint32_t u32WantU,
                                       uint32_t u32WantG,
                                       uint32_t u32WantO)
    __attribute__((alias("gj_mkdirat_mode_pack_u_7478")));
