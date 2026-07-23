/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5461: Steam CDN cell id validity hint.
 *
 * Surface (unique symbols):
 *   int gj_steam_cdn_cell_ok_5461(uint32_t cell_id);
 *     — Return 1 if cell_id > 0 (plausible Steam CDN content-server
 *       cell id), else 0. Soft network layout hint only.
 *   int __gj_steam_cdn_cell_ok_5461  (alias)
 *   __libcgj_batch5461_marker = "libcgj-batch5461"
 *
 * Steam network/filesystem exclusive pure-data wave (5461–5470).
 * Does NOT redefine prior batch symbols — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5461_marker[] = "libcgj-batch5461";

/* ---- freestanding helpers ---------------------------------------------- */

static int
b5461_cell_ok(uint32_t uCell)
{
	return (uCell > 0u) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_steam_cdn_cell_ok_5461 — 1 iff cell_id is strictly positive.
 *
 * cell_id: Steam CDN cell / content-server region id.
 * Returns 1 when cell_id > 0, else 0.
 */
int
gj_steam_cdn_cell_ok_5461(uint32_t uCell)
{
	(void)NULL;
	return b5461_cell_ok(uCell);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_steam_cdn_cell_ok_5461(uint32_t uCell)
    __attribute__((alias("gj_steam_cdn_cell_ok_5461")));
