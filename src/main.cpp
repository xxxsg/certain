/*
  自动循环示例：上电后每秒交替运行/停止（不依赖任何按键）

  说明：根据你的要求，程序将不再使用按键控制；上电后立即进入循环：
    Motor ON（1 秒） -> Motor OFF（1 秒） -> 重复

  接线最简（同前）：
    - TB6612 AIN1 -> ESP32 GPIO 26
    - TB6612 AIN2 -> ESP32 GPIO 27
    - TB6612 PWMA -> ESP32 GPIO 25 (PWM 输出)
    - TB6612 STBY -> ESP32 GPIO 33 (使能脚，HIGH 使能)
    - 共地：ESP32 GND 与 TB6612 GND/VM 共地
*/

#include <Arduino.h>

// ----- 引脚配置 -----
const int AIN1_PIN = 26; // 驱动 IN1，决定转向
const int AIN2_PIN = 27; // 驱动 IN2，决定转向
const int PWMA_PIN = 25; // 驱动 PWM 控制速度
const int STBY_PIN = 33; // 驱动使能：HIGH=使能, LOW=休眠/禁用

// ----- PWM 配置 -----
const int PWM_CHANNEL = 0;    // LEDC 通道
const int PWM_FREQ = 20000;   // 20kHz，避免听觉噪音
const int PWM_RES = 8;        // 8-bit 分辨率 -> 0-255
const int MOTOR_SPEED = 200;  // 默认速度（0-255）

// ----- 自动循环时间（毫秒） -----
const unsigned long ON_MS = 1000;  // 运行 1 秒
const unsigned long OFF_MS = 1000; // 停止 1 秒

// 状态变量
bool motorIsOn = false;
unsigned long lastToggleMs = 0;

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

  // 初始化 PWM
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(PWMA_PIN, PWM_CHANNEL);

  // 启用驱动，并确保初始为停止状态
  enableDriver(true);
  motorStop();

  // 初始化计时器并立即开始第一次运行周期（上电即转）
  motorIsOn = true;
  lastToggleMs = millis();
  motorForward();
  Serial.println("Auto demo: Motor ON 1s, OFF 1s, repeat (串口 115200)");
}

void loop() {
  unsigned long now = millis();

  if (motorIsOn) {
    // 如果正在运行，检查是否已超过 ON_MS
    if (now - lastToggleMs >= ON_MS) {
      motorIsOn = false;
      lastToggleMs = now;
      motorStop();
      Serial.println("Motor OFF");
    }
  } else {
    // 如果停止状态，检查是否已超过 OFF_MS
    if (now - lastToggleMs >= OFF_MS) {
      motorIsOn = true;
      lastToggleMs = now;
      motorForward();
      Serial.println("Motor ON");
    }
  }

  // 小延时，避免 busy loop（不会影响定时精度）
  delay(10);
}


