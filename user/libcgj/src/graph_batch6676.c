/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6676: netlink control message type soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_type_ctrl_ok_6676(uint32_t nlmsg_type);
 *     - Soft control-type gate: ok when type is in [1, 4]
 *       (NOOP/ERROR/DONE/OVERRUN soft codes). Type 0 and >= 5 → 0.
 *   uint32_t __gj_nlmsg_type_ctrl_ok_6676  (alias)
 *   __libcgj_batch6676_marker = "libcgj-batch6676"
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

const char __libcgj_batch6676_marker[] = "libcgj-batch6676";

/* Soft control type range: NLMSG_NOOP .. NLMSG_OVERRUN. */
#define B6676_CTRL_MIN  1u
#define B6676_CTRL_MAX  4u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6676_ctrl_ok(uint32_t u32Type)
{
	if (u32Type < B6676_CTRL_MIN || u32Type > B6676_CTRL_MAX) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_type_ctrl_ok_6676 - soft netlink control type range gate.
 *
 * nlmsg_type: netlink message type field (soft integer)
 *
 * Returns 1 when type is in [1, 4], else 0. Pure bounds only.
 * No parent wires.
 */
uint32_t
gj_nlmsg_type_ctrl_ok_6676(uint32_t nlmsg_type)
{
	(void)NULL;
	return b6676_ctrl_ok(nlmsg_type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_type_ctrl_ok_6676(uint32_t nlmsg_type)
    __attribute__((alias("gj_nlmsg_type_ctrl_ok_6676")));
