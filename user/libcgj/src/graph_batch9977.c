/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9977: product install score soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_install_score_u_9977(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       install-path product readiness probe.
 *   uint32_t __gj_product_install_score_u_9977  (alias)
 *   __libcgj_batch9977_marker = "libcgj-batch9977"
 *
 * Exclusive continuum CREATE-ONLY (9971-9980: product score aggregation
 * soft stubs — product_shell_score_u, product_libcgj_score_u,
 * product_smoke_score_u, product_dyn_score_u, product_bar3_score_u,
 * product_steam_score_u, product_install_score_u, product_deck_score_u,
 * product_total_soft_u, batch_id_9980). Unique
 * gj_product_install_score_u_9977 surface only; no multi-def. No parent
 * wires. No __int128. No install path walk.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9977_marker[] = "libcgj-batch9977";

/* Soft product install score unit: always off (not a real install probe). */
#define B9977_PRODUCT_INSTALL_SCORE  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9977_product_install_score(void)
{
	return B9977_PRODUCT_INSTALL_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_install_score_u_9977 - product install score soft unit.
 *
 * Always returns 0. Soft continuum stub; does not walk install paths.
 * No parent wires.
 */
uint32_t
gj_product_install_score_u_9977(void)
{
	(void)NULL;
	return b9977_product_install_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_install_score_u_9977(void)
    __attribute__((alias("gj_product_install_score_u_9977")));
