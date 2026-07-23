/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * dl_iterate_phdr — walk main executable PHDRs via AT_PHDR auxv (bring-up).
 * Soft deepen: load bias from first PT_LOAD, AT_EXECFN name, soft TLS
 * modid, adds counter. Full shared-object iteration is owned by ld-gj later.
 */
#include <elf.h>
#include <link.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/auxv.h>

static unsigned long long g_u64Adds = 1;
static unsigned long long g_u64Subs;

/* PT_LOAD / PT_TLS values when headers omit them. */
#ifndef PT_LOAD
#define PT_LOAD 1
#endif
#ifndef PT_TLS
#define PT_TLS 7
#endif

int
dl_iterate_phdr(int (*pfn)(struct dl_phdr_info *pInfo, size_t cbSize,
			   void *pData),
		void *pData)
{
	struct dl_phdr_info info;
	unsigned long uPhdr;
	unsigned long uPhnum;
	unsigned long uPhent;
	unsigned long uBase;
	unsigned long uExecfn;
	const Elf_Phdr *pPhdr;
	Elf_Addr uBias = 0;
	size_t iPh;
	int nRet;
	int fSawLoad = 0;
	size_t uTlsMod = 0;

	if (pfn == NULL) {
		return 0;
	}

	uPhdr = getauxval(AT_PHDR);
	uPhnum = getauxval(AT_PHNUM);
	uPhent = getauxval(AT_PHENT);
	uBase = getauxval(AT_BASE);
	uExecfn = getauxval(AT_EXECFN);

	if (uPhdr == 0 || uPhnum == 0) {
		return 0;
	}

	/* Soft: require standard phent or accept common 56-byte ELF64 Phdr. */
	if (uPhent != 0 && uPhent != sizeof(Elf_Phdr) &&
	    uPhent != 32 && uPhent != 56) {
		/* Still try with our Elf_Phdr layout. */
	}

	pPhdr = (const Elf_Phdr *)(uintptr_t)uPhdr;

	/*
	 * Load bias: for PIE, first PT_LOAD p_vaddr + bias == mapped VA.
	 * AT_PHDR points into the mapped image; bias ≈ AT_PHDR - p_offset
	 * of the segment containing the headers, or AT_PHDR - p_vaddr when
	 * headers live in a PT_LOAD.
	 */
	for (iPh = 0; iPh < (size_t)uPhnum; iPh++) {
		if (pPhdr[iPh].p_type == PT_LOAD) {
			if (!fSawLoad) {
				/*
				 * Prefer bias from phdr VA vs segment:
				 * if AT_PHDR falls in this segment's file
				 * range, compute bias from vaddr/offset.
				 */
				uintptr_t uSegFile =
				    (uintptr_t)uPhdr - (uintptr_t)pPhdr[iPh].p_offset;
				/* Soft: use AT_PHDR - p_vaddr when p_offset==0 */
				if (pPhdr[iPh].p_offset == 0) {
					uBias = (Elf_Addr)(
					    (uintptr_t)uPhdr -
					    (uintptr_t)pPhdr[iPh].p_vaddr);
				} else {
					uBias = (Elf_Addr)(
					    uSegFile -
					    (uintptr_t)pPhdr[iPh].p_vaddr);
				}
				fSawLoad = 1;
			}
		} else if (pPhdr[iPh].p_type == PT_TLS) {
			uTlsMod = 1; /* main object soft TLS modid */
		}
	}
	/*
	 * Non-PIE ET_EXEC often has absolute vaddrs matching AT_PHDR region
	 * with bias 0. If computed bias looks wild vs AT_BASE, prefer 0.
	 */
	if (!fSawLoad) {
		uBias = 0;
	} else if (uBase == 0 && uBias > 0x100000000ULL) {
		/* Soft guard: huge bias without AT_BASE → treat as absolute */
		uBias = 0;
	}
	(void)uBase;

	memset(&info, 0, sizeof(info));
	info.dlpi_addr = uBias;
	if (uExecfn != 0) {
		info.dlpi_name = (const char *)(uintptr_t)uExecfn;
	} else {
		info.dlpi_name = "";
	}
	info.dlpi_phdr = pPhdr;
	info.dlpi_phnum = (Elf_Half)uPhnum;
	info.dlpi_adds = g_u64Adds;
	info.dlpi_subs = g_u64Subs;
	info.dlpi_tls_modid = uTlsMod;
	info.dlpi_tls_data = NULL;

	nRet = pfn(&info, sizeof(info), pData);
	if (nRet != 0) {
		return nRet;
	}
	return 0;
}
