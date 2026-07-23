/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7263: recvmsg MSG_DONTWAIT flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_dontwait_id_7263(void);
 *     - Return soft MSG_DONTWAIT constant (0x40).
 *   uint32_t __gj_msg_dontwait_id_7263  (alias)
 *   __libcgj_batch7263_marker = "libcgj-batch7263"
 *
 * Exclusive continuum CREATE-ONLY (7261-7270: recvmsg flag stubs —
 * peek_id, waitall_id, dontwait_id, has_peek, has_waitall, has_dontwait,
 * flags_ok, flags_pack, flags_errorish, batch_id_7270).
 * Unique gj_msg_dontwait_id_7263 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7263_marker[] = "libcgj-batch7263";

/* MSG_DONTWAIT: nonblocking operation (0x40). */
#define B7263_MSG_DONTWAIT ((uint32_t)0x40u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7263_msg_dontwait_id(void)
{
	return B7263_MSG_DONTWAIT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_dontwait_id_7263 - soft MSG_DONTWAIT flag constant.
 *
 * Always returns 0x40 (POSIX/glibc MSG_DONTWAIT). Catalog id only; does
 * not call recvmsg. No parent wires.
 */
uint32_t
gj_msg_dontwait_id_7263(void)
{
	(void)NULL;
	return b7263_msg_dontwait_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_dontwait_id_7263(void)
    __attribute__((alias("gj_msg_dontwait_id_7263")));
