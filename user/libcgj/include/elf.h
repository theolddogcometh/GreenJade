/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room ELF types/constants for libcgj / ld-gj consumers.
 * Not binutils or GNU libc headers; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Elf*_Ehdr/Phdr/Shdr/Dyn/Sym shapes and common EM_*/PT_*/DT_*/STT_* macros
 * needed by dynlink and tools. Kernel has a separate loader surface.
 *
 * Design notes
 * ------------
 * Prefer matching System V / Linux ELF numbers used by product arches
 * (x86_64, aarch64). Expand only when a real relocation/graph needs it.
 *
 * Non-goals
 * ---------
 * Full ELF gABI encyclopedia of every OS-specific note.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t Elf32_Half;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf64_Word;
typedef int32_t  Elf32_Sword;
typedef int32_t  Elf64_Sword;
typedef uint64_t Elf32_Xword;
typedef uint64_t Elf64_Xword;
typedef int64_t  Elf32_Sxword;
typedef int64_t  Elf64_Sxword;
typedef uint32_t Elf32_Addr;
typedef uint64_t Elf64_Addr;
typedef uint32_t Elf32_Off;
typedef uint64_t Elf64_Off;

typedef struct {
    Elf64_Word  p_type;
    Elf64_Word  p_flags;
    Elf64_Off   p_offset;
    Elf64_Addr  p_vaddr;
    Elf64_Addr  p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

typedef struct {
    Elf32_Word p_type;
    Elf32_Off  p_offset;
    Elf32_Addr p_vaddr;
    Elf32_Addr p_paddr;
    Elf32_Word p_filesz;
    Elf32_Word p_memsz;
    Elf32_Word p_flags;
    Elf32_Word p_align;
} Elf32_Phdr;

#if defined(__x86_64__) || defined(__aarch64__) || defined(__LP64__)
typedef Elf64_Phdr Elf_Phdr;
typedef Elf64_Addr Elf_Addr;
typedef Elf64_Half Elf_Half;
typedef Elf64_Word Elf_Word;
#else
typedef Elf32_Phdr Elf_Phdr;
typedef Elf32_Addr Elf_Addr;
typedef Elf32_Half Elf_Half;
typedef Elf32_Word Elf_Word;
#endif

/* Program header types (subset) */
#define PT_NULL    0
#define PT_LOAD    1
#define PT_DYNAMIC 2
#define PT_INTERP  3
#define PT_NOTE    4
#define PT_PHDR    6
#define PT_TLS     7

#define PF_X 1
#define PF_W 2
#define PF_R 4

#ifdef __cplusplus
}
#endif
