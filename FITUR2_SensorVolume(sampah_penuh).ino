/*
 * =============================================
 * FITUR 2: Sensor Volume + Monitoring Persentase
 * Anggota 2
 * =============================================
 */

#define TRIG_PIN_VOLUME  5
#define ECHO_PIN_VOLUME  4

const float TINGGI_MAX_SAMPAH = 25.0;  // cm (sesuaikan)

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN_VOLUME, OUTPUT);
  pinMode(ECHO_PIN_VOLUME, INPUT);
  Serial.println("Fitur 2: Monitoring Volume siap.");
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
    char status[16];
    dapatkanStatus(persen, status);

    Serial.println("========================================");
    Serial.println("          📊 MONITORING SAMPAH");
    Serial.println("========================================");
    Serial.print("📏 Jarak ke sampah   : ");
    Serial.print(jarak);
    Serial.println(" cm");
    Serial.print("📊 Persentase        : ");
    Serial.print(persen);
    Serial.println(" %");
    Serial.print("📌 Status            : ");
    Serial.println(status);
    Serial.println("========================================");
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

void dapatkanStatus(int persen, char* output) {
  if (persen <= 30) strcpy(output, "Kosong");
  else if (persen <= 60) strcpy(output, "Setengah Penuh");
  else if (persen <= 79) strcpy(output, "Hampir Penuh");
  else strcpy(output, "PENUH!");
}