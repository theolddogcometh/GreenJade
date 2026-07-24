/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 90000 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_90000.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_90000 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_90000 \
 *   user/libcgj/tests/cgj_soft_milestone_90000.c -ldl
 * Direct: -DCGJ_SOFT_M90000_DIRECT + graph_batch89991.c … graph_batch90000.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M90000_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 90000
#define CGJ_SOFT_B0 89991
#define CGJ_SOFT_B1 89992
#define CGJ_SOFT_B2 89993
#define CGJ_SOFT_B3 89994
#define CGJ_SOFT_B4 89995
#define CGJ_SOFT_B5 89996
#define CGJ_SOFT_B6 89997
#define CGJ_SOFT_B7 89998
#define CGJ_SOFT_B8 89999
#include "_cgj_soft_m_template.inc"
