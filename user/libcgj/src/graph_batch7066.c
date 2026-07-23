/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7066: capget/capset soft VERSION_2 id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_v2_id_7066(void);
 *     - Return soft _LINUX_CAPABILITY_VERSION_2 (0x20071026).
 *   uint32_t __gj_cap_v2_id_7066  (alias)
 *   __libcgj_batch7066_marker = "libcgj-batch7066"
 *
 * Exclusive continuum CREATE-ONLY (7061-7070: capset/capget version stubs —
 * is_v1, is_v2, is_v3, version_ok, v1_id, v2_id, v3_id, u32s,
 * version_errorish, batch_id_7070). Unique gj_cap_v2_id_7066 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7066_marker[] = "libcgj-batch7066";

/* Soft CAPABILITY_VERSION_2 catalog constant. */
#define B7066_CAP_VERSION_2  ((uint32_t)0x20071026u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7066_cap_v2_id(void)
{
	return B7066_CAP_VERSION_2;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_v2_id_7066 - soft CAPABILITY_VERSION_2 catalog constant.
 *
 * Always returns 0x20071026. Pure-data stub for product capget/capset
 * catalogs; does not invoke capget/capset. No parent wires.
 */
uint32_t
gj_cap_v2_id_7066(void)
{
	(void)NULL;
	return b7066_cap_v2_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_v2_id_7066(void)
    __attribute__((alias("gj_cap_v2_id_7066")));
