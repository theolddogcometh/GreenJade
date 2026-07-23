/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7061: capget/capset soft VERSION_1 stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_is_v1_u_7061(uint32_t version);
 *     - Return 1 if version is soft _LINUX_CAPABILITY_VERSION_1
 *       (0x19980330), else 0.
 *   uint32_t __gj_cap_is_v1_u_7061  (alias)
 *   __libcgj_batch7061_marker = "libcgj-batch7061"
 *
 * Exclusive continuum CREATE-ONLY (7061-7070: capset/capget version stubs —
 * is_v1, is_v2, is_v3, version_ok, v1_id, v2_id, v3_id, u32s,
 * version_errorish, batch_id_7070). Unique gj_cap_is_v1_u_7061 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7061_marker[] = "libcgj-batch7061";

/* Soft _LINUX_CAPABILITY_VERSION_1 (not a live kernel probe). */
#define B7061_CAP_VERSION_1  ((uint32_t)0x19980330u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7061_cap_is_v1(uint32_t u32Version)
{
	return (u32Version == B7061_CAP_VERSION_1) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_is_v1_u_7061 - 1 if version is soft CAPABILITY_VERSION_1.
 *
 * version: soft capget/capset header version field
 *
 * Returns 1 when version equals 0x19980330, else 0.
 * Pure integer compare; does not call capget/capset. No parent wires.
 */
uint32_t
gj_cap_is_v1_u_7061(uint32_t u32Version)
{
	(void)NULL;
	return b7061_cap_is_v1(u32Version);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_is_v1_u_7061(uint32_t u32Version)
    __attribute__((alias("gj_cap_is_v1_u_7061")));
