#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <vector>
#include <cstdlib>
#include <ctime>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int CELL_SIZE = 20;
const char* window_icon = "assets\\images\\ksot.png";
const char* lose_game = window_icon;
const char* starting_screen = "assets\\images\\kendrick.png";
const char* background_img = "assets\\images\\background.png";
const char* eat_sound_file = "assets\\music\\mixkit-arcade-retro-jump-223.wav";
const char* start_music = "assets\\music\\Not Like Us-[AudioTrimmer.com].mp3";
const char* lose_sound_file = "assets\\music\\mixkit-arcade-space-shooter-dead-notification-272.wav";
const char* background_music_file = "assets\\music\\Future, Metro Boomin, Kendrick Lamar - Like That (Official Audio)(mp3j.cc).mp3.mp3";


enum Direction { UP, DOWN, LEFT, RIGHT };
enum GameState { RUNNING, GAME_OVER };

struct SnakeSegment {
    int x, y;
};

void moveSnake(Direction dir);
void playGameOverSound();
void generateFood();
bool checkCollision(int x, int y);
bool checkSelfCollision();
void handleInput(SDL_Event& event, Direction& dir);
void updateGame();
void render(SDL_Renderer* renderer);
void cleanup();
bool initializeSDL();
void show_start_image(SDL_Renderer* renderer);

std::vector<SnakeSegment> snake;
SDL_Rect food = { 0, 0, CELL_SIZE, CELL_SIZE };
Direction currentDirection = RIGHT;
GameState gameState = RUNNING;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Mix_Chunk* eatSound = nullptr;
Mix_Chunk* loseSound = nullptr;
Mix_Music* backgroundMusic = nullptr;
Mix_Music* startMusic = nullptr;

SDL_Texture* backgroundTexture = nullptr;
SDL_Texture* gameOverTexture = nullptr;

int main(int argc, char** argv) {
    if (!initializeSDL()) {
        cleanup();
        return EXIT_FAILURE;
    }

    show_start_image(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    Mix_PlayMusic(backgroundMusic, -1);

    SDL_Event event;
    while (gameState != GAME_OVER) {
        while (SDL_PollEvent(&event)) 
        {
            handleInput(event, currentDirection);
        }

        updateGame();
        render(renderer);
        SDL_Delay(100); // Adjust the delay as needed
    }

    playGameOverSound();

    Uint32 gameOverStartTime = SDL_GetTicks();
    while (SDL_GetTicks() - gameOverStartTime < 2000) {
        render(renderer);
        SDL_Delay(100); // Adjust the delay as needed
    }

    cleanup();
    return EXIT_SUCCESS;
}

bool initializeSDL() {

    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
    IMG_Init(imgFlags);

    SDL_Surface* icon = IMG_Load(window_icon);
    window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    SDL_SetWindowIcon(window, icon);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    snake.push_back({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });
    generateFood();

    backgroundTexture = IMG_LoadTexture(renderer, background_img);

    gameOverTexture = IMG_LoadTexture(renderer, lose_game);

    eatSound = Mix_LoadWAV(eat_sound_file);
    loseSound = Mix_LoadWAV(lose_sound_file);
    backgroundMusic = Mix_LoadMUS(background_music_file);
    startMusic = Mix_LoadMUS(start_music);

    return true;
}

void moveSnake(Direction dir) {
    int newX = snake[0].x;
    int newY = snake[0].y;

    switch (dir) {
    case UP:
        newY -= CELL_SIZE; break;
    case DOWN:
        newY += CELL_SIZE; break;
    case LEFT:
        newX -= CELL_SIZE; break;
    case RIGHT:
        newX += CELL_SIZE; break;
    }

    if (newX == food.x && newY == food.y) {
        snake.insert(snake.begin(), { newX, newY });
        generateFood();
        Mix_PlayChannel(-1, eatSound, 0);
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

    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    for (const auto& segment : snake) {
        SDL_Rect rect = { segment.x, segment.y, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_RenderFillRect(renderer, &food);

    if (gameState == GAME_OVER) {
        SDL_RenderCopy(renderer, gameOverTexture, nullptr, nullptr);
    }

    SDL_RenderPresent(renderer);
}

void playGameOverSound() {
    Mix_PlayChannelTimed(-1, loseSound, -1, 1000);
}

void cleanup() {
    Mix_FreeChunk(eatSound);
    Mix_FreeChunk(loseSound);
    Mix_FreeMusic(backgroundMusic);
    Mix_FreeMusic(startMusic);

    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(gameOverTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit();
    Mix_CloseAudio();
    SDL_Quit();
}

void show_start_image(SDL_Renderer* renderer) {
    SDL_Surface* start_image = IMG_Load(starting_screen);
    SDL_Texture* startImage = SDL_CreateTextureFromSurface(renderer, start_image);
    SDL_RenderCopy(renderer, startImage, nullptr, nullptr);
    SDL_RenderPresent(renderer);
    Mix_PlayMusic(startMusic, 0);
    SDL_Delay(2500); // Display for 2 seconds
    SDL_DestroyTexture(startImage);
}
