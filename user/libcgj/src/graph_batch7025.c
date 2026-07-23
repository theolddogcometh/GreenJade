/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7025: rseq cpu_id special soft values.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_cpu_id_uninit_7025(void);
 *     - Returns 0xFFFFFFFF (soft RSEQ_CPU_ID_UNINITIALIZED = -1).
 *   uint32_t gj_rseq_cpu_id_reg_failed_7025(void);
 *     - Returns 0xFFFFFFFE (soft RSEQ_CPU_ID_REGISTRATION_FAILED = -2).
 *   uint32_t __gj_rseq_cpu_id_uninit_7025  (alias)
 *   uint32_t __gj_rseq_cpu_id_reg_failed_7025  (alias)
 *   __libcgj_batch7025_marker = "libcgj-batch7025"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs).
 * Unique gj_rseq_cpu_id_*_7025 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7025_marker[] = "libcgj-batch7025";

/* Soft: RSEQ_CPU_ID_UNINITIALIZED as unsigned 32-bit (-1). */
#define B7025_CPU_ID_UNINIT      0xFFFFFFFFu
/* Soft: RSEQ_CPU_ID_REGISTRATION_FAILED as unsigned 32-bit (-2). */
#define B7025_CPU_ID_REG_FAILED  0xFFFFFFFEu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7025_uninit(void)
{
	return B7025_CPU_ID_UNINIT;
}

static uint32_t
b7025_reg_failed(void)
{
	return B7025_CPU_ID_REG_FAILED;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_cpu_id_uninit_7025 - soft RSEQ_CPU_ID_UNINITIALIZED.
 *
 * Always returns 0xFFFFFFFF. Soft pure-data tag for the cpu_id field
 * value before successful registration. Does not touch TLS. No parent
 * wires.
 */
uint32_t
gj_rseq_cpu_id_uninit_7025(void)
{
	(void)NULL;
	return b7025_uninit();
}

/*
 * gj_rseq_cpu_id_reg_failed_7025 - soft RSEQ_CPU_ID_REGISTRATION_FAILED.
 *
 * Always returns 0xFFFFFFFE. Soft pure-data tag for the cpu_id field
 * value after a failed rseq registration attempt. No parent wires.
 */
uint32_t
gj_rseq_cpu_id_reg_failed_7025(void)
{
	return b7025_reg_failed();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_rseq_cpu_id_uninit_7025(void)
    __attribute__((alias("gj_rseq_cpu_id_uninit_7025")));

uint32_t __gj_rseq_cpu_id_reg_failed_7025(void)
    __attribute__((alias("gj_rseq_cpu_id_reg_failed_7025")));
