/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6215: DRM/KMS HDMI connector-type stub.
 *
 * Surface (unique symbols):
 *   int gj_drm_connector_type_is_hdmi_6215(uint32_t type);
 *     - Return 1 if type is HDMI-A(11) or HDMI-B(12). Else 0.
 *   int __gj_drm_connector_type_is_hdmi_6215  (alias)
 *   __libcgj_batch6215_marker = "libcgj-batch6215"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs). Unique gj_drm_connector_type_is_hdmi_6215 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6215_marker[] = "libcgj-batch6215";

#define B6215_HDMIA  11u
#define B6215_HDMIB  12u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6215_is_hdmi(uint32_t u32Type)
{
	if (u32Type == B6215_HDMIA || u32Type == B6215_HDMIB) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_type_is_hdmi_6215 - 1 if type is HDMI-A or HDMI-B.
 *
 * Soft classification for HDMI-specific sink policy (audio path hints,
 * infoframes). Pure predicate; no DRM fd. Does not call libc.
 * No parent wires.
 */
int
gj_drm_connector_type_is_hdmi_6215(uint32_t u32Type)
{
	(void)NULL;
	return b6215_is_hdmi(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_drm_connector_type_is_hdmi_6215(uint32_t u32Type)
    __attribute__((alias("gj_drm_connector_type_is_hdmi_6215")));
