/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Static configure results for vendored dash 0.5.13.5 against libcgj.
 * Glue only — not a relicense of dash (BSD-3-Clause). Dual MIT OR Apache-2.0.
 *
 * HAVE_* here match symbols libcgj actually provides. Omitted on purpose:
 * HAVE_SIGSETMASK, HAVE_PATHS_H, HAVE_GLOB, HAVE_MEMFD_CREATE,
 * HAVE_GETPWNAM, HAVE_HISTEDIT.
 */
#ifndef DASH_GJ_CONFIG_H
#define DASH_GJ_CONFIG_H

#define PACKAGE "dash"
#define PACKAGE_NAME "dash"
#define PACKAGE_STRING "dash 0.5.13.5"
#define PACKAGE_TARNAME "dash"
#define PACKAGE_VERSION "0.5.13.5"
#define PACKAGE_BUGREPORT ""
#define PACKAGE_URL ""
#define VERSION "0.5.13.5"

#define SMALL 1

#define HAVE_ALLOCA_H 1
#define HAVE_BSEARCH 1
#define HAVE_DECL_ISBLANK 1
#define HAVE_MEMRCHR 1
#define HAVE_FACCESSAT 1
#define HAVE_FNMATCH 1
#define HAVE_F_DUPFD_CLOEXEC 1
#define HAVE_GETRLIMIT 1
#define HAVE_ISALPHA 1
#define HAVE_MEMPCPY 1
#define HAVE_STPCPY 1
#define HAVE_STRCHRNUL 1

#define HAVE_STDINT_H 1
#define HAVE_STDIO_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_STRINGS_H 1
#define HAVE_UNISTD_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_SYS_TYPES_H 1

#define HAVE_INTTYPES_H 1
#define HAVE_SYS_WAIT_H 1
#define HAVE_WCHAR_H 1
#define HAVE_KILLPG 1
#define HAVE_STRSIGNAL 1
#define HAVE_STRTOIMAX 1
#define HAVE_STRTOUMAX 1
#define HAVE_SYSCONF 1
#define HAVE_WAIT3 1
#define HAVE_ALIAS_ATTRIBUTE 1

#define STDC_HEADERS 1
#define SIZEOF_INTMAX_T 8
#define SIZEOF_LONG_LONG_INT 8
#define PRIdMAX "ld"

#define HAVE_TEE 1
#define USE_TEE 0
#define USE_MEMFD_CREATE 0

#define _PATH_BSHELL "/bin/sh"
#define _PATH_DEVNULL "/dev/null"
#define _PATH_TTY "/dev/tty"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE 1
#endif

#endif /* DASH_GJ_CONFIG_H */
