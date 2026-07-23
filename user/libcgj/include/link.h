/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <link.h> for libcgj / ld-gj (GreenJade).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * link_map / dl_phdr_info and dl_iterate_phdr for introspection of loaded
 * objects. Complements <dlfcn.h>.
 *
 * Design notes
 * ------------
 * Real iteration is owned by ld-gj when dynamic; static soft fills may report
 * the main executable only.
 *
 * Non-goals
 * ---------
 * Full auditor interface (la_*) surface.
 */
#pragma once

#include <elf.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct dl_phdr_info {
    Elf_Addr        dlpi_addr;
    const char     *dlpi_name;
    const Elf_Phdr *dlpi_phdr;
    Elf_Half        dlpi_phnum;
    /* glibc extensions (zeroed on bring-up) */
    unsigned long long dlpi_adds;
    unsigned long long dlpi_subs;
    size_t          dlpi_tls_modid;
    void           *dlpi_tls_data;
};

int dl_iterate_phdr(int (*pfn)(struct dl_phdr_info *pInfo, size_t cbSize,
                               void *pData),
                    void *pData);

#ifdef __cplusplus
}
#endif
