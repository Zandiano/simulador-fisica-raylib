#include <stdio.h>
#include "raylib.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define WINDOWH 600
#define WINDOWW 600
#define TARGET_FPS 15
#define PARTICLE_QNT 10000
#define PARTICLE_MASS 1
#define PARTICLE_COLOR BLACK
#define DAMPING 0.55f
#define PITAGORAS(x,y) sqrt(x*x+y*y)

int Pause = 0;
char buffer[64] = "";
int KEY = 0;
Vector2 dist = {0};
Vector2 dir = {0};
Vector2 massCenter = {0};
int massCenterRendered = 0;
float randomness = 0.0f;
float FORCE = 0.0f;
float pullForce = 0;
float GRAVITY = 0.0f;
int gravityOn = 1;
float dragCoefficient = 0;
float drag = 0;
float velMag = 0.0f;
float distMag = 0.0f;
float ammount = 0.1f;
float deltaTime = 0.0f;

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

    deltaTime = fmax(GetFrameTime(), 0.01);

    for(int i = 0; i < PARTICLE_QNT; i++){
        massCenter.x += particleList[i].pos.x;
        massCenter.y += particleList[i].pos.y;
    }

    massCenter.x /= PARTICLE_QNT;
    massCenter.y /= PARTICLE_QNT;
    
    KEY = GetKeyPressed();
    switch(KEY){
        case KEY_UP:
            randomness += ammount;
            break;
        case KEY_DOWN:
            randomness -= ammount;
            break;
        case KEY_LEFT:
            GRAVITY -= ammount;
            break;
        case KEY_RIGHT:
            GRAVITY += ammount;
            break;
        case KEY_R:
            Init();
            break;
        case KEY_PAGE_UP:
            FORCE += ammount;
            break;
        case KEY_PAGE_DOWN:
            FORCE -= ammount;
            break;
        case KEY_M:
            dragCoefficient += ammount;
            break;
        case KEY_N:
            dragCoefficient -= ammount;
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
        case KEY_K:
            ammount *= 10;
            break;
        case KEY_J:
            ammount /= 10;
            break;
    }
    dragCoefficient = fabs(dragCoefficient);
}

void Update(){
    
    
    for(int i = 0; i < PARTICLE_QNT; i++){
        particleList[i].vel.x += GetRandomValue(-1,1)*randomness;
        particleList[i].vel.y += GetRandomValue(-1,1)*randomness;

        particleList[i].vel.y += GRAVITY * gravityOn;

        dist.x = massCenter.x - particleList[i].pos.x;
        dist.y = massCenter.y - particleList[i].pos.y;

        distMag = PITAGORAS(dist.x, dist.y);

        if(distMag > 0.01f){
            dir.x = dist.x / distMag;
            dir.y = dist.y / distMag;
        }

        pullForce = (PARTICLE_QNT/fmax(pow(dist.x,2)+pow(dist.y,2), 25.0f)) * FORCE;

        velMag = PITAGORAS(particleList[i].vel.x, particleList[i].vel.y);

        if(velMag > 0.01f){
            drag = velMag*velMag*(dragCoefficient/100);
    
            particleList[i].vel.x -= (particleList[i].vel.x / velMag) * drag * deltaTime;
            particleList[i].vel.y -= (particleList[i].vel.y / velMag) * drag * deltaTime;
        }

        particleList[i].vel.x += dir.x * pullForce * deltaTime;
        particleList[i].vel.y += dir.y * pullForce * deltaTime;
    }
}

void PosUpdate(){
    
    for(int i = 0; i < PARTICLE_QNT; i++){
        particleList[i].pos.x += particleList[i].vel.x*deltaTime;
        particleList[i].pos.y += particleList[i].vel.y*deltaTime;

        if(particleList[i].pos.x > WINDOWW){particleList[i].pos.x = WINDOWW; particleList[i].vel.x *= -DAMPING;}
        else if(particleList[i].pos.x < 0){particleList[i].pos.x = 0; particleList[i].vel.x *= -DAMPING;}
        if(particleList[i].pos.y > WINDOWH){particleList[i].pos.y = WINDOWH; particleList[i].vel.y *= -DAMPING;}
        else if(particleList[i].pos.y < 0){particleList[i].pos.y = 0; particleList[i].vel.y *= -DAMPING;}
    }
}

void Render(){
    
    for(int i = 0; i < PARTICLE_QNT; i++){DrawCircleV(particleList[i].pos, 2, particleList[i].color);}
    
    if(massCenterRendered){DrawCircle(massCenter.x,massCenter.y, 3, RED);}
    
    sprintf(buffer, "DRAG C: %.2f", dragCoefficient);
    DrawText(buffer, WINDOWW-WINDOWW/4, WINDOWH-62, 12, RED);
    
    sprintf(buffer, "PULL: %.2f", FORCE);
    DrawText(buffer, WINDOWW-WINDOWW/4, WINDOWH-50, 12, RED);

    sprintf(buffer, "GRAVITY: %.2f", GRAVITY);
    DrawText(buffer, WINDOWW-WINDOWW/4, WINDOWH-38, 12, RED);

    sprintf(buffer, "RANDOMNESS: %.2f", randomness);
    DrawText(buffer, WINDOWW-WINDOWW/4,WINDOWH-26, 12, RED);

    sprintf(buffer, "AMMOUNT: %.1f", ammount);
    DrawText(buffer, WINDOWW-WINDOWW/4,WINDOWH-12, 12, RED);

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
