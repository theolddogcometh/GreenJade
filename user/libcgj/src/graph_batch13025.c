/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch13025: Lavapipe soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lavapipe_ok_u_13025(void);
 *     - Returns 0 (Lavapipe soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live lavapipe software ICD
 *       probe.
 *   uint32_t __gj_lavapipe_ok_u_13025  (alias)
 *   __libcgj_batch13025_marker = "libcgj-batch13025"
 *
 * Exclusive continuum CREATE-ONLY (13021-13030: mesa soft stubs —
 * all soft lamps →0; batch_id→13030). Unique gj_lavapipe_ok_u_13025
 * surface only; no multi-def. Distinct from gj_lavapipe_ok_u_12825,
 * gj_lavapipe_ok_u_12625, gj_lavapipe_ok_u_12425, gj_lavapipe_ok_u_12225,
 * gj_lavapipe_ok_u_12025, gj_lavapipe_ok_u_11925, gj_lavapipe_ok_u_11725,
 * gj_lavapipe_ok_u_11525, gj_lavapipe_ok_u_11325, gj_lavapipe_ok_u_11125,
 * gj_lavapipe_ok_u_10925, gj_lavapipe_ok_u_10725, gj_lavapipe_ok_u_10025
 * and sibling mesa soft ok_u stubs in this wave. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch13025_marker[] = "libcgj-batch13025";

/* Soft lamp: Lavapipe ok stub (not asserted). */
#define B13025_LAVAPIPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b13025_lavapipe_ok(void)
{
	return B13025_LAVAPIPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lavapipe_ok_u_13025 - Lavapipe soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load lavapipe,
 * enumerate software Vulkan devices, or probe ICD paths. Does not
 * call libc. No parent wires.
 */
uint32_t
gj_lavapipe_ok_u_13025(void)
{
	(void)NULL;
	return b13025_lavapipe_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lavapipe_ok_u_13025(void)
    __attribute__((alias("gj_lavapipe_ok_u_13025")));
