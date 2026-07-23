/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1958: Flatpak ref soft-ok hint.
 *
 * Surface (unique symbols):
 *   int gj_flatpak_ref_ok_hint(const char *ref);
 *     — Soft validity hint for a Flatpak-style ref string of the form
 *       kind/id/arch/branch (exactly three '/' separators, four
 *       non-empty segments). Each segment character must be A-Za-z0-9,
 *       '-', '_', or '.'. Length 1..255 total. Pure shape data check;
 *       does not contact a remote or parse ostree commits.
 *   int __gj_flatpak_ref_ok_hint  (alias)
 *   __libcgj_batch1958_marker = "libcgj-batch1958"
 *
 * Package/install exclusive pure-data wave (1951–1960). Clean-room
 * freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>

const char __libcgj_batch1958_marker[] = "libcgj-batch1958";

/* Soft max total ref length. */
#define B1958_REF_MAX  255u

/* Required number of '/' separators (kind/id/arch/branch). */
#define B1958_SLASHES  3u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1958_seg_char_ok(unsigned char uCh)
{
	if ((uCh >= 'A' && uCh <= 'Z') ||
	    (uCh >= 'a' && uCh <= 'z') ||
	    (uCh >= '0' && uCh <= '9')) {
		return 1;
	}
	if (uCh == '-' || uCh == '_' || uCh == '.') {
		return 1;
	}
	return 0;
}

/* 1 if sz is a soft-ok Flatpak ref shape. */
static int
b1958_ref_ok(const char *szRef)
{
	size_t i;
	size_t cSlash;
	size_t cbSeg;

	if (szRef == NULL) {
		return 0;
	}
	if (szRef[0] == '\0' || szRef[0] == '/') {
		return 0;
	}

	cSlash = 0u;
	cbSeg = 0u;
	i = 0u;
	while (szRef[i] != '\0') {
		if (i >= B1958_REF_MAX) {
			return 0;
		}
		if (szRef[i] == '/') {
			if (cbSeg == 0u) {
				return 0; /* empty segment */
			}
			cSlash++;
			if (cSlash > B1958_SLASHES) {
				return 0;
			}
			cbSeg = 0u;
		} else {
			if (!b1958_seg_char_ok((unsigned char)szRef[i])) {
				return 0;
			}
			cbSeg++;
		}
		i++;
	}
	/* final segment must be non-empty; exactly three slashes */
	if (cbSeg == 0u || cSlash != B1958_SLASHES) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_flatpak_ref_ok_hint — 1 if ref is a soft-ok kind/id/arch/branch.
 *
 * ref: NUL-terminated Flatpak ref (NULL / bad shape → 0)
 * Exactly three '/', four non-empty restricted segments, len <= 255.
 */
int
gj_flatpak_ref_ok_hint(const char *szRef)
{
	(void)NULL;
	return b1958_ref_ok(szRef);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_flatpak_ref_ok_hint(const char *szRef)
    __attribute__((alias("gj_flatpak_ref_ok_hint")));
