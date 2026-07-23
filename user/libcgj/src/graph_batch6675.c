/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6675: netlink NLMSG_MIN_TYPE soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_type_min_ok_6675(uint32_t nlmsg_type);
 *     - Soft stand-in for NLMSG_MIN_TYPE (0x10): returns 1 when
 *       nlmsg_type >= 0x10 and nlmsg_type <= 0xffff, else 0.
 *       Soft family/user message type range gate.
 *   uint32_t __gj_nlmsg_type_min_ok_6675  (alias)
 *   __libcgj_batch6675_marker = "libcgj-batch6675"
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

const char __libcgj_batch6675_marker[] = "libcgj-batch6675";

/* Soft NLMSG_MIN_TYPE and 16-bit type field max. */
#define B6675_NLMSG_MIN_TYPE  0x10u
#define B6675_TYPE_MAX        0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6675_min_ok(uint32_t u32Type)
{
	if (u32Type < B6675_NLMSG_MIN_TYPE || u32Type > B6675_TYPE_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_type_min_ok_6675 - soft NLMSG_MIN_TYPE range gate.
 *
 * nlmsg_type: netlink message type field (soft integer)
 *
 * Returns 1 when type is in [0x10, 0xffff], else 0. Pure bounds only.
 * No parent wires.
 */
uint32_t
gj_nlmsg_type_min_ok_6675(uint32_t nlmsg_type)
{
	(void)NULL;
	return b6675_min_ok(nlmsg_type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_type_min_ok_6675(uint32_t nlmsg_type)
    __attribute__((alias("gj_nlmsg_type_min_ok_6675")));
