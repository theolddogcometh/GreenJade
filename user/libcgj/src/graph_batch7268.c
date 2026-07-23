/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7268: recvmsg flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_flags_pack_u_7268(uint32_t want_peek,
 *                                     uint32_t want_dontwait,
 *                                     uint32_t want_waitall);
 *     - Pack soft MSG_* wants into a flags mask (0x2|0x40|0x100).
 *   uint32_t __gj_msg_flags_pack_u_7268  (alias)
 *   __libcgj_batch7268_marker = "libcgj-batch7268"
 *
 * Exclusive continuum CREATE-ONLY (7261-7270: recvmsg flag stubs —
 * peek_id, waitall_id, dontwait_id, has_peek, has_waitall, has_dontwait,
 * flags_ok, flags_pack, flags_errorish, batch_id_7270).
 * Unique gj_msg_flags_pack_u_7268 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7268_marker[] = "libcgj-batch7268";

/* Soft pack bits matching POSIX/glibc MSG_* layout. */
#define B7268_MSG_PEEK     ((uint32_t)0x2u)
#define B7268_MSG_DONTWAIT ((uint32_t)0x40u)
#define B7268_MSG_WAITALL  ((uint32_t)0x100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7268_msg_flags_pack(uint32_t u32WantPeek,
                     uint32_t u32WantDontwait,
                     uint32_t u32WantWaitall)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantPeek != 0u)
		u32Pack |= B7268_MSG_PEEK;
	if (u32WantDontwait != 0u)
		u32Pack |= B7268_MSG_DONTWAIT;
	if (u32WantWaitall != 0u)
		u32Pack |= B7268_MSG_WAITALL;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_flags_pack_u_7268 - soft-pack recvmsg flag wants.
 *
 * want_peek:     non-zero to set MSG_PEEK (0x2)
 * want_dontwait: non-zero to set MSG_DONTWAIT (0x40)
 * want_waitall:  non-zero to set MSG_WAITALL (0x100)
 *
 * Returns bitmask of requested MSG_* bits. Pure integer pack; does not
 * call recvmsg. No parent wires.
 */
uint32_t
gj_msg_flags_pack_u_7268(uint32_t u32WantPeek,
                         uint32_t u32WantDontwait,
                         uint32_t u32WantWaitall)
{
	(void)NULL;
	return b7268_msg_flags_pack(u32WantPeek, u32WantDontwait,
	                            u32WantWaitall);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_flags_pack_u_7268(uint32_t u32WantPeek,
                                    uint32_t u32WantDontwait,
                                    uint32_t u32WantWaitall)
    __attribute__((alias("gj_msg_flags_pack_u_7268")));
