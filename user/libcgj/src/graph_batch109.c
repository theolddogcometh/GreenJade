/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch109: missing BSD string helpers.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * No third-party BSD source was copied.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   strlcpy / strlcat / strnlen / strsep / memmem  → string.c
 *   __memmem / __strsep                             → graph_batch20/21
 *
 * This TU adds only missing symbols:
 *   char *stresep(char **stringp, const char *delim, int esc);
 *     OpenBSD-shaped: like strsep, but esc (when non-zero) escapes the
 *     next character so delim/esc may appear inside a token. The escape
 *     byte is removed from the token.
 *   int strpct(char *buf, size_t bufsiz, uintmax_t num, uintmax_t den,
 *              size_t precision);
 *     NetBSD-shaped: write "N.M%" (unsigned ratio * 100) into buf.
 *   int strspct(char *buf, size_t bufsiz, intmax_t num, intmax_t den,
 *               size_t precision);
 *     NetBSD-shaped: signed variant of strpct (leading '-' when negative).
 *   long long strsuftoll(const char *desc, const char *val,
 *                        long long min, long long max);
 *     NetBSD-shaped: parse integer with optional magnitude suffix
 *     (b/k/m/g/t/p/e and KiB-style binary via trailing 'i' or uppercase
 *     K/M/G…). Library form sets errno and returns 0 on error (no errx).
 *   __stresep / __strpct / __strspct / __strsuftoll  (aliases)
 *   __libcgj_batch109_marker = "libcgj-batch109"
 */

#include <errno.h>
#include <stddef.h>
#include <stdint.h>

/* Freestanding limits (avoid host limits.h dependency). */
#ifndef LLONG_MAX
#define LLONG_MAX  0x7fffffffffffffffLL
#endif
#ifndef LLONG_MIN
#define LLONG_MIN  (-LLONG_MAX - 1LL)
#endif
#ifndef INTMAX_MAX
#define INTMAX_MAX  ((intmax_t)LLONG_MAX)
#endif
#ifndef INTMAX_MIN
#define INTMAX_MIN  ((intmax_t)LLONG_MIN)
#endif
#ifndef UINTMAX_MAX
#define UINTMAX_MAX ((uintmax_t)-1)
#endif

const char __libcgj_batch109_marker[] = "libcgj-batch109";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b109_is_delim(unsigned char ch, const char *szDelim)
{
	const unsigned char *p;

	if (szDelim == NULL) {
		return 0;
	}
	p = (const unsigned char *)szDelim;
	while (*p != '\0') {
		if (*p == ch) {
			return 1;
		}
		p++;
	}
	return 0;
}

static int
b109_is_digit(unsigned char ch)
{
	return ch >= '0' && ch <= '9';
}

static int
b109_is_space(unsigned char ch)
{
	return ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' ||
	       ch == '\f' || ch == '\v';
}

/*
 * Write unsigned decimal digits of uVal into pEnd-1, pEnd-2, … working
 * backwards. Returns pointer to first digit. pEnd points one past buffer
 * used region; caller must have room.
 */
static char *
b109_utoa_rev(char *pEnd, uintmax_t uVal)
{
	if (uVal == 0u) {
		*--pEnd = '0';
		return pEnd;
	}
	while (uVal > 0u) {
		*--pEnd = (char)('0' + (unsigned)(uVal % 10u));
		uVal /= 10u;
	}
	return pEnd;
}

/* Copy [pSrc, pSrcEnd) into szDst if it fits with NUL. Returns length or -1. */
static int
b109_copy_out(char *szDst, size_t cbDst, const char *pSrc, size_t cbSrc)
{
	size_t i;

	if (szDst == NULL || cbDst == 0u) {
		return -1;
	}
	if (cbSrc + 1u > cbDst) {
		return -1;
	}
	for (i = 0u; i < cbSrc; i++) {
		szDst[i] = pSrc[i];
	}
	szDst[cbSrc] = '\0';
	return (int)cbSrc;
}

/*
 * Format (num/den)*100 with `cPrec` digits after the decimal into szDst.
 * fNeg selects a leading '-'. Returns bytes written (excl. NUL) or -1.
 */
static int
b109_fmt_pct(char *szDst, size_t cbDst, uintmax_t uNum, uintmax_t uDen,
             size_t cPrec, int fNeg)
{
	char aTmp[80];
	char *p;
	char *pEnd;
	uintmax_t uWhole;
	uintmax_t uFrac;
	uintmax_t uScale;
	uintmax_t uRem;
	size_t i;
	size_t cb;

	if (szDst == NULL || cbDst == 0u) {
		errno = EINVAL;
		return -1;
	}
	if (uDen == 0u) {
		errno = EINVAL;
		return -1;
	}

	/* Cap precision so scale fits in 64-bit-ish uintmax workspace. */
	if (cPrec > 18u) {
		cPrec = 18u;
	}

	/*
	 * value = num * 100 / den
	 * With precision p: scaled = num * 100 * 10^p / den
	 * whole = scaled / 10^p, frac = scaled % 10^p
	 */
	uScale = 1u;
	for (i = 0u; i < cPrec; i++) {
		if (uScale > (uintmax_t)UINTMAX_MAX / 10u) {
			errno = ERANGE;
			return -1;
		}
		uScale *= 10u;
	}

	/* num * 100 * scale  — check overflow stepwise */
	if (uNum > 0u && uNum > (uintmax_t)UINTMAX_MAX / 100u) {
		errno = ERANGE;
		return -1;
	}
	{
		uintmax_t uN100 = uNum * 100u;

		if (uScale > 1u && uN100 > (uintmax_t)UINTMAX_MAX / uScale) {
			errno = ERANGE;
			return -1;
		}
		uN100 *= uScale;
		uWhole = uN100 / uDen;
		uRem = uN100 % uDen;
		/* Round half up */
		if (uRem * 2u >= uDen) {
			if (uWhole == (uintmax_t)UINTMAX_MAX) {
				errno = ERANGE;
				return -1;
			}
			uWhole++;
		}
	}

	if (cPrec == 0u) {
		uFrac = 0u;
	} else {
		uFrac = uWhole % uScale;
		uWhole = uWhole / uScale;
	}

	pEnd = aTmp + (sizeof(aTmp) - 1u);
	*pEnd = '\0';
	p = pEnd;

	/* optional fractional digits */
	if (cPrec > 0u) {
		for (i = 0u; i < cPrec; i++) {
			*--p = (char)('0' + (unsigned)(uFrac % 10u));
			uFrac /= 10u;
		}
		*--p = '.';
	}

	p = b109_utoa_rev(p, uWhole);

	/* trailing '%' */
	/* Build final: [sign] digits... % */
	{
		char aOut[96];
		size_t n = 0u;

		if (fNeg) {
			aOut[n++] = '-';
		}
		while (p < pEnd && n + 1u < sizeof(aOut)) {
			aOut[n++] = *p++;
		}
		if (n + 1u >= sizeof(aOut)) {
			errno = ERANGE;
			return -1;
		}
		aOut[n++] = '%';
		aOut[n] = '\0';
		cb = n;
		return b109_copy_out(szDst, cbDst, aOut, cb);
	}
}

/* ---- stresep (OpenBSD-shaped) ------------------------------------------ */

/*
 * Isolate the next token in *ppString separated by any char in szDelim.
 * When nEsc is non-zero, a char equal to nEsc escapes the following byte:
 * that byte is never a delimiter, and the escape itself is deleted from
 * the token (in-place compact). Returns the token start, or NULL if
 * *ppString is NULL. Empty fields are returned as zero-length strings.
 */
char *
stresep(char **ppString, const char *szDelim, int nEsc)
{
	char *pTok;
	char *pRd;
	char *pWr;
	unsigned char ch;
	unsigned char chEsc;
	int fTakeLiteral;

	if (ppString == NULL || *ppString == NULL) {
		return NULL;
	}

	pTok = *ppString;
	pRd = pTok;
	pWr = pTok;
	chEsc = (unsigned char)nEsc;
	fTakeLiteral = 0;

	for (;;) {
		ch = (unsigned char)*pRd;
		if (ch == '\0') {
			*pWr = '\0';
			*ppString = NULL;
			return pTok;
		}

		if (fTakeLiteral) {
			*pWr++ = (char)ch;
			pRd++;
			fTakeLiteral = 0;
			continue;
		}

		if (nEsc != 0 && ch == chEsc) {
			/*
			 * Escape: drop the esc byte; next byte (if any) is
			 * literal. At end-of-string a trailing esc is dropped.
			 */
			pRd++;
			if (*pRd == '\0') {
				*pWr = '\0';
				*ppString = NULL;
				return pTok;
			}
			fTakeLiteral = 1;
			continue;
		}

		if (b109_is_delim(ch, szDelim)) {
			*pWr = '\0';
			*ppString = pRd + 1;
			return pTok;
		}

		*pWr++ = (char)ch;
		pRd++;
	}
}

char *__stresep(char **ppString, const char *szDelim, int nEsc)
    __attribute__((alias("stresep")));

/* ---- strpct / strspct (NetBSD-shaped) ---------------------------------- */

int
strpct(char *szBuf, size_t cbBuf, uintmax_t uNum, uintmax_t uDen,
       size_t cPrec)
{
	return b109_fmt_pct(szBuf, cbBuf, uNum, uDen, cPrec, 0);
}

int
strspct(char *szBuf, size_t cbBuf, intmax_t iNum, intmax_t iDen, size_t cPrec)
{
	int fNeg = 0;
	uintmax_t uNum;
	uintmax_t uDen;

	if (iDen == 0) {
		errno = EINVAL;
		return -1;
	}

	/*
	 * Sign of the quotient: negative if exactly one of num/den is < 0.
	 * Magnitude via careful negation of INTMAX_MIN.
	 */
	if (iNum < 0) {
		fNeg = !fNeg;
		if (iNum == INTMAX_MIN) {
			uNum = (uintmax_t)INTMAX_MAX + 1u;
		} else {
			uNum = (uintmax_t)(-iNum);
		}
	} else {
		uNum = (uintmax_t)iNum;
	}

	if (iDen < 0) {
		fNeg = !fNeg;
		if (iDen == INTMAX_MIN) {
			uDen = (uintmax_t)INTMAX_MAX + 1u;
		} else {
			uDen = (uintmax_t)(-iDen);
		}
	} else {
		uDen = (uintmax_t)iDen;
	}

	/* -0.00% → 0.00% */
	if (uNum == 0u) {
		fNeg = 0;
	}

	return b109_fmt_pct(szBuf, cbBuf, uNum, uDen, cPrec, fNeg);
}

int __strpct(char *szBuf, size_t cbBuf, uintmax_t uNum, uintmax_t uDen,
             size_t cPrec) __attribute__((alias("strpct")));
int __strspct(char *szBuf, size_t cbBuf, intmax_t iNum, intmax_t iDen,
              size_t cPrec) __attribute__((alias("strspct")));

/* ---- strsuftoll (NetBSD-shaped, non-fatal) ----------------------------- */

/*
 * Multiply uAcc *= uMul with overflow detection. Returns 0 ok, -1 overflow.
 */
static int
b109_mul_ull(unsigned long long *pAcc, unsigned long long uMul)
{
	unsigned long long uA;

	if (pAcc == NULL) {
		return -1;
	}
	uA = *pAcc;
	if (uMul != 0ull && uA > ~0ull / uMul) {
		return -1;
	}
	*pAcc = uA * uMul;
	return 0;
}

/*
 * Parse decimal (or 0x/0b/0 octal) integer with optional suffix.
 * Suffixes (case-insensitive letter, optional trailing 'B'/'i'/'iB'):
 *   b=512, k=1024, m=1024^2, g=1024^3, t=1024^4, p=1024^5, e=1024^6
 *   w=sizeof(int)  (treated as 4 for freestanding portability)
 * Plain trailing digits already consumed; suffix is the remainder.
 */
static int
b109_suffix_mul(const char *szSuf, unsigned long long *pMul)
{
	unsigned char ch;
	unsigned long long uMul = 1ull;
	unsigned uPow = 0u;
	unsigned i;

	if (szSuf == NULL || *szSuf == '\0') {
		*pMul = 1ull;
		return 0;
	}

	ch = (unsigned char)*szSuf;
	/* accept one magnitude letter */
	if (ch >= 'A' && ch <= 'Z') {
		ch = (unsigned char)(ch - 'A' + 'a');
	}

	switch (ch) {
	case 'b':
		uMul = 512ull;
		szSuf++;
		break;
	case 'k':
		uPow = 1u;
		szSuf++;
		break;
	case 'm':
		uPow = 2u;
		szSuf++;
		break;
	case 'g':
		uPow = 3u;
		szSuf++;
		break;
	case 't':
		uPow = 4u;
		szSuf++;
		break;
	case 'p':
		uPow = 5u;
		szSuf++;
		break;
	case 'e':
		uPow = 6u;
		szSuf++;
		break;
	case 'w':
		uMul = 4ull;
		szSuf++;
		break;
	default:
		errno = EINVAL;
		return -1;
	}

	if (uPow > 0u) {
		uMul = 1ull;
		for (i = 0u; i < uPow; i++) {
			if (b109_mul_ull(&uMul, 1024ull) != 0) {
				errno = ERANGE;
				return -1;
			}
		}
	}

	/* optional "i", "B", "iB", "ib" noise after the letter */
	if (*szSuf == 'i' || *szSuf == 'I') {
		szSuf++;
	}
	if (*szSuf == 'b' || *szSuf == 'B') {
		szSuf++;
	}
	if (*szSuf != '\0') {
		errno = EINVAL;
		return -1;
	}

	*pMul = uMul;
	return 0;
}

/*
 * strsuftoll — parse val into [nMin, nMax].
 * `szDesc` is accepted for API shape (diagnostics in NetBSD errx path);
 * this freestanding port never prints; it sets errno and returns 0 on error.
 */
long long
strsuftoll(const char *szDesc, const char *szVal, long long nMin,
           long long nMax)
{
	const char *p;
	int fNeg = 0;
	int nBase = 10;
	unsigned long long uAcc = 0ull;
	unsigned long long uMul = 1ull;
	unsigned long long uLim;
	long long nOut;

	(void)szDesc;

	if (szVal == NULL) {
		errno = EINVAL;
		return 0;
	}
	if (nMin > nMax) {
		errno = EINVAL;
		return 0;
	}

	p = szVal;
	while (b109_is_space((unsigned char)*p)) {
		p++;
	}
	if (*p == '+' || *p == '-') {
		if (*p == '-') {
			fNeg = 1;
		}
		p++;
	}

	/* base prefixes */
	if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
		nBase = 16;
		p += 2;
	} else if (p[0] == '0' && (p[1] == 'b' || p[1] == 'B') &&
	           (p[2] == '0' || p[2] == '1')) {
		nBase = 2;
		p += 2;
	} else if (p[0] == '0' && b109_is_digit((unsigned char)p[1])) {
		nBase = 8;
		/* keep leading 0 as part of octal digits */
	}

	if (*p == '\0') {
		errno = EINVAL;
		return 0;
	}

	/* magnitude limit for unsigned accumulation */
	if (fNeg) {
		/* abs(LLONG_MIN) as ULL */
		uLim = (unsigned long long)LLONG_MAX + 1ull;
	} else {
		uLim = (unsigned long long)LLONG_MAX;
	}

	{
		int fGotDigit = 0;

		for (;;) {
			unsigned char ch = (unsigned char)*p;
			unsigned uDig;

			if (b109_is_digit(ch)) {
				uDig = (unsigned)(ch - '0');
			} else if (ch >= 'a' && ch <= 'f') {
				uDig = (unsigned)(ch - 'a' + 10);
			} else if (ch >= 'A' && ch <= 'F') {
				uDig = (unsigned)(ch - 'A' + 10);
			} else {
				break;
			}
			if (uDig >= (unsigned)nBase) {
				break;
			}
			fGotDigit = 1;
			if (uAcc > (uLim - (unsigned long long)uDig) /
			               (unsigned long long)nBase) {
				errno = ERANGE;
				return 0;
			}
			uAcc = uAcc * (unsigned long long)nBase +
			       (unsigned long long)uDig;
			p++;
		}

		if (!fGotDigit) {
			errno = EINVAL;
			return 0;
		}
	}

	/* suffix */
	if (*p != '\0') {
		if (b109_suffix_mul(p, &uMul) != 0) {
			return 0;
		}
		if (uMul != 1ull) {
			if (uAcc > uLim / uMul) {
				errno = ERANGE;
				return 0;
			}
			uAcc *= uMul;
		}
	}

	if (fNeg) {
		if (uAcc == (unsigned long long)LLONG_MAX + 1ull) {
			nOut = LLONG_MIN;
		} else {
			nOut = -(long long)uAcc;
		}
	} else {
		nOut = (long long)uAcc;
	}

	if (nOut < nMin || nOut > nMax) {
		errno = ERANGE;
		return 0;
	}

	return nOut;
}

long long __strsuftoll(const char *szDesc, const char *szVal, long long nMin,
                       long long nMax) __attribute__((alias("strsuftoll")));
