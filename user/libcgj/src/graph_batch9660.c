/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9660: libvirt soft continuum + batch id.
 *
 * Surface (unique symbols):
 *   uint32_t gj_batch_id_9660(void);
 *     - Returns the compile-time graph batch number for this TU (9660).
 *   uint32_t __gj_batch_id_9660  (alias)
 *   __libcgj_batch9660_marker = "libcgj-batch9660"
 *
 * Exclusive continuum CREATE-ONLY (9651-9660: libvirt soft id stubs —
 * virt_conn_ok_u_9651, virt_domain_ok_u_9652, virt_pool_ok_u_9653,
 * virt_vol_ok_u_9654, virt_net_ok_u_9655, virt_iface_ok_u_9656,
 * virt_start_ok_u_9657, virt_stop_ok_u_9658, libvirt_ready_u_9659,
 * batch_id_9660). Unique surfaces only; no multi-def. Does NOT redefine
 * gj_batch_id / gj_graph_milestone / prior batch_id_* symbols — avoid
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9660_marker[] = "libcgj-batch9660";

#define B9660_BATCH_ID  9660u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9660_id(void)
{
	return B9660_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_batch_id_9660 - report this TU's graph batch number.
 *
 * Always returns 9660. Link-time presence tags the exclusive libvirt
 * soft-id continuum wave end. No parent wires.
 */
uint32_t
gj_batch_id_9660(void)
{
	(void)NULL;
	return b9660_id();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_batch_id_9660(void)
    __attribute__((alias("gj_batch_id_9660")));
