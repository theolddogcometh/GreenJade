/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Dash glue over libcgj <sys/stat.h>:
 *   - st_mtime / st_atime / st_ctime aliases (libcgj stores st_*tim[2])
 *   - `struct stat64` is `struct stat` (LP64; dash uses the type name)
 *
 * Dual MIT OR Apache-2.0. Not a dash source.
 */
#ifndef DASH_GJ_SYS_STAT_H
#define DASH_GJ_SYS_STAT_H

#include_next <sys/stat.h>

#ifndef st_mtime
#define st_mtime st_mtim[0]
#endif
#ifndef st_atime
#define st_atime st_atim[0]
#endif
#ifndef st_ctime
#define st_ctime st_ctim[0]
#endif

/*
 * After libcgj declares stat64() / fstat64() / lstat64(), alias the
 * *type* name so `struct stat64` is `struct stat`. Calls to stat64()
 * then resolve to stat() (same ABI on LP64).
 */
#ifndef DASH_GJ_STAT64_TYPE
#define DASH_GJ_STAT64_TYPE
#undef stat64
#define stat64 stat
#endif

#endif /* DASH_GJ_SYS_STAT_H */
