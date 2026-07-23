/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8652: epoll EPOLL_CTL_MOD op id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_epoll_ctl_mod_id_8652(void);
 *     - Return soft epoll_ctl op id for EPOLL_CTL_MOD (3).
 *   uint32_t __gj_epoll_ctl_mod_id_8652  (alias)
 *   __libcgj_batch8652_marker = "libcgj-batch8652"
 *
 * Exclusive continuum CREATE-ONLY (8651-8660: epoll/eventfd id stubs —
 * ctl_add_id, ctl_mod_id, ctl_del_id, in_id, out_id, err_id, hup_id,
 * eventfd_init_ok, poll_ready, batch_id_8660).
 * Unique gj_epoll_ctl_mod_id_8652 surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8652_marker[] = "libcgj-batch8652";

/* EPOLL_CTL_MOD: modify interest of existing fd. */
#define B8652_EPOLL_CTL_MOD  ((uint32_t)3u)

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8652_ctl_mod_id(void)
{
	return B8652_EPOLL_CTL_MOD;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_epoll_ctl_mod_id_8652 - soft EPOLL_CTL_MOD op id.
 *
 * Always returns 3. Pure constant tag; no parent wires. Does not call
 * epoll_ctl.
 */
uint32_t
gj_epoll_ctl_mod_id_8652(void)
{
	(void)NULL;
	return b8652_ctl_mod_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_epoll_ctl_mod_id_8652(void)
    __attribute__((alias("gj_epoll_ctl_mod_id_8652")));
