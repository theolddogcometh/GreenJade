/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9536: cargo build ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_cargo_build_ok_u_9536(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       cargo build / compile success probe.
 *   uint32_t __gj_cargo_build_ok_u_9536  (alias)
 *   __libcgj_batch9536_marker = "libcgj-batch9536"
 *
 * Exclusive continuum CREATE-ONLY (9531-9540: rustc/cargo soft id
 * stubs — rustc_ok_u_9531, cargo_ok_u_9532, crate_ok_u_9533,
 * rust_edition_id_9534, rust_target_ok_u_9535, cargo_build_ok_u_9536,
 * cargo_test_ok_u_9537, rlib_ok_u_9538, rust_ready_u_9539,
 * batch_id_9540). Unique surface only; no multi-def. No parent wires.
 * No __int128. No rustc/cargo implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9536_marker[] = "libcgj-batch9536";

/* Soft cargo-build-ok lamp: always off (not a real build probe). */
#define B9536_CARGO_BUILD_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9536_cargo_build_ok(void)
{
	return B9536_CARGO_BUILD_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cargo_build_ok_u_9536 - cargo build ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not run cargo build or
 * call libc. No parent wires.
 */
uint32_t
gj_cargo_build_ok_u_9536(void)
{
	(void)NULL;
	return b9536_cargo_build_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cargo_build_ok_u_9536(void)
    __attribute__((alias("gj_cargo_build_ok_u_9536")));
