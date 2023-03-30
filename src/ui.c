#include "ui.h"

#include <stdlib.h>
#include <SDL2/SDL.h>

#include "nuklear.h"
#include "nuklear_sdl_renderer.h"

struct UIContext {
    SDL_Window *sdl_window;
    SDL_Renderer *sdl_renderer;
    struct nk_context *nuklear_context;

    void (*on_quit_callback)(void*);
    void *on_quit_context;
} static ui_invalid = { 0 };

void ui_init(void)
{
    SDL_SetHint(SDL_HINT_VIDEO_HIGHDPI_DISABLED, "0");
    SDL_Init(SDL_INIT_VIDEO);
}

void ui_deinit(void)
{
    SDL_Quit();
}

UI* ui_create(const char *window_name, int window_width, int window_height)
{
    UI* ui = malloc(sizeof(UI));
    if (!ui)
        goto fin_0;
    *ui = ui_invalid;

    ui->sdl_window = SDL_CreateWindow(window_name,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        window_width, window_height, SDL_WINDOW_SHOWN|SDL_WINDOW_ALLOW_HIGHDPI);

    if (ui->sdl_window == NULL) {
        SDL_Log("Error SDL_CreateWindow %s", SDL_GetError());
        goto fin_1;
    }

    ui->sdl_renderer = SDL_CreateRenderer(ui->sdl_window, -1,
        SDL_RENDERER_ACCELERATED|SDL_RENDERER_PRESENTVSYNC);
    if (ui->sdl_renderer == NULL) {
        SDL_Log("Error SDL_CreateRenderer %s", SDL_GetError());
        goto fin_2;
    }

    float font_scale = 1;
    {
        int render_w, render_h;
        int window_w, window_h;
        float scale_x, scale_y;
        SDL_GetRendererOutputSize(ui->sdl_renderer, &render_w, &render_h);
        SDL_GetWindowSize(ui->sdl_window, &window_w, &window_h);
        scale_x = (float)(render_w) / (float)(window_w);
        scale_y = (float)(render_h) / (float)(window_h);
        SDL_RenderSetScale(ui->sdl_renderer, scale_x, scale_y);
        font_scale = scale_y;
    }

    ui->nuklear_context = nk_sdl_init(ui->sdl_window, ui->sdl_renderer);
    if (!ui->nuklear_context)
        goto fin_3;
    {
        struct nk_font_atlas *atlas;
        struct nk_font_config config = nk_font_config(0);
        nk_sdl_font_stash_begin(&atlas);
        struct nk_font *font = nk_font_atlas_add_default(atlas, 13 * font_scale, &config);
        nk_sdl_font_stash_end();

        font->handle.height /= font_scale;
        nk_style_set_font(ui->nuklear_context, &font->handle);
    }

    return ui;
fin_3: SDL_DestroyRenderer(ui->sdl_renderer);
fin_2: SDL_DestroyWindow(ui->sdl_window);
fin_1: free(ui);
fin_0: return NULL;
}

void ui_destroy(UI *ui)
{
    nk_sdl_shutdown();
    SDL_DestroyRenderer(ui->sdl_renderer);
    SDL_DestroyWindow(ui->sdl_window);
    *ui = ui_invalid;
    free(ui);
}

struct nk_context *ui_get_nuklear_context(const UI *ui)
{
    return ui->nuklear_context;
}

void ui_handle_events(const UI *ui)
{
    SDL_Event evt;
    nk_input_begin(ui->nuklear_context);
    while (SDL_PollEvent(&evt)) {
        if (evt.type == SDL_QUIT && ui->on_quit_callback)
            ui->on_quit_callback(ui->on_quit_context);
        nk_sdl_handle_event(&evt);
    }
    nk_input_end(ui->nuklear_context);
}

void ui_render_frame(const UI *ui)
{
    SDL_RenderClear(ui->sdl_renderer);
    nk_sdl_render(NK_ANTI_ALIASING_ON);
    SDL_RenderPresent(ui->sdl_renderer);
}

void ui_set_on_quit_event(UI *ui, void(*on_quit)(void*), void *context)
{
    ui->on_quit_callback = on_quit;
    ui->on_quit_context = context;
}
