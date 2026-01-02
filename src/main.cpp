/*
  按钮控制电机：按一下开关，电机转3秒

  接线：
    - TB6612 AIN1 -> ESP32 GPIO 26
    - TB6612 AIN2 -> ESP32 GPIO 27
    - TB6612 PWMA -> ESP32 GPIO 25 (PWM 输出)
    - TB6612 STBY -> ESP32 GPIO 33 (使能脚，HIGH 使能)
    - 按钮 -> ESP32 GPIO 2 (一端接GPIO2，一端接地)
    - 共地：ESP32 GND 与 TB6612 GND/VM 共地
*/

#include <Arduino.h>

// ----- 引脚配置 -----
const int AIN1_PIN = 26; // 驱动 IN1，决定转向
const int AIN2_PIN = 27; // 驱动 IN2，决定转向
const int PWMA_PIN = 25; // 驱动 PWM 控制速度
const int STBY_PIN = 33; // 驱动使能：HIGH=使能, LOW=休眠/禁用
const int BUTTON_PIN = 4; // 按钮引脚

// ----- PWM 配置 -----
const int PWM_CHANNEL = 0;    // LEDC 通道
const int PWM_FREQ = 20000;   // 20kHz，避免听觉噪音
const int PWM_RES = 8;        // 8-bit 分辨率 -> 0-255
const int MOTOR_SPEED = 200;  // 默认速度（0-255）

// ----- 电机运行时间 -----
const unsigned long MOTOR_RUN_MS = 3000; // 3秒

// 状态变量
bool motorRunning = false;
unsigned long motorStartTime = 0;
bool lastButtonState = HIGH; // 初始化为 HIGH
bool buttonPressed = false; // 新增：标记按钮是否已按下

void setPWM(int duty) {
  duty = constrain(duty, 0, 255);
  ledcWrite(PWM_CHANNEL, duty);
}

void enableDriver(bool en) {
  digitalWrite(STBY_PIN, en ? HIGH : LOW);
}

void motorStop() {
  digitalWrite(AIN1_PIN, LOW);
  digitalWrite(AIN2_PIN, LOW);
  setPWM(0);
}

void motorForward(int speed = MOTOR_SPEED) {
  digitalWrite(AIN1_PIN, HIGH);
  digitalWrite(AIN2_PIN, LOW);
  setPWM(speed);
}

void setup() {
  Serial.begin(115200);
  // 配置 IO
  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(PWMA_PIN, OUTPUT);
  pinMode(STBY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // 按钮输入，上拉

  // 初始化 PWM
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(PWMA_PIN, PWM_CHANNEL);

  // 启用驱动，并确保初始为停止状态
  enableDriver(true);
  motorStop();

  Serial.println("Button control: Press button to run motor for 3s (串口 115200)");
}

void loop() {
  // 读取按钮状态
  bool buttonState = digitalRead(BUTTON_PIN);

  // 检测按钮按下（从高到低，且未按下状态）
  if (lastButtonState == HIGH && buttonState == LOW && !buttonPressed) {
    buttonPressed = true;
    if (!motorRunning) {
      motorRunning = true;
      motorStartTime = millis();
      motorForward();
      Serial.println("Motor ON for 3s");
    }
  }

  // 检测按钮松开（从低到高）
  if (lastButtonState == LOW && buttonState == HIGH) {
    buttonPressed = false;
  }

  // 打印 GPIO 状态变化（调试用）
  if (buttonState != lastButtonState) {
    Serial.print("Button GPIO ");
    Serial.print(BUTTON_PIN);
    Serial.print(": ");
    Serial.println(buttonState);
  }

  lastButtonState = buttonState;

  // 检查电机运行时间
  if (motorRunning && (millis() - motorStartTime >= MOTOR_RUN_MS)) {
    motorRunning = false;
    motorStop();
    Serial.println("Motor OFF");
  }

  // 小延时
  delay(100);
}


