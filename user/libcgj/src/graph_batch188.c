/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch188: Windows SID string parse (S-1-5-...).
 * Clean-room freestanding pure C — integer/pointer only (no libc string,
 * no errno). Builds under userspace -msse2. No third-party SID source
 * was copied.
 *
 * Surface (unique symbols):
 *   int gj_sid_parse(const char *s, unsigned *rev, uint64_t *auth,
 *                    unsigned *sub_count, uint32_t *subs, size_t sub_cap);
 *     Parse a textual SID in the ConvertSidToStringSid form:
 *       S-<revision>-<authority>[-<subauthority>]*
 *     Examples:
 *       "S-1-5-18"
 *       "S-1-5-21-3623811015-3361044348-30300820-1013"
 *       "S-1-0-0"
 *     Grammar (strict, no whitespace):
 *       'S' | 's'
 *       '-'  decimal-revision          (0..255)
 *       '-'  authority                 (decimal, or 0x/0X hex; 0..2^48-1)
 *       ( '-' decimal-subauthority )*  (each 0..0xffffffff; at most 15)
 *       NUL
 *     On success writes *rev, *auth, *sub_count and up to *sub_count
 *     entries into subs[0..], returns 0. *sub_count is the full count
 *     of subauthorities in the string (0..15). Requires sub_cap >=
 *     that count when count > 0 (subs may be NULL only when count is 0).
 *     On failure returns -1 and leaves outputs untouched.
 *   __gj_sid_parse  (alias)
 *   __libcgj_batch188_marker = "libcgj-batch188"
 *
 * No malloc, no errno (freestanding-safe).
 */

#include <stddef.h>
#include <stdint.h>

/* Windows SID_MAX_SUB_AUTHORITIES. */
#define B188_MAX_SUBS 15u

/* IdentifierAuthority is 6 bytes (48 bits). */
#define B188_AUTH_MAX 0xffffffffffffull

const char __libcgj_batch188_marker[] = "libcgj-batch188";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b188_is_digit(unsigned char ch)
{
	return ch >= (unsigned char)'0' && ch <= (unsigned char)'9';
}

static int
b188_hex_val(unsigned char ch)
{
	if (ch >= (unsigned char)'0' && ch <= (unsigned char)'9') {
		return (int)(ch - (unsigned char)'0');
	}
	if (ch >= (unsigned char)'A' && ch <= (unsigned char)'F') {
		return (int)(ch - (unsigned char)'A') + 10;
	}
	if (ch >= (unsigned char)'a' && ch <= (unsigned char)'f') {
		return (int)(ch - (unsigned char)'a') + 10;
	}
	return -1;
}

/*
 * Parse one or more ASCII decimal digits from *pp into *pOut (uint64_t).
 * Rejects empty digit runs and overflow past uMax (inclusive). Advances
 * *pp past the digits. Returns 0 on success, -1 on failure.
 */
static int
b188_parse_u64_dec(const char **pp, uint64_t *pOut, uint64_t uMax)
{
	const char *p = *pp;
	uint64_t u = 0ull;
	int fDig = 0;

	while (b188_is_digit((unsigned char)*p)) {
		unsigned d = (unsigned)(*p - '0');

		fDig = 1;
		/* Overflow / range: u * 10 + d > uMax */
		if (u > uMax / 10ull) {
			return -1;
		}
		u *= 10ull;
		if (u > uMax - (uint64_t)d) {
			return -1;
		}
		u += (uint64_t)d;
		p++;
	}
	if (!fDig) {
		return -1;
	}
	*pOut = u;
	*pp = p;
	return 0;
}

/*
 * Parse "0x" / "0X" hex authority (one or more hex digits) into *pOut.
 * Range limited to uMax. Advances *pp. Returns 0 ok, -1 fail.
 * *pp must currently point at '0'.
 */
static int
b188_parse_u64_hex(const char **pp, uint64_t *pOut, uint64_t uMax)
{
	const char *p = *pp;
	uint64_t u = 0ull;
	int fDig = 0;
	int d;

	if (p[0] != '0' || (p[1] != 'x' && p[1] != 'X')) {
		return -1;
	}
	p += 2;
	d = b188_hex_val((unsigned char)*p);
	if (d < 0) {
		return -1;
	}
	while ((d = b188_hex_val((unsigned char)*p)) >= 0) {
		fDig = 1;
		if (u > (uMax >> 4)) {
			return -1;
		}
		u = (u << 4) | (uint64_t)(unsigned)d;
		if (u > uMax) {
			return -1;
		}
		p++;
	}
	if (!fDig) {
		return -1;
	}
	*pOut = u;
	*pp = p;
	return 0;
}

/*
 * Parse identifier authority: hex if "0x"/"0X" prefix, else decimal.
 * Authority is a 48-bit field in the binary SID.
 */
static int
b188_parse_auth(const char **pp, uint64_t *pOut)
{
	const char *p = *pp;

	if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
		return b188_parse_u64_hex(pp, pOut, B188_AUTH_MAX);
	}
	return b188_parse_u64_dec(pp, pOut, B188_AUTH_MAX);
}

/* ---- public: gj_sid_parse ---------------------------------------------- */

/*
 * gj_sid_parse — parse "S-R-I[-S...]" Windows textual SID into components.
 *
 * Writes outputs only on success. sub_cap must be large enough to hold
 * every subauthority present (Windows max 15).
 */
int
gj_sid_parse(const char *s, unsigned *rev, uint64_t *auth,
             unsigned *sub_count, uint32_t *subs, size_t sub_cap)
{
	const char *p;
	uint64_t uRev;
	uint64_t uAuth;
	uint32_t aSubs[B188_MAX_SUBS];
	unsigned nSubs = 0u;
	unsigned i;

	if (s == NULL || rev == NULL || auth == NULL || sub_count == NULL) {
		return -1;
	}

	p = s;

	/* Leading 'S' / 's'. */
	if (*p != 'S' && *p != 's') {
		return -1;
	}
	p++;
	if (*p != '-') {
		return -1;
	}
	p++;

	/* Revision: decimal, fits in a BYTE (0..255). */
	if (b188_parse_u64_dec(&p, &uRev, 255ull) != 0) {
		return -1;
	}
	if (*p != '-') {
		return -1;
	}
	p++;

	/* Identifier authority (decimal or 0x-hex, ≤ 48 bits). */
	if (b188_parse_auth(&p, &uAuth) != 0) {
		return -1;
	}

	/* Zero or more "-<subauthority>" decimal DWORDs (max 15). */
	while (*p == '-') {
		uint64_t uSub;

		p++;
		if (nSubs >= B188_MAX_SUBS) {
			return -1;
		}
		if (b188_parse_u64_dec(&p, &uSub, 0xffffffffull) != 0) {
			return -1;
		}
		aSubs[nSubs] = (uint32_t)uSub;
		nSubs++;
	}

	/* No trailing garbage. */
	if (*p != '\0') {
		return -1;
	}

	/* Caller buffer must hold every subauthority. */
	if (nSubs > 0u) {
		if (subs == NULL || sub_cap < (size_t)nSubs) {
			return -1;
		}
		for (i = 0u; i < nSubs; i++) {
			subs[i] = aSubs[i];
		}
	}

	*rev = (unsigned)uRev;
	*auth = uAuth;
	*sub_count = nSubs;
	return 0;
}

int __gj_sid_parse(const char *s, unsigned *rev, uint64_t *auth,
                   unsigned *sub_count, uint32_t *subs, size_t sub_cap)
    __attribute__((alias("gj_sid_parse")));
