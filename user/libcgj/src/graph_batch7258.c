/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7258: sendmsg flags pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_flags_pack_u_7258(uint32_t want_dontwait,
 *                                     uint32_t want_nosignal,
 *                                     uint32_t want_more);
 *     - Pack soft MSG_* wants into a flags mask (0x40|0x4000|0x8000).
 *   uint32_t __gj_msg_flags_pack_u_7258  (alias)
 *   __libcgj_batch7258_marker = "libcgj-batch7258"
 *
 * Exclusive continuum CREATE-ONLY (7251-7260: sendmsg flag stubs —
 * dontwait_id, nosignal_id, more_id, has_dontwait, has_nosignal,
 * has_more, flags_ok, flags_pack, flags_errorish, batch_id_7260).
 * Unique gj_msg_flags_pack_u_7258 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7258_marker[] = "libcgj-batch7258";

/* Soft pack bits matching Linux MSG_* layout for common sendmsg flags. */
#define B7258_MSG_DONTWAIT ((uint32_t)0x40u)
#define B7258_MSG_NOSIGNAL ((uint32_t)0x4000u)
#define B7258_MSG_MORE     ((uint32_t)0x8000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7258_msg_flags_pack(uint32_t u32WantDontwait,
                     uint32_t u32WantNosignal,
                     uint32_t u32WantMore)
{
	uint32_t u32Pack;

	u32Pack = 0u;
	if (u32WantDontwait != 0u)
		u32Pack |= B7258_MSG_DONTWAIT;
	if (u32WantNosignal != 0u)
		u32Pack |= B7258_MSG_NOSIGNAL;
	if (u32WantMore != 0u)
		u32Pack |= B7258_MSG_MORE;
	return u32Pack;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_flags_pack_u_7258 - soft-pack sendmsg flag wants.
 *
 * want_dontwait: non-zero to set MSG_DONTWAIT (0x40)
 * want_nosignal: non-zero to set MSG_NOSIGNAL (0x4000)
 * want_more:     non-zero to set MSG_MORE (0x8000)
 *
 * Returns bitmask of requested MSG_* bits. Pure integer pack; does not
 * call sendmsg. No parent wires.
 */
uint32_t
gj_msg_flags_pack_u_7258(uint32_t u32WantDontwait,
                         uint32_t u32WantNosignal,
                         uint32_t u32WantMore)
{
	(void)NULL;
	return b7258_msg_flags_pack(u32WantDontwait, u32WantNosignal,
	                            u32WantMore);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_flags_pack_u_7258(uint32_t u32WantDontwait,
                                    uint32_t u32WantNosignal,
                                    uint32_t u32WantMore)
    __attribute__((alias("gj_msg_flags_pack_u_7258")));
