/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9540: rustc/cargo soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9540(void);
 *     - Returns the compile-time graph batch number for this TU (9540).
 *   uint32_t __gj_batch_id_9540  (alias)
 *   __libcgj_batch9540_marker = "libcgj-batch9540"
 *
 * Exclusive continuum CREATE-ONLY (9531-9540: rustc/cargo soft id
 * stubs — rustc_ok_u_9531, cargo_ok_u_9532, crate_ok_u_9533,
 * rust_edition_id_9534, rust_target_ok_u_9535, cargo_build_ok_u_9536,
 * cargo_test_ok_u_9537, rlib_ok_u_9538, rust_ready_u_9539,
 * batch_id_9540). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9540_marker[] = "libcgj-batch9540";

#define B9540_BATCH_ID  9540u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9540_id(void)
{
	return B9540_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9540 - report this TU's graph batch number.
 *
 * Always returns 9540.
 */
uint32_t
gj_batch_id_9540(void)
{
	(void)NULL;
	return b9540_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9540(void)
    __attribute__((alias("gj_batch_id_9540")));
