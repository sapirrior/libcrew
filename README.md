# icrawl

Fetch and render documentation from the [libcrew](https://github.com/sapirrior/icrawl) registry directly in your terminal.

```
icrawl pkg:python-torch
icrawl lang:c
icrawl tool:cmake
```

---

## Install

```sh
curl -fsSL https://raw.githubusercontent.com/sapirrior/icrawl/main/install.sh | bash
```

Supports Linux (x86\_64, aarch64, armv7), macOS (x86\_64, arm64), Windows (x86\_64), and Termux (aarch64).

The binary is placed in `/usr/local/bin`, `~/.local/bin`, or `$PREFIX/bin` on Termux.

---

## Usage

```
icrawl [category:]<item>
```

If no category is given, `pkg` is assumed.

```
icrawl torch              # same as icrawl pkg:torch
icrawl lang:rust
icrawl tool:ninja
```

---

## Build from source

Requires `gcc` and `libcurl-dev`.

```sh
git clone https://github.com/sapirrior/icrawl
cd icrawl
make
sudo make install
```

To cross-compile for a specific target, override `CC` and `CFLAGS`:

```sh
make CC=aarch64-linux-gnu-gcc
```

---

## Registry layout

Documentation is fetched from:

```
https://raw.githubusercontent.com/sapirrior/icrawl/main/registry/<category>/<item>.md
```

To contribute a package entry, add a Markdown file under `registry/`.
