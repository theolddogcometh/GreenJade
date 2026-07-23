/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * crypt / getpass — freestanding fill-in.
 * Traditional 13-char DES-shaped password hash (clean-room soft DES
 * replacement using MD5 compression of key+salt). Not FIPS/OpenSSL.
 * Sufficient for link + password-check soft path; upgrade to real DES
 * crypt or $6$ sha512crypt later if needed.
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

static const char g_aIto64[] =
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static int
is_salt_char(char c)
{
	return (c >= '.' && c <= '9') || (c >= 'A' && c <= 'Z') ||
	       (c >= 'a' && c <= 'z');
}

static void
to64(char *s, unsigned long v, int n)
{
	while (--n >= 0) {
		*s++ = g_aIto64[v & 0x3f];
		v >>= 6;
	}
}

/*
 * Produce traditional 13-char crypt(3) string: 2 salt + 11 hash.
 * Uses 25 rounds of MD5(key||salt||prev) folded into 66 bits → 11×6.
 */
static char *
crypt_trad(const char *szKey, const char *szSalt, char *szOut, size_t cbOut)
{
	unsigned char aCtx[128];
	unsigned char aDig[16];
	unsigned char aAcc[16];
	char aSalt[3];
	int i;
	int r;
	unsigned long v;
	size_t cbKey;

	if (szKey == NULL || szSalt == NULL || szOut == NULL || cbOut < 14) {
		errno = EINVAL;
		return NULL;
	}
	aSalt[0] = is_salt_char(szSalt[0]) ? szSalt[0] : '.';
	aSalt[1] = (szSalt[0] != '\0' && is_salt_char(szSalt[1])) ? szSalt[1]
								   : '.';
	aSalt[2] = '\0';
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
	v = ((unsigned long)aAcc[0] << 16) | ((unsigned long)aAcc[1] << 8) |
	    (unsigned long)aAcc[2];
	to64(szOut + 2, v, 4);
	v = ((unsigned long)aAcc[3] << 16) | ((unsigned long)aAcc[4] << 8) |
	    (unsigned long)aAcc[5];
	to64(szOut + 6, v, 4);
	v = ((unsigned long)aAcc[6] << 16) | ((unsigned long)aAcc[7] << 8) |
	    (unsigned long)aAcc[8];
	to64(szOut + 10, v, 3);
	szOut[13] = '\0';
	errno = 0;
	return szOut;
}

static char g_szCryptBuf[128];

char *
crypt(const char *szKey, const char *szSalt)
{
	return crypt_trad(szKey, szSalt, g_szCryptBuf, sizeof(g_szCryptBuf));
}

char *
crypt_r(const char *szKey, const char *szSalt, void *pData)
{
	/* glibc crypt_data is large; we only need 14+ bytes of output space. */
	char *pOut = (char *)pData;

	if (pOut == NULL) {
		errno = EINVAL;
		return NULL;
	}
	return crypt_trad(szKey, szSalt, pOut, 128);
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
