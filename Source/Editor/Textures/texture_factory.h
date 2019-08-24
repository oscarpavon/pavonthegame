#ifndef TEXTURES_RENDER_H
#define TEXTURES_RENDER_H

#include "../../Engine/model.h"

const char* texture_current_export_name;

void init_UV_draw(Model* model);

void draw_UV();

void merge_textures(const char* name);

void render_to_texture(int size, void(*function)(int) );

void init_model_to_draw_texture();

void scale_UV(float size, Model* model, vec2 UV_offset);

void translate_UV(vec3 tranlation, Model* model, vec2 UV_offset);

void texture_export(int size);

#endif // !TEXTURES_RENDER_H