/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <getopt.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * getopt, getopt_long, getopt_long_only, and optarg/optind/opterr/optopt
 * globals for CLI parsing in tools and daemons.
 *
 * Design notes
 * ------------
 * GNU permutation of argv is commonly expected; keep behavior stable for
 * scripts. struct option matches glibc field order.
 *
 * Non-goals
 * ---------
 * Full argp (see <argp.h>) feature parity in this header.
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern char *optarg;
extern int   optind;
extern int   opterr;
extern int   optopt;

#define no_argument       0
#define required_argument 1
#define optional_argument 2

struct option {
    const char *name;
    int         has_arg;
    int        *flag;
    int         val;
};

int getopt(int argc, char *const argv[], const char *szOptstring);
int getopt_long(int argc, char *const argv[], const char *szOptstring,
                const struct option *pLongopts, int *pLongindex);
int getopt_long_only(int argc, char *const argv[], const char *szOptstring,
                     const struct option *pLongopts, int *pLongindex);

#ifdef __cplusplus
}
#endif
