/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9658: libvirt domain stop ok soft unit (off).
 *
 * Surface (unique symbols):
 *   uint32_t gj_virt_stop_ok_u_9658(void);
 *     - Always returns 0. Soft stub only; intentionally not a real
 *       virDomainDestroy / domain stop operation.
 *   uint32_t __gj_virt_stop_ok_u_9658  (alias)
 *   __libcgj_batch9658_marker = "libcgj-batch9658"
 *
 * Exclusive continuum CREATE-ONLY (9651-9660: libvirt soft id stubs —
 * virt_conn_ok_u_9651, virt_domain_ok_u_9652, virt_pool_ok_u_9653,
 * virt_vol_ok_u_9654, virt_net_ok_u_9655, virt_iface_ok_u_9656,
 * virt_start_ok_u_9657, virt_stop_ok_u_9658, libvirt_ready_u_9659,
 * batch_id_9660). Unique surface only; no multi-def. Distinct from
 * gj_virt_start_ok_u_9657 (batch9657) and gj_ctr_stop_ok_u_9387
 * (batch9387). No parent wires. No __int128. No libvirt implementation.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9658_marker[] = "libcgj-batch9658";

/* Soft virt-stop-ok lamp: always off (not a real domain stop). */
#define B9658_VIRT_STOP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9658_virt_stop_ok(void)
{
	return B9658_VIRT_STOP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_virt_stop_ok_u_9658 - libvirt domain stop ok soft unit flag.
 *
 * Always returns 0. Soft continuum stub; does not stop domains or
 * call libvirt/libc. No parent wires.
 */
uint32_t
gj_virt_stop_ok_u_9658(void)
{
	(void)NULL;
	return b9658_virt_stop_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_virt_stop_ok_u_9658(void)
    __attribute__((alias("gj_virt_stop_ok_u_9658")));
