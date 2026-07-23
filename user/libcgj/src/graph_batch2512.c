/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2512: JSON string-body escape expanded length (u).
 *
 * Surface (unique symbols):
 *   size_t gj_json_escape_len_u(const char *s, size_t n);
 *     - Return octets needed to JSON-escape s[0..n) as a string body
 *       (no surrounding quotes). Escapes (uniform control form):
 *         '"' / '\\' → 2 each
 *         control 0x00..0x1f → 6 (\u00XX only; no short \n/\t forms)
 *         all other octets → 1
 *       NULL s with n > 0 → 0. n == 0 → 0.
 *   size_t __gj_json_escape_len_u  (alias)
 *   __libcgj_batch2512_marker = "libcgj-batch2512"
 *
 * JSON exclusive lightweight wave (2511-2520). Distinct from
 * gj_json_escape_len (batch1135 short-control forms) and
 * gj_str_json_escape_len (batch2073) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2512_marker[] = "libcgj-batch2512";

/* ---- freestanding helpers ---------------------------------------------- */

/* Output byte count for one input octet (uniform \u00XX controls). */
static size_t
b2512_esc_len(unsigned char u8C)
{
	if (u8C == (unsigned char)'"' || u8C == (unsigned char)'\\') {
		return 2u;
	}
	if (u8C < 0x20u) {
		return 6u; /* \u00XX */
	}
	return 1u;
}

static size_t
b2512_escape_len(const char *sz, size_t cN)
{
	size_t cbOut;
	size_t i;

	if (sz == NULL || cN == 0u) {
		return 0u;
	}
	cbOut = 0u;
	for (i = 0u; i < cN; i++) {
		cbOut += b2512_esc_len((unsigned char)sz[i]);
	}
	return cbOut;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_escape_len_u - expanded length after JSON string escaping.
 *
 * s: input bytes (NULL with n > 0 → 0)
 * n: byte count to measure
 *
 * Binary-safe; result excludes surrounding quotes and a trailing NUL.
 * Control octets always expand to \u00XX (6), matching batch271 style.
 */
size_t
gj_json_escape_len_u(const char *sz, size_t cN)
{
	(void)NULL;
	return b2512_escape_len(sz, cN);
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_json_escape_len_u(const char *sz, size_t cN)
    __attribute__((alias("gj_json_escape_len_u")));
