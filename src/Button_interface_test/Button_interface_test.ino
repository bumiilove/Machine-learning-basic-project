#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

TFT_eSPI tft = TFT_eSPI();
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

// 디스플레이의 가로 및 세로 해상도 정의
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// 폰트 크기 정의
#define FONT_SIZE 2

// 버튼 위치와 크기 지정
#define BUTTON_X 100
#define BUTTON_Y 100
#define BUTTON_WIDTH 120
#define BUTTON_HEIGHT 60

void setup() {
  Serial.begin(115200);

  // 터치스크린 초기화
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(3);

  // TFT 디스플레이 초기화
  tft.init();
  tft.setRotation(3);

  // TFT 화면을 흰색으로 채움
  tft.fillScreen(TFT_WHITE);

  // 직사각형을 위치와 크기에 맞게 파란색으로 채움 ( drawRect != fillRect )
  tft.fillRect(BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_BLUE);
  // 텍스트의 색을 검정색으로 지정
  tft.setTextColor(TFT_WHITE);
  // 문구를 해당 위치에 해당 폰트 크기로 출력
  tft.drawCentreString("Press Me", BUTTON_X + BUTTON_WIDTH / 2, BUTTON_Y + BUTTON_HEIGHT / 2 - 8, 2);
}

void loop() {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();

    // 터치 좌표를 map 함수를 써서 변환 
    int x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    int y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    
    // 터치 좌표가 버튼의 범위일 경우 
    if (x > BUTTON_X && x < (BUTTON_X + BUTTON_WIDTH) && y > BUTTON_Y && y < (BUTTON_Y + BUTTON_HEIGHT)) {
      tft.fillScreen(TFT_GREEN);
      tft.setCursor(10, 10);
      tft.println("Button Pressed!");
      
      delay(1000);  // 대기 시간 후 원래 화면으로 복귀
      
      tft.fillScreen(TFT_WHITE);
      tft.fillRect(BUTTON_X, BUTTON_Y, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_BLUE);
      tft.setTextColor(TFT_WHITE);
      tft.drawCentreString("Press Me", BUTTON_X + BUTTON_WIDTH / 2, BUTTON_Y + BUTTON_HEIGHT / 2 - 8, 2);
    }
  }
}
