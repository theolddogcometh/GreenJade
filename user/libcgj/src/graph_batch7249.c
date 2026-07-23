/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch7249: setsockopt level/opt soft-ok stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_sock_level_opt_ok_u_7249(uint32_t level, uint32_t opt);
 *     - Return 1 if level is SOL_SOCKET and opt is a known soft SO_*
 *       option from this continuum catalog, else 0.
 *   uint32_t __gj_sock_level_opt_ok_u_7249  (alias)
 *   __libcgj_batch7249_marker = "libcgj-batch7249"
 *
 * Exclusive continuum CREATE-ONLY (7241-7250: setsockopt option stubs —
 * sol_socket_id, so_reuseaddr_id, so_keepalive_id, so_broadcast_id,
 * so_rcvbuf_id, so_sndbuf_id, so_linger_id, is_sol_socket, level_opt_ok,
 * batch_id_7250). Unique gj_sock_level_opt_ok_u_7249 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch7249_marker[] = "libcgj-batch7249";

/*
 * Soft catalog level (Linux):
 *   SOL_SOCKET=1
 * Soft catalog SOL_SOCKET options from this continuum:
 *   SO_REUSEADDR=2, SO_BROADCAST=6, SO_SNDBUF=7, SO_RCVBUF=8,
 *   SO_KEEPALIVE=9, SO_LINGER=13
 */
#define B7249_SOL_SOCKET   ((uint32_t)1u)
#define B7249_SO_REUSEADDR ((uint32_t)2u)
#define B7249_SO_BROADCAST ((uint32_t)6u)
#define B7249_SO_SNDBUF    ((uint32_t)7u)
#define B7249_SO_RCVBUF    ((uint32_t)8u)
#define B7249_SO_KEEPALIVE ((uint32_t)9u)
#define B7249_SO_LINGER    ((uint32_t)13u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b7249_level_ok(uint32_t u32Level)
{
	return (u32Level == B7249_SOL_SOCKET) ? 1u : 0u;
}

static uint32_t
b7249_opt_ok(uint32_t u32Opt)
{
	return (u32Opt == B7249_SO_REUSEADDR ||
	        u32Opt == B7249_SO_BROADCAST ||
	        u32Opt == B7249_SO_SNDBUF ||
	        u32Opt == B7249_SO_RCVBUF ||
	        u32Opt == B7249_SO_KEEPALIVE ||
	        u32Opt == B7249_SO_LINGER) ? 1u : 0u;
}

static uint32_t
b7249_level_opt_ok(uint32_t u32Level, uint32_t u32Opt)
{
	return (b7249_level_ok(u32Level) != 0u &&
	        b7249_opt_ok(u32Opt) != 0u) ? 1u : 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_sock_level_opt_ok_u_7249 - 1 if level+opt are soft-known.
 *
 * level: setsockopt(2) level argument
 * opt:   setsockopt(2) option name argument
 *
 * Returns 1 when level is SOL_SOCKET and opt is in
 * {SO_REUSEADDR, SO_BROADCAST, SO_SNDBUF, SO_RCVBUF, SO_KEEPALIVE,
 * SO_LINGER}, else 0. Soft catalog check; does not call setsockopt.
 * No parent wires.
 */
uint32_t
gj_sock_level_opt_ok_u_7249(uint32_t u32Level, uint32_t u32Opt)
{
	(void)NULL;
	return b7249_level_opt_ok(u32Level, u32Opt);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_sock_level_opt_ok_u_7249(uint32_t u32Level, uint32_t u32Opt)
    __attribute__((alias("gj_sock_level_opt_ok_u_7249")));
