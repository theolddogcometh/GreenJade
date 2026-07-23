/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7261: recvmsg MSG_PEEK flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_peek_id_7261(void);
 *     - Return soft MSG_PEEK constant (0x2).
 *   uint32_t __gj_msg_peek_id_7261  (alias)
 *   __libcgj_batch7261_marker = "libcgj-batch7261"
 *
 * Exclusive continuum CREATE-ONLY (7261-7270: recvmsg flag stubs —
 * peek_id, waitall_id, dontwait_id, has_peek, has_waitall, has_dontwait,
 * flags_ok, flags_pack, flags_errorish, batch_id_7270).
 * Unique gj_msg_peek_id_7261 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7261_marker[] = "libcgj-batch7261";

/* MSG_PEEK: peek at incoming data without removing it (0x2). */
#define B7261_MSG_PEEK ((uint32_t)0x2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7261_msg_peek_id(void)
{
	return B7261_MSG_PEEK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_peek_id_7261 - soft MSG_PEEK flag constant.
 *
 * Always returns 0x2 (POSIX/glibc MSG_PEEK). Catalog id only; does not
 * call recvmsg. No parent wires.
 */
uint32_t
gj_msg_peek_id_7261(void)
{
	(void)NULL;
	return b7261_msg_peek_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_peek_id_7261(void)
    __attribute__((alias("gj_msg_peek_id_7261")));
