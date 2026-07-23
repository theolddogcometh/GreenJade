/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch46: PBKDF2-HMAC-SHA512, RFC 7914 scrypt/scrypt_kdf
 * (working; memory-hard with small-N bring-up safe), bcrypt_pbkdf ENOSYS
 * surface, argon2id_hash_raw ENOSYS surface. Uses existing hmac_sha256 /
 * hmac_sha512 from libcgj (extern only; no SHA reimplementation).
 * Integer/pointer only (no SSE). Clean-room public ABI.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

/* Existing crypto surface (batch38/39/40) */
void hmac_sha256(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                 unsigned char aOut[32]);
void hmac_sha512(const void *pKey, size_t cbKey, const void *pData, size_t cbData,
                 unsigned char aOut[64]);

void *malloc(size_t cb);
void free(void *p);

const char __libcgj_batch46_marker[] = "libcgj-batch46";

/* Soft cap so scrypt cannot request unbounded RAM during bring-up. */
#define B46_SCRYPT_MAX_MEM (16u * 1024u * 1024u)

/* ---- helpers ------------------------------------------------------------ */

static void
b46_hmac_sha512_w(const void *pKey, size_t cbKey, const void *pData,
                  size_t cbData, unsigned char *pOut)
{
    hmac_sha512(pKey, cbKey, pData, cbData, pOut);
}

static void
b46_hmac_sha256_w(const void *pKey, size_t cbKey, const void *pData,
                  size_t cbData, unsigned char *pOut)
{
    hmac_sha256(pKey, cbKey, pData, cbData, pOut);
}

/*
 * Generic PBKDF2 (RFC 8018 / PKCS #5). Salt length is not capped to a small
 * stack buffer — scrypt reuses this with intermediate B of size p*128*r.
 */
static int
b46_pbkdf2(void (*pfnHmac)(const void *, size_t, const void *, size_t,
                           unsigned char *),
           size_t cbHmac, const void *pPass, size_t cbPass,
           const unsigned char *pSalt, size_t cbSalt, unsigned uRounds,
           unsigned char *pOut, size_t cbOut)
{
    unsigned char aU[64];
    unsigned char aT[64];
    unsigned char *pBlock;
    unsigned uBlk = 1;
    size_t nOff = 0;
    size_t cbBlock;

    if (pOut == NULL || cbOut == 0u || uRounds == 0u || cbHmac == 0u ||
        cbHmac > 64u) {
        errno = EINVAL;
        return -1;
    }
    if (pPass == NULL) {
        pPass = "";
        cbPass = 0;
    }
    if (pSalt == NULL) {
        pSalt = (const unsigned char *)"";
        cbSalt = 0;
    }
    if (cbSalt > (size_t)(SIZE_MAX - 4u)) {
        errno = EINVAL;
        return -1;
    }
    cbBlock = cbSalt + 4u;
    pBlock = (unsigned char *)malloc(cbBlock);
    if (pBlock == NULL) {
        errno = ENOMEM;
        return -1;
    }
    if (cbSalt > 0u) {
        memcpy(pBlock, pSalt, cbSalt);
    }

    while (nOff < cbOut) {
        size_t i;
        size_t cbCopy;
        unsigned r;

        pBlock[cbSalt] = (unsigned char)(uBlk >> 24);
        pBlock[cbSalt + 1u] = (unsigned char)(uBlk >> 16);
        pBlock[cbSalt + 2u] = (unsigned char)(uBlk >> 8);
        pBlock[cbSalt + 3u] = (unsigned char)uBlk;
        pfnHmac(pPass, cbPass, pBlock, cbBlock, aU);
        memcpy(aT, aU, cbHmac);
        for (r = 1; r < uRounds; r++) {
            pfnHmac(pPass, cbPass, aU, cbHmac, aU);
            for (i = 0; i < cbHmac; i++) {
                aT[i] ^= aU[i];
            }
        }
        cbCopy = (cbOut - nOff < cbHmac) ? (cbOut - nOff) : cbHmac;
        memcpy(pOut + nOff, aT, cbCopy);
        nOff += cbCopy;
        uBlk++;
    }

    memset(pBlock, 0, cbBlock);
    free(pBlock);
    memset(aU, 0, sizeof(aU));
    memset(aT, 0, sizeof(aT));
    return 0;
}

/* ---- PBKDF2-HMAC-SHA512 ------------------------------------------------- */

int
pbkdf2_hmac_sha512(const void *pPass, size_t cbPass, const unsigned char *pSalt,
                   size_t cbSalt, unsigned uRounds, unsigned char *pOut,
                   size_t cbOut)
{
    return b46_pbkdf2(b46_hmac_sha512_w, 64u, pPass, cbPass, pSalt, cbSalt,
                      uRounds, pOut, cbOut);
}

int __pbkdf2_hmac_sha512(const void *pPass, size_t cbPass,
                         const unsigned char *pSalt, size_t cbSalt,
                         unsigned uRounds, unsigned char *pOut, size_t cbOut)
    __attribute__((alias("pbkdf2_hmac_sha512")));

/* ---- scrypt (RFC 7914) -------------------------------------------------- */

static uint32_t
b46_load_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
           ((uint32_t)p[3] << 24);
}

static void
b46_store_le32(unsigned char *p, uint32_t x)
{
    p[0] = (unsigned char)x;
    p[1] = (unsigned char)(x >> 8);
    p[2] = (unsigned char)(x >> 16);
    p[3] = (unsigned char)(x >> 24);
}

static uint32_t
b46_rol32(uint32_t x, unsigned n)
{
    return (x << n) | (x >> (32u - n));
}

/* salsa20/8 core on a 64-byte block (RFC 7914 §5 / §3) */
static void
b46_salsa20_8(unsigned char aB[64])
{
    uint32_t x0, x1, x2, x3, x4, x5, x6, x7;
    uint32_t x8, x9, x10, x11, x12, x13, x14, x15;
    uint32_t j0, j1, j2, j3, j4, j5, j6, j7;
    uint32_t j8, j9, j10, j11, j12, j13, j14, j15;
    unsigned i;

    j0 = x0 = b46_load_le32(aB + 0);
    j1 = x1 = b46_load_le32(aB + 4);
    j2 = x2 = b46_load_le32(aB + 8);
    j3 = x3 = b46_load_le32(aB + 12);
    j4 = x4 = b46_load_le32(aB + 16);
    j5 = x5 = b46_load_le32(aB + 20);
    j6 = x6 = b46_load_le32(aB + 24);
    j7 = x7 = b46_load_le32(aB + 28);
    j8 = x8 = b46_load_le32(aB + 32);
    j9 = x9 = b46_load_le32(aB + 36);
    j10 = x10 = b46_load_le32(aB + 40);
    j11 = x11 = b46_load_le32(aB + 44);
    j12 = x12 = b46_load_le32(aB + 48);
    j13 = x13 = b46_load_le32(aB + 52);
    j14 = x14 = b46_load_le32(aB + 56);
    j15 = x15 = b46_load_le32(aB + 60);

    for (i = 0; i < 8u; i += 2u) {
        /* column rounds */
        x4 ^= b46_rol32(x0 + x12, 7);
        x8 ^= b46_rol32(x4 + x0, 9);
        x12 ^= b46_rol32(x8 + x4, 13);
        x0 ^= b46_rol32(x12 + x8, 18);
        x9 ^= b46_rol32(x5 + x1, 7);
        x13 ^= b46_rol32(x9 + x5, 9);
        x1 ^= b46_rol32(x13 + x9, 13);
        x5 ^= b46_rol32(x1 + x13, 18);
        x14 ^= b46_rol32(x10 + x6, 7);
        x2 ^= b46_rol32(x14 + x10, 9);
        x6 ^= b46_rol32(x2 + x14, 13);
        x10 ^= b46_rol32(x6 + x2, 18);
        x3 ^= b46_rol32(x15 + x11, 7);
        x7 ^= b46_rol32(x3 + x15, 9);
        x11 ^= b46_rol32(x7 + x3, 13);
        x15 ^= b46_rol32(x11 + x7, 18);
        /* row rounds */
        x1 ^= b46_rol32(x0 + x3, 7);
        x2 ^= b46_rol32(x1 + x0, 9);
        x3 ^= b46_rol32(x2 + x1, 13);
        x0 ^= b46_rol32(x3 + x2, 18);
        x6 ^= b46_rol32(x5 + x4, 7);
        x7 ^= b46_rol32(x6 + x5, 9);
        x4 ^= b46_rol32(x7 + x6, 13);
        x5 ^= b46_rol32(x4 + x7, 18);
        x11 ^= b46_rol32(x10 + x9, 7);
        x8 ^= b46_rol32(x11 + x10, 9);
        x9 ^= b46_rol32(x8 + x11, 13);
        x10 ^= b46_rol32(x9 + x8, 18);
        x12 ^= b46_rol32(x15 + x14, 7);
        x13 ^= b46_rol32(x12 + x15, 9);
        x14 ^= b46_rol32(x13 + x12, 13);
        x15 ^= b46_rol32(x14 + x13, 18);
    }

    b46_store_le32(aB + 0, x0 + j0);
    b46_store_le32(aB + 4, x1 + j1);
    b46_store_le32(aB + 8, x2 + j2);
    b46_store_le32(aB + 12, x3 + j3);
    b46_store_le32(aB + 16, x4 + j4);
    b46_store_le32(aB + 20, x5 + j5);
    b46_store_le32(aB + 24, x6 + j6);
    b46_store_le32(aB + 28, x7 + j7);
    b46_store_le32(aB + 32, x8 + j8);
    b46_store_le32(aB + 36, x9 + j9);
    b46_store_le32(aB + 40, x10 + j10);
    b46_store_le32(aB + 44, x11 + j11);
    b46_store_le32(aB + 48, x12 + j12);
    b46_store_le32(aB + 52, x13 + j13);
    b46_store_le32(aB + 56, x14 + j14);
    b46_store_le32(aB + 60, x15 + j15);
}

/* scryptBlockMix: in/out are 128*r bytes */
static void
b46_blockmix(unsigned char *pB, uint32_t uR, unsigned char *pY)
{
    unsigned char aX[64];
    uint32_t i;
    uint32_t u2r = uR * 2u;

    memcpy(aX, pB + (size_t)(u2r - 1u) * 64u, 64u);
    for (i = 0; i < u2r; i++) {
        size_t j;

        for (j = 0; j < 64u; j++) {
            aX[j] ^= pB[(size_t)i * 64u + j];
        }
        b46_salsa20_8(aX);
        memcpy(pY + (size_t)i * 64u, aX, 64u);
    }
    /* even then odd blocks */
    for (i = 0; i < uR; i++) {
        memcpy(pB + (size_t)i * 64u, pY + (size_t)(2u * i) * 64u, 64u);
    }
    for (i = 0; i < uR; i++) {
        memcpy(pB + (size_t)(uR + i) * 64u, pY + (size_t)(2u * i + 1u) * 64u,
               64u);
    }
    memset(aX, 0, sizeof(aX));
}

/* Integerify: LE 64-bit of last 64-byte block */
static uint64_t
b46_integerify(const unsigned char *pB, uint32_t uR)
{
    const unsigned char *p = pB + (size_t)(2u * uR - 1u) * 64u;
    uint64_t uLo = b46_load_le32(p);
    uint64_t uHi = b46_load_le32(p + 4);

    return uLo | (uHi << 32);
}

static int
b46_romix(unsigned char *pB, uint64_t uN, uint32_t uR)
{
    size_t cbBlk = (size_t)uR * 128u;
    size_t cbV;
    unsigned char *pV;
    unsigned char *pY;
    unsigned char *pX;
    uint64_t i;

    if (uN > (uint64_t)(SIZE_MAX / cbBlk)) {
        errno = EINVAL;
        return -1;
    }
    cbV = (size_t)uN * cbBlk;
    if (cbV > B46_SCRYPT_MAX_MEM) {
        errno = EINVAL;
        return -1;
    }
    pV = (unsigned char *)malloc(cbV);
    pY = (unsigned char *)malloc(cbBlk);
    pX = (unsigned char *)malloc(cbBlk);
    if (pV == NULL || pY == NULL || pX == NULL) {
        free(pV);
        free(pY);
        free(pX);
        errno = ENOMEM;
        return -1;
    }
    memcpy(pX, pB, cbBlk);
    for (i = 0; i < uN; i++) {
        memcpy(pV + (size_t)i * cbBlk, pX, cbBlk);
        b46_blockmix(pX, uR, pY);
    }
    for (i = 0; i < uN; i++) {
        uint64_t uJ = b46_integerify(pX, uR) & (uN - 1u);
        size_t k;

        for (k = 0; k < cbBlk; k++) {
            pX[k] ^= pV[(size_t)uJ * cbBlk + k];
        }
        b46_blockmix(pX, uR, pY);
    }
    memcpy(pB, pX, cbBlk);
    memset(pV, 0, cbV);
    memset(pY, 0, cbBlk);
    memset(pX, 0, cbBlk);
    free(pV);
    free(pY);
    free(pX);
    return 0;
}

/*
 * scrypt / scrypt_kdf — RFC 7914.
 * N must be a power of 2 greater than 1; r,p >= 1; memory soft-capped.
 */
int
scrypt(const uint8_t *pPass, size_t cbPass, const uint8_t *pSalt, size_t cbSalt,
       uint64_t uN, uint32_t uR, uint32_t uP, uint8_t *pOut, size_t cbOut)
{
    size_t cbMF;
    size_t cbB;
    unsigned char *pB;
    uint32_t i;

    if (pOut == NULL || cbOut == 0u || uN < 2u || uR == 0u || uP == 0u) {
        errno = EINVAL;
        return -1;
    }
    /* N power of two */
    if ((uN & (uN - 1u)) != 0u) {
        errno = EINVAL;
        return -1;
    }
    /* Overflow guards: p * 128 * r */
    if (uR > (UINT32_MAX / 128u) || uP > (UINT32_MAX / (uR * 128u))) {
        errno = EINVAL;
        return -1;
    }
    cbMF = (size_t)uR * 128u;
    cbB = (size_t)uP * cbMF;
    if (cbB == 0u || cbB > B46_SCRYPT_MAX_MEM) {
        errno = EINVAL;
        return -1;
    }
    /* V size in ROMix is N * 128 * r — reject early if over soft cap */
    if (uN > (uint64_t)(B46_SCRYPT_MAX_MEM / cbMF)) {
        errno = EINVAL;
        return -1;
    }

    pB = (unsigned char *)malloc(cbB);
    if (pB == NULL) {
        errno = ENOMEM;
        return -1;
    }
    if (b46_pbkdf2(b46_hmac_sha256_w, 32u, pPass, cbPass, pSalt, cbSalt, 1u, pB,
                   cbB) != 0) {
        free(pB);
        return -1;
    }
    for (i = 0; i < uP; i++) {
        if (b46_romix(pB + (size_t)i * cbMF, uN, uR) != 0) {
            memset(pB, 0, cbB);
            free(pB);
            return -1;
        }
    }
    if (b46_pbkdf2(b46_hmac_sha256_w, 32u, pPass, cbPass, pB, cbB, 1u, pOut,
                   cbOut) != 0) {
        memset(pB, 0, cbB);
        free(pB);
        return -1;
    }
    memset(pB, 0, cbB);
    free(pB);
    return 0;
}

int
scrypt_kdf(const uint8_t *pPass, size_t cbPass, const uint8_t *pSalt,
           size_t cbSalt, uint64_t uN, uint32_t uR, uint32_t uP, uint8_t *pOut,
           size_t cbOut)
{
    return scrypt(pPass, cbPass, pSalt, cbSalt, uN, uR, uP, pOut, cbOut);
}

int __scrypt(const uint8_t *pPass, size_t cbPass, const uint8_t *pSalt,
             size_t cbSalt, uint64_t uN, uint32_t uR, uint32_t uP, uint8_t *pOut,
             size_t cbOut) __attribute__((alias("scrypt")));
int __scrypt_kdf(const uint8_t *pPass, size_t cbPass, const uint8_t *pSalt,
                 size_t cbSalt, uint64_t uN, uint32_t uR, uint32_t uP,
                 uint8_t *pOut, size_t cbOut)
    __attribute__((alias("scrypt_kdf")));

/* ---- bcrypt_pbkdf (OpenBSD shape; Blowfish out of batch scope) ---------- */

int
bcrypt_pbkdf(const char *szPass, size_t cbPass, const uint8_t *pSalt,
             size_t cbSalt, uint8_t *pKey, size_t cbKey, unsigned uRounds)
{
    (void)szPass;
    (void)cbPass;
    (void)pSalt;
    (void)cbSalt;
    (void)pKey;
    (void)cbKey;
    (void)uRounds;
    errno = ENOSYS;
    return -1;
}

int __bcrypt_pbkdf(const char *szPass, size_t cbPass, const uint8_t *pSalt,
                   size_t cbSalt, uint8_t *pKey, size_t cbKey, unsigned uRounds)
    __attribute__((alias("bcrypt_pbkdf")));

/* ---- Argon2 raw hash (public prototype; full KDF is large) -------------- */

int
argon2id_hash_raw(uint32_t uTCost, uint32_t uMCost, uint32_t uParallelism,
                  const void *pPwd, size_t cbPwd, const void *pSalt,
                  size_t cbSalt, void *pHash, size_t cbHash)
{
    (void)uTCost;
    (void)uMCost;
    (void)uParallelism;
    (void)pPwd;
    (void)cbPwd;
    (void)pSalt;
    (void)cbSalt;
    (void)pHash;
    (void)cbHash;
    errno = ENOSYS;
    return -1;
}

int
argon2i_hash_raw(uint32_t uTCost, uint32_t uMCost, uint32_t uParallelism,
                 const void *pPwd, size_t cbPwd, const void *pSalt,
                 size_t cbSalt, void *pHash, size_t cbHash)
{
    (void)uTCost;
    (void)uMCost;
    (void)uParallelism;
    (void)pPwd;
    (void)cbPwd;
    (void)pSalt;
    (void)cbSalt;
    (void)pHash;
    (void)cbHash;
    errno = ENOSYS;
    return -1;
}

int __argon2id_hash_raw(uint32_t uTCost, uint32_t uMCost, uint32_t uParallelism,
                        const void *pPwd, size_t cbPwd, const void *pSalt,
                        size_t cbSalt, void *pHash, size_t cbHash)
    __attribute__((alias("argon2id_hash_raw")));
int __argon2i_hash_raw(uint32_t uTCost, uint32_t uMCost, uint32_t uParallelism,
                       const void *pPwd, size_t cbPwd, const void *pSalt,
                       size_t cbSalt, void *pHash, size_t cbHash)
    __attribute__((alias("argon2i_hash_raw")));

/* ---- soft deepen: param checks + argon2d ENOSYS surface (unique) -------- */

/*
 * scrypt_check: validate N/r/p against bring-up soft caps without allocating.
 * Returns 0 if parameters are acceptable for scrypt(); -1 + errno otherwise.
 */
int
scrypt_check(uint64_t uN, uint32_t uR, uint32_t uP)
{
    uint64_t uMem;

    if (uN < 2u || (uN & (uN - 1u)) != 0u) {
        errno = EINVAL;
        return -1;
    }
    if (uR == 0u || uP == 0u) {
        errno = EINVAL;
        return -1;
    }
    /* V is N * 128 * r bytes; reject if over soft cap (same as scrypt). */
    if (uR > (B46_SCRYPT_MAX_MEM / 128u)) {
        errno = ENOMEM;
        return -1;
    }
    uMem = uN * 128ull * (uint64_t)uR;
    if (uMem > (uint64_t)B46_SCRYPT_MAX_MEM) {
        errno = ENOMEM;
        return -1;
    }
    return 0;
}

int __scrypt_check(uint64_t uN, uint32_t uR, uint32_t uP)
    __attribute__((alias("scrypt_check")));

/*
 * pbkdf2_hmac_sha512_ex: same as pbkdf2_hmac_sha512 with explicit PRF id
 * reserved for future (0 = HMAC-SHA512). Rejects unknown PRF.
 */
int
pbkdf2_hmac_sha512_ex(const void *pPass, size_t cbPass,
                      const unsigned char *pSalt, size_t cbSalt,
                      unsigned long uIter, unsigned char *pOut, size_t cbOut,
                      int nPrf)
{
    if (nPrf != 0) {
        errno = ENOTSUP;
        return -1;
    }
    return pbkdf2_hmac_sha512(pPass, cbPass, pSalt, cbSalt, uIter, pOut, cbOut);
}

int __pbkdf2_hmac_sha512_ex(const void *pPass, size_t cbPass,
                            const unsigned char *pSalt, size_t cbSalt,
                            unsigned long uIter, unsigned char *pOut,
                            size_t cbOut, int nPrf)
    __attribute__((alias("pbkdf2_hmac_sha512_ex")));

int
argon2d_hash_raw(uint32_t uTCost, uint32_t uMCost, uint32_t uParallelism,
                 const void *pPwd, size_t cbPwd, const void *pSalt,
                 size_t cbSalt, void *pHash, size_t cbHash)
{
    (void)uTCost;
    (void)uMCost;
    (void)uParallelism;
    (void)pPwd;
    (void)cbPwd;
    (void)pSalt;
    (void)cbSalt;
    (void)pHash;
    (void)cbHash;
    errno = ENOSYS;
    return -1;
}

int __argon2d_hash_raw(uint32_t uTCost, uint32_t uMCost, uint32_t uParallelism,
                       const void *pPwd, size_t cbPwd, const void *pSalt,
                       size_t cbSalt, void *pHash, size_t cbHash)
    __attribute__((alias("argon2d_hash_raw")));

