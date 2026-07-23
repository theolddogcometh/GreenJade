/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch628: force UUID version-4 + RFC 4122 variant
 * layout bits in place (no RNG, no format).
 *
 * Surface (unique symbols):
 *   void gj_uuid_set_v4_bits(unsigned char u[16]);
 *     — In-place: set version nibble = 4 on octet 6 and RFC 4122/9562
 *       variant bits 10xxxxxx on octet 8. Leaves remaining random bits
 *       untouched. NULL → no-op.
 *   void __gj_uuid_set_v4_bits  (alias)
 *   __libcgj_batch628_marker = "libcgj-batch628"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_uuid_parse / gj_uuid_unparse       → graph_batch116.c
 *   gj_uuid_v7_set_timestamp / version    → graph_batch163.c
 *   gj_uuid_v4_format                     → graph_batch262.c
 *   gj_uuid_parse_strict                  → graph_batch332.c
 *   gj_uuid_is_nil / gj_uuid_nil          → graph_batch467.c
 *   gj_uuid_cmp                           → graph_batch468.c
 *   uuid_generate* / uuid_unparse         → graph_batch36.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party UUID source copied.
 */

#include <stddef.h>

const char __libcgj_batch628_marker[] = "libcgj-batch628";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_set_v4_bits — stamp version 4 + RFC 4122 variant bits in place.
 *
 * Wire layout (RFC 4122 / RFC 9562):
 *   octet 6 high nibble = 4  (version 4); low 4 bits preserved
 *   octet 8 high 2 bits = 10 (variant);   low 6 bits preserved
 *
 * Edge cases:
 *   u == NULL → no-op
 */
void
gj_uuid_set_v4_bits(unsigned char u[16])
{
	if (u == NULL) {
		return;
	}

	/* version = 4 in high nibble of octet 6; keep low 4 bits */
	u[6] = (unsigned char)((u[6] & 0x0fu) | 0x40u);

	/* RFC 4122 variant 10xxxxxx in octet 8; keep low 6 bits */
	u[8] = (unsigned char)((u[8] & 0x3fu) | 0x80u);
}

/* ---- underscored alias ------------------------------------------------- */

void __gj_uuid_set_v4_bits(unsigned char u[16])
    __attribute__((alias("gj_uuid_set_v4_bits")));
