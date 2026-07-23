/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch901: bounded n-char string copy into out.
 *
 * Surface (unique symbols):
 *   int gj_str_dup_n(char *out, size_t out_cap, const char *src, size_t n);
 *     — Copy up to n characters from src into out and always write a
 *       terminating NUL when out_cap > 0. Stops early at src NUL.
 *       Returns 0 on success, -1 on bad args or insufficient capacity
 *       (need min(n, src_len) + 1 bytes in out).
 *   int __gj_str_dup_n  (alias)
 *   __libcgj_batch901_marker = "libcgj-batch901"
 *
 * Does NOT redefine gj_str_cpy / strncpy / strlcpy — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch901_marker[] = "libcgj-batch901";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_str_dup_n — copy up to n chars of src into out + NUL.
 *
 * out:     destination buffer
 * out_cap: capacity of out including room for the terminating NUL
 * src:     NUL-terminated source (may be empty)
 * n:       maximum payload characters to copy (not counting NUL)
 *
 * Returns 0 when the copy (including NUL) fits and is written.
 * Returns -1 if out or src is NULL, out_cap is 0, or out_cap is too
 * small for the resulting string (payload length + 1).
 */
int
gj_str_dup_n(char *out, size_t out_cap, const char *src, size_t n)
{
	size_t i;

	if (out == NULL || src == NULL || out_cap == 0u) {
		return -1;
	}

	i = 0u;
	while (i < n && src[i] != '\0') {
		if ((i + 1u) >= out_cap) {
			/* Not enough room for this char + final NUL. */
			return -1;
		}
		out[i] = src[i];
		i++;
	}

	if (i >= out_cap) {
		return -1;
	}
	out[i] = '\0';
	return 0;
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_str_dup_n(char *out, size_t out_cap, const char *src, size_t n)
    __attribute__((alias("gj_str_dup_n")));
