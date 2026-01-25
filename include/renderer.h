#ifndef RENDERER_H
#define RENDERER_H

#include "text_entity.h"
#include <SDL.h>
#include <SDL_ttf.h>

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    SDL_Color text_color;
} Renderer;

Renderer *renderer_init(int window_width, int window_height, const char *font_path, int font_size, SDL_Color text_color, SDL_Color background_color);
void renderer_cleanup(Renderer *renderer);
void render_text_entities(const Renderer *renderer, TextEntity **entities, int count, int padding_top);
void renderer_clear(const Renderer *renderer);
void renderer_present(const Renderer *renderer);
void update_entity_dimensions(const Renderer *renderer, TextEntity *entity);

#endif
