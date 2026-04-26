#include <memory>
#include <vector>
#include <ncurses.h>
#include <iostream>

int constexpr h = 25;
int constexpr w = 88;

int clamp(int min, int max, int v) {
	if(v < min) return min;
	if(v > max) return max;
	return v;
}

class Wall{
    private:
		static int vx;
        int x;
        int y;
		static int const wallWidth = 3;
		static int const gapY = 7;
		static int const gapX = 5;
    public:
        Wall(int startX, int startY) : x(startX), y(startY) { randomize(); }
        ~Wall() {}

        bool update() {
            x -= 1;
			return true;
        }

        bool draw() {
			// don't draw walls out of scope (only really at the beginning)
			if(x > w || x <= 0) return false;

			move(0,x);
			attron(COLOR_PAIR(3));

			// top part
			for(int i = 0; i < y; i++){
				move(i, x);
				for(int k = 0; k < wallWidth; k++){
					printw(" ");
				}
			}
			// bottom part
			for(int i = y + gapY; i < h; i++){
				move(i, x);
				for(int k = 0; k < wallWidth; k++){
					printw(" ");
				}
			}

			attroff(COLOR_PAIR(3));
			return true;
        }  

		bool outOfBounds() { return x < -2 || x > w; }

        int randomize() {
			int minY = 5;
			y = minY + rand() % (h - 2 * minY);
            return y;
        }

		int getX() { return x; }
		int getY() { return y; }
};

class Bird{
    private:
		static float constexpr ay = 0.6;
		static float constexpr jumpVel = -3;
		float vy;
        int x;
        int y;
    public:
        Bird(int startX, int startY) : x(startX), y(startY) {}
        ~Bird() {}

		bool registerJump() {
			vy = jumpVel;
			return false;
		}

        bool draw() {
			attron(COLOR_PAIR(2));
			move(y, x);
			printw("  ");
			attroff(COLOR_PAIR(2));
			return true;
        }

        bool dead(std::vector<Wall> walls) {
			for(auto& wall : walls){

			}
			return (y <= 0 || y >= h);
        }

        void update() {
			vy += ay;
			y += (int)vy;
			vy = clamp(jumpVel, -jumpVel, vy);
			y = clamp(0, h, y);
        }

		int getX() { return x; }
		int getY() { return y; }
};

class Game {
	private:
		std::unique_ptr<Bird> bird;
		std::vector<Wall> walls;
		int score;
		int tick;
		bool isGameOver;
		static int constexpr numWalls = 10;
		static int constexpr birdStartX = 6;
		static int constexpr birdStartY = h / 2;

		void spawnWall() {
			walls.emplace_back(w, 0);
		}

	public:
		Game() : bird(std::make_unique<Bird>(birdStartX, birdStartY)), score(0), tick(0), isGameOver(false) {
			walls.reserve(numWalls);
		}
		~Game() { walls.clear(); }

		void gameOver() {
			isGameOver = true;
			walls.clear();
			tick = 0;
			score = 0;
		}

		void restart() {
			isGameOver = false;
		}

		int update(char cmd) {

			if(!isGameOver){
				// update bird
				if(cmd == ' ' || cmd == 'j'){
					bird->registerJump();
				}
				bird->update();

				// update walls
				for(int i = 0; i < walls.size(); i++){
					walls.at(i).update();
					// remove walls that are out of bounds
					if(walls.at(i).outOfBounds()){
						std::swap(walls.at(i), walls.back());
						walls.pop_back();
						score++;
					}
				}
				if(tick % 30 == 0){
					spawnWall();
				}

				tick++;
				if(bird->dead(walls)) {
					gameOver();
				}
			} else {
				if(cmd == ' '){ 
					restart(); 
				}
			}

			return tick;
		}

		void display() { 
			move(0,0);

			if(isGameOver){
				move(h/2, w/2);
				printw("YOU DIED");
				move(h/2+2, w/2);
			} else {
				// background
				attron(COLOR_PAIR(1));
				for(int i = 0; i < h; i++){
					for(int j = 0; j < w; j++){
						printw(" ");
					}
					printw("\n");
				}
				attroff(COLOR_PAIR(1));

				// bird
				bird->draw();

				// walls
				for(auto& wall : walls){
					wall.draw();
				}

				// score
				move(h, 0);
				printw("Score: %d", score);
			}
		}

};

int main(int argc, char **argv){
	initscr(); 
	start_color();
	noecho();               // don't print keypresses
	cbreak();               // don't wait for Enter
	nodelay(stdscr, TRUE);

	init_pair(1, COLOR_BLUE,  COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_YELLOW);
	init_pair(3, COLOR_GREEN, COLOR_GREEN);

	auto g = std::make_unique<Game>(); 

	// main loop
	char cmd;
	int t = 0;
	do {
		clear();
		g->display();
		refresh();

		napms(50);

		cmd = getch();
		t = g->update(cmd);

	} while(cmd != 'q');

	endwin();
	return 0;
}