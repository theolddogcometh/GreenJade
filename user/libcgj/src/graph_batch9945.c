/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9945: KVM ioevent soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kvm_ioevent_ok_u_9945(void);
 *     - Return 0 (soft lamp: KVM ioeventfd not claimed present for this
 *       freestanding continuum). Catalog only; not ioeventfd ioctl.
 *   uint32_t __gj_kvm_ioevent_ok_u_9945  (alias)
 *   __libcgj_batch9945_marker = "libcgj-batch9945"
 *
 * Exclusive continuum CREATE-ONLY (9941-9950: kvm soft id stubs —
 * kvm_ok_u, kvm_vcpu_ok_u, kvm_mem_ok_u, kvm_irq_ok_u, kvm_ioevent_ok_u,
 * kvm_run_ok_u, kvm_exit_ok_u, kvm_cap_ok_u, kvm_ready_u, batch_id_9950).
 * Unique gj_kvm_ioevent_ok_u_9945 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9945_marker[] = "libcgj-batch9945";

/* Soft KVM ioevent lamp (not claimed / soft-off). */
#define B9945_KVM_IOEVENT_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9945_ioevent_ok(void)
{
	return B9945_KVM_IOEVENT_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kvm_ioevent_ok_u_9945 - soft KVM ioeventfd lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not register
 * ioeventfds or issue KVM_IOEVENTFD. No parent wires.
 */
uint32_t
gj_kvm_ioevent_ok_u_9945(void)
{
	(void)NULL;
	return b9945_ioevent_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kvm_ioevent_ok_u_9945(void)
    __attribute__((alias("gj_kvm_ioevent_ok_u_9945")));
