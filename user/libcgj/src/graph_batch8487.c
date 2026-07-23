/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8487: ET_EXEC ELF object type constant stub.
 *
 * Surface (unique symbols):
 *   uint32_t gj_elf_type_exec_id_8487(void);
 *     - Returns ET_EXEC (2): executable file object type.
 *   uint32_t __gj_elf_type_exec_id_8487  (alias)
 *   __libcgj_batch8487_marker = "libcgj-batch8487"
 *
 * Exclusive continuum CREATE-ONLY (8481-8490: elf_mag0_id, elf_class64_id,
 * elf_data2lsb_id, elf_version_id, elf_osabi_sysv_id, elf_type_dyn_id,
 * elf_type_exec_id, elf_mach_x86_64_id, elf_ident_ok_u, batch_id_8490).
 * Distinct from gj_elf_et_exec_u (batch4964) / gj_elf_type_dyn_id_8486 —
 * unique gj_elf_type_exec_id_8487 surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8487_marker[] = "libcgj-batch8487";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b8487_type_exec(void)
{
	return 2u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_elf_type_exec_id_8487 - ET_EXEC constant (2).
 *
 * Always returns 2, the e_type value for executable ELF objects.
 * Self-contained; no parent wires.
 */
uint32_t
gj_elf_type_exec_id_8487(void)
{
	(void)NULL;
	return b8487_type_exec();
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_elf_type_exec_id_8487(void)
    __attribute__((alias("gj_elf_type_exec_id_8487")));
