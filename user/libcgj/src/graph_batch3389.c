/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3389: Ethernet II Ethertype validity (_u).
 *
 * Surface (unique symbols):
 *   int gj_ethertype_ok_u(uint16_t etype);
 *     - Return 1 if etype is a valid Ethernet II type field
 *       (etype >= 0x0600 / 1536), else 0 (length/undefined).
 *   int __gj_ethertype_ok_u  (alias)
 *   __libcgj_batch3389_marker = "libcgj-batch3389"
 *
 * Net exclusive wave (3381-3390). Unique ethertype_ok_u surface; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3389_marker[] = "libcgj-batch3389";

/* Minimum Ethernet II type value (RFC 894 / IEEE 802.3). */
#define B3389_ETYPE_MIN  0x0600u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if etype is in the Ethernet II type range. */
static int
b3389_etype_ok_p(uint16_t u16Etype)
{
	return (u16Etype >= (uint16_t)B3389_ETYPE_MIN) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ethertype_ok_u - 1 iff etype is a valid Ethernet II type (>= 1536).
 *
 * Values below 0x0600 are IEEE 802.3 length fields or undefined and
 * are rejected. Does not call libc. No parent wires.
 */
int
gj_ethertype_ok_u(uint16_t u16Etype)
{
	(void)NULL;
	return b3389_etype_ok_p(u16Etype);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_ethertype_ok_u(uint16_t u16Etype)
    __attribute__((alias("gj_ethertype_ok_u")));
