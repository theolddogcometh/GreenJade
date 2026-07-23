/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/fanotify.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <sys/fanotify.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
 */
#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FAN_ACCESS         0x00000001
#define FAN_MODIFY         0x00000002
#define FAN_CLOSE_WRITE    0x00000008
#define FAN_CLOSE_NOWRITE  0x00000010
#define FAN_OPEN           0x00000020
#define FAN_Q_OVERFLOW     0x00004000
#define FAN_OPEN_PERM      0x00010000
#define FAN_ACCESS_PERM    0x00020000
#define FAN_ONDIR          0x40000000
#define FAN_EVENT_ON_CHILD 0x08000000

#define FAN_CLOEXEC        0x00000001
#define FAN_NONBLOCK       0x00000002
#define FAN_CLASS_NOTIF    0x00000000
#define FAN_CLASS_CONTENT  0x00000004
#define FAN_CLASS_PRE_CONTENT 0x00000008
#define FAN_UNLIMITED_QUEUE 0x00000010
#define FAN_UNLIMITED_MARKS 0x00000020

#define FAN_MARK_ADD       0x00000001
#define FAN_MARK_REMOVE    0x00000002
#define FAN_MARK_DONT_FOLLOW 0x00000004
#define FAN_MARK_ONLYDIR   0x00000008
#define FAN_MARK_MOUNT     0x00000010
#define FAN_MARK_IGNORED_MASK 0x00000020
#define FAN_MARK_IGNORED_SURV_MODIFY 0x00000040
#define FAN_MARK_FLUSH     0x00000080

struct fanotify_event_metadata {
    uint32_t event_len;
    uint8_t  vers;
    uint8_t  reserved;
    uint16_t metadata_len;
    uint64_t mask;
    int32_t  fd;
    int32_t  pid;
};

int fanotify_init(unsigned uFlags, unsigned uEventFflags);
int fanotify_mark(int nFanotifyFd, unsigned uFlags, uint64_t u64Mask,
                  int nDfd, const char *szPathname);

#ifdef __cplusplus
}
#endif
