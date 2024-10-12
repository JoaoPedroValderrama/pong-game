#include "raylib.h"

int playerScore = 0;
int cpuScore = 0;
typedef struct {
    float x, y;
    int speedX, speedY;
    float radius;
    void (*Draw)(struct Ball *); //Tem q colocar struct Ball pq typedef nao foi terminado de ser criado!
    void (*Update)(struct Ball *, float deltaTime);
    void (*Reset)(struct Ball*);

} Ball;

typedef struct {
    float x, y;
    int width, height;
    int speed;
    void (*Draw)(struct Paddle*);
    void (*Update)(struct Paddle*, float deltaTime);
    void (*LimitMovement)(struct Paddle*);
} Paddle;


typedef struct{
    Paddle base;
    void (*Update)(struct CpuPaddle*, struct Ball*, float deltaTime);
} CpuPaddle;


void DrawBall(Ball* ball);

void UpdateBall(Ball* ball, float deltaTime);

void ResetBall(Ball* ball);

void DrawPaddle(Paddle* ball);

void UpdatePaddle(Paddle* paddle, float deltaTime);

void UpdateCpuPaddle(CpuPaddle* cpuPaddle, Ball* ball, float deltaTime);

void LimitMovement(Paddle* paddle);



int main(void)
{
    const int screenWidth = 1000;
    const int screenHeight = 500;

    InitWindow(screenWidth, screenHeight, "raylib [core] example - basic window");
    Ball ball = {screenWidth / 2, screenHeight / 2, 300, 300, 20, DrawBall, UpdateBall, ResetBall};

    Paddle paddle = {10, screenHeight/2 - 120/2, 25, 120, 300, DrawPaddle, UpdatePaddle, LimitMovement};
    CpuPaddle cpuPaddle = {{screenWidth - paddle.width - 10, screenHeight / 2 - paddle.height / 2, 25, 120, 150, DrawPaddle, UpdatePaddle, LimitMovement}, UpdateCpuPaddle};
    SetTargetFPS(60);

    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        //Update
        float deltaTime = GetFrameTime();

        ball.Update(&ball, deltaTime);
        paddle.Update(&paddle, deltaTime);
        cpuPaddle.Update(&cpuPaddle, &ball, deltaTime);

        if(CheckCollisionCircleRec((Vector2){ball.x, ball.y}, ball.radius, (Rectangle){paddle.x, paddle.y, paddle.width, paddle.height})){
           ball.speedX *= -1;
        }

        if(CheckCollisionCircleRec((Vector2){ball.x, ball.y}, ball.radius, (Rectangle){cpuPaddle.base.x, cpuPaddle.base.y, cpuPaddle.base.width, cpuPaddle.base.height})){
            ball.speedX *= -1;
        }

        //Draw
        BeginDrawing();
            ClearBackground(BLACK);
            DrawLine(screenWidth/2, 0, screenWidth/2, screenHeight, WHITE);
            ball.Draw(&ball);
            paddle.Draw(&paddle);
            cpuPaddle.base.Draw(&cpuPaddle.base);

            DrawText(TextFormat("%i", playerScore), screenWidth / 4 - 20, 20, 80, WHITE);
            DrawText(TextFormat("%i", cpuScore), 3*screenWidth / 4 - 20, 20, 80, WHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}







void DrawBall(Ball *ball){
    DrawCircle(ball->x, ball->y, ball->radius, WHITE);
};

void UpdateBall(Ball *ball, float deltaTime){
    ball->x += ball->speedX * deltaTime;
    ball->y += ball->speedY * deltaTime;

    if(ball->y + ball->radius >= GetScreenHeight() || ball->y - ball->radius <= 0){
        ball->speedY *= -1;
    }
    if(ball->x + ball->radius >= GetScreenWidth()){
        ball->speedX *= -1;
        playerScore++;
        ball->Reset(ball);
    }
    if(ball->x - ball->radius <= 0){
        ball->speedX *= -1;
        cpuScore++;
        ball->Reset(ball);
    }


};

void ResetBall(Ball* ball){
    ball->x = GetScreenWidth()/2;
    ball->y = GetScreenHeight()/2;

    int speedChoices[2] = {-1, 1};
    ball->speedX *= speedChoices[GetRandomValue(0, 1)];
    ball->speedY *= speedChoices[GetRandomValue(0, 1)];
}

void DrawPaddle(Paddle* paddle){
    DrawRectangle(paddle->x, paddle->y, paddle->width, paddle->height, WHITE);
}

void UpdatePaddle(Paddle* paddle, float deltaTime){
    if(IsKeyDown(KEY_UP)){
        paddle->y -= paddle->speed * deltaTime;
    }

    if(IsKeyDown(KEY_DOWN)){
        paddle->y += paddle->speed * deltaTime;
    }

    paddle->LimitMovement(paddle);
}

void LimitMovement(Paddle* paddle){
    if(paddle->y <= 0){
        paddle->y = 0;
    }

    if(paddle->y + paddle->height >= GetScreenHeight()){
        paddle->y = GetScreenHeight() - paddle->height;
    }
}

void UpdateCpuPaddle(CpuPaddle* cpuPaddle, Ball* ball, float deltaTime){
    if(cpuPaddle->base.y + cpuPaddle->base.height / 2 > ball->y){
        cpuPaddle->base.y -= cpuPaddle->base.speed * deltaTime;
    }
    if(cpuPaddle->base.y + cpuPaddle->base.height / 2 <= ball->y){
        cpuPaddle->base.y += cpuPaddle->base.speed * deltaTime;
    }

    cpuPaddle->base.LimitMovement(&(cpuPaddle->base));
}
