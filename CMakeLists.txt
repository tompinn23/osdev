cmake_minimum_required(VERSION 3.10)

project(yonside C)

enable_language(ASM_NASM)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf64)

set(KERNEL_SRC
    kernel/kmain.c
    kernel/terminal.c
    kernel/serial.c
    kernel/common.c
    kernel/isr.c
    kernel/util.c

    kernel/mm/bootmem.c
    kernel/mm/phys_mem.c
    
    kernel/cpu/cpu.c
    kernel/cpu/idt.c
    kernel/cpu/idt_asm.asm
    kernel/cpu/interrupt.asm

    kernel/console/console.c
    kernel/console/dina_fnt.c
    )

set(LIBK_SRC
    libk/string.c
    libk/vsprintf.c
    )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T${CMAKE_SOURCE_DIR}/linker.ld")

add_library(libk STATIC ${LIBK_SRC})
target_include_directories(libk PUBLIC include/libk include/kernel)

add_executable(kernel.elf ${KERNEL_SRC})
target_link_libraries(kernel.elf libk)
target_include_directories(kernel.elf PUBLIC include/kernel)

target_compile_definitions(kernel.elf PUBLIC DEBUG_early_pmm)

add_custom_target(iso ${CMAKE_SOURCE_DIR}/make_iso.sh ${CMAKE_BINARY_DIR}
                  DEPENDS kernel.elf
                  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                  )

                