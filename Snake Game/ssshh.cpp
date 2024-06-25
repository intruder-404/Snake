#include <iostream>
#include <SDL.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <SDL_image.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;
const char* window_icon = "assets/OIP.jpg";
const char* lose_game = "assets/Untitled.bmp";

enum Direction { UP, DOWN, LEFT, RIGHT };
enum GameState { RUNNING, GAME_OVER };

struct SnakeSegment {
    int x, y;
};

void moveSnake(Direction dir);
void generateFood();
bool checkCollision(int x, int y);
bool checkSelfCollision();
void handleInput(SDL_Event& event, Direction& dir);
void updateGame();
void render(SDL_Renderer* renderer);
void cleanup();
bool initialize();

std::vector<SnakeSegment> snake;
SDL_Rect food = { 0, 0, CELL_SIZE, CELL_SIZE };
Direction currentDirection = RIGHT;
GameState gameState = RUNNING;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

SDL_Surface* lost = nullptr;

int main(int argc, char** argv) {
    
    if (!initialize()) {
        cleanup();
        return EXIT_FAILURE;
    }

    SDL_Event event;

    while (gameState != GAME_OVER) {
        while (SDL_PollEvent(&event)) {
            handleInput(event, currentDirection);
        }

        updateGame();
        render(renderer);
        SDL_Delay(100); // Adjust the delay as needed
    }

    Uint32 gameOverStartTime = SDL_GetTicks();
    while (SDL_GetTicks() - gameOverStartTime < 2000) {
        render(renderer);
        SDL_Delay(100); // Adjust the delay as needed
    }

    cleanup();
    return EXIT_SUCCESS;
}

bool initialize() {
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    SDL_Surface* icon = IMG_Load(window_icon);

    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetWindowIcon(window, icon);
    std::srand(std::time(nullptr));

    snake.push_back({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });
    generateFood();

    lost = SDL_LoadBMP(lose_game);

    return true;
}

void moveSnake(Direction dir) {
    int newX = snake[0].x;
    int newY = snake[0].y;

    switch (dir) {
    case UP:
        newY -= CELL_SIZE; break;
    case DOWN:
        newY += CELL_SIZE;
        break;
    case LEFT:
        newX -= CELL_SIZE; break;
    case RIGHT:
        newX += CELL_SIZE; break;
    }

    if (newX == food.x && newY == food.y) {
        snake.insert(snake.begin(), { newX, newY });
        generateFood();
    }
    else {
        snake.pop_back();
        snake.insert(snake.begin(), { newX, newY });
    }

    if (newX < 0 || newX >= SCREEN_WIDTH || newY < 0 || newY >= SCREEN_HEIGHT || checkSelfCollision()) {
        gameState = GAME_OVER;
    }
}

void generateFood() {
    bool overlap = true;
    while (overlap) {
        food.x = std::rand() % (SCREEN_WIDTH / CELL_SIZE) * CELL_SIZE;
        food.y = std::rand() % (SCREEN_HEIGHT / CELL_SIZE) * CELL_SIZE;
        overlap = checkCollision(food.x, food.y);
    }
}

bool checkCollision(int x, int y) {
    for (const auto& segment : snake) {
        if (segment.x == x && segment.y == y) {
            gameState = GAME_OVER;
            return true;
        }
    }
    return false;
}

bool checkSelfCollision() {
    for (size_t i = 1; i < snake.size(); ++i) {
        if (snake[i].x == snake[0].x && snake[i].y == snake[0].y) {
            gameState = GAME_OVER;
            return true;
        }
    }
    return false;
}

void handleInput(SDL_Event& event, Direction& dir) {
    if (event.type == SDL_QUIT) {
        gameState = GAME_OVER;
    }
    else if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_UP:
            if (dir != DOWN) dir = UP; 
            break;
        case SDLK_DOWN:
            if (dir != UP) dir = DOWN; 
            break;
        case SDLK_LEFT:
            if (dir != RIGHT) dir = LEFT; 
            break;
        case SDLK_RIGHT:
            if (dir != LEFT) dir = RIGHT; 
            break;
        }
    }
}

void updateGame() {
    moveSnake(currentDirection);
}

void render(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (const auto& segment : snake) {
        SDL_Rect rect = { segment.x, segment.y, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
    SDL_RenderFillRect(renderer, &food);

    if (gameState == GAME_OVER) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, lost);
        SDL_RenderCopy(renderer, texture, 0, 0);
        SDL_DestroyTexture(texture);
    }

    SDL_RenderPresent(renderer);
}

void cleanup() {
    SDL_FreeSurface(lost);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
