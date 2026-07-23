/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10023: AMDVLK soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_amdvlk_ok_u_10023(void);
 *     - Returns 0 (AMDVLK soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live amdvlk ICD probe.
 *   uint32_t __gj_amdvlk_ok_u_10023  (alias)
 *   __libcgj_batch10023_marker = "libcgj-batch10023"
 *
 * Exclusive CREATE-ONLY (10021-10030: mesa/radv soft id stubs). Unique
 * gj_amdvlk_ok_u_10023 surface only; no multi-def. Distinct from
 * gj_mesa_ok_u_10021 and gj_radv_ok_u_10022. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10023_marker[] = "libcgj-batch10023";

/* Soft lamp: AMDVLK ok stub (not asserted). */
#define B10023_AMDVLK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10023_amdvlk_ok(void)
{
	return B10023_AMDVLK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_amdvlk_ok_u_10023 - AMDVLK soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load amdvlk, query
 * Vulkan, or probe ICD paths. Does not call libc. No parent wires.
 */
uint32_t
gj_amdvlk_ok_u_10023(void)
{
	(void)NULL;
	return b10023_amdvlk_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_amdvlk_ok_u_10023(void)
    __attribute__((alias("gj_amdvlk_ok_u_10023")));
