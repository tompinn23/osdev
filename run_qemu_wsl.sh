#!/bin/bash

cp -v build/image.iso /mnt/c/Windows/Temp/yonside_os.iso
cp -v run_qemu.cmd /mnt/c/Windows/Temp/run_qemu.cmd

/mnt/c/Windows/System32/cmd.exe /mnt/c/Windows/Temp/run_qemu.cmd
