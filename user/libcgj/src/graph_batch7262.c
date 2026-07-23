/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7262: recvmsg MSG_WAITALL flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_waitall_id_7262(void);
 *     - Return soft MSG_WAITALL constant (0x100).
 *   uint32_t __gj_msg_waitall_id_7262  (alias)
 *   __libcgj_batch7262_marker = "libcgj-batch7262"
 *
 * Exclusive continuum CREATE-ONLY (7261-7270: recvmsg flag stubs —
 * peek_id, waitall_id, dontwait_id, has_peek, has_waitall, has_dontwait,
 * flags_ok, flags_pack, flags_errorish, batch_id_7270).
 * Unique gj_msg_waitall_id_7262 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7262_marker[] = "libcgj-batch7262";

/* MSG_WAITALL: wait for full request (0x100). */
#define B7262_MSG_WAITALL ((uint32_t)0x100u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7262_msg_waitall_id(void)
{
	return B7262_MSG_WAITALL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_waitall_id_7262 - soft MSG_WAITALL flag constant.
 *
 * Always returns 0x100 (POSIX/glibc MSG_WAITALL). Catalog id only; does
 * not call recvmsg. No parent wires.
 */
uint32_t
gj_msg_waitall_id_7262(void)
{
	(void)NULL;
	return b7262_msg_waitall_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_waitall_id_7262(void)
    __attribute__((alias("gj_msg_waitall_id_7262")));
