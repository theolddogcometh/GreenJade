/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7026: rseq_cs no-restart flag soft bits.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rseq_cs_flag_no_restart_preempt_7026(void);
 *     - Returns 0x1 (soft RSEQ_CS_FLAG_NO_RESTART_ON_PREEMPT).
 *   uint32_t gj_rseq_cs_flag_no_restart_signal_7026(void);
 *     - Returns 0x2 (soft RSEQ_CS_FLAG_NO_RESTART_ON_SIGNAL).
 *   uint32_t gj_rseq_cs_flag_no_restart_migrate_7026(void);
 *     - Returns 0x4 (soft RSEQ_CS_FLAG_NO_RESTART_ON_MIGRATE).
 *   uint32_t gj_rseq_cs_flags_all_no_restart_7026(void);
 *     - Returns 0x7 (soft OR of the three no-restart bits).
 *   uint32_t __gj_rseq_cs_flag_*_7026  (aliases)
 *   __libcgj_batch7026_marker = "libcgj-batch7026"
 *
 * Exclusive continuum CREATE-ONLY (7021-7030: rseq signature stubs).
 * Unique gj_rseq_cs_flag_*_7026 surfaces only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7026_marker[] = "libcgj-batch7026";

#define B7026_NO_RESTART_PREEMPT  0x1u
#define B7026_NO_RESTART_SIGNAL   0x2u
#define B7026_NO_RESTART_MIGRATE  0x4u
#define B7026_NO_RESTART_ALL      0x7u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7026_preempt(void)
{
	return B7026_NO_RESTART_PREEMPT;
}

static uint32_t
b7026_signal(void)
{
	return B7026_NO_RESTART_SIGNAL;
}

static uint32_t
b7026_migrate(void)
{
	return B7026_NO_RESTART_MIGRATE;
}

static uint32_t
b7026_all(void)
{
	return B7026_NO_RESTART_ALL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rseq_cs_flag_no_restart_preempt_7026 - soft NO_RESTART_ON_PREEMPT.
 *
 * Always returns 0x1. Soft pure-data tag for rseq_cs.flags bit 0.
 */
uint32_t
gj_rseq_cs_flag_no_restart_preempt_7026(void)
{
	(void)NULL;
	return b7026_preempt();
}

/*
 * gj_rseq_cs_flag_no_restart_signal_7026 - soft NO_RESTART_ON_SIGNAL.
 *
 * Always returns 0x2. Soft pure-data tag for rseq_cs.flags bit 1.
 */
uint32_t
gj_rseq_cs_flag_no_restart_signal_7026(void)
{
	return b7026_signal();
}

/*
 * gj_rseq_cs_flag_no_restart_migrate_7026 - soft NO_RESTART_ON_MIGRATE.
 *
 * Always returns 0x4. Soft pure-data tag for rseq_cs.flags bit 2.
 */
uint32_t
gj_rseq_cs_flag_no_restart_migrate_7026(void)
{
	return b7026_migrate();
}

/*
 * gj_rseq_cs_flags_all_no_restart_7026 - OR of the three no-restart bits.
 *
 * Always returns 0x7. Soft pure-data aggregate for rseq_cs.flags when
 * all no-restart-on-* bits are set. No parent wires.
 */
uint32_t
gj_rseq_cs_flags_all_no_restart_7026(void)
{
	return b7026_all();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_rseq_cs_flag_no_restart_preempt_7026(void)
    __attribute__((alias("gj_rseq_cs_flag_no_restart_preempt_7026")));

uint32_t __gj_rseq_cs_flag_no_restart_signal_7026(void)
    __attribute__((alias("gj_rseq_cs_flag_no_restart_signal_7026")));

uint32_t __gj_rseq_cs_flag_no_restart_migrate_7026(void)
    __attribute__((alias("gj_rseq_cs_flag_no_restart_migrate_7026")));

uint32_t __gj_rseq_cs_flags_all_no_restart_7026(void)
    __attribute__((alias("gj_rseq_cs_flags_all_no_restart_7026")));
