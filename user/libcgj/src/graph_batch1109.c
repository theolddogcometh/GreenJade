/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1109: Ethernet MAC address copy (6 octets).
 *
 * Surface (unique symbols):
 *   void gj_ether_copy(unsigned char dst[6], const unsigned char src[6]);
 *     — Copy six MAC octets from src to dst. Either NULL → no-op.
 *   void __gj_ether_copy  (alias)
 *   __libcgj_batch1109_marker = "libcgj-batch1109"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1109_marker[] = "libcgj-batch1109";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ether_copy — copy 6-byte Ethernet MAC from src into dst.
 */
void
gj_ether_copy(unsigned char aDst[6], const unsigned char aSrc[6])
{
	unsigned i;

	if (aDst == NULL || aSrc == NULL) {
		return;
	}

	for (i = 0u; i < 6u; i++) {
		aDst[i] = aSrc[i];
	}
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_ether_copy(unsigned char aDst[6], const unsigned char aSrc[6])
    __attribute__((alias("gj_ether_copy")));
