/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch278: duration parse to u64 whole seconds.
 *
 * Surface (unique symbols):
 *   int gj_duration_parse_sec(const char *s, uint64_t *out_sec);
 *     Parse a compact duration string into whole seconds (unsigned).
 *     Accepts either:
 *       (1) plain decimal seconds:  DIGITS          e.g. "3723", "0"
 *       (2) unit form:              (DIGITS UNIT)+  e.g. "1h2m3s"
 *     UNIT is one of: h (hour=3600), m (min=60), s (sec=1).
 *     Components may appear in any order; each unit letter at most once.
 *     DIGITS is one or more ASCII decimal digits (no sign, no fraction).
 *     Returns 0 on success, -1 on error (NULL args, empty, bad unit,
 *     duplicate unit, overflow, trailing junk, bare unit without digits).
 *
 *   __gj_duration_parse_sec  (alias)
 *   __libcgj_batch278_marker = "libcgj-batch278"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Distinct from graph_batch148 (int64 /
 * signed / d-h-m-s surface).
 *
 * Examples:
 *   "1h2m3s"  →  3723
 *   "2h"      →  7200
 *   "45"      →  45
 *   "45s"     →  45
 *   "0"       →  0
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch278_marker[] = "libcgj-batch278";

/* Seconds per unit (h / m / s only). */
#define B278_SEC_H 3600ull
#define B278_SEC_M   60ull
#define B278_SEC_S    1ull

/* Max u64 for overflow checks. */
#define B278_U64_MAX 0xffffffffffffffffull

/* ---- freestanding helpers ---------------------------------------------- */

static int
b278_is_digit(unsigned char ch)
{
	return ch >= '0' && ch <= '9';
}

/*
 * Unit letter → seconds multiplier. Returns 0 if not a known unit.
 */
static uint64_t
b278_unit_secs(unsigned char ch)
{
	if (ch == 'h') {
		return B278_SEC_H;
	}
	if (ch == 'm') {
		return B278_SEC_M;
	}
	if (ch == 's') {
		return B278_SEC_S;
	}
	return 0ull;
}

/*
 * Unit bit for duplicate detection: h=1, m=2, s=4.
 */
static unsigned
b278_unit_bit(unsigned char ch)
{
	if (ch == 'h') {
		return 1u;
	}
	if (ch == 'm') {
		return 2u;
	}
	if (ch == 's') {
		return 4u;
	}
	return 0u;
}

/*
 * Multiply uVal * uMul into *pOut with overflow check.
 * Returns 0 ok, -1 overflow. Both factors are non-negative.
 */
static int
b278_mul_ok(uint64_t uVal, uint64_t uMul, uint64_t *pOut)
{
	if (uVal == 0ull || uMul == 0ull) {
		*pOut = 0ull;
		return 0;
	}
	if (uVal > B278_U64_MAX / uMul) {
		return -1;
	}
	*pOut = uVal * uMul;
	return 0;
}

/*
 * Add two uint64 values with overflow check.
 */
static int
b278_add_ok(uint64_t uA, uint64_t uB, uint64_t *pOut)
{
	if (uA > B278_U64_MAX - uB) {
		return -1;
	}
	*pOut = uA + uB;
	return 0;
}

/*
 * True if s is non-empty and every character is an ASCII digit.
 */
static int
b278_all_digits(const char *sz)
{
	const char *p = sz;

	if (*p == '\0') {
		return 0;
	}
	while (*p != '\0') {
		if (!b278_is_digit((unsigned char)*p)) {
			return 0;
		}
		p++;
	}
	return 1;
}

/*
 * Parse plain decimal digits into *pOut. Overflow → -1.
 * Caller guarantees at least one digit and all-digit string.
 */
static int
b278_parse_u64(const char *sz, uint64_t *pOut)
{
	const char *p = sz;
	uint64_t uNum = 0ull;

	while (*p != '\0') {
		unsigned uDigit = (unsigned)(*p - '0');

		if (uNum > (B278_U64_MAX - (uint64_t)uDigit) / 10ull) {
			return -1;
		}
		uNum = uNum * 10ull + (uint64_t)uDigit;
		p++;
	}
	*pOut = uNum;
	return 0;
}

/* ---- public: gj_duration_parse_sec ------------------------------------- */

/*
 * Parse duration text into whole seconds (unsigned u64).
 *
 * Forms:
 *   plain:  DIGITS               → seconds
 *   units:  (DIGITS UNIT)+       UNIT ∈ {h,m,s}, each unit at most once
 */
int
gj_duration_parse_sec(const char *s, uint64_t *out_sec)
{
	const char *p;
	uint64_t uTotal = 0ull;
	unsigned uSeen = 0u;
	int fAny = 0;

	if (s == NULL || out_sec == NULL) {
		return -1;
	}
	if (*s == '\0') {
		return -1;
	}

	/* Plain seconds: entire string is decimal digits. */
	if (b278_all_digits(s)) {
		return b278_parse_u64(s, out_sec);
	}

	/* Unit form: one or more digit+unit pairs. */
	p = s;
	while (*p != '\0') {
		uint64_t uNum = 0ull;
		uint64_t uMul;
		uint64_t uPart;
		uint64_t uNext;
		unsigned char ch;
		unsigned uBit;
		int fDig = 0;

		if (!b278_is_digit((unsigned char)*p)) {
			return -1;
		}
		while (b278_is_digit((unsigned char)*p)) {
			unsigned uDigit = (unsigned)(*p - '0');

			if (uNum > (B278_U64_MAX - (uint64_t)uDigit) / 10ull) {
				return -1;
			}
			uNum = uNum * 10ull + (uint64_t)uDigit;
			p++;
			fDig = 1;
		}
		if (!fDig) {
			return -1;
		}

		ch = (unsigned char)*p;
		uMul = b278_unit_secs(ch);
		if (uMul == 0ull) {
			return -1;
		}
		uBit = b278_unit_bit(ch);
		if ((uSeen & uBit) != 0u) {
			return -1; /* duplicate unit */
		}
		uSeen |= uBit;
		p++;

		if (b278_mul_ok(uNum, uMul, &uPart) != 0) {
			return -1;
		}
		if (b278_add_ok(uTotal, uPart, &uNext) != 0) {
			return -1;
		}
		uTotal = uNext;
		fAny = 1;
	}

	if (!fAny) {
		return -1;
	}

	*out_sec = uTotal;
	return 0;
}

int __gj_duration_parse_sec(const char *s, uint64_t *out_sec)
    __attribute__((alias("gj_duration_parse_sec")));
