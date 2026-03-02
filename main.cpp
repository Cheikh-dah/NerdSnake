#include "raylib.h"
#include <string>

    void snakeBlock(Rectangle rec)
{
  DrawRectangleRec(rec, YELLOW);
}

   void foodBlock(Rectangle rec)
{
  DrawRectangleRec(rec, RED);
}

/* void gameOver()*- later */

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Hello Raylib");
    int score = 0;
    float speed = 600; // pixels per second
    
    // our "sprites" 
    Rectangle snakeRec = { 370, 300, 30, 30 };
    Rectangle foodRec  = { (float)GetRandomValue(0, 770), (float)GetRandomValue(0, 570), 30, 30 };
    
    // Store original windowed size
    int originalWidth = 800;
    int originalHeight = 600;
       


    
    while (!WindowShouldClose()) {

    int panelWidth = GetScreenWidth() * 0.25f; // 25% of window width
    
    if (panelWidth < 250) panelWidth = 250;   // minimum width
    if (panelWidth > 500) panelWidth = 500;   // maximum width
      
    //Our side ANKI :-)
    Rectangle panel = { GetScreenWidth() - panelWidth, 0, panelWidth, GetScreenHeight() };
      
      /* --- 1. INPUT TAKING (START HERE) --- */
       
        if (IsKeyDown(KEY_W)) snakeRec.y -= speed * GetFrameTime();
        if (IsKeyDown(KEY_A)) snakeRec.x -= speed * GetFrameTime();
        if (IsKeyDown(KEY_D)) snakeRec.x += speed * GetFrameTime();
        if (IsKeyDown(KEY_S)) snakeRec.y += speed * GetFrameTime();
        	
      // Toggle fullscreen with F11
        if (IsKeyPressed(KEY_F11)) {
            if (!IsWindowFullscreen()) {
         // Switch to fullscreen at monitor's native resolution
                int monitor = GetCurrentMonitor();
                SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
                ToggleFullscreen();
            } else {
         // Return to windowed mode with original size
                ToggleFullscreen();
                SetWindowSize(originalWidth, originalHeight);
             }
	 }
	
       /* --- 2. UPDATE / LOGIC ---
       (Collision checks, AI, etc. go here)*/
	float gameWidth = GetScreenWidth() - panelWidth;
	//eating - yammmmmmmmmy ;)                
	//collision
         if (CheckCollisionRecs(snakeRec, foodRec))
             {
             score += 1;
             foodRec.x = (float)GetRandomValue(0, gameWidth - (int)foodRec.width);
             foodRec.y = (float)GetRandomValue(0, GetScreenHeight() - (int)foodRec.height);
             }
	
	// prevent the snake from going under the panel
        if (snakeRec.x < 0) snakeRec.x = 0;
        if (snakeRec.x + snakeRec.width > gameWidth)
               snakeRec.x = gameWidth - snakeRec.width;
	                
        if (snakeRec.y < 0) snakeRec.y = 0;
        if (snakeRec.y + snakeRec.height > GetScreenHeight())
               snakeRec.y = GetScreenHeight() - snakeRec.height;
	
	/* --- 3. Drawing --- get updated every frame --- */
	
        BeginDrawing();
        ClearBackground(BLACK);
	DrawRectangleRec(panel, DARKGRAY);
        DrawText(std::to_string(score).c_str(), 25, 25, 20, RED);
        snakeBlock(snakeRec);
	foodBlock(foodRec);
        EndDrawing();

    }

    CloseWindow();
    return 0;
} //What a lovely numOfLines for a lovely Game!! MADE WITH LOVE BY CHEIKH_DAH <3  
