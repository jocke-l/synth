#include "ui.h"

#include <stdlib.h>
#include <SDL2/SDL.h>

#include <nuklear/nuklear.h>
#include <nuklear/nuklear_sdl_renderer.h>

struct UIContext {
    struct nk_sdl sdl;

    void (*on_quit_callback)(void*);
    void *on_quit_context;
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

void ui_set_on_quit_event(UI *ui, void(*on_quit)(void*), void *context)
{
    ui->on_quit_callback = on_quit;
    ui->on_quit_context = context;
}
