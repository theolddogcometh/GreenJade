/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * crypt / getpass — freestanding fill-in (soft deepen).
 *   - Traditional 13-char DES-shaped path (MD5 fold of key+salt).
 *   - Soft modular $1$ / $5$ / $6$ (iterated MD5/SHA256/SHA512).
 * Not FIPS/OpenSSL; deterministic soft password-check surface only.
 */
#include <crypt.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void MD5Init(void *pCtx);
void MD5Update(void *pCtx, const void *pData, size_t cb);
void MD5Final(unsigned char aDig[16], void *pCtx);
void SHA256_Init(void *pCtx);
void SHA256_Update(void *pCtx, const void *pData, size_t cb);
void SHA256_Final(unsigned char aDig[32], void *pCtx);
void SHA512_Init(void *pCtx);
void SHA512_Update(void *pCtx, const void *pData, size_t cb);
void SHA512_Final(unsigned char aDig[64], void *pCtx);

static const char g_aIto64[] =
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static int
is_salt_char(char ch)
{
	return (ch >= '.' && ch <= '9') || (ch >= 'A' && ch <= 'Z') ||
	       (ch >= 'a' && ch <= 'z');
}

static void
to64(char *sz, unsigned long uVal, int n)
{
	while (--n >= 0) {
		*sz++ = g_aIto64[uVal & 0x3f];
		uVal >>= 6;
	}
}

/* Encode cb bytes of digest into aOut using 6-bit groups (glibc-shaped). */
static size_t
b64_from_digest(char *szOut, size_t cbOut, const unsigned char *pDig,
		size_t cbDig)
{
	size_t i = 0;
	size_t o = 0;
	unsigned long uV;

	while (i < cbDig && o + 4 < cbOut) {
		uV = (unsigned long)pDig[i++];
		if (i < cbDig) {
			uV |= (unsigned long)pDig[i++] << 8;
		}
		if (i < cbDig) {
			uV |= (unsigned long)pDig[i++] << 16;
		}
		szOut[o++] = g_aIto64[uV & 0x3f];
		szOut[o++] = g_aIto64[(uV >> 6) & 0x3f];
		szOut[o++] = g_aIto64[(uV >> 12) & 0x3f];
		szOut[o++] = g_aIto64[(uV >> 18) & 0x3f];
	}
	if (o < cbOut) {
		szOut[o] = '\0';
	}
	return o;
}

/*
 * Parse modular salt: "$id$salt$..." or traditional 2-char.
 * Returns id 0=trad, 1, 5, 6; writes salt chars into aSalt (NUL-term).
 */
static int
parse_salt(const char *szSalt, char *aSalt, size_t cbSalt, size_t *pCbSalt)
{
	const char *p;
	size_t n = 0;
	int nId = 0;

	if (szSalt == NULL || aSalt == NULL || cbSalt < 2) {
		return -1;
	}
	if (szSalt[0] == '$' && szSalt[1] != '\0' && szSalt[2] == '$') {
		if (szSalt[1] == '1') {
			nId = 1;
		} else if (szSalt[1] == '5') {
			nId = 5;
		} else if (szSalt[1] == '6') {
			nId = 6;
		} else {
			/* Unknown modular id — soft reject */
			return -1;
		}
		p = szSalt + 3;
		while (*p != '\0' && *p != '$' && n + 1 < cbSalt && n < 16) {
			if (!is_salt_char(*p)) {
				break;
			}
			aSalt[n++] = *p++;
		}
		if (n == 0) {
			aSalt[0] = '.';
			aSalt[1] = '.';
			n = 2;
		}
		aSalt[n] = '\0';
		if (pCbSalt != NULL) {
			*pCbSalt = n;
		}
		return nId;
	}
	/* Traditional: first two salt chars */
	aSalt[0] = is_salt_char(szSalt[0]) ? szSalt[0] : '.';
	aSalt[1] = (szSalt[0] != '\0' && is_salt_char(szSalt[1])) ? szSalt[1]
								  : '.';
	aSalt[2] = '\0';
	if (pCbSalt != NULL) {
		*pCbSalt = 2;
	}
	return 0;
}

/*
 * Produce traditional 13-char crypt(3) string: 2 salt + 11 hash.
 * Uses 25 rounds of MD5(key||salt||prev) folded into 66 bits → 11×6.
 */
static char *
crypt_trad(const char *szKey, const char *aSalt, char *szOut, size_t cbOut)
{
	unsigned char aCtx[128];
	unsigned char aDig[16];
	unsigned char aAcc[16];
	int i;
	int r;
	unsigned long uV;
	size_t cbKey;

	if (szKey == NULL || aSalt == NULL || szOut == NULL || cbOut < 14) {
		errno = EINVAL;
		return NULL;
	}
	cbKey = strlen(szKey);
	memset(aAcc, 0, sizeof(aAcc));
	for (r = 0; r < 25; r++) {
		MD5Init(aCtx);
		MD5Update(aCtx, szKey, cbKey);
		MD5Update(aCtx, aSalt, 2);
		MD5Update(aCtx, aAcc, 16);
		MD5Final(aDig, aCtx);
		for (i = 0; i < 16; i++) {
			aAcc[i] ^= aDig[i];
		}
	}
	szOut[0] = aSalt[0];
	szOut[1] = aSalt[1];
	uV = ((unsigned long)aAcc[0] << 16) | ((unsigned long)aAcc[1] << 8) |
	     (unsigned long)aAcc[2];
	to64(szOut + 2, uV, 4);
	uV = ((unsigned long)aAcc[3] << 16) | ((unsigned long)aAcc[4] << 8) |
	     (unsigned long)aAcc[5];
	to64(szOut + 6, uV, 4);
	uV = ((unsigned long)aAcc[6] << 16) | ((unsigned long)aAcc[7] << 8) |
	     (unsigned long)aAcc[8];
	to64(szOut + 10, uV, 3);
	szOut[13] = '\0';
	errno = 0;
	return szOut;
}

/*
 * Soft modular $1$ / $5$ / $6$: iterated digest of key||salt||prev.
 * Not bit-identical to glibc crypt(3) modular; soft auth + link surface.
 */
static char *
crypt_mod(int nId, const char *szKey, const char *aSalt, size_t cbSalt,
	  char *szOut, size_t cbOut)
{
	unsigned char aCtx[256];
	unsigned char aDig[64];
	unsigned char aAcc[64];
	size_t cbDig;
	size_t cbKey;
	size_t o;
	int r;
	int nRounds;
	int i;
	char aPfx[4];

	if (szKey == NULL || aSalt == NULL || szOut == NULL) {
		errno = EINVAL;
		return NULL;
	}
	cbKey = strlen(szKey);
	if (nId == 1) {
		cbDig = 16;
		nRounds = 1000;
		aPfx[0] = '$';
		aPfx[1] = '1';
		aPfx[2] = '$';
		aPfx[3] = '\0';
	} else if (nId == 5) {
		cbDig = 32;
		nRounds = 5000;
		aPfx[0] = '$';
		aPfx[1] = '5';
		aPfx[2] = '$';
		aPfx[3] = '\0';
	} else if (nId == 6) {
		cbDig = 64;
		nRounds = 5000;
		aPfx[0] = '$';
		aPfx[1] = '6';
		aPfx[2] = '$';
		aPfx[3] = '\0';
	} else {
		errno = EINVAL;
		return NULL;
	}
	/* Need room: $id$ + salt + $ + ~86 b64 + NUL */
	if (cbOut < 4 + cbSalt + 1 + 88) {
		errno = ERANGE;
		return NULL;
	}
	memset(aAcc, 0, sizeof(aAcc));
	for (r = 0; r < nRounds; r++) {
		if (nId == 1) {
			MD5Init(aCtx);
			MD5Update(aCtx, szKey, cbKey);
			MD5Update(aCtx, aSalt, cbSalt);
			MD5Update(aCtx, aAcc, cbDig);
			MD5Final(aDig, aCtx);
		} else if (nId == 5) {
			SHA256_Init(aCtx);
			SHA256_Update(aCtx, szKey, cbKey);
			SHA256_Update(aCtx, aSalt, cbSalt);
			SHA256_Update(aCtx, aAcc, cbDig);
			SHA256_Final(aDig, aCtx);
		} else {
			SHA512_Init(aCtx);
			SHA512_Update(aCtx, szKey, cbKey);
			SHA512_Update(aCtx, aSalt, cbSalt);
			SHA512_Update(aCtx, aAcc, cbDig);
			SHA512_Final(aDig, aCtx);
		}
		for (i = 0; i < (int)cbDig; i++) {
			aAcc[i] ^= aDig[i];
		}
	}
	o = 0;
	szOut[o++] = aPfx[0];
	szOut[o++] = aPfx[1];
	szOut[o++] = aPfx[2];
	memcpy(szOut + o, aSalt, cbSalt);
	o += cbSalt;
	szOut[o++] = '$';
	(void)b64_from_digest(szOut + o, cbOut - o, aAcc, cbDig);
	errno = 0;
	return szOut;
}

static char *
crypt_dispatch(const char *szKey, const char *szSalt, char *szOut,
	       size_t cbOut)
{
	char aSalt[20];
	size_t cbSalt = 0;
	int nId;

	if (szKey == NULL || szSalt == NULL || szOut == NULL) {
		errno = EINVAL;
		return NULL;
	}
	nId = parse_salt(szSalt, aSalt, sizeof(aSalt), &cbSalt);
	if (nId < 0) {
		errno = EINVAL;
		return NULL;
	}
	if (nId == 0) {
		return crypt_trad(szKey, aSalt, szOut, cbOut);
	}
	return crypt_mod(nId, szKey, aSalt, cbSalt, szOut, cbOut);
}

static char g_szCryptBuf[256];

char *
crypt(const char *szKey, const char *szSalt)
{
	return crypt_dispatch(szKey, szSalt, g_szCryptBuf, sizeof(g_szCryptBuf));
}

char *
crypt_r(const char *szKey, const char *szSalt, void *pData)
{
	/* glibc crypt_data is large; we only need modular output space. */
	char *pOut = (char *)pData;

	if (pOut == NULL) {
		errno = EINVAL;
		return NULL;
	}
	return crypt_dispatch(szKey, szSalt, pOut, 256);
}

char *
getpass(const char *szPrompt)
{
	static char aBuf[128];
	size_t i;
	int ch;

	if (szPrompt != NULL) {
		(void)fputs(szPrompt, stderr);
		(void)fflush(stderr);
	}
	i = 0;
	for (;;) {
		ch = fgetc(stdin);
		if (ch == EOF || ch == '\n' || ch == '\r') {
			break;
		}
		if (i + 1 < sizeof(aBuf)) {
			aBuf[i++] = (char)ch;
		}
	}
	aBuf[i] = '\0';
	(void)fputc('\n', stderr);
	return aBuf;
}
