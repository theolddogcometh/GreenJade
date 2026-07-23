/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8923: DRM/KMS CRTC id-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_drm_crtc_ok_u_8923(void);
 *     - Returns 1 (DRM/KMS CRTC soft-id continuum ok). Pure-data product
 *       tag; does not enumerate CRTCs or open DRM devices.
 *   uint32_t __gj_drm_crtc_ok_u_8923  (alias)
 *   __libcgj_batch8923_marker = "libcgj-batch8923"
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

const char __libcgj_batch8923_marker[] = "libcgj-batch8923";

#define B8923_CRTC_OK  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8923_crtc_ok(void)
{
	return B8923_CRTC_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_crtc_ok_u_8923 - DRM/KMS CRTC soft-id continuum ok flag.
 *
 * Always returns 1. Soft pure-data product tag; does not modeset CRTCs
 * or call libc. No parent wires.
 */
uint32_t
gj_drm_crtc_ok_u_8923(void)
{
	(void)NULL;
	return b8923_crtc_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_drm_crtc_ok_u_8923(void)
    __attribute__((alias("gj_drm_crtc_ok_u_8923")));
