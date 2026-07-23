/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6217: DRM/KMS connector status validity stub.
 *
 * Surface (unique symbols):
 *   int gj_drm_connector_status_ok_6217(uint32_t status);
 *     - Return 1 if status is a known soft connector-status code:
 *         1 = connected, 2 = disconnected, 3 = unknown.
 *       Any other value → 0.
 *   int __gj_drm_connector_status_ok_6217  (alias)
 *   __libcgj_batch6217_marker = "libcgj-batch6217"
 *
 * Exclusive continuum CREATE-ONLY (6211-6220: DRM/KMS connector type
 * stubs). Unique gj_drm_connector_status_ok_6217 surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6217_marker[] = "libcgj-batch6217";

/* Soft connector-status encodings (1-based classic DRM values). */
#define B6217_CONNECTED     1u
#define B6217_DISCONNECTED  2u
#define B6217_UNKNOWN       3u

/* ---- freestanding helpers ---------------------------------------------- */

static int
b6217_status_ok(uint32_t u32Status)
{
	if (u32Status == B6217_CONNECTED ||
	    u32Status == B6217_DISCONNECTED ||
	    u32Status == B6217_UNKNOWN) {
		return 1;
	}
	return 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_drm_connector_status_ok_6217 - 1 if status is a known status code.
 *
 * Screens soft connector_status fields for display policy. Pure
 * predicate; no DRM fd, no hotplug. Does not call libc. No parent wires.
 */
int
gj_drm_connector_status_ok_6217(uint32_t u32Status)
{
	(void)NULL;
	return b6217_status_ok(u32Status);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_drm_connector_status_ok_6217(uint32_t u32Status)
    __attribute__((alias("gj_drm_connector_status_ok_6217")));
