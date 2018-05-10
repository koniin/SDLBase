#ifndef ENGINE_H
#define ENGINE_H

#include "precompiled.h"
#include "SDL.h"

namespace Input {
    extern int mousex;
	extern int mousey;

    void init();
    void updateStates();
    void map(const SDL_Event *event);
    bool keyDown(const SDL_Keycode &keyCode);
    bool keyReleased(const SDL_Keycode &keyCode);
    bool keyPressed(const SDL_Keycode &keyCode);
}

struct Point {
	int x;
	int y;
	Point() { x = 0; y = 0; };
	Point(int xPos, int yPos) {
		x = xPos;
		y = yPos;
	}
};
inline Point operator+( Point const& lhs, Point const& rhs ) {
	return Point(lhs.x + rhs.x, lhs.y + rhs.y);
}
inline bool operator==( Point const& lhs, Point const& rhs ) {
	return lhs.x == rhs.x && lhs.y == rhs.y;
}
namespace Engine {
	struct Rectangle {
		int x;
		int y;
		int w;
		int h;
		Rectangle() {
			x = y = w = h = 0;
		}
		Rectangle(int xx, int yy, int ww, int hh) {
			x = xx;
			y = yy;
			w = ww;
			h = hh;
		}
		bool contains(int xi, int yi) {
			return ((((x <= xi) && (xi < (x + w))) && (y <= yi)) && (yi < (y + h)));
		}
		bool contains(Point &p) {
			return contains(p.x, p.y);
		}
	};
	
	static bool is_running = true;
	inline void Exit() {
		is_running = false;
	}
	inline bool IsRunning() {
		return is_running;
	}
}

struct Scene {
	virtual void init() = 0;
  	virtual void cleanup() = 0;
	virtual void update(double dt) = 0;
	virtual void render() = 0;
};

namespace Scenes {
	void setScene(Scene* scene);
	Scene* getCurrentScene();
	void free();
	void update(double dt);
	void render();
}

//std::random_device seed;
//std::mt19937 rng(seed());
//std::default_random_engine rng;

namespace RNG {
	static std::random_device RNG_seed;
	static std::mt19937 RNG_generator(RNG_seed());
	
    // Returns int between 0 and max (exclusive)
    inline int nextInt(int max) {
        std::uniform_int_distribution<int> range(0, max - 1);
        return range(RNG_generator);
    }

    // Range is exclusive
    inline int range(int min, int max) {
        std::uniform_int_distribution<int> range(min, max - 1);
        return range(RNG_generator);
    }

	// Generates two values between 0 and max (exclusive)
	inline void randomPoint(int xMax, int yMax, int &xOut, int &yOut) {
		std::uniform_int_distribution<int> xgen(0, xMax - 1);
		std::uniform_int_distribution<int> ygen(0, yMax - 1);
		xOut = xgen(RNG_generator);
		yOut = ygen(RNG_generator);
	}
}

namespace Localization {
    struct Text {
        Text(const char *file);
        char *getText(std::string s);
        std::map<std::string, char *> texts;
    };

    void loadTexts(const char *file);
    char *text_lookup(std::string s);
}

typedef struct {
    unsigned id;
    std::shared_ptr<void> data;
} GameEvent;

typedef struct {
    int listener_count;
    void (*listener_list[255])(unsigned, std::shared_ptr<void>);
    int queue_count;
    GameEvent queue[255];
} EventQueue;

void eventqueue_Init(EventQueue *e);
void eventqueue_RemoveListener(EventQueue *e, void (*fp)(unsigned, std::shared_ptr<void>));
void eventqueue_AddListener(EventQueue *e, void (*fp)(unsigned, std::shared_ptr<void>));
void eventqueue_TriggerEvent(EventQueue *e, unsigned eventId, std::shared_ptr<void> data);
void eventqueue_QueueEvent(EventQueue *e, unsigned eventId, std::shared_ptr<void> data);
void eventqueue_PumpQueuedEvents(EventQueue *e);

namespace GlobalEvents {
    void initGlobalEvents();
    void updateGlobalEvents();
    EventQueue *getGlobalEventQueue();
}

namespace Serialization {	
	void write_string(std::ofstream &out, const std::string &s);
	void read_string(std::ifstream &in, std::string &s);
}
#endif