/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7065: capget/capset soft VERSION_1 id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_v1_id_7065(void);
 *     - Return soft _LINUX_CAPABILITY_VERSION_1 (0x19980330).
 *   uint32_t __gj_cap_v1_id_7065  (alias)
 *   __libcgj_batch7065_marker = "libcgj-batch7065"
 *
 * Exclusive continuum CREATE-ONLY (7061-7070: capset/capget version stubs —
 * is_v1, is_v2, is_v3, version_ok, v1_id, v2_id, v3_id, u32s,
 * version_errorish, batch_id_7070). Unique gj_cap_v1_id_7065 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7065_marker[] = "libcgj-batch7065";

/* Soft CAPABILITY_VERSION_1 catalog constant. */
#define B7065_CAP_VERSION_1  ((uint32_t)0x19980330u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7065_cap_v1_id(void)
{
	return B7065_CAP_VERSION_1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_v1_id_7065 - soft CAPABILITY_VERSION_1 catalog constant.
 *
 * Always returns 0x19980330. Pure-data stub for product capget/capset
 * catalogs; does not invoke capget/capset. No parent wires.
 */
uint32_t
gj_cap_v1_id_7065(void)
{
	(void)NULL;
	return b7065_cap_v1_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_v1_id_7065(void)
    __attribute__((alias("gj_cap_v1_id_7065")));
