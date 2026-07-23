/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch346: BGP common-header Marker helpers (RFC 4271).
 *
 * Surface (unique symbols):
 *   int gj_bgp_check_marker(const uint8_t m[16]);
 *     — Return 1 if m[0..15] are all 0xFF (the unauthenticated BGP Marker
 *       used for OPEN / UPDATE / NOTIFICATION / KEEPALIVE, RFC 4271 §4.1).
 *       Return 0 if m is NULL or any octet is not 0xFF.
 *   void gj_bgp_write_marker(uint8_t m[16]);
 *     — Write the all-ones 16-octet BGP Marker into m[0..15]. No-op if m
 *       is NULL.
 *   __gj_bgp_check_marker  (alias)
 *   __gj_bgp_write_marker  (alias)
 *   __libcgj_batch346_marker = "libcgj-batch346"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party BGP source was copied. Distinct
 * from batch153 (OPEN body parse embeds a private marker scan).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch346_marker[] = "libcgj-batch346";

/* BGP Marker length (RFC 4271 §4.1). */
#define B346_MARKER_LEN  16u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bgp_check_marker — true when the 16-octet Marker is all ones.
 *
 * m: pointer to at least 16 readable octets, or NULL.
 *
 * Returns 1 only when every octet is 0xFF; otherwise 0.
 */
int
gj_bgp_check_marker(const uint8_t m[16])
{
	unsigned i;

	if (m == NULL) {
		return 0;
	}

	for (i = 0u; i < B346_MARKER_LEN; i++) {
		if (m[i] != 0xffu) {
			return 0;
		}
	}
	return 1;
}

/*
 * gj_bgp_write_marker — fill the 16-octet all-ones BGP Marker.
 *
 * m: pointer to at least 16 writable octets, or NULL (no-op).
 */
void
gj_bgp_write_marker(uint8_t m[16])
{
	unsigned i;

	if (m == NULL) {
		return;
	}

	for (i = 0u; i < B346_MARKER_LEN; i++) {
		m[i] = 0xffu;
	}
}

int __gj_bgp_check_marker(const uint8_t m[16])
    __attribute__((alias("gj_bgp_check_marker")));

void __gj_bgp_write_marker(uint8_t m[16])
    __attribute__((alias("gj_bgp_write_marker")));
