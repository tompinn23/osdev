#!/bin/sh -e

mkdir -p iso_root
cp -v "$1" limine.cfg limine/limine.sys \
  limine/limine-cd.bin limine/limine-cd-efi.bin iso_root/

xorriso -as mkisofs -b limine-cd.bin \
  -no-emul-boot -boot-load-size 4 -boot-info-table \
  --efi-boot limine-cd-efi.bin \
  -efi-boot-part --efi-boot-image --protective-msdos-label \
  iso_root -o "$2"

./limine/limine-deploy "$2"
