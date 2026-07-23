/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped langinfo.h (C locale). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <langinfo.h>. Constants and type sizes
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

#ifdef __cplusplus
extern "C" {
#endif

typedef int nl_item;

#define CODESET     14
#define D_T_FMT     131112
#define D_FMT       131113
#define T_FMT       131114
#define T_FMT_AMPM  131115
#define AM_STR      131110
#define PM_STR      131111
#define DAY_1       131079
#define DAY_2       131080
#define DAY_3       131081
#define DAY_4       131082
#define DAY_5       131083
#define DAY_6       131084
#define DAY_7       131085
#define ABDAY_1     131072
#define ABDAY_2     131073
#define ABDAY_3     131074
#define ABDAY_4     131075
#define ABDAY_5     131076
#define ABDAY_6     131077
#define ABDAY_7     131078
#define MON_1       131098
#define MON_2       131099
#define MON_3       131100
#define MON_4       131101
#define MON_5       131102
#define MON_6       131103
#define MON_7       131104
#define MON_8       131105
#define MON_9       131106
#define MON_10      131107
#define MON_11      131108
#define MON_12      131109
#define ABMON_1     131086
#define ABMON_2     131087
#define ABMON_3     131088
#define ABMON_4     131089
#define ABMON_5     131090
#define ABMON_6     131091
#define ABMON_7     131092
#define ABMON_8     131093
#define ABMON_9     131094
#define ABMON_10    131095
#define ABMON_11    131096
#define ABMON_12    131097
#define RADIXCHAR   65536
#define THOUSEP     65537
#define YESEXPR     327680
#define NOEXPR      327681
#define CRNCYSTR    262159

char *nl_langinfo(nl_item item);

#ifdef __cplusplus
}
#endif
