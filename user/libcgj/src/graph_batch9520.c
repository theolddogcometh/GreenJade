/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9520: Lua soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9520(void);
 *     - Returns the compile-time graph batch number for this TU (9520).
 *   uint32_t __gj_batch_id_9520  (alias)
 *   __libcgj_batch9520_marker = "libcgj-batch9520"
 *
 * Exclusive continuum CREATE-ONLY (9511-9520: lua soft id
 * stubs — lua_state_ok_u_9511, lua_stack_ok_u_9512,
 * lua_table_ok_u_9513, lua_func_ok_u_9514,
 * lua_string_ok_u_9515, lua_number_ok_u_9516,
 * lua_pcall_ok_u_9517, lua_load_ok_u_9518, lua_ready_u_9519,
 * batch_id_9520). Unique surfaces only; no multi-def. Does NOT
 * redefine gj_batch_id / gj_graph_milestone / prior batch_id_*
 * symbols — avoid multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9520_marker[] = "libcgj-batch9520";

#define B9520_BATCH_ID  9520u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9520_id(void)
{
	return B9520_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9520 - report this TU's graph batch number.
 *
 * Always returns 9520.
 */
uint32_t
gj_batch_id_9520(void)
{
	(void)NULL;
	return b9520_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9520(void)
    __attribute__((alias("gj_batch_id_9520")));
