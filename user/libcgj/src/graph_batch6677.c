/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6677: netlink family-scoped type soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_type_family_ok_6677(uint32_t nlmsg_type,
 *                                         uint32_t type_base,
 *                                         uint32_t type_count);
 *     - Soft family type gate: type_count 0 → 0; else ok when
 *       nlmsg_type is in [type_base, type_base + type_count - 1] without
 *       wrap, and type_base >= 0x10. Soft stand-in for family cmd range.
 *   uint32_t __gj_nlmsg_type_family_ok_6677  (alias)
 *   __libcgj_batch6677_marker = "libcgj-batch6677"
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

const char __libcgj_batch6677_marker[] = "libcgj-batch6677";

/* Soft NLMSG_MIN_TYPE floor for family-scoped types. */
#define B6677_NLMSG_MIN_TYPE  0x10u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6677_family_ok(uint32_t u32Type, uint32_t u32Base, uint32_t u32Count)
{
	uint32_t u32Last;

	if (u32Count == 0u) {
		return 0u;
	}
	if (u32Base < B6677_NLMSG_MIN_TYPE) {
		return 0u;
	}
	/* Reject wrap of type_base + type_count - 1. */
	if (u32Count > (0xffffffffu - u32Base + 1u)) {
		return 0u;
	}
	u32Last = u32Base + u32Count - 1u;
	if (u32Type < u32Base || u32Type > u32Last) {
		return 0u;
	}
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_type_family_ok_6677 - soft family-scoped nlmsg type gate.
 *
 * nlmsg_type: candidate type
 * type_base:  family first type (must be >= 0x10)
 * type_count: number of types in the soft family window
 *
 * Returns 1 when type is inside [type_base, type_base+type_count-1]
 * without wrap and type_base is at least soft MIN_TYPE, else 0.
 * No parent wires.
 */
uint32_t
gj_nlmsg_type_family_ok_6677(uint32_t nlmsg_type, uint32_t type_base,
    uint32_t type_count)
{
	(void)NULL;
	return b6677_family_ok(nlmsg_type, type_base, type_count);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_type_family_ok_6677(uint32_t nlmsg_type,
    uint32_t type_base, uint32_t type_count)
    __attribute__((alias("gj_nlmsg_type_family_ok_6677")));
