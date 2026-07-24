/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 77000 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_77000.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_77000 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_77000 \
 *   user/libcgj/tests/cgj_soft_milestone_77000.c -ldl
 * Direct: -DCGJ_SOFT_M77000_DIRECT + graph_batch76991.c … graph_batch77000.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M77000_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 77000
#define CGJ_SOFT_B0 76991
#define CGJ_SOFT_B1 76992
#define CGJ_SOFT_B2 76993
#define CGJ_SOFT_B3 76994
#define CGJ_SOFT_B4 76995
#define CGJ_SOFT_B5 76996
#define CGJ_SOFT_B6 76997
#define CGJ_SOFT_B7 76998
#define CGJ_SOFT_B8 76999
#include "_cgj_soft_m_template.inc"
