#include "config.h"
#include "ini.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Handler function for inih parser
static int config_handler(void *user, const char *section, const char *name, const char *value) {
    AppConfig *config = (AppConfig *)user;

    // Parse window settings
    if (strcmp(name, "window_width") == 0) {
        config->window_width = atoi(value);
    } else if (strcmp(name, "window_height") == 0) {
        config->window_height = atoi(value);
    }

    else if (strcmp(name, "scroll_speed") == 0) {
        config->scroll_speed = atoi(value);
    } else if (strcmp(name, "spacing") == 0) {
        config->spacing = atoi(value);
    }

    else if (strcmp(name, "text_color_r") == 0) {
        config->text_color.r = atoi(value);
    } else if (strcmp(name, "text_color_g") == 0) {
        config->text_color.g = atoi(value);
    } else if (strcmp(name, "text_color_b") == 0) {
        config->text_color.b = atoi(value);
    } else if (strcmp(name, "text_color_a") == 0) {
        config->text_color.a = atoi(value);
    }

    else if (strcmp(name, "font_path") == 0) {
        strncpy(config->font_path, value, sizeof(config->font_path) - 1);
        config->font_path[sizeof(config->font_path) - 1] = '\0';
    } else if (strcmp(name, "font_size") == 0) {
        config->font_size = atoi(value);
    }

    else if (strncmp(name, "sentence_", 9) == 0) {
        int index = atoi(name + 9) - 1; // Convert sentence_1 to index 0
        if (index >= 0 && index < MAX_SENTENCES) {
            strncpy(config->sentences[index], value, MAX_SENTENCE_LENGTH - 1);
            config->sentences[index][MAX_SENTENCE_LENGTH - 1] = '\0';
            if (index >= config->sentence_count) {
                config->sentence_count = index + 1;
            }
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
    config->scroll_speed = 2;
    config->spacing = 200;
    config->text_color.r = 255;
    config->text_color.g = 255;
    config->text_color.b = 255;
    config->text_color.a = 255;
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