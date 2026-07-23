/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5673: game launch depot id validate stub.
 *
 * Surface (unique symbols):
 *   int gj_depot_id_validate_5673(uint32_t depot_id);
 *     - Return 1 if depot_id is a plausible Steam depot id: nonzero and
 *       not the reserved soft-sentinel 0xFFFFFFFFu, else 0.
 *   int __gj_depot_id_validate_5673  (alias)
 *   __libcgj_batch5673_marker = "libcgj-batch5673"
 *
 * Game launch pipeline (appid validate / depot mount / process spawn
 * stub scores) exclusive CREATE-ONLY wave (5671-5680). Unique
 * gj_depot_id_validate_5673 surface only; no multi-def. Distinct from
 * gj_depot_id_ok (batch1862) and appid validate surfaces. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5673_marker[] = "libcgj-batch5673";

#define B5673_DEPOT_BAD  0xFFFFFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5673_depot_ok(uint32_t uDepot)
{
	if (uDepot == 0u) {
		return 0;
	}
	if (uDepot == B5673_DEPOT_BAD) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_depot_id_validate_5673 - soft check that depot_id is mount-plausible.
 *
 * depot_id: Steam depot identifier.
 * Returns 1 when depot_id is nonzero and not 0xFFFFFFFF, else 0.
 * Does not mount storage. Does not call libc. No parent wires.
 */
int
gj_depot_id_validate_5673(uint32_t uDepotId)
{
	(void)NULL;
	return b5673_depot_ok(uDepotId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_depot_id_validate_5673(uint32_t uDepotId)
    __attribute__((alias("gj_depot_id_validate_5673")));
