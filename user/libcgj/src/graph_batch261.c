/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch261: dotted-quad IPv4 text → network-order u32.
 *
 * Surface (unique symbols — gj_* only; do NOT redefine inet_pton / inet_aton):
 *   int gj_inet_pton4(const char *s, uint32_t *out_be);
 *     — Parse strict dotted-quad "A.B.C.D" (each octet 0..255 decimal).
 *       On success write 32-bit address in big-endian network order to
 *       *out_be and return 0. On failure return -1 (no write).
 *   __gj_inet_pton4  (alias)
 *   __libcgj_batch261_marker = "libcgj-batch261"
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   inet_pton / inet_aton / inet_addr  → inet.c
 *   gj_inet_pton6                      → graph_batch156.c
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch261_marker[] = "libcgj-batch261";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b261_parse_dec_octet — one decimal octet 0..255.
 * At least one digit required; at most three digits; rejects >255.
 * Leading zeros allowed (001). Advances *pp past consumed digits.
 * Returns 0 on success, -1 on failure.
 */
static int
b261_parse_dec_octet(const char **pp, unsigned *puVal)
{
	const char *p;
	unsigned u;
	int nDig;

	if (pp == NULL || puVal == NULL) {
		return -1;
	}
	p = *pp;
	if (p == NULL || *p < '0' || *p > '9') {
		return -1;
	}

	u = 0u;
	nDig = 0;
	while (*p >= '0' && *p <= '9') {
		if (nDig >= 3) {
			return -1;
		}
		u = u * 10u + (unsigned)(*p - '0');
		p++;
		nDig++;
	}
	if (nDig == 0 || u > 255u) {
		return -1;
	}
	*puVal = u;
	*pp = p;
	return 0;
}

/*
 * b261_store_be32 — write four octets into *out as big-endian network order.
 * First address octet lands at the lowest address (wire / network order).
 */
static void
b261_store_be32(uint32_t *out, unsigned o0, unsigned o1, unsigned o2,
                unsigned o3)
{
	unsigned char *b = (unsigned char *)out;

	b[0] = (unsigned char)o0;
	b[1] = (unsigned char)o1;
	b[2] = (unsigned char)o2;
	b[3] = (unsigned char)o3;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_inet_pton4 — dotted-quad IPv4 text → big-endian network-order u32.
 *
 * Accepts exactly four decimal octets 0..255 separated by '.' with no
 * leading/trailing junk (full-string match). NULL s or out_be → -1.
 *
 * Examples:
 *   "0.0.0.0"         → 00 00 00 00
 *   "127.0.0.1"       → 7f 00 00 01
 *   "192.0.2.1"       → c0 00 02 01
 *   "255.255.255.255" → ff ff ff ff
 *
 * Rejects empty, incomplete ("1.2.3"), extra dots, non-digits, overflow
 * octets (256), and trailing characters.
 */
int
gj_inet_pton4(const char *s, uint32_t *out_be)
{
	const char *p;
	unsigned a[4];
	unsigned i;

	if (s == NULL || out_be == NULL) {
		return -1;
	}

	p = s;
	for (i = 0u; i < 4u; i++) {
		if (b261_parse_dec_octet(&p, &a[i]) != 0) {
			return -1;
		}
		if (i < 3u) {
			if (*p != '.') {
				return -1;
			}
			p++;
		}
	}
	if (*p != '\0') {
		return -1;
	}

	b261_store_be32(out_be, a[0], a[1], a[2], a[3]);
	return 0;
}

int __gj_inet_pton4(const char *s, uint32_t *out_be)
    __attribute__((alias("gj_inet_pton4")));
