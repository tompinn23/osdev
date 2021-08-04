#include "console/console.h"

#include "stddef.h"

#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include "console/ssfn.h"


extern void dina_sfn;

void init_console(struct stivale2_struct_tag_framebuffer* fb_tag) {
    ssfn_src = &dina_sfn;
    ssfn_dst.ptr = fb_tag->framebuffer_addr;
    ssfn_dst.p = fb_tag->framebuffer_pitch;
    ssfn_dst.fg = 0xFFFFFFFF;
    ssfn_dst.bg = 0x00000000;
    ssfn_dst.x = 0;
    ssfn_dst.y = 0;

    ssfn_putc('C');
}

void putchar(char c) {
    ssfn_putc(c);
}
