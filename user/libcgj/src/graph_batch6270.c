/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6270: fan curve point continuum ready + id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_fc_pt_ready_6270(void);
 *     - Returns 1 (fan curve point continuum ready lamp for the
 *       6261-6270 exclusive wave). Soft compile-time product tag.
 *   uint32_t gj_batch_id_6270(void);
 *     - Returns the compile-time graph batch number for this TU (6270).
 *   uint32_t __gj_fc_pt_ready_6270  (alias)
 *   uint32_t __gj_batch_id_6270  (alias)
 *   __libcgj_batch6270_marker = "libcgj-batch6270"
 *
 * Exclusive continuum CREATE-ONLY (6261-6270: fan curve point stubs —
 * temp_ok, duty_ok, pt_ok, clamp_temp, clamp_duty, pack_u, mono_ok,
 * lerp_duty, pwm_from_duty, continuum ready + batch_id_6270). Unique
 * *_6270 surfaces only; no multi-def. Does NOT redefine gj_batch_id /
 * gj_graph_milestone / prior batch_id_* symbols — avoid multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6270_marker[] = "libcgj-batch6270";

#define B6270_BATCH_ID  6270u
#define B6270_READY     1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6270_ready(void)
{
	return B6270_READY;
}

static uint32_t
b6270_id(void)
{
	return B6270_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_fc_pt_ready_6270 - fan curve point continuum readiness lamp.
 *
 * Always returns 1 (wave 6261-6270 present/ready). Does not call libc
 * or probe hardware. No parent wires.
 */
uint32_t
gj_fc_pt_ready_6270(void)
{
	(void)NULL;
	return b6270_ready();
}

/*
 * gj_batch_id_6270 - report this TU's graph batch number.
 *
 * Always returns 6270.
 */
uint32_t
gj_batch_id_6270(void)
{
	return b6270_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_fc_pt_ready_6270(void)
    __attribute__((alias("gj_fc_pt_ready_6270")));

uint32_t __gj_batch_id_6270(void)
    __attribute__((alias("gj_batch_id_6270")));
