/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10725: Lavapipe soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lavapipe_ok_u_10725(void);
 *     - Returns 0 (Lavapipe soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live lavapipe software ICD
 *       probe.
 *   uint32_t __gj_lavapipe_ok_u_10725  (alias)
 *   __libcgj_batch10725_marker = "libcgj-batch10725"
 *
 * Exclusive continuum CREATE-ONLY (10721-10730: mesa soft stubs —
 * ok slots all→0; mesa_soft_ready→1; batch_id→10730). Unique
 * gj_lavapipe_ok_u_10725 surface only; no multi-def. Distinct from
 * gj_lavapipe_ok_u_10025 and sibling mesa soft ok_u stubs in this wave.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10725_marker[] = "libcgj-batch10725";

/* Soft lamp: Lavapipe ok stub (not asserted). */
#define B10725_LAVAPIPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10725_lavapipe_ok(void)
{
	return B10725_LAVAPIPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lavapipe_ok_u_10725 - Lavapipe soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load lavapipe, run
 * software rasterization, or probe ICD paths. Does not call libc. No
 * parent wires.
 */
uint32_t
gj_lavapipe_ok_u_10725(void)
{
	(void)NULL;
	return b10725_lavapipe_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lavapipe_ok_u_10725(void)
    __attribute__((alias("gj_lavapipe_ok_u_10725")));
