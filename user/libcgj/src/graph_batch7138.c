/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7138: mq_open flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mq_flags_pack_u_7138(uint32_t want_creat,
 *                                    uint32_t want_excl,
 *                                    uint32_t want_nonblock);
 *     - Pack soft O_* wants into a flags mask (0x40|0x80|0x800).
 *   uint32_t __gj_mq_flags_pack_u_7138  (alias)
 *   __libcgj_batch7138_marker = "libcgj-batch7138"
 *
 * Exclusive continuum CREATE-ONLY (7131-7140: mq_open flag stubs —
 * creat_id, excl_id, nonblock_id, has_creat, has_excl, has_nonblock,
 * flags_ok, flags_pack, flags_errorish, batch_id_7140).
 * Unique gj_mq_flags_pack_u_7138 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7138_marker[] = "libcgj-batch7138";

/* Soft pack bits matching Linux mq_open oflag layout. */
#define B7138_O_CREAT    ((uint32_t)0x40u)
#define B7138_O_EXCL     ((uint32_t)0x80u)
#define B7138_O_NONBLOCK ((uint32_t)0x800u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7138_mq_flags_pack(uint32_t u32WantCreat,
                    uint32_t u32WantExcl,
                    uint32_t u32WantNonblock)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantCreat != 0u)
		u32Pack |= B7138_O_CREAT;
	if (u32WantExcl != 0u)
		u32Pack |= B7138_O_EXCL;
	if (u32WantNonblock != 0u)
		u32Pack |= B7138_O_NONBLOCK;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mq_flags_pack_u_7138 - soft-pack mq_open oflag wants.
 *
 * want_creat:    non-zero to set O_CREAT (0x40)
 * want_excl:     non-zero to set O_EXCL (0x80)
 * want_nonblock: non-zero to set O_NONBLOCK (0x800)
 *
 * Returns bitmask of requested O_* bits. Pure integer pack; does not
 * call mq_open. No parent wires.
 */
uint32_t
gj_mq_flags_pack_u_7138(uint32_t u32WantCreat,
                        uint32_t u32WantExcl,
                        uint32_t u32WantNonblock)
{
	(void)NULL;
	return b7138_mq_flags_pack(u32WantCreat, u32WantExcl, u32WantNonblock);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mq_flags_pack_u_7138(uint32_t u32WantCreat,
                                   uint32_t u32WantExcl,
                                   uint32_t u32WantNonblock)
    __attribute__((alias("gj_mq_flags_pack_u_7138")));
