# Schmitriz — web edition

A faithful browser port of **Schmitriz**, the two-way Tetris originally written
in C++ / DirectDraw for Windows. The gameplay is ported directly from the
original source: the 7 piece/rotation tables, the scoring formula, the per-level
gravity speeds, spawn rules and line clearing — including the signature
**2-player shared board**, where player 1 falls *down* from the top and player 2
falls *up* from the bottom, fighting over a moving centre line.

## Play / host

Everything lives in [`web/`](web/) and is fully self-contained — the original
tilesets and background art are embedded in `web/assets.js` as data URIs, so
there are no external requests.

- **Locally:** open `web/index.html` in any modern browser (double-click works).
- **Hosted:** copy the `web/` folder to any static host (GitHub Pages, Netlify,
  S3, …). No build step, no server code.

## Controls

| Action | Player 1 | Player 2 |
| --- | --- | --- |
| Move | `4` / `6` or `←` / `→` | `J` / `L` |
| Rotate | `5` or `↑` | `K` |
| Rotate (reverse) | `8` | `I` |
| Soft drop | `+` or `↓` | `A` |
| Hard drop | `0` (or `Space` in 1P) | `Space` |

`P` or `Esc` pauses. In 2-player, clear lines at the centre line to push it into
your opponent's territory and crush them.

High scores are kept in the browser's `localStorage`.

## Files

- `web/index.html` — UI, canvas renderer, screens, input, high scores
- `web/schmitriz.js` — the ported game engine (board, pieces, controllers, timers)
- `web/assets.js` — embedded original tilesets / theme backgrounds / title art
- `web/build_assets.py` — regenerates `assets.js` from the original art

## Regenerating assets

`assets.js` was generated from the original game's art (the `tilesets/`,
`themes/` and `Images/` folders of the Windows project). Those source files, and
the complete original C++ source, remain in this repository's git history. To
rebuild the bundle:

```sh
git checkout 8ade326 -- tilesets themes Images   # restore the original art
python3 web/build_assets.py                       # rewrites web/assets.js
```

The tileset BMPs use magenta (`255, 0, 255`) as a colour key; the build script
converts that to transparency when baking the PNG sprite sheets.

## Credits

Original game and artwork from the Windows release — game backgrounds by Mikhail
Popenov of Fedoskino and from Sunbirds.com; high-score backdrop from NASA.
