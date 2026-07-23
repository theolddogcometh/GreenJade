/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9513: Lua table ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_lua_table_ok_u_9513(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Lua table / lua_istable / lua_newtable validity probe.
 *   uint32_t __gj_lua_table_ok_u_9513  (alias)
 *   __libcgj_batch9513_marker = "libcgj-batch9513"
 *
 * Exclusive continuum CREATE-ONLY (9511-9520: lua soft id
 * stubs — lua_state_ok_u_9511, lua_stack_ok_u_9512,
 * lua_table_ok_u_9513, lua_func_ok_u_9514,
 * lua_string_ok_u_9515, lua_number_ok_u_9516,
 * lua_pcall_ok_u_9517, lua_load_ok_u_9518, lua_ready_u_9519,
 * batch_id_9520). Unique surface only; no multi-def. No parent wires.
 * No __int128. No Lua/liblua implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9513_marker[] = "libcgj-batch9513";

/* Soft Lua table-ok lamp: always off (not a real table probe). */
#define B9513_LUA_TABLE_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9513_lua_table_ok(void)
{
	return B9513_LUA_TABLE_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lua_table_ok_u_9513 - Lua table ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not create or probe
 * Lua tables or call liblua/libc. No parent wires.
 */
uint32_t
gj_lua_table_ok_u_9513(void)
{
	(void)NULL;
	return b9513_lua_table_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lua_table_ok_u_9513(void)
    __attribute__((alias("gj_lua_table_ok_u_9513")));
