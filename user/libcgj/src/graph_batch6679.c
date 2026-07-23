/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6679: netlink type+flags soft pack stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nlmsg_type_pack_6679(uint32_t nlmsg_type,
 *                                    uint32_t nlmsg_flags);
 *     - Soft pack word: (nlmsg_type & 0xffff) | ((nlmsg_flags & 0xffff) << 16).
 *       Soft stand-in for folding type/flags fields; not wire encode.
 *   uint32_t __gj_nlmsg_type_pack_6679  (alias)
 *   __libcgj_batch6679_marker = "libcgj-batch6679"
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

const char __libcgj_batch6679_marker[] = "libcgj-batch6679";

#define B6679_FIELD_MASK  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6679_type_pack(uint32_t u32Type, uint32_t u32Flags)
{
	return (u32Type & B6679_FIELD_MASK) |
	    ((u32Flags & B6679_FIELD_MASK) << 16);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nlmsg_type_pack_6679 - soft netlink type/flags fold.
 *
 * nlmsg_type:  soft type field
 * nlmsg_flags: soft flags field (e.g. NLM_F_* stand-ins)
 *
 * Returns low 16 bits type, high 16 bits flags. Pure integer fold.
 * No parent wires.
 */
uint32_t
gj_nlmsg_type_pack_6679(uint32_t nlmsg_type, uint32_t nlmsg_flags)
{
	(void)NULL;
	return b6679_type_pack(nlmsg_type, nlmsg_flags);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nlmsg_type_pack_6679(uint32_t nlmsg_type, uint32_t nlmsg_flags)
    __attribute__((alias("gj_nlmsg_type_pack_6679")));
