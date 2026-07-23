/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8484: ELF EV_CURRENT version constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_version_id_8484(void);
 *     - Returns EV_CURRENT (1): current ELF file format version.
 *   uint32_t __gj_elf_version_id_8484  (alias)
 *   __libcgj_batch8484_marker = "libcgj-batch8484"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Unique gj_elf_version_id_8484 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8484_marker[] = "libcgj-batch8484";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8484_version(void)
{
	return 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_version_id_8484 - EV_CURRENT constant (1).
 *
 * Always returns 1, the EI_VERSION / e_version value for current ELF.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_version_id_8484(void)
{
	(void)NULL;
	return b8484_version();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_version_id_8484(void)
    __attribute__((alias("gj_elf_version_id_8484")));
