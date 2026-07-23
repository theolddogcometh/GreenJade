/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch271: JSON string-body escape into a caller buffer.
 *
 * Surface (unique symbols — does NOT redefine batch77 json_escape):
 *   int gj_json_escape(const char *in, char *out, size_t out_cap);
 *     — Escape a NUL-terminated C string into a JSON string body (no
 *       surrounding quotes) written into out. Result is NUL-terminated.
 *       Returns bytes written excluding the trailing NUL, or -1 on error.
 *       Escapes:
 *         '"'  → \"
 *         '\\' → \\
 *         control (0x00..0x1F) → \u00XX  (lowercase hex)
 *       All other bytes (including UTF-8 tails) are copied unchanged.
 *   __gj_json_escape  (alias)
 *   __libcgj_batch271_marker = "libcgj-batch271"
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno,
 * no libc string calls. No third-party source copied. Distinct from
 * graph_batch77 json_escape (ssize_t / short \n forms / errno).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch271_marker[] = "libcgj-batch271";

/* ---- freestanding helpers ---------------------------------------------- */

/* Map nibble 0..15 → lowercase hex ASCII. */
static char
b271_hex_digit(unsigned uNib)
{
	return (char)((uNib < 10u) ? ('0' + (char)uNib) : ('a' + (char)(uNib - 10u)));
}

/*
 * b271_escaped_len — output byte count for one input byte (excl. NUL).
 * '"' / '\\' → 2; control → 6 (\u00XX); else → 1.
 */
static size_t
b271_escaped_len(unsigned char uch)
{
	if (uch == (unsigned char)'"' || uch == (unsigned char)'\\') {
		return 2u;
	}
	if (uch < 0x20u) {
		return 6u;
	}
	return 1u;
}

/*
 * b271_putc — append one byte if room remains for this byte + final NUL.
 * *pw is the current write index. Returns 0 on success, -1 if short.
 */
static int
b271_putc(char *out, size_t out_cap, size_t *pw, char ch)
{
	if (*pw + 1u >= out_cap) {
		return -1;
	}
	out[(*pw)++] = ch;
	return 0;
}

/*
 * b271_put_escape — write the escaped form of uch into out.
 * Returns 0 on success, -1 if the buffer cannot hold the escape + NUL.
 */
static int
b271_put_escape(char *out, size_t out_cap, size_t *pw, unsigned char uch)
{
	size_t nNeed = b271_escaped_len(uch);

	if (*pw + nNeed + 1u > out_cap) {
		return -1;
	}
	if (uch == (unsigned char)'"' || uch == (unsigned char)'\\') {
		if (b271_putc(out, out_cap, pw, '\\') != 0) {
			return -1;
		}
		if (b271_putc(out, out_cap, pw, (char)uch) != 0) {
			return -1;
		}
		return 0;
	}
	if (uch < 0x20u) {
		if (b271_putc(out, out_cap, pw, '\\') != 0) {
			return -1;
		}
		if (b271_putc(out, out_cap, pw, 'u') != 0) {
			return -1;
		}
		if (b271_putc(out, out_cap, pw, '0') != 0) {
			return -1;
		}
		if (b271_putc(out, out_cap, pw, '0') != 0) {
			return -1;
		}
		if (b271_putc(out, out_cap, pw, b271_hex_digit((uch >> 4) & 0xfu)) != 0) {
			return -1;
		}
		if (b271_putc(out, out_cap, pw, b271_hex_digit(uch & 0xfu)) != 0) {
			return -1;
		}
		return 0;
	}
	return b271_putc(out, out_cap, pw, (char)uch);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_json_escape — escape C string to JSON string body in caller buffer.
 *
 * in:      NUL-terminated input (required).
 * out:     destination buffer (required).
 * out_cap: capacity of out in bytes (must fit result + NUL).
 *
 * Returns the number of bytes written to out excluding the trailing NUL,
 * or -1 on error:
 *   in == NULL or out == NULL
 *   out_cap == 0 (cannot store NUL)
 *   escaped form does not fit in out_cap (including trailing NUL)
 *
 * On success out is always NUL-terminated. On failure out contents are
 * undefined (partial writes may have occurred).
 *
 * Note: empty input writes a single NUL and returns 0 when out_cap >= 1.
 * Control characters use \u00XX only (no short \n / \t forms).
 */
int
gj_json_escape(const char *in, char *out, size_t out_cap)
{
	size_t w;
	size_t i;

	if (in == NULL || out == NULL || out_cap == 0u) {
		return -1;
	}

	w = 0u;
	for (i = 0u; in[i] != '\0'; i++) {
		unsigned char uch = (unsigned char)in[i];

		if (b271_put_escape(out, out_cap, &w, uch) != 0) {
			return -1;
		}
	}

	/* Trailing NUL: need out_cap > w. */
	if (w >= out_cap) {
		return -1;
	}
	out[w] = '\0';
	return (int)w;
}

int __gj_json_escape(const char *in, char *out, size_t out_cap)
    __attribute__((alias("gj_json_escape")));
