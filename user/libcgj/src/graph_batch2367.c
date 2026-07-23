/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2367: Steam Deck model string soft-ok
 * predicate (post-2360 product path exclusive surface).
 *
 * Surface (unique symbols):
 *   int gj_deck_model_ok(const char *model);
 *     - Return 1 if model is a non-NULL non-empty string of length
 *       1..32 using only printable ASCII (0x20..0x7e). Soft product
 *       model label check (e.g. "Jupiter", "Galileo"); no DMI probe.
 *   int __gj_deck_model_ok  (alias)
 *   __libcgj_batch2367_marker = "libcgj-batch2367"
 *
 * Post-2360 product path exclusive wave (2361-2370). Distinct from
 * gj_deck_res_is_native_hint (batch1868) and gj_deck_res_scale_u
 * (batch2274) - unique deck_model_ok surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2367_marker[] = "libcgj-batch2367";

/* Max length for a Deck model product label. */
#define B2367_MODEL_MAX  32u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if ch is printable ASCII (space through tilde). */
static int
b2367_ch_ok(unsigned char u8Ch)
{
	if (u8Ch >= 0x20u && u8Ch <= 0x7eu) {
		return 1;
	}
	return 0;
}

/* Soft shape check for Deck model labels. */
static int
b2367_model_ok(const char *szModel)
{
	size_t nLen;
	const unsigned char *p;

	if (szModel == NULL) {
		return 0;
	}
	p = (const unsigned char *)szModel;
	nLen = 0u;
	while (p[nLen] != 0u) {
		if (nLen >= B2367_MODEL_MAX) {
			return 0;
		}
		if (!b2367_ch_ok(p[nLen])) {
			return 0;
		}
		nLen++;
	}
	if (nLen == 0u) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_deck_model_ok - soft check for a Steam Deck model label.
 *
 * model: product model string (e.g. "Jupiter")
 *
 * Returns 1 when model is non-empty, length <= 32, and printable
 * ASCII only, else 0. Does not call libc.
 */
int
gj_deck_model_ok(const char *szModel)
{
	(void)NULL;
	return b2367_model_ok(szModel);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_deck_model_ok(const char *szModel)
    __attribute__((alias("gj_deck_model_ok")));
