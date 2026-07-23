/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch122: public HMAC-SM3 (RFC 2104 over SM3).
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   SM3_Init / SM3_Update / SM3_Final / SM3_Data / SM3_Hex  → graph_batch42.c
 *   pbkdf2_hmac_sm3 / static b72_hmac_sm3                   → graph_batch72.c
 *   hmac_sha1 / hmac_sha256                                 → graph_batch38.c
 *   hmac_md5 / hmac_sha512                                  → graph_batch39.c
 *   hmac_sha384                                             → graph_batch40.c
 *   hmac_sha224 / hmac_sha3_256                             → graph_batch41.c
 *   hmac_sha3_512                                           → graph_batch42.c
 *   blake3_hash / blake3_hasher_* (unkeyed)                 → graph_batch63.c
 *
 * This TU adds only unique symbols:
 *   int hmac_sm3(const void *key, size_t klen, const void *data, size_t dlen,
 *                unsigned char out[32]);
 *     — RFC 2104 HMAC with SM3 (digest 32, block 64). Returns 0 on success,
 *       -1 if out is NULL. Key longer than block is hashed with SM3 first.
 *   __hmac_sm3 (alias)
 *   __libcgj_batch122_marker = "libcgj-batch122"
 *
 * Clean-room public ABI over extern SM3_Init/Update/Final. Integer/pointer
 * only (no SSE). Compiles freestanding with -msse2.
 */

#include <stddef.h>
#include <string.h>

/* SM3 from batch42 — digest 32, Merkle-Damgård block 64 */
void SM3_Init(void *pCtx);
void SM3_Update(void *pCtx, const void *pData, size_t cb);
void SM3_Final(unsigned char aDig[32], void *pCtx);

const char __libcgj_batch122_marker[] = "libcgj-batch122";

#define B122_SM3_DIG 32u
#define B122_SM3_BLK 64u
/* Room for batch42 SM3 context (struct ~112B) plus alignment pad */
#define B122_SM3_CTX 256u

/*
 * hmac_sm3 — RFC 2104 HMAC-SM3 one-shot.
 * out must be non-NULL (32 bytes). key/data may be NULL when len is 0.
 */
int
hmac_sm3(const void *key, size_t klen, const void *data, size_t dlen,
         unsigned char out[32])
{
	unsigned char aKey[B122_SM3_BLK];
	unsigned char aIpad[B122_SM3_BLK];
	unsigned char aOpad[B122_SM3_BLK];
	unsigned char aInner[B122_SM3_DIG];
	unsigned char aCtx[B122_SM3_CTX];
	size_t i;

	if (out == NULL) {
		return -1;
	}

	memset(aKey, 0, B122_SM3_BLK);
	if (key != NULL && klen > 0u) {
		if (klen > B122_SM3_BLK) {
			memset(aCtx, 0, sizeof(aCtx));
			SM3_Init(aCtx);
			SM3_Update(aCtx, key, klen);
			SM3_Final(aKey, aCtx);
		} else {
			memcpy(aKey, key, klen);
		}
	}

	for (i = 0; i < B122_SM3_BLK; i++) {
		aIpad[i] = (unsigned char)(aKey[i] ^ 0x36u);
		aOpad[i] = (unsigned char)(aKey[i] ^ 0x5cu);
	}

	memset(aCtx, 0, sizeof(aCtx));
	SM3_Init(aCtx);
	SM3_Update(aCtx, aIpad, B122_SM3_BLK);
	if (data != NULL && dlen > 0u) {
		SM3_Update(aCtx, data, dlen);
	}
	SM3_Final(aInner, aCtx);

	memset(aCtx, 0, sizeof(aCtx));
	SM3_Init(aCtx);
	SM3_Update(aCtx, aOpad, B122_SM3_BLK);
	SM3_Update(aCtx, aInner, B122_SM3_DIG);
	SM3_Final(out, aCtx);

	memset(aKey, 0, sizeof(aKey));
	memset(aIpad, 0, sizeof(aIpad));
	memset(aOpad, 0, sizeof(aOpad));
	memset(aInner, 0, sizeof(aInner));
	memset(aCtx, 0, sizeof(aCtx));
	return 0;
}

int __hmac_sm3(const void *key, size_t klen, const void *data, size_t dlen,
               unsigned char out[32]) __attribute__((alias("hmac_sm3")));
