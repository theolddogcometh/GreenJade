/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <linux/audit.h> for libcgj. OpenSSH sandbox-seccomp-filter
 * (HAVE_LINUX_AUDIT_H) needs AUDIT_ARCH_X86_64. ELF machine numbers plus
 * Linux audit arch flags. Dual DoD B OPEN.
 */
#pragma once

#ifndef EM_X86_64
#define EM_X86_64 62
#endif
#ifndef EM_AARCH64
#define EM_AARCH64 183
#endif

#ifndef __AUDIT_ARCH_64BIT
#define __AUDIT_ARCH_64BIT 0x80000000u
#endif
#ifndef __AUDIT_ARCH_LE
#define __AUDIT_ARCH_LE    0x40000000u
#endif

#ifndef AUDIT_ARCH_X86_64
#define AUDIT_ARCH_X86_64 \
    ((unsigned)EM_X86_64 | __AUDIT_ARCH_64BIT | __AUDIT_ARCH_LE)
#endif
#ifndef AUDIT_ARCH_AARCH64
#define AUDIT_ARCH_AARCH64 \
    ((unsigned)EM_AARCH64 | __AUDIT_ARCH_64BIT | __AUDIT_ARCH_LE)
#endif
