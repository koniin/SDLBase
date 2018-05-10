#include "engine.h"

namespace Input {
    // class KeyboardListener {
    //     public:
    //         void OnKeyEvent(const KeyEvent& evt) {}
    // };

    // class Input {
    //     public:
    //     void AddListener(KeyboardListener* pListener) {
    //         /* add pointer to a list */
    //     }
    // };

	int mousex = 0;
	int mousey = 0;
    SDL_GameController *controller;
    const Uint8* current_keyboard_state;
    // Uint8* current_gamepad_button_state;
    // Uint8* previous_gamepad_button_state;
    // Sint16* gamepad_axis_state;
    // Uint8* current_mouse_state;
    // Uint8* previous_mouse_state;

    std::unordered_set<int> keysDown;
    std::unordered_set<int> keysUp;

    void init() {
        controller = NULL;
        for (int i = 0; i < SDL_NumJoysticks(); ++i) {
            if (SDL_IsGameController(i)) {
                controller = SDL_GameControllerOpen(i);
                if (controller) break;
            }
        }
    }
 
    void updateStates() {
        current_keyboard_state = SDL_GetKeyboardState(NULL);
        keysDown.clear();
        keysUp.clear();
		SDL_GetMouseState(&mousex, &mousey);
    }

    void map(const SDL_Event *event) {
        if (event->type == SDL_KEYDOWN) {
		    keysDown.insert(event->key.keysym.sym);
		}
        if (event->type == SDL_KEYUP) {
		    keysUp.insert(event->key.keysym.sym);
		}
    }

    bool keyDown(const SDL_Keycode &keyCode) {
        return current_keyboard_state[keyCode];
    }

    bool keyReleased(const SDL_Keycode &keyCode) {
        return keysUp.count(keyCode) > 0;
    }

    bool keyPressed(const SDL_Keycode &keyCode) {
        return keysDown.count(keyCode) > 0;
    }
}

// bool isAliveEntity(Entity e) {
	// EntityManager *entityManager = &currentScene->entityManager;
	// int c = entityManager->entities.count(e.id);
	// return c > 0;
// }

// void destroyEntity(Entity e) {
	// EntityManager *entityManager = &currentScene->entityManager;
	// entityManager->entities.erase(e.id);
// }

// Entity createEntity() {
	// EntityManager *entityManager = &currentScene->entityManager;
	// ++entityManager->next.id;
	// while(isAliveEntity(entityManager->next))
		// ++entityManager->next.id;
	// entityManager->entities.insert(entityManager->next.id);
	// return entityManager->next;
// }

namespace Scenes {
	static Scene* currentScene;

	void setScene(Scene* scene) {
		if(currentScene != nullptr)
			currentScene->cleanup();
		delete currentScene;
		
		scene->init();
		currentScene = scene;
	}

	Scene* getCurrentScene() {
		assert(currentScene != nullptr && "current scene is null");
		return currentScene;
	}

	void free() {
		if(currentScene != nullptr) {
			currentScene->cleanup();
			delete currentScene;
		}
	}

	void update(double dt) {
		assert(currentScene != nullptr && "current scene is null");
		currentScene->update(dt);
	}

	void render() {
		assert(currentScene != nullptr && "current scene is null");
		currentScene->render();
	}
}

namespace Localization {
	static Text *localization_text;

	Text::Text(const char *file) {
		//texts.insert(std::pair<std::string, char *>("test", "test result"));
		texts.insert(std::make_pair(std::string("test"), (char*)"test result"));
		printf("actually not loading anything from file");
	}

	char *Text::getText(std::string s) {
		if(texts.count(s) <= 0)
			return NULL;
		
		return texts[s];
	}

	void loadTexts(const char *file) {
		localization_text = new Text(file);
	}

	char *text_lookup(std::string s) {
		return localization_text->getText(s);
	}
}

void eventqueue_Init(EventQueue *e) {
    e->listener_count = 0;
    e->queue_count = 0;
}

void eventqueue_RemoveListener(EventQueue *e, void (*fp)(unsigned, std::shared_ptr<void>)) {
    for(unsigned i = 0; i < e->listener_count; i++) {
        if(e->listener_list[i] == fp) {
            e->listener_count--;
            e->listener_list[i] = e->listener_list[e->listener_count];
        }
    }
}

void eventqueue_AddListener(EventQueue *e, void (*fp)(unsigned, std::shared_ptr<void>)) {
    eventqueue_RemoveListener(e, fp);
    e->listener_list[e->listener_count++] = fp;
}

void eventqueue_TriggerEvent(EventQueue *e, unsigned eventId, std::shared_ptr<void> data) {
    for(unsigned i = 0; i < e->listener_count; i++) {
        e->listener_list[i](eventId, data);
    }
}

void eventqueue_QueueEvent(EventQueue *e, unsigned eventId, std::shared_ptr<void> data) {
    GameEvent ge;
    ge.id = eventId;
    ge.data = data;
    e->queue[e->queue_count++] = ge;
}

void eventqueue_PumpQueuedEvents(EventQueue *e) {
    for(unsigned i = 0; i < e->queue_count; i++) {
        auto ge = &e->queue[i];
        eventqueue_TriggerEvent(e, ge->id, ge->data);
        ge->id = -1;
        ge->data = nullptr;
    }
    e->queue_count = 0;
}

namespace GlobalEvents {
    static EventQueue *globalEvents;

    void initGlobalEvents() {
        globalEvents = new EventQueue;
        eventqueue_Init(globalEvents);
    }

    void updateGlobalEvents() {
        eventqueue_PumpQueuedEvents(globalEvents);
    }

    EventQueue *getGlobalEventQueue() {
        return globalEvents;
    }
}

namespace Serialization {	
	void write_string(std::ofstream &out, const std::string &s) {
    	int size = s.size();
    	out.write(reinterpret_cast<char *>(&size), sizeof(int));
    	out.write(s.c_str(), size);
	}

	void read_string(std::ifstream &in, std::string &s) {
		int size;
		in.read(reinterpret_cast<char *>(&size), sizeof(int));
		char *buf = new char[size];
		in.read(buf, size);
		s.append(buf, size);
		delete buf;
	}
}