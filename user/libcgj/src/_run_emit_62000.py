#!/usr/bin/env python3
# Inline self-executing CREATE-ONLY for 61001..62000 (imports sibling emitter).
import runpy
from pathlib import Path
runpy.run_path(str(Path(__file__).resolve().parent / "_emit_wave_62000_once.py"), run_name="__main__")
