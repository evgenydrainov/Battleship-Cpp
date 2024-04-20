#pragma once

#include <raylib.h>

#define ArrayLength(a) (sizeof(a)/sizeof(a[0]))

RenderTexture2D LoadRenderTextureNoAlphaNoDepth(int width, int height);

int mouse_get_x_world();
int mouse_get_y_world();

void DrawCross(int x, int y, int width, int height, Color color);
