/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8485: ELFOSABI_SYSV OS ABI constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_osabi_sysv_id_8485(void);
 *     - Returns ELFOSABI_NONE / ELFOSABI_SYSV (0): System V ABI.
 *   uint32_t __gj_elf_osabi_sysv_id_8485  (alias)
 *   __libcgj_batch8485_marker = "libcgj-batch8485"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Unique gj_elf_osabi_sysv_id_8485 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8485_marker[] = "libcgj-batch8485";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8485_osabi_sysv(void)
{
	return 0u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_osabi_sysv_id_8485 - ELFOSABI_SYSV constant (0).
 *
 * Always returns 0, the EI_OSABI value for System V / unspecified ABI.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_osabi_sysv_id_8485(void)
{
	(void)NULL;
	return b8485_osabi_sysv();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_osabi_sysv_id_8485(void)
    __attribute__((alias("gj_elf_osabi_sysv_id_8485")));
