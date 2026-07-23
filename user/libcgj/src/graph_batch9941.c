/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9941: KVM soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kvm_ok_u_9941(void);
 *     - Return 0 (soft lamp: KVM device not claimed present for this
 *       freestanding continuum). Catalog only; not a /dev/kvm probe.
 *   uint32_t __gj_kvm_ok_u_9941  (alias)
 *   __libcgj_batch9941_marker = "libcgj-batch9941"
 *
 * Exclusive continuum CREATE-ONLY (9941-9950: kvm soft id stubs —
 * kvm_ok_u, kvm_vcpu_ok_u, kvm_mem_ok_u, kvm_irq_ok_u, kvm_ioevent_ok_u,
 * kvm_run_ok_u, kvm_exit_ok_u, kvm_cap_ok_u, kvm_ready_u, batch_id_9950).
 * Unique gj_kvm_ok_u_9941 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9941_marker[] = "libcgj-batch9941";

/* Soft KVM lamp (not claimed / soft-off). */
#define B9941_KVM_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9941_kvm_ok(void)
{
	return B9941_KVM_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kvm_ok_u_9941 - soft KVM presence lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not open /dev/kvm
 * or issue KVM_GET_API_VERSION. No parent wires.
 */
uint32_t
gj_kvm_ok_u_9941(void)
{
	(void)NULL;
	return b9941_kvm_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kvm_ok_u_9941(void)
    __attribute__((alias("gj_kvm_ok_u_9941")));
