/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 89000 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_89000.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_89000 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_89000 \
 *   user/libcgj/tests/cgj_soft_milestone_89000.c -ldl
 * Direct: -DCGJ_SOFT_M89000_DIRECT + graph_batch88991.c … graph_batch89000.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M89000_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 89000
#define CGJ_SOFT_B0 88991
#define CGJ_SOFT_B1 88992
#define CGJ_SOFT_B2 88993
#define CGJ_SOFT_B3 88994
#define CGJ_SOFT_B4 88995
#define CGJ_SOFT_B5 88996
#define CGJ_SOFT_B6 88997
#define CGJ_SOFT_B7 88998
#define CGJ_SOFT_B8 88999
#include "_cgj_soft_m_template.inc"
