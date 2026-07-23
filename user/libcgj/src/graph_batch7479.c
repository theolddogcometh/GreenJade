/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7479: mkdirat mode errorish/unknown stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_mkdirat_mode_errorish_u_7479(uint32_t mode);
 *     - Return 1 if mode has any bits outside ALLPERMS (07777), else 0.
 *   uint32_t __gj_mkdirat_mode_errorish_u_7479  (alias)
 *   __libcgj_batch7479_marker = "libcgj-batch7479"
 *
 * Exclusive continuum CREATE-ONLY (7471-7480: mkdirat mode stubs —
 * def_mode_id, priv_mode_id, world_mode_id, allperms_id,
 * has_irwxu, has_irwxo, mode_ok, mode_pack, mode_errorish,
 * batch_id_7480).
 * Unique gj_mkdirat_mode_errorish_u_7479 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7479_marker[] = "libcgj-batch7479";

/* Soft ALLPERMS mask: bits outside this are errorish for mkdirat mode. */
#define B7479_MKDIRAT_ALLPERMS ((uint32_t)07777u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7479_mkdirat_mode_errorish(uint32_t u32Mode)
{
	return ((u32Mode & ~B7479_MKDIRAT_ALLPERMS) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_mkdirat_mode_errorish_u_7479 - 1 if mode has unknown/type bits.
 *
 * mode: mkdirat(2) mode argument (soft mode_t-like u32)
 *
 * Returns 1 when any bit outside 07777 is set (e.g. S_IFDIR type
 * nibble), else 0. Soft inverse of mode_ok; does not call mkdirat.
 * No parent wires.
 */
uint32_t
gj_mkdirat_mode_errorish_u_7479(uint32_t u32Mode)
{
	(void)NULL;
	return b7479_mkdirat_mode_errorish(u32Mode);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_mkdirat_mode_errorish_u_7479(uint32_t u32Mode)
    __attribute__((alias("gj_mkdirat_mode_errorish_u_7479")));
