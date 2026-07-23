/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9561: soft meson-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_meson_ok_u_9561(void);
 *     - Returns 0 (meson soft-id continuum ok / soft success). Pure-data
 *       product tag; does not invoke meson or run configure.
 *   uint32_t __gj_meson_ok_u_9561  (alias)
 *   __libcgj_batch9561_marker = "libcgj-batch9561"
 *
 * Exclusive continuum CREATE-ONLY (9561-9570: meson/ninja soft id stubs —
 * meson_ok_u_9561, ninja_ok_u_9562, meson_setup_ok_u_9563,
 * meson_compile_ok_u_9564, meson_test_ok_u_9565, ninja_build_ok_u_9566,
 * build_dir_ok_u_9567, cross_file_ok_u_9568, meson_ready_u_9569,
 * batch_id_9570).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No meson/ninja implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9561_marker[] = "libcgj-batch9561";

/* Soft meson continuum ok (exit-style success = 0). */
#define B9561_MESON_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9561_meson_ok(void)
{
	return B9561_MESON_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_meson_ok_u_9561 - meson soft-id continuum ok flag.
 *
 * Always returns 0. Soft pure-data product tag; does not invoke meson
 * or run setup/compile. No parent wires.
 */
uint32_t
gj_meson_ok_u_9561(void)
{
	(void)NULL;
	return b9561_meson_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_meson_ok_u_9561(void)
    __attribute__((alias("gj_meson_ok_u_9561")));
