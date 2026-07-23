/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9512: Lua stack ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_lua_stack_ok_u_9512(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       Lua stack / lua_gettop / lua_checkstack validity probe.
 *   uint32_t __gj_lua_stack_ok_u_9512  (alias)
 *   __libcgj_batch9512_marker = "libcgj-batch9512"
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

const char __libcgj_batch9512_marker[] = "libcgj-batch9512";

/* Soft Lua stack-ok lamp: always off (not a real stack probe). */
#define B9512_LUA_STACK_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9512_lua_stack_ok(void)
{
	return B9512_LUA_STACK_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_lua_stack_ok_u_9512 - Lua stack ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not inspect the Lua
 * stack or call liblua/libc. No parent wires.
 */
uint32_t
gj_lua_stack_ok_u_9512(void)
{
	(void)NULL;
	return b9512_lua_stack_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_lua_stack_ok_u_9512(void)
    __attribute__((alias("gj_lua_stack_ok_u_9512")));
