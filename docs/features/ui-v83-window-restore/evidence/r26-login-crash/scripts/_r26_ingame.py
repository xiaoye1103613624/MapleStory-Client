# -*- coding: utf-8 -*-
"""Classify what the client window is actually showing: splash / login / in-game.

Why: _boot.py's old probe only counted bright pixels in the bottom 60 rows, and the
NEXON splash is almost pure white -> it reported "in game" at t=11s for a build that
was about to crash. Discriminators here are frame-wide, not band-local.

Usage:
  python _r26_ingame.py            # one-shot report
  python _r26_ingame.py wait [N]   # poll until in-game or N seconds (default 300)
"""
import subprocess
import sys
import time

import numpy as np
from PIL import Image

DRV = ['python', 'E:/project/_drive.py']
SHOT = 'E:/project/_r26_state.png'


def window_up():
    out = subprocess.run(DRV + ['where'], capture_output=True, text=True).stdout.strip()
    return (not out.startswith('NO WINDOW')), out


def grab():
    subprocess.run(DRV + ['cap', 'r26_state'], capture_output=True)
    try:
        im = Image.open('E:/project/_v_r26_state.png').convert('RGB')
    except OSError:
        return None
    return np.asarray(im).astype(int)


def classify(a):
    h, w, _ = a.shape
    mx = a.max(axis=2)
    white = float((mx > 240).mean())
    dark = float((mx < 60).mean())
    # HUD chrome (status bar / chat bar / minimap band) lives in the bottom 22% of the client.
    band = mx[int(h * 0.78):, :]
    band_mid = float(((band > 70) & (band < 235)).mean())
    # colourful (HP/MP bars, map art) anywhere below the middle
    sub = a[int(h * 0.5):, :, :]
    sat = (sub.max(axis=2) - sub.min(axis=2))
    colourful = float((sat > 40).mean())
    if white > 0.90:
        state = 'SPLASH'          # near-pure white NEXON card
    elif colourful < 0.020 and band_mid < 0.05:
        state = 'LOGIN/CHARSEL'   # flat dark UI, no map art
    else:
        state = 'INGAME'
    return state, dict(white=round(white, 4), dark=round(dark, 4),
                       band_mid=round(band_mid, 4), colourful=round(colourful, 4),
                       size=(w, h))


def main():
    mode = sys.argv[1] if len(sys.argv) > 1 else 'once'
    budget = int(sys.argv[2]) if len(sys.argv) > 2 else 300
    up, info = window_up()
    if not up:
        print('NO WINDOW (process gone or crashed)')
        return 2
    print('window:', info)
    t0 = time.time()
    while True:
        a = grab()
        if a is None:
            print('capture failed')
        else:
            state, m = classify(a)
            print('t=%3ds %-14s %s' % (time.time() - t0, state, m))
            if state == 'INGAME':
                return 0
        if mode != 'wait':
            return 0
        if time.time() - t0 > budget:
            print('TIMEOUT not in-game')
            return 1
        up, _ = window_up()
        if not up:
            print('WINDOW DISAPPEARED -> crashed')
            return 2
        time.sleep(8)


if __name__ == '__main__':
    sys.exit(main())
