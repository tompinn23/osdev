set(CMAKE_SYSTEM_NAME yonside)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(tools /opt/cross)
set(CMAKE_C_COMPILER ${tools}/bin/x86_64-elf-gcc)
set(CMAKE_LINKER ${tools}/bin/x86_64-elf-ld)

set(CMAKE_C_LINK_EXECUTABLE "<CMAKE_LINKER> <CMAKE_C_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")

set(CMAKE_C_FLAGS "-I. -std=gnu11 -ffreestanding -fno-stack-protector -fno-pic -fPIE -mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 -mno-red-zone")
set(CMAKE_EXE_LINKER_FLAGS "-nostdlib -zmax-page-size=0x1000 -static -pie --no-dynamic-linker -ztext")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
