/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch11980: power soft + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_11980(void);
 *     - Returns the compile-time graph batch number for this TU (11980).
 *   uint32_t __gj_batch_id_11980  (alias)
 *   __libcgj_batch11980_marker = "libcgj-batch11980"
 *
 * Exclusive continuum CREATE-ONLY (11971-11980: power soft all→0 —
 * power_ok_u_11971, tdp_ok_u_11972, battery_ok_u_11973,
 * thermal_ok_u_11974, fan_ok_u_11975, suspend_ok_u_11976,
 * resume_ok_u_11977, performance_ok_u_11978, power_soft_ready_u_11979,
 * batch_id_11980). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_* symbols
 * — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch11980_marker[] = "libcgj-batch11980";

#define B11980_BATCH_ID  11980u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b11980_id(void)
{
return B11980_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_11980 - report this TU's graph batch number.
 *
 * Always returns 11980.
 */
uint32_t
gj_batch_id_11980(void)
{
(void)NULL;
return b11980_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_11980(void)
    __attribute__((alias("gj_batch_id_11980")));
