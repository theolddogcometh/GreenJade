/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9545: go version soft catalog id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_go_version_id_9545(void);
 *     - Returns 122 (soft go version catalog id; e.g. go1.22-class
 *       encoding as 122). Soft catalog constant only; not a runtime
 *       go version string probe.
 *   uint32_t __gj_go_version_id_9545  (alias)
 *   __libcgj_batch9545_marker = "libcgj-batch9545"
 *
 * Exclusive continuum CREATE-ONLY (9541-9550: go toolchain soft id stubs —
 * go_build_ok_u_9541, go_test_ok_u_9542, go_mod_ok_u_9543,
 * go_pkg_ok_u_9544, go_version_id_9545, go_arch_ok_u_9546,
 * go_os_ok_u_9547, go_cgo_ok_u_9548, go_ready_u_9549,
 * batch_id_9550). Unique surface only; no multi-def. No parent wires.
 * No __int128. No Go toolchain implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9545_marker[] = "libcgj-batch9545";

/* Soft go version catalog id (go1.22-class encoding). */
#define B9545_GO_VERSION_ID  122u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9545_go_version_id(void)
{
	return B9545_GO_VERSION_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_go_version_id_9545 - soft go version catalog id.
 *
 * Always returns 122. Soft continuum constant; does not invoke go
 * version or call libc. No parent wires.
 */
uint32_t
gj_go_version_id_9545(void)
{
	(void)NULL;
	return b9545_go_version_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_go_version_id_9545(void)
    __attribute__((alias("gj_go_version_id_9545")));
