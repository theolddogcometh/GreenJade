/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1863: Steam buildid validity predicate.
 *
 * Surface (unique symbols):
 *   int gj_buildid_ok(uint32_t buildid);
 *     — Return 1 if buildid > 0 (plausible Steam build id), else 0.
 *   int __gj_buildid_ok  (alias)
 *   __libcgj_batch1863_marker = "libcgj-batch1863"
 *
 * Steam/desktop product exclusive pure-data wave (1861–1870).
 * Distinct from gj_depot_id_ok / gj_appid_is_valid_hint. Avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1863_marker[] = "libcgj-batch1863";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b1863_buildid_ok(uint32_t uBuild)
{
	return (uBuild > 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_buildid_ok — 1 iff buildid is strictly positive.
 *
 * buildid: Steam depot/app build identifier.
 * Returns 1 when buildid > 0, else 0.
 */
int
gj_buildid_ok(uint32_t uBuildid)
{
	(void)sizeof(NULL);
	return b1863_buildid_ok(uBuildid);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_buildid_ok(uint32_t uBuildid)
    __attribute__((alias("gj_buildid_ok")));
