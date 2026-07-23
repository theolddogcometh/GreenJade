/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7252: sendmsg MSG_NOSIGNAL flag id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_msg_nosignal_id_7252(void);
 *     - Return soft MSG_NOSIGNAL constant (0x4000).
 *   uint32_t __gj_msg_nosignal_id_7252  (alias)
 *   __libcgj_batch7252_marker = "libcgj-batch7252"
 *
 * Exclusive continuum CREATE-ONLY (7251-7260: sendmsg flag stubs —
 * dontwait_id, nosignal_id, more_id, has_dontwait, has_nosignal,
 * has_more, flags_ok, flags_pack, flags_errorish, batch_id_7260).
 * Unique gj_msg_nosignal_id_7252 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7252_marker[] = "libcgj-batch7252";

/* MSG_NOSIGNAL: do not generate SIGPIPE on errors (0x4000). */
#define B7252_MSG_NOSIGNAL ((uint32_t)0x4000u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7252_msg_nosignal_id(void)
{
	return B7252_MSG_NOSIGNAL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_msg_nosignal_id_7252 - soft MSG_NOSIGNAL flag constant.
 *
 * Always returns 0x4000 (Linux MSG_NOSIGNAL). Catalog id only; does not
 * call sendmsg. No parent wires.
 */
uint32_t
gj_msg_nosignal_id_7252(void)
{
	(void)NULL;
	return b7252_msg_nosignal_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_msg_nosignal_id_7252(void)
    __attribute__((alias("gj_msg_nosignal_id_7252")));
