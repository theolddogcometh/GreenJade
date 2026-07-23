/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9972: product libcgj score soft unit (on).
 *
 * Surface (unique symbols):
 *   uint32_t gj_product_libcgj_score_u_9972(void);
 *     - Always returns 1. Soft stub only; intentionally not a real
 *       libcgj soname / dlopen product readiness probe.
 *   uint32_t __gj_product_libcgj_score_u_9972  (alias)
 *   __libcgj_batch9972_marker = "libcgj-batch9972"
 *
 * Exclusive continuum CREATE-ONLY (9971-9980: product score aggregation
 * soft stubs — product_shell_score_u, product_libcgj_score_u,
 * product_smoke_score_u, product_dyn_score_u, product_bar3_score_u,
 * product_steam_score_u, product_install_score_u, product_deck_score_u,
 * product_total_soft_u, batch_id_9980). Unique
 * gj_product_libcgj_score_u_9972 surface only; no multi-def. No parent
 * wires. No __int128. No dlopen / soname probe.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9972_marker[] = "libcgj-batch9972";

/* Soft product libcgj score unit: always on (not a real lib probe). */
#define B9972_PRODUCT_LIBCGJ_SCORE  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9972_product_libcgj_score(void)
{
	return B9972_PRODUCT_LIBCGJ_SCORE;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_product_libcgj_score_u_9972 - product libcgj score soft unit.
 *
 * Always returns 1. Soft continuum stub; does not dlopen or probe
 * libcgj soname. No parent wires.
 */
uint32_t
gj_product_libcgj_score_u_9972(void)
{
	(void)NULL;
	return b9972_product_libcgj_score();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_product_libcgj_score_u_9972(void)
    __attribute__((alias("gj_product_libcgj_score_u_9972")));
