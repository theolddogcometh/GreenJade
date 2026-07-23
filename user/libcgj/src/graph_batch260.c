/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch260: Punycode encode lite (RFC 3492 core).
 *
 * Surface (unique symbols):
 *   int gj_puny_encode(const uint32_t *cps, size_t ncps, char *out, size_t out_cap);
 *     — RFC 3492 §6.3 encode of ncps Unicode code points (ASCII + BMP
 *       intended; any CP < 0x80 is basic). Max ncps is 64. Basic CPs emit
 *       first, then '-' if any basic, then generalized variable-length
 *       integers for non-basic deltas. Returns encoded length excluding
 *       NUL on success, -1 on error (bad args, overflow, or short cap).
 *   __gj_puny_encode  (alias)
 *   __libcgj_batch260_marker = "libcgj-batch260"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party Punycode source copied. Distinct from batch187.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch260_marker[] = "libcgj-batch260";

/* RFC 3492 bootstring parameters. */
#define B260_BASE   36u
#define B260_TMIN   1u
#define B260_TMAX   26u
#define B260_SKEW   38u
#define B260_DAMP   700u
#define B260_IBIAS  72u
#define B260_N0     0x80u

/* Lite IDNA boot: bounded label length in code points. */
#define B260_MAX_CPS 64u

/* ---- freestanding helpers ---------------------------------------------- */

/* Map digit 0..35 → a-z then 0-9 (RFC 3492 encode alphabet). */
static int
b260_digit(unsigned d)
{
	return (d < 26u) ? (int)('a' + d) : (int)('0' + (d - 26u));
}

/*
 * RFC 3492 §6.1 adapt.
 * first: use DAMP on first call after basic block, else halve delta.
 */
static unsigned
b260_adapt(unsigned delta, unsigned npoints, int first)
{
	unsigned k;

	delta = first ? (delta / B260_DAMP) : (delta >> 1);
	delta += delta / npoints;
	k = 0u;
	while (delta > ((B260_BASE - B260_TMIN) * B260_TMAX) / 2u) {
		delta /= (B260_BASE - B260_TMIN);
		k += B260_BASE;
	}
	return k + (((B260_BASE - B260_TMIN + 1u) * delta) / (delta + B260_SKEW));
}

/* Append one char; leave room for eventual NUL. Returns 0 or -1. */
static int
b260_putc(char *out, size_t cap, size_t *po, int ch)
{
	if (*po + 1u >= cap) {
		return -1;
	}
	out[(*po)++] = (char)ch;
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_puny_encode — RFC 3492 core Punycode encode (lite, max 64 CPs).
 *
 * cps[0..ncps) are Unicode scalar values (ASCII basic + non-basic BMP).
 * Empty input (ncps==0) yields empty string when out/cap allow.
 * cps may be NULL only when ncps==0.
 */
int
gj_puny_encode(const uint32_t *cps, size_t ncps, char *out, size_t out_cap)
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

	if (out == NULL || out_cap == 0u) {
		return -1;
	}
	if (ncps > 0u && cps == NULL) {
		return -1;
	}
	if (ncps > B260_MAX_CPS) {
		return -1;
	}

	o = 0u;
	b = 0u;

	/* Emit basic code points (U+0000..U+007F). */
	for (i = 0u; i < ncps; i++) {
		c = cps[i];
		if (c < 0x80u) {
			if (b260_putc(out, out_cap, &o, (int)c) != 0) {
				return -1;
			}
			b++;
		}
	}
	h = b;

	/* Delimiter only when at least one basic CP was present. */
	if (b > 0u) {
		if (b260_putc(out, out_cap, &o, '-') != 0) {
			return -1;
		}
	}

	bias = B260_IBIAS;
	delta = 0u;
	nn = B260_N0;

	while (h < ncps) {
		/* m = minimum non-basic code point >= nn */
		m = 0xffffffffu;
		for (i = 0u; i < ncps; i++) {
			c = cps[i];
			if (c >= nn && c < m) {
				m = c;
			}
		}
		if (m == 0xffffffffu) {
			return -1;
		}

		/* delta += (m - nn) * (h + 1); overflow-safe */
		{
			uint32_t step = m - nn;
			uint32_t mul = (uint32_t)(h + 1u);

			if (step != 0u && step > (0xffffffffu - delta) / mul) {
				return -1;
			}
			delta += step * mul;
		}
		nn = m;

		for (i = 0u; i < ncps; i++) {
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

				for (k = B260_BASE;; k += B260_BASE) {
					unsigned t;
					unsigned rest;
					unsigned digit;

					if (k <= bias) {
						t = B260_TMIN;
					} else if (k >= bias + B260_TMAX) {
						t = B260_TMAX;
					} else {
						t = k - bias;
					}
					if (q < t) {
						break;
					}
					rest = B260_BASE - t;
					digit = t + ((q - t) % rest);
					if (b260_putc(out, out_cap, &o, b260_digit(digit)) != 0) {
						return -1;
					}
					q = (q - t) / rest;
				}
				if (b260_putc(out, out_cap, &o, b260_digit(q)) != 0) {
					return -1;
				}
			}

			bias = b260_adapt(delta, (unsigned)(h + 1u), h == b);
			delta = 0u;
			h++;
		}

		if (delta == 0xffffffffu) {
			return -1;
		}
		delta++;
		nn++;
	}

	if (o >= out_cap) {
		return -1;
	}
	out[o] = '\0';
	return (int)o;
}

int __gj_puny_encode(const uint32_t *cps, size_t ncps, char *out, size_t out_cap)
    __attribute__((alias("gj_puny_encode")));
