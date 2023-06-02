#pragma once
#include "uikey.h"

typedef struct UIContext UI;

int ui_init(void);
void ui_deinit(void);

UI* ui_create(const char *window_name, int window_width, int window_height);
void ui_destroy(UI *);
void ui_handle_events(UI *);

void ui_set_on_quit_event(UI *context, void(*)(void*), void *);
void ui_set_on_key_down_event(UI *ui, void(*callback)(void*, UIKey), void *context);
void ui_set_on_key_up_event(UI *ui, void(*callback)(void*, UIKey), void *context);

void ui_render_frame(UI *);

struct nk_context *ui_get_nuklear_context(UI *);
