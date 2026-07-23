/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch465: MessagePack unsigned integer pack/unpack.
 * Clean-room freestanding pure C — builds under userspace -msse2.
 * No third-party MessagePack source was copied.
 *
 * Wire (https://msgpack.org/ format family, unsigned subset):
 *   positive fixint  0x00..0x7f     single-byte value 0..127
 *   uint8            0xcc + u8      1-byte big-endian payload
 *   uint16           0xcd + u16be   2-byte big-endian payload
 *   uint32           0xce + u32be   4-byte big-endian payload
 *   uint64           0xcf + u64be   8-byte big-endian payload
 *
 * Surface (unique symbols):
 *   int gj_mp_pack_u64(uint64_t v, uint8_t *out, size_t cap, size_t *n);
 *     Compact-encode v (smallest legal form). *n = bytes written, or
 *     required size when out is NULL (size query, returns 0) or when
 *     cap is too small (returns -1). n must be non-NULL.
 *   int gj_mp_unpack_u64(const uint8_t *in, size_t nin, uint64_t *v,
 *                        size_t *nread);
 *     Decode one unsigned MessagePack integer (fixint / uint8..64).
 *     *v = value; *nread = bytes consumed. in/v/nread must be non-NULL.
 *   __gj_mp_pack_u64 / __gj_mp_unpack_u64  (aliases)
 *   __libcgj_batch465_marker = "libcgj-batch465"
 *
 * Semantics: 0 success, -1 error.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch465_marker[] = "libcgj-batch465";

/* MessagePack type tags for the unsigned integer family. */
#define B465_UINT8  0xccu
#define B465_UINT16 0xcdu
#define B465_UINT32 0xceu
#define B465_UINT64 0xcfu
#define B465_FIXINT_MAX 0x7fu

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b465_store_be — write u64Val as big-endian into pDst[0..cb-1] (cb 1..8).
 * High bytes of u64Val are discarded when cb < 8.
 */
static void
b465_store_be(uint8_t *pDst, size_t cb, uint64_t u64Val)
{
	size_t i;

	for (i = 0u; i < cb; i++) {
		size_t uShift = (cb - 1u - i) * 8u;

		pDst[i] = (uint8_t)((u64Val >> uShift) & 0xffull);
	}
}

/*
 * b465_load_be — read big-endian integer of cb bytes (1..8) from pSrc.
 */
static uint64_t
b465_load_be(const uint8_t *pSrc, size_t cb)
{
	uint64_t u64Val;
	size_t i;

	u64Val = 0ull;
	for (i = 0u; i < cb; i++) {
		u64Val = (u64Val << 8) | (uint64_t)pSrc[i];
	}
	return u64Val;
}

/*
 * b465_need — required wire length for compact-encoding u64Val.
 * Always returns one of 1, 2, 3, 5, 9.
 */
static size_t
b465_need(uint64_t u64Val)
{
	if (u64Val <= (uint64_t)B465_FIXINT_MAX) {
		return 1u;
	}
	if (u64Val <= 0xffull) {
		return 2u;
	}
	if (u64Val <= 0xffffull) {
		return 3u;
	}
	if (u64Val <= 0xffffffffull) {
		return 5u;
	}
	return 9u;
}

/* ---- public: pack ------------------------------------------------------ */

/*
 * gj_mp_pack_u64 — MessagePack-encode unsigned integer v compactly.
 *
 * Forms chosen (smallest legal):
 *   v <= 0x7f         → positive fixint (1 byte)
 *   v <= 0xff         → uint8  (tag + 1)
 *   v <= 0xffff       → uint16 (tag + 2)
 *   v <= 0xffffffff   → uint32 (tag + 4)
 *   else              → uint64 (tag + 8)
 *
 * Edge cases:
 *   n == NULL → -1
 *   out == NULL → size query: *n = required, return 0
 *   cap < required → *n = required, return -1
 *   success → *n = bytes written, return 0
 */
int
gj_mp_pack_u64(uint64_t v, uint8_t *out, size_t cap, size_t *n)
{
	size_t cbNeed;
	uint8_t aWire[9];
	size_t i;

	if (n == NULL) {
		return -1;
	}

	cbNeed = b465_need(v);
	*n = cbNeed;

	/* Build full encoding into stack temp (always fits in 9 bytes). */
	if (cbNeed == 1u) {
		aWire[0] = (uint8_t)v;
	} else if (cbNeed == 2u) {
		aWire[0] = (uint8_t)B465_UINT8;
		aWire[1] = (uint8_t)v;
	} else if (cbNeed == 3u) {
		aWire[0] = (uint8_t)B465_UINT16;
		b465_store_be(aWire + 1, 2u, v);
	} else if (cbNeed == 5u) {
		aWire[0] = (uint8_t)B465_UINT32;
		b465_store_be(aWire + 1, 4u, v);
	} else {
		aWire[0] = (uint8_t)B465_UINT64;
		b465_store_be(aWire + 1, 8u, v);
	}

	if (out == NULL) {
		/* Size query: success with *n set. */
		return 0;
	}
	if (cap < cbNeed) {
		return -1;
	}

	for (i = 0u; i < cbNeed; i++) {
		out[i] = aWire[i];
	}
	return 0;
}

/* ---- public: unpack ---------------------------------------------------- */

/*
 * gj_mp_unpack_u64 — decode one MessagePack unsigned integer from in.
 *
 * Accepts positive fixint and uint8/16/32/64. Non-unsigned tags → -1.
 * *nread is set only on success.
 *
 * Edge cases:
 *   in == NULL || v == NULL || nread == NULL || nin == 0 → -1
 *   truncated payload → -1
 *   success → *v = value, *nread = bytes consumed, return 0
 */
int
gj_mp_unpack_u64(const uint8_t *in, size_t nin, uint64_t *v, size_t *nread)
{
	uint8_t u8Tag;
	size_t cbPay;
	size_t cbTotal;

	if (in == NULL || v == NULL || nread == NULL || nin == 0u) {
		return -1;
	}

	u8Tag = in[0];

	if (u8Tag <= (uint8_t)B465_FIXINT_MAX) {
		/* positive fixint: value is the tag itself */
		*v = (uint64_t)u8Tag;
		*nread = 1u;
		return 0;
	}

	if (u8Tag == (uint8_t)B465_UINT8) {
		cbPay = 1u;
	} else if (u8Tag == (uint8_t)B465_UINT16) {
		cbPay = 2u;
	} else if (u8Tag == (uint8_t)B465_UINT32) {
		cbPay = 4u;
	} else if (u8Tag == (uint8_t)B465_UINT64) {
		cbPay = 8u;
	} else {
		/* Not an unsigned integer form in this subset. */
		return -1;
	}

	cbTotal = 1u + cbPay;
	if (nin < cbTotal) {
		return -1;
	}

	*v = b465_load_be(in + 1, cbPay);
	*nread = cbTotal;
	return 0;
}

/* ---- aliases ----------------------------------------------------------- */

int __gj_mp_pack_u64(uint64_t v, uint8_t *out, size_t cap, size_t *n)
    __attribute__((alias("gj_mp_pack_u64")));

int __gj_mp_unpack_u64(const uint8_t *in, size_t nin, uint64_t *v,
                       size_t *nread)
    __attribute__((alias("gj_mp_unpack_u64")));
