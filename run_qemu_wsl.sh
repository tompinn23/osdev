#!/bin/bash

cp -v leek.iso /mnt/c/Windows/Temp/leek.iso
cp -v run_qemu.cmd /mnt/c/Windows/Temp/run_qemu.cmd

/mnt/c/Windows/System32/cmd.exe /mnt/c/Windows/Temp/run_qemu.cmd
