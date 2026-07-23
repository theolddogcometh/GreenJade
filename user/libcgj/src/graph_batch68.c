/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch68: Adobe ASCII85 / base85 encode+decode.
 * Clean-room pure C, freestanding, integer/pointer only (no SSE).
 * Alphabet: '!' (33) .. 'u' (117) for digits 0..84 (Adobe / btoa style).
 *
 * Surface:
 *   base85_encode / base85_decode   — pure base85 (no 'z' compression)
 *   ascii85_encode / ascii85_decode — Adobe with 'z' for a zero 4-byte block
 *   __base85_encode / __base85_decode / __ascii85_encode / __ascii85_decode
 *   __libcgj_batch68_marker = "libcgj-batch68"
 *
 * Encode API (matches base64/base32 graph shape):
 *   ssize_t base85_encode(const unsigned char *in, size_t inlen,
 *                         char *out, size_t outcap);
 *   out == NULL  → returns bytes needed including trailing NUL
 *   success      → returns encoded length excluding NUL
 *   error        → -1 (EINVAL / ENOSPC)
 *
 * Decode: skips ASCII whitespace; returns decoded byte count, or -1.
 * No <~/~> stream delimiters (raw payload codec, like base64).
 *
 * Soft deepen (no API break / no multi-def):
 *   Null contract: in NULL with inlen!=0 → -1/EINVAL; out NULL on encode
 *   returns size needed (including NUL). Decode may size-probe with out NULL.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

const char __libcgj_batch68_marker[] = "libcgj-batch68";

/* Adobe digit range: '!' + d for d in 0..84 → '!' .. 'u' */
#define B68_DIG0 ((unsigned)'!')
#define B68_DIG_MAX 84u
#define B68_POW85_4 52200625u /* 85^4 */
#define B68_POW85_3 614125u   /* 85^3 */
#define B68_POW85_2 7225u     /* 85^2 */
#define B68_POW85_1 85u

static int
b68_is_ws(unsigned char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ||
           c == '\v';
}

/*
 * Worst-case encoded size including NUL (no 'z' compression):
 *   full 4-byte groups → 5 chars each
 *   remainder n (1..3) → n+1 chars
 */
static size_t
b68_enc_need(size_t cbIn)
{
    size_t nFull = cbIn / 4u;
    size_t nRem = cbIn % 4u;
    size_t cb = nFull * 5u;

    if (nRem != 0u) {
        cb += nRem + 1u;
    }
    return cb + 1u; /* NUL */
}

/* Encode one 4-byte big-endian group to 5 base85 digits (always 5 chars). */
static void
b68_enc_group(uint32_t u, char *pOut)
{
    unsigned d0, d1, d2, d3, d4;

    d4 = (unsigned)(u % 85u);
    u /= 85u;
    d3 = (unsigned)(u % 85u);
    u /= 85u;
    d2 = (unsigned)(u % 85u);
    u /= 85u;
    d1 = (unsigned)(u % 85u);
    u /= 85u;
    d0 = (unsigned)u; /* 0..84 by construction for u < 2^32 */

    pOut[0] = (char)(B68_DIG0 + d0);
    pOut[1] = (char)(B68_DIG0 + d1);
    pOut[2] = (char)(B68_DIG0 + d2);
    pOut[3] = (char)(B68_DIG0 + d3);
    pOut[4] = (char)(B68_DIG0 + d4);
}

/*
 * Decode up to 5 digits into a 32-bit value.
 * nDigits is 2..5 (caller pads partial groups with 'u' semantics separately).
 * Returns 0 on success, -1 on overflow / bad digit.
 */
static int
b68_dec_digits(const unsigned char *pDig, unsigned nDigits, uint32_t *pVal)
{
    uint64_t u = 0;
    unsigned i;

    for (i = 0; i < nDigits; i++) {
        unsigned char c = pDig[i];
        unsigned d;

        if (c < (unsigned char)B68_DIG0 ||
            c > (unsigned char)(B68_DIG0 + B68_DIG_MAX)) {
            return -1;
        }
        d = (unsigned)c - B68_DIG0;
        u = u * 85ull + (uint64_t)d;
        if (u > 0xffffffffull) {
            return -1;
        }
    }
    /* Pad remaining positions as digit 84 ('u') for partial groups. */
    for (; i < 5u; i++) {
        u = u * 85ull + (uint64_t)B68_DIG_MAX;
        if (u > 0xffffffffull) {
            return -1;
        }
    }
    *pVal = (uint32_t)u;
    return 0;
}

static ssize_t
b68_encode(const unsigned char *pIn, size_t cbIn, char *szOut, size_t cbOut,
           int bUseZ)
{
    size_t i = 0;
    size_t j = 0;
    size_t cbNeed = b68_enc_need(cbIn);

    if (pIn == NULL && cbIn != 0u) {
        errno = EINVAL;
        return -1;
    }
    if (szOut == NULL) {
        return (ssize_t)cbNeed;
    }
    if (cbOut < cbNeed) {
        errno = ENOSPC;
        return -1;
    }

    while (i + 4u <= cbIn) {
        uint32_t u = ((uint32_t)pIn[i] << 24) | ((uint32_t)pIn[i + 1u] << 16) |
                     ((uint32_t)pIn[i + 2u] << 8) | (uint32_t)pIn[i + 3u];

        if (bUseZ && u == 0u) {
            szOut[j++] = 'z';
        } else {
            b68_enc_group(u, szOut + j);
            j += 5u;
        }
        i += 4u;
    }

    if (i < cbIn) {
        /* Partial final group: zero-pad to 4 bytes, emit (n+1) chars. */
        size_t nLeft = cbIn - i;
        uint32_t u = 0;
        char aTmp[5];
        size_t k;

        for (k = 0; k < nLeft; k++) {
            u |= (uint32_t)pIn[i + k] << (24u - (unsigned)(k * 8u));
        }
        /* Partial groups never use 'z' (Adobe rule). */
        b68_enc_group(u, aTmp);
        for (k = 0; k < nLeft + 1u; k++) {
            szOut[j++] = aTmp[k];
        }
    }

    szOut[j] = '\0';
    return (ssize_t)j;
}

static ssize_t
b68_decode(const char *szIn, size_t cbIn, unsigned char *pOut, size_t cbOut,
           int bAllowZ)
{
    size_t i = 0;
    size_t j = 0;
    unsigned char aDig[5];
    unsigned nDig = 0;

    if (szIn == NULL && cbIn != 0u) {
        errno = EINVAL;
        return -1;
    }

    while (i < cbIn) {
        unsigned char c = (unsigned char)szIn[i++];

        if (b68_is_ws(c)) {
            continue;
        }
        if (bAllowZ && c == (unsigned char)'z') {
            if (nDig != 0u) {
                errno = EINVAL;
                return -1;
            }
            if (pOut == NULL) {
                j += 4u;
            } else {
                if (j + 4u > cbOut) {
                    errno = ENOSPC;
                    return -1;
                }
                pOut[j++] = 0;
                pOut[j++] = 0;
                pOut[j++] = 0;
                pOut[j++] = 0;
            }
            continue;
        }
        if (c < (unsigned char)B68_DIG0 ||
            c > (unsigned char)(B68_DIG0 + B68_DIG_MAX)) {
            errno = EINVAL;
            return -1;
        }
        aDig[nDig++] = c;
        if (nDig == 5u) {
            uint32_t u;

            if (b68_dec_digits(aDig, 5u, &u) != 0) {
                errno = EINVAL;
                return -1;
            }
            if (pOut == NULL) {
                j += 4u;
            } else {
                if (j + 4u > cbOut) {
                    errno = ENOSPC;
                    return -1;
                }
                pOut[j++] = (unsigned char)(u >> 24);
                pOut[j++] = (unsigned char)(u >> 16);
                pOut[j++] = (unsigned char)(u >> 8);
                pOut[j++] = (unsigned char)u;
            }
            nDig = 0u;
        }
    }

    if (nDig == 1u) {
        /* One leftover digit is never a valid partial group. */
        errno = EINVAL;
        return -1;
    }
    if (nDig > 1u) {
        /* Partial final: n digits (2..4) → n-1 output bytes. */
        uint32_t u;
        unsigned nOut = nDig - 1u;
        unsigned k;

        if (b68_dec_digits(aDig, nDig, &u) != 0) {
            errno = EINVAL;
            return -1;
        }
        if (pOut == NULL) {
            j += (size_t)nOut;
        } else {
            if (j + (size_t)nOut > cbOut) {
                errno = ENOSPC;
                return -1;
            }
            for (k = 0; k < nOut; k++) {
                pOut[j++] = (unsigned char)(u >> (24u - k * 8u));
            }
        }
    }

    return (ssize_t)j;
}

/* ---- public base85 (pure; no 'z') --------------------------------------- */

ssize_t
base85_encode(const unsigned char *pIn, size_t cbIn, char *szOut, size_t cbOut)
{
    return b68_encode(pIn, cbIn, szOut, cbOut, 0);
}

ssize_t
base85_decode(const char *szIn, size_t cbIn, unsigned char *pOut, size_t cbOut)
{
    return b68_decode(szIn, cbIn, pOut, cbOut, 0);
}

/* ---- public Adobe ascii85 (with 'z' zero-block shortcut) ---------------- */

ssize_t
ascii85_encode(const unsigned char *pIn, size_t cbIn, char *szOut,
               size_t cbOut)
{
    return b68_encode(pIn, cbIn, szOut, cbOut, 1);
}

ssize_t
ascii85_decode(const char *szIn, size_t cbIn, unsigned char *pOut,
               size_t cbOut)
{
    return b68_decode(szIn, cbIn, pOut, cbOut, 1);
}

ssize_t __base85_encode(const unsigned char *p, size_t n, char *s, size_t m)
    __attribute__((alias("base85_encode")));
ssize_t __base85_decode(const char *s, size_t n, unsigned char *p, size_t m)
    __attribute__((alias("base85_decode")));
ssize_t __ascii85_encode(const unsigned char *p, size_t n, char *s, size_t m)
    __attribute__((alias("ascii85_encode")));
ssize_t __ascii85_decode(const char *s, size_t n, unsigned char *p, size_t m)
    __attribute__((alias("ascii85_decode")));
