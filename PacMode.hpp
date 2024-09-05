#include "PPU466.hpp"
#include "Mode.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <deque>

struct PacMode : Mode {
	PacMode();
	virtual ~PacMode();

	//functions called by main loop:
	virtual bool handle_event(SDL_Event const &, glm::uvec2 const &window_size) override;
	virtual void update(float elapsed) override;
	virtual void draw(glm::uvec2 const &drawable_size) override;

	// from https://15466.courses.cs.cmu.edu/lesson/timing
	void tick();
	static constexpr float Tick = 0.5f; // timestep for tick()
	float tick_acc = 0.0f;

	//----- game state -----

	int palletcount;
	// 0: not win or lose, 1: win, 2: lose
	int iswinlose;

	bool has_wall(glm::uvec2 pos, uint8_t dir);

	//input tracking:
	// up = 0, right = 1, down = 2, left = 3, none = 4
	uint8_t button_pressed = 0;

	//some weird background animation:
	float background_fade = 0.0f;

	//player position:
	glm::uvec2 player_at = glm::uvec2(0, 0);
	//glm::vec2 player_at = glm::vec2(0.0f);

	//player direction:
	// 0: up, 1: right, 2: down, 3: left
	uint8_t player_dir = 0;

	struct Grid {
		// 0: empty, 1: green, 2: yellow, 3: red
		uint8_t pallet;
		bool is_wall;
	};

	struct Board {
		std::vector<std::vector<Grid>> board;
		glm::uvec2 size = glm::uvec2(0, 0);

		Board(std::string const &path);
	};

	Board *gb = nullptr;


	//----- drawing handled by PPU466 -----
	
	PPU466 ppu;
};
