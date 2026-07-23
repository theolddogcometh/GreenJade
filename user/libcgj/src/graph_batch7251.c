/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7251: sendmsg MSG_DONTWAIT flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_dontwait_id_7251(void);
 *     - Return soft MSG_DONTWAIT constant (0x40).
 *   uint32_t __gj_msg_dontwait_id_7251  (alias)
 *   __libcgj_batch7251_marker = "libcgj-batch7251"
 *
 * Exclusive continuum CREATE-ONLY (7251-7260: sendmsg flag stubs —
 * dontwait_id, nosignal_id, more_id, has_dontwait, has_nosignal,
 * has_more, flags_ok, flags_pack, flags_errorish, batch_id_7260).
 * Unique gj_msg_dontwait_id_7251 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7251_marker[] = "libcgj-batch7251";

/* MSG_DONTWAIT: nonblocking send/recv (0x40). */
#define B7251_MSG_DONTWAIT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7251_msg_dontwait_id(void)
{
	return B7251_MSG_DONTWAIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_dontwait_id_7251 - soft MSG_DONTWAIT flag constant.
 *
 * Always returns 0x40 (Linux MSG_DONTWAIT). Catalog id only; does not
 * call sendmsg. No parent wires.
 */
uint32_t
gj_msg_dontwait_id_7251(void)
{
	(void)NULL;
	return b7251_msg_dontwait_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_dontwait_id_7251(void)
    __attribute__((alias("gj_msg_dontwait_id_7251")));
