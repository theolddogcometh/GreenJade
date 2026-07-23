/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch147: ISO-8601 basic datetime (UTC).
 * Clean-room freestanding pure C — integer/pointer only (builds with
 * -msse2). No third-party date/time library source was copied.
 *
 * Surface:
 *   int gj_iso8601_parse(const char *s, int64_t *unix_sec);
 *     Parse one ISO-8601 UTC datetime into Unix seconds since 1970-01-01.
 *     Accepts exactly "YYYY-MM-DDTHH:MM:SSZ". Returns 0 on success, -1
 *     on error (errno set).
 *   int gj_iso8601_format(int64_t unix_sec, char *out, size_t cap);
 *     Format UTC Unix seconds as "YYYY-MM-DDTHH:MM:SSZ" (20 bytes + NUL).
 *     Requires cap >= 21. Returns 0 on success, -1 on error.
 *   __gj_iso8601_parse / __gj_iso8601_format  (aliases)
 *   __libcgj_batch147_marker = "libcgj-batch147"
 *
 * Year range: 1970..9999 (pre-epoch and 5+ digit years rejected).
 * Second may be 60 (leap second) on parse; format always emits 00..59.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch147_marker[] = "libcgj-batch147";

/* Wire length excluding trailing NUL: "YYYY-MM-DDTHH:MM:SSZ". */
#define B147_ISO_LEN 20u
/* Buffer capacity including NUL. */
#define B147_ISO_CAP 21u

/* ---- freestanding tables ----------------------------------------------- */

static const int s_aB147Dim[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/* ---- freestanding helpers ---------------------------------------------- */

static int
b147_is_digit(unsigned char ch)
{
	return ch >= '0' && ch <= '9';
}

static int
b147_is_leap(int nY)
{
	return ((nY % 4 == 0) && (nY % 100 != 0)) || (nY % 400 == 0);
}

static int
b147_dim(int nY, int nMon0)
{
	if (nMon0 == 1 && b147_is_leap(nY)) {
		return 29;
	}
	if (nMon0 < 0 || nMon0 > 11) {
		return 0;
	}
	return s_aB147Dim[nMon0];
}

/*
 * Parse exactly two ASCII digits at pSz into *pOut (0..99).
 * Returns 0 on success, -1 if not digits.
 */
static int
b147_parse_2(const char *pSz, int *pOut)
{
	if (!b147_is_digit((unsigned char)pSz[0]) ||
	    !b147_is_digit((unsigned char)pSz[1])) {
		return -1;
	}
	*pOut = (pSz[0] - '0') * 10 + (pSz[1] - '0');
	return 0;
}

/*
 * Parse exactly four ASCII digits at pSz into *pOut (0..9999).
 */
static int
b147_parse_4(const char *pSz, int *pOut)
{
	if (!b147_is_digit((unsigned char)pSz[0]) ||
	    !b147_is_digit((unsigned char)pSz[1]) ||
	    !b147_is_digit((unsigned char)pSz[2]) ||
	    !b147_is_digit((unsigned char)pSz[3])) {
		return -1;
	}
	*pOut = (pSz[0] - '0') * 1000 + (pSz[1] - '0') * 100 +
	        (pSz[2] - '0') * 10 + (pSz[3] - '0');
	return 0;
}

static void
b147_put_2(char *pOut, int nV)
{
	pOut[0] = (char)('0' + (nV / 10) % 10);
	pOut[1] = (char)('0' + nV % 10);
}

static void
b147_put_4(char *pOut, int nV)
{
	pOut[0] = (char)('0' + (nV / 1000) % 10);
	pOut[1] = (char)('0' + (nV / 100) % 10);
	pOut[2] = (char)('0' + (nV / 10) % 10);
	pOut[3] = (char)('0' + nV % 10);
}

/*
 * Convert civil UTC date/time to Unix seconds (int64).
 * nMon0 is 0..11. nSec may be 0..60 (leap second tolerated).
 * Year range: 1970..9999. Returns 0 and stores *pOut, or -1 with errno.
 */
static int
b147_civil_to_unix(int nY, int nMon0, int nMday, int nHour, int nMin,
                   int nSec, int64_t *pOut)
{
	int64_t i64Days = 0;
	int nYWalk;
	int nMon;
	int nDim;
	int nDiy;

	if (pOut == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (nY < 1970 || nY > 9999 || nMon0 < 0 || nMon0 > 11 || nMday < 1 ||
	    nHour < 0 || nHour > 23 || nMin < 0 || nMin > 59 || nSec < 0 ||
	    nSec > 60) {
		errno = EINVAL;
		return -1;
	}
	nDim = b147_dim(nY, nMon0);
	if (nMday > nDim) {
		errno = EINVAL;
		return -1;
	}

	for (nYWalk = 1970; nYWalk < nY; nYWalk++) {
		nDiy = b147_is_leap(nYWalk) ? 366 : 365;
		i64Days += (int64_t)nDiy;
	}
	for (nMon = 0; nMon < nMon0; nMon++) {
		i64Days += (int64_t)b147_dim(nY, nMon);
	}
	i64Days += (int64_t)(nMday - 1);

	*pOut = i64Days * 86400ll + (int64_t)nHour * 3600ll +
	        (int64_t)nMin * 60ll + (int64_t)nSec;
	return 0;
}

/*
 * Convert Unix seconds to civil UTC fields.
 * Rejects negative times and years past 9999.
 * On success: *pY, *pMon0 (0..11), *pMday, *pHour, *pMin, *pSec.
 * Returns 0 or -1 with errno.
 */
static int
b147_unix_to_civil(int64_t tSec, int *pY, int *pMon0, int *pMday, int *pHour,
                   int *pMin, int *pSec)
{
	int64_t i64Days;
	int64_t i64Rem;
	int nY;
	int nYday;
	int nMon;
	int nDim;
	int nDiy;

	if (pY == NULL || pMon0 == NULL || pMday == NULL || pHour == NULL ||
	    pMin == NULL || pSec == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (tSec < 0) {
		errno = ERANGE;
		return -1;
	}

	i64Days = tSec / 86400ll;
	i64Rem = tSec % 86400ll;
	*pHour = (int)(i64Rem / 3600ll);
	i64Rem %= 3600ll;
	*pMin = (int)(i64Rem / 60ll);
	*pSec = (int)(i64Rem % 60ll);

	nY = 1970;
	for (;;) {
		nDiy = b147_is_leap(nY) ? 366 : 365;
		if (i64Days < (int64_t)nDiy) {
			break;
		}
		i64Days -= (int64_t)nDiy;
		nY++;
		if (nY > 9999) {
			errno = ERANGE;
			return -1;
		}
	}
	nYday = (int)i64Days;
	for (nMon = 0; nMon < 12; nMon++) {
		nDim = b147_dim(nY, nMon);
		if (nYday < nDim) {
			break;
		}
		nYday -= nDim;
	}
	if (nMon >= 12) {
		errno = ERANGE;
		return -1;
	}

	*pY = nY;
	*pMon0 = nMon;
	*pMday = nYday + 1;
	return 0;
}

/* ---- public: gj_iso8601_parse ------------------------------------------ */

/*
 * Parse ISO-8601 UTC datetime "YYYY-MM-DDTHH:MM:SSZ" only.
 * Layout (fixed positions):
 *   0..3   year (4DIGIT)
 *   4      '-'
 *   5..6   month (01..12)
 *   7      '-'
 *   8..9   day (01..31, calendar-checked)
 *   10     'T'
 *   11..12 hour (00..23)
 *   13     ':'
 *   14..15 minute (00..59)
 *   16     ':'
 *   17..18 second (00..60; leap second tolerated)
 *   19     'Z'
 * Trailing HT/SP after the 20-byte field is tolerated; anything else is
 * EINVAL. Offset forms (+00:00) and fractional seconds are not accepted.
 */
int
gj_iso8601_parse(const char *sz, int64_t *pUnixSec)
{
	const char *pSz;
	int nY = 0;
	int nMon = 0;
	int nMday = 0;
	int nHour = 0;
	int nMin = 0;
	int nSec = 0;
	int i;

	if (sz == NULL || pUnixSec == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* Need the full 20-byte field. */
	for (i = 0; i < (int)B147_ISO_LEN; i++) {
		if (sz[i] == '\0') {
			errno = EINVAL;
			return -1;
		}
	}

	pSz = sz;

	if (b147_parse_4(pSz, &nY) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 4;
	if (pSz[0] != '-') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	if (b147_parse_2(pSz, &nMon) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;
	if (pSz[0] != '-') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	if (b147_parse_2(pSz, &nMday) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;

	if (pSz[0] != 'T') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	if (b147_parse_2(pSz, &nHour) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;
	if (pSz[0] != ':') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	if (b147_parse_2(pSz, &nMin) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;
	if (pSz[0] != ':') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	if (b147_parse_2(pSz, &nSec) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;

	if (pSz[0] != 'Z') {
		errno = EINVAL;
		return -1;
	}
	pSz++; /* now at offset B147_ISO_LEN */

	/* Tolerate trailing HT/SP only. */
	while (*pSz == ' ' || *pSz == '\t') {
		pSz++;
	}
	if (*pSz != '\0') {
		errno = EINVAL;
		return -1;
	}

	/* nMon is 1..12 wire form; civil helper wants 0..11. */
	return b147_civil_to_unix(nY, nMon - 1, nMday, nHour, nMin, nSec,
	                          pUnixSec);
}

/* ---- public: gj_iso8601_format ----------------------------------------- */

/*
 * Format tSec (UTC Unix seconds) as "YYYY-MM-DDTHH:MM:SSZ" into szOut.
 * Always writes exactly 20 printable chars + NUL when successful.
 */
int
gj_iso8601_format(int64_t i64UnixSec, char *szOut, size_t cbOut)
{
	int nY = 0;
	int nMon0 = 0;
	int nMday = 0;
	int nHour = 0;
	int nMin = 0;
	int nSec = 0;

	if (szOut == NULL || cbOut < B147_ISO_CAP) {
		errno = (szOut == NULL || cbOut == 0u) ? EINVAL : ENOSPC;
		return -1;
	}

	if (b147_unix_to_civil(i64UnixSec, &nY, &nMon0, &nMday, &nHour, &nMin,
	                       &nSec) != 0) {
		return -1;
	}

	/* "YYYY-MM-DDTHH:MM:SSZ" */
	b147_put_4(szOut + 0, nY);
	szOut[4] = '-';
	b147_put_2(szOut + 5, nMon0 + 1);
	szOut[7] = '-';
	b147_put_2(szOut + 8, nMday);
	szOut[10] = 'T';
	b147_put_2(szOut + 11, nHour);
	szOut[13] = ':';
	b147_put_2(szOut + 14, nMin);
	szOut[16] = ':';
	b147_put_2(szOut + 17, nSec);
	szOut[19] = 'Z';
	szOut[20] = '\0';
	return 0;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_iso8601_parse(const char *sz, int64_t *pUnixSec)
    __attribute__((alias("gj_iso8601_parse")));

int __gj_iso8601_format(int64_t i64UnixSec, char *szOut, size_t cbOut)
    __attribute__((alias("gj_iso8601_format")));
