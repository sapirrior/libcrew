#!/usr/bin/env bash
set -euo pipefail

REPO="sapirrior/icrawl"
BIN_NAME="icrawl"

die() {
	echo "error: $1" >&2
	exit 1
}

detect_target() {
	local os arch

	os="$(uname -s)"
	arch="$(uname -m)"

	if [ -n "${PREFIX:-}" ] && echo "$PREFIX" | grep -q "com.termux"; then
		case "$arch" in
			aarch64) echo "termux-aarch64"; return ;;
			*) die "unsupported Termux architecture: $arch" ;;
		esac
	fi

	case "$os" in
		Linux)
			case "$arch" in
				x86_64)  echo "linux-x86_64" ;;
				aarch64) echo "linux-aarch64" ;;
				armv7l)  echo "linux-armv7" ;;
				*) die "unsupported Linux architecture: $arch" ;;
			esac
			;;
		Darwin)
			case "$arch" in
				x86_64)  echo "macos-x86_64" ;;
				arm64)   echo "macos-arm64" ;;
				*) die "unsupported macOS architecture: $arch" ;;
			esac
			;;
		MINGW*|CYGWIN*|MSYS*)
			echo "windows-x86_64"
			;;
		*)
			die "unsupported OS: $os"
			;;
	esac
}

latest_tag() {
	curl -fsSL "https://api.github.com/repos/${REPO}/releases/latest" \
		| grep '"tag_name"' \
		| sed 's/.*"tag_name": *"\([^"]*\)".*/\1/'
}

install_dir() {
	if [ -n "${PREFIX:-}" ] && echo "$PREFIX" | grep -q "com.termux"; then
		echo "$PREFIX/bin"
	elif [ -w /usr/local/bin ]; then
		echo "/usr/local/bin"
	else
		echo "$HOME/.local/bin"
	fi
}

main() {
	command -v curl >/dev/null 2>&1 || die "curl is required but not installed"

	local target tag asset_name download_url dest_dir dest

	target="$(detect_target)"
	tag="$(latest_tag)"

	[ -z "$tag" ] && die "could not fetch latest release tag"

	if [ "$target" = "windows-x86_64" ]; then
		asset_name="${BIN_NAME}-${target}.exe"
	else
		asset_name="${BIN_NAME}-${target}"
	fi

	download_url="https://github.com/${REPO}/releases/download/${tag}/${asset_name}"
	dest_dir="$(install_dir)"
	dest="${dest_dir}/${BIN_NAME}"

	echo "detected: $target"
	echo "release:  $tag"
	echo "asset:    $asset_name"
	echo "dest:     $dest"
	echo ""

	mkdir -p "$dest_dir"
	curl -fsSL --progress-bar "$download_url" -o "$dest"
	chmod +x "$dest"

	echo ""
	echo "installed: $dest"
}

main "$@"
