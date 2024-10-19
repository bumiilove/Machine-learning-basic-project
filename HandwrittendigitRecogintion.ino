// 터치스크린 및 통신을 위한 라이브러리 선언
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// MLP 가중치 및 바이어스
#include "MLP_32_16_96.24_15.h" // 여기를 작성하세요. 가중치, 바이어스를 포함한 헤더 파일 이름 

// TFT 및 터치스크린 초기화
TFT_eSPI tft = TFT_eSPI();
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

// 화면 해상도
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// 숫자를 작성할 박스의 크기
#define BOX_SIZE 200

// 박스가 시작할 좌표 
#define BOX_X 10
#define BOX_Y 20  // (SCREEN_HEIGHT - BOX_SIZE) / 2 의 계산 결과인 20으로 설정

// 버튼의 위치, 크기 정의 
#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 20

#define BUTTON_INFER_X 230  // SCREEN_WIDTH - BUTTON_WIDTH - 10 의 계산 결과인 230으로 설정
#define BUTTON_RESET_X 230  // SCREEN_WIDTH - BUTTON_WIDTH - 10 의 계산 결과인 230으로 설정
#define BUTTON_PRINT_X 230  // SCREEN_WIDTH - BUTTON_WIDTH - 10 의 계산 결과인 230으로 설정

#define BUTTON_INFER_Y 90  // (SCREEN_HEIGHT - (3 * BUTTON_HEIGHT + 20)) / 2 의 계산 결과인 90으로 설정
#define BUTTON_RESET_Y 120  // BUTTON_INFER_Y + BUTTON_HEIGHT + 10 의 계산 결과인 120으로 설정
#define BUTTON_PRINT_Y 150  // BUTTON_RESET_Y + BUTTON_HEIGHT + 10 의 계산 결과인 150으로 설정

// 글씨의 크기 정의
#define FONT_SIZE 2

// 손글씨 입력 이미지 배열
float input_image[784] = {0}; // 28x28 이미지를 1차원 배열로 저장

// 초기 화면 설정
void drawBox() {
  // 여기에 손글씨를 입력 박스를 그리는 코드를 구현하세요. (drawRect(), BOX_X, BOX_Y, BOX_SIZE 활용)
}

void drawButtons() {
  // 여기에 Inference, Reset, Print 버튼을 그리는 코드를 구현하세요.
  // Inference 버튼
  tft.fillRect(BUTTON_INFER_X, BUTTON_INFER_Y, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_BLUE);
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString("Infer", BUTTON_INFER_X + BUTTON_WIDTH / 2, BUTTON_INFER_Y + 2, FONT_SIZE);

  // Reset 버튼


  // Print 버튼

}


// 초기 화면 클리어 및 초기화
void initScreen(){
  tft.fillScreen(TFT_WHITE);
  drawBox();
  drawButtons();
}

// ReLU 활성화 함수
float relu(float x) {
  // 여기에 ReLU 함수를 구현하세요. (네트워크에 다른 활성화 함수를 사용한 경우 해당 함수를 구현 
}

// Softmax 함수
void softmax(float* input, int len, float* output) {
  float max_val = input[0];
  
  for (int i = 1; i < len; i++) {
    if (input[i] > max_val) max_val = input[i];
  }

  float sum = 0.0;
  for (int i = 0; i < len; i++) {
    output[i] = exp(input[i] - max_val);
    sum += output[i];
  }

  for (int i = 0; i < len; i++) {
    output[i] /= sum;
  }
}


// MLP 예측 함수
int inference(float* input_image) {
  float layer1[32];
  float layer2[16];
  float output[10];

  // 첫 번째 은닉층 계산
  for (int i = 0; i < 32; i++) {
    layer1[i] = b1[i];
    for (int j = 0; j < 784; j++) {
      layer1[i] += w1[i][j] * input_image[j];
    }
    layer1[i] = relu(layer1[i]);
  }

  // 두 번째 은닉층 계산
  for (int i = 0; i < 16; i++) {
    layer2[i] = b2[i];
    for (int j = 0; j < 32; j++) {
      layer2[i] += w2[i][j] * layer1[j];
    }
    layer2[i] = relu(layer2[i]);
  }

  // 출력층 계산
  for (int i = 0; i < 10; i++) {
    output[i] = b3[i];
    for (int j = 0; j < 16; j++) {
      output[i] += w3[i][j] * layer2[j];
    }
  }

  // Softmax를 통해 확률로 변환
  float softmax_output[10];
  softmax(output, 10, softmax_output);

  // 가장 높은 확률을 가진 인덱스를 결과로 반환
  int predicted_digit = 0;
  float max_prob = softmax_output[0];
  
  for (int i = 1; i < 10; i++) {
    if (softmax_output[i] > max_prob) {
      max_prob = softmax_output[i];
      predicted_digit = i;
    }
  }
  return predicted_digit;
}


// 손글씨 입력 내용을 화면에 출력
void printInputImage() {

  tft.fillScreen(TFT_WHITE);
  
  int startX = BOX_X;
  int startY = BOX_Y;
  int boxSize = BOX_SIZE;
  int cellSize = boxSize / 28;

  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 28; j++) {
      if (input_image[i * 28 + j] > 0) {
        tft.fillRect(startX + j * cellSize, startY + i * cellSize, cellSize, cellSize, TFT_BLACK);
      }
    }
  }

  tft.setTextColor(TFT_BLACK);
  tft.drawCentreString("Input Image", BOX_X + 70, SCREEN_HEIGHT - 30, FONT_SIZE);
  delay(2000);  // 2초 대기 후 초기 화면으로 복귀
}

void setup() {
  Serial.begin(115200);

  // 터치스크린 초기화
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(3);

  // TFT 디스플레이 초기화
  tft.init();
  tft.setRotation(3);

  // 스크린 초기화
  initScreen();
}

void loop() {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    int x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    int y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    int z = p.z;

    // Reset 버튼 처리
    if (x > BUTTON_RESET_X && x < (BUTTON_RESET_X + BUTTON_WIDTH) && y > BUTTON_RESET_Y && y < (BUTTON_RESET_Y + BUTTON_HEIGHT) ) {
      tft.fillRect(BOX_X, BOX_Y, BOX_SIZE, BOX_SIZE, TFT_WHITE);
      drawBox();
      // 여기에 입력된 이미지 초기화 코드 구현하세요. (memset(); 함수 사용) 
      memset(input_image, 0, sizeof(input_image)); // 입력된 이미지 초기화
    }
    
    // Inference 버튼 처리
    else if (x > BUTTON_INFER_X && x < (BUTTON_INFER_X + BUTTON_WIDTH) && y > BUTTON_INFER_Y && y < (BUTTON_INFER_Y + BUTTON_HEIGHT)) {
      int predicted_digit = inference(input_image);

      tft.fillScreen(TFT_WHITE);  // 새 화면으로 전환
      tft.setTextColor(TFT_BLACK);
      tft.drawCentreString("Predicted Digit:", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3, FONT_SIZE);
      tft.drawCentreString(String(predicted_digit), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE * 3);

      delay(2000);  // 2초 대기 후 초기 화면으로 복귀
      initScreen();
      memset(input_image, 0, sizeof(input_image)); // 입력된 이미지 초기화
    }
    
    // Print 버튼 처리
    else if (x > BUTTON_PRINT_X && x < (BUTTON_PRINT_X + BUTTON_WIDTH) && y > BUTTON_PRINT_Y && y < (BUTTON_PRINT_Y + BUTTON_HEIGHT)) {
      printInputImage();
      
      int predicted_digit = inference(input_image);
      tft.setTextColor(TFT_BLACK);
      tft.drawCentreString("Predicted Digit:", BUTTON_PRINT_X, BUTTON_PRINT_Y, FONT_SIZE);
      tft.drawCentreString(String(predicted_digit), BUTTON_PRINT_X, BUTTON_PRINT_Y + 20, FONT_SIZE * 3);
      delay(2000);  // 2초 대기 후 초기 화면으로 복귀
      initScreen();
      memset(input_image, 0, sizeof(input_image)); // 입력된 이미지 초기화
    }
    
    // 박스 안의 손글씨 입력 처리
    else if (x > BOX_X && x < (BOX_X + BOX_SIZE) && y > BOX_Y && y < (BOX_Y + BOX_SIZE)) {
      tft.fillCircle(x, y, 5, TFT_BLACK);

      // 입력된 점을 input_image로 변환하여 저장
      int mapped_x = map(x, BOX_X, BOX_X + BOX_SIZE, 0, 27);
      int mapped_y = map(y, BOX_Y, BOX_Y + BOX_SIZE, 0, 27);
      // mapped_x, mapped_y 를 이용해서 input_image 배열 안에 진하기 값을 할당 
      input_image[mapped_y * 28 + mapped_x] = 0.8f; // 해당 좌표에 값을 저장
    }

    delay(5);
  }
}

// 터치스크린 및 통신을 위한 라이브러리 선언
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

// MLP 가중치 및 바이어스
#include "MLP_32_16_96.24_15.h" // 여기를 작성하세요. 가중치, 바이어스를 포함한 헤더 파일 이름 

// TFT 및 터치스크린 초기화
TFT_eSPI tft = TFT_eSPI();
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

// 화면 해상도
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

// 숫자를 작성할 박스의 크기
#define BOX_SIZE 200

// 박스가 시작할 좌표 
#define BOX_X 10
#define BOX_Y 20  // (SCREEN_HEIGHT - BOX_SIZE) / 2 의 계산 결과인 20으로 설정

// 버튼의 위치, 크기 정의 
#define BUTTON_WIDTH 80
#define BUTTON_HEIGHT 20

#define BUTTON_INFER_X 230  // SCREEN_WIDTH - BUTTON_WIDTH - 10 의 계산 결과인 230으로 설정
#define BUTTON_RESET_X 230  // SCREEN_WIDTH - BUTTON_WIDTH - 10 의 계산 결과인 230으로 설정
#define BUTTON_PRINT_X 230  // SCREEN_WIDTH - BUTTON_WIDTH - 10 의 계산 결과인 230으로 설정

#define BUTTON_INFER_Y 90  // (SCREEN_HEIGHT - (3 * BUTTON_HEIGHT + 20)) / 2 의 계산 결과인 90으로 설정
#define BUTTON_RESET_Y 120  // BUTTON_INFER_Y + BUTTON_HEIGHT + 10 의 계산 결과인 120으로 설정
#define BUTTON_PRINT_Y 150  // BUTTON_RESET_Y + BUTTON_HEIGHT + 10 의 계산 결과인 150으로 설정

// 글씨의 크기 정의
#define FONT_SIZE 2

// 손글씨 입력 이미지 배열
float input_image[784] = {0}; // 28x28 이미지를 1차원 배열로 저장

// 초기 화면 설정
void drawBox() {
  // 여기에 손글씨를 입력 박스를 그리는 코드를 구현하세요. (drawRect(), BOX_X, BOX_Y, BOX_SIZE 활용)
}

void drawButtons() {
  // 여기에 Inference, Reset, Print 버튼을 그리는 코드를 구현하세요.
  // Inference 버튼
  tft.fillRect(BUTTON_INFER_X, BUTTON_INFER_Y, BUTTON_WIDTH, BUTTON_HEIGHT, TFT_BLUE);
  tft.setTextColor(TFT_WHITE);
  tft.drawCentreString("Infer", BUTTON_INFER_X + BUTTON_WIDTH / 2, BUTTON_INFER_Y + 2, FONT_SIZE);

  // Reset 버튼


  // Print 버튼

}


// 초기 화면 클리어 및 초기화
void initScreen(){
  tft.fillScreen(TFT_WHITE);
  drawBox();
  drawButtons();
}

// ReLU 활성화 함수
float relu(float x) {
  // 여기에 ReLU 함수를 구현하세요. (네트워크에 다른 활성화 함수를 사용한 경우 해당 함수를 구현 
}

// Softmax 함수
void softmax(float* input, int len, float* output) {
  float max_val = input[0];
  
  for (int i = 1; i < len; i++) {
    if (input[i] > max_val) max_val = input[i];
  }

  float sum = 0.0;
  for (int i = 0; i < len; i++) {
    output[i] = exp(input[i] - max_val);
    sum += output[i];
  }

  for (int i = 0; i < len; i++) {
    output[i] /= sum;
  }
}


// MLP 예측 함수
int inference(float* input_image) {
  float layer1[32];
  float layer2[16];
  float output[10];

  // 첫 번째 은닉층 계산
  for (int i = 0; i < 32; i++) {
    layer1[i] = b1[i];
    for (int j = 0; j < 784; j++) {
      layer1[i] += w1[i][j] * input_image[j];
    }
    layer1[i] = relu(layer1[i]);
  }

  // 두 번째 은닉층 계산
  for (int i = 0; i < 16; i++) {
    layer2[i] = b2[i];
    for (int j = 0; j < 32; j++) {
      layer2[i] += w2[i][j] * layer1[j];
    }
    layer2[i] = relu(layer2[i]);
  }

  // 출력층 계산
  for (int i = 0; i < 10; i++) {
    output[i] = b3[i];
    for (int j = 0; j < 16; j++) {
      output[i] += w3[i][j] * layer2[j];
    }
  }

  // Softmax를 통해 확률로 변환
  float softmax_output[10];
  softmax(output, 10, softmax_output);

  // 가장 높은 확률을 가진 인덱스를 결과로 반환
  int predicted_digit = 0;
  float max_prob = softmax_output[0];
  
  for (int i = 1; i < 10; i++) {
    if (softmax_output[i] > max_prob) {
      max_prob = softmax_output[i];
      predicted_digit = i;
    }
  }
  return predicted_digit;
}


// 손글씨 입력 내용을 화면에 출력
void printInputImage() {

  tft.fillScreen(TFT_WHITE);
  
  int startX = BOX_X;
  int startY = BOX_Y;
  int boxSize = BOX_SIZE;
  int cellSize = boxSize / 28;

  for (int i = 0; i < 28; i++) {
    for (int j = 0; j < 28; j++) {
      if (input_image[i * 28 + j] > 0) {
        tft.fillRect(startX + j * cellSize, startY + i * cellSize, cellSize, cellSize, TFT_BLACK);
      }
    }
  }

  tft.setTextColor(TFT_BLACK);
  tft.drawCentreString("Input Image", BOX_X + 70, SCREEN_HEIGHT - 30, FONT_SIZE);
  delay(2000);  // 2초 대기 후 초기 화면으로 복귀
}

void setup() {
  Serial.begin(115200);

  // 터치스크린 초기화
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  touchscreen.setRotation(3);

  // TFT 디스플레이 초기화
  tft.init();
  tft.setRotation(3);

  // 스크린 초기화
  initScreen();
}

void loop() {
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    TS_Point p = touchscreen.getPoint();
    int x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    int y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    int z = p.z;

    // Reset 버튼 처리
    if (x > BUTTON_RESET_X && x < (BUTTON_RESET_X + BUTTON_WIDTH) && y > BUTTON_RESET_Y && y < (BUTTON_RESET_Y + BUTTON_HEIGHT) ) {
      tft.fillRect(BOX_X, BOX_Y, BOX_SIZE, BOX_SIZE, TFT_WHITE);
      drawBox();
      // 여기에 입력된 이미지 초기화 코드 구현하세요. (memset(); 함수 사용) 
      memset(input_image, 0, sizeof(input_image)); // 입력된 이미지 초기화
    }
    
    // Inference 버튼 처리
    else if (x > BUTTON_INFER_X && x < (BUTTON_INFER_X + BUTTON_WIDTH) && y > BUTTON_INFER_Y && y < (BUTTON_INFER_Y + BUTTON_HEIGHT)) {
      int predicted_digit = inference(input_image);

      tft.fillScreen(TFT_WHITE);  // 새 화면으로 전환
      tft.setTextColor(TFT_BLACK);
      tft.drawCentreString("Predicted Digit:", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 3, FONT_SIZE);
      tft.drawCentreString(String(predicted_digit), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, FONT_SIZE * 3);

      delay(2000);  // 2초 대기 후 초기 화면으로 복귀
      initScreen();
      memset(input_image, 0, sizeof(input_image)); // 입력된 이미지 초기화
    }
    
    // Print 버튼 처리
    else if (x > BUTTON_PRINT_X && x < (BUTTON_PRINT_X + BUTTON_WIDTH) && y > BUTTON_PRINT_Y && y < (BUTTON_PRINT_Y + BUTTON_HEIGHT)) {
      printInputImage();
      
      int predicted_digit = inference(input_image);
      tft.setTextColor(TFT_BLACK);
      tft.drawCentreString("Predicted Digit:", BUTTON_PRINT_X, BUTTON_PRINT_Y, FONT_SIZE);
      tft.drawCentreString(String(predicted_digit), BUTTON_PRINT_X, BUTTON_PRINT_Y + 20, FONT_SIZE * 3);
      delay(2000);  // 2초 대기 후 초기 화면으로 복귀
      initScreen();
      memset(input_image, 0, sizeof(input_image)); // 입력된 이미지 초기화
    }
    
    // 박스 안의 손글씨 입력 처리
    else if (x > BOX_X && x < (BOX_X + BOX_SIZE) && y > BOX_Y && y < (BOX_Y + BOX_SIZE)) {
      tft.fillCircle(x, y, 5, TFT_BLACK);

      // 입력된 점을 input_image로 변환하여 저장
      int mapped_x = map(x, BOX_X, BOX_X + BOX_SIZE, 0, 27);
      int mapped_y = map(y, BOX_Y, BOX_Y + BOX_SIZE, 0, 27);
      // mapped_x, mapped_y 를 이용해서 input_image 배열 안에 진하기 값을 할당 
      input_image[mapped_y * 28 + mapped_x] = 0.8f; // 해당 좌표에 값을 저장
    }

    delay(5);
  }
}
