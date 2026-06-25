/*
 * =============================================
 * FITUR 3: Alarm Buzzer + Notifikasi Penuh
 * Anggota 3
 * =============================================
 */

#define TRIG_PIN_VOLUME  5
#define ECHO_PIN_VOLUME  4
#define BUZZER_PIN       8

const float TINGGI_MAX_SAMPAH = 25.0;
const int AMBANG_PENUH = 80;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN_VOLUME, OUTPUT);
  pinMode(ECHO_PIN_VOLUME, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println("Fitur 3: Alarm & Notifikasi siap.");
}

void loop() {
  static unsigned long waktuSebelumnya = 0;
  if (millis() - waktuSebelumnya >= 1000) {
    waktuSebelumnya = millis();

    float jarak = bacaJarakAman(TRIG_PIN_VOLUME, ECHO_PIN_VOLUME);
    if (jarak < 0) {
      Serial.println("⚠ Error baca sensor volume!");
      return;
    }

    int persen = hitungPersentase(jarak);

    if (persen >= AMBANG_PENUH) {
      // Buzzer bunyi 3 kali
      for (int i = 0; i < 3; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(300);
        digitalWrite(BUZZER_PIN, LOW);
        delay(300);
      }
      Serial.println(" ⚠⚠⚠ PERINGATAN: TEMPAT SAMPAH PENUH! ⚠⚠⚠");
      Serial.println(" Segera kosongkan tempat sampah!");
    }
  }
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

int hitungPersentase(float jarak) {
  if (jarak < 0 || jarak > TINGGI_MAX_SAMPAH) return 0;
  float persen = ((TINGGI_MAX_SAMPAH - jarak) / TINGGI_MAX_SAMPAH) * 100;
  if (persen < 0) persen = 0;
  if (persen > 100) persen = 100;
  return (int)persen;
}