/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch340: WebSocket payload mask XOR (RFC 6455 §5.3).
 *
 * Surface (unique symbols):
 *   void gj_ws_payload_mask(uint8_t *data, size_t n, const uint8_t key[4]);
 *     — In-place XOR of data[0..n) with the repeating 4-octet masking
 *       key from RFC 6455 §5.3: data[i] ^= key[i % 4]. The same
 *       operation masks and unmasks (XOR is its own inverse). No-op
 *       when data or key is NULL, or when n == 0.
 *   void __gj_ws_payload_mask  (alias)
 *   __libcgj_batch340_marker = "libcgj-batch340"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc string helpers. No third-party WebSocket source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch340_marker[] = "libcgj-batch340";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ws_payload_mask — RFC 6455 §5.3 payload masking / unmasking.
 *
 * data: mutable payload octets (NULL → no-op)
 * n:    number of bytes to XOR
 * key:  4-octet masking key (NULL → no-op)
 *
 * For each index i in [0, n): data[i] ^= key[i & 3]. Safe for n == 0.
 * Masking is applied from index 0 relative to this buffer (caller is
 * responsible for frame-relative offsets when fragmenting).
 */
void
gj_ws_payload_mask(uint8_t *data, size_t n, const uint8_t key[4])
{
	size_t i;

	if (data == NULL || key == NULL || n == 0u) {
		return;
	}

	i = 0u;
	/* Process full 4-byte mask periods first. */
	while (i + 4u <= n) {
		data[i] ^= key[0];
		data[i + 1u] ^= key[1];
		data[i + 2u] ^= key[2];
		data[i + 3u] ^= key[3];
		i += 4u;
	}
	while (i < n) {
		data[i] ^= key[i & 3u];
		i++;
	}
}

void __gj_ws_payload_mask(uint8_t *data, size_t n, const uint8_t key[4])
    __attribute__((alias("gj_ws_payload_mask")));
