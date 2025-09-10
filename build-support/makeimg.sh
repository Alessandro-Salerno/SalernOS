#!/bin/sh

set -e

script_dir="$(dirname "$0")"
test -z "${script_dir}" && script_dir=.

source_dir="$(cd "${script_dir}"/.. && pwd -P)"
build_dir="$(pwd -P)"

# Let the user pass their own $SUDO (or doas).
: "${SUDO:=sudo}"

# Set ARCH based on the build directory.
case "$(basename "${build_dir}")" in
    build-x86_64) ARCH=x86_64 ;;
    build-riscv64) ARCH=riscv64 ;;
    *)
        echo "error: The build directory must be called 'build-<architecture>'." 1>&2
        exit 1
        ;;
esac

# Create build directory if needed.
mkdir -p "${build_dir}"

# Enter build directory.
cd "${build_dir}"

# If already initialized, get ARCH from .jinx-parameters file.
if [ -f .jinx-parameters ]; then
    if ! [ "${ARCH}" = "$(. ./.jinx-parameters && echo "${JINX_ARCH}")" ]; then
        echo "error: Jinx architecture and build dir derived architecture mismatch. Delete build dir." 1>&2
        exit 1
    fi
fi

if ! [ -f .jinx-parameters ]; then
    "${source_dir}"/jinx init "${source_dir}" ARCH="${ARCH}"
fi

"${source_dir}"/jinx build-if-needed base '*'
