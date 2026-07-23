/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6218: DRM/KMS connector type clamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_drm_connector_type_clamp_6218(uint32_t type);
 *     - Clamp a connector-type code into [0, 20]. Values above 20 → 20
 *       (USB soft ceiling). Values already in range pass through.
 *   uint32_t __gj_drm_connector_type_clamp_6218  (alias)
 *   __libcgj_batch6218_marker = "libcgj-batch6218"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs). Unique gj_drm_connector_type_clamp_6218 surface only;
 * no multi-def. Pure data — not a modeset writer. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6218_marker[] = "libcgj-batch6218";

#define B6218_TYPE_MAX  20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6218_clamp(uint32_t u32Type)
{
	if (u32Type > B6218_TYPE_MAX) {
		return B6218_TYPE_MAX;
	}
	return u32Type;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_type_clamp_6218 - clamp type into [0, 20].
 *
 * Soft sanitizer for untrusted connector-type fields before policy
 * tables. Pure data; no DRM fd. Does not call libc. No parent wires.
 */
uint32_t
gj_drm_connector_type_clamp_6218(uint32_t u32Type)
{
	(void)NULL;
	return b6218_clamp(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_drm_connector_type_clamp_6218(uint32_t u32Type)
    __attribute__((alias("gj_drm_connector_type_clamp_6218")));
