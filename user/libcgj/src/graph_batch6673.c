/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6673: netlink NLMSG_DONE type soft predicate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_type_done_p_6673(uint32_t nlmsg_type);
 *     - Soft stand-in for NLMSG_DONE (3): returns 1 when nlmsg_type == 3,
 *       else 0. Soft end-of-multipart dump marker only.
 *   uint32_t __gj_nlmsg_type_done_p_6673  (alias)
 *   __libcgj_batch6673_marker = "libcgj-batch6673"
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

const char __libcgj_batch6673_marker[] = "libcgj-batch6673";

/* Soft NLMSG_DONE type code. */
#define B6673_NLMSG_DONE  3u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6673_done_p(uint32_t u32Type)
{
	if (u32Type == B6673_NLMSG_DONE) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_type_done_p_6673 - soft NLMSG_DONE type predicate.
 *
 * nlmsg_type: netlink message type field (soft integer)
 *
 * Returns 1 when type equals soft DONE (3), else 0. Pure data only.
 * No parent wires.
 */
uint32_t
gj_nlmsg_type_done_p_6673(uint32_t nlmsg_type)
{
	(void)NULL;
	return b6673_done_p(nlmsg_type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_type_done_p_6673(uint32_t nlmsg_type)
    __attribute__((alias("gj_nlmsg_type_done_p_6673")));
