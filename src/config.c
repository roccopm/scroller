#include "config.h"
#define INI_USE_STACK 0
#define INI_ALLOW_REALLOC 1
#define INI_INITIAL_ALLOC 256
#include "ini.h"
#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int parse_rgba(const char *value, SDL_Color *color) {
    size_t len = strlen(value);
    char *value_copy = (char *)malloc(len + 1);
    if (value_copy == NULL) {
        return 0;
    }
    strcpy(value_copy, value);

    char *token;
    char *saveptr;
    int components[4] = {255, 255, 255, 255};
    int count = 0;

    token = strtok_r(value_copy, ",", &saveptr);
    while (token != NULL && count < 4) {
        while (*token == ' ' || *token == '\t') {
            token++;
        }
        components[count] = atoi(token);
        count++;
        token = strtok_r(NULL, ",", &saveptr);
    }

    free(value_copy);

    if (count >= 3) {
        color->r = components[0];
        color->g = components[1];
        color->b = components[2];
        color->a = (count >= 4) ? components[3] : 255;
        return 1;
    }

    return 0;
}

static int config_handler(void *user, const char *section, const char *name, const char *value) {
    AppConfig *config = (AppConfig *)user;

    if (strcmp(name, "window_width") == 0) {
        config->window_width = atoi(value);
    } else if (strcmp(name, "window_height") == 0) {
        config->window_height = atoi(value);
    } else if (strcmp(name, "padding_top") == 0) {
        config->padding_top = atoi(value);
    }

    else if (strcmp(name, "scroll_speed") == 0) {
        config->scroll_speed = atoi(value);
    } else if (strcmp(name, "spacing") == 0) {
        config->spacing = atoi(value);
    }

    else if (strcmp(name, "text_color") == 0) {
        if (!parse_rgba(value, &config->text_color)) {
            printf("Warning: Invalid text_color format, expected 'R,G,B,A' or 'R,G,B'\n");
        }
    } else if (strcmp(name, "background_color") == 0) {
        if (!parse_rgba(value, &config->background_color)) {
            printf("Warning: Invalid background_color format, expected 'R,G,B,A' or 'R,G,B'\n");
        }
    }

    else if (strcmp(name, "font_path") == 0) {
        strncpy(config->font_path, value, sizeof(config->font_path) - 1);
        config->font_path[sizeof(config->font_path) - 1] = '\0';
    } else if (strcmp(name, "font_size") == 0) {
        config->font_size = atoi(value);
    }

    else if (strcmp(name, "sentence") == 0) {
        if (config->sentence_count < MAX_SENTENCES) {
            strncpy(config->sentences[config->sentence_count], value, MAX_SENTENCE_LENGTH - 1);
            config->sentences[config->sentence_count][MAX_SENTENCE_LENGTH - 1] = '\0';
            config->sentence_count++;
        }
    }

    return 1;
}

AppConfig *load_config(const char *filename) {
    AppConfig *config = (AppConfig *)malloc(sizeof(AppConfig));
    if (config == NULL) {
        printf("Failed to allocate memory for config!\n");
        return NULL;
    }

    config->window_width = 800;
    config->window_height = 600;
    config->padding_top = 0;
    config->scroll_speed = 2;
    config->spacing = 200;
    config->text_color.r = 255;
    config->text_color.g = 255;
    config->text_color.b = 255;
    config->text_color.a = 255;
    config->background_color.r = 25;
    config->background_color.g = 25;
    config->background_color.b = 112;
    config->background_color.a = 255;
    strcpy(config->font_path, "fonts/arial.ttf");
    config->font_size = 24;
    config->sentence_count = 0;

    if (parse_config_file(filename, config) != 0) {
        printf("Failed to parse config file, using defaults\n");
    }

    return config;
}

void free_config(AppConfig *config) {
    if (config != NULL) {
        free(config);
    }
}

int parse_config_file(const char *filename, AppConfig *config) {
    int result = ini_parse(filename, config_handler, config);
    if (result < 0) {
        printf("Config file '%s' not found, using defaults\n", filename);
    } else if (result > 0) {
        printf("Error parsing config file '%s' at line %d, using defaults\n", filename, result);
    }
    return result == 0 ? 0 : -1;
}

int generate_default_config(const char *filename) {
    // Use SDL to get display information
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL for monitor detection: %s\n", SDL_GetError());
        return -1;
    }

    SDL_Rect display_bounds;
    if (SDL_GetDisplayBounds(0, &display_bounds) != 0) {
        printf("Failed to get display bounds: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    int monitor_width = display_bounds.w;
    SDL_Quit(); // once we have the width, we don't need SDL anymore

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Failed to create config file '%s'\n", filename);
        return -1;
    }

    // Generate a config with these defaults if there isn't one
    const char *config_template =
        "# Window settings\n"
        "window_width = %d\n"
        "window_height = 32\n"
        "padding_top = -4\n"
        "\n"
        "# Scrolling settings\n"
        "scroll_speed = 2\n"
        "spacing = 300\n"
        "\n"
        "# Colours\n"
        "text_color = 255,255,255,255\n"
        "background_color = 25,25,112,255\n"
        "\n"
        "# Font settings\n"
        "font_path = fonts/LiberationSans-Regular.ttf\n"
        "font_size = 32\n"
        "\n"
        "# Sentences to cycle through (add as many as you like)\n"
        "sentence = Welcome to the Scrolling Text Demo!\n"
        "sentence = You can customize colors, speed, and text content\n"
        "sentence = The sentences will scroll one after another\n"
        "sentence = And then repeat when they reach the end\n";

    fprintf(file, config_template, monitor_width);
    fclose(file);
    printf("Generated default config file '%s' with monitor width %d\n", filename, monitor_width);
    return 0;
}