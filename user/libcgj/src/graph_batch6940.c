/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch6940: splice/tee/vmsplice flags continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_splice_f_continuum_6940(void);
 *     - Returns 1 (soft compile-time product tag: splice/tee/vmsplice
 *       flags stub continuum 6931-6940 is complete / ready).
 *   uint32_t gj_batch_id_6940(void);
 *     - Returns the compile-time graph batch number for this TU (6940).
 *   uint32_t __gj_splice_f_continuum_6940  (alias)
 *   uint32_t __gj_batch_id_6940  (alias)
 *   __libcgj_batch6940_marker = "libcgj-batch6940"
 *
 * Exclusive continuum CREATE-ONLY (6931-6940: splice/tee/vmsplice flags
 * stubs —
 * splice_f_move_6931, splice_f_nonblock_6932, splice_f_more_6933,
 * splice_f_gift_6934, splice_f_all_6935, splice_f_ok_6936,
 * splice_f_has_6937, tee_f_ok_6938, vmsplice_f_ok_6939,
 * continuum + batch_id_6940). Unique surfaces only; no multi-def.
 * Does NOT redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch6940_marker[] = "libcgj-batch6940";

/* Continuum-complete lamp for the 6931-6940 exclusive wave. */
#define B6940_CONTINUUM_READY  1u
#define B6940_BATCH_ID         6940u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b6940_continuum(void)
{
	return B6940_CONTINUUM_READY;
}

static uint32_t
b6940_id(void)
{
	return B6940_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_splice_f_continuum_6940 - continuum-ready tag for 6931-6940.
 *
 * Always returns 1. Soft pure-data product tag that the splice/tee/
 * vmsplice flags stub exclusive wave is present. Does not call libc.
 * No parent wires.
 */
uint32_t
gj_splice_f_continuum_6940(void)
{
	(void)NULL;
	return b6940_continuum();
}

/*
 * gj_batch_id_6940 - report this TU's graph batch number.
 *
 * Always returns 6940.
 */
uint32_t
gj_batch_id_6940(void)
{
	return b6940_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_splice_f_continuum_6940(void)
    __attribute__((alias("gj_splice_f_continuum_6940")));

uint32_t __gj_batch_id_6940(void)
    __attribute__((alias("gj_batch_id_6940")));
