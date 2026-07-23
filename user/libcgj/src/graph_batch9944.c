/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9944: KVM IRQ soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kvm_irq_ok_u_9944(void);
 *     - Return 0 (soft lamp: KVM IRQ routing not claimed present for
 *       this freestanding continuum). Catalog only; not irqchip I/O.
 *   uint32_t __gj_kvm_irq_ok_u_9944  (alias)
 *   __libcgj_batch9944_marker = "libcgj-batch9944"
 *
 * Exclusive continuum CREATE-ONLY (9941-9950: kvm soft id stubs —
 * kvm_ok_u, kvm_vcpu_ok_u, kvm_mem_ok_u, kvm_irq_ok_u, kvm_ioevent_ok_u,
 * kvm_run_ok_u, kvm_exit_ok_u, kvm_cap_ok_u, kvm_ready_u, batch_id_9950).
 * Unique gj_kvm_irq_ok_u_9944 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9944_marker[] = "libcgj-batch9944";

/* Soft KVM IRQ lamp (not claimed / soft-off). */
#define B9944_KVM_IRQ_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9944_irq_ok(void)
{
	return B9944_KVM_IRQ_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kvm_irq_ok_u_9944 - soft KVM IRQ-routing lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not create irqchips
 * or issue KVM_CREATE_IRQCHIP / KVM_IRQ_LINE. No parent wires.
 */
uint32_t
gj_kvm_irq_ok_u_9944(void)
{
	(void)NULL;
	return b9944_irq_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kvm_irq_ok_u_9944(void)
    __attribute__((alias("gj_kvm_irq_ok_u_9944")));
