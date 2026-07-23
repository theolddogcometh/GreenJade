/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch169: URL/path slugify helper.
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Builds under userspace -msse2; this TU does not require SSE.
 * No third-party slugify source was copied.
 *
 * Surface (unique symbols):
 *   int gj_slugify(const char *in, char *out, size_t cap);
 *     Transform in into a URL-safe slug written into out (capacity cap,
 *     including the trailing NUL). Rules:
 *       1. ASCII A–Z → lowercase a–z
 *       2. non-alphanumeric octets → '-'
 *       3. collapse consecutive '-' into a single '-'
 *       4. strip leading and trailing '-'
 *     Empty / all-separator input yields "".
 *     Returns 0 on success, -1 on error (NULL args, cap == 0, or the
 *     result including NUL does not fit in cap). On error out is undefined.
 *   __gj_slugify  (alias)
 *   __libcgj_batch169_marker = "libcgj-batch169"
 *
 * No errno (freestanding-safe); callers use return codes only.
 */

#include <stddef.h>

const char __libcgj_batch169_marker[] = "libcgj-batch169";

/* ---- freestanding character class -------------------------------------- */

static int
b169_is_upper(unsigned char ch)
{
	return ch >= (unsigned char)'A' && ch <= (unsigned char)'Z';
}

static int
b169_is_alnum(unsigned char ch)
{
	if (ch >= (unsigned char)'0' && ch <= (unsigned char)'9') {
		return 1;
	}
	if (ch >= (unsigned char)'a' && ch <= (unsigned char)'z') {
		return 1;
	}
	if (b169_is_upper(ch)) {
		return 1;
	}
	return 0;
}

static unsigned char
b169_to_lower(unsigned char ch)
{
	if (b169_is_upper(ch)) {
		return (unsigned char)(ch - (unsigned char)'A' +
		    (unsigned char)'a');
	}
	return ch;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_slugify — lower, map non-alnum to '-', collapse runs, trim edges.
 *
 * Single pass: separators before the first alnum are ignored; a pending
 * dash is emitted only when the next alnum arrives after content has
 * already been written (so runs collapse and trailing dashes never land).
 *
 * Writes a NUL-terminated slug into out[0..cap-1]. Returns 0 / -1.
 */
int
gj_slugify(const char *szIn, char *szOut, size_t cbCap)
{
	const char *pSrc;
	size_t iOut;
	unsigned char ch;
	int fHaveContent;
	int fPendingDash;

	if (szIn == NULL || szOut == NULL || cbCap == 0u) {
		return -1;
	}

	pSrc = szIn;
	iOut = 0u;
	fHaveContent = 0;
	fPendingDash = 0;

	while (*pSrc != '\0') {
		ch = (unsigned char)*pSrc;
		pSrc++;

		if (b169_is_alnum(ch)) {
			if (fHaveContent && fPendingDash) {
				if (iOut + 1u >= cbCap) {
					return -1;
				}
				szOut[iOut] = '-';
				iOut++;
				fPendingDash = 0;
			}
			if (iOut + 1u >= cbCap) {
				return -1;
			}
			szOut[iOut] = (char)b169_to_lower(ch);
			iOut++;
			fHaveContent = 1;
			fPendingDash = 0;
		} else {
			/* non-alnum: become a pending separator only after
			 * at least one alnum has been emitted (leading
			 * separators are dropped; runs stay collapsed). */
			if (fHaveContent) {
				fPendingDash = 1;
			}
		}
	}

	/* trailing pending dash is never emitted */
	if (iOut >= cbCap) {
		return -1;
	}
	szOut[iOut] = '\0';
	return 0;
}

int __gj_slugify(const char *szIn, char *szOut, size_t cbCap)
    __attribute__((alias("gj_slugify")));
