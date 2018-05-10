#include "renderer.h"
#include "SDL_image.h"

unsigned gw;
unsigned gh;
unsigned stepScale;
float w;
float h;
int windowPos;

Font *defaultFont;
gfx renderer;

namespace Resources {
	std::map<std::string, Sprite*> sprites;
	std::map<std::string, Font*> fonts;

	static SDL_Texture* loadTexture(const std::string &path) { 
		//The final texture 
		SDL_Texture* newTexture = NULL; 
		//Load image at specified path 
		SDL_Surface* loadedSurface = IMG_Load( path.c_str() ); 
		if( loadedSurface == NULL ) { 
			printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() ); 
		} else { 
			//Create texture from surface pixels 
			newTexture = SDL_CreateTextureFromSurface(renderer.renderer, loadedSurface ); 
			if( newTexture == NULL ) { 
				printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() ); 
			} 
			//Get rid of old loaded surface 
			SDL_FreeSurface( loadedSurface ); 
		} 
		return newTexture; 
	}

    Sprite *loadSprite(const std::string& name, const std::string &filename) {
		Sprite *s = new Sprite;
    	s->image = loadTexture(filename);
		sprites[name] = s;
		return s;
	}

    Sprite *getSprite(const std::string& name) {
		return sprites.at(name);
	}

    Font *loadFont(const std::string& name, const std::string &filename, int pointSize) {
		Font *f = new Font;
		//f->font = new NFont(filename.c_str(), pointSize);
		fonts[name] = f;
		return f;
	}

    Font *getFont(const std::string& name) {
		return fonts.at(name);
	}
	
    void removeSprite(const std::string& name) {
		auto itr = sprites.find(name);
		if (itr != sprites.end()) {
    		delete itr->second;
    		sprites.erase(itr);
		}
	}

    void removeFont(const std::string& name) {
		auto itr = fonts.find(name);
		if (itr != fonts.end()) {
    		delete itr->second;
    		fonts.erase(itr);
		}
	}

    void cleanup() {
		for(std::map<std::string, Sprite*>::iterator itr = sprites.begin(); itr != sprites.end(); itr++) {
			//GPU_FreeImage(itr->second->image);
        	delete itr->second;
    	}
		sprites.clear();
		for(std::map<std::string, Font*>::iterator itr = fonts.begin(); itr != fonts.end(); itr++) {
        	delete itr->second;
    	}
		fonts.clear();
	}
}

void setWindowPosition(int x, int y) {
	SDL_SetWindowPosition(renderer.sdl_window, x, y);
}

void centerWindow() {
	setWindowPosition(windowPos, windowPos);
}

void setWindowTitle(const char* title) {
	SDL_SetWindowTitle(renderer.sdl_window, title);
}

void setWindowScale(float s) {
	if (s == stepScale)
		return;

	stepScale = s;
	w = gw * s;
	h = gh * s;
	
	//GPU_SetWindowResolution(w, h);
}

void toggleFullscreen(bool useDesktopResolution) {
	//GPU_SetFullscreen(!GPU_GetFullscreen(), useDesktopResolution);
}

void setDefaultFont(Font *font) {
	defaultFont = font;
}

void drawFilledRect(int x1, int y1, int x2, int y2, const SDL_Color &color) {
	//SDL_FillRect()
    //GPU_RectangleFilled(renderer.target, x1, y1, x2, y2, color);
}

void drawFilledRectRounded(int x1, int y1, int x2, int y2, float radius, const SDL_Color &color) {
    //GPU_RectangleRoundFilled(renderer.target, x1, y1, x2, y2, radius, color);
}

void drawCircle(int x, int y, int radius, const SDL_Color &color) {
    //GPU_Circle(renderer.target, x, y, radius, color);
}

void drawFilledCircle(int x, int y, int radius, const SDL_Color &color) {
    //GPU_CircleFilled(renderer.target, x, y, radius, color);
}

void drawSprite(Sprite *sprite, int x, int y) {
	static int w;
	static int h;
	SDL_QueryTexture(sprite->image, NULL, NULL, &w, &h);

	static SDL_Rect DestR;
	DestR.x = x - (w / 2);
 	DestR.y = y - (h / 2);
  	DestR.w = w;
  	DestR.h = h;

	SDL_RenderCopy(renderer.renderer, sprite->image, NULL, &DestR);
	//GPU_Blit(sprite->image, src_rect, renderer.target, x, y);
}

void drawText(int x, int y, const SDL_Color &color, const char *text) {
	//defaultFont->font->draw(renderer.target, x, y, color, text);
}

void drawText(Font *font, int x, int y, const SDL_Color &color, const char *text) {
	//font->font->draw(renderer.target, x, y, color, text);
}

void drawTextCenter(int x, int y, const char *text) {
	//defaultFont->font->draw(renderer.target, x, y, NFont::CENTER, text);
}

void drawTextCenter(Font *font, int x, int y, const char *text) {
	//font->font->draw(renderer.target, x, y, NFont::CENTER, text);
}

int initRenderer(const char *title, unsigned vw, unsigned vh, unsigned scale) {
	gw = vw;
	gh = vh;
	stepScale = scale;
	w = gw * stepScale;
	h = gh * stepScale;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		printf("Could not initialize: %s\n", SDL_GetError());
		return -1;
	} 
	
	renderer.sdl_window = SDL_CreateWindow("SDL2", 
		SDL_WINDOWPOS_UNDEFINED, 
		SDL_WINDOWPOS_UNDEFINED, 
		640, 
		360, 
		SDL_WINDOW_SHOWN );
	//renderer.screen = GPU_Init(w, h, GPU_DEFAULT_INIT_FLAGS);
	//renderer.screen = GPU_InitRenderer(GPU_RENDERER_OPENGL_3, 640, 360, GPU_DEFAULT_INIT_FLAGS);
	
	// if (renderer.screen == NULL) {
	// 	printf("GPU INIT FAILED");
	// 	return -1;
	// }

	renderer.renderer = SDL_CreateRenderer(renderer.sdl_window, -1, SDL_RENDERER_ACCELERATED );

    renderer.clearColor = { 0, 0, 0, 255 };
	SDL_SetRenderDrawColor(renderer.renderer, 0x00, 0x00, 0x00, 0xFF ); 
	
	//Initialize PNG loading 
	int imgFlags = IMG_INIT_PNG; 
	if( !( IMG_Init( imgFlags ) & imgFlags ) ) { 
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() ); 
		abort();
	}


    SDL_ShowCursor(SDL_DISABLE);

	setWindowTitle(title);
	
    windowPos = SDL_WINDOWPOS_CENTERED;
	int displays = SDL_GetNumVideoDisplays();
	printf("detected [ %d ] displays\n", displays);
	if(displays > 1 && PREFERRED_DISPLAY) {
		windowPos = SDL_WINDOWPOS_CENTERED_DISPLAY(1);
	}
	setWindowPosition(windowPos, windowPos);
    
	// GPU_Image* renderTargetImage;
	// renderTargetImage = GPU_CreateImage(gw, gh, GPU_FORMAT_RGBA);
	// GPU_LoadTarget(renderTargetImage);
	
    // renderer.renderTargetImage = renderTargetImage;
    // renderer.target = renderTargetImage->target;
	// renderer.sdl_window = SDL_GetWindowFromID(renderer.screen->context->windowID);

	// GPU_SetImageFilter(renderTargetImage, GPU_FILTER_NEAREST);

	// const SDL_Color defaltFontColor = { 255, 255, 255, 255 };
	// defaultFont = Resources::loadFont("default", "data/fonts/pixeltype.ttf", 14);
	// defaultFont->set_color(defaltFontColor);
	
	// camera = GPU_GetDefaultCamera();

    return 1;
}

void rendererSetClearColor(const SDL_Color &color) {
    renderer.clearColor = color;
}

void rendererClear() {
	SDL_RenderClear(renderer.renderer);
	//GPU_SetCamera(renderer.screen, NULL);
    //GPU_ClearColor(renderer.target, renderer.clearColor);
	//GPU_SetCamera(renderer.target, &camera);
}

void rendererDrawRenderTargetToScreen() {
	//GPU_BlitScale(renderer.renderTargetImage, NULL, renderer.screen, w / 2, h / 2, stepScale, stepScale);
}

void rendererFlip() {
    //GPU_Flip(renderer.screen);
	SDL_RenderPresent(renderer.renderer);
}

void rendererDestroy() {
	Resources::cleanup();
	//GPU_FreeImage(renderer.renderTargetImage);
	//SDL_DestroyWindow(window);
	//GPU_Quit();
}

// GPU_Camera *getCamera() {
// 	return &camera;
// }

void renderer_clearCamera() {
	//GPU_SetCamera(renderer.target, NULL);
}

void renderer_resetCamera() {
	//camera = GPU_GetDefaultCamera();
}

void camera_lookat(int x, int y) {
	// camera.x = x - (static_cast<int>(gw) / 2);
	// camera.y = y - (static_cast<int>(gh) / 2);
}

void camera_move(int x, int y) {
	// camera.x += x;
	// camera.y += y;
}
