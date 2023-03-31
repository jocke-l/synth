#pragma once

typedef struct UIContext UI;

int ui_init(void);
void ui_deinit(void);

UI* ui_create(const char *window_name, int window_width, int window_height);
void ui_destroy(UI *);
void ui_handle_events(const UI *);
void ui_set_on_quit_event(UI *context, void(*)(void*), void *);
void ui_render_frame(const UI *);

struct nk_context *ui_get_nuklear_context(const UI *);
