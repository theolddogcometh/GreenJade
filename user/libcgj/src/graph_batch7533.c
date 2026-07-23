/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7533: kill signal SIGINT number stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ksig_int_num_7533(void);
 *     - Returns 2 (soft Linux x86_64 SIGINT number). Pure-data catalog
 *       stub; not a live kill(2) syscall.
 *   uint32_t __gj_ksig_int_num_7533  (alias)
 *   __libcgj_batch7533_marker = "libcgj-batch7533"
 *
 * Exclusive continuum CREATE-ONLY (7531-7540: kill signal number stubs —
 * kill_num, term_num, int_num, hup_num, stop_num, is_kill, is_term,
 * is_coop_term, is_fatal, batch_id_7540). Unique gj_ksig_int_num_7533
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7533_marker[] = "libcgj-batch7533";

/* Linux x86_64: SIGINT — interactive interrupt (Ctrl-C). */
#define B7533_SIGINT  ((uint32_t)2u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7533_int_num(void)
{
	return B7533_SIGINT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ksig_int_num_7533 - soft SIGINT signal number.
 *
 * Always returns 2. Catalog stub for kill/signal number tables.
 * No parent wires.
 */
uint32_t
gj_ksig_int_num_7533(void)
{
	(void)NULL;
	return b7533_int_num();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ksig_int_num_7533(void)
    __attribute__((alias("gj_ksig_int_num_7533")));
