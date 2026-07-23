/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2273: Wine bottle / prefix slot soft-ok
 * predicate (unsigned).
 *
 * Surface (unique symbols):
 *   int gj_wine_prefix_ok_u(uint32_t bottle_id);
 *     - Return 1 if bottle_id is a plausible Wine prefix / bottle slot
 *       identifier: nonzero and strictly less than 0x10000 (soft upper
 *       band for local bottle registries). Else 0. Soft catalog-free
 *       hint only; does not inspect WINEPREFIX paths.
 *   int __gj_wine_prefix_ok_u  (alias)
 *   __libcgj_batch2273_marker = "libcgj-batch2273"
 *
 * Post-2270 product exclusive wave (2271-2280). Distinct from
 * gj_proton_prefix_ok_u (batch2272) - unique gj_wine_prefix_ok_u
 * surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2273_marker[] = "libcgj-batch2273";

/* Soft exclusive upper bound for local bottle registry slots. */
#define B2273_BOTTLE_MAX  0x10000u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 if bottle_id is in (0, B2273_BOTTLE_MAX). */
static int
b2273_bottle_ok(uint32_t uBottle)
{
	if (uBottle == 0u) {
		return 0;
	}
	if (uBottle >= B2273_BOTTLE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wine_prefix_ok_u - soft check for a Wine bottle / prefix slot id.
 *
 * bottle_id: candidate local bottle registry identifier
 *
 * Returns 1 when 0 < bottle_id < 0x10000, else 0.
 * Does not call libc or inspect the filesystem.
 */
int
gj_wine_prefix_ok_u(uint32_t uBottleId)
{
	(void)NULL;
	return b2273_bottle_ok(uBottleId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_wine_prefix_ok_u(uint32_t uBottleId)
    __attribute__((alias("gj_wine_prefix_ok_u")));
