/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7265: recvmsg MSG_WAITALL presence stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_has_waitall_u_7265(uint32_t flags);
 *     - Return 1 if flags has MSG_WAITALL (0x100) set, else 0.
 *   uint32_t __gj_msg_has_waitall_u_7265  (alias)
 *   __libcgj_batch7265_marker = "libcgj-batch7265"
 *
 * Exclusive continuum CREATE-ONLY (7261-7270: recvmsg flag stubs —
 * peek_id, waitall_id, dontwait_id, has_peek, has_waitall, has_dontwait,
 * flags_ok, flags_pack, flags_errorish, batch_id_7270).
 * Unique gj_msg_has_waitall_u_7265 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7265_marker[] = "libcgj-batch7265";

/* MSG_WAITALL bit (0x100). */
#define B7265_MSG_WAITALL ((uint32_t)0x100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7265_msg_has_waitall(uint32_t u32Flags)
{
	return ((u32Flags & B7265_MSG_WAITALL) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_has_waitall_u_7265 - 1 if flags includes MSG_WAITALL.
 *
 * flags: recvmsg(2) / recv(2) flags argument
 *
 * Returns 1 when bit 0x100 is set, else 0. Soft bit test only; does not
 * call recvmsg. No parent wires.
 */
uint32_t
gj_msg_has_waitall_u_7265(uint32_t u32Flags)
{
	(void)NULL;
	return b7265_msg_has_waitall(u32Flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_has_waitall_u_7265(uint32_t u32Flags)
    __attribute__((alias("gj_msg_has_waitall_u_7265")));
