/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6678: netlink nlmsg header length soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_hdr_len_ok_6678(uint32_t nlmsg_len);
 *     - Soft nlmsghdr length gate: ok when nlmsg_len >= 16 (soft header
 *       size) and nlmsg_len <= 0xffff (soft sk_buff style cap). Else 0.
 *   uint32_t __gj_nlmsg_hdr_len_ok_6678  (alias)
 *   __libcgj_batch6678_marker = "libcgj-batch6678"
 *
 * Exclusive continuum CREATE-ONLY (6671-6680: netlink message type stubs —
 * type_noop_p_6671, type_error_p_6672, type_done_p_6673,
 * type_overrun_p_6674, type_min_ok_6675, type_ctrl_ok_6676,
 * type_family_ok_6677, hdr_len_ok_6678, type_pack_6679,
 * type_ready_6680). Unique surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6678_marker[] = "libcgj-batch6678";

/* Soft sizeof(nlmsghdr) and upper soft message length cap. */
#define B6678_NLMSG_HDR_MIN  16u
#define B6678_NLMSG_LEN_MAX  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6678_hdr_len_ok(uint32_t u32Len)
{
	if (u32Len < B6678_NLMSG_HDR_MIN || u32Len > B6678_NLMSG_LEN_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_hdr_len_ok_6678 - soft netlink message total-length gate.
 *
 * nlmsg_len: claimed nlmsghdr.nlmsg_len (soft integer)
 *
 * Returns 1 when length is in [16, 0xffff], else 0. Pure bounds only.
 * No parent wires.
 */
uint32_t
gj_nlmsg_hdr_len_ok_6678(uint32_t nlmsg_len)
{
	(void)NULL;
	return b6678_hdr_len_ok(nlmsg_len);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_hdr_len_ok_6678(uint32_t nlmsg_len)
    __attribute__((alias("gj_nlmsg_hdr_len_ok_6678")));
