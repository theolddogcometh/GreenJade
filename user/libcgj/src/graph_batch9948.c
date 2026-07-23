/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch9948: KVM capability soft lamp stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_kvm_cap_ok_u_9948(void);
 *     - Return 0 (soft lamp: KVM extension capability not claimed
 *       present for this freestanding continuum). Catalog only.
 *   uint32_t __gj_kvm_cap_ok_u_9948  (alias)
 *   __libcgj_batch9948_marker = "libcgj-batch9948"
 *
 * Exclusive continuum CREATE-ONLY (9941-9950: kvm soft id stubs —
 * kvm_ok_u, kvm_vcpu_ok_u, kvm_mem_ok_u, kvm_irq_ok_u, kvm_ioevent_ok_u,
 * kvm_run_ok_u, kvm_exit_ok_u, kvm_cap_ok_u, kvm_ready_u, batch_id_9950).
 * Unique gj_kvm_cap_ok_u_9948 surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch9948_marker[] = "libcgj-batch9948";

/* Soft KVM capability lamp (not claimed / soft-off). */
#define B9948_KVM_CAP_OK  0u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b9948_cap_ok(void)
{
	return B9948_KVM_CAP_OK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_kvm_cap_ok_u_9948 - soft KVM extension-capability lamp.
 *
 * Always returns 0. Soft pure-data status tag; does not issue
 * KVM_CHECK_EXTENSION. No parent wires.
 */
uint32_t
gj_kvm_cap_ok_u_9948(void)
{
	(void)NULL;
	return b9948_cap_ok();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_kvm_cap_ok_u_9948(void)
    __attribute__((alias("gj_kvm_cap_ok_u_9948")));
