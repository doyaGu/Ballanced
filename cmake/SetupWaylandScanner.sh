#!/usr/bin/env bash

set -euo pipefail

readonly WAYLAND_VERSION="1.26.0"
readonly WAYLAND_SHA256="64176eaa46e4969903e286f8e5ef8331affc17fdf03ac9b58381d2b23162b7a3"
readonly WAYLAND_ARCHIVE="wayland-${WAYLAND_VERSION}.tar.xz"
readonly WAYLAND_URL="https://gitlab.freedesktop.org/wayland/wayland/-/releases/${WAYLAND_VERSION}/downloads/${WAYLAND_ARCHIVE}"

if [[ -z "${GITHUB_PATH:-}" ]]; then
    echo "GITHUB_PATH is required to expose the CI wayland-scanner" >&2
    exit 1
fi

work_root="$(mktemp -d "${RUNNER_TEMP:-/tmp}/ballanced-wayland-scanner.XXXXXX")"
archive_path="${work_root}/${WAYLAND_ARCHIVE}"
source_root="${work_root}/wayland-${WAYLAND_VERSION}"
build_root="${work_root}/build"
scanner_root="${build_root}/src"

sudo apt-get update -qq
sudo env DEBIAN_FRONTEND=noninteractive \
    apt-get install -y -qq meson ninja-build libexpat1-dev libxml2-dev

curl --fail --location --retry 3 --silent --show-error \
    --output "${archive_path}" "${WAYLAND_URL}"
echo "${WAYLAND_SHA256}  ${archive_path}" | sha256sum --check --status
tar -xf "${archive_path}" -C "${work_root}"

meson setup "${build_root}" "${source_root}" \
    --buildtype=release \
    -Dlibraries=false \
    -Dscanner=true \
    -Dtests=false \
    -Ddocumentation=false \
    -Ddtd_validation=true
meson compile -C "${build_root}"

scanner_version="$("${scanner_root}/wayland-scanner" --version 2>&1)"
if [[ "${scanner_version}" != "wayland-scanner ${WAYLAND_VERSION}" ]]; then
    echo "Unexpected wayland-scanner version: ${scanner_version}" >&2
    exit 1
fi

echo "${scanner_root}" >> "${GITHUB_PATH}"
echo "Using ${scanner_version} from ${scanner_root}"
