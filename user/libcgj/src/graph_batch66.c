/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch66: HMAC-DRBG (NIST SP 800-90A Rev. 1 §10.1.2)
 * using SHA-256. Clean-room from the public NIST document; pure C,
 * freestanding, integer/pointer only (no SSE). Dual MIT OR Apache-2.0.
 *
 * Surface:
 *   hmac_drbg_init / hmac_drbg_reseed / hmac_drbg_generate
 *   __hmac_drbg_init / __hmac_drbg_reseed / __hmac_drbg_generate  (aliases)
 *   __libcgj_batch66_marker = "libcgj-batch66"
 *
 * Opaque context:
 *   Callers must provide a buffer of at least HMAC_DRBG_CTX_BYTES (256)
 *   bytes, suitably aligned for uint64_t. Layout is private to this TU.
 *
 * Extern digests (batch31/32):
 *   SHA256_Init / SHA256_Update / SHA256_Final
 * HMAC-SHA256 is implemented locally (streaming) over those primitives;
 * does not re-export hmac_sha256 (batch38 owns that name).
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch66_marker[] = "libcgj-batch66";

/* Opaque context size for callers (bytes). */
#define HMAC_DRBG_CTX_BYTES 256

/* SHA-256 / HMAC-DRBG parameters (SP 800-90A Table 2 / §10.1.2). */
#define B66_OUTLEN 32u          /* outlen in bytes (256 bits) */
#define B66_BLK 64u             /* SHA-256 block size */
#define B66_MAX_REQUEST 65536u  /* 2^19 bits = 2^16 bytes per request */
/* reseed_interval = 2^48 (SP 800-90A Table 2 recommendation for HMAC_DRBG) */
#define B66_RESEED_INTERVAL 0x0001000000000000ull

/* Existing SHA-256 (batch31/32). Context is 112 bytes; pad stack to 128. */
void SHA256_Init(void *pCtx);
void SHA256_Update(void *pCtx, const void *pData, size_t cb);
void SHA256_Final(unsigned char aDig[32], void *pCtx);

/* --------------------------------------------------------------------------
 * Freestanding mem helpers (no libc string dependency in this TU)
 * -------------------------------------------------------------------------- */

static void
b66_memzero(void *pDst, size_t cb)
{
    unsigned char *p = (unsigned char *)pDst;
    size_t i;

    for (i = 0; i < cb; i++) {
        p[i] = 0;
    }
}

static void
b66_memcpy(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *pOut = (unsigned char *)pDst;
    const unsigned char *pIn = (const unsigned char *)pSrc;
    size_t i;

    for (i = 0; i < cb; i++) {
        pOut[i] = pIn[i];
    }
}

/* --------------------------------------------------------------------------
 * Opaque working state (fits in HMAC_DRBG_CTX_BYTES)
 *
 *   V[32]  | Key[32] | reseed_counter(u64) | flags(u32) | pad...
 * -------------------------------------------------------------------------- */

struct b66_state {
    unsigned char aV[B66_OUTLEN];
    unsigned char aKey[B66_OUTLEN];
    uint64_t uReseed;
    uint32_t uFlags; /* bit0 = instantiated */
    unsigned char aPad[HMAC_DRBG_CTX_BYTES - B66_OUTLEN - B66_OUTLEN - 8u - 4u];
};

#define B66_FLAG_INIT 1u

/* Compile-time size check: negative array size if struct drifts from 256. */
typedef char b66_ctx_size_ok[(sizeof(struct b66_state) == HMAC_DRBG_CTX_BYTES)
                                 ? 1
                                 : -1];

static struct b66_state *
b66_cast(void *pCtx)
{
    /* Reference the typedef size so -Wunused-local-typedefs stays quiet. */
    (void)sizeof(b66_ctx_size_ok);
    return (struct b66_state *)pCtx;
}

/* --------------------------------------------------------------------------
 * Streaming HMAC-SHA256 (RFC 2104) over SHA256_* 
 * -------------------------------------------------------------------------- */

struct b66_hmac {
    unsigned char aIpad[B66_BLK];
    unsigned char aOpad[B66_BLK];
    unsigned char aSha[128]; /* >= 112-byte SHA-256 context */
};

static void
b66_hmac_start(struct b66_hmac *pH, const unsigned char *pKey, size_t cbKey)
{
    unsigned char aKey[B66_BLK];
    unsigned char aDig[B66_OUTLEN];
    size_t i;

    b66_memzero(aKey, sizeof(aKey));
    if (pKey != NULL && cbKey > 0u) {
        if (cbKey > B66_BLK) {
            b66_memzero(pH->aSha, sizeof(pH->aSha));
            SHA256_Init(pH->aSha);
            SHA256_Update(pH->aSha, pKey, cbKey);
            SHA256_Final(aDig, pH->aSha);
            b66_memcpy(aKey, aDig, B66_OUTLEN);
            b66_memzero(aDig, sizeof(aDig));
        } else {
            b66_memcpy(aKey, pKey, cbKey);
        }
    }
    for (i = 0; i < B66_BLK; i++) {
        pH->aIpad[i] = (unsigned char)(aKey[i] ^ 0x36u);
        pH->aOpad[i] = (unsigned char)(aKey[i] ^ 0x5cu);
    }
    b66_memzero(aKey, sizeof(aKey));
    b66_memzero(pH->aSha, sizeof(pH->aSha));
    SHA256_Init(pH->aSha);
    SHA256_Update(pH->aSha, pH->aIpad, B66_BLK);
}

static void
b66_hmac_update(struct b66_hmac *pH, const void *pData, size_t cb)
{
    if (pData != NULL && cb > 0u) {
        SHA256_Update(pH->aSha, pData, cb);
    }
}

static void
b66_hmac_finish(struct b66_hmac *pH, unsigned char aOut[B66_OUTLEN])
{
    unsigned char aInner[B66_OUTLEN];

    SHA256_Final(aInner, pH->aSha);
    b66_memzero(pH->aSha, sizeof(pH->aSha));
    SHA256_Init(pH->aSha);
    SHA256_Update(pH->aSha, pH->aOpad, B66_BLK);
    SHA256_Update(pH->aSha, aInner, B66_OUTLEN);
    SHA256_Final(aOut, pH->aSha);
    b66_memzero(aInner, sizeof(aInner));
    b66_memzero(pH->aIpad, sizeof(pH->aIpad));
    b66_memzero(pH->aOpad, sizeof(pH->aOpad));
    b66_memzero(pH->aSha, sizeof(pH->aSha));
}

/* One-shot HMAC(K, data) with 32-byte key (normal DRBG working-state case). */
static void
b66_hmac_kv(const unsigned char aKey[B66_OUTLEN],
            const unsigned char aData[B66_OUTLEN], unsigned char aOut[B66_OUTLEN])
{
    struct b66_hmac h;

    b66_hmac_start(&h, aKey, B66_OUTLEN);
    b66_hmac_update(&h, aData, B66_OUTLEN);
    b66_hmac_finish(&h, aOut);
}

/* --------------------------------------------------------------------------
 * HMAC_DRBG_Update (SP 800-90A §10.1.2.2)
 *
 * provided_data is the concatenation of up to three optional segments
 * (any may be empty). Null provided_data <=> all lengths zero.
 * -------------------------------------------------------------------------- */

static void
b66_update(struct b66_state *p, const unsigned char *p0, size_t n0,
           const unsigned char *p1, size_t n1, const unsigned char *p2, size_t n2)
{
    struct b66_hmac h;
    unsigned char aTmp[B66_OUTLEN];
    unsigned char uSep;
    int fHas;

    fHas = (n0 > 0u) || (n1 > 0u) || (n2 > 0u);

    /* K = HMAC(K, V || 0x00 || provided_data) */
    uSep = 0x00u;
    b66_hmac_start(&h, p->aKey, B66_OUTLEN);
    b66_hmac_update(&h, p->aV, B66_OUTLEN);
    b66_hmac_update(&h, &uSep, 1u);
    if (n0 > 0u) {
        b66_hmac_update(&h, p0, n0);
    }
    if (n1 > 0u) {
        b66_hmac_update(&h, p1, n1);
    }
    if (n2 > 0u) {
        b66_hmac_update(&h, p2, n2);
    }
    b66_hmac_finish(&h, aTmp);
    b66_memcpy(p->aKey, aTmp, B66_OUTLEN);

    /* V = HMAC(K, V) */
    b66_hmac_kv(p->aKey, p->aV, aTmp);
    b66_memcpy(p->aV, aTmp, B66_OUTLEN);

    if (!fHas) {
        b66_memzero(aTmp, sizeof(aTmp));
        return;
    }

    /* K = HMAC(K, V || 0x01 || provided_data) */
    uSep = 0x01u;
    b66_hmac_start(&h, p->aKey, B66_OUTLEN);
    b66_hmac_update(&h, p->aV, B66_OUTLEN);
    b66_hmac_update(&h, &uSep, 1u);
    if (n0 > 0u) {
        b66_hmac_update(&h, p0, n0);
    }
    if (n1 > 0u) {
        b66_hmac_update(&h, p1, n1);
    }
    if (n2 > 0u) {
        b66_hmac_update(&h, p2, n2);
    }
    b66_hmac_finish(&h, aTmp);
    b66_memcpy(p->aKey, aTmp, B66_OUTLEN);

    /* V = HMAC(K, V) */
    b66_hmac_kv(p->aKey, p->aV, aTmp);
    b66_memcpy(p->aV, aTmp, B66_OUTLEN);
    b66_memzero(aTmp, sizeof(aTmp));
}

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

/*
 * hmac_drbg_init — HMAC_DRBG_Instantiate_algorithm (§10.1.2.3)
 *
 * seed_material = entropy || nonce || personalization_string
 * Key = 0x00..00, V = 0x01..01; Update(seed_material); reseed_counter = 1.
 *
 * Returns 0 on success, -1 on bad arguments (NULL ctx, missing entropy).
 * Opaque ctx must be at least HMAC_DRBG_CTX_BYTES (256) bytes.
 */
int
hmac_drbg_init(void *pCtx, const unsigned char *pEntropy, size_t cbEntropy,
               const unsigned char *pNonce, size_t cbNonce,
               const unsigned char *pPers, size_t cbPers)
{
    struct b66_state *p;
    size_t i;

    if (pCtx == NULL) {
        return -1;
    }
    if (pEntropy == NULL || cbEntropy == 0u) {
        return -1;
    }
    if ((pNonce == NULL && cbNonce != 0u) || (pPers == NULL && cbPers != 0u)) {
        return -1;
    }

    p = b66_cast(pCtx);
    b66_memzero(p, HMAC_DRBG_CTX_BYTES);

    for (i = 0; i < B66_OUTLEN; i++) {
        p->aKey[i] = 0x00u;
        p->aV[i] = 0x01u;
    }

    b66_update(p, pEntropy, cbEntropy, pNonce, cbNonce, pPers, cbPers);
    p->uReseed = 1ull;
    p->uFlags = B66_FLAG_INIT;
    return 0;
}

/*
 * hmac_drbg_reseed — HMAC_DRBG_Reseed_algorithm (§10.1.2.4)
 *
 * seed_material = entropy || additional_input; Update; reseed_counter = 1.
 * Returns 0 on success, -1 on error.
 */
int
hmac_drbg_reseed(void *pCtx, const unsigned char *pEntropy, size_t cbEntropy,
                 const unsigned char *pAdd, size_t cbAdd)
{
    struct b66_state *p;

    if (pCtx == NULL) {
        return -1;
    }
    p = b66_cast(pCtx);
    if ((p->uFlags & B66_FLAG_INIT) == 0u) {
        return -1;
    }
    if (pEntropy == NULL || cbEntropy == 0u) {
        return -1;
    }
    if (pAdd == NULL && cbAdd != 0u) {
        return -1;
    }

    b66_update(p, pEntropy, cbEntropy, pAdd, cbAdd, NULL, 0u);
    p->uReseed = 1ull;
    return 0;
}

/*
 * hmac_drbg_generate — HMAC_DRBG_Generate_algorithm (§10.1.2.5)
 *
 * Writes outlen bytes into out. outlen must be <= 65536 (2^19 bits).
 * Returns 0 on success, -1 on error (not init, reseed required, bad args,
 * request too large).
 */
int
hmac_drbg_generate(void *pCtx, unsigned char *pOut, size_t cbOut,
                   const unsigned char *pAdd, size_t cbAdd)
{
    struct b66_state *p;
    unsigned char aTmp[B66_OUTLEN];
    size_t cbDone;
    size_t cbChunk;

    if (pCtx == NULL) {
        return -1;
    }
    p = b66_cast(pCtx);
    if ((p->uFlags & B66_FLAG_INIT) == 0u) {
        return -1;
    }
    if (cbOut > B66_MAX_REQUEST) {
        return -1;
    }
    if (pOut == NULL && cbOut != 0u) {
        return -1;
    }
    if (pAdd == NULL && cbAdd != 0u) {
        return -1;
    }
    if (p->uReseed > B66_RESEED_INTERVAL) {
        return -1; /* reseed required */
    }

    /* Optional additional_input before generation. */
    if (cbAdd > 0u) {
        b66_update(p, pAdd, cbAdd, NULL, 0u, NULL, 0u);
    }

    cbDone = 0u;
    while (cbDone < cbOut) {
        /* V = HMAC(Key, V) */
        b66_hmac_kv(p->aKey, p->aV, aTmp);
        b66_memcpy(p->aV, aTmp, B66_OUTLEN);
        cbChunk = cbOut - cbDone;
        if (cbChunk > B66_OUTLEN) {
            cbChunk = B66_OUTLEN;
        }
        b66_memcpy(pOut + cbDone, p->aV, cbChunk);
        cbDone += cbChunk;
    }

    /* Update with additional_input (may be Null). */
    if (cbAdd > 0u) {
        b66_update(p, pAdd, cbAdd, NULL, 0u, NULL, 0u);
    } else {
        b66_update(p, NULL, 0u, NULL, 0u, NULL, 0u);
    }

    p->uReseed += 1ull;
    b66_memzero(aTmp, sizeof(aTmp));
    return 0;
}

int __hmac_drbg_init(void *pCtx, const unsigned char *pEntropy, size_t cbEntropy,
                     const unsigned char *pNonce, size_t cbNonce,
                     const unsigned char *pPers, size_t cbPers)
    __attribute__((alias("hmac_drbg_init")));

int __hmac_drbg_reseed(void *pCtx, const unsigned char *pEntropy,
                       size_t cbEntropy, const unsigned char *pAdd, size_t cbAdd)
    __attribute__((alias("hmac_drbg_reseed")));

int __hmac_drbg_generate(void *pCtx, unsigned char *pOut, size_t cbOut,
                         const unsigned char *pAdd, size_t cbAdd)
    __attribute__((alias("hmac_drbg_generate")));
