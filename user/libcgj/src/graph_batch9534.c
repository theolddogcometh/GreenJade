/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9534: Rust edition soft identity tag.
 *
 * Surface (unique symbols):
 *   uint32_t gj_rust_edition_id_9534(void);
 *     - Always returns 2021. Soft compile-time Rust edition id tag
 *       (edition 2021); not a real cargo rustc --edition probe.
 *   uint32_t __gj_rust_edition_id_9534  (alias)
 *   __libcgj_batch9534_marker = "libcgj-batch9534"
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

const char __libcgj_batch9534_marker[] = "libcgj-batch9534";

/* Soft Rust edition id: compile-time 2021 tag (not a real probe). */
#define B9534_RUST_EDITION_ID  2021u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9534_rust_edition_id(void)
{
	return B9534_RUST_EDITION_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rust_edition_id_9534 - Rust edition soft identity tag.
 *
 * Always returns 2021. Soft continuum pure-data edition tag; does not
 * invoke rustc/cargo or call libc. No parent wires.
 */
uint32_t
gj_rust_edition_id_9534(void)
{
	(void)NULL;
	return b9534_rust_edition_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rust_edition_id_9534(void)
    __attribute__((alias("gj_rust_edition_id_9534")));
