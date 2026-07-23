/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3388: Ethernet locally-administered MAC (_u).
 *
 * Surface (unique symbols):
 *   int gj_mac_is_local_u(const uint8_t mac[6]);
 *     - Return 1 if the U/L bit (bit 1 of the first octet) is set
 *       (locally administered address), else 0. mac == NULL -> 0.
 *   int __gj_mac_is_local_u  (alias)
 *   __libcgj_batch3388_marker = "libcgj-batch3388"
 *
 * Net exclusive wave (3381-3390). Unique mac_is_local_u surface; no
 * multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3388_marker[] = "libcgj-batch3388";

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if U/L bit (bit 1 of first octet) is set. */
static int
b3388_local_p(const uint8_t aMac[6])
{
	return ((aMac[0] & 0x02u) != 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mac_is_local_u - 1 iff MAC is locally administered (U/L bit set).
 *
 * IEEE 802: second-least-significant bit of the first octet is the
 * Universal/Local (U/L) bit. Set means locally administered.
 * Does not call libc. No parent wires.
 */
int
gj_mac_is_local_u(const uint8_t aMac[6])
{
	if (aMac == NULL) {
		return 0;
	}
	return b3388_local_p(aMac);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_mac_is_local_u(const uint8_t aMac[6])
    __attribute__((alias("gj_mac_is_local_u")));
