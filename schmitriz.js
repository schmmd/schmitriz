/* ============================================================================
 * Schmitriz — faithful web port of the original Windows/DirectDraw game.
 *
 * Gameplay is ported directly from the C++ source (Game.cpp, Player.cpp,
 * Pieces.cpp, Settings.cpp, Highscore.cpp): the 7 piece/rotation tables, the
 * scoring formula, the per-level gravity speeds, spawn rules, line clearing,
 * and — the signature feature — the 2-player shared board where player 1
 * falls DOWN from the top and player 2 falls UP from the bottom, fighting over
 * a moving centre line.
 * ==========================================================================*/

'use strict';

const EMPTY = -1;
const UP = 0, DOWN = 1;

// --- Piece shape tables, transcribed verbatim from Pieces.h -----------------
// cShapes[type][rotation][row][col]
const SHAPES = [
  // 0 Straight
  [[[0,0,0,0],[1,1,1,1],[0,0,0,0],[0,0,0,0]],
   [[0,1,0,0],[0,1,0,0],[0,1,0,0],[0,1,0,0]],
   [[0,0,0,0],[1,1,1,1],[0,0,0,0],[0,0,0,0]],
   [[0,1,0,0],[0,1,0,0],[0,1,0,0],[0,1,0,0]]],
  // 1 Left L
  [[[0,0,1,0],[1,1,1,0],[0,0,0,0],[0,0,0,0]],
   [[1,1,0,0],[0,1,0,0],[0,1,0,0],[0,0,0,0]],
   [[0,0,0,0],[1,1,1,0],[1,0,0,0],[0,0,0,0]],
   [[0,1,0,0],[0,1,0,0],[0,1,1,0],[0,0,0,0]]],
  // 2 Right L
  [[[1,0,0,0],[1,1,1,0],[0,0,0,0],[0,0,0,0]],
   [[0,1,0,0],[0,1,0,0],[1,1,0,0],[0,0,0,0]],
   [[0,0,0,0],[1,1,1,0],[0,0,1,0],[0,0,0,0]],
   [[0,1,1,0],[0,1,0,0],[0,1,0,0],[0,0,0,0]]],
  // 3 T
  [[[0,1,0,0],[1,1,1,0],[0,0,0,0],[0,0,0,0]],
   [[0,1,0,0],[1,1,0,0],[0,1,0,0],[0,0,0,0]],
   [[0,0,0,0],[1,1,1,0],[0,1,0,0],[0,0,0,0]],
   [[0,1,0,0],[0,1,1,0],[0,1,0,0],[0,0,0,0]]],
  // 4 S
  [[[0,1,1,0],[1,1,0,0],[0,0,0,0],[0,0,0,0]],
   [[1,0,0,0],[1,1,0,0],[0,1,0,0],[0,0,0,0]],
   [[0,1,1,0],[1,1,0,0],[0,0,0,0],[0,0,0,0]],
   [[1,0,0,0],[1,1,0,0],[0,1,0,0],[0,0,0,0]]],
  // 5 Z
  [[[1,1,0,0],[0,1,1,0],[0,0,0,0],[0,0,0,0]],
   [[0,1,0,0],[1,1,0,0],[1,0,0,0],[0,0,0,0]],
   [[1,1,0,0],[0,1,1,0],[0,0,0,0],[0,0,0,0]],
   [[0,1,0,0],[1,1,0,0],[1,0,0,0],[0,0,0,0]]],
  // 6 Square
  [[[1,1,0,0],[1,1,0,0],[0,0,0,0],[0,0,0,0]],
   [[1,1,0,0],[1,1,0,0],[0,0,0,0],[0,0,0,0]],
   [[1,1,0,0],[1,1,0,0],[0,0,0,0],[0,0,0,0]],
   [[1,1,0,0],[1,1,0,0],[0,0,0,0],[0,0,0,0]]],
];

// Per (type,rotation) bounds, mirroring InitPieces():
//   left/top  = first filled col/row
//   right/bot = one past last filled col/row  (so width=right-left, height=bot-top)
const METRICS = SHAPES.map(rots => rots.map(m => {
  let top = -1, bottom = -1, left = -1, right = -1;
  for (let y = 0; y < 4; y++) for (let x = 0; x < 4; x++) if (m[y][x]) {
    if (top < 0 || y < top) top = y;
    if (y + 1 > bottom) bottom = y + 1;
    if (left < 0 || x < left) left = x;
    if (x + 1 > right) right = x + 1;
  }
  return { top, left, right, bottom, width: right - left, height: bottom - top };
}));

// --- Timer, ported from Timer.cpp -------------------------------------------
class Timer {
  constructor() { this.last = 0; this.duration = 0; this.enabled = true; this.disabledAt = 0; }
  get() { return this.duration; }
  set(d, now) { this.duration = d; this.reset(now); }
  reset(now) { this.last = now; }
  ring(now) { return this.enabled && (now - this.last > this.duration); }
  enable(now) { if (!this.enabled) { this.last += now - this.disabledAt; this.enabled = true; } }
}

// --- Key, ported from Keys.h (state machine + auto-repeat) -------------------
class Key {
  constructor(codes) {
    this.codes = codes;        // array of KeyboardEvent.code / synthetic ids
    this.state = 'up';
    this.repeat = false;
    this.hasBegan = false;
    this.begin = 0;
    this.initDelay = 0;
    this.delay = 0;
  }
  setRepeat(initDelay, delay) { this.repeat = true; this.hasBegan = false; this.initDelay = initDelay; this.delay = delay; }
  process(now, held) {
    const down = this.codes.some(c => held.has(c));
    if (down) this.state = (this.state === 'up' || this.state === 'released') ? 'pressed' : 'down';
    else { if (this.state === 'down' || this.state === 'pressed') { this.state = 'released'; this.hasBegan = false; } else this.state = 'up'; }

    if (this.repeat) {
      if (this.state === 'pressed') this.begin = now;
      else if (this.state === 'down') {
        if (!this.hasBegan) { if (now - this.begin > this.initDelay) { this.state = 'pressed'; this.hasBegan = true; this.begin = now; } }
        else { if (now - this.begin > this.delay) { this.state = 'pressed'; this.begin = now; } }
      }
    }
  }
  isPressed() { return this.state === 'pressed'; }
  isDown() { return this.state === 'down'; }
  isReleased() { return this.state === 'released'; }
  isUp() { return this.state === 'up'; }
}

// --- Board, ported from CBoard ----------------------------------------------
class Board {
  constructor(tileX, tileY, W, H) {
    this.nTileX = tileX; this.nTileY = tileY; this.nTileW = W; this.nTileH = H;
    this.nX = Math.floor((200 - tileX * W) / 2);
    this.nY = 0;
    this.cells = new Int8Array(tileX * tileY);
  }
  clear() { this.cells.fill(EMPTY); }
  get(x, y) { return this.cells[y * this.nTileX + x]; }
  set(v, x, y) { this.cells[y * this.nTileX + x] = v; }

  horCollide(p) { const m = METRICS[p.type][p.rot]; return p.x < -m.left || p.x > this.nTileX - m.right; }
  downCollide(p, boundY) { return p.y + METRICS[p.type][p.rot].bottom > boundY; }
  upCollide(p, boundY) { return p.y + METRICS[p.type][p.rot].top < boundY; }
  boardCollide(p) {
    const m = SHAPES[p.type][p.rot];
    for (let r = 0; r < 4; r++) for (let c = 0; c < 4; c++) {
      if (!m[r][c]) continue;
      const bx = p.x + c, by = p.y + r;
      if (by >= 0 && by < this.nTileY && bx >= 0 && bx < this.nTileX && this.get(bx, by) !== EMPTY) return true;
    }
    return false;
  }
  copyTo(p) {
    const m = SHAPES[p.type][p.rot];
    for (let c = 0; c < 4; c++) for (let r = 0; r < 4; r++)
      if (m[r][c]) this.set(p.type, p.x + c, p.y + r);
  }
  // rows above iLine fall down one; top row emptied
  shiftDown(iLine) {
    const W = this.nTileX;
    this.cells.copyWithin(W, 0, iLine * W);
    this.cells.fill(EMPTY, 0, W);
  }
  // rows below iLine rise one; bottom row emptied
  shiftUp(iLine) {
    const W = this.nTileX, H = this.nTileY;
    this.cells.copyWithin(iLine * W, (iLine + 1) * W, H * W);
    this.cells.fill(EMPTY, (H - 1) * W, H * W);
  }
}

// --- Controller, ported from CController (a piece + its keys + gravity) ------
class Controller {
  constructor(board, index, dir) {
    this.board = board;
    this.iIndex = index;
    this.Dir = dir;
    this.gravity = new Timer();
    this.next = { type: 0, rot: 0, x: 0, y: 0 };
    this.type = -1; this.rot = 0; this.x = 0; this.y = 0;
    this.lBoundryY = 0;
    this.lInitDrop = 0; this.lCurScore = 0; this.lCurLines = 0;
  }
  setBound(y) { this.lBoundryY = y; }

  newRandom(p) { p.type = (Math.random() * 7) | 0; p.rot = (this.Dir === UP) ? 2 : 0; }
  setPoint(p) {
    const m = METRICS[p.type][p.rot];
    p.x = Math.floor((this.board.nTileX - m.right) / 2);
    p.y = (this.Dir === DOWN) ? (-m.height - m.top + 1) : (this.board.nTileY - m.top - 1);
  }
  new_() {
    this.newRandom(this); this.newRandom(this.next);
    this.setPoint(this); this.setPoint(this.next);
    this.lCurScore = 0; this.lCurLines = 0; this.lInitDrop = 0;
  }
  nextPiece() {
    this.type = this.next.type; this.rot = this.next.rot; this.x = this.next.x; this.y = this.next.y;
    this.newRandom(this.next); this.setPoint(this.next);
  }
  moveBack() { if (this.Dir === UP) this.y++; else this.y--; }
  outOfBounds() {
    return this.Dir === UP ? this.board.downCollide(this, this.board.nTileY)
                           : this.board.upCollide(this, 0);
  }
  addGravity() {
    if (this.Dir === DOWN) { this.y++; return !(this.board.downCollide(this, this.lBoundryY) || this.board.boardCollide(this)); }
    else { this.y--; return !(this.board.upCollide(this, this.lBoundryY) || this.board.boardCollide(this)); }
  }
  processKeys(now, held) {
    for (const k of ['KeyRotate','KeyRevRotate','KeyLeft','KeyRight','KeyDrop','KeyFast']) this[k].process(now, held);
  }
  // Return: >=0 hard-drop distance ; -2 fast released ; -10 normal
  handleKeys(now, held, fastDelay) {
    const nw = { type: this.type, rot: this.rot, x: this.x, y: this.y };
    this.processKeys(now, held);

    if (this.KeyDrop.isPressed()) {
      this.lInitDrop = (this.Dir === DOWN)
        ? this.board.nTileY - this.y - METRICS[this.type][this.rot].bottom
        : this.y;
      while (this.addGravity()) { /* fall to rest */ }
      return this.lInitDrop;
    }

    if (this.KeyRotate.isPressed() && !this.KeyRevRotate.isDown()) nw.rot = (nw.rot + 1) & 3;
    if (this.KeyRevRotate.isPressed() && !this.KeyRotate.isDown()) nw.rot = (nw.rot + 3) & 3;
    if (this.KeyLeft.isPressed() && !this.KeyRight.isDown()) nw.x--;
    if (this.KeyRight.isPressed() && !this.KeyLeft.isDown()) nw.x++;

    if (this.KeyFast.isPressed()) { if (fastDelay < this.gravity.get()) this.gravity.set(fastDelay, now); }
    else if (this.KeyFast.isReleased()) return -2;

    if (!this.board.horCollide(nw) && !this.board.boardCollide(nw)) {
      this.type = nw.type; this.rot = nw.rot; this.x = nw.x; this.y = nw.y;
    }
    return -10;
  }
}

// --- Gravity speed per level, ported from SetGravity() ----------------------
function levelSpeed(level) {
  const table = [800, 650, 500, 350, 275, 200, 162, 125, 87];
  let t = level <= 8 ? table[level] : 90 - (level - 8) * 10;
  return t < 0 ? 0 : t;
}

// ============================================================================
// Game — orchestration ported from CGUIGame / CGUIGame1 / CGUIGame2
// ============================================================================
class Game {
  constructor(cfg) {
    this.numPlayers = cfg.numPlayers;
    this.linesPerLevel = cfg.linesPerLevel;
    this.board = new Board(cfg.tileX, cfg.tileY, cfg.W, cfg.H);
    this.controllers = [];
    for (let i = 0; i < this.numPlayers; i++) {
      const dir = (i === 1) ? UP : DOWN;
      const c = new Controller(this.board, i, dir);
      c.setBound(this.numPlayers === 1 ? this.board.nTileY : this.board.nTileY / 2);
      this.controllers.push(c);
    }
    this.paused = false; this.gameover = false; this.loser = -1;
    this.level = 0; this.initHeight = 0;
    this.fastDelay = 25;
    this.onScore = () => {};
    this.onEnd = () => {};
  }

  applyKeys(keymaps) {
    for (let i = 0; i < this.controllers.length; i++) {
      const c = this.controllers[i], km = keymaps[i];
      c.KeyRotate = new Key(km.rotate); c.KeyRevRotate = new Key(km.revrotate);
      c.KeyLeft = new Key(km.left); c.KeyRight = new Key(km.right);
      c.KeyDrop = new Key(km.drop); c.KeyFast = new Key(km.fast);
      // Horizontal auto-repeat (DAS): 500ms then 100ms, as in Settings.cpp defaults.
      c.KeyLeft.setRepeat(500, 100); c.KeyRight.setRepeat(500, 100);
      // Rotation is tap-only (the shipped default was bRepeatRotKeys = 0).
    }
  }

  new_(level, height, now) {
    this.paused = false; this.gameover = false; this.loser = -1;
    this.board.clear();
    this.level = level;
    this.initHeight = height;
    // reset boundaries for a fresh 2p match
    for (const c of this.controllers) c.setBound(this.numPlayers === 1 ? this.board.nTileY : this.board.nTileY / 2);
    this.doInitHeight(height);
    for (const c of this.controllers) { c.new_(); c.gravity.enabled = true; this.setGravity(c, now); }
  }

  setGravity(c, now) { c.gravity.set(levelSpeed(this.level), now); }

  levelUp(now) {
    this.level++;
    for (const c of this.controllers) if (c.KeyFast.isUp()) this.setGravity(c, now);
    this.onScore();
  }

  end(loserIndex) {
    if (!this.gameover) { this.gameover = true; this.loser = loserIndex; this.onEnd(); }
  }

  run(now, held) {
    if (this.paused || this.gameover) return;
    for (const c of this.controllers) {
      c.gravity.enable(now);
      if (c.gravity.ring(now)) { c.gravity.reset(now); if (!c.addGravity()) this.handleCollision(c, now); }
      const ret = c.handleKeys(now, held, this.fastDelay);
      if (ret === -2) this.setGravity(c, now);
      else if (ret !== -10) this.handleCollision(c, now);
    }
  }

  handleCollision(c, now) {
    c.moveBack();
    if (c.outOfBounds()) { this.end(c.iIndex); return; }
    this.board.copyTo(c);
    c.lCurLines += this.clearLines(c);

    let plus = 5 + 2 * this.level + c.lInitDrop;
    if (this.showNext) plus -= (Math.floor(this.level / 2) + 2);

    if (c.lCurLines > (this.level + 1) * this.linesPerLevel) this.levelUp(now);

    if (this.board.boardCollide(c.next)) { this.end(c.iIndex); return; }

    c.nextPiece();
    c.lInitDrop = 0;
    c.lCurScore += plus;
    this.onScore();
    c.gravity.reset(now);
  }

  clearLines(c) {
    const b = this.board, W = b.nTileX, H = b.nTileY;
    let cleared = 0;
    const rowFull = (y) => { for (let x = 0; x < W; x++) if (b.get(x, y) === EMPTY) return false; return true; };
    if (c.Dir === DOWN) {
      for (let y = 0; y < H; y++) if (rowFull(y)) { cleared++; b.shiftDown(y); this.extraClear(c, y); }
    } else {
      for (let y = H - 1; y >= 0; y--) if (rowFull(y)) { cleared++; b.shiftUp(y); this.extraClear(c, y); }
    }
    return cleared;
  }

  // 2-player only: clearing a line at the frontier pushes the centre line into
  // the opponent's territory (ported from CGUIGame2::ExtraClear).
  extraClear(c, y) {
    if (this.numPlayers !== 2) return;
    if (c.Dir === DOWN) { if (y === c.lBoundryY - 1) for (const k of this.controllers) k.lBoundryY++; }
    else { if (y === c.lBoundryY) for (const k of this.controllers) k.lBoundryY--; }
  }

  doInitHeight(height) {
    if (!height) return;
    const b = this.board, W = b.nTileX;
    const numSquares = Math.floor(W / 2) - 1;
    for (const c of this.controllers) {
      let row = (c.Dir === DOWN) ? c.lBoundryY - 1 : c.lBoundryY;
      for (let line = 0; line < height; line++) {
        let filled = 0;
        while (filled < numSquares) {
          const x = (Math.random() * W) | 0;
          if (b.get(x, row) === EMPTY) { b.set((Math.random() * 7) | 0, x, row); filled++; }
        }
        row += (c.Dir === DOWN) ? -1 : 1;
      }
    }
  }
}
