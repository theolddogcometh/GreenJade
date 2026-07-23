/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9565: soft meson-test-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_meson_test_ok_u_9565(void);
 *     - Returns 0 (meson test soft-id continuum ok / soft success).
 *       Pure-data product tag; does not run meson test or any suite.
 *   uint32_t __gj_meson_test_ok_u_9565  (alias)
 *   __libcgj_batch9565_marker = "libcgj-batch9565"
 *
 * Exclusive continuum CREATE-ONLY (9561-9570: meson/ninja soft id stubs —
 * meson_ok_u_9561, ninja_ok_u_9562, meson_setup_ok_u_9563,
 * meson_compile_ok_u_9564, meson_test_ok_u_9565, ninja_build_ok_u_9566,
 * build_dir_ok_u_9567, cross_file_ok_u_9568, meson_ready_u_9569,
 * batch_id_9570).
 * Unique surface only; no multi-def. Distinct from setup/compile ok
 * flags. No parent wires. No __int128. No meson/ninja implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9565_marker[] = "libcgj-batch9565";

/* Soft meson test continuum ok (exit-style success = 0). */
#define B9565_MESON_TEST_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9565_meson_test_ok(void)
{
	return B9565_MESON_TEST_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_meson_test_ok_u_9565 - meson test soft-id continuum ok flag.
 *
 * Always returns 0. Soft pure-data product tag; does not run
 * meson test or execute suites. No parent wires.
 */
uint32_t
gj_meson_test_ok_u_9565(void)
{
	(void)NULL;
	return b9565_meson_test_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_meson_test_ok_u_9565(void)
    __attribute__((alias("gj_meson_test_ok_u_9565")));
