/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch723: SLIP (RFC 1055) packet encode.
 *
 * Surface (unique symbols):
 *   int gj_slip_encode(const unsigned char *in, size_t n,
 *                      unsigned char *out, size_t cap);
 *     — Encode n octets at in into a SLIP frame at out[0..cap).
 *       Frame form: END || stuffed(in[0..n)) || END.
 *       Stuffing (RFC 1055):
 *         END (0xC0) → ESC (0xDB) ESC_END (0xDC)
 *         ESC (0xDB) → ESC (0xDB) ESC_ESC (0xDD)
 *         other      → raw byte
 *       Returns 0 on success, -1 on error (NULL out, NULL in with n > 0,
 *       or cap too small for the framed output).
 *       Empty payload (n == 0, in may be NULL) writes END END when
 *       cap >= 2.
 *   int __gj_slip_encode  (alias)
 *   __libcgj_batch723_marker = "libcgj-batch723"
 *
 * Distinct from gj_cobs_encode (batch285, no frame delimiter) and from
 * SLIP-0039 RS1024 helpers (batch300). Unique gj_slip_encode names only.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch723_marker[] = "libcgj-batch723";

/* RFC 1055 SLIP special octets. */
#define B723_END     0xC0u
#define B723_ESC     0xDBu
#define B723_ESC_END 0xDCu
#define B723_ESC_ESC 0xDDu

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slip_encode — RFC 1055 SLIP frame encode with leading and trailing END.
 *
 * in:  input payload (NULL only legal when n == 0)
 * n:   payload length in bytes
 * out: destination buffer (must be non-NULL)
 * cap: capacity of out in bytes
 *
 * Worst-case size: 2 + 2*n (both ENDs plus every payload byte stuffed).
 * On success out holds a complete frame; on failure out is undefined.
 *
 * Edge cases:
 *   out == NULL                → -1
 *   in == NULL && n != 0       → -1
 *   n == 0 with cap >= 2       → out = {END, END}, return 0
 *   cap too small for frame    → -1
 */
int
gj_slip_encode(const unsigned char *in, size_t n, unsigned char *out,
               size_t cap)
{
	size_t i;
	size_t iOut;

	if (out == NULL) {
		return -1;
	}
	if (in == NULL && n != 0u) {
		return -1;
	}

	/* Leading END. */
	if (cap < 1u) {
		return -1;
	}
	out[0] = (unsigned char)B723_END;
	iOut = 1u;

	for (i = 0u; i < n; i++) {
		unsigned char uByte = in[i];

		if (uByte == (unsigned char)B723_END) {
			if (iOut + 2u > cap) {
				return -1;
			}
			out[iOut++] = (unsigned char)B723_ESC;
			out[iOut++] = (unsigned char)B723_ESC_END;
		} else if (uByte == (unsigned char)B723_ESC) {
			if (iOut + 2u > cap) {
				return -1;
			}
			out[iOut++] = (unsigned char)B723_ESC;
			out[iOut++] = (unsigned char)B723_ESC_ESC;
		} else {
			if (iOut + 1u > cap) {
				return -1;
			}
			out[iOut++] = uByte;
		}
	}

	/* Trailing END. */
	if (iOut + 1u > cap) {
		return -1;
	}
	out[iOut] = (unsigned char)B723_END;
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_slip_encode(const unsigned char *in, size_t n, unsigned char *out,
                     size_t cap)
    __attribute__((alias("gj_slip_encode")));
