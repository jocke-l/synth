#include "ui.h"

#include <stdlib.h>
#include <SDL2/SDL.h>

#include <nuklear/nuklear.h>
#include <nuklear/nuklear_sdl_renderer.h>

static UIKey ui_key_from_sdl_key(SDL_KeyCode code);

struct UIContext {
    struct nk_sdl sdl;

    void (*on_quit_callback)(void*);
    void *on_quit_context;

    void (*on_key_down_callback)(void*, UIKey);
    void *on_key_down_context;

    void (*on_key_up_callback)(void*, UIKey);
    void *on_key_up_context;
} static ui_invalid = { 0 };

int ui_init(void)
{
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return -1;
    return 0;
}

void ui_deinit(void)
{
    SDL_Quit();
}

UI* ui_create(const char *window_name, int window_width, int window_height)
{
    UI* ui = malloc(sizeof(UI));
    if (!ui) {
        return NULL;
    }
    *ui = ui_invalid;

    ui->sdl.win = SDL_CreateWindow(window_name,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);

    if (ui->sdl.win == NULL) {
        SDL_Log("Error SDL_CreateWindow %s", SDL_GetError());
        free(ui);
        return NULL;
    }

    ui->sdl.renderer = SDL_CreateRenderer(ui->sdl.win, -1,
        SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (ui->sdl.renderer == NULL) {
        SDL_Log("Error SDL_CreateRenderer %s", SDL_GetError());
        SDL_DestroyWindow(ui->sdl.win);
        free(ui);
        return NULL;
    }

    float font_scale = 1;
    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(ui->sdl.renderer, &render_w, &render_h);
        SDL_GetWindowSize(ui->sdl.win, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_RenderSetScale(ui->sdl.renderer, scale_x, scale_y);
        font_scale = scale_y;
    }

    ui->sdl = nk_sdl_init(ui->sdl.win, ui->sdl.renderer);
    {
        struct nk_font_atlas *atlas;
        struct nk_font_config config = nk_font_config(0);
        nk_sdl_font_stash_begin(&ui->sdl, &atlas);
        struct nk_font *font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
        nk_sdl_font_stash_end(&ui->sdl);

        font->handle.height /= font_scale;
        nk_style_set_font(&ui->sdl.ctx, &font->handle);
    }

    return ui;
}

void ui_destroy(UI *ui)
{
    nk_sdl_deinit(&ui->sdl);
    SDL_DestroyRenderer(ui->sdl.renderer);
    SDL_DestroyWindow(ui->sdl.win);
    *ui = ui_invalid;
    free(ui);
}

struct nk_context *ui_get_nuklear_context(UI *ui)
{
    return &ui->sdl.ctx;
}

void ui_handle_events(UI *ui)
{
    SDL_Event evt;
    nk_input_begin(&ui->sdl.ctx);
    while (SDL_PollEvent(&evt)) {
        if (evt.type == SDL_QUIT && ui->on_quit_callback) {
            ui->on_quit_callback(ui->on_quit_context);
            continue;
        }
#ifndef NDEBUG
        if (evt.type == SDL_KEYDOWN && evt.key.keysym.sym == SDLK_ESCAPE) {
            if (ui->on_quit_callback) {
                ui->on_quit_callback(ui->on_quit_context);
                continue;
            }
        }
#endif

        switch (evt.type) {
        case SDL_KEYDOWN:
            if (ui->on_key_down_callback) {
                UIKey key = ui_key_from_sdl_key(evt.key.keysym.sym);
                ui->on_key_down_callback(ui->on_key_down_context, key);
                continue;
            }

        case SDL_KEYUP:
            if (ui->on_key_up_callback) {
                UIKey key = ui_key_from_sdl_key(evt.key.keysym.sym);
                ui->on_key_up_callback(ui->on_key_down_context, key);
                continue;
            }
        }

        nk_sdl_handle_event(&ui->sdl, &evt);
    }
    nk_input_end(&ui->sdl.ctx);
}

void ui_render_frame(UI *ui)
{
    SDL_RenderClear(ui->sdl.renderer);
    nk_sdl_render(&ui->sdl, NK_ANTI_ALIASING_ON);
    SDL_RenderPresent(ui->sdl.renderer);
}

void ui_set_on_quit_event(UI *ui, void(*callback)(void*), void *context)
{
    ui->on_quit_callback = callback;
    ui->on_quit_context = context;
}

void ui_set_on_key_down_event(UI *ui, void(*callback)(void*, UIKey), void *context)
{
    ui->on_key_down_callback = callback;
    ui->on_key_down_context = context;
}

void ui_set_on_key_up_event(UI *ui, void(*callback)(void*, UIKey), void *context)
{
    ui->on_key_up_callback = callback;
    ui->on_key_up_context = context;
}

static UIKey ui_key_from_sdl_key(SDL_KeyCode code)
{
    // clang-format off
    switch (code) {
    case SDLK_UNKNOWN:           return UIKey_UNKNOWN;

    case SDLK_RETURN:            return UIKey_RETURN;
    case SDLK_ESCAPE:            return UIKey_ESCAPE;
    case SDLK_BACKSPACE:         return UIKey_BACKSPACE;
    case SDLK_TAB:               return UIKey_TAB;
    case SDLK_SPACE:             return UIKey_SPACE;
    case SDLK_EXCLAIM:           return UIKey_EXCLAIM;
    case SDLK_QUOTEDBL:          return UIKey_QUOTEDBL;
    case SDLK_HASH:              return UIKey_HASH;
    case SDLK_PERCENT:           return UIKey_PERCENT;
    case SDLK_DOLLAR:            return UIKey_DOLLAR;
    case SDLK_AMPERSAND:         return UIKey_AMPERSAND;
    case SDLK_QUOTE:             return UIKey_QUOTE;
    case SDLK_LEFTPAREN:         return UIKey_LEFTPAREN;
    case SDLK_RIGHTPAREN:        return UIKey_RIGHTPAREN;
    case SDLK_ASTERISK:          return UIKey_ASTERISK;
    case SDLK_PLUS:              return UIKey_PLUS;
    case SDLK_COMMA:             return UIKey_COMMA;
    case SDLK_MINUS:             return UIKey_MINUS;
    case SDLK_PERIOD:            return UIKey_PERIOD;
    case SDLK_SLASH:             return UIKey_SLASH;
    case SDLK_0:                 return UIKey_0;
    case SDLK_1:                 return UIKey_1;
    case SDLK_2:                 return UIKey_2;
    case SDLK_3:                 return UIKey_3;
    case SDLK_4:                 return UIKey_4;
    case SDLK_5:                 return UIKey_5;
    case SDLK_6:                 return UIKey_6;
    case SDLK_7:                 return UIKey_7;
    case SDLK_8:                 return UIKey_8;
    case SDLK_9:                 return UIKey_9;
    case SDLK_COLON:             return UIKey_COLON;
    case SDLK_SEMICOLON:         return UIKey_SEMICOLON;
    case SDLK_LESS:              return UIKey_LESS;
    case SDLK_EQUALS:            return UIKey_EQUALS;
    case SDLK_GREATER:           return UIKey_GREATER;
    case SDLK_QUESTION:          return UIKey_QUESTION;
    case SDLK_AT:                return UIKey_AT;

    case SDLK_LEFTBRACKET:       return UIKey_LEFTBRACKET;
    case SDLK_BACKSLASH:         return UIKey_BACKSLASH;
    case SDLK_RIGHTBRACKET:      return UIKey_RIGHTBRACKET;
    case SDLK_CARET:             return UIKey_CARET;
    case SDLK_UNDERSCORE:        return UIKey_UNDERSCORE;
    case SDLK_BACKQUOTE:         return UIKey_BACKQUOTE;
    case SDLK_a:                 return UIKey_a;
    case SDLK_b:                 return UIKey_b;
    case SDLK_c:                 return UIKey_c;
    case SDLK_d:                 return UIKey_d;
    case SDLK_e:                 return UIKey_e;
    case SDLK_f:                 return UIKey_f;
    case SDLK_g:                 return UIKey_g;
    case SDLK_h:                 return UIKey_h;
    case SDLK_i:                 return UIKey_i;
    case SDLK_j:                 return UIKey_j;
    case SDLK_k:                 return UIKey_k;
    case SDLK_l:                 return UIKey_l;
    case SDLK_m:                 return UIKey_m;
    case SDLK_n:                 return UIKey_n;
    case SDLK_o:                 return UIKey_o;
    case SDLK_p:                 return UIKey_p;
    case SDLK_q:                 return UIKey_q;
    case SDLK_r:                 return UIKey_r;
    case SDLK_s:                 return UIKey_s;
    case SDLK_t:                 return UIKey_t;
    case SDLK_u:                 return UIKey_u;
    case SDLK_v:                 return UIKey_v;
    case SDLK_w:                 return UIKey_w;
    case SDLK_x:                 return UIKey_x;
    case SDLK_y:                 return UIKey_y;
    case SDLK_z:                 return UIKey_z;

    case SDLK_CAPSLOCK:          return UIKey_CAPSLOCK;

    case SDLK_F1:                return UIKey_F1;
    case SDLK_F2:                return UIKey_F2;
    case SDLK_F3:                return UIKey_F3;
    case SDLK_F4:                return UIKey_F4;
    case SDLK_F5:                return UIKey_F5;
    case SDLK_F6:                return UIKey_F6;
    case SDLK_F7:                return UIKey_F7;
    case SDLK_F8:                return UIKey_F8;
    case SDLK_F9:                return UIKey_F9;
    case SDLK_F10:               return UIKey_F10;
    case SDLK_F11:               return UIKey_F11;
    case SDLK_F12:               return UIKey_F12;

    case SDLK_PRINTSCREEN:       return UIKey_PRINTSCREEN;
    case SDLK_SCROLLLOCK:        return UIKey_SCROLLLOCK;
    case SDLK_PAUSE:             return UIKey_PAUSE;
    case SDLK_INSERT:            return UIKey_INSERT;
    case SDLK_HOME:              return UIKey_HOME;
    case SDLK_PAGEUP:            return UIKey_PAGEUP;
    case SDLK_DELETE:            return UIKey_DELETE;
    case SDLK_END:               return UIKey_END;
    case SDLK_PAGEDOWN:          return UIKey_PAGEDOWN;
    case SDLK_RIGHT:             return UIKey_RIGHT;
    case SDLK_LEFT:              return UIKey_LEFT;
    case SDLK_DOWN:              return UIKey_DOWN;
    case SDLK_UP:                return UIKey_UP;

    case SDLK_NUMLOCKCLEAR:      return UIKey_NUMLOCKCLEAR;
    case SDLK_KP_DIVIDE:         return UIKey_KP_DIVIDE;
    case SDLK_KP_MULTIPLY:       return UIKey_KP_MULTIPLY;
    case SDLK_KP_MINUS:          return UIKey_KP_MINUS;
    case SDLK_KP_PLUS:           return UIKey_KP_PLUS;
    case SDLK_KP_ENTER:          return UIKey_KP_ENTER;
    case SDLK_KP_1:              return UIKey_KP_1;
    case SDLK_KP_2:              return UIKey_KP_2;
    case SDLK_KP_3:              return UIKey_KP_3;
    case SDLK_KP_4:              return UIKey_KP_4;
    case SDLK_KP_5:              return UIKey_KP_5;
    case SDLK_KP_6:              return UIKey_KP_6;
    case SDLK_KP_7:              return UIKey_KP_7;
    case SDLK_KP_8:              return UIKey_KP_8;
    case SDLK_KP_9:              return UIKey_KP_9;
    case SDLK_KP_0:              return UIKey_KP_0;
    case SDLK_KP_PERIOD:         return UIKey_KP_PERIOD;

    case SDLK_APPLICATION:       return UIKey_APPLICATION;
    case SDLK_POWER:             return UIKey_POWER;
    case SDLK_KP_EQUALS:         return UIKey_KP_EQUALS;
    case SDLK_F13:               return UIKey_F13;
    case SDLK_F14:               return UIKey_F14;
    case SDLK_F15:               return UIKey_F15;
    case SDLK_F16:               return UIKey_F16;
    case SDLK_F17:               return UIKey_F17;
    case SDLK_F18:               return UIKey_F18;
    case SDLK_F19:               return UIKey_F19;
    case SDLK_F20:               return UIKey_F20;
    case SDLK_F21:               return UIKey_F21;
    case SDLK_F22:               return UIKey_F22;
    case SDLK_F23:               return UIKey_F23;
    case SDLK_F24:               return UIKey_F24;
    case SDLK_EXECUTE:           return UIKey_EXECUTE;
    case SDLK_HELP:              return UIKey_HELP;
    case SDLK_MENU:              return UIKey_MENU;
    case SDLK_SELECT:            return UIKey_SELECT;
    case SDLK_STOP:              return UIKey_STOP;
    case SDLK_AGAIN:             return UIKey_AGAIN;
    case SDLK_UNDO:              return UIKey_UNDO;
    case SDLK_CUT:               return UIKey_CUT;
    case SDLK_COPY:              return UIKey_COPY;
    case SDLK_PASTE:             return UIKey_PASTE;
    case SDLK_FIND:              return UIKey_FIND;
    case SDLK_MUTE:              return UIKey_MUTE;
    case SDLK_VOLUMEUP:          return UIKey_VOLUMEUP;
    case SDLK_VOLUMEDOWN:        return UIKey_VOLUMEDOWN;
    case SDLK_KP_COMMA:          return UIKey_KP_COMMA;
    case SDLK_KP_EQUALSAS400:    return UIKey_KP_EQUALSAS400;

    case SDLK_ALTERASE:          return UIKey_ALTERASE;
    case SDLK_SYSREQ:            return UIKey_SYSREQ;
    case SDLK_CANCEL:            return UIKey_CANCEL;
    case SDLK_CLEAR:             return UIKey_CLEAR;
    case SDLK_PRIOR:             return UIKey_PRIOR;
    case SDLK_RETURN2:           return UIKey_RETURN2;
    case SDLK_SEPARATOR:         return UIKey_SEPARATOR;
    case SDLK_OUT:               return UIKey_OUT;
    case SDLK_OPER:              return UIKey_OPER;
    case SDLK_CLEARAGAIN:        return UIKey_CLEARAGAIN;
    case SDLK_CRSEL:             return UIKey_CRSEL;
    case SDLK_EXSEL:             return UIKey_EXSEL;

    case SDLK_KP_00:             return UIKey_KP_00;
    case SDLK_KP_000:            return UIKey_KP_000;
    case SDLK_THOUSANDSSEPARATOR:return UIKey_THOUSANDSSEPARATOR;
    case SDLK_DECIMALSEPARATOR:  return UIKey_DECIMALSEPARATOR;
    case SDLK_CURRENCYUNIT:      return UIKey_CURRENCYUNIT;
    case SDLK_CURRENCYSUBUNIT:   return UIKey_CURRENCYSUBUNIT;
    case SDLK_KP_LEFTPAREN:      return UIKey_KP_LEFTPAREN;
    case SDLK_KP_RIGHTPAREN:     return UIKey_KP_RIGHTPAREN;
    case SDLK_KP_LEFTBRACE:      return UIKey_KP_LEFTBRACE;
    case SDLK_KP_RIGHTBRACE:     return UIKey_KP_RIGHTBRACE;
    case SDLK_KP_TAB:            return UIKey_KP_TAB;
    case SDLK_KP_BACKSPACE:      return UIKey_KP_BACKSPACE;
    case SDLK_KP_A:              return UIKey_KP_A;
    case SDLK_KP_B:              return UIKey_KP_B;
    case SDLK_KP_C:              return UIKey_KP_C;
    case SDLK_KP_D:              return UIKey_KP_D;
    case SDLK_KP_E:              return UIKey_KP_E;
    case SDLK_KP_F:              return UIKey_KP_F;
    case SDLK_KP_XOR:            return UIKey_KP_XOR;
    case SDLK_KP_POWER:          return UIKey_KP_POWER;
    case SDLK_KP_PERCENT:        return UIKey_KP_PERCENT;
    case SDLK_KP_LESS:           return UIKey_KP_LESS;
    case SDLK_KP_GREATER:        return UIKey_KP_GREATER;
    case SDLK_KP_AMPERSAND:      return UIKey_KP_AMPERSAND;
    case SDLK_KP_DBLAMPERSAND:   return UIKey_KP_DBLAMPERSAND;
    case SDLK_KP_VERTICALBAR:    return UIKey_KP_VERTICALBAR;
    case SDLK_KP_DBLVERTICALBAR: return UIKey_KP_DBLVERTICALBAR;
    case SDLK_KP_COLON:          return UIKey_KP_COLON;
    case SDLK_KP_HASH:           return UIKey_KP_HASH;
    case SDLK_KP_SPACE:          return UIKey_KP_SPACE;
    case SDLK_KP_AT:             return UIKey_KP_AT;
    case SDLK_KP_EXCLAM:         return UIKey_KP_EXCLAM;
    case SDLK_KP_MEMSTORE:       return UIKey_KP_MEMSTORE;
    case SDLK_KP_MEMRECALL:      return UIKey_KP_MEMRECALL;
    case SDLK_KP_MEMCLEAR:       return UIKey_KP_MEMCLEAR;
    case SDLK_KP_MEMADD:         return UIKey_KP_MEMADD;
    case SDLK_KP_MEMSUBTRACT:    return UIKey_KP_MEMSUBTRACT;
    case SDLK_KP_MEMMULTIPLY:    return UIKey_KP_MEMMULTIPLY;
    case SDLK_KP_MEMDIVIDE:      return UIKey_KP_MEMDIVIDE;
    case SDLK_KP_PLUSMINUS:      return UIKey_KP_PLUSMINUS;
    case SDLK_KP_CLEAR:          return UIKey_KP_CLEAR;
    case SDLK_KP_CLEARENTRY:     return UIKey_KP_CLEARENTRY;
    case SDLK_KP_BINARY:         return UIKey_KP_BINARY;
    case SDLK_KP_OCTAL:          return UIKey_KP_OCTAL;
    case SDLK_KP_DECIMAL:        return UIKey_KP_DECIMAL;
    case SDLK_KP_HEXADECIMAL:    return UIKey_KP_HEXADECIMAL;

    case SDLK_LCTRL:             return UIKey_LCTRL;
    case SDLK_LSHIFT:            return UIKey_LSHIFT;
    case SDLK_LALT:              return UIKey_LALT;
    case SDLK_LGUI:              return UIKey_LGUI;
    case SDLK_RCTRL:             return UIKey_RCTRL;
    case SDLK_RSHIFT:            return UIKey_RSHIFT;
    case SDLK_RALT:              return UIKey_RALT;
    case SDLK_RGUI:              return UIKey_RGUI;

    case SDLK_MODE:              return UIKey_MODE;

    case SDLK_AUDIONEXT:         return UIKey_AUDIONEXT;
    case SDLK_AUDIOPREV:         return UIKey_AUDIOPREV;
    case SDLK_AUDIOSTOP:         return UIKey_AUDIOSTOP;
    case SDLK_AUDIOPLAY:         return UIKey_AUDIOPLAY;
    case SDLK_AUDIOMUTE:         return UIKey_AUDIOMUTE;
    case SDLK_MEDIASELECT:       return UIKey_MEDIASELECT;
    case SDLK_WWW:               return UIKey_WWW;
    case SDLK_MAIL:              return UIKey_MAIL;
    case SDLK_CALCULATOR:        return UIKey_CALCULATOR;
    case SDLK_COMPUTER:          return UIKey_COMPUTER;
    case SDLK_AC_SEARCH:         return UIKey_AC_SEARCH;
    case SDLK_AC_HOME:           return UIKey_AC_HOME;
    case SDLK_AC_BACK:           return UIKey_AC_BACK;
    case SDLK_AC_FORWARD:        return UIKey_AC_FORWARD;
    case SDLK_AC_STOP:           return UIKey_AC_STOP;
    case SDLK_AC_REFRESH:        return UIKey_AC_REFRESH;
    case SDLK_AC_BOOKMARKS:      return UIKey_AC_BOOKMARKS;

    case SDLK_BRIGHTNESSDOWN:    return UIKey_BRIGHTNESSDOWN;
    case SDLK_BRIGHTNESSUP:      return UIKey_BRIGHTNESSUP;
    case SDLK_DISPLAYSWITCH:     return UIKey_DISPLAYSWITCH;
    case SDLK_KBDILLUMTOGGLE:    return UIKey_KBDILLUMTOGGLE;
    case SDLK_KBDILLUMDOWN:      return UIKey_KBDILLUMDOWN;
    case SDLK_KBDILLUMUP:        return UIKey_KBDILLUMUP;
    case SDLK_EJECT:             return UIKey_EJECT;
    case SDLK_SLEEP:             return UIKey_SLEEP;
    case SDLK_APP1:              return UIKey_APP1;
    case SDLK_APP2:              return UIKey_APP2;

    case SDLK_AUDIOREWIND:       return UIKey_AUDIOREWIND;
    case SDLK_AUDIOFASTFORWARD:  return UIKey_AUDIOFASTFORWARD;

    case SDLK_SOFTLEFT:          return UIKey_SOFTLEFT;
    case SDLK_SOFTRIGHT:         return UIKey_SOFTRIGHT;
    case SDLK_CALL:              return UIKey_CALL;
    case SDLK_ENDCALL:           return UIKey_ENDCALL;
    }
    // clang-format on
}
