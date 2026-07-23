/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7535: kill signal SIGSTOP number stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_ksig_stop_num_7535(void);
 *     - Returns 19 (soft Linux x86_64 SIGSTOP number). Pure-data catalog
 *       stub; not a live kill(2) syscall.
 *   uint32_t __gj_ksig_stop_num_7535  (alias)
 *   __libcgj_batch7535_marker = "libcgj-batch7535"
 *
 * Exclusive continuum CREATE-ONLY (7531-7540: kill signal number stubs —
 * kill_num, term_num, int_num, hup_num, stop_num, is_kill, is_term,
 * is_coop_term, is_fatal, batch_id_7540). Unique gj_ksig_stop_num_7535
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7535_marker[] = "libcgj-batch7535";

/* Linux x86_64: SIGSTOP — stop process; cannot be handled. */
#define B7535_SIGSTOP  ((uint32_t)19u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7535_stop_num(void)
{
	return B7535_SIGSTOP;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_ksig_stop_num_7535 - soft SIGSTOP signal number.
 *
 * Always returns 19. Catalog stub for kill/signal number tables.
 * No parent wires.
 */
uint32_t
gj_ksig_stop_num_7535(void)
{
	(void)NULL;
	return b7535_stop_num();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_ksig_stop_num_7535(void)
    __attribute__((alias("gj_ksig_stop_num_7535")));
