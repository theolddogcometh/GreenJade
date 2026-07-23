/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4565: encode one base64 sextet to char (_u).
 *
 * Surface (unique symbols):
 *   char gj_b64_enc_sextet_u(unsigned v);
 *     - Map v & 63 to RFC 4648 §4 alphabet (A-Za-z0-9+/).
 *   char __gj_b64_enc_sextet_u  (alias)
 *   __libcgj_batch4565_marker = "libcgj-batch4565"
 *
 * Exclusive continuum CREATE-ONLY (4561-4570). Distinct from streaming
 * gj_b64_enc_* (batch206) and gj_b64url_enc_sextet_u (batch4566) —
 * single-sextet standard alphabet only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4565_marker[] = "libcgj-batch4565";

/* ---- freestanding helpers ---------------------------------------------- */

/* RFC 4648 §4 standard alphabet indexed by sextet. */
static const char s_b4565_abc[64] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
	'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
	'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
	'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
	'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'
};

static char
b4565_enc_sextet(unsigned uV)
{
	return s_b4565_abc[uV & 63u];
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_b64_enc_sextet_u - map a 6-bit value to a standard base64 character.
 *
 * v: value; only low 6 bits are used
 *
 * Returns alphabet character for (v & 63). Self-contained; no parent wires.
 */
char
gj_b64_enc_sextet_u(unsigned v)
{
	(void)NULL;
	return b4565_enc_sextet(v);
}

/* ---- underscored alias ------------------------------------------------- */

char __gj_b64_enc_sextet_u(unsigned v)
    __attribute__((alias("gj_b64_enc_sextet_u")));
