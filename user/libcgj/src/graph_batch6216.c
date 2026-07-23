/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6216: DRM/KMS DisplayPort-family type stub.
 *
 * Surface (unique symbols):
 *   int gj_drm_connector_type_is_dp_family_6216(uint32_t type);
 *     - Return 1 if type is DisplayPort(10) or eDP(14). Else 0.
 *   int __gj_drm_connector_type_is_dp_family_6216  (alias)
 *   __libcgj_batch6216_marker = "libcgj-batch6216"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs). Unique gj_drm_connector_type_is_dp_family_6216 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6216_marker[] = "libcgj-batch6216";

#define B6216_DISPLAYPORT  10u
#define B6216_EDP          14u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6216_is_dp_family(uint32_t u32Type)
{
	if (u32Type == B6216_DISPLAYPORT || u32Type == B6216_EDP) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_type_is_dp_family_6216 - 1 if DP or eDP connector.
 *
 * Soft classification for DP-link policy (MST, HBR rates). Pure
 * predicate; no DRM fd. Does not call libc. No parent wires.
 */
int
gj_drm_connector_type_is_dp_family_6216(uint32_t u32Type)
{
	(void)NULL;
	return b6216_is_dp_family(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_drm_connector_type_is_dp_family_6216(uint32_t u32Type)
    __attribute__((alias("gj_drm_connector_type_is_dp_family_6216")));
