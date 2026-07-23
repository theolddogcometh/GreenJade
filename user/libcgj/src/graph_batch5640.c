/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch5640: install Secure Boot + TPM gate.
 *
 * Surface (unique symbols):
 *   uint32_t gj_install_sb_tpm_gate_ok_5640(uint32_t sb_state,
 *                                           uint32_t tpm_present,
 *                                           uint32_t tpm_family);
 *     - Combined install-path gate: Secure Boot enabled (state==2),
 *       TPM present (non-zero), and TPM family >= 2. Returns 1 only
 *       when all three soft conditions hold.
 *   uint32_t gj_batch_id_5640(void);
 *     - Returns 5640 (wave end marker for 5631-5640 continuum).
 *   uint32_t __gj_install_sb_tpm_gate_ok_5640  (alias)
 *   uint32_t __gj_batch_id_5640  (alias)
 *   __libcgj_batch5640_marker = "libcgj-batch5640"
 *
 * Exclusive continuum CREATE-ONLY (5631-5640: secure boot / TPM /
 * measured boot stubs for install path — secure_boot_required_5631,
 * secure_boot_state_ok_5632, tpm_present_ok_5633, tpm_family_min_5634,
 * tpm_family_ok_5635, pcr_index_ok_5636, pcr_bank_alg_sha256_5637,
 * measured_boot_extend_5638, measured_boot_event_ok_5639,
 * install_sb_tpm_gate_ok_5640). Unique *_5640 surfaces only; no
 * multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch5640_marker[] = "libcgj-batch5640";

#define B5640_BATCH_ID        5640u
#define B5640_SB_STATE_EN     2u
#define B5640_TPM_FAMILY_MIN  2u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b5640_gate_ok(uint32_t u32SbState, uint32_t u32TpmPresent,
    uint32_t u32TpmFamily)
{
	if (u32SbState != B5640_SB_STATE_EN) {
		return 0u;
	}
	if (u32TpmPresent == 0u) {
		return 0u;
	}
	if (u32TpmFamily < B5640_TPM_FAMILY_MIN) {
		return 0u;
	}
	return 1u;
}

static uint32_t
b5640_id(void)
{
	return B5640_BATCH_ID;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_install_sb_tpm_gate_ok_5640 - combined SB + TPM install gate.
 *
 * sb_state:     soft SB state (2 = enabled; see batch5632)
 * tpm_present:  non-zero if TPM present (see batch5633)
 * tpm_family:   TPM family major ( >= 2; see batch5635)
 *
 * Returns 1 when all conditions pass, else 0. Pure integer gate;
 * does not probe firmware. No parent wires.
 */
uint32_t
gj_install_sb_tpm_gate_ok_5640(uint32_t u32SbState, uint32_t u32TpmPresent,
    uint32_t u32TpmFamily)
{
	(void)NULL;
	return b5640_gate_ok(u32SbState, u32TpmPresent, u32TpmFamily);
}

/*
 * gj_batch_id_5640 - report this TU's graph batch number.
 *
 * Always returns 5640.
 */
uint32_t
gj_batch_id_5640(void)
{
	return b5640_id();
}

/* ---- underscored aliases ----------------------------------------------- */

uint32_t __gj_install_sb_tpm_gate_ok_5640(uint32_t u32SbState,
    uint32_t u32TpmPresent, uint32_t u32TpmFamily)
    __attribute__((alias("gj_install_sb_tpm_gate_ok_5640")));

uint32_t __gj_batch_id_5640(void)
    __attribute__((alias("gj_batch_id_5640")));
