/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1135: JSON string escape expanded length.
 *
 * Surface (unique symbols):
 *   size_t gj_json_escape_len(const char *s, size_t n);
 *     — Return the number of octets needed to JSON-escape the n-byte
 *       sequence s (without surrounding quotes). Escapes:
 *         " \ → 2 each; control 0x00..0x1f → 6 (\u00XX); else 1.
 *       NULL s and n > 0 → 0. n == 0 → 0 (empty input).
 *   size_t __gj_json_escape_len  (alias)
 *   __libcgj_batch1135_marker = "libcgj-batch1135"
 *
 * Distinct from full JSON encoders — unique name; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1135_marker[] = "libcgj-batch1135";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_escape_len — expanded length after JSON string escaping.
 */
size_t
gj_json_escape_len(const char *sz, size_t cN)
{
	size_t cbOut;
	size_t i;
	unsigned char uCh;

	if (cN == 0u) {
		return 0u;
	}
	if (sz == NULL) {
		return 0u;
	}

	cbOut = 0u;
	for (i = 0u; i < cN; i++) {
		uCh = (unsigned char)sz[i];
		if (uCh == '"' || uCh == '\\' ||
		    uCh == '\b' || uCh == '\f' ||
		    uCh == '\n' || uCh == '\r' || uCh == '\t') {
			cbOut += 2u; /* \X short form */
		} else if (uCh < 0x20u) {
			cbOut += 6u; /* \u00XX */
		} else {
			cbOut += 1u;
		}
	}
	return cbOut;
}

/* ---- underscored alias ------------------------------------------------- */

size_t __gj_json_escape_len(const char *sz, size_t cN)
    __attribute__((alias("gj_json_escape_len")));
