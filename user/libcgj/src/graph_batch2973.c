/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2973: UUID RFC 4122/9562 variant OK (_u).
 *
 * Surface (unique symbols):
 *   unsigned gj_uuid_variant_ok_u(const unsigned char u[16]);
 *     - Return 1u if octet 8 has the RFC 4122 / 9562 variant bits
 *       (10xxxxxx — high two bits == 0b10); else 0u. NULL → 0u.
 *   unsigned __gj_uuid_variant_ok_u  (alias)
 *   __libcgj_batch2973_marker = "libcgj-batch2973"
 *
 * UUID/ELF/PE exclusive continuum wave (2971-2980). Distinct from
 * gj_uuid_variant (batch163) / gj_uuid_var_u (batch746) — unique
 * gj_uuid_variant_ok_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No __int128. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2973_marker[] = "libcgj-batch2973";

/* ---- freestanding helpers ---------------------------------------------- */

/* RFC 4122 variant: top two bits of octet 8 are 10. pU non-NULL. */
static unsigned
b2973_var_ok(const unsigned char *pU)
{
	return ((pU[8] & 0xc0u) == 0x80u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_variant_ok_u - 1u if UUID has RFC 4122/9562 variant bits.
 *
 * u: pointer to 16 UUID octets (NULL → 0u)
 *
 * Accepts only the DCE / RFC variant (10xxxxxx). NCS, Microsoft, and
 * reserved variants return 0u.
 */
unsigned
gj_uuid_variant_ok_u(const unsigned char u[16])
{
	(void)NULL;
	if (u == NULL) {
		return 0u;
	}
	return b2973_var_ok(u);
}

/* ---- underscored alias ------------------------------------------------- */

unsigned __gj_uuid_variant_ok_u(const unsigned char u[16])
    __attribute__((alias("gj_uuid_variant_ok_u")));
