#if 0  // 注：此文件为备份，已被封存以避免与 main.cpp 冲突
// 备份：原 main.cpp（已保存为 main_old.cpp）
// 如果需要恢复：将此文件内容拷贝回 main.cpp

/*
  ESP32 + TB6612FNG 窗帘控制（备份副本）
  — 原始完整实现保存在此处
*/

#include <Arduino.h>

// 这里保留原始文件的全部实现（备份）

const int AIN1_PIN = 26; // IN1
const int AIN2_PIN = 27; // IN2
const int PWMA_PIN = 25; // PWM 控制
const int STBY_PIN = 33; // 低电平进入休眠（停用），高电平使能驱动

const int DIR_UP_PIN = 14;   // 上升开关（edge 触发开始计时）
const int DIR_DOWN_PIN = 12; // 下降开关（保持按下则下降）
const int ESTOP_PIN = 13;    // 紧急停止按钮（按下时触发急停）

const int STATUS_LED = 2; // 状态指示灯（可选）

const int PWM_CHANNEL = 0;
const int PWM_FREQ = 20000; // 20kHz 足够安静
const int PWM_RES = 8;      // 8-bit 分辨率 -> 0-255
const int DEFAULT_SPEED = 200; // 0-255

unsigned long upDurationMs = 10000;

bool estopActive = false;
bool upTimerRunning = false;
unsigned long upStartMs = 0;

bool lastUpRaw = HIGH;
bool lastDownRaw = HIGH;
unsigned long lastUpChange = 0;
unsigned long lastDownChange = 0;
const unsigned long DEBOUNCE_MS = 50;

void setPWM(int duty) {
  duty = constrain(duty, 0, 255);
  ledcWrite(PWM_CHANNEL, duty);
}

void enableDriver(bool en) {
  digitalWrite(STBY_PIN, en ? HIGH : LOW);
}

void motorStopCoast() {
  digitalWrite(AIN1_PIN, LOW);
  digitalWrite(AIN2_PIN, LOW);
  setPWM(0);
}

void motorBrake() {
  digitalWrite(AIN1_PIN, HIGH);
  digitalWrite(AIN2_PIN, HIGH);
  setPWM(255);
}

void motorForward(int speed = DEFAULT_SPEED) {
  digitalWrite(AIN1_PIN, HIGH);
  digitalWrite(AIN2_PIN, LOW);
  setPWM(speed);
}

void motorReverse(int speed = DEFAULT_SPEED) {
  digitalWrite(AIN1_PIN, LOW);
  digitalWrite(AIN2_PIN, HIGH);
  setPWM(speed);
}

bool readButtonStable(int pin, bool &lastRaw, unsigned long &lastChange) {
  bool raw = digitalRead(pin);
  if (raw != lastRaw) {
    lastChange = millis();
    lastRaw = raw;
  }
  return (millis() - lastChange) > DEBOUNCE_MS ? raw : !raw;
}

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("[backup] setup...");

  pinMode(AIN1_PIN, OUTPUT);
  pinMode(AIN2_PIN, OUTPUT);
  pinMode(PWMA_PIN, OUTPUT);
  pinMode(STBY_PIN, OUTPUT);

  pinMode(DIR_UP_PIN, INPUT_PULLUP);
  pinMode(DIR_DOWN_PIN, INPUT_PULLUP);
  pinMode(ESTOP_PIN, INPUT_PULLUP);
  pinMode(STATUS_LED, OUTPUT);

  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(PWMA_PIN, PWM_CHANNEL);
  setPWM(0);
  enableDriver(true);
  motorStopCoast();
}

void loop() {
  // 备份副本 loop（简短输出）
  delay(1000);
  Serial.println("[backup] running");
}

#endif // end of backup wrapper
