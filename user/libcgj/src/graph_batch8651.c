/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8651: epoll EPOLL_CTL_ADD op id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_ctl_add_id_8651(void);
 *     - Return soft epoll_ctl op id for EPOLL_CTL_ADD (1).
 *   uint32_t __gj_epoll_ctl_add_id_8651  (alias)
 *   __libcgj_batch8651_marker = "libcgj-batch8651"
 *
 * Exclusive continuum CREATE-ONLY (8651-8660: epoll/eventfd id stubs —
 * ctl_add_id, ctl_mod_id, ctl_del_id, in_id, out_id, err_id, hup_id,
 * eventfd_init_ok, poll_ready, batch_id_8660).
 * Unique gj_epoll_ctl_add_id_8651 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8651_marker[] = "libcgj-batch8651";

/* EPOLL_CTL_ADD: register new fd on epoll set. */
#define B8651_EPOLL_CTL_ADD  ((uint32_t)1u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8651_ctl_add_id(void)
{
	return B8651_EPOLL_CTL_ADD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_ctl_add_id_8651 - soft EPOLL_CTL_ADD op id.
 *
 * Always returns 1. Pure constant tag; no parent wires. Does not call
 * epoll_ctl.
 */
uint32_t
gj_epoll_ctl_add_id_8651(void)
{
	(void)NULL;
	return b8651_ctl_add_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_ctl_add_id_8651(void)
    __attribute__((alias("gj_epoll_ctl_add_id_8651")));
