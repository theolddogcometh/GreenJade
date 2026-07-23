/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10025: Lavapipe soft id ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_lavapipe_ok_u_10025(void);
 *     - Returns 0 (Lavapipe soft id stub not asserted ready). Soft
 *       compile-time product tag; not a live lavapipe/software Vulkan
 *       probe.
 *   uint32_t __gj_lavapipe_ok_u_10025  (alias)
 *   __libcgj_batch10025_marker = "libcgj-batch10025"
 *
 * Exclusive CREATE-ONLY (10021-10030: mesa/radv soft id stubs). Unique
 * gj_lavapipe_ok_u_10025 surface only; no multi-def. Distinct from
 * gj_mesa_ok_u_10021 and gj_anv_ok_u_10024. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10025_marker[] = "libcgj-batch10025";

/* Soft lamp: Lavapipe ok stub (not asserted). */
#define B10025_LAVAPIPE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10025_lavapipe_ok(void)
{
	return B10025_LAVAPIPE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lavapipe_ok_u_10025 - Lavapipe soft id ok stub.
 *
 * Always returns 0. Soft pure-data lamp; does not load lavapipe, run
 * software Vulkan, or probe ICD paths. Does not call libc. No parent
 * wires.
 */
uint32_t
gj_lavapipe_ok_u_10025(void)
{
	(void)NULL;
	return b10025_lavapipe_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lavapipe_ok_u_10025(void)
    __attribute__((alias("gj_lavapipe_ok_u_10025")));
