/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch148: human duration parse / format (1h2m3s).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party duration source was copied.
 *
 * Surface (unique symbols):
 *   int gj_duration_parse(const char *s, int64_t *secs);
 *     Parse a compact duration string into whole seconds.
 *     Grammar:  [ "-" ] ( DIGITS UNIT )+   |  "0"
 *     UNIT is one of: d (day=86400), h (hour=3600), m (min=60), s (sec=1).
 *     Components may appear in any order; each unit letter at most once.
 *     DIGITS is one or more ASCII decimal digits (no leading '+' / frac).
 *     Returns 0 on success, -1 on error (NULL args, empty, bad unit,
 *     duplicate unit, overflow, trailing junk).
 *
 *   int gj_duration_format(int64_t secs, char *out, size_t cap);
 *     Format whole seconds as a compact duration (no zero components).
 *     Zero → "0s". Negative values get a leading '-'. Prefer largest
 *     units first: d, h, m, s. Requires out != NULL and cap large enough
 *     for the result including NUL. Returns 0 ok, -1 error.
 *
 *   __gj_duration_parse / __gj_duration_format  (aliases)
 *   __libcgj_batch148_marker = "libcgj-batch148"
 *
 * Examples:
 *   "1h2m3s"  →  3723
 *   "2d"      →  172800
 *   "-45s"    →  -45
 *    3723     →  "1h2m3s"
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch148_marker[] = "libcgj-batch148";

/* Seconds per unit. */
#define B148_SEC_D 86400ll
#define B148_SEC_H  3600ll
#define B148_SEC_M    60ll
#define B148_SEC_S     1ll

/* Max magnitude for int64_t (2^63 - 1). */
#define B148_I64_MAX 0x7fffffffffffffffLL

/* ---- freestanding helpers ---------------------------------------------- */

static int
b148_is_digit(unsigned char ch)
{
	return ch >= '0' && ch <= '9';
}

/*
 * Unit letter → seconds multiplier. Returns 0 if not a known unit.
 */
static int64_t
b148_unit_secs(unsigned char ch)
{
	if (ch == 'd') {
		return B148_SEC_D;
	}
	if (ch == 'h') {
		return B148_SEC_H;
	}
	if (ch == 'm') {
		return B148_SEC_M;
	}
	if (ch == 's') {
		return B148_SEC_S;
	}
	return 0ll;
}

/*
 * Unit bit for duplicate detection: d=1, h=2, m=4, s=8.
 */
static unsigned
b148_unit_bit(unsigned char ch)
{
	if (ch == 'd') {
		return 1u;
	}
	if (ch == 'h') {
		return 2u;
	}
	if (ch == 'm') {
		return 4u;
	}
	if (ch == 's') {
		return 8u;
	}
	return 0u;
}

/*
 * Multiply uVal * uMul into *pOut with overflow check against B148_I64_MAX.
 * Returns 0 ok, -1 overflow. Both factors are non-negative.
 */
static int
b148_mul_ok(int64_t uVal, int64_t uMul, int64_t *pOut)
{
	if (uVal == 0ll || uMul == 0ll) {
		*pOut = 0ll;
		return 0;
	}
	if (uVal > B148_I64_MAX / uMul) {
		return -1;
	}
	*pOut = uVal * uMul;
	return 0;
}

/*
 * Add two non-negative int64 values with overflow check.
 */
static int
b148_add_ok(int64_t uA, int64_t uB, int64_t *pOut)
{
	if (uA > B148_I64_MAX - uB) {
		return -1;
	}
	*pOut = uA + uB;
	return 0;
}

/*
 * Write unsigned decimal digits of uVal into out[pos..), advance *pPos.
 * Returns 0 ok, -1 if out of room for digits (excluding final NUL).
 */
static int
b148_put_u(char *out, size_t cap, size_t *pPos, int64_t uVal)
{
	char aRev[24];
	size_t n = 0u;
	size_t i;
	size_t pos;

	if (uVal < 0ll) {
		return -1;
	}
	if (uVal == 0ll) {
		aRev[n++] = '0';
	} else {
		while (uVal > 0ll) {
			aRev[n++] = (char)('0' + (int)(uVal % 10ll));
			uVal /= 10ll;
		}
	}

	pos = *pPos;
	if (pos + n >= cap) {
		return -1;
	}
	for (i = 0u; i < n; i++) {
		out[pos + i] = aRev[n - 1u - i];
	}
	*pPos = pos + n;
	return 0;
}

/*
 * Append one char; leave room for a later NUL (need pos+1 < cap so
 * pos+2 slots: char + NUL). Returns 0 ok, -1 ENOSPC-style.
 */
static int
b148_put_ch(char *out, size_t cap, size_t *pPos, char ch)
{
	size_t pos = *pPos;

	if (pos + 1u >= cap) {
		return -1;
	}
	out[pos] = ch;
	*pPos = pos + 1u;
	return 0;
}

/*
 * Emit "NUM UNIT" when num > 0. Returns 0 ok, -1 no space.
 */
static int
b148_emit(char *out, size_t cap, size_t *pPos, int64_t num, char unit)
{
	if (num <= 0ll) {
		return 0;
	}
	if (b148_put_u(out, cap, pPos, num) != 0) {
		return -1;
	}
	if (b148_put_ch(out, cap, pPos, unit) != 0) {
		return -1;
	}
	return 0;
}

/* ---- public: gj_duration_parse ----------------------------------------- */

/*
 * Parse compact duration text into whole seconds.
 * Accepts optional leading '-', then one or more digit+unit pairs, or
 * a lone "0" (with optional '-'). Units: d/h/m/s; each at most once.
 */
int
gj_duration_parse(const char *s, int64_t *secs)
{
	const char *p;
	int fNeg = 0;
	int64_t uTotal = 0ll;
	unsigned uSeen = 0u;
	int fAny = 0;

	if (s == NULL || secs == NULL) {
		return -1;
	}

	p = s;
	if (*p == '-') {
		fNeg = 1;
		p++;
	}

	/* Lone "0" / "-0" is zero seconds. */
	if (p[0] == '0' && p[1] == '\0') {
		*secs = 0ll;
		return 0;
	}

	if (*p == '\0') {
		return -1;
	}

	while (*p != '\0') {
		int64_t uNum = 0ll;
		int64_t uMul;
		int64_t uPart;
		int64_t uNext;
		unsigned char ch;
		unsigned uBit;
		int fDig = 0;

		/* At least one digit required before a unit. */
		if (!b148_is_digit((unsigned char)*p)) {
			return -1;
		}
		while (b148_is_digit((unsigned char)*p)) {
			int d = (int)(*p - '0');

			if (uNum > (B148_I64_MAX - (int64_t)d) / 10ll) {
				return -1;
			}
			uNum = uNum * 10ll + (int64_t)d;
			p++;
			fDig = 1;
		}
		if (!fDig) {
			return -1;
		}

		ch = (unsigned char)*p;
		uMul = b148_unit_secs(ch);
		if (uMul == 0ll) {
			return -1;
		}
		uBit = b148_unit_bit(ch);
		if ((uSeen & uBit) != 0u) {
			return -1; /* duplicate unit */
		}
		uSeen |= uBit;
		p++;

		if (b148_mul_ok(uNum, uMul, &uPart) != 0) {
			return -1;
		}
		if (b148_add_ok(uTotal, uPart, &uNext) != 0) {
			return -1;
		}
		uTotal = uNext;
		fAny = 1;
	}

	if (!fAny) {
		return -1;
	}

	/* Negating INT64_MIN is undefined; our total is in [0, MAX]. */
	if (fNeg) {
		*secs = -uTotal;
	} else {
		*secs = uTotal;
	}
	return 0;
}

int __gj_duration_parse(const char *s, int64_t *secs)
    __attribute__((alias("gj_duration_parse")));

/* ---- public: gj_duration_format ---------------------------------------- */

/*
 * Format secs as d/h/m/s compact form. Zero components are omitted
 * except for zero itself ("0s"). Negative values get a leading '-'.
 */
int
gj_duration_format(int64_t secs, char *out, size_t cap)
{
	int64_t u;
	int64_t nD;
	int64_t nH;
	int64_t nM;
	int64_t nS;
	size_t pos = 0u;

	if (out == NULL || cap == 0u) {
		return -1;
	}

	if (secs == 0ll) {
		if (cap < 3u) {
			return -1;
		}
		out[0] = '0';
		out[1] = 's';
		out[2] = '\0';
		return 0;
	}

	if (secs < 0ll) {
		/*
		 * INT64_MIN magnitude cannot be represented as positive
		 * int64_t; reject rather than invoke undefined negation.
		 */
		if (secs == (int64_t)((uint64_t)1ull << 63)) {
			return -1;
		}
		if (b148_put_ch(out, cap, &pos, '-') != 0) {
			return -1;
		}
		u = -secs;
	} else {
		u = secs;
	}

	nD = u / B148_SEC_D;
	u %= B148_SEC_D;
	nH = u / B148_SEC_H;
	u %= B148_SEC_H;
	nM = u / B148_SEC_M;
	nS = u % B148_SEC_M;

	if (b148_emit(out, cap, &pos, nD, 'd') != 0 ||
	    b148_emit(out, cap, &pos, nH, 'h') != 0 ||
	    b148_emit(out, cap, &pos, nM, 'm') != 0 ||
	    b148_emit(out, cap, &pos, nS, 's') != 0) {
		return -1;
	}

	if (pos >= cap) {
		return -1;
	}
	out[pos] = '\0';
	return 0;
}

int __gj_duration_format(int64_t secs, char *out, size_t cap)
    __attribute__((alias("gj_duration_format")));
