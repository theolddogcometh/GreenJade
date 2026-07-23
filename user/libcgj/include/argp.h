/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped argp.h (bring-up subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <argp.h>. Constants and type sizes
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

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __error_t_defined
#define __error_t_defined 1
typedef int error_t;
#endif

#define ARGP_ERR_UNKNOWN ((error_t)0xFFFFFF)

#define OPTION_ARG_OPTIONAL 0x1
#define OPTION_HIDDEN       0x2
#define OPTION_ALIAS        0x4
#define OPTION_DOC          0x8
#define OPTION_NO_USAGE     0x10

struct argp_option {
    const char *name;
    int         key;
    const char *arg;
    int         flags;
    const char *doc;
    int         group;
};

struct argp_state;

typedef error_t (*argp_parser_t)(int nKey, char *szArg,
                                 struct argp_state *pState);

struct argp {
    const struct argp_option *options;
    argp_parser_t             parser;
    const char               *args_doc;
    const char               *doc;
    const struct argp *const *children;
    char *(*help_filter)(int nKey, const char *szText, void *pInput);
    const char *argp_domain;
};

struct argp_child {
    const struct argp *argp;
    int                flags;
    const char        *header;
    int                group;
};

struct argp_state {
    const struct argp *root_argp;
    int                argc;
    char             **argv;
    int                next;
    unsigned           flags;
    unsigned           arg_num;
    int                quoted;
    void              *input;
    void             **child_inputs;
    void              *hook;
    char              *name;
    FILE              *err_stream;
    FILE              *out_stream;
    void              *pstate;
};

#define ARGP_PARSE_ARGV0  0x01
#define ARGP_NO_ERRS      0x02
#define ARGP_NO_EXIT      0x04
#define ARGP_NO_HELP      0x08
#define ARGP_NO_ARGS      0x10
#define ARGP_IN_ORDER     0x20
#define ARGP_LONG_ONLY    0x40
#define ARGP_SILENT       (ARGP_NO_EXIT | ARGP_NO_ERRS | ARGP_NO_HELP)

#define ARGP_KEY_ARG      0
#define ARGP_KEY_END      0x1000001
#define ARGP_KEY_NO_ARGS  0x1000002
#define ARGP_KEY_INIT     0x1000003
#define ARGP_KEY_SUCCESS  0x1000004
#define ARGP_KEY_ERROR    0x1000005
#define ARGP_KEY_FINI     0x1000007

error_t argp_parse(const struct argp *pArgp, int nArgc, char **ppArgv,
                   unsigned uFlags, int *pArgIndex, void *pInput);
void argp_help(const struct argp *pArgp, FILE *pStream, unsigned uFlags,
               char *szName);
void argp_usage(const struct argp_state *pState);
void argp_state_help(const struct argp_state *pState, FILE *pStream,
                     unsigned uFlags);
void argp_error(const struct argp_state *pState, const char *szFmt, ...);
void argp_failure(const struct argp_state *pState, int nStatus, int nErrnum,
                  const char *szFmt, ...);

extern const char *argp_program_version;
extern const char *argp_program_bug_address;
extern void (*argp_program_version_hook)(FILE *pStream,
                                         struct argp_state *pState);
extern error_t argp_err_exit_status;

#ifdef __cplusplus
}
#endif
