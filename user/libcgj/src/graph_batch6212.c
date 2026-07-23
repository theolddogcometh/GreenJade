/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6212: DRM/KMS digital connector-type stub.
 *
 * Surface (unique symbols):
 *   int gj_drm_connector_type_is_digital_6212(uint32_t type);
 *     - Return 1 if type is a soft digital/TMDS/DP-family connector:
 *       DVI-I(2), DVI-D(3), DisplayPort(10), HDMI-A(11), HDMI-B(12),
 *       eDP(14), VIRTUAL(15), DSI(16), DPI(17), WRITEBACK(18), SPI(19),
 *       USB(20). Else 0 (including Unknown and analog types).
 *   int __gj_drm_connector_type_is_digital_6212  (alias)
 *   __libcgj_batch6212_marker = "libcgj-batch6212"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs). Unique gj_drm_connector_type_is_digital_6212 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6212_marker[] = "libcgj-batch6212";

/* Soft DRM connector-type codes used by this predicate. */
#define B6212_DVII         2u
#define B6212_DVID         3u
#define B6212_DISPLAYPORT  10u
#define B6212_HDMIA        11u
#define B6212_HDMIB        12u
#define B6212_EDP          14u
#define B6212_VIRTUAL      15u
#define B6212_DSI          16u
#define B6212_DPI          17u
#define B6212_WRITEBACK    18u
#define B6212_SPI          19u
#define B6212_USB          20u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6212_is_digital(uint32_t u32Type)
{
	switch (u32Type) {
	case B6212_DVII:
	case B6212_DVID:
	case B6212_DISPLAYPORT:
	case B6212_HDMIA:
	case B6212_HDMIB:
	case B6212_EDP:
	case B6212_VIRTUAL:
	case B6212_DSI:
	case B6212_DPI:
	case B6212_WRITEBACK:
	case B6212_SPI:
	case B6212_USB:
		return 1;
	default:
		return 0;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_type_is_digital_6212 - 1 if type is digital family.
 *
 * Soft classification for compositor policy (prefer digital sinks).
 * Pure predicate; no DRM fd. Does not call libc. No parent wires.
 */
int
gj_drm_connector_type_is_digital_6212(uint32_t u32Type)
{
	(void)NULL;
	return b6212_is_digital(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_drm_connector_type_is_digital_6212(uint32_t u32Type)
    __attribute__((alias("gj_drm_connector_type_is_digital_6212")));
