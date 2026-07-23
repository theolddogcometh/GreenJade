/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch187: Punycode encode (RFC 3492) + IDNA label lite.
 * Clean-room freestanding pure C — integer/pointer only (no SSE required;
 * builds under userspace -msse2). No third-party Punycode source was copied.
 *
 * Surface (unique symbols):
 *   int gj_punycode_encode(const uint32_t *cps, size_t n, char *out, size_t cap);
 *     — RFC 3492 §6.3 encode of n Unicode code points into out (NUL-term).
 *       Basic CPs (< 0x80) emit first, then '-' if any basic, then deltas.
 *       Returns encoded length (excluding NUL) on success, -1 on error
 *       (NULL args with n>0, overflow, or insufficient cap).
 *
 *   int gj_idna_to_ascii_label(const char *utf8, char *out, size_t cap);
 *     — Lite IDNA ToASCII for a single label: ASCII-only passthrough.
 *       Accepts LDH / underscore labels (A-Za-z0-9_-) and empty string.
 *       Rejects NUL mid-string is N/A (C string); rejects any byte >= 0x80
 *       or control chars / spaces / dots. Does not add "xn--" (no non-ASCII).
 *       Returns length excluding NUL, or -1 on error.
 *
 *   __gj_punycode_encode / __gj_idna_to_ascii_label  (aliases)
 *   __libcgj_batch187_marker = "libcgj-batch187"
 *
 * No malloc, no errno (freestanding-safe).
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch187_marker[] = "libcgj-batch187";

/* RFC 3492 bootstring parameters. */
#define B187_BASE     36u
#define B187_TMIN     1u
#define B187_TMAX     26u
#define B187_SKEW     38u
#define B187_DAMP     700u
#define B187_IBIAS    72u
#define B187_N0       0x80u

/* DNS label max octets; also a sane cap for stack-free encode loops. */
#define B187_MAX_CP   256u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b187_digit(unsigned d)
{
	/* 0..25 → a-z, 26..35 → 0-9 */
	return (d < 26u) ? (int)('a' + d) : (int)('0' + (d - 26u));
}

/*
 * RFC 3492 §6.1 adapt: damp/halve delta, then fold until within range.
 */
static unsigned
b187_adapt(unsigned delta, unsigned npoints, int first)
{
	unsigned k;

	delta = first ? (delta / B187_DAMP) : (delta >> 1);
	delta += delta / npoints;
	k = 0u;
	while (delta > ((B187_BASE - B187_TMIN) * B187_TMAX) / 2u) {
		delta /= (B187_BASE - B187_TMIN);
		k += B187_BASE;
	}
	return k + (((B187_BASE - B187_TMIN + 1u) * delta) / (delta + B187_SKEW));
}

/* Append one char; returns 0 or -1 if no room for char + eventual NUL. */
static int
b187_putc(char *out, size_t cap, size_t *po, int ch)
{
	if (*po + 1u >= cap) {
		return -1;
	}
	out[(*po)++] = (char)ch;
	return 0;
}

/* ---- gj_punycode_encode ------------------------------------------------ */

/*
 * Encode code points cps[0..n) to Punycode into out[0..cap).
 * Empty input (n==0) yields empty string if out/cap allow. cps may be NULL
 * only when n==0.
 */
int
gj_punycode_encode(const uint32_t *cps, size_t n, char *out, size_t cap)
{
	size_t o;
	size_t b;
	size_t h;
	size_t i;
	unsigned bias;
	unsigned delta;
	uint32_t nn;
	uint32_t m;
	uint32_t c;

	if (out == NULL || cap == 0u) {
		return -1;
	}
	if (n > 0u && cps == NULL) {
		return -1;
	}
	if (n > B187_MAX_CP) {
		return -1;
	}

	o = 0u;
	b = 0u;

	/* Emit basic code points (U+0000..U+007F). */
	for (i = 0u; i < n; i++) {
		c = cps[i];
		if (c < 0x80u) {
			if (b187_putc(out, cap, &o, (int)c) != 0) {
				return -1;
			}
			b++;
		}
	}
	h = b;

	/* Delimiter only when at least one basic CP was present. */
	if (b > 0u) {
		if (b187_putc(out, cap, &o, '-') != 0) {
			return -1;
		}
	}

	bias = B187_IBIAS;
	delta = 0u;
	nn = B187_N0;

	while (h < n) {
		/* m = minimum non-basic code point >= nn */
		m = 0xffffffffu;
		for (i = 0u; i < n; i++) {
			c = cps[i];
			if (c >= nn && c < m) {
				m = c;
			}
		}
		if (m == 0xffffffffu) {
			return -1;
		}

		/* delta += (m - nn) * (h + 1); overflow-safe check */
		{
			uint32_t step = m - nn;
			uint32_t mul = (uint32_t)(h + 1u);

			if (step != 0u && step > (0xffffffffu - delta) / mul) {
				return -1;
			}
			delta += step * mul;
		}
		nn = m;

		for (i = 0u; i < n; i++) {
			c = cps[i];
			if (c < nn) {
				if (delta == 0xffffffffu) {
					return -1;
				}
				delta++;
				continue;
			}
			if (c != nn) {
				continue;
			}

			/* Encode delta as variable-length integer. */
			{
				unsigned q = delta;
				unsigned k;

				for (k = B187_BASE;; k += B187_BASE) {
					unsigned t;
					unsigned rest;
					unsigned digit;

					if (k <= bias) {
						t = B187_TMIN;
					} else if (k >= bias + B187_TMAX) {
						t = B187_TMAX;
					} else {
						t = k - bias;
					}
					if (q < t) {
						break;
					}
					rest = B187_BASE - t;
					digit = t + ((q - t) % rest);
					if (b187_putc(out, cap, &o, b187_digit(digit)) != 0) {
						return -1;
					}
					q = (q - t) / rest;
				}
				if (b187_putc(out, cap, &o, b187_digit(q)) != 0) {
					return -1;
				}
			}

			bias = b187_adapt(delta, (unsigned)(h + 1u), h == b);
			delta = 0u;
			h++;
		}

		if (delta == 0xffffffffu) {
			return -1;
		}
		delta++;
		nn++;
	}

	/* Pure-basic inputs get a trailing '-' from the delimiter rule above
	 * (RFC 3492). Keep it — decoders tolerate / expect the form. */
	if (o >= cap) {
		return -1;
	}
	out[o] = '\0';
	return (int)o;
}

int __gj_punycode_encode(const uint32_t *cps, size_t n, char *out, size_t cap)
    __attribute__((alias("gj_punycode_encode")));

/* ---- gj_idna_to_ascii_label -------------------------------------------- */

/*
 * ASCII-only label passthrough for IDNA ToASCII lite.
 * Valid: empty, or [A-Za-z0-9_-]+ (no dots — single label).
 */
int
gj_idna_to_ascii_label(const char *utf8, char *out, size_t cap)
{
	size_t i;
	unsigned char ch;

	if (utf8 == NULL || out == NULL || cap == 0u) {
		return -1;
	}

	for (i = 0u;; i++) {
		ch = (unsigned char)utf8[i];
		if (ch == 0u) {
			break;
		}
		if (ch >= 0x80u) {
			return -1; /* non-ASCII: not handled by this lite stub */
		}
		if (!((ch >= 'A' && ch <= 'Z') ||
		      (ch >= 'a' && ch <= 'z') ||
		      (ch >= '0' && ch <= '9') ||
		      ch == '-' || ch == '_')) {
			return -1;
		}
		if (i + 1u >= cap) {
			return -1;
		}
		out[i] = (char)ch;
	}

	if (i >= cap) {
		return -1;
	}
	out[i] = '\0';
	return (int)i;
}

int __gj_idna_to_ascii_label(const char *utf8, char *out, size_t cap)
    __attribute__((alias("gj_idna_to_ascii_label")));
