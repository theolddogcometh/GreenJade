/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9574: soft cmake test-ok unit flag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cmake_test_ok_u_9574(void);
 *     - Returns 0 (cmake soft-id continuum test not-ok / idle).
 *       Pure-data product tag; does not run ctest.
 *   uint32_t __gj_cmake_test_ok_u_9574  (alias)
 *   __libcgj_batch9574_marker = "libcgj-batch9574"
 *
 * Exclusive continuum CREATE-ONLY (9571-9580: cmake soft id stubs —
 * cmake_ok_u_9571→0, cmake_config_ok_u_9572→0, cmake_build_ok_u_9573→0,
 * cmake_test_ok_u_9574→0, cmake_install_ok_u_9575→0,
 * cmake_generator_ok_u_9576, cmake_toolchain_ok_u_9577,
 * cmake_preset_ok_u_9578, cmake_ready_u_9579, batch_id_9580).
 * Unique surface only; no multi-def. No parent wires. No __int128.
 * No cmake implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9574_marker[] = "libcgj-batch9574";

/* Soft cmake test-ok flag: idle / not-ok (0). */
#define B9574_CMAKE_TEST_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9574_cmake_test_ok(void)
{
	return B9574_CMAKE_TEST_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cmake_test_ok_u_9574 - cmake soft-id continuum test-ok unit flag.
 *
 * Always returns 0. Soft pure-data product tag; does not run tests
 * or call libc. No parent wires.
 */
uint32_t
gj_cmake_test_ok_u_9574(void)
{
	(void)NULL;
	return b9574_cmake_test_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cmake_test_ok_u_9574(void)
    __attribute__((alias("gj_cmake_test_ok_u_9574")));
