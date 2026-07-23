/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7266: recvmsg MSG_DONTWAIT presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_has_dontwait_u_7266(uint32_t flags);
 *     - Return 1 if flags has MSG_DONTWAIT (0x40) set, else 0.
 *   uint32_t __gj_msg_has_dontwait_u_7266  (alias)
 *   __libcgj_batch7266_marker = "libcgj-batch7266"
 *
 * Exclusive continuum CREATE-ONLY (7261-7270: recvmsg flag stubs —
 * peek_id, waitall_id, dontwait_id, has_peek, has_waitall, has_dontwait,
 * flags_ok, flags_pack, flags_errorish, batch_id_7270).
 * Unique gj_msg_has_dontwait_u_7266 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7266_marker[] = "libcgj-batch7266";

/* MSG_DONTWAIT bit (0x40). */
#define B7266_MSG_DONTWAIT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7266_msg_has_dontwait(uint32_t u32Flags)
{
	return ((u32Flags & B7266_MSG_DONTWAIT) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_has_dontwait_u_7266 - 1 if flags includes MSG_DONTWAIT.
 *
 * flags: recvmsg(2) / recv(2) flags argument
 *
 * Returns 1 when bit 0x40 is set, else 0. Soft bit test only; does not
 * call recvmsg. No parent wires.
 */
uint32_t
gj_msg_has_dontwait_u_7266(uint32_t u32Flags)
{
	(void)NULL;
	return b7266_msg_has_dontwait(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_has_dontwait_u_7266(uint32_t u32Flags)
    __attribute__((alias("gj_msg_has_dontwait_u_7266")));
