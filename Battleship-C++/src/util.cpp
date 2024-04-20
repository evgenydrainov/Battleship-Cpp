#include "util.h"

#include "game.h"
#include <rlgl.h>

RenderTexture2D LoadRenderTextureNoAlphaNoDepth(int width, int height) {
	RenderTexture2D target = { 0 };

	target.id = rlLoadFramebuffer(width, height);   // Load an empty framebuffer

	if (target.id > 0) {
		rlEnableFramebuffer(target.id);

		// Create color texture (default to RGBA)
		target.texture.id = rlLoadTexture(NULL, width, height, PIXELFORMAT_UNCOMPRESSED_R8G8B8, 1);
		target.texture.width = width;
		target.texture.height = height;
		target.texture.format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
		target.texture.mipmaps = 1;

		// Attach color texture and depth renderbuffer/texture to FBO
		rlFramebufferAttach(target.id, target.texture.id, RL_ATTACHMENT_COLOR_CHANNEL0, RL_ATTACHMENT_TEXTURE2D, 0);

		// Check if fbo is complete with attachments (valid)
		if (rlFramebufferComplete(target.id)) {
			TRACELOG(LOG_INFO, "FBO: [ID %i] Framebuffer object created successfully", target.id);
		}

		rlDisableFramebuffer();
	} else {
		TRACELOG(LOG_WARNING, "FBO: Framebuffer object can not be created");
	}

	return target;
}

int mouse_get_x_world() {
	return (int)((float)GetMouseX() / (float)GetScreenWidth() * (float)GAME_W);
}

int mouse_get_y_world() {
	return (int)((float)GetMouseY() / (float)GetScreenHeight() * (float)GAME_H);
}

void DrawCross(int x, int y, int width, int height, Color color) {
	DrawLine(x, y, x + width, y + height, color);
	DrawLine(x + width, y, x, y + height, color);
}
