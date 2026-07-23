/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6266: fan curve point pack (temp|duty) (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_pack_u_6266(uint32_t temp_c, uint32_t duty_pct);
 *     - Pack a control point as (temp_c << 16) | (duty_pct & 0xFFFF).
 *       Inputs are not clamped here; callers may sanitize first.
 *   uint32_t __gj_fc_pt_pack_u_6266  (alias)
 *   __libcgj_batch6266_marker = "libcgj-batch6266"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs).
 * Unique gj_fc_pt_pack_u_6266 surface only; no multi-def. Distinct
 * from clamp / ok predicates. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6266_marker[] = "libcgj-batch6266";

#define B6266_DUTY_MASK  0xFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6266_pack(uint32_t uTempC, uint32_t uDutyPct)
{
	return (uTempC << 16) | (uDutyPct & B6266_DUTY_MASK);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_pack_u_6266 - pack (temp_c, duty_pct) into one u32.
 *
 * High 16 bits: temperature (°C-style). Low 16 bits: duty percent.
 * Pure bit packing; no clamp, no libc. No parent wires.
 */
uint32_t
gj_fc_pt_pack_u_6266(uint32_t uTempC, uint32_t uDutyPct)
{
	(void)NULL;
	return b6266_pack(uTempC, uDutyPct);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_fc_pt_pack_u_6266(uint32_t uTempC, uint32_t uDutyPct)
    __attribute__((alias("gj_fc_pt_pack_u_6266")));
