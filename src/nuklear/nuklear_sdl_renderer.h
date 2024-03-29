#pragma once
#include "nuklear.h"
#include <SDL2/SDL.h>

struct nk_sdl_device {
    struct nk_buffer cmds;
    struct nk_draw_null_texture tex_null;
    SDL_Texture *font_tex;
};

struct nk_sdl {
    SDL_Window *win;
    SDL_Renderer *renderer;
    struct nk_sdl_device ogl;
    struct nk_context ctx;
    struct nk_font_atlas atlas;
};

NK_API struct nk_sdl nk_sdl_init(SDL_Window *win, SDL_Renderer *renderer);
NK_API void nk_sdl_font_stash_begin(struct nk_sdl *, struct nk_font_atlas **);
NK_API void nk_sdl_font_stash_end(struct nk_sdl *);
NK_API int nk_sdl_handle_event(struct nk_sdl *, SDL_Event *evt);
NK_API void nk_sdl_render(struct nk_sdl *, enum nk_anti_aliasing);
NK_API void nk_sdl_deinit(struct nk_sdl *);

#if SDL_COMPILEDVERSION < SDL_VERSIONNUM(2, 0, 22)
/* Metal API does not support cliprects with negative coordinates or large
 * dimensions. The issue is fixed in SDL2 with version 2.0.22 but until
 * that version is released, the NK_SDL_CLAMP_CLIP_RECT flag can be used to
 * ensure the cliprect is itself clipped to the viewport.
 * See discussion at https://discourse.libsdl.org/t/rendergeometryraw-producing-different-results-in-metal-vs-opengl/34953
 */
#define NK_SDL_CLAMP_CLIP_RECT
#endif
