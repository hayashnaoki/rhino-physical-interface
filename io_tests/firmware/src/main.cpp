/*
// IMU Test
#include <Arduino.h>
#include <Wire.h>

const int MPU_addr = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

void setup() {
  Serial.begin(115200);

  // Initialize I2C communication
  Wire.begin();

  // Wake up MPU-6050 from sleep mode
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // Write 0 to wake up
  Wire.endTransmission(true);

  delay(100);
}

void loop() {
  // Specify starting register address for data reading (0x3B = ACCEL_XOUT_H)
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);

  // Retrieve 14 bytes of data
  Wire.requestFrom(MPU_addr, 14, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read(); // Temperature data (skipped / not
output in this project) GyX = Wire.read() << 8 | Wire.read(); GyY = Wire.read()
<< 8 | Wire.read(); GyZ = Wire.read() << 8 | Wire.read();

  // Output in CSV format for easy parsing in Grasshopper
  Serial.print(AcX);
  Serial.print(",");
  Serial.print(AcY);
  Serial.print(",");
  Serial.print(AcZ);
  Serial.print(",");
  Serial.print(GyX);
  Serial.print(",");
  Serial.print(GyY);
  Serial.print(",");
  Serial.println(GyZ);

  delay(100); // 10Hz frequency (100ms interval)
}
*/

// HC-SR04 Ultrasonic Test
/*
#include <Arduino.h>

// HC-SR04
#define TRIG_PIN D2
#define ECHO_PIN D3

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    ; // Wait for serial connection (with timeout)

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  digitalWrite(TRIG_PIN, LOW);
  delay(100);
}

void loop() {
  // Transmit ultrasonic pulse (10us HIGH pulse)
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure the duration (in microseconds) that the Echo pin is HIGH
  // Set timeout to 30,000us (corresponding to approx. 5m distance)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  // Distance calculation: Speed of sound 340m/s -> 0.034 cm/us (divided by 2
for round trip) float distance_cm = (duration * 0.034f) / 2.0f;

  // Simple error handling for timeout or out of range (<= 0)
  if (duration == 0 || distance_cm > 400.0f) {
    // Output -1.0 if acquisition fails
    Serial.println("-1.0");
  } else {
    // Output distance to Serial (to 1 decimal place)
    Serial.println(distance_cm, 1);
  }

  // Measurement interval (100ms = 10Hz to send data smoothly to Grasshopper)
  delay(100);
}
*/

/*
// MPU-6050 and HC-SR04 Combined Test
#include <Arduino.h>
#include <Wire.h>

// MPU-6050 (IMU) settings
const int MPU_ADDR = 0x68;
int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

// HC-SR04 (Ultrasonic sensor) pin settings
#define TRIG_PIN D2
#define ECHO_PIN D3

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    ; // Wait for serial connection

  // --- 1. Initialize ultrasonic sensor ---
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // --- 2. Initialize MPU-6050 (I2C) ---
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // Wake up from sleep mode
  Wire.endTransmission(true);

  delay(100);
}

void loop() {
  // 1. Get MPU-6050 data
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // Start from ACCEL_XOUT_H register
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  AcX = Wire.read() << 8 | Wire.read();
  AcY = Wire.read() << 8 | Wire.read();
  AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read(); // Temperature (read to skip)
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // 2. HC-SR04 ultrasonic distance measurement
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure Echo HIGH duration (30,000us timeout)
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  float distance_cm = -1.0f;
  if (duration > 0) {
    float dist = (duration * 0.034f) / 2.0f;
    if (dist <= 400.0f) {
      distance_cm = dist;
    }
  }

  // 3. Serial output (CSV format)
  // [AcX],[AcY],[AcZ],[GyX],[GyY],[GyZ],[Distance]
  Serial.print(AcX);
  Serial.print(",");
  Serial.print(AcY);
  Serial.print(",");
  Serial.print(AcZ);
  Serial.print(",");
  Serial.print(GyX);
  Serial.print(",");
  Serial.print(GyY);
  Serial.print(",");
  Serial.print(GyZ);
  Serial.print(",");
  Serial.println(distance_cm, 1);

  // Transmit at 10Hz (100ms interval)
  delay(100);
}
*/

// MPU-6050 and HC-SR04 Combined Test (Filtered)
#include <Arduino.h>
#include <Wire.h>

// MPU-6050 (IMU) 設定
const int MPU_ADDR = 0x68;
int16_t raw_AcX, raw_AcY, raw_AcZ;
int16_t Tmp, GyX, GyY, GyZ;

// ハイパスフィルター用変数 (重力成分の保持)
float gravity_x = 0.0f;
float gravity_y = 0.0f;
float gravity_z = 0.0f;

// フィルター係数 alpha (0.8 ~ 0.95 程度で調整)
// 1.0
// に近づくほど「傾きの影響」を強力にカットし、「素早い動き」だけを抽出します
const float ALPHA = 0.90f;

// HC-SR04 (超音波センサー) ピン設定
#define TRIG_PIN D2
#define ECHO_PIN D3

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000)
    ;

  // --- 1. 超音波センサーの初期化 ---
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);

  // --- 2. MPU-6050 (I2C) の初期化 ---
  Wire.begin();
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x6B); // PWR_MGMT_1 register
  Wire.write(0);    // スリープモードを解除
  Wire.endTransmission(true);

  delay(100);
}

void loop() {
  // ==========================================
  // 1. MPU-6050 データ取得
  // ==========================================
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(0x3B); // ACCEL_XOUT_H
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_ADDR, 14, true);

  raw_AcX = Wire.read() << 8 | Wire.read();
  raw_AcY = Wire.read() << 8 | Wire.read();
  raw_AcZ = Wire.read() << 8 | Wire.read();
  Tmp = Wire.read() << 8 | Wire.read();
  GyX = Wire.read() << 8 | Wire.read();
  GyY = Wire.read() << 8 | Wire.read();
  GyZ = Wire.read() << 8 | Wire.read();

  // ==========================================
  // 2. ハイパスフィルターによる重力分分離
  // ==========================================
  // ローパスフィルターで重力成分（ゆっくりした変化）を推定
  gravity_x = ALPHA * gravity_x + (1.0f - ALPHA) * (float)raw_AcX;
  gravity_y = ALPHA * gravity_y + (1.0f - ALPHA) * (float)raw_AcY;
  gravity_z = ALPHA * gravity_z + (1.0f - ALPHA) * (float)raw_AcZ;

  // 全加速度から重力成分を差し引いて「運動加速度（純粋な動かし）」を取り出す
  int lin_AcX = (int)((float)raw_AcX - gravity_x);
  int lin_AcY = (int)((float)raw_AcY - gravity_y);
  int lin_AcZ = (int)((float)raw_AcZ - gravity_z);

  // ==========================================
  // 3. HC-SR04 超音波距離計測
  // ==========================================
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000);

  float distance_cm = -1.0f;
  if (duration > 0) {
    float dist = (duration * 0.034f) / 2.0f;
    if (dist <= 400.0f) {
      distance_cm = dist;
    }
  }

  // ==========================================
  // 4. シリアル出力 (CSVフォーマット)
  // [lin_AcX],[lin_AcY],[lin_AcZ],[GyX],[GyY],[GyZ],[Distance]
  // ==========================================
  Serial.print(lin_AcX);
  Serial.print(",");
  Serial.print(lin_AcY);
  Serial.print(",");
  Serial.print(lin_AcZ);
  Serial.print(",");
  Serial.print(GyX);
  Serial.print(",");
  Serial.print(GyY);
  Serial.print(",");
  Serial.print(GyZ);
  Serial.print(",");
  Serial.println(distance_cm, 1);

  delay(100); // 10Hz
}