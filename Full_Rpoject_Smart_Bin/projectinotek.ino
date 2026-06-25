/*
 * =====================================================
 * PROYEK : Smart Trash Bin (Dengan Watchdog Timer)
 * =====================================================
 */

#include <Servo.h>
#include <avr/wdt.h>   // ← Tambahkan ini!

#define TRIG_PIN_TANGAN  7
#define ECHO_PIN_TANGAN  6
#define TRIG_PIN_VOLUME  5
#define ECHO_PIN_VOLUME  4
#define SERVO_PIN        9
#define BUZZER_PIN       8

const float JARAK_AMBANG_TANGAN = 20.0;
const float TINGGI_MAX_SAMPAH   = 19.65;
const int   AMBANG_PENUH        = 80;
const int   DELAY_TUTUP         = 500;

Servo servo;
float jarakTangan, jarakVolume;
int persentase;
char statusSampah[16];

bool tutupSedangBuka = false;
unsigned long waktuBukaMulai = 0;

void setup() {
    wdt_enable(WDTO_4S);  // ← Aktifkan watchdog 4 detik
    
    Serial.begin(115200);
    pinMode(TRIG_PIN_TANGAN, OUTPUT);
    pinMode(ECHO_PIN_TANGAN, INPUT);
    pinMode(TRIG_PIN_VOLUME, OUTPUT);
    pinMode(ECHO_PIN_VOLUME, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);
    servo.attach(SERVO_PIN);
    servo.write(0);
    Serial.println("=== SMART TRASH BIN READY (dengan WDT) ===");
}

void loop() {
    wdt_reset();  // ← Reset watchdog setiap loop

    // ---------- DETEKSI TANGAN (Non-blocking) ----------
    if (!tutupSedangBuka) {
        jarakTangan = bacaJarakAman(TRIG_PIN_TANGAN, ECHO_PIN_TANGAN);
        if (jarakTangan <= JARAK_AMBANG_TANGAN && jarakTangan > 0) {
            mulaiBuka();
        }
    } else {
        if (millis() - waktuBukaMulai >= DELAY_TUTUP) {
            tutupTutup();
        }
    }

    // ---------- BACA VOLUME (setiap 1 detik) ----------
    static unsigned long waktuSebelumnya = 0;
    if (millis() - waktuSebelumnya >= 1000) {
        waktuSebelumnya = millis();
        bacaDanTampilkanVolume();
    }
}

// ===== FUNGSI BACA JARAK DENGAN TIMEOUT AMAN =====
float bacaJarakAman(int trig, int echo) {
    // Kirim trigger
    digitalWrite(trig, LOW);
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    
    // Baca echo dengan timeout
    unsigned long startTime = micros();
    while (digitalRead(echo) == LOW) {
        if (micros() - startTime > 30000) return -1;  // Timeout 30ms
    }
    startTime = micros();
    while (digitalRead(echo) == HIGH) {
        if (micros() - startTime > 30000) return -1;  // Timeout 30ms
    }
    long duration = micros() - startTime;
    
    return (duration * 0.034) / 2;
}

// ===== FUNGSI LAINNYA SAMA =====
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

void mulaiBuka() {
    tutupSedangBuka = true;
    waktuBukaMulai = millis();
    Serial.println(">> Buka tutup");
    servo.write(270);
    digitalWrite(LED_BUILTIN, HIGH);
}

void tutupTutup() {
    tutupSedangBuka = false;
    servo.write(0);
    digitalWrite(LED_BUILTIN, LOW);
    Serial.println(">> Tutup tertutup");
}

void bunyiBuzzer(int kali) {
    for (int i = 0; i < kali; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(300);
        digitalWrite(BUZZER_PIN, LOW);
        delay(300);
    }
}

void bacaDanTampilkanVolume() {
    jarakVolume = bacaJarakAman(TRIG_PIN_VOLUME, ECHO_PIN_VOLUME);
    if (jarakVolume < 0) {
        Serial.println("⚠ Error sensor volume");
        return;
    }
    persentase = hitungPersentase(jarakVolume);
    dapatkanStatus(persentase, statusSampah);

    Serial.println("========================================");
    Serial.println("          📊 MONITORING SAMPAH");
    Serial.println("========================================");
    Serial.print("📏 Jarak: "); Serial.print(jarakVolume); Serial.println(" cm");
    Serial.print("📊 Persentase: "); Serial.print(persentase); Serial.println(" %");
    Serial.print("📌 Status: "); Serial.println(statusSampah);
    Serial.println("========================================");

    if (persentase >= AMBANG_PENUH) {
        bunyiBuzzer(3);
        Serial.println(" ⚠⚠⚠ SAMPAH PENUH! ⚠⚠⚠");
    }
}