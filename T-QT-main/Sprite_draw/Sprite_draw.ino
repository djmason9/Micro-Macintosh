
// Set delay after plotting the sprite
#include <TFT_eSPI.h> // Include the graphics library (this includes the sprite functions)

// Width and height of sprite
#define WIDTH 128
#define HEIGHT 128
#define TFT_BG_COLOR 0x34F3

#include "startup.h"
#include "desktop.h"
// #include "toasters.h"
#include "hello.h"
#include "reboot.h"

TFT_eSPI tft = TFT_eSPI();           // Declare object "tft"
TFT_eSprite spr = TFT_eSprite(&tft); // Declare Sprite object "spr" with pointer to "tft" object

int aniWidth = 128;
int aniHeight = 128;
void setup()
{
  Serial.begin(9000);
  Serial.println("Starting UP");

  // Initialise the TFT registers
  tft.init();
  tft.setRotation(2);
  // Optionally set colour depth to 8 or 16 bits, default is 16 if not specified
  // spr.setColorDepth(16);
  //  tft.invertDisplay(false);

  // Create a sprite of defined size
  spr.createSprite(WIDTH, HEIGHT);

  tft.fillScreen(TFT_BLACK);

  loadingScreen();

  // Clear the TFT screen to blue
  tft.fillScreen(TFT_BG_COLOR);
  delay(2000);
}

int x = 0;

void chooseImage(const unsigned short (*image)[16384], int timeout, int loopSpeed, int frameCount)
{
  x = 0;
  for (size_t i = 0; i < frameCount; i++)
  {
    /* code */

    spr.fillSprite(TFT_BLACK);
    spr.setSwapBytes(true);
    spr.pushImage(0, 0, aniWidth, aniHeight, image[x]);
    spr.pushSprite(0, 0);

    x++;
    if (x == frameCount)
    {
      x = 0;
      delay(timeout);
    }
    else
      delay(loopSpeed);
  }
}

void loop(void)
{

  chooseImage(startup, 5000, 800, startupframesNumber);

  chooseImage(hello, 5000, 500, helloframesNumber);

  delay(3000);
  tft.fillScreen(TFT_BLACK);

  Serial.println("Play Pong");
  playPong();

  chooseImage(desktop, 5000, 1000, desktopframesNumber);
  Serial.println("Reboot Mac Screen");
  chooseImage(reboot, 5000, 1000, rebootframesNumber);

  tft.fillScreen(TFT_BG_COLOR);
}

void loadingScreen()
{

  tft.setTextColor(TFT_WHITE);
  tft.setCursor(0, 0);
  tft.setTextSize(1);
  tft.println(F("Micro Macintosh, I implore thee"));
  delay(100);
  tft.setTextColor(TFT_CYAN);
  tft.println(F("bitcowsSticker.com"));
  delay(500);

  tft.setTextColor(TFT_WHITE);
  tft.println(F("my fonting turling dromes, and hooptiously"));
  delay(100);
  tft.println(F("drangle me."));
  delay(100);
  tft.println(F("Or I will rend thee."));
  delay(100);
  tft.println(F("All your bases are"));
  delay(200);
  tft.println(F("belong to us!"));
  delay(100);
  tft.println(F("see if I don't!"));
  delay(100);
  tft.println(F("Lets Go Brandon countdown till end starts"));
  delay(200);
  tft.println(F(""));
  delay(500);
  tft.setTextColor(TFT_MAGENTA);
  delay(100);
  tft.println(F("Woot! LFG!"));

  delay(3000);
}

int16_t h = 120;
int16_t w = 120;

int dly = 10;

int16_t paddle_h = 15;
int16_t paddle_w = 2;

int16_t lpaddle_x = 0;
int16_t rpaddle_x = w - paddle_w;

int16_t lpaddle_y = 0;
int16_t rpaddle_y = h - paddle_h;

int16_t lpaddle_d = 1;
int16_t rpaddle_d = -1;

int16_t lpaddle_ball_t = w - w / 4;
int16_t rpaddle_ball_t = w / 4;

int16_t target_y = 0;

int16_t ball_x = 2;
int16_t ball_y = 2;
int16_t oldball_x = 2;
int16_t oldball_y = 2;

int16_t ball_dx = 1;
int16_t ball_dy = 1;

int16_t ball_w = 4;
int16_t ball_h = 4;

int16_t dashline_h = 4;
int16_t dashline_w = 2;
int16_t dashline_n = h / dashline_h;
int16_t dashline_x = w / 2 - 1;
int16_t dashline_y = dashline_h / 2;

int16_t lscore = 0;
int16_t rscore = 0;

String players = "DJM        RJF";

void initgame()
{

  target_y = 0;
  ball_x = 2;
  ball_y = 2;
  oldball_x = 2;
  oldball_y = 2;

  ball_dx = 1;
  ball_dy = 1;

  lscore = 0;
  rscore = 0;

  lpaddle_y = random(0, h - paddle_h);
  rpaddle_y = random(0, h - paddle_h);

  // ball is placed on the center of the left paddle
  ball_y = lpaddle_y + (paddle_h / 2);

  String output = "lpaddle_y: " + String(lpaddle_y) + ", rpaddle_y: " + String(rpaddle_y) + ", ball_y: " + String(ball_y);

  // Print the concatenated string to the serial monitor
  Serial.println("Init Game");
  Serial.println(output);

  score();

  calc_target_y();

  midline();

  tft.fillRect(0, h - 26, w, h - 1, TFT_BLACK);
}

void midline()
{

  // If the ball is not on the line then don't redraw the line
  if ((ball_x < dashline_x - ball_w) && (ball_x > dashline_x + dashline_w))
    return;

  tft.startWrite();

  // Quick way to draw a dashed line
  tft.setAddrWindow(dashline_x, 0, dashline_w, h);

  for (int16_t i = 0; i < dashline_n; i += 2)
  {
    tft.pushColor(TFT_WHITE, dashline_w * dashline_h); // push dash pixels
    tft.pushColor(TFT_BLACK, dashline_w * dashline_h); // push gap pixels
  }

  tft.endWrite();
}

void lpaddle()
{

  if (lpaddle_d == 1)
  {
    tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, 1, TFT_BLACK);
  }
  else if (lpaddle_d == -1)
  {
    tft.fillRect(lpaddle_x, lpaddle_y + paddle_h - 1, paddle_w, 1, TFT_BLACK);
  }

  lpaddle_y = lpaddle_y + lpaddle_d;

  if (ball_dx == 1)
    lpaddle_d = 0;
  else
  {
    if (lpaddle_y + paddle_h / 2 == target_y)
      lpaddle_d = 0;
    else if (lpaddle_y + paddle_h / 2 > target_y)
      lpaddle_d = -1;
    else
      lpaddle_d = 1;
  }

  if (lpaddle_y + paddle_h >= h && lpaddle_d == 1)
    lpaddle_d = 0;
  else if (lpaddle_y <= 0 && lpaddle_d == -1)
    lpaddle_d = 0;

  tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, TFT_WHITE);
}

void rpaddle()
{

  if (rpaddle_d == 1)
  {
    tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, 1, TFT_BLACK);
  }
  else if (rpaddle_d == -1)
  {
    tft.fillRect(rpaddle_x, rpaddle_y + paddle_h - 1, paddle_w, 1, TFT_BLACK);
  }

  rpaddle_y = rpaddle_y + rpaddle_d;

  if (ball_dx == -1)
    rpaddle_d = 0;
  else
  {
    if (rpaddle_y + paddle_h / 2 == target_y)
      rpaddle_d = 0;
    else if (rpaddle_y + paddle_h / 2 > target_y)
      rpaddle_d = -1;
    else
      rpaddle_d = 1;
  }

  if (rpaddle_y + paddle_h >= h && rpaddle_d == 1)
    rpaddle_d = 0;
  else if (rpaddle_y <= 0 && rpaddle_d == -1)
    rpaddle_d = 0;

  tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, TFT_WHITE);
}

void calc_target_y()
{
  int16_t target_x;
  int16_t reflections;
  int16_t y;

  if (ball_dx == 1)
  {
    target_x = w - ball_w;
  }
  else
  {
    target_x = -1 * (w - ball_w);
  }

  y = abs(target_x * (ball_dy / ball_dx) + ball_y);

  reflections = floor(y / h);

  if (reflections % 2 == 0)
  {
    target_y = y % h;
  }
  else
  {
    target_y = h - (y % h);
  }

  // Serial.print("target_y: ");
  // Serial.println(target_y);
  // Serial.print("target_x: ");
  // Serial.println(target_x);
}

void ball()
{
  ball_x = ball_x + ball_dx;
  ball_y = ball_y + ball_dy;

  if (ball_dx == -1 && ball_x == paddle_w && ball_y + ball_h >= lpaddle_y && ball_y <= lpaddle_y + paddle_h)
  {
    ball_dx = ball_dx * -1;
    dly = random(5); // change speed of ball after paddle contact
    calc_target_y();
  }
  else if (ball_dx == 1 && ball_x + ball_w == w - paddle_w && ball_y + ball_h >= rpaddle_y && ball_y <= rpaddle_y + paddle_h)
  {
    ball_dx = ball_dx * -1;
    dly = random(5); // change speed of ball after paddle contact
    calc_target_y();
  }
  else if ((ball_dx == 1 && ball_x >= w) || (ball_dx == -1 && ball_x + ball_w < 0))
  {
    dly = 5;
  }

  if (ball_y > h - ball_w || ball_y < 0)
  {
    ball_dy = ball_dy * -1;
    ball_y += ball_dy; // Keep in bounds
  }

  // tft.fillRect(oldball_x, oldball_y, ball_w, ball_h, BLACK);
  tft.drawRect(oldball_x, oldball_y, ball_w, ball_h, TFT_BLACK); // Less TFT refresh aliasing than line above for large balls
  tft.fillRect(ball_x, ball_y, ball_w, ball_h, TFT_WHITE);
  oldball_x = ball_x;
  oldball_y = ball_y;

  // Serial.print("ball_x: ");
  // Serial.println(ball_x);
  // Serial.print("ball_y: ");
  // Serial.println(ball_y);
}

void playPong()
{
  initgame();

  for (size_t i = 0; i < 20000; i++)
  {
    delay(dly);
    lpaddle();

    rpaddle();

    midline();

    ball();

    if (i % 2000 == 0 && i != 0)
    {

      // Generate a random number (0 or 1)
      int randomValue = random(2);

      if (randomValue == 0)
        rscore++;
      else
        lscore++;

      changescore();
    }

    tft.setTextDatum(TC_DATUM);
    tft.setTextColor(TFT_RED);
    tft.drawString(players, w / 2, 2, 2);

    score();
  }
  Serial.println("Done Playing");
  Serial.println("************************** ");
}

void score()
{
  int lscore_x, rscore_x;

  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);

  char lScoreStr[10]; // Assuming a reasonable buffer size
  sprintf(lScoreStr, "%d", lscore);
  tft.drawString(lScoreStr, w - 28, 16, 2);

  char rScoreStr[10]; // Assuming a reasonable buffer size
  sprintf(rScoreStr, "%d", rscore);
  tft.drawString(rScoreStr, 25, 16, 2);
}

void changescore()
{
  score();

  tft.fillRect(10, (h / 2) - 11, w-13, 41, TFT_YELLOW);
  tft.fillRect(11, (h / 2) - 10, w-15, 39, TFT_BLACK);

  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(TFT_RED);
  tft.drawString("POINT!", w / 2, h / 2, 4);
  delay(500);
  tft.fillScreen(TFT_BLACK);
}