/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11123: AMDVLK soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_amdvlk_ok_u_11123(void);
 *     - Returns 0 (AMDVLK soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live amdvlk/Vulkan ICD probe.
 *   uint32_t __gj_amdvlk_ok_u_11123  (alias)
 *   __libcgj_batch11123_marker = "libcgj-batch11123"
 *
 * Exclusive continuum CREATE-ONLY (11121-11130: mesa soft stubs —
 * all soft lamps →0; batch_id→11130). Unique gj_amdvlk_ok_u_11123
 * surface only; no multi-def. Distinct from gj_amdvlk_ok_u_10923,
 * gj_amdvlk_ok_u_10723, gj_amdvlk_ok_u_10023 and sibling mesa soft ok_u stubs in this wave.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11123_marker[] = "libcgj-batch11123";

/* Soft lamp: AMDVLK ok stub (not asserted). */
#define B11123_AMDVLK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11123_amdvlk_ok(void)
{
	return B11123_AMDVLK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_amdvlk_ok_u_11123 - AMDVLK soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load amdvlk, query
 * Vulkan ICDs, or probe driver paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_amdvlk_ok_u_11123(void)
{
	(void)NULL;
	return b11123_amdvlk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_amdvlk_ok_u_11123(void)
    __attribute__((alias("gj_amdvlk_ok_u_11123")));
