/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch126: WebSocket frame header (RFC 6455 §5.2)
 * parse/build and payload masking (§5.3).
 * Clean-room freestanding pure C — integer/pointer only (builds under
 * userspace -msse2). No third-party WebSocket source was copied.
 *
 * Surface (unique symbols):
 *   int gj_ws_header_parse(const unsigned char *buf, size_t len,
 *                          int *fin, int *opcode, int *masked,
 *                          uint64_t *payload_len, size_t *hdr_len);
 *     Parse a WebSocket frame header from buf[0..len). On success
 *     (return 0) fills fin/opcode/masked/payload_len and *hdr_len
 *     (bytes consumed by the header, including the 4-byte mask key
 *     when MASK is set). Returns -1 if buf is short, NULL, or the
 *     extended-length encoding is non-minimal / has MSB set (RFC).
 *   int gj_ws_header_build(unsigned char *out, size_t outcap,
 *                          int fin, int opcode, uint64_t payload_len,
 *                          size_t *hdr_len);
 *     Build an unmasked (MASK=0) frame header into out. Uses minimal
 *     length encoding. *hdr_len = 2 / 4 / 10. Returns 0 on success,
 *     -1 if out/outcap insufficient or payload_len has bit 63 set.
 *   void gj_ws_mask(unsigned char *data, size_t len,
 *                   const unsigned char mask[4]);
 *     In-place XOR payload with repeating 4-octet masking key (RFC
 *     6455 §5.3). Same operation masks and unmasks. No-op on NULL.
 *   __gj_ws_header_parse / __gj_ws_header_build / __gj_ws_mask
 *   __libcgj_batch126_marker = "libcgj-batch126"
 *
 * Frame header layout (network / big-endian extended lengths):
 *   byte0: FIN|RSV1|RSV2|RSV3|opcode(4)
 *   byte1: MASK|payload_len(7)   len 0..125 direct; 126→u16; 127→u64
 *   [mask key 4 octets if MASK]
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch126_marker[] = "libcgj-batch126";

/* ---- big-endian loads / stores ----------------------------------------- */

static uint16_t
b126_rd16be(const unsigned char *p)
{
	return (uint16_t)(((uint16_t)p[0] << 8) | (uint16_t)p[1]);
}

static uint64_t
b126_rd64be(const unsigned char *p)
{
	return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
	       ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
	       ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
	       ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

static void
b126_wr16be(unsigned char *p, uint16_t v)
{
	p[0] = (unsigned char)((v >> 8) & 0xffu);
	p[1] = (unsigned char)(v & 0xffu);
}

static void
b126_wr64be(unsigned char *p, uint64_t v)
{
	p[0] = (unsigned char)((v >> 56) & 0xffu);
	p[1] = (unsigned char)((v >> 48) & 0xffu);
	p[2] = (unsigned char)((v >> 40) & 0xffu);
	p[3] = (unsigned char)((v >> 32) & 0xffu);
	p[4] = (unsigned char)((v >> 24) & 0xffu);
	p[5] = (unsigned char)((v >> 16) & 0xffu);
	p[6] = (unsigned char)((v >> 8) & 0xffu);
	p[7] = (unsigned char)(v & 0xffu);
}

/* ---- parse ------------------------------------------------------------- */

/*
 * Decode a WebSocket base header + optional extended length + mask key.
 * RSV bits are not validated (application may check opcode/RSV policy).
 */
int
gj_ws_header_parse(const unsigned char *buf, size_t len, int *fin,
    int *opcode, int *masked, uint64_t *payload_len, size_t *hdr_len)
{
	unsigned char b0;
	unsigned char b1;
	unsigned u7;
	int bMasked;
	uint64_t uPay;
	size_t cbHdr;

	if (buf == NULL || fin == NULL || opcode == NULL || masked == NULL ||
	    payload_len == NULL || hdr_len == NULL) {
		return -1;
	}
	if (len < 2u) {
		return -1;
	}

	b0 = buf[0];
	b1 = buf[1];
	bMasked = (b1 & 0x80u) != 0;
	u7 = (unsigned)(b1 & 0x7fu);

	if (u7 <= 125u) {
		uPay = (uint64_t)u7;
		cbHdr = 2u;
	} else if (u7 == 126u) {
		if (len < 4u) {
			return -1;
		}
		uPay = (uint64_t)b126_rd16be(buf + 2);
		/* Minimal encoding: 16-bit form only for len >= 126. */
		if (uPay < 126u) {
			return -1;
		}
		cbHdr = 4u;
	} else {
		/* u7 == 127 */
		if (len < 10u) {
			return -1;
		}
		uPay = b126_rd64be(buf + 2);
		/* RFC 6455: MSB of 64-bit length MUST be 0. */
		if ((uPay & 0x8000000000000000ULL) != 0u) {
			return -1;
		}
		/* Minimal encoding: 64-bit form only for len > 65535. */
		if (uPay <= 0xffffu) {
			return -1;
		}
		cbHdr = 10u;
	}

	if (bMasked) {
		cbHdr += 4u;
	}
	if (len < cbHdr) {
		return -1;
	}

	*fin = (b0 & 0x80u) != 0 ? 1 : 0;
	*opcode = (int)(b0 & 0x0fu);
	*masked = bMasked ? 1 : 0;
	*payload_len = uPay;
	*hdr_len = cbHdr;
	return 0;
}

/* ---- build (unmasked) -------------------------------------------------- */

/*
 * Encode an unmasked header. opcode is masked to 4 bits; fin is boolean.
 * RSV bits are left clear.
 */
int
gj_ws_header_build(unsigned char *out, size_t outcap, int fin, int opcode,
    uint64_t payload_len, size_t *hdr_len)
{
	size_t cbHdr;
	unsigned char b0;
	unsigned char b1;

	if (out == NULL || hdr_len == NULL) {
		return -1;
	}
	/* 64-bit length MSB must be 0 (RFC 6455 §5.2). */
	if ((payload_len & 0x8000000000000000ULL) != 0u) {
		return -1;
	}

	if (payload_len <= 125u) {
		cbHdr = 2u;
	} else if (payload_len <= 0xffffu) {
		cbHdr = 4u;
	} else {
		cbHdr = 10u;
	}

	if (outcap < cbHdr) {
		return -1;
	}

	b0 = (unsigned char)((fin ? 0x80u : 0u) | ((unsigned)opcode & 0x0fu));
	if (payload_len <= 125u) {
		b1 = (unsigned char)payload_len;
		out[0] = b0;
		out[1] = b1;
	} else if (payload_len <= 0xffffu) {
		b1 = 126u;
		out[0] = b0;
		out[1] = b1;
		b126_wr16be(out + 2, (uint16_t)payload_len);
	} else {
		b1 = 127u;
		out[0] = b0;
		out[1] = b1;
		b126_wr64be(out + 2, payload_len);
	}

	*hdr_len = cbHdr;
	return 0;
}

/* ---- masking ----------------------------------------------------------- */

/*
 * XOR data[0..len) with mask[(i % 4)]. Masking is its own inverse.
 */
void
gj_ws_mask(unsigned char *data, size_t len, const unsigned char mask[4])
{
	size_t i;

	if (data == NULL || mask == NULL || len == 0u) {
		return;
	}

	i = 0u;
	/* Unrolled 4-byte groups when aligned to mask period. */
	while (i + 4u <= len) {
		data[i] ^= mask[0];
		data[i + 1u] ^= mask[1];
		data[i + 2u] ^= mask[2];
		data[i + 3u] ^= mask[3];
		i += 4u;
	}
	while (i < len) {
		data[i] ^= mask[i & 3u];
		i++;
	}
}

/* ---- underscored aliases ----------------------------------------------- */

int __gj_ws_header_parse(const unsigned char *buf, size_t len, int *fin,
    int *opcode, int *masked, uint64_t *payload_len, size_t *hdr_len)
    __attribute__((alias("gj_ws_header_parse")));
int __gj_ws_header_build(unsigned char *out, size_t outcap, int fin,
    int opcode, uint64_t payload_len, size_t *hdr_len)
    __attribute__((alias("gj_ws_header_build")));
void __gj_ws_mask(unsigned char *data, size_t len,
    const unsigned char mask[4])
    __attribute__((alias("gj_ws_mask")));
