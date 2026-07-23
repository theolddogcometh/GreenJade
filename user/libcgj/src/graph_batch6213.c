/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6213: DRM/KMS analog connector-type stub.
 *
 * Surface (unique symbols):
 *   int gj_drm_connector_type_is_analog_6213(uint32_t type);
 *     - Return 1 if type is a soft analog / legacy video connector:
 *       VGA(1), DVI-A(4), Composite(5), S-Video(6), Component(8),
 *       9-Pin DIN(9), TV(13). Else 0.
 *   int __gj_drm_connector_type_is_analog_6213  (alias)
 *   __libcgj_batch6213_marker = "libcgj-batch6213"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs). Unique gj_drm_connector_type_is_analog_6213 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6213_marker[] = "libcgj-batch6213";

#define B6213_VGA        1u
#define B6213_DVIA       4u
#define B6213_COMPOSITE  5u
#define B6213_SVIDEO     6u
#define B6213_COMPONENT  8u
#define B6213_DIN9       9u
#define B6213_TV         13u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6213_is_analog(uint32_t u32Type)
{
	switch (u32Type) {
	case B6213_VGA:
	case B6213_DVIA:
	case B6213_COMPOSITE:
	case B6213_SVIDEO:
	case B6213_COMPONENT:
	case B6213_DIN9:
	case B6213_TV:
		return 1;
	default:
		return 0;
	}
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_type_is_analog_6213 - 1 if type is analog family.
 *
 * Soft classification for legacy-sink policy. Pure predicate; no DRM
 * fd. Does not call libc. No parent wires.
 */
int
gj_drm_connector_type_is_analog_6213(uint32_t u32Type)
{
	(void)NULL;
	return b6213_is_analog(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_drm_connector_type_is_analog_6213(uint32_t u32Type)
    __attribute__((alias("gj_drm_connector_type_is_analog_6213")));
