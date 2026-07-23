/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7063: capget/capset soft VERSION_3 stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_is_v3_u_7063(uint32_t version);
 *     - Return 1 if version is soft _LINUX_CAPABILITY_VERSION_3
 *       (0x20080522), else 0.
 *   uint32_t __gj_cap_is_v3_u_7063  (alias)
 *   __libcgj_batch7063_marker = "libcgj-batch7063"
 *
 * Exclusive continuum CREATE-ONLY (7061-7070: capset/capget version stubs —
 * is_v1, is_v2, is_v3, version_ok, v1_id, v2_id, v3_id, u32s,
 * version_errorish, batch_id_7070). Unique gj_cap_is_v3_u_7063 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7063_marker[] = "libcgj-batch7063";

/* Soft _LINUX_CAPABILITY_VERSION_3 (preferred modern layout). */
#define B7063_CAP_VERSION_3  ((uint32_t)0x20080522u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7063_cap_is_v3(uint32_t u32Version)
{
	return (u32Version == B7063_CAP_VERSION_3) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_is_v3_u_7063 - 1 if version is soft CAPABILITY_VERSION_3.
 *
 * version: soft capget/capset header version field
 *
 * Returns 1 when version equals 0x20080522, else 0.
 * Pure integer compare; does not call capget/capset. No parent wires.
 */
uint32_t
gj_cap_is_v3_u_7063(uint32_t u32Version)
{
	(void)NULL;
	return b7063_cap_is_v3(u32Version);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_is_v3_u_7063(uint32_t u32Version)
    __attribute__((alias("gj_cap_is_v3_u_7063")));
