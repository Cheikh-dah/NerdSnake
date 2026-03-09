#include "raylib.h"
#include <string>
#include <vector>
#include <cmath>
#include "terms.h"


void snakeBlock(Rectangle rec)
{
  DrawRectangleRec(rec, YELLOW);
}

void foodBlock(Rectangle rec)
{
  DrawRectangleRec(rec, RED);
}

void GenerateQuiz(const std::vector<Term>& terms,
                  int& correctIndex,
                  int optionIndices[3],
                  Rectangle optionRects[3],
                  float gameWidth,
                  float screenHeight)
{
  correctIndex = GetRandomValue(0, terms.size() - 1);
  optionIndices[0] = correctIndex;
  
  // Generate 2 different wrong answers
  for (int i = 1; i < 3; i++)
    {
      int r;
      do {
	r = GetRandomValue(0, terms.size() - 1);
      } while (r == correctIndex ||
	       r == optionIndices[0] ||
	       r == optionIndices[1]);
      optionIndices[i] = r;
    }
  
  // Position them randomly
  for (int i = 0; i < 3; i++){
    optionRects[i].x = (float)GetRandomValue(0,(int)(gameWidth - 120));
    optionRects[i].y = (float)GetRandomValue(0, (int)(screenHeight - 150));
    optionRects[i].width = 120.0f;
    optionRects[i].height = 40.0f; 
  }
}


enum GameState {
    MENU,
    GAMEPLAY
};

int main() {
  GameState state = MENU;
  std::vector<Term> terms = GetTerms();
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "Hello Raylib");
  int score = 0;
  int lives = 3;
  float dt = GetFrameTime();

  int correctIndex = 0;          // index in terms
  int optionIndices[3];          // 3 word choices
  Rectangle optionRects[3];      // rectangles for each word
  
  int currentTermIndex = GetRandomValue(0, terms.size() - 1);
  std::string eatenWord = "";
  std::string eatenDefinition = "";
  // our "sprites" 
  Rectangle snakeRec = { 370, 300, 30, 30 };
  float angle = 0.0f;
  //float speed = 250.0f;
  float speed = 250.0f;
  //float baseSpeed = 300.0f;
  //float speed;
  Vector2 target = { snakeRec.x, snakeRec.y };
  Vector2 lastMouse = { -1, -1 };
  float threshold = 5.0f;
  Rectangle foodRec  = { (float)GetRandomValue(0, 600), (float)GetRandomValue(0, 500), 100, 40};
  
  // Store original windowed size
  int originalWidth = 800;
  int originalHeight = 600;
  
  GenerateQuiz(terms, correctIndex, optionIndices, optionRects, 800 - 250, 600);


 
  while (!WindowShouldClose()) {

    float dt = GetFrameTime();
    
    int panelWidth = GetScreenWidth() * 0.25f; // 25% of window width
    
    if (panelWidth < 250) panelWidth = 250;   // minimum width
    if (panelWidth > 500) panelWidth = 500;   // maximum width
    
    //Our side ANKI :-)
    Rectangle panel = {
      (float)(GetScreenWidth() - panelWidth),
      0.0f,
      (float)panelWidth,
      (float)GetScreenHeight()
    };
    
    /* --- 1. INPUT TAKING (START HERE) --- */
    /* --- 1. INPUT TAKING (START HERE) --- */
    
    // snake center
    float centerX = snakeRec.x + snakeRec.width / 2;
    float centerY = snakeRec.y + snakeRec.height / 2;
    
    // mouse position
    float mouseX = GetMouseX();
    float mouseY = GetMouseY();
    
    // angle toward mouse
    float targetAngle = atan2(mouseY - centerY, mouseX - centerX);
    
    // maximum turning speed
    float maxTurnSpeed = 5.0f; // radians per second
    
    float angleDiff = targetAngle - angle;
    
    // normalize to [-PI, PI]
    while (angleDiff > PI) angleDiff -= 2 * PI;
    while (angleDiff < -PI) angleDiff += 2 * PI;
    
    // smooth turning
    if (angleDiff > maxTurnSpeed * dt) angleDiff = maxTurnSpeed * dt;
    if (angleDiff < -maxTurnSpeed * dt) angleDiff = -maxTurnSpeed * dt;
    
    angle += angleDiff;
    
    // move forward
    snakeRec.x += cos(angle) * speed * dt;
    snakeRec.y += sin(angle) * speed * dt;
    /*float centerX = snakeRec.x + snakeRec.width / 2;
    float centerY = snakeRec.y + snakeRec.height / 2;
    
    float mouseX = GetMouseX();
    float mouseY = GetMouseY();
    
    // If mouse moved → compute new direction ONCE
    if (mouseX != lastMouse.x || mouseY != lastMouse.y)
      {
	float dx = mouseX - centerX;
	float dy = mouseY - centerY;
	
	angle = atan2(dy, dx);   //set new direction instantly
	
	lastMouse = { mouseX, mouseY };
      }
    
    // Always move forward
    snakeRec.x += cos(angle) * speed * dt;
    snakeRec.y += sin(angle) * speed * dt;
    */
    /* ------------------------------------ */
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
    
    /*if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
      speed = 400;
    else
    speed = 250;*/
    
    /* --- 2. UPDATE / LOGIC ---
       (Collision checks, AI, etc. go here)*/
    float gameWidth = GetScreenWidth() - panelWidth;
    
    //eating - yammmmmmmmmy ;)                
    //collision
    for (int i = 0; i < 3; i++)
      {
	if (CheckCollisionRecs(snakeRec, optionRects[i]))
	  {
	    if (optionIndices[i] == correctIndex)
	      {
		score++;
	      }
	    else
	      {
		lives--;
	      }
		
	    GenerateQuiz(terms, correctIndex, optionIndices, optionRects,
			 gameWidth, GetScreenHeight());
	    break;
	  }
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
        if (state == MENU)
    {
        // Draw menu
        DrawText("NERD SNAKE", GetScreenWidth()/2 - 150, 150, 50, GREEN);
        DrawText("Press ENTER to Start", GetScreenWidth()/2 - 150, 300, 30, WHITE);

        // Start game when ENTER is pressed
        if (IsKeyPressed(KEY_ENTER))
        {
            state = GAMEPLAY;
        }
    }
    else if (state == GAMEPLAY)
    {
    DrawRectangleRec(panel, DARKGRAY);
    DrawText(std::to_string(score).c_str(), 25, 25, 20, RED);
    DrawText(("Your lives:" + std::to_string(lives)).c_str(), 25, 45, 20, RED); 
    for (int i = 0; i < 3; i++)
	  {
	    DrawRectangleRec(optionRects[i], MAROON);
	    
	    DrawText(
		     terms[optionIndices[i]].word.c_str(),
		     optionRects[i].x + 10,
		     optionRects[i].y + 10,
		     20,
		     WHITE
		     );
	  }
    snakeBlock(snakeRec);
    
    //Show eaten word in our side ANKI :-)
    int panelTextX = GetScreenWidth() - panelWidth + 20;
    int panelTextY = 50;
    
    DrawText("Definition:", panelTextX, panelTextY, 20, WHITE);
    DrawText(
	     terms[correctIndex].definition.c_str(),
	     panelTextX,
	     panelTextY + 30,
	     18,
	     LIGHTGRAY
	     );
    }
    EndDrawing();
    
  }
  
  CloseWindow();
  return 0;
}

//MADE WITH LOVE BY CHEIKH_DAH <3  
  
