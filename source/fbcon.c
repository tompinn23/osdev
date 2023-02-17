#include "fbcon.h"

#include <stddef.h>

#include "libk/fmt.h"
#include "libk/kutil.h"
#define SSFN_CONSOLEBITMAP_TRUECOLOR
#include "libk/ssfn.h"

#include "libk/kfont.h"

static struct limine_framebuffer fb1;

static uint32_t colors8[] = {0x000000, 0xcc0000, 0x4e9a06, 0xc4a000,
                             0x729fcf, 0x75507b, 0x06989a, 0xd3d7cf};

static ssfn_font_t *bold;
static ssfn_font_t *normal;

enum style_flag {
  STYLE_BOLD = (1 << 1),
  STYLE_DIM = (1 << 2),
  STYLE_ITALIC = (1 << 3),
  STYLE_UNDERLINE = (1 << 4),
  STYLE_BLINKING = (1 << 5),
  STYLE_INVERSE = (1 << 6),
  STYLE_HIDDEN = (1 << 7),
  STYLE_STRIKETHROUGH = (1 << 8)
};

enum {
  COLOR_NO,
  COLOR_EXT,
  COLOR_256,
  COLOR_TRUE, // Completed truecolor
  COLOR_TRUE_R,
  COLOR_TRUE_G,
  COLOR_TRUE_B
};

static struct con_state {
  enum { ESC, BRACKET, PARSE, FG, BG, EQ, END } state;
  uint32_t style_flags;
  uint32_t reset_flags;
  int parsing;
  int color_ext;
  int bg;
  int colparse;
  uint8_t r, g, b;
} console;

void initialize_fbcon(struct limine_framebuffer_response *rsp) {
  fb1 = *rsp->framebuffers[0];
  if (fb1.width > 1920) {
    bold = &ter_i32b_sfn;
    normal = &ter_i32n_sfn;
  } else {
    bold = &ter_i16b_sfn;
    normal = &ter_i16n_sfn;
  }
  ssfn_src = normal;
  ssfn_dst.ptr = fb1.address;
  ssfn_dst.p = fb1.pitch;
  ssfn_dst.fg = colors8[7];
  ssfn_dst.bg = 0;
  ssfn_dst.x = 0;
  ssfn_dst.y = 0;
  console.state = ESC;
  console.color_ext = 0;
  console.r = 0;
  console.g = 0;
  console.b = 0;
  console.bg = 0;
  console.colparse = 0;
}

static void end_parsing() {
  console.state = ESC;
  console.colparse = 0;
  console.color_ext = 0;
  console.parsing = 0;
  console.reset_flags = 0;
}

static void fbcon_putc(char c, void (*putc)(uint32_t)) {
  switch (console.state) {
  case ESC:
    if (c == '\n') {
      ssfn_dst.x = 0;
      ssfn_dst.y += 16;
    } else if (c == '\t') {
      ssfn_dst.x += 16 * 4;
    } else if (c == '\x1B') {
      console.parsing = 1;
      console.state = BRACKET;
    } else {
      putc(c);
    }
    break;
  case BRACKET:
    if (c == '[') {
      console.state = PARSE;
    } else {
      end_parsing();
      putc(c);
    }
    break;
  case PARSE:
    if (c == '2') {
      if (console.reset_flags & STYLE_BOLD) {
        console.style_flags &= ~STYLE_BOLD;
        ssfn_src = normal;
      } else if (console.style_flags & STYLE_BOLD) {
        console.reset_flags |= STYLE_BOLD;
      } else {
        console.style_flags |= STYLE_BOLD;
        ssfn_src = bold;
      }
      console.state = END;
    }
    if (c == '3') {
      console.state = FG;
    }
    if (c == '4') {
      console.state = BG;
    }
    if (c == '0') {
      console.state = END;
      console.style_flags = 0;
      console.reset_flags = 0;
      ssfn_src = normal;
      ssfn_dst.fg = colors8[7];
      ssfn_dst.bg = colors8[0];
    }
    if (console.color_ext == COLOR_EXT) {
      if (c == '5') {
        console.color_ext = COLOR_256;
      }
      if (c == '2') {
        console.color_ext = COLOR_TRUE_R;
      }
    } else if (console.color_ext == COLOR_TRUE_R) {
      if (c >= '0' && c <= '9') {
        console.r *= 10;
        console.r += c - '0';
        console.colparse++;
        if (console.colparse == 3) {
          console.state = END;
          console.color_ext = COLOR_TRUE_G;
        }
      } else if (c == ';') {
        console.color_ext = COLOR_TRUE_G;
      } else {
        console.r = 0;
        end_parsing();
      }
    } else if (console.color_ext == COLOR_TRUE_G) {
      if (c >= '0' && c <= '9') {
        console.g *= 10;
        console.g += c - '0';
        console.colparse++;
        if (console.colparse == 3) {
          console.state = END;
          console.color_ext = COLOR_TRUE_B;
          console.colparse = 0;
        }
      } else if (c == ';') {
        console.color_ext = COLOR_TRUE_B;
        console.colparse = 0;
      } else {
        console.g = 0;
        end_parsing();
      }
    } else if (console.color_ext == COLOR_TRUE_B) {
      if (c >= '0' && c <= '9') {
        console.b *= 10;
        console.b += c - '0';
        console.colparse++;
        if (console.colparse == 3) {
          console.state = END;
          console.color_ext = COLOR_TRUE;
          console.colparse = 0;
        }
      } else if (c == ';') {
        console.color_ext = COLOR_TRUE;
        console.colparse = 0;
      } else {
        console.b = 0;
        end_parsing();
      }
    }
    break;
  case FG:
    if (c >= '0' && c <= '7') {
      console.state = END;
      ssfn_dst.fg = colors8[c - '0'];
    } else if (c == '8') {
      console.state = END;
      console.color_ext = COLOR_EXT;
    }
    break;
  case BG:
    if (c >= '0' && c <= '7') {
      console.state = END;
      ssfn_dst.bg = colors8[c - '0'];
    } else if (c == '8') {
      console.state = END;
      console.bg = 1;
      console.color_ext = COLOR_EXT;
    }
    break;
  case END:
    if (c == ';') {
      console.state = PARSE;
    } else if (c == 'm') {
      if (console.color_ext == COLOR_TRUE && console.bg == 0) {
        console.color_ext = 0;
        ssfn_dst.fg = (console.r << 16) | (console.g << 8) | console.b;
      } else if (console.color_ext == COLOR_TRUE && console.bg == 1) {
        console.color_ext = 0;
        ssfn_dst.bg = (console.r << 16) | (console.g << 8) | console.b;
      }
      end_parsing();
    }
    break;
  }
}

void kprintf(const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  uint32_t pos = 0;
  vsprintf_helper(NULL, fbcon_putc, fmt, &pos, ap);
  va_end(ap);
}

void vkprintf(const char *fmt, va_list ap) {
  uint32_t pos = 0;
  vsprintf_helper(NULL, fbcon_putc, fmt, &pos, ap);
}
