/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * PE32/PE32+ header + section parse (clean-room MS PE layout) for WoW64 /
 * Proton bring-up. Pure C; dual MIT OR Apache-2.0 — no Wine/GPL paste.
 * Full process spawn stays userspace; kernel validates and can stage/map.
 */
#pragma once

#include <gj/types.h>

#define GJ_PE32_MAX_SECTIONS 16u

struct gj_pe32_info {
    u16 u16Machine;   /* 0x14c = i386, 0x8664 = amd64 */
    u16 u16Chars;
    u32 u32EntryRva;
    u32 u32ImageBase32; /* PE32 Preferred ImageBase (low) */
    u64 u64ImageBase;   /* PE32+ or extended */
    u32 u32SizeOfImage;
    u16 u16Subsystem;
    u8  u8IsPe32;       /* 1 = PE32, 0 = PE32+ */
    u8  u8IsDll;
    u16 u16NumSections;
    u16 u16SizeOfOptional;
    u32 u32E_lfanew;
    u32 u32Ready;
};

struct gj_pe32_section {
    char aName[9]; /* 8 + NUL */
    u32  u32VirtSize;
    u32  u32VirtAddr;
    u32  u32RawSize;
    u32  u32RawPtr;
    u32  u32Chars;
};

/**
 * Parse DOS+NT headers from a buffer. Bounds-checked (e_lfanew/opt overflow).
 * Returns 0 on valid PE, -1 otherwise.
 */
int pe32_parse(const void *pBuf, u32 cbLen, struct gj_pe32_info *pOut);

/**
 * Parse section table after a successful pe32_parse.
 * Rejects truncated tables; returns section count or -1.
 */
int pe32_parse_sections(const void *pBuf, u32 cbLen,
                        const struct gj_pe32_info *pInfo,
                        struct gj_pe32_section *pSec, u32 u32MaxSec);

/**
 * Stage image into pImage (SizeOfImage bytes, zero-filled first).
 * Copies headers + each section's raw data to VirtualAddress.
 * Requires pSec non-NULL when u32NSec > 0; rejects raw/VA overflow.
 * Returns 0 on success.
 */
int pe32_image_stage(const void *pFile, u32 cbFile,
                     const struct gj_pe32_info *pInfo,
                     const struct gj_pe32_section *pSec, u32 u32NSec,
                     void *pImage, u32 cbImage);

/**
 * Map staged image pages into the *active* address space at u64VaBase
 * (page-aligned). Prot: R for all; +X for executable sections.
 * Rejects unaligned base / NULL section table when sections requested.
 * Returns 0 on success.
 */
int pe32_map_user(const void *pImage, u32 cbImage, u64 u64VaBase,
                  const struct gj_pe32_section *pSec, u32 u32NSec);

struct gj_process;

struct gj_pe32_load {
    u64 u64ImageVa;  /* where image is mapped */
    u64 u64Entry;    /* absolute entry VA */
    u64 u64StackTop; /* user stack top */
    u32 u32Ready;
};

/**
 * Stage + map PE into pProc AS. Uses preferred base when free, else high VA.
 * Allocates a small stack. Fills *pOut. Caller creates user thread.
 */
int pe32_load_process(struct gj_process *pProc, const void *pFile, u32 cbFile,
                      struct gj_pe32_load *pOut);

/** Smoke: header + section stage + user map (no thread). */
int pe32_smoke(void);
/** Smoke: PE32+ process load + user thread (after SYSCALL/ring-3 ready). */
int pe32_spawn_smoke(void);
/**
 * Smoke: PE32 (i386) image staged + 64-bit WoW64 trampoline entry
 * (compat mode deferred; trampoline exits after “would enter 32-bit”).
 */
int pe32_wow64_smoke(void);

/**
 * Build a software iret-shaped frame for 32-bit user entry (no hardware jump).
 * Proves CS32/SS/entry/stack layout for true compat far-return later.
 * Returns 0 on valid frame.
 */
int pe32_compat_frame_prepare(u64 u64Entry32, u64 u64Stack32, u64 *pFrameOut,
                              u32 u32FrameWords);

/**
 * Soft-execute a tiny i386 PE payload from staged image (no real 32-bit CS).
 * Recognizes clean-room sequences used in smokes:
 *   mov eax,1; xor ebx,ebx; int 0x80  → exit via i386 Linux NR 1 / 252
 *   int 0x80 subset: read/write/open/close/brk/mmap2(90|192)/path-ish NRs
 * Soft mmap2 returns synthetic VA 0x57000000 (matches hw int80 mmap band).
 * Returns 0 on recognized exit, -1 on unknown opcodes / bounds error.
 * *pExitCode set when non-NULL.
 */
int pe32_i386_soft_exec(const void *pImage, u32 cbImage, u32 u32EntryRva,
                        i32 *pExitCode);

/**
 * Validate GDT CS32 is true compat-mode descriptor + prepare frame.
 * Logs pe32: compat gdt PASS when ready for future hardware far-return.
 */
int pe32_compat_gdt_validate(void);

/**
 * Prepare an iretq-shaped 5-qword frame as if pushed on a 32-bit user stack.
 * pStackMem is a kernel buffer standing in for the top of a user stack page
 * (cbStack >= 40). Writes frame at end-40 and validates CS32 L=0 D=1.
 * Logs pe32: enter prep PASS. Hardware iretq still deferred (needs ring transition).
 */
int pe32_compat_enter_prep(u64 u64Entry32, void *pStackMem, u32 cbStack);

/**
 * Hardware-visible readiness for 32-bit enter: LAR on CS32 + frame prep.
 * Does not execute iretq (still deferred). Logs pe32: hw ready PASS.
 */
int pe32_compat_hw_ready(void);

/**
 * Soft-iretq bridge: prepare frame on stack page, "pop" RIP/CS/RFLAGS/RSP/SS
 * and soft-execute image at entry (no real privilege/CS change).
 * Proves enter_prep → soft-exec pipeline for PE32 smoke payloads.
 */
int pe32_compat_soft_iretq(const void *pImage, u32 cbImage, u32 u32EntryRva,
                           void *pStackMem, u32 cbStack, i32 *pExitCode);

/**
 * Hardware enter: map int3 payload, iretq into CS32, trap #BP records PASS.
 * Returns 0 if pe32_hw_enter_hits increased.
 */
int pe32_hw_enter_smoke(void);

/** Hits / last CS from trap_dispatch PE32 hw enter path. */
u32 pe32_hw_enter_hits(void);
u32 pe32_hw_enter_last_cs(void);

/**
 * int 0x80 counters (implemented in trap_dispatch CS32 path):
 * exit hits / last exit code; total calls / last NR.
 */
u32 pe32_int80_exit_hits(void);
u32 pe32_int80_last_code(void);
u32 pe32_int80_call_hits(void);
u32 pe32_int80_last_nr(void);

/**
 * Hardware PE32 int80 surface smoke (CS32):
 *   exit → trap logs "pe32: int80 exit PASS"
 *   getpid+exit, multi-NR, mmap2, pipe/socket, path open/fstat/vfs_io, …
 * Greppable PASS markers must stay stable for scripts/smoke-all.sh.
 */
int pe32_hw_int80_smoke(void);

/**
 * Stage PE on vfs_ram path, pe32_load_process, enter CS32, int80 exit.
 * Logs pe32: vfs load PASS.
 */
int pe32_vfs_load_smoke(void);
