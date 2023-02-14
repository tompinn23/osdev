#!/bin/sh -e

KERNEL="${1:-leek.elf}"
IMAGE_FILE="${2:-leek.img}"

# Create an empty zeroed out 64MiB image file.
dd if=/dev/zero bs=1M count=0 seek=64 of="$IMAGE_FILE"
 
# Create a GPT partition table.
parted -s "$IMAGE_FILE" mklabel gpt
 
# Create an ESP partition that spans the whole disk.
parted -s "$IMAGE_FILE" mkpart ESP fat32 2048s 100%
parted -s "$IMAGE_FILE" set 1 esp on
 
# Install the Limine BIOS stages onto the image.
./limine/bin/limine-deploy "$IMAGE_FILE"
 
# Mount the loopback device.
USED_LOOPBACK=$(losetup -f)
doas losetup -Pf "$IMAGE_FILE"
 
# Format the ESP partition as FAT32.
doas mkfs.vfat -F 32 ${USED_LOOPBACK}p1
 
# Mount the partition itself.
mkdir -p img_mount
doas mount ${USED_LOOPBACK}p1 img_mount
 
# Copy the relevant files over.
doas mkdir -p img_mount/EFI/BOOT
doas cp -v "$KERNEL" limine.cfg limine/bin/limine.sys img_mount/
doas cp -v limine/bin/BOOTX64.EFI img_mount/EFI/BOOT/
 
# Sync system cache and unmount partition and loopback device.
sync
doas umount img_mount
doas losetup -d ${USED_LOOPBACK}