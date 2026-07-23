/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9560: llvm/clang soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9560(void);
 *     - Returns the compile-time graph batch number for this TU (9560).
 *   uint32_t __gj_batch_id_9560  (alias)
 *   __libcgj_batch9560_marker = "libcgj-batch9560"
 *
 * Exclusive continuum CREATE-ONLY (9551-9560: llvm/clang soft id stubs —
 * clang_ok_u_9551, llvm_ir_ok_u_9552, llvm_bitcode_ok_u_9553,
 * llvm_target_ok_u_9554, lld_ok_u_9555, clang_ast_ok_u_9556,
 * clang_format_ok_u_9557, clang_tidy_ok_u_9558, llvm_ready_u_9559,
 * batch_id_9560). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9560_marker[] = "libcgj-batch9560";

#define B9560_BATCH_ID  9560u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9560_id(void)
{
	return B9560_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9560 - report this TU's graph batch number.
 *
 * Always returns 9560.
 */
uint32_t
gj_batch_id_9560(void)
{
	(void)NULL;
	return b9560_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9560(void)
    __attribute__((alias("gj_batch_id_9560")));
