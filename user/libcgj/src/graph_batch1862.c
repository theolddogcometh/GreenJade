/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1862: Steam depot id validity predicate.
 *
 * Surface (unique symbols):
 *   int gj_depot_id_ok(uint32_t depot_id);
 *     — Return 1 if depot_id > 0 (plausible Steam depot id), else 0.
 *   int __gj_depot_id_ok  (alias)
 *   __libcgj_batch1862_marker = "libcgj-batch1862"
 *
 * Steam/desktop product exclusive pure-data wave (1861–1870).
 * Distinct from gj_appid_is_valid_hint — unique name. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1862_marker[] = "libcgj-batch1862";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1862_depot_ok(uint32_t uDepot)
{
	return (uDepot > 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_depot_id_ok — 1 iff depot_id is strictly positive.
 *
 * depot_id: Steam depot identifier.
 * Returns 1 when depot_id > 0, else 0.
 */
int
gj_depot_id_ok(uint32_t uDepotId)
{
	(void)sizeof(NULL);
	return b1862_depot_ok(uDepotId);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_depot_id_ok(uint32_t uDepotId)
    __attribute__((alias("gj_depot_id_ok")));
