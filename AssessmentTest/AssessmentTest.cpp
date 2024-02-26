// AssessmentTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <fstream>
#include <iostream>

#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_image.h>

using namespace std;//So I don't have to constantly write out std

//The Wild West Spaghetti code of Dragon-kid, but hey, it runs ;-;

Mix_Music* SOUND_aBckMusic = NULL;
Mix_Chunk* SOUND_aShot = NULL;
SDL_Texture* load_texture(const char* filename, SDL_Renderer* renderer) //Function to load in textures
{
    //Load the image surface
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        printf("Failed to load image: %s\n", SDL_GetError());
        return NULL;
    }

    //Creates a texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    // Free the surface and return the texture
    SDL_FreeSurface(surface);
    return texture;
}

int main(int argc, char* argv[]) {
    //Health of enemy and player character
    int enemyHealth = 2;
    int health = 10;

    //Strength of enemy and player character
    int enemyStrength = 1;
    int strength = 1;

    //Players scored
    int score = 0;

    //Score 
    bool scoreMultiplier = false;

    bool featherGet = false;//I made this earlier on and forgot about it

    SDL_Init(SDL_INIT_TIMER);

    //Set up the game loop
    bool quit = false;

    //Timing
    Uint32 previousTicks = SDL_GetTicks();
    Uint32 currentTicks, elapsedTicks, totalTime;

    int leftOverTime = 0;
    bool dracoFeather = false;
    
    SDL_Init(SDL_INIT_AUDIO);
    SDL_Init(SDL_INIT_VIDEO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    //Background Music
    Mix_Music* music = Mix_LoadMUS("DragonsWakeCompress.wav");
    //Sound Effects
    Mix_Chunk* sound = Mix_LoadWAV("HuCrunch.mp3");//Sound effect for collision
    Mix_Chunk* sound2 = Mix_LoadWAV("HyaCronch.wav");//Sound effect for jumps and attacks
    Mix_Chunk* sound3 = Mix_LoadWAV("OwCronch.wav");//Sound effect for damage
    Mix_Chunk* sound4 = Mix_LoadWAV("WahaCronch.wav");//Sound effect for getting an item

    //Plays music on a loop
    bool music_on = true;
    Mix_PlayMusic(music, -1);

    //SDL_Init(SDL_INIT_VIDEO);

    //Creates Window
    SDL_Window* window = SDL_CreateWindow("Zola Tiger Lindner - Games Programming - 25282485 - Dragon-Kid", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    //Load image into an SDL_Surface
    SDL_Surface* png_surfaceEnemy = IMG_Load("DragonHunter.png");
    //Create SDL_Texture from the loaded surface
    SDL_Texture* png_textureEnemy = SDL_CreateTextureFromSurface(renderer, png_surfaceEnemy);

    //Heart
    SDL_Surface* png_surfaceHeart = IMG_Load("Heart.png");
    SDL_Texture* png_textureHeart = SDL_CreateTextureFromSurface(renderer, png_surfaceHeart);

    //Feather
    SDL_Surface* png_surfaceFeather = IMG_Load("Feather.png");
    SDL_Texture* png_textureFeather = SDL_CreateTextureFromSurface(renderer, png_surfaceFeather);

    //Fruit
    SDL_Surface* png_surfaceFruit = IMG_Load("Fruit.png");
    SDL_Texture* png_textureFruit = SDL_CreateTextureFromSurface(renderer, png_surfaceFruit);

    //Key
    SDL_Surface* png_surfaceKey = IMG_Load("Key.png");
    SDL_Texture* png_textureKey = SDL_CreateTextureFromSurface(renderer, png_surfaceKey);

    //Splash screen
    SDL_Surface* png_surfaceSplash = IMG_Load("SplashScreen.png");
    SDL_Texture* png_textureSplash = SDL_CreateTextureFromSurface(renderer, png_surfaceSplash);

    //SDL_Texture* sprite_texture = load_texture("KidRestA.gif", renderer);
    //I had a gif that I couldn't impement

    SDL_Surface* png_surface = IMG_Load("RestA.png");//Load animation
    int frame_count = 4;//Animation has 4 frames
    SDL_Texture* frame_textures[4];

    //This animates the 
    for (int i = 0; i < frame_count; i++)
    {
        SDL_Rect frame_rect = { i * 16, 0, 16, 16 };
        SDL_Surface* frame_surface = SDL_CreateRGBSurface(0, 16, 16, 32, 0, 0, 0, 0);
        SDL_BlitSurface(png_surface, &frame_rect, frame_surface, NULL);
        frame_textures[i] = SDL_CreateTextureFromSurface(renderer, frame_surface);
        SDL_FreeSurface(frame_surface);
    }

    SDL_Surface* png_surfaceR = IMG_Load("RightA.png");//Load animation for the right, although this wasn't implemented in the final version
    frame_count = 4;
    SDL_Texture* frame_texturesR[4];

    for (int i = 0; i < frame_count; i++) {
        SDL_Rect frame_rectR = { i * 16, 0, 16, 16 };
        SDL_Surface* frame_surfaceR = SDL_CreateRGBSurface(0, 16, 16, 32, 0, 0, 0, 0);
        SDL_BlitSurface(png_surface, &frame_rectR, frame_surfaceR, NULL);
        frame_textures[i] = SDL_CreateTextureFromSurface(renderer, frame_surfaceR);
        //SDL_Delay(2000);
        SDL_FreeSurface(frame_surfaceR);
    }

    //Below is my failed attempt at trying to import gifs into this code

    //SDL_Surface* gif_surface = IMG_Load("KidRestA.gif");
    //if (!gif_surface) {
    //    printf("Failed to load GIF: %s\n", IMG_GetError());
    //    // Handle error
    //}
    //printf("Width: %d, Height: %d\n", gif_surface->w, gif_surface->h);


    //printf("gif_surface: %p\n", (void*)gif_surface);

    // Calculate the number of frames in the GIF
    //printf("frame_count: %d\n", frame_count);

    SDL_Rect rectangle = { 10, 780, 32, 32 };//This is the player character, rectangle was supposed to be a temporary team, but then 
    int velocity = 4;

    //I was trying to make my movement more nuanced, but I couldn't get it to work properly
    //int velocity_x = 10;
    //int velocity_y = 0;

    //Gravity that's constantly acting upon the
    const int GRAVITY = 2;
    int gravity2 = 1;//This second gravity is activated upon collecting 

    //All of the below are checks for whether items have been collected, this should have been a list, or included in a class to those respective game objects
    bool collectedh1 = false;
    bool collectedh2 = false;
    bool collectedfr = false;
    bool collectedfe = false;
    bool fallTime = false;

    //All of the in game objects, rendered as rectangles
    SDL_Rect enemy = { 200, 248, 32, 32 };
    SDL_Rect heart = {100, 550, 32, 32};
    SDL_Rect heart2 = { 384, 180, 32, 32 };
    SDL_Rect fruit = { 386, 248, 32, 32};
    SDL_Rect key = { 10, 75, 32, 32};
    SDL_Rect feather = { 746, 444, 32, 32};
    SDL_Rect splash = { 0, 0, 800, 600 };

    //Walls for the game
    SDL_Rect wall_left = { 0, 0, 10, 600 };
    SDL_Rect wall_right = { 790, 0, 10, 600 };
    SDL_Rect wall_top = { 0, 0, 800, 10 };
    SDL_Rect wall_bottom = { 0, 590, 800, 10 };

    //The game world, platforms and the such
    SDL_Rect danger = { 320, 580, 470, 10 };
    SDL_Rect fall = { 440, 480, 290, 10 };
    SDL_Rect platform1 = { 260, 580, 60, 10 };
    SDL_Rect platform2 = { 320, 530, 60, 10 };
    SDL_Rect platform3 = { 380, 480, 60, 10 };
    SDL_Rect platform4 = { 730, 480, 60, 10 };
    SDL_Rect platform5 = { 130, 430, 60, 10 };
    SDL_Rect platform6 = { 10, 380, 60, 10 };
    SDL_Rect platform7 = { 130, 330, 60, 10 };
    SDL_Rect platform8 = { 200, 280, 400, 10 };
    SDL_Rect platform9 = { 380, 220, 40, 10 };
    SDL_Rect platform10 = { 680, 200, 60, 10 };
    SDL_Rect platform11 = { 480, 110, 310, 10 };
    SDL_Rect platform12 = { 10, 110, 300, 10 };



    //Enemy path, the enemy will walk right and left from this path
    int pathW = 400;
    int pathH = 300;
    int pathX = (800 - pathW) / 2;
    int pathY = (600 - pathH) / 2;
    int pathL = pathX;
    int pathR = pathX + pathW - enemy.w;

    int direction = 1;//Initial direction
    int speed = 1;//Trying to fix the speed

    bool running = true;//Whether the game is running or not
    bool windowed = true;//Check if the game is windowed or not

    int current_frame = 0;
    Uint32 last_frame_time = 0;
    const int FRAME_DELAY_MS = 400;
    const int FRAME_DELAY_MSMove = 100;

    //The game itself
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_w) {
                    windowed = !windowed; //Toggle windowed mode on and off
                    if (windowed) {
                        SDL_SetWindowFullscreen(window, 0);
                    }
                    else {
                        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    }
                }
            }
        }

        enemy.x += direction*speed;
        if (enemy.x <= pathL || enemy.x >= pathR)
        {
            direction = -(direction);
            //SDL_Delay(1000);
        }


        // get the state of the arrow keys
        const Uint8* state = SDL_GetKeyboardState(NULL);
        Uint32 current_time = SDL_GetTicks();
        Uint32 elapsed_time = current_time - last_frame_time;
        // update the position of the rectangle based on arrow key input
        
        if (state[SDL_SCANCODE_LEFT]) {
            rectangle.x -= velocity;
        }
        if (state[SDL_SCANCODE_RIGHT]) {
            rectangle.x += velocity;

        }
        if (state[SDL_SCANCODE_UP]) {
            rectangle.y -= velocity+GRAVITY+10;
            Mix_PlayChannel(-1, sound2, 0);
            
        }
        if (state[SDL_SCANCODE_DOWN]) {
            rectangle.y += velocity;
        }
        rectangle.y = rectangle.y += GRAVITY;//Implements Gravity
        

        //velocity_y += GRAVITY;

        //Check collisions with the walls
        if (SDL_HasIntersection(&rectangle, &wall_left)) {
            rectangle.x = (wall_left.x + wall_left.w)+15;//Bounces player back from wall
            Mix_PlayChannel(-1, sound, 0);
        }
        if (SDL_HasIntersection(&rectangle, &wall_right)) {
            rectangle.x = (wall_right.x - rectangle.w)-15;
            Mix_PlayChannel(-1, sound, 0);
        }
        if (SDL_HasIntersection(&rectangle, &wall_top)) {
            rectangle.y = wall_top.y + wall_top.h;
            //Mix_PlayChannel(-1, sound, 0);
        }
        if (SDL_HasIntersection(&rectangle, &wall_bottom)) {
            rectangle.y = wall_bottom.y - (rectangle.h+GRAVITY);
        }

        //Collisions for items
        if (SDL_HasIntersection(&rectangle, &heart)) {
            Mix_PlayChannel(-1, sound4, 0);
            score = score + 500;
            health = health + 1;
            collectedh1 = true;
        }
        if (SDL_HasIntersection(&rectangle, &heart2)) {
            Mix_PlayChannel(-1, sound4, 0);
            score = score + 500;
            health = health + 1;
            collectedh2 = true;
        }
        //This doesn't disapear after being collected, although it should
        if (SDL_HasIntersection(&rectangle, &feather)) {
            Mix_PlayChannel(-1, sound4, 0);
            score = score + 1000;
            featherGet = true;
            collectedfr = true;
        }
        if (SDL_HasIntersection(&rectangle, &fruit)) {
            score = score + 500;
            strength = strength + 1;
            collectedfr = true;
        }
        //The enemy is capable of getting a Strong-strong Fruit, making their attacks more dangerous(Well, collisions)
        if (SDL_HasIntersection(&enemy, &fruit)) {
            enemyStrength = enemyStrength + 1;
            collectedfr = true;
        }
        if (SDL_HasIntersection(&rectangle, &key)) {
            cout << "Level Complete, Well Done!"<< endl;
            score = score + 10000;
            quit = true;
            running = false;//Okay, this will end it
        }
        //Enemy collisions
        if (SDL_HasIntersection(&rectangle, &enemy)) {
            Mix_PlayChannel(-1, sound3, 0);
            health = health - enemyStrength;
        }

        //Collision for level
        if (SDL_HasIntersection(&rectangle, &danger)) {
            rectangle.y = danger.y - rectangle.h;
            Mix_PlayChannel(-1, sound3, 0);
            health = 0;
        }
        if (SDL_HasIntersection(&rectangle, &fall)) {
            rectangle.y = fall.y - rectangle.h;
            fallTime = true;
        }
        if (SDL_HasIntersection(&rectangle, &platform1)) {
            rectangle.y = platform1.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform2)) {
            rectangle.y = platform2.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform3)) {
            rectangle.y = platform3.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform4)) {
            rectangle.y = platform4.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform5)) {
            rectangle.y = platform5.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform6)) {
            rectangle.y = platform6.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform7)) {
            rectangle.y = platform7.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform8)) {
            rectangle.y = platform8.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform9)) {
            rectangle.y = platform9.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform10)) {
            rectangle.y = platform10.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform11)) {
            rectangle.y = platform11.y - rectangle.h;
        }
        if (SDL_HasIntersection(&rectangle, &platform12)) {
            rectangle.y = platform12.y - rectangle.h;
        }

        //Makes sure that the player character stays on the screen
        if (rectangle.x + rectangle.w > 800) {
            rectangle.x = 800 - rectangle.w;
        }
        if (rectangle.x < 0) {
            rectangle.x = 0;
        }
        if (rectangle.y + rectangle.h > 600) {
            rectangle.y = 600 - rectangle.h;
        }
        if (rectangle.y < 0) {
            rectangle.y = 0;
        }
        
        //Counts ticks
        currentTicks = SDL_GetTicks();
        cout<<currentTicks <<endl;
        elapsedTicks = currentTicks - previousTicks;
        previousTicks = currentTicks;
        totalTime = currentTicks/1000;
        cout << "Ticks: "+totalTime << endl;

        // clear the renderer and draw the walls
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(renderer, &wall_left);
        SDL_RenderFillRect(renderer, &wall_right);
        SDL_RenderFillRect(renderer, &wall_top);
        SDL_RenderFillRect(renderer, &wall_bottom);

        //Renders all of the the level
        SDL_SetRenderDrawColor(renderer, 255, 165, 0, 0);
        if (!fallTime)
        {
            SDL_RenderFillRect(renderer, &fall);
        }
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer, &danger);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(renderer, &platform1);
        SDL_RenderFillRect(renderer, &platform2);
        SDL_RenderFillRect(renderer, &platform3);
        SDL_RenderFillRect(renderer, &platform4);
        SDL_RenderFillRect(renderer, &platform5);
        SDL_RenderFillRect(renderer, &platform6);
        SDL_RenderFillRect(renderer, &platform7);
        SDL_RenderFillRect(renderer, &platform8);
        SDL_RenderFillRect(renderer, &platform9);
        SDL_RenderFillRect(renderer, &platform10);
        SDL_RenderFillRect(renderer, &platform11);
        SDL_RenderFillRect(renderer, &platform12);

        //Bellow renders all of the sprites, the enemy, player, items
        //Objects will only be rendered if they haven't collided with the player
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        //SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, png_textureEnemy, NULL, &enemy);
        if (!collectedh1) 
        {
            SDL_RenderCopy(renderer, png_textureHeart, NULL, &heart);
        }
        if (!collectedh2)
        {
            SDL_RenderCopy(renderer, png_textureHeart, NULL, &heart2);
        }
        if (!collectedfe)
        {
            SDL_RenderCopy(renderer, png_textureFeather, NULL, &feather);
        }
        if (totalTime >= 15)// && (!collectedfr)
        {
            SDL_RenderCopy(renderer, png_textureFruit, NULL, &fruit);
        }
        //SDL_RenderCopy(renderer, png_textureFruit, NULL, &fruit);
        SDL_RenderCopy(renderer, png_textureKey, NULL, &key);
        if (totalTime <= 5)// && (!collectedfr)
        {
            SDL_RenderCopy(renderer, png_textureSplash, NULL, &splash);
        }
        //SDL_RenderCopy(renderer, png_textureSplash, NULL, &splash);
        SDL_RenderCopy(renderer, frame_textures[current_frame], NULL, &rectangle);//Renders player character
        SDL_RenderPresent(renderer);

        if (elapsed_time >= FRAME_DELAY_MS)//Handles frames, keeps everything at approximately 30 fps
        {
            last_frame_time = current_time;

            SDL_RenderClear(renderer);
            SDL_RenderCopy(renderer, frame_textures[current_frame], NULL, &rectangle);
            SDL_RenderPresent(renderer);

            current_frame = (current_frame + 1) % frame_count;
        }
        cout << "Elapsed time: " << totalTime << " seconds" << endl;//Logs time
        leftOverTime = 45 - totalTime;
        cout << "Leftover time: " << leftOverTime << " seconds" << endl;//Shows leftover time
        //int timeScore = leftOverTime*10;
        cout << "Total Score: " << score << endl;//Logs score


        if (health == 0)//If the player dies, the game ends
        {
            cout << "Player Health Reached 0: Game Over" << endl;//Logs that player character died
            running = false;
        }
        // Check if the total time has exceeded 45 seconds, if it has, the game ends
        if (totalTime >= 45)
        {
            quit = true;
        }
        if (elapsedTicks < 33)//Sets up a frame rate of approximately 30fps
        {
            SDL_Delay(33 - elapsedTicks);
        }
        //cout<<(elapsedTicks)<<endl;
    }

    //Just freeing everything below
    SDL_FreeSurface(png_surfaceEnemy);
    SDL_DestroyTexture(png_textureEnemy);
    Mix_FreeChunk(sound);

    Mix_FreeMusic(music);
    Mix_CloseAudio();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    //Well, time and score is viewable from the console, just not at the end
    //cout<<"Time Left: " << endl;//These don't work, why???
    //cout<<"Total Score: "+score << endl;
    return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
