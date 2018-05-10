#ifndef _RENDERER_H
#define _RENDERER_H

#include "engine.h"

// prefered display to show the window
#ifndef PREFERRED_DISPLAY
    #define PREFERRED_DISPLAY 1
#endif

extern unsigned gw;
extern unsigned gh;

typedef struct {
	SDL_Window *sdl_window;
    SDL_Renderer *renderer;
    // GPU_Target *screen;
    // GPU_Target *target;
    // GPU_Image *renderTargetImage;
    SDL_Color default_color;
    SDL_Color clearColor;
} gfx;

extern gfx renderer;

struct Sprite {
    SDL_Texture *image;
    bool isValid() {
        return image != NULL;
    }
};

struct Font {
    //NFont *font;
	inline void set_color(const SDL_Color &color) {
		//font->setDefaultColor(color);
	}
};

namespace Resources {
    Sprite *loadSprite(const std::string& name, const std::string &filename);
    Sprite *getSprite(const std::string& name);

    Font *loadFont(const std::string& name, const std::string &filename, int pointSize);
    Font *getFont(const std::string& name);

    void removeSprite(const std::string& name);
    void removeFont(const std::string& name);
    void cleanup();
}

namespace Colors {
	const SDL_Color white = { 255, 255, 255, 255 };
	const SDL_Color black = { 0, 0, 0, 255 };
}

void setWindowPosition(int x, int y);
void centerWindow();
void setWindowTitle(const char* title);
void setWindowScale(float s);
void toggleFullscreen(bool useDesktopResolution);
void setDefaultFont(Font *font);

void drawFilledRect(int x1, int y1, int x2, int y2, const SDL_Color &color);
void drawFilledRectRounded(int x1, int y1, int x2, int y2, float radius, const SDL_Color &color);
void drawCircle(int x, int y, int radius, const SDL_Color &color);
void drawFilledCircle(int x, int y, int radius, const SDL_Color &color);
void drawSprite(Sprite *sprite, int x, int y);
void drawText(int x, int y, const SDL_Color &color, const char *text);
void drawText(Font *font, int x, int y, const SDL_Color &color, const char *text);
void drawTextCenter(int x, int y, const char *text);
void drawTextCenter(Font *font, int x, int y, const char *text);

int initRenderer(const char *title, unsigned vw, unsigned vh, unsigned scale);
void rendererSetClearColor(const SDL_Color &color);
void rendererClear();
void rendererDrawRenderTargetToScreen();
void rendererFlip();
void rendererDestroy();

// GPU_Camera *getCamera();
// void renderer_clearCamera();
// void renderer_resetCamera();
// void camera_lookat(int x, int y);
// void camera_move(int x, int y);
#endif