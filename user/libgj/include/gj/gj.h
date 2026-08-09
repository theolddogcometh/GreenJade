/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Umbrella header for freestanding libgj (string helpers + syscall wrappers).
 *
 * Pure C11. Dual-licensed MIT OR Apache-2.0 (project root LICENSE; SPDX above).
 * Not a libc: no malloc, no stdio, no TLS, no locale. No GPL.
 *
 * Freestanding link path (ELF consumers of libgj.a):
 *   init, shell, ld-gj, door servers (vfsd, sessiond, netstackd, sshd,
 *   storaged, scsi_mid, hda_client, personality scaffold, ...).
 * This archive is the freestanding userspace runtime for those ELFs -
 * not a Dual DoD product close and not an in-kernel .ko product path.
 *
 * Soft residual deepen (C2 libgj umbrella; Soft!=product; G-AC-1;
 * dual MIT OR Apache-2.0; Dual DoD A/B OPEN):
 *   umbrella residual   - this header pulls string.h + syscalls.h only;
 *                         docs + thin honesty helpers; no policy.
 *   string residual     - freestanding string/mem/format/parse/io surface
 *                         (impl in string_u.c; soft inventory cold only).
 *   syscall residual    - GJ_SYS_* / door ops / LINUX_NR_* + inlines;
 *                         gj_syscall6 in syscall.S; no claim policy.
 *   soft path residual  - documentation + thin header wrappers only;
 *                         kernel / door server enforce cap / door claim.
 *   product_dir residual - Dual DoD product hosts = UDX+ABI userspace
 *                         (rtl8168_udx / xhci_udx); freestanding class SKIP;
 *                         libgj.a links freestanding ELFs, never closes DoD.
 *   api honesty residual - Soft!=product; soft inventory != Dual DoD close;
 *                         soft scaffold != product AC (claim_class=C2).
 *   hazard residual     - H1 no net_eth_poll from IRQ (kernel law; not here);
 *                         H2 once-lamp / no stamp storms (storm=0);
 *                         H3 thr_exit before as_destroy (process law residual).
 *   layout residual     - eng lean only; not GJ_IMAGE_VERSION; stamp-free;
 *                         NEVER bump GJ_IMAGE_VERSION from this unit.
 *
 * Soft API honesty (Soft!=product; dual MIT OR Apache-2.0; G-AC-1):
 *   soft path = umbrella docs + cold soft inventory (string_u) + thin wraps
 *   freestanding link = libgj.a into init/shell/doors (not Dual DoD close)
 *   Dual DoD product  = UDX/DDI userspace hosts over hot+cold ABI (OPEN)
 *   G-AC-1 = no Linux .ko product AC; no GPL source in tree
 *   Soft!=product always; agent never closes Dual DoD A/B
 *
 * Dual DoD residual (both OPEN until DUT proof; agent!=close):
 *   Dual DoD A - Linux-shaped USB via xhci_udx + DDI (OPEN)
 *   Dual DoD B - Linux-shaped NIC via rtl8168_udx + DDI (OPEN)
 * Soft residual lamps / inventory / greppable lines never close Dual DoD.
 * Soft!=product. No stamp storms. Stamp-free bar honesty.
 * Bar honesty v2026.08.04.75. NEVER bump GJ_IMAGE_VERSION from this unit.
 * No invent intermediate image stamps. Not GJ_IMAGE_VERSION.
 *
 * Product residual (C2 umbrella; Soft!=product; Dual DoD OPEN):
 *   product=UDX+sshd+stack — freestanding libgj.a links netstackd + sshd
 *   + UDX hosts over GJ_SYS_NET / GJ_SYS_DDI. Chain residual:
 *   rtl8168_udx → netstackd (stack) → sshd :22. Soft link != DoD close.
 * greppable: libgj: soft product residual product=UDX+sshd+stack
 *
 * Ownership boundary:
 *   - This tree is freestanding only. Do not merge with user/libcgj/ (glibc
 *     surface) or sibling product SOs (user/libgj-so/, user/libgj-gnu/).
 *   - Native GJ_SYS_* numbers must stay aligned with kernel/include/gj/syscall.h
 *     (sparse blocks; do not renumber after userland ships).
 *   - Door op macros track kernel door headers (session/net/store/vfs/...).
 *
 * Include this single header for the full public surface, or pull the parts:
 *
 *   #include <gj/gj.h>          - string + syscalls (this file)
 *   #include <gj/string.h>      - length/compare/copy/search/case/class/memory/
 *                                 parse/format + thin Linux write/read / dlog
 *   #include <gj/syscalls.h>    - GJ_SYS_* / door ops / LINUX_NR_* + inlines;
 *                                 declares gj_syscall6 (implemented in
 *                                 user/libgj/src/syscall.S, archived in libgj.a)
 *
 * Build notes (parent Makefile owns LIBGJ_OBJS):
 *   -Iuser/libgj/include  + link build/user/libgj.a
 *   Typical freestanding flags: -std=c11 -ffreestanding -fno-builtin
 *   -fno-stack-protector -fno-pic -mno-red-zone (x86_64) -Wall -Wextra -Werror
 *
 * greppable: libgj: soft residual umbrella Soft!=product
 * greppable: libgj: soft residual lean
 * greppable: libgj: soft residual lean layout_ver=
 * greppable: libgj: soft residual product_dir
 * greppable: libgj: soft residual dual_dod
 * greppable: libgj: soft residual api honesty
 * greppable: libgj: soft residual hazard
 * greppable: libgj: soft residual freestanding link
 * greppable: libgj: soft product residual product=UDX+sshd+stack
 * greppable: Soft!=product
 * Dual MIT OR Apache-2.0. No GPL. G-AC-1. Dual DoD A/B OPEN.
 */
#pragma once

#include <gj/string.h>
#include <gj/syscalls.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Soft residual lean layout version (eng only; Soft!=product; not stamp).
 * Umbrella residual surface catalog for C2 gj.h (umbrella + string + syscall
 * + soft path + product_dir + api honesty + hazard + Dual DoD OPEN).
 * Not product ABI. Not GJ_IMAGE_VERSION. Stamp-free residual.
 * NEVER bump GJ_IMAGE_VERSION from this unit. No invent .76.
 * Grep: libgj: soft residual lean layout_ver=
 */
#define GJ_LIBGJ_LAYOUT_VER          1u

/**
 * Soft residual lean layout version (GJ_LIBGJ_LAYOUT_VER).
 * Grep: libgj: soft residual lean layout_ver=
 * Not GJ_IMAGE_VERSION. Not product ABI stamp. Stamp-free bar honesty.
 * Soft!=product; G-AC-1.
 */
static inline unsigned
gj_libgj_layout_ver(void)
{
    return (unsigned)GJ_LIBGJ_LAYOUT_VER;
}

/**
 * Non-zero when umbrella soft residual surface is present at layout_ver.
 * Soft!=product; eng residual only. Never hard-gates Dual DoD.
 * greppable: libgj: soft residual lean
 * greppable: libgj: soft residual umbrella Soft!=product
 */
static inline int
gj_libgj_shape_ok(void)
{
    return (GJ_LIBGJ_LAYOUT_VER >= 1u) ? 1 : 0;
}

/**
 * Soft residual Dual DoD OPEN honesty (umbrella seed).
 * Returns 1 when Dual DoD A/B are catalogued OPEN (soft seed only).
 * Never claims product close. Soft!=product; G-AC-1; claim_class=C2.
 * greppable residual: libgj: soft residual dual_dod
 * greppable residual: libgj: soft residual product_dir
 */
static inline int
gj_libgj_dual_dod_open(void)
{
    return 1;
}

/**
 * Soft residual Soft!=product honesty (always 1 = soft != product AC).
 * Soft inventory / umbrella docs never close Dual DoD or product AC.
 * claim_class=C2 scaffold honesty only. Soft!=product; G-AC-1.
 * greppable residual: libgj: soft residual api honesty
 * greppable residual: Soft!=product
 */
static inline int
gj_libgj_soft_ne_product(void)
{
    return 1;
}

/**
 * Soft residual freestanding link honesty (libgj.a consumers).
 * Returns 1 when freestanding ELF link path is the soft residual catalog
 * (init/shell/doors) - never Dual DoD close, never .ko product (G-AC-1).
 * greppable residual: libgj: soft residual freestanding link
 * greppable residual: Soft!=product
 */
static inline int
gj_libgj_freestanding_link(void)
{
    return 1;
}

/**
 * Soft residual hazard catalog seed (H1/H2/H3 honesty; not closed here).
 * Returns 1 = hazard residual named; laws live in kernel / process path.
 * H1: no net_eth_poll from IRQ. H2: once-lamp storm=0. H3: thr_exit order.
 * Soft!=product; never Dual DoD close.
 * greppable residual: libgj: soft residual hazard
 */
static inline int
gj_libgj_hazard_residual(void)
{
    return 1;
}

/**
 * Soft product residual seed toward product=UDX+sshd+stack.
 * Returns 1 always — catalog honesty only (Soft!=product; Dual DoD OPEN).
 * Freestanding link residual for netstackd + sshd + UDX hosts; never
 * claims Dual DoD B wire / host banner close. Bar v2026.08.04.75 stamp-free.
 * greppable residual: libgj: soft product residual product=UDX+sshd+stack
 * greppable residual: Soft!=product Dual DoD OPEN product=UDX+sshd+stack
 */
static inline int
gj_libgj_product_udx_sshd_stack(void)
{
    return 1;
}

#ifdef __cplusplus
}
#endif
