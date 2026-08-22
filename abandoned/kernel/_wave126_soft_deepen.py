#!/usr/bin/env python3
"""Wave 126 exclusive soft deepen: retgradientangle + retblendangle (kernel/** only)."""
from __future__ import annotations

import os
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent

# Match retstrokeangle stamp pair (comment + emit). Keep historical wave=125 on those lines.
RE_STROKE = re.compile(
    r"(?P<comment>/\* Grep: (?P<label>[^*]*?)retstrokeangle — Wave 125 exclusive strokeangle stamp \*/\n)"
    r"(?P<indent>[ \t]*)"
    r"(?P<emit>(?:kprintf|aarch64_uart_puts)\("
    r'"(?P<prefix>[^"]*?)retstrokeangle exclusive=1 soft_ne_product=1 product_kernel=OPEN bar3=0 wave=125 \(retstrokeangle stamp; Soft≠product\)\\n"\);\n)',
)

RE_WAVE_DEFINE = re.compile(
    r"(#define\s+\w*(?:SOFT(?:_DEEPEN)?_WAVE|SOFT_WAVE)\s+)125u"
)
RE_MAIN_WAVE = re.compile(r"(MAIN_SOFT_SMOKE_WAVE\s*=\s*)125u")
RE_MAIN_AREAS = re.compile(r"(MAIN_SOFT_SMOKE_AREAS\s*=\s*)228u")
RE_AREAS_DEFINE = re.compile(
    r"(#define\s+\w*(?:SOFT(?:_DEEPEN)?_AREAS|SOFT_AREAS)\s+)(\d+)u"
)
RE_HARD_DEEPEN = re.compile(r"wave=125 areas=187")
RE_CATALOG = re.compile(r"retpaletteangle,retstrokeangle")
RE_GREP_WAVE_COMMENT = re.compile(r"(greppable wave=)125")
RE_DEEPEN_COMMENT = re.compile(r"(soft deepen wave=)125")
RE_WAVE125_COMMENT = re.compile(r"(Wave \d+ exclusive soft deepen stamp \(greppable wave=)125(\))")
RE_WAVE_STAMP_COMMENT = re.compile(
    r"(soft inventory stamp \(greppable wave=)125(\))"
)


def bump_areas(m: re.Match[str]) -> str:
    n = int(m.group(2)) + 2
    return f"{m.group(1)}{n}u"


def insert_new_surfaces(m: re.Match[str]) -> str:
    label = m.group("label")  # e.g. "soft: " or "cpu: soft " or "aarch64: uart: soft "
    indent = m.group("indent")
    prefix = m.group("prefix")  # e.g. "soft: " or "cpu: soft " or "aarch64: uart: soft "
    emit_fn = "kprintf" if m.group("emit").lstrip().startswith("kprintf") else "aarch64_uart_puts"

    # Preserve exact indent of the emit line for new stamps
    emit_indent = indent

    gradient_comment = (
        f"/* Grep: {label}retgradientangle — Wave 126 return-gradientangle honesty */\n"
    )
    gradient_emit = (
        f'{emit_indent}{emit_fn}("{prefix}retgradientangle soft_only=1 product_gate=0 '
        f'soft_ne_product=1 never_blocks_m0=1 wave=126 '
        f'(retgradientangle honesty; Soft≠product; not bar3)\\n");\n'
    )
    blend_comment = (
        f"/* Grep: {label}retblendangle — Wave 126 exclusive blendangle stamp */\n"
    )
    blend_emit = (
        f'{emit_indent}{emit_fn}("{prefix}retblendangle exclusive=1 soft_ne_product=1 '
        f'product_kernel=OPEN bar3=0 wave=126 '
        f'(retblendangle stamp; Soft≠product)\\n");\n'
    )
    return m.group(0) + gradient_comment + gradient_emit + blend_comment + blend_emit


def process(text: str) -> tuple[str, dict[str, int]]:
    stats = {
        "insert": 0,
        "wave_def": 0,
        "areas_def": 0,
        "catalog": 0,
        "hard_deepen": 0,
        "comments": 0,
    }

    # Idempotent: skip insert if Wave 126 surfaces already present.
    if "retgradientangle" not in text:
        new_text, n = RE_STROKE.subn(insert_new_surfaces, text)
        stats["insert"] = n
        text = new_text
    else:
        text = text

    # Only bump wave/areas when still on 125 (idempotent).
    if re.search(r"(?:SOFT(?:_DEEPEN)?_WAVE|SOFT_WAVE|MAIN_SOFT_SMOKE_WAVE)\s*=?\s*125u", text) or re.search(
        r"#define\s+\w*SOFT(?:_DEEPEN)?_WAVE\s+125u", text
    ) or "MAIN_SOFT_SMOKE_WAVE = 125u" in text:
        def wave_def_sub(m: re.Match[str]) -> str:
            stats["wave_def"] += 1
            return m.group(1) + "126u"

        text = RE_WAVE_DEFINE.sub(wave_def_sub, text)

        def main_wave_sub(m: re.Match[str]) -> str:
            stats["wave_def"] += 1
            return m.group(1) + "126u"

        text = RE_MAIN_WAVE.sub(main_wave_sub, text)

        def main_areas_sub(m: re.Match[str]) -> str:
            stats["areas_def"] += 1
            return m.group(1) + "230u"

        text = RE_MAIN_AREAS.sub(main_areas_sub, text)

        def areas_sub(m: re.Match[str]) -> str:
            stats["areas_def"] += 1
            return bump_areas(m)

        text = RE_AREAS_DEFINE.sub(areas_sub, text)

        def hard_sub(m: re.Match[str]) -> str:
            stats["hard_deepen"] += 1
            return "wave=126 areas=189"

        text = RE_HARD_DEEPEN.sub(hard_sub, text)

        # Comment stamp bumps for current-wave greppable markers
        def grep_wave_sub(m: re.Match[str]) -> str:
            stats["comments"] += 1
            return m.group(1) + "126"

        text = RE_GREP_WAVE_COMMENT.sub(grep_wave_sub, text)
        text = RE_DEEPEN_COMMENT.sub(grep_wave_sub, text)

        def wave_paren_sub(m: re.Match[str]) -> str:
            stats["comments"] += 1
            return m.group(1) + "126" + m.group(2)

        text = RE_WAVE125_COMMENT.sub(wave_paren_sub, text)
        text = RE_WAVE_STAMP_COMMENT.sub(wave_paren_sub, text)

    def cat_sub(m: re.Match[str]) -> str:
        stats["catalog"] += 1
        return "retpaletteangle,retstrokeangle,retgradientangle,retblendangle"

    text = RE_CATALOG.sub(cat_sub, text)

    return text, stats


def main() -> int:
    files = sorted(ROOT.rglob("*.c"))
    touched = []
    total = {
        "insert": 0,
        "wave_def": 0,
        "areas_def": 0,
        "catalog": 0,
        "hard_deepen": 0,
        "comments": 0,
    }
    for path in files:
        # skip this helper
        if path.name.startswith("_wave126"):
            continue
        raw = path.read_text(encoding="utf-8", errors="surrogateescape")
        if "retstrokeangle" not in raw and "MAIN_SOFT_SMOKE_WAVE = 125u" not in raw:
            # still allow wave-125 define-only files that have retstrokeangle elsewhere
            if "SOFT_WAVE 125u" not in raw and "SOFT_DEEPEN_WAVE 125u" not in raw:
                continue
            if "retstrokeangle" not in raw:
                continue
        if "retstrokeangle" not in raw:
            continue
        new, stats = process(raw)
        if new != raw:
            path.write_text(new, encoding="utf-8", errors="surrogateescape")
            touched.append((str(path.relative_to(ROOT)), stats))
            for k, v in stats.items():
                total[k] += v

    print(f"touched_files={len(touched)}")
    print(f"totals={total}")
    for rel, st in touched:
        print(f"  {rel}: {st}")
    if total["insert"] == 0:
        print("ERROR: no retgradientangle insertions", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
