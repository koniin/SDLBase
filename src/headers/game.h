#ifndef GAME_H
#define GAME_H

#include "engine.h"
#include "renderer.h"

namespace Lights {
	enum BlendMode {
		Multiply,
		Add
	};

	struct Light {
		int x;
		int y;
		std::string sprite;
	};

	struct LightLayer {
		//GPU_Image *renderTarget;
		SDL_Texture *renderTarget;
		SDL_Color ambient;
		std::vector<std::shared_ptr<Light>> lights;
		~LightLayer() {
			
			//GPU_FreeTarget(renderTarget->target);
			//GPU_FreeImage(renderTarget);
		};
	};

	std::vector<std::shared_ptr<LightLayer>> light_layers;

	void add_light_layer(SDL_Color ambient_color, BlendMode blendMode) {
		SDL_Texture *renderTarget = SDL_CreateTexture(renderer.renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, gw, gh);

		// GPU_Image *renderTarget = GPU_CreateImage(gw, gh, GPU_FORMAT_RGBA);
		// GPU_LoadTarget(renderTarget);

		SDL_SetTextureBlendMode(renderTarget, SDL_BlendMode::SDL_BLENDMODE_MOD);
		// if(blendMode == BlendMode::Multiply)
		// 	GPU_SetBlendMode(renderTarget, GPU_BLEND_MULTIPLY);
		// else if(blendMode == BlendMode::Add)
		// 	GPU_SetBlendMode(renderTarget, GPU_BLEND_ADD);
		// else 
		// 	assert(false && "No blendmode conversion exits");

		std::shared_ptr<LightLayer> layer(new LightLayer { renderTarget, ambient_color });
		light_layers.push_back(layer);
	}

	void remove_light_layer(int layer) {
		//if(light_layers.size() > layer)
		//	light_layers.erase(light_layers.begin() + layer);
	}

	void add_light(std::string sprite, int x, int y, int layer = 0) {
		std::shared_ptr<Light> l(new Light { x, y, sprite });
		light_layers[layer]->lights.push_back(l);
	}

	void render_lights() {
		for(auto &layer : light_layers) {
			
			SDL_SetRenderTarget(renderer.renderer, layer->renderTarget);
			SDL_SetRenderDrawColor(renderer.renderer, layer->ambient.r, layer->ambient.g, layer->ambient.b, layer->ambient.a);
			SDL_RenderClear(renderer.renderer);

			for(auto &light : layer->lights) {
				auto t = Resources::getSprite(light->sprite)->image;
				//SDL_SetTextureAlphaMod(t, 255); 
				SDL_SetTextureBlendMode(t, SDL_BlendMode::SDL_BLENDMODE_ADD);
				drawSprite(Resources::getSprite(light->sprite), light->x, light->y);
				//SDL_RenderCopy(renderer.renderer, ->image, NULL, NULL);
			}
			//Detach the texture
			SDL_SetRenderTarget(renderer.renderer, NULL);
			SDL_Rect r;
			r.w = gw;
			r.h = gh;
			r.x = gw / 2;
			r.y = gh / 2;
			SDL_RenderCopy(renderer.renderer, layer->renderTarget, NULL, NULL);

			// GPU_ClearColor(layer->renderTarget->target, layer->ambient);	
			
			// 	drawSpriteToTarget(Resources::getSprite(light->sprite), NULL, light->x, light->y, layer->renderTarget->target);
			
			//GPU_Blit(layer->renderTarget, NULL, renderer.target, gw / 2, gh / 2);
		}
	}

	void cleanup() {
		light_layers.clear();
	}
}

SDL_Color ambientColor = { 73, 96, 79, 255 };
float my_color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

static int number_of_sprites_to_render = 100;
static bool hitEffect = false;
static bool render_lights = true;

inline void game_load() {
	Resources::loadSprite("light", "light.png");
	Resources::loadSprite("rocks", "stones.png");
	Resources::loadSprite("knight", "knight.png");
	
	Lights::add_light_layer(ambientColor, Lights::BlendMode::Multiply);
	Lights::add_light("light", gw / 2, gh / 2);
	Lights::add_light("light", 100, 100);
	Lights::add_light("light", gw - 100, gh - 100);

	// Lights::add_light_layer(ambientColor, Lights::BlendMode::Add);
	// Lights::add_light("light", 200, 200, 1);
}

inline void game_update() {
	if(Input::keyPressed(SDLK_r)) {
		Lights::remove_light_layer(1);
	}
}

inline void game_render() {
	rendererClear();
	
	drawSprite(Resources::getSprite("rocks"), gw / 2, gh / 2);

	// NOTE: EITHER MODIFY THE TEXTURE AND RENDER IT OR CREATE NEW TEXTURES (?)

	// For hit effect set the color mod to 255 and render the sprite
	if(hitEffect) {
		SDL_SetTextureColorMod(Resources::getSprite("knight")->image, 255, 0, 0);
	}
	drawSprite(Resources::getSprite("knight"), gw / 2, gh / 2);
	// After the sprite is rendered we reset the color mod to default values so other renders of that 
	// sprite is normal 
	SDL_SetTextureColorMod(Resources::getSprite("knight")->image, 255, 255, 255);

	// This is how you fade something
	SDL_SetTextureAlphaMod(Resources::getSprite("knight")->image, 100);
	drawSprite(Resources::getSprite("knight"), gw / 2 + 100, gh / 2);
	SDL_SetTextureAlphaMod(Resources::getSprite("knight")->image, 255);

	// for(int i = 0; i < number_of_sprites_to_render; i++) {
	// 	int x, y;
	// 	RNG::randomPoint(gw, gh, x, y);
	// 	drawSprite(Resources::getSprite("knight"), x, y);	
	// }

	if(render_lights)
		Lights::render_lights();
	
	rendererDrawRenderTargetToScreen();
	rendererFlip();
}

inline void game_unload() {
	Lights::cleanup();
}

#endif