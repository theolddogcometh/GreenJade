/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7625: setrlimit resource id ok/gate stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_setrlimit_res_ok_7625(uint32_t resource);
 *     - Return 1 if resource is a known Linux x86_64 RLIMIT_* index
 *       (0..15 inclusive / resource < RLIM_NLIMITS); else 0. Soft
 *       pure-data gate; does not call setrlimit(2).
 *   uint32_t __gj_setrlimit_res_ok_7625  (alias)
 *   __libcgj_batch7625_marker = "libcgj-batch7625"
 *
 * Exclusive continuum CREATE-ONLY (7621-7630: setrlimit resource stubs).
 * Unique gj_setrlimit_res_ok_7625 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7625_marker[] = "libcgj-batch7625";

/* Local freestanding RLIM_NLIMITS (Linux x86_64). */
#define B7625_RLIM_NLIMITS  16u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7625_ok(uint32_t u32Res)
{
	if (u32Res < B7625_RLIM_NLIMITS) {
		return 1u;
	}
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_setrlimit_res_ok_7625 - soft known-resource gate.
 *
 * Returns 1 when resource is in 0..15; else 0. Soft pure-data only.
 * No parent wires.
 */
uint32_t
gj_setrlimit_res_ok_7625(uint32_t resource)
{
	(void)NULL;
	return b7625_ok(resource);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_setrlimit_res_ok_7625(uint32_t resource)
    __attribute__((alias("gj_setrlimit_res_ok_7625")));
