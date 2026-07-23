/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7064: capget/capset soft version validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_version_ok_u_7064(uint32_t version);
 *     - Return 1 if version is a known soft capability version
 *       (V1/V2/V3), else 0.
 *   uint32_t __gj_cap_version_ok_u_7064  (alias)
 *   __libcgj_batch7064_marker = "libcgj-batch7064"
 *
 * Exclusive continuum CREATE-ONLY (7061-7070: capset/capget version stubs —
 * is_v1, is_v2, is_v3, version_ok, v1_id, v2_id, v3_id, u32s,
 * version_errorish, batch_id_7070). Unique gj_cap_version_ok_u_7064
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7064_marker[] = "libcgj-batch7064";

/* Known soft _LINUX_CAPABILITY_VERSION_{1,2,3}. */
#define B7064_CAP_VERSION_1  ((uint32_t)0x19980330u)
#define B7064_CAP_VERSION_2  ((uint32_t)0x20071026u)
#define B7064_CAP_VERSION_3  ((uint32_t)0x20080522u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7064_cap_version_ok(uint32_t u32Version)
{
	if (u32Version == B7064_CAP_VERSION_1 ||
	    u32Version == B7064_CAP_VERSION_2 ||
	    u32Version == B7064_CAP_VERSION_3) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_version_ok_u_7064 - 1 if version is a known soft cap version.
 *
 * version: soft capget/capset header version field
 *
 * Returns 1 for V1 (0x19980330), V2 (0x20071026), or V3 (0x20080522),
 * else 0. Soft catalog check; no parent wires.
 */
uint32_t
gj_cap_version_ok_u_7064(uint32_t u32Version)
{
	(void)NULL;
	return b7064_cap_version_ok(u32Version);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_version_ok_u_7064(uint32_t u32Version)
    __attribute__((alias("gj_cap_version_ok_u_7064")));
