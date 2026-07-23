/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8929: DRM/KMS id stub continuum ready.
 *
 * Surface (unique symbols):
 *   uint32_t gj_drm_ready_u_8929(void);
 *     - Returns 1 (drm/kms id stub continuum ready). Pure-data product
 *       tag that exclusive wave 8921-8930 surfaces are present.
 *   uint32_t __gj_drm_ready_u_8929  (alias)
 *   __libcgj_batch8929_marker = "libcgj-batch8929"
 *
 * Exclusive continuum CREATE-ONLY (8921-8930: drm/kms id stubs —
 * mode_ok_u_8921, connector_ok_u_8922, crtc_ok_u_8923, plane_ok_u_8924,
 * fb_ok_u_8925, dumb_ok_u_8926, prime_ok_u_8927, atomic_ok_u_8928,
 * ready_u_8929, batch_id_8930). Unique surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8929_marker[] = "libcgj-batch8929";

#define B8929_DRM_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8929_drm_ready(void)
{
	return B8929_DRM_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_ready_u_8929 - drm/kms id stub continuum ready.
 *
 * Always returns 1. Soft pure-data product tag that the exclusive wave
 * surfaces are present. Does not call libc. No parent wires.
 */
uint32_t
gj_drm_ready_u_8929(void)
{
	(void)NULL;
	return b8929_drm_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_drm_ready_u_8929(void)
    __attribute__((alias("gj_drm_ready_u_8929")));
