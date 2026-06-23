#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/../.." && pwd)"
DEB_META_DIR="${SCRIPT_DIR}/DEBIAN"

# VERSION 可通过环境变量覆盖；否则从 DEBIAN/control 解析
VERSION="${VERSION:-}"
if [ -z "${VERSION}" ]; then
    VERSION=$(grep -oP '(?<=Version: ).*' "${DEB_META_DIR}/control" | tr -d '[:space:]')
fi
if [ -z "${VERSION}" ]; then
    echo "ERROR: cannot find Version in ${DEB_META_DIR}/control"
    exit 1
fi

PACKAGE_NAME=$(grep -oP '(?<=Package: ).*' "${DEB_META_DIR}/control" | tr -d '[:space:]')
ARCH=$(grep -oP '(?<=Architecture: ).*' "${DEB_META_DIR}/control" | tr -d '[:space:]')
DEB_FILENAME="${PACKAGE_NAME}_${VERSION}_${ARCH}.deb"

echo "=== Building ${DEB_FILENAME} ==="

# paths
BUILD_DIR="${PROJECT_DIR}/build-deb"
STAGE_DIR="/tmp/${PACKAGE_NAME}-deb-$$"
OUTPUT_DIR="${PROJECT_DIR}"

# step 1: build
echo "[1/4] Building release binary..."
mkdir -p "${BUILD_DIR}"
cmake -S "${PROJECT_DIR}" -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE=Release
cmake --build "${BUILD_DIR}" --parallel "$(nproc)"

BINARY="${BUILD_DIR}/SimpleFOCStudio"
if [ ! -f "${BINARY}" ]; then
    echo "ERROR: binary not found: ${BINARY}"
    exit 1
fi

# step 2: create staging directory
echo "[2/4] Creating staging directory..."
rm -rf "${STAGE_DIR}"
mkdir -p "${STAGE_DIR}/DEBIAN"
mkdir -p "${STAGE_DIR}/usr/bin"
mkdir -p "${STAGE_DIR}/usr/share/applications"
mkdir -p "${STAGE_DIR}/usr/share/icons/hicolor/256x256/apps"

# copy DEBIAN/control (rewrite version)
sed "s/^Version: .*/Version: ${VERSION}/" "${DEB_META_DIR}/control" > "${STAGE_DIR}/DEBIAN/control"

# copy binary
cp "${BINARY}" "${STAGE_DIR}/usr/bin/SimpleFOCStudio"
strip "${STAGE_DIR}/usr/bin/SimpleFOCStudio"

# auto-detect shared library dependencies from binary
DEPS_WORK_DIR=$(mktemp -d)
mkdir -p "${DEPS_WORK_DIR}/debian"
printf "Source: simplefocstudio\nPackage: simplefocstudio\nArchitecture: any\n" > "${DEPS_WORK_DIR}/debian/control"
DEPS=$(cd "${DEPS_WORK_DIR}" && dpkg-shlibdeps -O "${STAGE_DIR}/usr/bin/SimpleFOCStudio" 2>/dev/null \
       | grep '^shlibs:Depends=' | cut -d= -f2-)
rm -rf "${DEPS_WORK_DIR}"
if [ -z "${DEPS}" ]; then
    DEPS="libqt6core6, libqt6gui6, libqt6widgets6, libqt6serialport6, libqt6charts6, libc6, libgcc-s1, libstdc++6"
fi
echo "Detected dependencies: ${DEPS}"
sed -i "s/\${shlibs:Depends}/${DEPS}/" "${STAGE_DIR}/DEBIAN/control"

# copy desktop entry
if [ -f "${SCRIPT_DIR}/simplefocstudio.desktop" ]; then
    cp "${SCRIPT_DIR}/simplefocstudio.desktop" "${STAGE_DIR}/usr/share/applications/"
fi

# copy icon
ICON="${PROJECT_DIR}/resources/icons/motor.png"
if [ -f "${ICON}" ]; then
    cp "${ICON}" "${STAGE_DIR}/usr/share/icons/hicolor/256x256/apps/simplefocstudio.png"
fi

# step 3: build deb
echo "[3/4] Packaging..."
dpkg-deb --build "${STAGE_DIR}" "${OUTPUT_DIR}/${DEB_FILENAME}"

# step 4: cleanup
echo "[4/4] Cleaning up staging directory..."
rm -rf "${STAGE_DIR}"

DEB_PATH="${OUTPUT_DIR}/${DEB_FILENAME}"
DEB_SIZE=$(du -h "${DEB_PATH}" | cut -f1)

echo ""
echo "=== Done ==="
echo "  Package: ${DEB_FILENAME}"
echo "  Size:    ${DEB_SIZE}"
echo "  Path:    ${DEB_PATH}"
echo ""
echo "Install:"
echo "  sudo dpkg -i ${DEB_PATH}"
echo "  # or with auto-dependency:"
echo "  sudo apt install -y ./${DEB_FILENAME}"
