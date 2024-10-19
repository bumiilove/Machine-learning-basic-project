#include <SPI.h>

/* "TFT_eSPI" 라이브러리를 설치하여 TFT 디스플레이와 통신합니다.
   이 라이브러리는 ESP32와 같은 마이크로컨트롤러에서 TFT 디스플레이를 제어하는 데 사용됩니다.
   라이브러리를 설치한 후, User_Setup.h 파일을 반드시 수정하여 디스플레이에 맞게 설정해야 합니다.
   Random Nerd Tutorials에서 제공하는 예제를 사용할 때는 사이트에서 제공하는 User_Setup.h 파일을 사용해야 합니다.
   라이브러리 설치와 설정 방법에 대한 자세한 내용은 다음 링크에서 확인하세요:
   https://RandomNerdTutorials.com/cyd/
   https://RandomNerdTutorials.com/esp32-tft/
*/
#include <TFT_eSPI.h>

// "XPT2046_Touchscreen" 라이브러리를 설치하여 터치스크린 센서를 제어합니다.
// 이 라이브러리는 XPT2046 터치 컨트롤러와 인터페이스하며, 별도의 추가 설정 없이 바로 사용할 수 있습니다.
#include <XPT2046_Touchscreen.h>

// TFT 디스플레이 객체 생성
TFT_eSPI tft = TFT_eSPI();

// 터치스크린 핀 설정 (ESP32와 터치스크린 모듈 간의 연결 핀)
#define XPT2046_IRQ 36   // T_IRQ: 터치 인터럽트 핀
#define XPT2046_MOSI 32  // T_DIN: SPI 데이터 입력 핀 (MOSI)
#define XPT2046_MISO 39  // T_OUT: SPI 데이터 출력 핀 (MISO)
#define XPT2046_CLK 25   // T_CLK: SPI 클록 핀 (SCLK)
#define XPT2046_CS 33    // T_CS: 터치스크린 칩 선택 핀 (CS)

// SPI 통신을 위한 SPIClass 객체 생성 (VSPI는 ESP32의 기본 SPI 포트 중 하나)
SPIClass touchscreenSPI = SPIClass(VSPI);
// 터치스크린 객체 생성 (CS 핀과 IRQ 핀을 사용하여 초기화)
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

// 디스플레이의 가로 및 세로 해상도 정의
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
// 폰트 크기 정의
#define FONT_SIZE 2

// 터치스크린 좌표 변수: x, y 위치 및 압력 z
int x, y, z;


// 시리얼 모니터에 터치 좌표와 압력 정보 출력
void printTouchToSerial(int touchX, int touchY, int touchZ) {
  Serial.print("X = ");
  Serial.print(touchX);
  Serial.print(" | Y = ");
  Serial.print(touchY);
  Serial.print(" | Pressure = ");
  Serial.print(touchZ);
  Serial.println();
}


// TFT 디스플레이에 터치 좌표와 압력 정보 출력
void printTouchToDisplay(int touchX, int touchY, int touchZ) {
  // 디스플레이 화면을 흰색으로 초기화
  tft.fillScreen(TFT_WHITE);
  
  // 텍스트 색상을 검정으로 설정하고, 배경을 흰색으로 설정
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  // 디스플레이 중심의 X 좌표 계산
  int centerX = SCREEN_WIDTH / 2;
  
  // 텍스트의 Y 좌표 초기값 설정
  int textY = 80;
 
  // X 좌표 정보 문자열 생성 후 화면 중앙에 출력
  String tempText = "X = " + String(touchX);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);

  // Y 좌표 정보 문자열 생성 후 화면 중앙에 출력
  textY += 20;
  tempText = "Y = " + String(touchY);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);

  // 압력 정보 문자열 생성 후 화면 중앙에 출력
  textY += 20;
  tempText = "Pressure = " + String(touchZ);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);
}


void setup() {
  // 시리얼 통신을 115200bps로 시작. (디버깅 및 로그 출력을 위한 설정)
  Serial.begin(115200);

  // 터치스크린 SPI 통신 초기화
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  // 터치스크린 시작 및 SPI 설정 적용
  touchscreen.begin(touchscreenSPI);

  // 터치스크린을 가로 모드로 설정 (1 or 3)
  // 일부 디스플레이에서는 회전 방향이 반대일 수 있으므로, 이 경우 회전을 1이 아닌 3으로 설정
  touchscreen.setRotation(1);

  // TFT 디스플레이 초기화
  tft.init();
  // TFT 디스플레이를 가로 모드로 설정(회전 값은 터치스크린의 회전값과 동일하게 맞출 것)
  tft.setRotation(1);

  // 화면을 흰색으로 지운 후, 검은색 텍스트를 위한 준비
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  
  // 디스플레이의 중심 X 및 Y 좌표 계산
  int centerX = SCREEN_WIDTH / 2;
  int centerY = SCREEN_HEIGHT / 2;

  // 화면에 "Hello, world!" 텍스트를 출력
  tft.drawCentreString("Hello, world!", centerX, 30, FONT_SIZE);
  // 화면에 "Touch screen to test" 텍스트를 출력
  tft.drawCentreString("Touch screen to test", centerX, centerY, FONT_SIZE);
}

void loop() {
  // 터치스크린이 터치되었는지 확인
  // 터치가 감지되면, X, Y 좌표와 압력(Z) 값을 가져와 시리얼 모니터와 디스플레이에 출력
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    // 터치스크린 좌표를 가져옴.
    TS_Point p = touchscreen.getPoint();
  
    // map 함수를 사용하여 터치스크린의 X, Y 좌표(p.x, p.y)를 디스플레이의 해상도에 맞게 변환
    x = p.x;
    y = p.y;
    z = p.z;
    
    ??
    x = map(x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(y, 240, 3800, 1, SCREEN_HEIGHT);
    
    // 시리얼 모니터에 터치 좌표와 압력 정보를 출력
    printTouchToSerial(x, y, z);
    // 디스플레이에 터치 좌표와 압력 정보를 출력
    // ???
    printTouchToSerial(x, y, z);
    // 터치가 연속으로 감지되지 않도록 약간의 지연
    delay(100);
  }
}
