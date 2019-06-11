#!/bin/bash

echo "AppImage creator"
usage() { echo "Usage: $0 [-b <build_dir_path>] [-o <output_path>]" 1>&2; exit 1; }

while getopts ":b:o:" o; do
    case "${o}" in
        b)
            build_dir=${OPTARG}
            ;;
        o)
            output_dir=${OPTARG}
            ;;
        *)
            usage
            ;;
    esac
done
shift $((OPTIND-1))

if [ -z "${build_dir}" ] || [ -o "${output_dir}" ]; then
    usage
fi

echo "build_dir = ${build_dir}"
echo "output_dir = ${output_dir}"

DIR="${output_dir}/kafkacow.AppDir/usr"
if [ -d ${output_dir}/kafkacow.AppDir ];
then
    printf '%s\n' "Overwriting directory(${output_dir}/kafkacow.AppDir)"
    rm -rf ${output_dir}/kafkacow.AppDir
fi
mkdir -p "$DIR";

cp -r "${build_dir}/." "$DIR"
cd ${output_dir}/kafkacow.AppDir
curl -L https://github.com/AppImage/AppImageKit/releases/download/12/AppRun-x86_64 --output AppRun
chmod a+x AppRun
cd ..
curl -L https://github.com/AppImage/AppImageKit/releases/download/12/appimagetool-x86_64.AppImage --output appimagetool.AppImage
chmod a+x appimagetool.AppImage
./appimagetool.AppImage --appimage-extract

cp ${build_dir}/kafkacow.desktop ${output_dir}/kafkacow.AppDir
cp ${build_dir}/icon.png ${output_dir}/kafkacow.AppDir

if [ -f "${output_dir}/kafkacow.AppImage" ];
then
    printf '%s\n' "Overwriting image..."
    rm -rf "${output_dir}/kafkacow.AppImage"
fi

./squashfs-root/AppRun ${output_dir}/kafkacow.AppDir ${output_dir}/kafkacow.AppImage
