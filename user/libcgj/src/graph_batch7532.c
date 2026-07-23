/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7532: kill signal SIGTERM number stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ksig_term_num_7532(void);
 *     - Returns 15 (soft Linux x86_64 SIGTERM number). Pure-data catalog
 *       stub; not a live kill(2) syscall.
 *   uint32_t __gj_ksig_term_num_7532  (alias)
 *   __libcgj_batch7532_marker = "libcgj-batch7532"
 *
 * Exclusive continuum CREATE-ONLY (7531-7540: kill signal number stubs —
 * kill_num, term_num, int_num, hup_num, stop_num, is_kill, is_term,
 * is_coop_term, is_fatal, batch_id_7540). Unique gj_ksig_term_num_7532
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7532_marker[] = "libcgj-batch7532";

/* Linux x86_64: SIGTERM — polite termination request. */
#define B7532_SIGTERM  ((uint32_t)15u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7532_term_num(void)
{
	return B7532_SIGTERM;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ksig_term_num_7532 - soft SIGTERM signal number.
 *
 * Always returns 15. Catalog stub for kill/signal number tables.
 * No parent wires.
 */
uint32_t
gj_ksig_term_num_7532(void)
{
	(void)NULL;
	return b7532_term_num();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ksig_term_num_7532(void)
    __attribute__((alias("gj_ksig_term_num_7532")));
