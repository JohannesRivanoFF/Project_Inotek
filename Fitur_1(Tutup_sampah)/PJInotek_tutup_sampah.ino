/*
 * =============================================
 * FITUR 1: Deteksi Tangan + Servo Buka/Tutup
 * Anggota 1 : Johannes Rivano Fashella Febriyanto
 * =============================================
 */

#include <Servo.h>

#define TRIG_PIN_TANGAN  7
#define ECHO_PIN_TANGAN  6
#define SERVO_PIN        9

const float JARAK_AMBANG = 10.0;  // cm
const int DELAY_TUTUP = 500;      // ms (0,5 detik)

Servo servo;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN_TANGAN, OUTPUT);
  pinMode(ECHO_PIN_TANGAN, INPUT);
  servo.attach(SERVO_PIN);
  servo.write(0); // tutup
  Serial.println("Fitur 1: Deteksi Tangan + Servo siap.");
}

void loop() {
  float jarak = bacaJarakAman(TRIG_PIN_TANGAN, ECHO_PIN_TANGAN);
  if (jarak <= JARAK_AMBANG && jarak > 0) {
    Serial.println(">> Tangan terdeteksi! Buka tutup.");
    servo.write(180);
    delay(DELAY_TUTUP);
    servo.write(0);
    Serial.println(">> Tutup tertutup.");
  }
  delay(100); // kecil agar tidak terlalu cepat
}

// Fungsi baca jarak dengan timeout aman
float bacaJarakAman(int trig, int echo) {
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);

  unsigned long startTime = micros();
  while (digitalRead(echo) == LOW) {
    if (micros() - startTime > 30000) return -1;
  }
  startTime = micros();
  while (digitalRead(echo) == HIGH) {
    if (micros() - startTime > 30000) return -1;
  }
  long duration = micros() - startTime;
  return (duration * 0.034) / 2;
}