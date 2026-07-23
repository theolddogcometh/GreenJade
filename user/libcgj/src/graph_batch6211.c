/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6211: DRM/KMS connector type validity stub.
 *
 * Surface (unique symbols):
 *   int gj_drm_connector_type_ok_6211(uint32_t type);
 *     - Return 1 if type is a known soft DRM connector-type code in
 *       [0, 20] (Unknown..USB), else 0. Pure predicate; not an ioctl.
 *   int __gj_drm_connector_type_ok_6211  (alias)
 *   __libcgj_batch6211_marker = "libcgj-batch6211"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs — type_ok, is_digital, is_analog, is_panel, is_hdmi,
 * is_dp_family, status_ok, type_clamp, type_bit, continuum +
 * batch_id_6220). Unique gj_drm_connector_type_ok_6211 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6211_marker[] = "libcgj-batch6211";

/* Soft known connector-type ceiling (USB = 20). */
#define B6211_TYPE_MAX  20u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6211_type_ok(uint32_t u32Type)
{
	if (u32Type > B6211_TYPE_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_type_ok_6211 - 1 if type is a known connector code.
 *
 * Screens soft DRM_MODE_CONNECTOR-style type fields for display policy
 * math. Pure predicate; no DRM fd, no modeset. Does not call libc.
 * No parent wires.
 */
int
gj_drm_connector_type_ok_6211(uint32_t u32Type)
{
	(void)NULL;
	return b6211_type_ok(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_drm_connector_type_ok_6211(uint32_t u32Type)
    __attribute__((alias("gj_drm_connector_type_ok_6211")));
