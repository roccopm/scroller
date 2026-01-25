#define SDL_MAIN_HANDLED
#include "config.h"
#include "renderer.h"
#include "text_entity.h"
#include <SDL.h>
#include <stdio.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    // Check if config file exists, generate if missing
    const char *config_file = (argc > 1) ? argv[1] : "config.ini";
    struct stat buffer;
    if (stat(config_file, &buffer) != 0) {
        printf("Config file '%s' not found, generating default config...\n", config_file);
        if (generate_default_config(config_file) != 0) {
            printf("Failed to generate default config file!\n");
            return -1;
        }
    }

    // Load configuration
    AppConfig *config = load_config(config_file);
    if (config == NULL) {
        printf("Failed to load configuration!\n");
        return -1;
    }

    Renderer *renderer = renderer_init(config->window_width, config->window_height, config->font_path, config->font_size, config->text_color, config->background_color);
    if (renderer == NULL) {
        printf("Failed to initialize renderer!\n");
        free_config(config);
        return -1;
    }

    // Create text entities from config sentences
    TextEntity *entities[MAX_SENTENCES];
    int entity_count = 0;
    int start_x = config->window_width;

    if (config->sentence_count > 0) {
        for (int i = 0; i < config->sentence_count; i++) {
            entities[i] = create_text_entity(
                config->sentences[i],
                start_x);
            entity_count++;

            update_entity_dimensions(renderer, entities[i]);
            start_x += entities[i]->width + config->spacing;
        }
    } else {
        entities[entity_count] = create_text_entity("Welcome to Scroller! Press C to open the config menu and add your own messages. You can also customise the speed, colours, font, and more!", start_x);
        update_entity_dimensions(renderer, entities[entity_count]);
        entity_count++;
    }

    // Main loop
    SDL_Event e;
    int quit = 0;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

        for (int i = 0; i < entity_count; i++) {
            update_text_entity(entities[i], config->scroll_speed);
        }

        // Check if first sentence has scrolled off-screen, then move it to the end
        if (entity_count > 0 && entities[0]->x + entities[0]->width < 0) {
            TextEntity *first = entities[0];
            for (int i = 0; i < entity_count - 1; i++) {
                entities[i] = entities[i + 1];
            }
            entities[entity_count - 1] = first;

            // Position it after the last sentence + spacing
            if (entity_count > 1) {
                entities[entity_count - 1]->x = entities[entity_count - 2]->x + entities[entity_count - 2]->width + config->spacing;
            } else {
                entities[entity_count - 1]->x = config->window_width;
            }
        }

        renderer_clear(renderer);
        render_text_entities(renderer, entities, entity_count, config->padding_top);
        renderer_present(renderer);
    }

    for (int i = 0; i < entity_count; i++) {
        free_text_entity(entities[i]);
    }

    renderer_cleanup(renderer);
    free_config(config);

    return 0;
}