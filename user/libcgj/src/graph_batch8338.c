/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8338: path length soft-bound probe (_u).
 *
 * Surface (unique symbols):
 *   int gj_path_len_ok_u_8338(uint32_t path_len);
 *     - Return 1 if path_len is non-zero and within the soft path
 *       budget (1..4096 inclusive); else 0. Length-only probe — no
 *       string body required. Complements abs/rel/dot path probes.
 *   int __gj_path_len_ok_u_8338  (alias)
 *   __libcgj_batch8338_marker = "libcgj-batch8338"
 *
 * Env/path probe stubs wave (8331-8340). Distinct from
 * gj_proton_rt_path_len_ok_5972 and similar domain-specific len_ok
 * surfaces — unique _u_8338 surface; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8338_marker[] = "libcgj-batch8338";

/* Soft inclusive upper bound for path octet length probes. */
#define B8338_PATH_MAX  4096u

/* ---- freestanding helpers ---------------------------------------------- */

/* 1 iff u32Len is in 1..B8338_PATH_MAX. */
static int
b8338_len_ok(uint32_t u32Len)
{
	if (u32Len == 0u) {
		return 0;
	}
	if (u32Len > B8338_PATH_MAX) {
		return 0;
	}
	return 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_path_len_ok_u_8338 - 1 if path length is within soft budget.
 *
 * path_len: byte length of a candidate path (no string body required)
 *
 * Returns 1 when 1 <= path_len <= 4096, else 0.
 */
int
gj_path_len_ok_u_8338(uint32_t u32PathLen)
{
	(void)NULL;
	return b8338_len_ok(u32PathLen);
}

/* ---- underscored alias ------------------------------------------------- */

int __gj_path_len_ok_u_8338(uint32_t u32PathLen)
    __attribute__((alias("gj_path_len_ok_u_8338")));
