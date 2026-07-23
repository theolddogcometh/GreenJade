/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3672: freestanding TLV length fit predicate
 * (_u surface).
 *
 * Surface (unique symbols):
 *   int gj_tlv_len_ok_u(uint32_t len, uint32_t avail);
 *     - Return 1 if a TLV value of len octets fits in avail remaining
 *       octets (len <= avail), else 0. Both zero is ok (empty value in
 *       empty remainder). Pure integer compare — no pointer walk.
 *   int __gj_tlv_len_ok_u  (alias)
 *   __libcgj_batch3672_marker = "libcgj-batch3672"
 *
 * CREATE-ONLY exclusive continuum wave (3671-3680). Unique
 * gj_tlv_len_ok_u surface only; no multi-def. Pairs with
 * gj_tlv_tag_ok_u (batch3671). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3672_marker[] = "libcgj-batch3672";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if u32Len octets fit in u32Avail remaining. */
static int
b3672_len_ok(uint32_t u32Len, uint32_t u32Avail)
{
	return (u32Len <= u32Avail) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_tlv_len_ok_u - true if TLV value length fits remaining capacity.
 *
 * len:   declared value length in octets
 * avail: remaining octets after the length field
 *
 * Pure predicate; does not allocate or touch memory. No parent wires.
 */
int
gj_tlv_len_ok_u(uint32_t len, uint32_t avail)
{
	(void)NULL;
	return b3672_len_ok(len, avail);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_tlv_len_ok_u(uint32_t len, uint32_t avail)
    __attribute__((alias("gj_tlv_len_ok_u")));
