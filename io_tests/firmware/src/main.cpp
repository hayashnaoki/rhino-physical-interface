// MPU-6050 and HC-SR04 Combined Test
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