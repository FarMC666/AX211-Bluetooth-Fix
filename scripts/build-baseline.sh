#!/bin/bash
# Usage: bash scripts/build-baseline.sh EXISTING_BUILD_DIRECTORY
set -euo pipefail
root=$(cd "$(dirname "$0")" && pwd)
cd "${1:?provide a build directory}"
fetch() {
    local repo=$1 revision=$2
    if [[ ! -d "$repo-$revision" ]]; then
        curl -fsSL "https://codeload.github.com/acidanthera/$repo/tar.gz/$revision" -o "$repo.tar.gz"
        tar -xzf "$repo.tar.gz"
    fi
}
brcm=65389020769837e92e9fb939bac34bd9a3e25bb2
lilu=0249c3a918c68a43891350f35fded615b1575776
sdk=3f750085caa17ec3a7880f11c11bf4f48cd6a164
fetch BrcmPatchRAM "$brcm"
fetch Lilu "$lilu"
fetch MacKernelSDK "$sdk"
k="MacKernelSDK-$sdk"
l="Lilu-$lilu/Lilu"
sources="BrcmPatchRAM-$brcm/BrcmPatchRAM"
dest=baseline/BlueToolFixup.kext/Contents
mkdir -p "$dest/MacOS"
flags=(-arch x86_64 -mmacosx-version-min=10.8 -mkernel -nostdinc "-I$k/Headers" -DKERNEL -DKERNEL_EXTENSION)
for src in "$sources/BlueToolFixup.cpp" "$l/Library/plugin_start.cpp"; do
    obj=$(basename "$src" .cpp)
    xcrun clang++ "${flags[@]}" -fapple-kext -fno-exceptions -fno-rtti -mno-red-zone -fno-builtin "-I$l" -DPRODUCT_NAME=BlueToolFixup -DMODULE_VERSION=2.7.3 -DLILU_CUSTOM_IOKIT_INIT=1 -std=gnu++17 -Os -c "$src" -o "$obj.o"
done
xcrun clang "${flags[@]}" -c "$root/baseline-module.c" -o baseline-module.o
xcrun ld -kext -arch x86_64 -platform_version macos 10.8 13.1 -undefined dynamic_lookup -o "$dest/MacOS/BlueToolFixup" BlueToolFixup.o plugin_start.o baseline-module.o "$k/Library/x86_64/libkmod.a"
sed -e 's/$(DEVELOPMENT_LANGUAGE)/en/g' -e 's/$(EXECUTABLE_NAME)/BlueToolFixup/g' -e 's/$(PRODUCT_BUNDLE_IDENTIFIER)/as.acidanthera.BlueToolFixup/g' -e 's/$(PRODUCT_NAME:rfc1034identifier)/BlueToolFixup/g' -e 's/$(PRODUCT_NAME)/BlueToolFixup/g' -e 's/$(MODULE_VERSION)/2.7.3/g' "$sources/BlueToolFixup-Info.plist" > "$dest/Info.plist"
plutil -lint "$dest/Info.plist"
otool -hv "$dest/MacOS/BlueToolFixup"
