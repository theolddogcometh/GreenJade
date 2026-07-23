/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch198: WebSocket Sec-WebSocket-Accept key
 * compute (RFC 6455 §1.3 / §4.2.2).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party WebSocket source was copied.
 *
 * RFC 6455 handshake accept value:
 *   Sec-WebSocket-Accept =
 *     base64( SHA1( Sec-WebSocket-Key + GUID ) )
 *   GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
 *
 * Surface (unique symbols):
 *   int gj_ws_accept_key(const char *client_key, char *out, size_t cap);
 *     — SHA1(client_key || GUID), then standard base64 into out.
 *       Writes a NUL-terminated 28-char base64 string (SHA-1 digests
 *       are 20 octets → 28 base64 chars). Requires cap >= 29.
 *       Returns 0 on success, -1 on NULL / short buffer / encode fail.
 *   __gj_ws_accept_key  (alias)
 *   __libcgj_batch198_marker = "libcgj-batch198"
 *
 * Digests / codec live elsewhere (extern only — avoid multi-def):
 *   SHA1Init / SHA1Update / SHA1Final  → graph_batch31/32.c
 *   base64_encode                      → graph_batch40.c
 *
 * Related (not redefined here):
 *   gj_ws_header_parse / build / mask  → graph_batch126.c
 */

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h> /* ssize_t for base64_encode */

const char __libcgj_batch198_marker[] = "libcgj-batch198";

/* ---- extern: SHA-1 (batch31/32) + base64 (batch40) --------------------- */

void SHA1Init(void *pCtx);
void SHA1Update(void *pCtx, const void *pData, size_t cb);
void SHA1Final(unsigned char aDig[20], void *pCtx);

extern ssize_t base64_encode(const unsigned char *pIn, size_t cbIn,
                             char *szOut, size_t cbOut);

/*
 * RFC 6455 §1.3 magic GUID concatenated after the client Sec-WebSocket-Key
 * before SHA-1. Length is 36 octets (no trailing NUL counted in the hash).
 */
#define B198_WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
#define B198_WS_GUID_LEN 36u

/* SHA-1 digest size; base64(20) is always 28 chars + NUL → need 29. */
#define B198_SHA1_LEN 20u
#define B198_B64_LEN 28u
#define B198_OUT_NEED (B198_B64_LEN + 1u)

/* Opaque SHA-1 context footprint (batch32 SHA1Init zeros 96 bytes). */
#define B198_SHA1_CTX_BYTES 128u

/* ---- freestanding helpers ---------------------------------------------- */

static size_t
b198_strlen(const char *sz)
{
	size_t cb = 0u;

	if (sz == NULL) {
		return 0u;
	}
	while (sz[cb] != '\0') {
		cb++;
	}
	return cb;
}

/* ---- gj_ws_accept_key -------------------------------------------------- */

/*
 * Compute Sec-WebSocket-Accept for a client Sec-WebSocket-Key string.
 * client_key is the raw header value (typically 24-char base64 + no
 * surrounding quotes/whitespace). out receives the accept value as a
 * C string; cap must be at least 29 (28 base64 chars + NUL).
 */
int
gj_ws_accept_key(const char *szClientKey, char *szOut, size_t cbCap)
{
	unsigned char aCtx[B198_SHA1_CTX_BYTES];
	unsigned char aDig[B198_SHA1_LEN];
	size_t cbKey;
	size_t i;
	ssize_t nB64;

	if (szClientKey == NULL || szOut == NULL) {
		return -1;
	}
	if (cbCap < B198_OUT_NEED) {
		return -1;
	}

	cbKey = b198_strlen(szClientKey);

	/* Clear context; SHA1Init also zeros its known footprint. */
	for (i = 0u; i < B198_SHA1_CTX_BYTES; i++) {
		aCtx[i] = 0u;
	}

	SHA1Init(aCtx);
	if (cbKey > 0u) {
		SHA1Update(aCtx, szClientKey, cbKey);
	}
	SHA1Update(aCtx, B198_WS_GUID, B198_WS_GUID_LEN);
	SHA1Final(aDig, aCtx);

	nB64 = base64_encode(aDig, B198_SHA1_LEN, szOut, cbCap);
	if (nB64 < 0) {
		return -1;
	}
	/* Fixed digest size → fixed base64 width. */
	if ((size_t)nB64 != B198_B64_LEN) {
		return -1;
	}
	/* base64_encode already wrote a trailing NUL when buffer fits. */
	return 0;
}

int __gj_ws_accept_key(const char *client_key, char *out, size_t cap)
    __attribute__((alias("gj_ws_accept_key")));
