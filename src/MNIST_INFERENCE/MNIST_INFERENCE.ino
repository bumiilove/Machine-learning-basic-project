// 필요 라이브러리 선언
#include <MLP_32_16_95.35_4.h>
#include <stdio.h>
#include <math.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <TouchScreen.h>
#include <TFT_eSPI.h>       // Hardware-specific library

// configuration for tft display only
// Calibrate values

// 터치 입력 값 설정
#define point_high 0.90
#define point_low 0.45

#define WINPUT 784
#define HIDDEN1 32
#define HIDDEN2 16
#define WOUTPUT 10

// Assign human-readable names to some common 16-bit color values:
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E3
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

TFT_eSPI tft = TFT_eSPI();   // Invoke custom library

// 2) configuration for touch screen read
#define SENSIBILITY 300
#define MINPRESSURE 10
#define MAXPRESSURE 3000

#define YP 14  // must be an analog pin, use "An" notation!
#define XM 12  // must be an analog pin, use "An" notation!
#define YM 22  // can be a digital pin
#define XP 21  // can be a digital pin

// touch instance 
TouchScreen ts = TouchScreen(XP, YP, XM, YM, SENSIBILITY);

// optional
#define LCD_RESET A4
#define BOXSIZE 224

#define BUTTON_W 80
#define BUTTON_H 30

#define x_margin 8
#define y_margin 30
#define X_MIN -2730
#define X_MAX 735
#define Y_MIN -830
#define Y_MAX 2230

float input[WINPUT] = {0};
int16_t dx[8] = {1, 1, -1, -1};
int16_t dy[8] = {1, -1, 1, -1};

float conf_score = 0.00;

// Functions
void inference();
void show_result(int x);
void printMap();

void setup() {
  Serial.begin(9600);
  Serial.println(F("Paint!"));

  tft.setRotation(0);
  tft.begin(0x9341);   // Hardware driver define
  tft.fillScreen(BLACK);  // screen setting
  tft.fillRect(x_margin, y_margin, BOXSIZE, BOXSIZE, WHITE);  // Blank for Input digit

  tft.fillRect(x_margin, y_margin + BOXSIZE + 5, BUTTON_W, BUTTON_H, GREEN);    // buttons GUI G:: inference Y:: reset
  tft.fillRect(tft.width() - x_margin - BUTTON_W, y_margin + BOXSIZE + 5, BUTTON_W, BUTTON_H, YELLOW);

  // text info
  tft.setCursor(10, 12);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.println("Hand written digit");
}

void loop() {
  if (Serial.available() > 0) {  // 시리얼 입력이 있는지 확인
    char input = Serial.read();  // 입력된 문자 읽기
    if (input == 'a') {  // 'a'를 입력받으면 동작 수행
      printMap();
      Serial.println("Function A executed!");  // 예시로 동작 수행 후 시리얼 모니터에 출력
    }
  }

  TSPoint p = ts.getPoint();      // point instance(structer) 

  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);

  p.z = abs(p.z);
  delay(1);

  if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {

    int16_t x = map(p.x, X_MIN, X_MAX, 0, tft.width());
    int16_t y = map(p.y, Y_MIN, Y_MAX, 0, tft.height());

    if (x >= x_margin && x <= x_margin + BOXSIZE && y >= y_margin && y <= y_margin + BOXSIZE) {

      tft.fillCircle(x, y, 15, BLACK);
      int16_t mapx = map(x, 8, 232, 0, 27);
      int16_t mapy = map(y, 30, 254, 0, 27);

      input[(mapy * 28) + mapx] = point_high;

      // 대각선 방향 입력
      for (int i = 0; i < 4; i++) {
        if (input[((mapy + dy[i]) * 28) + mapx + dx[i]] < point_low) {
          input[((mapy + dy[i]) * 28) + mapx + dx[i]] = point_low;
        }
      }
    }

    if (x > 120 && y > 260) {
      inference();
      delay(500);
    }
  }
}

void printMap() {
  for (int row = 0; row < 28; row++) {
    for (int col = 0; col < 28; col++) {
      if (input[row * 28 + col] != 0.00) {
        Serial.print(input[row * 28 + col]);
        Serial.print(" ");
      } else {
        Serial.print("   ");
      }
    }
    Serial.println();
  }
}

float relu(float x) {
  return (x > 0) * x;
}

void inference() {
  int inferenced_res = -1;

  float layer1[HIDDEN1] = {0};
  float layer2[HIDDEN2] = {0};
  float output[WOUTPUT] = {0};
  float max_out = 0;
  float exp_sum = 0;

  // ----------------- Inference ---------------------- //

  // input to layer1
  for (int x = 0; x < HIDDEN1; x++) {
    for (int y = 0; y < WINPUT; y++) {
      layer1[x] += w1[x][y] * input[y];
    }
    layer1[x] += b1[x];
    layer1[x] = relu(layer1[x]);
  }

  // layer1 to layer2
  for (int x = 0; x < HIDDEN2; x++) {
    for (int y = 0; y < HIDDEN1; y++) {
      layer2[x] += w2[x][y] * layer1[y];
    }
    layer2[x] += b2[x];
    layer2[x] = relu(layer2[x]);
  }

  // layer2 to output
  for (int x = 0; x < WOUTPUT; x++) {
    for (int y = 0; y < HIDDEN2; y++) {
      output[x] += w3[x][y] * layer2[y];
    }
    output[x] += b3[x];
    output[x] = relu(output[x]);
  }

  // apply log_softmax to output
  for (int i = 0; i < WOUTPUT; i++) {
    if (max_out < output[i]) {
      max_out = output[i];
    }
  }

  for (int i = 0; i < WOUTPUT; i++) {
    output[i] = (float)exp((double)output[i] - (double)max_out);
    exp_sum += output[i];
  }

  for (int i = 0; i < WOUTPUT; i++) {
    output[i] = output[i] / exp_sum;
  }

  Serial.println("\ninference!");

  // print output
  for (int i = 0; i < WOUTPUT; i++) {
    Serial.print(i);
    Serial.print(" ");
    Serial.println(output[i]);
    if (output[i] > conf_score) {
      conf_score = output[i];
      inferenced_res = i;
    }
  }

  Serial.println(inferenced_res);
  show_result(inferenced_res);

  // ----------------- Inference ---------------------- //
}

void show_result(int x) {
  printMap();

  // 첫 번째 픽셀부터 마지막 픽셀까지 점 하나하나를 검은색으로 채움
  for (int y = 0; y < TFT_HEIGHT; y++) {
    for (int x = 0; x < TFT_WIDTH; x++) {
      tft.drawPixel(x, y, TFT_BLACK);
    }
  }

  tft.setCursor(15, 80);
  tft.setTextSize(2);
  tft.setTextColor(RED);
  tft.println("Inferenced Class");

  tft.setCursor(100, 150);
  tft.setTextSize(20);
  tft.setTextColor(RED);
  tft.print(x);

  delay(1500);
  ESP.restart();
}
