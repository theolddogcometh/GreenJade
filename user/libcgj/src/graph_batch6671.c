/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6671: netlink NLMSG_NOOP type soft predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_type_noop_p_6671(uint32_t nlmsg_type);
 *     - Soft stand-in for NLMSG_NOOP (1): returns 1 when nlmsg_type == 1,
 *       else 0. Not a live netlink socket probe.
 *   uint32_t __gj_nlmsg_type_noop_p_6671  (alias)
 *   __libcgj_batch6671_marker = "libcgj-batch6671"
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

const char __libcgj_batch6671_marker[] = "libcgj-batch6671";

/* Soft NLMSG_NOOP type code. */
#define B6671_NLMSG_NOOP  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6671_noop_p(uint32_t u32Type)
{
	if (u32Type == B6671_NLMSG_NOOP) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_type_noop_p_6671 - soft NLMSG_NOOP type predicate.
 *
 * nlmsg_type: netlink message type field (soft integer)
 *
 * Returns 1 when type equals soft NOOP (1), else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_nlmsg_type_noop_p_6671(uint32_t nlmsg_type)
{
	(void)NULL;
	return b6671_noop_p(nlmsg_type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_type_noop_p_6671(uint32_t nlmsg_type)
    __attribute__((alias("gj_nlmsg_type_noop_p_6671")));
