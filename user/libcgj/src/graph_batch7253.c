/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7253: sendmsg MSG_MORE flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_more_id_7253(void);
 *     - Return soft MSG_MORE constant (0x8000).
 *   uint32_t __gj_msg_more_id_7253  (alias)
 *   __libcgj_batch7253_marker = "libcgj-batch7253"
 *
 * Exclusive continuum CREATE-ONLY (7251-7260: sendmsg flag stubs —
 * dontwait_id, nosignal_id, more_id, has_dontwait, has_nosignal,
 * has_more, flags_ok, flags_pack, flags_errorish, batch_id_7260).
 * Unique gj_msg_more_id_7253 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7253_marker[] = "libcgj-batch7253";

/* MSG_MORE: more data will follow (0x8000). */
#define B7253_MSG_MORE ((uint32_t)0x8000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7253_msg_more_id(void)
{
	return B7253_MSG_MORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_more_id_7253 - soft MSG_MORE flag constant.
 *
 * Always returns 0x8000 (Linux MSG_MORE). Catalog id only; does not
 * call sendmsg. No parent wires.
 */
uint32_t
gj_msg_more_id_7253(void)
{
	(void)NULL;
	return b7253_msg_more_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_more_id_7253(void)
    __attribute__((alias("gj_msg_more_id_7253")));
