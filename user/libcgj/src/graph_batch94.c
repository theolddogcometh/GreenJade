/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch94: HTTP-date IMF-fixdate (RFC 7231 §7.1.1.1).
 * Clean-room freestanding pure C — integer/pointer only (no SSE, no libc
 * string/time). No third-party HTTP-date source was copied.
 *
 * Surface:
 *   gj_http_date_parse(const char *sz, int64_t *pOut)
 *     Parse one IMF-fixdate into UTC Unix seconds since 1970-01-01.
 *     Returns 0 on success, -1 on error (errno set).
 *   gj_http_date_format(int64_t tSec, char *szOut, size_t cbOut)
 *     Format UTC Unix seconds as IMF-fixdate (29 bytes + NUL).
 *     Requires cbOut >= 30. Returns 0 on success, -1 on error.
 *   __gj_http_date_parse / __gj_http_date_format  (aliases)
 *   __libcgj_batch94_marker = "libcgj-batch94"
 *
 * Grammar (IMF-fixdate only — obs-date not accepted):
 *   day-name "," SP date1 SP time-of-day SP GMT
 *   e.g.  Sun, 06 Nov 1994 08:49:37 GMT
 * day-name and month are case-sensitive fixed English abbreviations.
 * second may be 60 (leap second) on parse; format always emits 00..59.
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch94_marker[] = "libcgj-batch94";

/* Fixed IMF-fixdate wire length excluding trailing NUL. */
#define B94_IMF_LEN 29u
/* Buffer capacity including NUL. */
#define B94_IMF_CAP 30u

/* ---- freestanding tables ----------------------------------------------- */

static const char s_aB94Wday[7][4] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char s_aB94Mon[12][4] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static const int s_aB94Dim[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

/* ---- freestanding helpers ---------------------------------------------- */

static int
b94_is_digit(unsigned char ch)
{
	return ch >= '0' && ch <= '9';
}

static int
b94_eq3(const char *pSz, const char *szLit)
{
	return pSz[0] == szLit[0] && pSz[1] == szLit[1] && pSz[2] == szLit[2];
}

static int
b94_is_leap(int nY)
{
	return ((nY % 4 == 0) && (nY % 100 != 0)) || (nY % 400 == 0);
}

static int
b94_dim(int nY, int nMon0)
{
	if (nMon0 == 1 && b94_is_leap(nY)) {
		return 29;
	}
	if (nMon0 < 0 || nMon0 > 11) {
		return 0;
	}
	return s_aB94Dim[nMon0];
}

/*
 * Parse exactly two ASCII digits at pSz into *pOut (0..99).
 * Returns 0 on success, -1 if not digits.
 */
static int
b94_parse_2(const char *pSz, int *pOut)
{
	if (!b94_is_digit((unsigned char)pSz[0]) ||
	    !b94_is_digit((unsigned char)pSz[1])) {
		return -1;
	}
	*pOut = (pSz[0] - '0') * 10 + (pSz[1] - '0');
	return 0;
}

/*
 * Parse exactly four ASCII digits at pSz into *pOut (0..9999).
 */
static int
b94_parse_4(const char *pSz, int *pOut)
{
	if (!b94_is_digit((unsigned char)pSz[0]) ||
	    !b94_is_digit((unsigned char)pSz[1]) ||
	    !b94_is_digit((unsigned char)pSz[2]) ||
	    !b94_is_digit((unsigned char)pSz[3])) {
		return -1;
	}
	*pOut = (pSz[0] - '0') * 1000 + (pSz[1] - '0') * 100 +
	        (pSz[2] - '0') * 10 + (pSz[3] - '0');
	return 0;
}

static void
b94_put_2(char *pOut, int nV)
{
	pOut[0] = (char)('0' + (nV / 10) % 10);
	pOut[1] = (char)('0' + nV % 10);
}

static void
b94_put_4(char *pOut, int nV)
{
	pOut[0] = (char)('0' + (nV / 1000) % 10);
	pOut[1] = (char)('0' + (nV / 100) % 10);
	pOut[2] = (char)('0' + (nV / 10) % 10);
	pOut[3] = (char)('0' + nV % 10);
}

/*
 * Convert civil UTC date/time to Unix seconds (int64).
 * nMon0 is 0..11. nSec may be 0..60 (leap second tolerated).
 * Year range: 1970..9999 (IMF year is 4DIGIT; pre-epoch rejected).
 * Returns 0 and stores *pOut, or -1 with errno.
 */
static int
b94_civil_to_unix(int nY, int nMon0, int nMday, int nHour, int nMin,
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
	nDim = b94_dim(nY, nMon0);
	if (nMday > nDim) {
		errno = EINVAL;
		return -1;
	}

	for (nYWalk = 1970; nYWalk < nY; nYWalk++) {
		nDiy = b94_is_leap(nYWalk) ? 366 : 365;
		i64Days += (int64_t)nDiy;
	}
	for (nMon = 0; nMon < nMon0; nMon++) {
		i64Days += (int64_t)b94_dim(nY, nMon);
	}
	i64Days += (int64_t)(nMday - 1);

	*pOut = i64Days * 86400ll + (int64_t)nHour * 3600ll +
	        (int64_t)nMin * 60ll + (int64_t)nSec;
	return 0;
}

/*
 * Convert Unix seconds to civil UTC fields.
 * Rejects negative times and years past 9999.
 * On success: *pY, *pMon0 (0..11), *pMday, *pHour, *pMin, *pSec, *pWday
 * (0=Sun..6=Sat). Returns 0 or -1 with errno.
 */
static int
b94_unix_to_civil(int64_t tSec, int *pY, int *pMon0, int *pMday, int *pHour,
                  int *pMin, int *pSec, int *pWday)
{
	int64_t i64Days;
	int64_t i64Rem;
	int nY;
	int nYday;
	int nMon;
	int nDim;
	int nDiy;

	if (pY == NULL || pMon0 == NULL || pMday == NULL || pHour == NULL ||
	    pMin == NULL || pSec == NULL || pWday == NULL) {
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

	/* 1970-01-01 was Thursday → wday = (days + 4) % 7, Sun=0. */
	*pWday = (int)((i64Days + 4ll) % 7ll);
	if (*pWday < 0) {
		*pWday += 7;
	}

	nY = 1970;
	for (;;) {
		nDiy = b94_is_leap(nY) ? 366 : 365;
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
		nDim = b94_dim(nY, nMon);
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

/* ---- public: gj_http_date_parse ---------------------------------------- */

/*
 * Parse RFC 7231 IMF-fixdate only.
 * Layout (fixed positions):
 *   0..2   day-name
 *   3      ','
 *   4      ' '
 *   5..6   day (2DIGIT)
 *   7      ' '
 *   8..10  month
 *   11     ' '
 *   12..15 year (4DIGIT)
 *   16     ' '
 *   17..18 hour
 *   19     ':'
 *   20..21 minute
 *   22     ':'
 *   23..24 second
 *   25     ' '
 *   26..28 "GMT"
 * Trailing whitespace after the 29-byte field is tolerated; anything else
 * is EINVAL. Day-name is required to be a valid abbreviation; it is not
 * cross-checked against the calendar date (common HTTP practice).
 */
int
gj_http_date_parse(const char *sz, int64_t *pOut)
{
	const char *pSz;
	int nWday = -1;
	int nMon0 = -1;
	int nMday = 0;
	int nY = 0;
	int nHour = 0;
	int nMin = 0;
	int nSec = 0;
	int i;

	if (sz == NULL || pOut == NULL) {
		errno = EINVAL;
		return -1;
	}

	/* Need the full 29-byte field. */
	for (i = 0; i < (int)B94_IMF_LEN; i++) {
		if (sz[i] == '\0') {
			errno = EINVAL;
			return -1;
		}
	}

	pSz = sz;

	/* day-name */
	for (i = 0; i < 7; i++) {
		if (b94_eq3(pSz, s_aB94Wday[i])) {
			nWday = i;
			break;
		}
	}
	if (nWday < 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 3;

	if (pSz[0] != ',' || pSz[1] != ' ') {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;

	if (b94_parse_2(pSz, &nMday) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;

	if (pSz[0] != ' ') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	for (i = 0; i < 12; i++) {
		if (b94_eq3(pSz, s_aB94Mon[i])) {
			nMon0 = i;
			break;
		}
	}
	if (nMon0 < 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 3;

	if (pSz[0] != ' ') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	if (b94_parse_4(pSz, &nY) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 4;

	if (pSz[0] != ' ') {
		errno = EINVAL;
		return -1;
	}
	pSz++;

	if (b94_parse_2(pSz, &nHour) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;
	if (pSz[0] != ':') {
		errno = EINVAL;
		return -1;
	}
	pSz++;
	if (b94_parse_2(pSz, &nMin) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;
	if (pSz[0] != ':') {
		errno = EINVAL;
		return -1;
	}
	pSz++;
	if (b94_parse_2(pSz, &nSec) != 0) {
		errno = EINVAL;
		return -1;
	}
	pSz += 2;

	/* SP "GMT" at fixed offsets 25..28. */
	if (pSz[0] != ' ' || pSz[1] != 'G' || pSz[2] != 'M' ||
	    pSz[3] != 'T') {
		errno = EINVAL;
		return -1;
	}
	pSz += 4; /* now at offset B94_IMF_LEN */

	/* Tolerate trailing HT/SP only. */
	while (*pSz == ' ' || *pSz == '\t') {
		pSz++;
	}
	if (*pSz != '\0') {
		errno = EINVAL;
		return -1;
	}

	(void)nWday; /* accepted but not calendar-checked */
	return b94_civil_to_unix(nY, nMon0, nMday, nHour, nMin, nSec, pOut);
}

/* ---- public: gj_http_date_format --------------------------------------- */

/*
 * Format tSec (UTC Unix seconds) as IMF-fixdate into szOut.
 * Always writes exactly 29 printable chars + NUL when successful.
 */
int
gj_http_date_format(int64_t tSec, char *szOut, size_t cbOut)
{
	int nY = 0;
	int nMon0 = 0;
	int nMday = 0;
	int nHour = 0;
	int nMin = 0;
	int nSec = 0;
	int nWday = 0;
	int i;

	if (szOut == NULL || cbOut < B94_IMF_CAP) {
		errno = (szOut == NULL || cbOut == 0u) ? EINVAL : ENOSPC;
		return -1;
	}

	if (b94_unix_to_civil(tSec, &nY, &nMon0, &nMday, &nHour, &nMin, &nSec,
	                      &nWday) != 0) {
		return -1;
	}

	/* "Wdy, DD Mon YYYY HH:MM:SS GMT" */
	for (i = 0; i < 3; i++) {
		szOut[i] = s_aB94Wday[nWday][i];
	}
	szOut[3] = ',';
	szOut[4] = ' ';
	b94_put_2(szOut + 5, nMday);
	szOut[7] = ' ';
	for (i = 0; i < 3; i++) {
		szOut[8 + i] = s_aB94Mon[nMon0][i];
	}
	szOut[11] = ' ';
	b94_put_4(szOut + 12, nY);
	szOut[16] = ' ';
	b94_put_2(szOut + 17, nHour);
	szOut[19] = ':';
	b94_put_2(szOut + 20, nMin);
	szOut[22] = ':';
	b94_put_2(szOut + 23, nSec);
	szOut[25] = ' ';
	szOut[26] = 'G';
	szOut[27] = 'M';
	szOut[28] = 'T';
	szOut[29] = '\0';
	return 0;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_http_date_parse(const char *sz, int64_t *pOut)
    __attribute__((alias("gj_http_date_parse")));

int __gj_http_date_format(int64_t tSec, char *szOut, size_t cbOut)
    __attribute__((alias("gj_http_date_format")));
