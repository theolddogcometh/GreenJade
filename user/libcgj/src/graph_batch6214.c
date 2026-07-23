/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6214: DRM/KMS internal-panel connector stub.
 *
 * Surface (unique symbols):
 *   int gj_drm_connector_type_is_panel_6214(uint32_t type);
 *     - Return 1 if type is a soft internal-panel connector:
 *       LVDS(7), eDP(14), DSI(16), DPI(17). Else 0.
 *   int __gj_drm_connector_type_is_panel_6214  (alias)
 *   __libcgj_batch6214_marker = "libcgj-batch6214"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs). Unique gj_drm_connector_type_is_panel_6214 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6214_marker[] = "libcgj-batch6214";

#define B6214_LVDS  7u
#define B6214_EDP   14u
#define B6214_DSI   16u
#define B6214_DPI   17u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6214_is_panel(uint32_t u32Type)
{
	switch (u32Type) {
	case B6214_LVDS:
	case B6214_EDP:
	case B6214_DSI:
	case B6214_DPI:
		return 1;
	default:
		return 0;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_type_is_panel_6214 - 1 if type is internal panel.
 *
 * Soft classification for laptop/tablet primary-display policy.
 * Pure predicate; no DRM fd. Does not call libc. No parent wires.
 */
int
gj_drm_connector_type_is_panel_6214(uint32_t u32Type)
{
	(void)NULL;
	return b6214_is_panel(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_drm_connector_type_is_panel_6214(uint32_t u32Type)
    __attribute__((alias("gj_drm_connector_type_is_panel_6214")));
