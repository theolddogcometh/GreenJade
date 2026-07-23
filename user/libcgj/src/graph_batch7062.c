/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7062: capget/capset soft VERSION_2 stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_is_v2_u_7062(uint32_t version);
 *     - Return 1 if version is soft _LINUX_CAPABILITY_VERSION_2
 *       (0x20071026), else 0.
 *   uint32_t __gj_cap_is_v2_u_7062  (alias)
 *   __libcgj_batch7062_marker = "libcgj-batch7062"
 *
 * Exclusive continuum CREATE-ONLY (7061-7070: capset/capget version stubs —
 * is_v1, is_v2, is_v3, version_ok, v1_id, v2_id, v3_id, u32s,
 * version_errorish, batch_id_7070). Unique gj_cap_is_v2_u_7062 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7062_marker[] = "libcgj-batch7062";

/* Soft _LINUX_CAPABILITY_VERSION_2 (not a live kernel probe). */
#define B7062_CAP_VERSION_2  ((uint32_t)0x20071026u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7062_cap_is_v2(uint32_t u32Version)
{
	return (u32Version == B7062_CAP_VERSION_2) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_is_v2_u_7062 - 1 if version is soft CAPABILITY_VERSION_2.
 *
 * version: soft capget/capset header version field
 *
 * Returns 1 when version equals 0x20071026, else 0.
 * Pure integer compare; does not call capget/capset. No parent wires.
 */
uint32_t
gj_cap_is_v2_u_7062(uint32_t u32Version)
{
	(void)NULL;
	return b7062_cap_is_v2(u32Version);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_is_v2_u_7062(uint32_t u32Version)
    __attribute__((alias("gj_cap_is_v2_u_7062")));
