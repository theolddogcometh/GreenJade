/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12823: AMDVLK soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_amdvlk_ok_u_12823(void);
 *     - Returns 0 (AMDVLK soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live amdvlk/Vulkan ICD probe.
 *   uint32_t __gj_amdvlk_ok_u_12823  (alias)
 *   __libcgj_batch12823_marker = "libcgj-batch12823"
 *
 * Exclusive continuum CREATE-ONLY (12821-12830: mesa soft stubs —
 * all soft lamps →0; batch_id→12830). Unique gj_amdvlk_ok_u_12823
 * surface only; no multi-def. Distinct from gj_amdvlk_ok_u_12623,
 * gj_amdvlk_ok_u_12423, gj_amdvlk_ok_u_12223, gj_amdvlk_ok_u_12023,
 * gj_amdvlk_ok_u_11923, gj_amdvlk_ok_u_11723, gj_amdvlk_ok_u_11523,
 * gj_amdvlk_ok_u_11323, gj_amdvlk_ok_u_11123, gj_amdvlk_ok_u_10923,
 * gj_amdvlk_ok_u_10723, gj_amdvlk_ok_u_10023 and sibling mesa soft
 * ok_u stubs in this wave. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch12823_marker[] = "libcgj-batch12823";

/* Soft lamp: AMDVLK ok stub (not asserted). */
#define B12823_AMDVLK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b12823_amdvlk_ok(void)
{
	return B12823_AMDVLK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_amdvlk_ok_u_12823 - AMDVLK soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load amdvlk,
 * enumerate Vulkan devices, or probe ICD paths. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_amdvlk_ok_u_12823(void)
{
	(void)NULL;
	return b12823_amdvlk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_amdvlk_ok_u_12823(void)
    __attribute__((alias("gj_amdvlk_ok_u_12823")));
