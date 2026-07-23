/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch199: HTTP/2 frame header parse (RFC 7540 §4.1).
 * Clean-room freestanding pure C from the public wire layout — fixed
 * 9-byte header:
 *   Length (24) | Type (8) | Flags (8) | R (1) | Stream Identifier (31)
 * All multi-byte fields are network (big-endian). Builds under userspace
 * -msse2. No third-party HTTP/2 source was copied.
 *
 * Surface (unique symbols):
 *   int gj_h2_frame_header(const unsigned char *in, size_t len,
 *                          unsigned *length, unsigned *type,
 *                          unsigned *flags, unsigned *stream_id);
 *     Parse a 9-byte HTTP/2 frame header. length = payload length
 *     (24-bit), type/flags = 8-bit fields, stream_id = 31-bit stream
 *     identifier with the reserved high bit cleared. Returns 0 on
 *     success, -1 on short/NULL input.
 *   __gj_h2_frame_header  (alias)
 *   __libcgj_batch199_marker = "libcgj-batch199"
 *
 * Note: this helper only decodes the header wire fields. It does not
 * enforce frame-type ranges, flag semantics, max frame size (SETTINGS),
 * or stream-id parity rules — callers apply policy.
 *
 * Existing HTTP/2 surface NOT redefined (avoid multi-def):
 *   gj_hpack_*  → graph_batch135.c  (HPACK integer coding)
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch199_marker[] = "libcgj-batch199";

/* Fixed HTTP/2 frame header size (RFC 7540 §4.1). */
#define B199_HDR_LEN 9u

/* Stream identifier is 31 bits; reserved high bit is ignored on receive. */
#define B199_STREAM_MASK 0x7fffffffu

/* ---- gj_h2_frame_header ------------------------------------------------ */

/*
 * Decode an HTTP/2 frame header from in[0..len). All out pointers are
 * required. length is the 24-bit payload length (not including the
 * 9-byte header). stream_id has bit 31 cleared (R bit ignored).
 */
int
gj_h2_frame_header(const unsigned char *pIn, size_t cbLen,
                   unsigned *puLength, unsigned *puType,
                   unsigned *puFlags, unsigned *puStreamId)
{
	unsigned uLength;
	unsigned uType;
	unsigned uFlags;
	unsigned uStreamId;

	if (pIn == NULL || puLength == NULL || puType == NULL ||
	    puFlags == NULL || puStreamId == NULL) {
		return -1;
	}
	if (cbLen < B199_HDR_LEN) {
		return -1;
	}

	/* Length: 24-bit big-endian at octets 0..2. */
	uLength = ((unsigned)pIn[0] << 16) |
	          ((unsigned)pIn[1] << 8) |
	          (unsigned)pIn[2];
	uType = (unsigned)pIn[3];
	uFlags = (unsigned)pIn[4];
	/* Stream Identifier: 31-bit big-endian at octets 5..8; R bit cleared. */
	uStreamId = ((unsigned)pIn[5] << 24) |
	            ((unsigned)pIn[6] << 16) |
	            ((unsigned)pIn[7] << 8) |
	            (unsigned)pIn[8];
	uStreamId &= B199_STREAM_MASK;

	*puLength = uLength;
	*puType = uType;
	*puFlags = uFlags;
	*puStreamId = uStreamId;
	return 0;
}

int __gj_h2_frame_header(const unsigned char *in, size_t len,
                         unsigned *length, unsigned *type,
                         unsigned *flags, unsigned *stream_id)
    __attribute__((alias("gj_h2_frame_header")));
