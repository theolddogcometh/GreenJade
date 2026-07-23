/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * dl_iterate_phdr — walk main executable PHDRs via AT_PHDR auxv (bring-up).
 * Full shared-object iteration is owned by ld-gj later.
 */
#include <link.h>
#include <stddef.h>
#include <string.h>
#include <sys/auxv.h>

int
dl_iterate_phdr(int (*pfn)(struct dl_phdr_info *pInfo, size_t cbSize,
                           void *pData),
                void *pData)
{
    struct dl_phdr_info info;
    unsigned long uPhdr;
    unsigned long uPhnum;
    unsigned long uBase;
    int nRet;

    if (pfn == NULL) {
        return 0;
    }

    uPhdr = getauxval(AT_PHDR);
    uPhnum = getauxval(AT_PHNUM);
    uBase = getauxval(AT_BASE);

    if (uPhdr == 0 || uPhnum == 0) {
        return 0;
    }

    memset(&info, 0, sizeof(info));
    /*
     * For non-PIE ET_EXEC, AT_PHDR is absolute and load bias is 0.
     * For PIE / shared, AT_BASE is the interpreter/load base; bring-up uses 0
     * for the main map when AT_BASE is the ldso base only.
     */
    (void)uBase;
    info.dlpi_addr = 0;
    info.dlpi_name = "";
    info.dlpi_phdr = (const Elf_Phdr *)(uintptr_t)uPhdr;
    info.dlpi_phnum = (Elf_Half)uPhnum;

    nRet = pfn(&info, sizeof(info), pData);
    if (nRet != 0) {
        return nRet;
    }
    return 0;
}
