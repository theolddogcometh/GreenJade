/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8859: if_nametoindex continuum ready lamp.
 *
 * Surface (unique symbols):
 *   uint32_t gj_if_ready_u_8859(void);
 *     - Return 1 (soft lamp: 8851-8860 if_nametoindex stubs continuum
 *       ready).
 *   uint32_t __gj_if_ready_u_8859  (alias)
 *   __libcgj_batch8859_marker = "libcgj-batch8859"
 *
 * Exclusive continuum CREATE-ONLY (8851-8860: if_nametoindex stubs —
 * if_name_ok_u, if_index_ok_u, if_loop_id, if_eth_ok_u, if_up_id,
 * if_running_id, if_mtu_ok_u, if_mac_len_id, if_ready_u,
 * batch_id_8860). Unique gj_if_ready_u_8859 surface only; no multi-def.
 * Soft pure-data tag only; no if_nametoindex. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8859_marker[] = "libcgj-batch8859";

/* Soft continuum-ready lamp for if_nametoindex stubs wave. */
#define B8859_IF_READY  1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8859_ready(void)
{
	return B8859_IF_READY;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_if_ready_u_8859 - soft if_nametoindex continuum ready lamp.
 *
 * Always returns 1. Pure-data product tag that the 8851-8860
 * if_nametoindex stubs exclusive wave is present. Does not call
 * if_nametoindex. No parent wires.
 */
uint32_t
gj_if_ready_u_8859(void)
{
	(void)NULL;
	return b8859_ready();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_if_ready_u_8859(void)
    __attribute__((alias("gj_if_ready_u_8859")));
