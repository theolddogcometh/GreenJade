/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft host gates for continuum MILESTONE 88600 markers (CREATE-ONLY).
 * Coherent soft package: this TU + NOTES_milestone_88600.txt + template.
 * Mirror 25800 soft pattern; bar3 still open (gj_bar3_ready_88600 == 0).
 *
 * Manual: cc -std=c11 -Wall -Wextra -O2 -o build/cgj_soft_milestone_88600 \
 *   user/libcgj/tests/cgj_soft_milestone_88600.c -ldl
 * Direct: -DCGJ_SOFT_M88600_DIRECT + graph_batch88591.c … graph_batch88600.c
 */

#define _GNU_SOURCE
#if defined(CGJ_SOFT_M88600_DIRECT)
#define CGJ_SOFT_IS_DIRECT 1
#else
#define CGJ_SOFT_IS_DIRECT 0
#endif
#define CGJ_SOFT_M 88600
#define CGJ_SOFT_B0 88591
#define CGJ_SOFT_B1 88592
#define CGJ_SOFT_B2 88593
#define CGJ_SOFT_B3 88594
#define CGJ_SOFT_B4 88595
#define CGJ_SOFT_B5 88596
#define CGJ_SOFT_B6 88597
#define CGJ_SOFT_B7 88598
#define CGJ_SOFT_B8 88599
#include "_cgj_soft_m_template.inc"
