/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch102: minimal ELF64 parse helpers.
 * Clean-room freestanding pure C — no full loader (ld-gj owns that).
 * Integer/pointer only; no string libc, no SSE required.
 *
 * Surface:
 *   int gj_elf64_validate(const void *img, size_t len);
 *     Check EI_MAG / ELFCLASS64 / ELFDATA2LSB / EM_X86_64.
 *     Returns 0 on success, -1 on error.
 *   const void *gj_elf64_phdr(const void *img, size_t len, unsigned idx);
 *     Return pointer to program header idx, or NULL if out of range.
 *   int gj_elf64_find_dynamic(const void *img, size_t len,
 *                             unsigned long *dyn_va, unsigned long *dyn_sz);
 *     Locate first PT_DYNAMIC; store p_vaddr and p_filesz.
 *     Returns 0 on success, -1 if absent / invalid.
 *   __libcgj_batch102_marker = "libcgj-batch102"
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch102_marker[] = "libcgj-batch102";

/* ---- ELF64 constants (local; no dependency on incomplete elf.h Ehdr) --- */

#define B102_EI_MAG0       0
#define B102_EI_MAG1       1
#define B102_EI_MAG2       2
#define B102_EI_MAG3       3
#define B102_EI_CLASS      4
#define B102_EI_DATA       5
#define B102_EI_VERSION    6

#define B102_ELFMAG0       0x7fu
#define B102_ELFMAG1       'E'
#define B102_ELFMAG2       'L'
#define B102_ELFMAG3       'F'
#define B102_ELFCLASS64    2u
#define B102_ELFDATA2LSB   1u
#define B102_EV_CURRENT    1u
#define B102_EM_X86_64     62u
#define B102_PT_DYNAMIC    2u

#define B102_EHDR_SIZE     64u
#define B102_PHDR_MIN      56u

/*
 * Local layout mirrors System V ELF64 on little-endian x86_64.
 * Packed so freestanding hosts never insert padding.
 */
struct b102_ehdr {
	unsigned char aIdent[16];
	uint16_t u16Type;
	uint16_t u16Machine;
	uint32_t u32Version;
	uint64_t u64Entry;
	uint64_t u64Phoff;
	uint64_t u64Shoff;
	uint32_t u32Flags;
	uint16_t u16Ehsize;
	uint16_t u16Phentsize;
	uint16_t u16Phnum;
	uint16_t u16Shentsize;
	uint16_t u16Shnum;
	uint16_t u16Shstrndx;
} __attribute__((packed));

struct b102_phdr {
	uint32_t u32Type;
	uint32_t u32Flags;
	uint64_t u64Offset;
	uint64_t u64Vaddr;
	uint64_t u64Paddr;
	uint64_t u64Filesz;
	uint64_t u64Memsz;
	uint64_t u64Align;
} __attribute__((packed));

/* ---- internal helpers -------------------------------------------------- */

static int
b102_hdr_ok(const void *pImg, size_t cb)
{
	const unsigned char *p;

	if (pImg == NULL || cb < B102_EHDR_SIZE) {
		return 0;
	}
	p = (const unsigned char *)pImg;
	if (p[B102_EI_MAG0] != B102_ELFMAG0 ||
	    p[B102_EI_MAG1] != B102_ELFMAG1 ||
	    p[B102_EI_MAG2] != B102_ELFMAG2 ||
	    p[B102_EI_MAG3] != B102_ELFMAG3) {
		return 0;
	}
	if (p[B102_EI_CLASS] != B102_ELFCLASS64) {
		return 0;
	}
	if (p[B102_EI_DATA] != B102_ELFDATA2LSB) {
		return 0;
	}
	/* EI_VERSION is part of the ident block; require current. */
	if (p[B102_EI_VERSION] != B102_EV_CURRENT) {
		return 0;
	}
	{
		const struct b102_ehdr *pEh =
		    (const struct b102_ehdr *)(const void *)pImg;
		if (pEh->u16Machine != (uint16_t)B102_EM_X86_64) {
			return 0;
		}
	}
	return 1;
}

/*
 * Return e_phentsize if table is in-bounds and each entry is large enough
 * for Elf64_Phdr fields we read; else 0.
 */
static uint16_t
b102_phentsize_ok(const struct b102_ehdr *pEh, size_t cb)
{
	uint64_t u64Need;
	uint16_t u16Ent;

	if (pEh == NULL) {
		return 0;
	}
	u16Ent = pEh->u16Phentsize;
	if (u16Ent < (uint16_t)B102_PHDR_MIN || pEh->u16Phnum == 0u) {
		return 0;
	}
	/* Overflow-safe: phoff + phnum * phentsize <= cb */
	if (pEh->u64Phoff > (uint64_t)cb) {
		return 0;
	}
	u64Need = (uint64_t)pEh->u16Phnum * (uint64_t)u16Ent;
	if (u64Need > (uint64_t)cb - pEh->u64Phoff) {
		return 0;
	}
	return u16Ent;
}

/* ---- public: gj_elf64_validate ----------------------------------------- */

/*
 * Validate ELF64 LE x86_64 identity: magic, class, endian, machine.
 * Does not verify program headers or that the image is loadable.
 */
int
gj_elf64_validate(const void *pImg, size_t cb)
{
	if (!b102_hdr_ok(pImg, cb)) {
		return -1;
	}
	return 0;
}

/* ---- public: gj_elf64_phdr --------------------------------------------- */

/*
 * Return a pointer into img for program header index idx.
 * Requires a validated identity and an in-bounds phdr table.
 * Returns NULL on any failure (bad image, idx past e_phnum, truncated).
 */
const void *
gj_elf64_phdr(const void *pImg, size_t cb, unsigned uIdx)
{
	const struct b102_ehdr *pEh;
	uint16_t u16Ent;
	uint64_t u64Off;

	if (!b102_hdr_ok(pImg, cb)) {
		return NULL;
	}
	pEh = (const struct b102_ehdr *)(const void *)pImg;
	u16Ent = b102_phentsize_ok(pEh, cb);
	if (u16Ent == 0u) {
		return NULL;
	}
	if (uIdx >= (unsigned)pEh->u16Phnum) {
		return NULL;
	}
	u64Off = pEh->u64Phoff + (uint64_t)uIdx * (uint64_t)u16Ent;
	/* phentsize_ok already proved full table fits; recheck for clarity */
	if (u64Off + (uint64_t)B102_PHDR_MIN > (uint64_t)cb) {
		return NULL;
	}
	return (const void *)((const unsigned char *)pImg + u64Off);
}

/* ---- public: gj_elf64_find_dynamic ------------------------------------- */

/*
 * Find the first PT_DYNAMIC program header.
 * On success writes:
 *   *pDynVa = p_vaddr   (pre-bias virtual address of .dynamic)
 *   *pDynSz = p_filesz  (file size of the dynamic segment)
 * Returns 0 on success, -1 if no PT_DYNAMIC or image is invalid.
 * Out pointers must be non-NULL.
 */
int
gj_elf64_find_dynamic(const void *pImg, size_t cb, unsigned long *pDynVa,
                      unsigned long *pDynSz)
{
	const struct b102_ehdr *pEh;
	uint16_t u16Ent;
	unsigned i;

	if (pDynVa == NULL || pDynSz == NULL) {
		return -1;
	}
	if (!b102_hdr_ok(pImg, cb)) {
		return -1;
	}
	pEh = (const struct b102_ehdr *)(const void *)pImg;
	u16Ent = b102_phentsize_ok(pEh, cb);
	if (u16Ent == 0u) {
		return -1;
	}

	for (i = 0; i < (unsigned)pEh->u16Phnum; i++) {
		const struct b102_phdr *pPh;
		uint64_t u64Off;

		u64Off = pEh->u64Phoff + (uint64_t)i * (uint64_t)u16Ent;
		if (u64Off + (uint64_t)B102_PHDR_MIN > (uint64_t)cb) {
			return -1;
		}
		pPh = (const struct b102_phdr *)(const void *)
		    ((const unsigned char *)pImg + u64Off);
		if (pPh->u32Type != B102_PT_DYNAMIC) {
			continue;
		}
		/* Empty dynamic is treated as absent. */
		if (pPh->u64Filesz == 0u) {
			return -1;
		}
		*pDynVa = (unsigned long)pPh->u64Vaddr;
		*pDynSz = (unsigned long)pPh->u64Filesz;
		return 0;
	}
	return -1;
}
