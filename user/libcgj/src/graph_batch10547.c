/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch10547: fps_limit soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fps_limit_ok_u_10547(void);
 *     - Returns 0 (soft stub: FPS limit slot is soft/unprobed; not a
 *       runtime FPS-limit probe).
 *   uint32_t __gj_fps_limit_ok_u_10547  (alias)
 *   __libcgj_batch10547_marker = "libcgj-batch10547"
 *
 * Exclusive continuum CREATE-ONLY (10541-10550: gamescope soft stubs —
 * all→0). Unique gj_fps_limit_ok_u_10547 surface only; no multi-def.
 * Distinct from gj_fps_limit_ok (batch1869), gj_fps_limit_ok_5488, and
 * sibling gamescope soft ok_u stubs in this wave. No parent wires. No
 * __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch10547_marker[] = "libcgj-batch10547";

/* Soft-stub fps_limit ok value (unprobed). */
#define B10547_FPS_LIMIT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b10547_fps_limit_ok(void)
{
	return B10547_FPS_LIMIT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fps_limit_ok_u_10547 - soft FPS limit ok stub.
 *
 * Always returns 0 (soft/unprobed fps_limit slot). Does not call libc
 * or probe FPS limits. No parent wires.
 */
uint32_t
gj_fps_limit_ok_u_10547(void)
{
	(void)NULL;
	return b10547_fps_limit_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fps_limit_ok_u_10547(void)
    __attribute__((alias("gj_fps_limit_ok_u_10547")));
