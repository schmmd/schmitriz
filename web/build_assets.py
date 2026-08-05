import struct, zlib, base64, os, json

ROOT = "/Users/michael/hack/schmmd/schmitriz"

def read_bmp_rgb(path):
    d = open(path, "rb").read()
    off = struct.unpack("<I", d[10:14])[0]
    w = struct.unpack("<i", d[18:22])[0]
    h = struct.unpack("<i", d[22:26])[0]
    bpp = struct.unpack("<H", d[28:30])[0]
    assert bpp == 24, bpp
    row = ((w * 3) + 3) // 4 * 4
    px = []  # top-down rows of (r,g,b)
    for y in range(h):
        yy = h - 1 - y
        line = []
        base = off + yy * row
        for x in range(w):
            i = base + x * 3
            b, g, r = d[i], d[i + 1], d[i + 2]
            line.append((r, g, b))
        px.append(line)
    return w, h, px

def png_rgba(w, h, rows_rgba):
    # rows_rgba: list of bytes rows, each w*4 bytes, prefixed later
    def chunk(typ, data):
        c = struct.pack(">I", len(data)) + typ + data
        c += struct.pack(">I", zlib.crc32(typ + data) & 0xffffffff)
        return c
    raw = bytearray()
    for r in rows_rgba:
        raw.append(0)  # filter none
        raw += r
    comp = zlib.compress(bytes(raw), 9)
    sig = b"\x89PNG\r\n\x1a\n"
    ihdr = struct.pack(">IIBBBBB", w, h, 8, 6, 0, 0, 0)  # 8-bit RGBA
    return sig + chunk(b"IHDR", ihdr) + chunk(b"IDAT", comp) + chunk(b"IEND", b"")

def tileset_datauri(path):
    w, h, px = read_bmp_rgb(path)
    rows = []
    for y in range(h):
        row = bytearray()
        for x in range(w):
            r, g, b = px[y][x]
            a = 0 if (r == 255 and g == 0 and b == 255) else 255
            row += bytes((r, g, b, a))
        rows.append(bytes(row))
    png = png_rgba(w, h, rows)
    return "data:image/png;base64," + base64.b64encode(png).decode()

def jpg_datauri(path):
    return "data:image/jpeg;base64," + base64.b64encode(open(path, "rb").read()).decode()

# Tilesets (order/name from tilesets.idx)
tileset_list = [
    ("Wacky", "wacky.bmp"),
    ("Glass", "glass.bmp"),
    ("Standard", "standard.bmp"),
    ("Standard Outlined", "face.bmp"),
    ("Hollow", "hollow.bmp"),
    ("Hollow Bold", "hollowbold.bmp"),
    ("Hollow Circle", "hollowcirc.bmp"),
    ("Blurred", "blurred.bmp"),
    ("Blurred Circle", "blurredcirc.bmp"),
    ("Standard Circle", "circle.bmp"),
]
tilesets = []
for name, fn in tileset_list:
    tilesets.append({"name": name, "src": tileset_datauri(os.path.join(ROOT, "tilesets", fn))})

# Themes (game background). Default rusbw.
theme_list = [
    ("Russian B&W", "rusbw_game.jpg", (75, 75, 75)),
    ("Russian", "rus_game.jpg", (75, 75, 75)),
    ("Digital Works", "digital_game.jpg", (50, 50, 50)),
    ("Clockwork", "clockwork_game.jpg", (50, 50, 50)),
    ("Scene", "scene_game.jpg", (50, 50, 50)),
    ("Kitten", "kitten_game.jpg", (50, 50, 50)),
    ("Wild", "wild_game.jpg", (50, 50, 50)),
    ("Crew", "crew_game.jpg", (50, 50, 50)),
    ("Black", "black_game.jpg", (200, 200, 200)),
    ("White", "white_game.jpg", (75, 75, 75)),
]
themes = []
for name, fn, txt in theme_list:
    p = os.path.join(ROOT, "themes", fn)
    if os.path.exists(p):
        entry = {"name": name, "src": jpg_datauri(p), "text": "#%02x%02x%02x" % txt}
        ph = os.path.join(ROOT, "themes", fn.replace("_game.jpg", "_high.jpg"))
        if os.path.exists(ph):
            entry["high"] = jpg_datauri(ph)
        themes.append(entry)

title = jpg_datauri(os.path.join(ROOT, "Images", "title.jpg"))

out = "// Auto-generated from original Schmitriz assets. Do not edit by hand.\n"
out += "const ASSETS = " + json.dumps({"tilesets": tilesets, "themes": themes, "title": title}) + ";\n"

path = os.path.join(ROOT, "web", "assets.js")
os.makedirs(os.path.dirname(path), exist_ok=True)
open(path, "w").write(out)
print("wrote", path, "bytes:", len(out))
print("tilesets:", len(tilesets), "themes:", len(themes))
