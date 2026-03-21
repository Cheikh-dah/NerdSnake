#include "raylib.h"
#include <string>
#include <vector>
#include <cmath>
#include <cstdlib>
#include "dotenv.h"
#include "terms.h"
#include "json.hpp"
#include <fstream>

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

  for (int i = 0; i < 3; i++) {
    optionRects[i].x = (float)GetRandomValue(0, (int)(gameWidth - 120));
    optionRects[i].y = (float)GetRandomValue(0, (int)(screenHeight - 150));
    optionRects[i].width = 120.0f;
    optionRects[i].height = 40.0f;
  }
}

enum GameState {
  MENU,
  GAMEPLAY,
  GAMEOVER
};

std::string scienceTopic = "";
int maxTopicLength = 12;

std::vector<Term> LoadTermsFromJSON(const std::string& filename) {
  std::vector<Term> terms;
  std::ifstream file(filename);
  if (!file.is_open()) return terms;
  try {
    nlohmann::json j;
    file >> j;
    if (!j.is_array()) return terms;
    for (auto& item : j) {
      if (!item.contains("term") || !item.contains("definition")) continue;
      if (!item["term"].is_string() || !item["definition"].is_string()) continue;
      Term t;
      t.word = item["term"].get<std::string>();
      t.definition = item["definition"].get<std::string>();
      terms.push_back(t);
    }
  } catch (...) {
    return terms;
  }
  return terms;
}

int main() {
  GameState state = MENU;
  std::vector<Term> terms = GetTerms();
  SetConfigFlags(FLAG_WINDOW_RESIZABLE);
  InitWindow(800, 600, "NerdSnake");

  int score = 0;
  int lives = 3;

  int correctIndex = 0;
  int optionIndices[3];
  Rectangle optionRects[3];

  Rectangle snakeRec = { 370, 300, 30, 30 };
  float angle = 0.0f;
  float speed = 250.0f;
  Vector2 lastMouse = { -1, -1 };

  int originalWidth = 800;
  int originalHeight = 600;

  GenerateQuiz(terms, correctIndex, optionIndices, optionRects, 800 - 250, 600);

  auto env = LoadEnv(".env");
  std::string apiKey = env["GEMINI_API_KEY"];

  while (!WindowShouldClose()) {

    float dt = GetFrameTime();

    int panelWidth = GetScreenWidth() * 0.25f;
    if (panelWidth < 250) panelWidth = 250;
    if (panelWidth > 500) panelWidth = 500;

    Rectangle panel = {
      (float)(GetScreenWidth() - panelWidth),
      0.0f,
      (float)panelWidth,
      (float)GetScreenHeight()
    };

    // Snake movement
    float centerX = snakeRec.x + snakeRec.width / 2;
    float centerY = snakeRec.y + snakeRec.height / 2;
    float mouseX = GetMouseX();
    float mouseY = GetMouseY();
    float targetAngle = atan2(mouseY - centerY, mouseX - centerX);
    float maxTurnSpeed = 5.0f;
    float angleDiff = targetAngle - angle;
    while (angleDiff > PI) angleDiff -= 2 * PI;
    while (angleDiff < -PI) angleDiff += 2 * PI;
    if (angleDiff > maxTurnSpeed * dt) angleDiff = maxTurnSpeed * dt;
    if (angleDiff < -maxTurnSpeed * dt) angleDiff = -maxTurnSpeed * dt;
    angle += angleDiff;
    snakeRec.x += cos(angle) * speed * dt;
    snakeRec.y += sin(angle) * speed * dt;

    // Toggle fullscreen
    if (IsKeyPressed(KEY_F11)) {
      if (!IsWindowFullscreen()) {
        int monitor = GetCurrentMonitor();
        SetWindowSize(GetMonitorWidth(monitor), GetMonitorHeight(monitor));
        ToggleFullscreen();
      } else {
        ToggleFullscreen();
        SetWindowSize(originalWidth, originalHeight);
      }
    }

    float gameWidth = GetScreenWidth() - panelWidth;

    // Collision
    for (int i = 0; i < 3; i++) {
      if (CheckCollisionRecs(snakeRec, optionRects[i])) {
        if (optionIndices[i] == correctIndex)
          score++;
        else
          lives--;
        GenerateQuiz(terms, correctIndex, optionIndices, optionRects,
                     gameWidth, GetScreenHeight());
        break;
      }
    }

    // Boundaries
    if (snakeRec.x < 0) snakeRec.x = 0;
    if (snakeRec.x + snakeRec.width > gameWidth)
      snakeRec.x = gameWidth - snakeRec.width;
    if (snakeRec.y < 0) snakeRec.y = 0;
    if (snakeRec.y + snakeRec.height > GetScreenHeight())
      snakeRec.y = GetScreenHeight() - snakeRec.height;

    // Game over
    if (lives <= 0) {
      state = GAMEOVER;
      lives = 3;
      score = 0;
      snakeRec.x = originalWidth / 2.0f;
      snakeRec.y = originalHeight / 2.0f;
    }

    // Drawing
    BeginDrawing();
    ClearBackground(BLACK);

    if (state == MENU)
      {
        int key = GetCharPressed();
        while (key > 0) {
          if ((key >= 32) && (key <= 125) && scienceTopic.length() < maxTopicLength)
            scienceTopic += (char)key;
          key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && !scienceTopic.empty())
          scienceTopic.pop_back();

        if (IsKeyPressed(KEY_ENTER) && !scienceTopic.empty()) {
          // 1- Generate JSON from C++
          std::string command = "generate_terms.exe " + scienceTopic;
          int result = system(command.c_str());

          // 2- Load terms
          if (result != 0) {
            TraceLog(LOG_WARNING, "API call failed, using default terms.");
            terms = GetTerms();
          } else {
            terms = LoadTermsFromJSON("terms.json");
            if (terms.size() < 10) {
              TraceLog(LOG_WARNING, "Too few terms, using defaults.");
              terms = GetTerms();
            }
          }

          // 3- Generate quiz
          float pw = GetScreenWidth() * 0.25f;
          if (pw < 250) pw = 250;
          if (pw > 500) pw = 500;
          GenerateQuiz(terms, correctIndex, optionIndices, optionRects,
                       GetScreenWidth() - pw, GetScreenHeight());

          // 4- Switch state
          state = GAMEPLAY;
        }

        DrawText("NERD SNAKE", GetScreenWidth()/2 - 150, 120, 50, GREEN);
        DrawText("Enter Science Topic:", GetScreenWidth()/2 - 150, 230, 25, WHITE);
        Rectangle topicBox = { (float)(GetScreenWidth()/2 - 150), 270.0f, 300.0f, 40.0f };
        DrawRectangleLinesEx(topicBox, 2, WHITE);
        DrawText(scienceTopic.c_str(), topicBox.x + 10, topicBox.y + 10, 20, YELLOW);
        DrawText("Press ENTER to Start", GetScreenWidth()/2 - 150, 340, 20, GRAY);
      }
    else if (state == GAMEPLAY)
      {
        DrawRectangleRec(panel, DARKGRAY);
        DrawText(("Score: " + std::to_string(score)).c_str(), 25, 25, 20, RED);
        DrawText(("Lives: " + std::to_string(lives)).c_str(), 25, 45, 20, RED);

        for (int i = 0; i < 3; i++) {
          DrawRectangleRec(optionRects[i], MAROON);
          DrawText(terms[optionIndices[i]].word.c_str(),
                   optionRects[i].x + 10, optionRects[i].y + 10, 20, WHITE);
        }

        snakeBlock(snakeRec);

        int panelTextX = GetScreenWidth() - panelWidth + 20;
        int panelTextY = 50;
        DrawText("Definition:", panelTextX, panelTextY, 20, WHITE);
        DrawText(terms[correctIndex].definition.c_str(),
                 panelTextX, panelTextY + 30, 18, LIGHTGRAY);
      }
    else if (state == GAMEOVER)
      {
        DrawText("GAME OVER!", GetScreenWidth()/2 - 150, 200, 50, RED);
        DrawText("Press ENTER to Restart", GetScreenWidth()/2 - 190, 350, 30, WHITE);
        DrawText("Press M to go to menu", GetScreenWidth()/2 - 170, 450, 30, WHITE);

        if (IsKeyPressed(KEY_ENTER))
          state = GAMEPLAY;
        else if (IsKeyDown(KEY_M))
          state = MENU;
      }

    EndDrawing();
  }

  CloseWindow();
  return 0;
}

//MADE WITH LOVE BY CHEIKH_DAH <3
