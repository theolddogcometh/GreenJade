/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8685: CAP_NET_ADMIN capability id stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_cap_net_admin_id_8685(void);
 *     - Return soft CAP_NET_ADMIN capability bit index (12).
 *   uint32_t __gj_cap_net_admin_id_8685  (alias)
 *   __libcgj_batch8685_marker = "libcgj-batch8685"
 *
 * Exclusive continuum CREATE-ONLY (8681-8690: cap/prctl id stubs —
 * cap_chown_id, cap_dac_override_id, cap_setuid_id, cap_setgid_id,
 * cap_net_admin_id, prctl_ready, securebits_ok, no_new_privs_id,
 * cap_ready, batch_id_8690). Unique gj_cap_net_admin_id_8685 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8685_marker[] = "libcgj-batch8685";

/* Soft CAP_NET_ADMIN capability bit index. */
#define B8685_CAP_NET_ADMIN  12u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8685_cap_net_admin_id(void)
{
	return B8685_CAP_NET_ADMIN;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_cap_net_admin_id_8685 - soft CAP_NET_ADMIN capability catalog id.
 *
 * Always returns 12. Pure-data stub for product capability catalogs;
 * does not invoke capget/capset or netlink. No parent wires.
 */
uint32_t
gj_cap_net_admin_id_8685(void)
{
	(void)NULL;
	return b8685_cap_net_admin_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_cap_net_admin_id_8685(void)
    __attribute__((alias("gj_cap_net_admin_id_8685")));
