/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch262: UUID version 4 string format from 16
 * random octets (RFC 4122 / RFC 9562 layout).
 *
 * Surface (unique symbols):
 *   int gj_uuid_v4_format(const uint8_t rnd[16], char *out, size_t cap);
 *     — Copy rnd[16], force version nibble = 4 (octet 6) and RFC
 *       variant bits 10xxxxxx (octet 8), then write the canonical
 *       lowercase hyphenated form "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx"
 *       (36 chars + NUL) into out. Requires cap >= 37.
 *       Returns 0 on success, -1 on error (NULL args or short cap).
 *       Does not mutate the caller's rnd buffer.
 *   __gj_uuid_v4_format  (alias)
 *   __libcgj_batch262_marker = "libcgj-batch262"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   gj_uuid_parse / gj_uuid_unparse       → graph_batch116.c
 *   gj_uuid_v7_set_timestamp / version    → graph_batch163.c
 *   uuid_generate* / uuid_unparse         → graph_batch36.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string calls. No third-party UUID source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch262_marker[] = "libcgj-batch262";

/* Canonical UUID text length including terminating NUL. */
#define B262_UUID_CAP 37u

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b262_hex_digit — map nibble 0..15 to lowercase hex ASCII.
 */
static char
b262_hex_digit(unsigned uNib)
{
	return (char)((uNib < 10u) ? ('0' + uNib) : ('a' + (uNib - 10u)));
}

/*
 * b262_unparse_v4 — write 16 UUID octets as 8-4-4-4-12 lowercase hex.
 *
 * Caller guarantees out has room for B262_UUID_CAP bytes and pUu is non-NULL.
 * Hyphens are placed after octet indices 3, 5, 7, 9 (wire order).
 */
static void
b262_unparse_v4(const uint8_t pUu[16], char *szOut)
{
	/* dash before octet indices 4, 6, 8, 10 */
	static const unsigned aDashBefore[] = { 4u, 6u, 8u, 10u };
	unsigned iOct;
	unsigned iPos = 0u;

	for (iOct = 0u; iOct < 16u; iOct++) {
		unsigned iDash;

		for (iDash = 0u; iDash < 4u; iDash++) {
			if (aDashBefore[iDash] == iOct) {
				szOut[iPos++] = '-';
			}
		}
		szOut[iPos++] = b262_hex_digit(((unsigned)pUu[iOct] >> 4) & 0xfu);
		szOut[iPos++] = b262_hex_digit((unsigned)pUu[iOct] & 0xfu);
	}
	szOut[iPos] = '\0';
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uuid_v4_format — format 16 random bytes as a UUID v4 string.
 *
 * Copies rnd into a stack buffer, sets:
 *   octet 6 high nibble = 4  (version 4)
 *   octet 8 high 2 bits = 10 (RFC 4122 / 9562 variant)
 * then emits lowercase canonical text into out (need cap >= 37).
 *
 * Edge cases:
 *   rnd == NULL or out == NULL → -1
 *   cap < 37                   → -1
 * Success returns 0; does not write when failing.
 */
int
gj_uuid_v4_format(const uint8_t rnd[16], char *out, size_t cap)
{
	uint8_t aUu[16];
	unsigned i;

	if (rnd == NULL || out == NULL || cap < (size_t)B262_UUID_CAP) {
		return -1;
	}

	for (i = 0u; i < 16u; i++) {
		aUu[i] = rnd[i];
	}

	/* version = 4 in high nibble of octet 6; keep low 4 random bits */
	aUu[6] = (uint8_t)((aUu[6] & 0x0fu) | 0x40u);

	/* RFC 4122 variant 10xxxxxx in octet 8; keep low 6 random bits */
	aUu[8] = (uint8_t)((aUu[8] & 0x3fu) | 0x80u);

	b262_unparse_v4(aUu, out);
	return 0;
}

int __gj_uuid_v4_format(const uint8_t rnd[16], char *out, size_t cap)
    __attribute__((alias("gj_uuid_v4_format")));
