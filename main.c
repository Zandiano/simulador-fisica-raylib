#include <stdio.h>
#include "raylib.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define WINDOWH 600
#define WINDOWW 600
#define TARGET_FPS 30
#define PARTICLE_QNT 10000
#define PARTICLE_MASS 1
#define PARTICLE_COLOR BLACK
#define RESISTENCIA 0.60f

int Pause = 0;
float randomness = 0.1f;
char buffer[64] = "";
int KEY = 0;
Vector2 dist = {0};
Vector2 dir = {0};
Vector2 massCenter = {0};
int massCenterRendered = 0;
float FORCE = 1.0f;
float gravityForceCalc = 0;
float GRAVITY = 0.1f;
int gravityOn = 1;

struct particle{
    Vector2 pos;
    Vector2 vel;
    Color color;
};

struct particle particleList[PARTICLE_QNT] = {0};

void Init(){
    SetRandomSeed(time(NULL));
    memset(particleList, 0, sizeof(particleList));
    for(int i = 0; i < PARTICLE_QNT; i++){
        particleList[i].pos.x = GetRandomValue(0,WINDOWW);
        particleList[i].pos.y = GetRandomValue(0,WINDOWH);
        particleList[i].color = PARTICLE_COLOR;
    }
}

void PreUpdate(){
    massCenter.x = 0;
    massCenter.y = 0;

    for(int i = 0; i < PARTICLE_QNT; i++){
        massCenter.x += particleList[i].pos.x;
        massCenter.y += particleList[i].pos.y;
    }

    massCenter.x /= PARTICLE_QNT;
    massCenter.y /= PARTICLE_QNT;
    
    KEY = GetKeyPressed();
    switch(KEY){
        case KEY_UP:
            randomness += 0.1f;
            break;
        case KEY_DOWN:
            randomness -= 0.1f;
            break;
        case KEY_LEFT:
            GRAVITY -= 0.1f;
            break;
        case KEY_RIGHT:
            GRAVITY += 0.1f;
            break;
        case KEY_R:
            Init();
            break;
        case KEY_PAGE_UP:
            FORCE += 0.1f;
            break;
        case KEY_PAGE_DOWN:
            FORCE -= 0.1f;
            break;
        case KEY_P:
            gravityOn = !gravityOn;
            break;
        case KEY_SPACE:
            Pause = !Pause;
            break;
        case KEY_Z:
            massCenterRendered = !massCenterRendered;
            break;
    }
}

void Update(){
    
    
    for(int i = 0; i < PARTICLE_QNT; i++){
        particleList[i].vel.x += GetRandomValue(-1,1)*randomness;
        particleList[i].vel.y += GetRandomValue(-1,1)*randomness;

        particleList[i].vel.y += GRAVITY * gravityOn;

        dist.x = massCenter.x - particleList[i].pos.x;
        dist.y = massCenter.y - particleList[i].pos.y;

        if(dist.x){dir.x = dist.x/fabs(dist.x);} else {dir.x = 0;}
        if(dist.y){dir.y = dist.y/fabs(dist.y);} else {dir.y = 0;}

        gravityForceCalc = (PARTICLE_QNT/(pow(dist.x,2)+pow(dist.y,2))) * FORCE;

        particleList[i].vel.x += dir.x * gravityForceCalc;
        particleList[i].vel.y += dir.y * gravityForceCalc;
    }
}

void PosUpdate(){
    float deltaTime = GetFrameTime();
    for(int i = 0; i < PARTICLE_QNT; i++){
        particleList[i].vel.x *= RESISTENCIA;
        particleList[i].vel.y *= RESISTENCIA;
        
        particleList[i].pos.x += particleList[i].vel.x/deltaTime;
        particleList[i].pos.y += particleList[i].vel.y/deltaTime;

        if(particleList[i].pos.x > WINDOWW){particleList[i].pos.x = WINDOWW;}
        else if(particleList[i].pos.x < 0){particleList[i].pos.x = 0;}
        if(particleList[i].pos.y > WINDOWH){particleList[i].pos.y = WINDOWH;}
        else if(particleList[i].pos.y < 0){particleList[i].pos.y = 0;}
    }
}

void Render(){
    
    for(int i = 0; i < PARTICLE_QNT; i++){DrawCircleV(particleList[i].pos, 2, particleList[i].color);}
    
    if(massCenterRendered){DrawCircle(massCenter.x,massCenter.y, 3, RED);}
    
    sprintf(buffer, "PULL: %.2f", FORCE);
    DrawText(buffer, WINDOWW-WINDOWW/4, WINDOWH-WINDOWH/12, 12, RED);

    sprintf(buffer, "GRAVITY: %.2f", GRAVITY);
    DrawText(buffer, WINDOWW-WINDOWW/4, WINDOWH-WINDOWH/16, 12, RED);

    sprintf(buffer, "RANDOMNESS: %.2f", randomness);
    DrawText(buffer, WINDOWW-WINDOWW/4,WINDOWH-WINDOWH/24, 12, RED);

    sprintf(buffer, "%d", GetFPS());
    DrawText(buffer, WINDOWW-WINDOWW/6, WINDOWH/20, 20, RED);
}

int main(){
    
    InitWindow(WINDOWW, WINDOWH, "Physics Sim");

    //INIT
    Init();

    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose())
    {
        //PRE-UPDATE
        PreUpdate();

        if(!Pause){
            //UPDATE
            Update();
            //POS-UPDATE
            PosUpdate();
        }
        //RENDER
        BeginDrawing();
        ClearBackground(RAYWHITE);
        Render();
        EndDrawing();

    }

    CloseWindow();                  // Close window and OpenGL context

    return 0;
}
