/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7534: kill signal SIGHUP number stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ksig_hup_num_7534(void);
 *     - Returns 1 (soft Linux x86_64 SIGHUP number). Pure-data catalog
 *       stub; not a live kill(2) syscall.
 *   uint32_t __gj_ksig_hup_num_7534  (alias)
 *   __libcgj_batch7534_marker = "libcgj-batch7534"
 *
 * Exclusive continuum CREATE-ONLY (7531-7540: kill signal number stubs —
 * kill_num, term_num, int_num, hup_num, stop_num, is_kill, is_term,
 * is_coop_term, is_fatal, batch_id_7540). Unique gj_ksig_hup_num_7534
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7534_marker[] = "libcgj-batch7534";

/* Linux x86_64: SIGHUP — hangup / controlling terminal closed. */
#define B7534_SIGHUP  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7534_hup_num(void)
{
	return B7534_SIGHUP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ksig_hup_num_7534 - soft SIGHUP signal number.
 *
 * Always returns 1. Catalog stub for kill/signal number tables.
 * No parent wires.
 */
uint32_t
gj_ksig_hup_num_7534(void)
{
	(void)NULL;
	return b7534_hup_num();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ksig_hup_num_7534(void)
    __attribute__((alias("gj_ksig_hup_num_7534")));
