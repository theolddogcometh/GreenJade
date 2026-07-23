/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8838: netlink port-id validity stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_nl_pid_ok_u_8838(uint32_t nl_pid);
 *     - Return 1 if nl_pid is a non-zero soft port id, else 0.
 *   uint32_t __gj_nl_pid_ok_u_8838  (alias)
 *   __libcgj_batch8838_marker = "libcgj-batch8838"
 *
 * Exclusive continuum CREATE-ONLY (8831-8840: netlink msg type id stubs —
 * nl_route_id, nl_addr_id, nl_link_id, nl_neigh_id, nlmsg_done_id,
 * nlmsg_error_id, nlmsg_ok_u, nl_pid_ok_u, netlink_ready_u,
 * batch_id_8840). Unique gj_nl_pid_ok_u_8838 surface only; no multi-def.
 * Soft pure-data gate only; no live netlink bind/getsockname. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8838_marker[] = "libcgj-batch8838";

/* Soft kernel/auto-assign port id sentinel (nl_pid == 0). */
#define B8838_NL_PID_KERNEL  ((uint32_t)0u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8838_nl_pid_ok(uint32_t u32Pid)
{
	if (u32Pid == B8838_NL_PID_KERNEL)
		return 0u;
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_nl_pid_ok_u_8838 - 1 if nl_pid is a non-zero soft port id.
 *
 * nl_pid: soft netlink port identifier (nlmsg_pid / sockaddr_nl.nl_pid)
 *
 * Returns 0 when pid is 0 (kernel/auto-assign sentinel), else 1. Soft
 * pure-data gate only; does not bind sockets. No parent wires.
 */
uint32_t
gj_nl_pid_ok_u_8838(uint32_t u32NlPid)
{
	(void)NULL;
	return b8838_nl_pid_ok(u32NlPid);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_nl_pid_ok_u_8838(uint32_t u32NlPid)
    __attribute__((alias("gj_nl_pid_ok_u_8838")));
