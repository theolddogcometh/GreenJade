/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9971: product shell score soft unit (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_shell_score_u_9971(void);
 *     - Always returns 1. Soft stub only; intentionally not a real
 *       greenjade-shell product readiness probe.
 *   uint32_t __gj_product_shell_score_u_9971  (alias)
 *   __libcgj_batch9971_marker = "libcgj-batch9971"
 *
 * Exclusive continuum CREATE-ONLY (9971-9980: product score aggregation
 * soft stubs — product_shell_score_u, product_libcgj_score_u,
 * product_smoke_score_u, product_dyn_score_u, product_bar3_score_u,
 * product_steam_score_u, product_install_score_u, product_deck_score_u,
 * product_total_soft_u, batch_id_9980). Unique
 * gj_product_shell_score_u_9971 surface only; no multi-def. No parent
 * wires. No __int128. No shell spawn / path probe.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9971_marker[] = "libcgj-batch9971";

/* Soft product shell score unit: always on (not a real shell probe). */
#define B9971_PRODUCT_SHELL_SCORE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9971_product_shell_score(void)
{
	return B9971_PRODUCT_SHELL_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_shell_score_u_9971 - product shell score soft unit.
 *
 * Always returns 1. Soft continuum stub; does not spawn or probe
 * greenjade-shell. No parent wires.
 */
uint32_t
gj_product_shell_score_u_9971(void)
{
	(void)NULL;
	return b9971_product_shell_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_shell_score_u_9971(void)
    __attribute__((alias("gj_product_shell_score_u_9971")));
