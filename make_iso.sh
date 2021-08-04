#!/bin/bash

build_dir=$1

rm -rf "$build_dir/iso_root"
mkdir "$build_dir/iso_root"

cp -v "$build_dir/kernel.elf" limine.cfg limine/limine.sys \
    limine/limine-cd.bin limine/limine-eltorito-efi.bin "$build_dir/iso_root/"

xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-eltorito-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        "$build_dir/iso_root" -o "$build_dir/image.iso"

./limine/limine-install "$build_dir/image.iso"

