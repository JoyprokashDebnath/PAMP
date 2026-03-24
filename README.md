# PAMP — PriX Advanced Music Player

> High-resolution audio. Pure terminal. Zero compromise.

![Preview](https://github.com/JoyprokashDebnath/PAMP/screenshots/475.png)

---

## What is PAMP?

**PAMP** (PriX Advanced Music Player) is a terminal-native music player written in C, built for audiophiles who live in the command line.

It plays high-resolution audio files directly from your terminal with a fully interactive TUI — no desktop environment, no GUI dependencies, no compromises on audio quality.

Browse your library, control playback, and manage your queue entirely through the keyboard or mouse, without ever leaving the terminal.

---

## Preview

```
┌─────────────────────────────────────────────────────────────────┐
│ PAMP  ~/Music  [shuffle: off]  [repeat: off]       v0.1.0       │
├──────────────────────┬──────────────────────────────────────────┤
│  > 2002              │  2002                                    │
│    Crave You         │  Anne-Marie  ·  Feelgood Mode  ·  2018   │
│    Friends           │                                          │
│    Alarm             │  ████████████░░░░░░░░░░  3:12 / 4:56     │
│    Rewrite The Stars │                                          │
│    Rockabye          │  ◀◀   ▐▐   ▶▶    ↺    ⇄  vol: ███░   │
│    Perfect           │                                          │
│    Shape of You      │  Up next:                                │
│    Blinding Lights   │  1. Crave You — Flight Facilities        │
│    Stay              │  2. Friends — Anne-Marie                 │
│                      │  3. Alarm — Anne-Marie                   │
├──────────────────────┴──────────────────────────────────────────┤
│ [space] play  [n/N] next/prev  [/] search  [?] help  [q] quit   │
└─────────────────────────────────────────────────────────────────┘
```

---

## Features

- **High-resolution audio playback** — supports up to 32-bit / 384 kHz via libmpv
- **Interactive TUI** — full ncurses interface with keyboard and mouse navigation
- **Wide format support** — MP3, FLAC, OGG, OPUS, AAC, M4A, WAV and anything libmpv can decode
- **Library browser** — recursive directory scanning with tag-based display
- **Live search** — filter your library instantly as you type
- **Queue management** — add, remove, and reorder tracks on the fly
- **Shuffle and repeat** — per-track, full playlist, or off
- **Mouse support** — click to select, scroll to navigate, double-click to play
- **Command-line control** — pass files, directories, or flags directly at launch
- **Low resource usage** — pure C, no runtime overhead, no Electron

---

## Supported Formats

| Format | Extension | Notes |
|---|---|---|
| MP3 | `.mp3` | Up to 320 kbps CBR / VBR |
| FLAC | `.flac` | Lossless, up to 32-bit / 384 kHz |
| OGG Vorbis | `.ogg` | Open lossless |
| Opus | `.opus` | High-efficiency codec |
| AAC / M4A | `.aac`, `.m4a` | Including Apple Lossless (ALAC) |
| WAV / AIFF | `.wav`, `.aiff` | Uncompressed PCM |
| Any libmpv codec | — | If mpv plays it, PAMP plays it |

---

## Requirements

### Runtime

| Dependency | Version | Purpose |
|---|---|---|
| `libmpv` | ≥ 0.35 | Audio decoding and output |
| `ncurses` | ≥ 6.0 | Terminal UI |
| `pipewire` or `pulseaudio` | any | Audio output backend |

### Build

| Tool | Purpose |
|---|---|
| `gcc` ≥ 10 or `clang` ≥ 12 | C compiler |
| `make` | Build system |

---

## Installation

### Arch Linux

```bash
sudo pacman -S mpv ncurses
git clone https://github.com/JoyprokashDebnath/PAMP.git
cd PAMP
make
sudo make install
```

### Debian / Ubuntu

```bash
sudo apt install libmpv-dev libncurses-dev build-essential
git clone https://github.com/JoyprokashDebnath/PAMP.git
cd PAMP
make
sudo make install
```

### Fedora

```bash
sudo dnf install mpv-libs-devel ncurses-devel gcc make
git clone https://github.com/JoyprokashDebnath/PAMP.git
cd PAMP
make
sudo make install
```

### Manual build (no install)

```bash
git clone https://github.com/JoyprokashDebnath/PAMP.git
cd PAMP
make clean && make all
./pamp ~/Music/song.flac
```

> Default install prefix is `/usr/local`. Override with `make install PREFIX=/usr`.

---

## Usage

### Basic

```bash
# Play a single file
pamp song.flac

# Play all music in a directory
pamp ~/Music/

# Play multiple files in order
pamp track1.flac track2.mp3 track3.ogg
```

### Command-line Options

```
Usage: pamp [OPTIONS] [FILE|DIR ...]

  -h, --help              Show this help message and exit
  -v, --version           Print version information and exit
  -s, --shuffle           Start with shuffle enabled
  -r, --repeat [one|all]  Start with repeat mode (default: all)
  -V, --volume <0-100>    Set initial volume (default: 100)
      --no-mouse          Disable mouse support

Examples:
  pamp ~/Music/                   Play entire music library
  pamp -s ~/Music/Jazz/           Shuffle a directory
  pamp -r one favourite.flac      Loop a single track
  pamp -V 80 ~/Music/             Start at 80% volume
```

---

## Keyboard Shortcuts

### Playback

| Key | Action |
|---|---|
| `Space` | Play / Pause |
| `n` | Next track |
| `N` | Previous track |
| `→` | Seek forward 5 seconds |
| `←` | Seek backward 5 seconds |
| `]` | Volume up |
| `[` | Volume down |
| `m` | Mute / Unmute |

### Library and Queue

| Key | Action |
|---|---|
| `↑` / `↓` | Navigate list |
| `Enter` | Play selected track |
| `Tab` | Switch pane (Library ↔ Queue) |
| `a` | Add selected track to queue |
| `d` | Remove selected track from queue |
| `c` | Clear queue |
| `g` / `G` | Jump to top / bottom of list |

### Modes and Navigation

| Key | Action |
|---|---|
| `s` | Toggle shuffle |
| `r` | Cycle repeat mode (off → all → one) |
| `/` | Open search |
| `Esc` | Close search / cancel |
| `?` | Show help overlay |
| `q` | Quit |

---

## Mouse Controls

| Action | Effect |
|---|---|
| Left click on track | Select track |
| Double left click on track | Play immediately |
| Scroll up / down | Scroll the active list |
| Left click on progress bar | Seek to position |
| Left click on volume bar | Set volume |

---

## Project Structure

```
PAMP/
├── src/
│   ├── main.c              ← entry point, event loop, cleanup
│   ├── audio/
│   │   ├── player.c        ← libmpv wrapper
│   │   ├── player.h
│   │   ├── playlist.c      ← queue, shuffle, repeat logic
│   │   └── playlist.h
│   ├── ui/
│   │   ├── layout.c        ← window creation and resize handling
│   │   ├── layout.h
│   │   ├── draw.c          ← all ncurses rendering functions
│   │   ├── draw.h
│   │   ├── colors.c        ← color pair definitions
│   │   └── colors.h
│   ├── input/
│   │   ├── keybinds.c      ← key dispatch table
│   │   └── keybinds.h
│   ├── library/
│   │   ├── scanner.c       ← recursive directory scanner
│   │   ├── scanner.h
│   │   ├── metadata.c      ← tag reading
│   │   └── metadata.h
│   └── state/
│       ├── state.c
│       └── state.h
├── Makefile
├── config.ini
└── README.md
```

---

## Configuration

PAMP reads `~/.config/pamp/config.ini` on startup.

```bash
mkdir -p ~/.config/pamp
cp config.ini ~/.config/pamp/config.ini
```

```ini
[library]
music_dir      = ~/Music
scan_recursive = yes

[playback]
default_volume = 100
audio_output   = pipewire    ; pipewire | pulse | alsa
resume_last    = yes

[ui]
show_album     = yes
show_duration  = yes
sidebar_width  = 36
```

---

## Audio Quality

PAMP passes audio data through libmpv directly to your audio output with no resampling or processing by default.

With PipeWire, PAMP negotiates the native sample rate and bit depth of your file directly with your DAC — bit-perfect output out of the box.

> Tested with 24-bit / 96 kHz and 32-bit / 384 kHz FLAC files on PipeWire with zero degradation.

---

## Roadmap

- [x] Phase 1 — libmpv audio backend (play / pause / seek / volume)
- [x] Phase 2 — ncurses TUI shell with full layout
- [ ] Phase 3 — UI and audio connected, live progress bar
- [ ] Phase 4 — Library scanner + TagLib metadata
- [ ] Phase 5 — Shuffle, repeat, search, mouse support
- [ ] Phase 6 — Config file, persistent queue, themes
- [ ] Phase 7 — Equalizer, ReplayGain, gapless playback
- [ ] Phase 8 — Lyrics display

---

## Contributing

Open an issue before submitting a pull request for significant changes.

Code style: K&R braces, 4-space indentation, `snake_case`. No dependencies beyond those listed in Requirements.

---

## License

MIT License — see [LICENSE](LICENSE) for full text.

---

## Acknowledgements

- [mpv](https://mpv.io/) — audio engine
- [ncurses](https://invisible-island.net/ncurses/) — terminal UI
- [TagLib](https://taglib.org/) — metadata reading
- [PipeWire](https://pipewire.org/) — low-latency audio server
