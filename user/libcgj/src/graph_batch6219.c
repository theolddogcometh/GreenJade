/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6219: DRM/KMS connector type → bit mask stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_drm_connector_type_bit_6219(uint32_t type);
 *     - Map a known connector type in [0, 20] to (1u << type).
 *       type > 20 → 0 (unknown / out of range).
 *   uint32_t __gj_drm_connector_type_bit_6219  (alias)
 *   __libcgj_batch6219_marker = "libcgj-batch6219"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs). Unique gj_drm_connector_type_bit_6219 surface only;
 * no multi-def. Pure data — not a connector filter ioctl. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6219_marker[] = "libcgj-batch6219";

#define B6219_TYPE_MAX  20u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6219_type_bit(uint32_t u32Type)
{
	if (u32Type > B6219_TYPE_MAX) {
		return 0u;
	}
	return (1u << u32Type);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_type_bit_6219 - map type id to a single-bit mask.
 *
 * Soft filter-bit helper for multi-type allow masks. Pure data; no
 * DRM fd. Does not call libc. No parent wires.
 */
uint32_t
gj_drm_connector_type_bit_6219(uint32_t u32Type)
{
	(void)NULL;
	return b6219_type_bit(u32Type);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_drm_connector_type_bit_6219(uint32_t u32Type)
    __attribute__((alias("gj_drm_connector_type_bit_6219")));
