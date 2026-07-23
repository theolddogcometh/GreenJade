/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7035: sched_attr flag set soft apply.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sched_attr_flag_set_7035(uint32_t cur, uint32_t set);
 *     - Return cur | set (soft enable listed sched_flags bits).
 *   uint32_t __gj_sched_attr_flag_set_7035  (alias)
 *   __libcgj_batch7035_marker = "libcgj-batch7035"
 *
 * Exclusive continuum CREATE-ONLY (7031-7040: sched_setattr policy stubs).
 * Unique gj_sched_attr_flag_set_7035 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7035_marker[] = "libcgj-batch7035";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7035_set(uint32_t u32Cur, uint32_t u32Set)
{
	return u32Cur | u32Set;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sched_attr_flag_set_7035 - soft sched_flags enable bits.
 *
 * cur: soft current sched_flags bitmask (low 32 of sched_attr.sched_flags)
 * set: bits to enable
 *
 * Returns cur | set. Soft pure-data op; does not call sched_setattr.
 */
uint32_t
gj_sched_attr_flag_set_7035(uint32_t u32Cur, uint32_t u32Set)
{
	(void)NULL;
	return b7035_set(u32Cur, u32Set);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sched_attr_flag_set_7035(uint32_t u32Cur, uint32_t u32Set)
    __attribute__((alias("gj_sched_attr_flag_set_7035")));
