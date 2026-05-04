#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
const char* ssid = "";
const char* password = "";
const char* discord_webhook = "";
const char* scriptID = "";
#define TRIG_PIN 5
#define ECHO_PIN 18
#define BIN_HEIGHT 50 // ความลึกถัง (cm)
bool isOverLimit = false;
unsigned long lastNotifyTime = 0;
const unsigned long notifyInterval = 60000; // แจ้งเตือน Discord ซ้ำทุก 1 นาทีถ้าเกิน 80%

unsigned long lastCheckTime = 0;
int lastLevel = -1;
float smoothFillRate = 0.0;
const float alpha = 0.2;
void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected!");
  sendDiscord("🚀 Smart Bin System Online! (WiFi Connected)");
}
void loop() {
  int distance = getAverageDistance();
  // ถ้าเซนเซอร์อ่านค่าไม่ได้ ให้ข้ามรอบนี้ไป
  if (distance <= 0 || distance > BIN_HEIGHT + 10) {
    Serial.println("❌ Sensor Error: อ่านค่าไม่ได้ หรือค่าเพี้ยน");
    delay(2000);
    return;
  }
  // 1. คำนวณระดับขยะ
  int trashHeight = BIN_HEIGHT - distance;
  int level = (trashHeight * 100) / BIN_HEIGHT;
  level = constrain(level, 0, 100);

  unsigned long currentTime = millis();
  String predictionMsg = "รอข้อมูล...";

  // 2. [Prediction & Sheets Update] คำนวณทุก 10 วินาที
  float timeDiffMinutes = (currentTime - lastCheckTime) / 60000.0;

  if (lastLevel == -1) {
    lastLevel = level;
    lastCheckTime = currentTime;
  } 
  else if (timeDiffMinutes >= 0.166) { // ครบประมาณ 10 วินาที
    if (level > lastLevel) {
      float currentRate = (float)(level - lastLevel) / timeDiffMinutes;
      smoothFillRate = (alpha * currentRate) + ((1.0 - alpha) * smoothFillRate);
    } 
    else if (level < lastLevel) {
      smoothFillRate = 0.0; // มีการเอาขยะไปทิ้ง
    }
    // สร้างข้อความพยากรณ์
    if (smoothFillRate > 0) {
      float hoursToFull = ((100.0 - level) / smoothFillRate) / 60.0;
      predictionMsg = (hoursToFull > 24.0) ? "มากกว่า 24 ชม." : String(hoursToFull, 1) + " ชม.";
    } 

    else {
      predictionMsg = "ระดับคงที่";
    }
    // --- ส่งข้อมูลไป Google Sheets ทุก 10 วินาที ---
    sendToGoogleSheets(level, smoothFillRate, predictionMsg);

    lastLevel = level;
    lastCheckTime = currentTime;
  }

  Serial.printf("Level: %d%% | Rate: %.2f | Dist: %d cm\n", level, smoothFillRate, distance);

  // 3. [Discord Alerts]
  if (level >= 80) {
    if (!isOverLimit || (currentTime - lastNotifyTime >= notifyInterval)) {
      String msg = "⚠️ **[Alert >80%]**\\nระดับขยะ: " + String(level) + "%\\nคาดว่าจะเต็มใน: " + predictionMsg;
      sendDiscord(msg);
      isOverLimit = true;
      lastNotifyTime = currentTime;
    }
  }

  if (level <= 20 && isOverLimit) {
    sendDiscord("✨ **[Cleared]** ขยะถูกเคลียร์เรียบร้อยแล้ว!");
    isOverLimit = false;
  }
  delay(2000);
}
void sendDiscord(String message) {
  if (WiFi.status() != WL_CONNECTED) return;
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  
  if (http.begin(client, discord_webhook)) {
    http.addHeader("Content-Type", "application/json");
    String jsonPayload = "{\"content\": \"" + message + "\"}";
    int httpCode = http.POST(jsonPayload);
    Serial.println(">> Discord Response: " + String(httpCode));
    http.end();
  }
}

void sendToGoogleSheets(int level, float rate, String prediction) {
  if (WiFi.status() != WL_CONNECTED) return;
 
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;

  // ปรับ URL ให้รองรับการส่งภาษาไทย (URL Encode)
  String url = "https://script.google.com/macros/s/" + String(scriptID) + "/exec";
  url += "?level=" + String(level);
  url += "&rate=" + String(rate);
  url += "&prediction=" + prediction;
  url.replace(" ", "%20"); // กัน Error กรณีมีช่องว่าง

  if (http.begin(client, url)) {
    int httpCode = http.GET();
    Serial.println(">> Sheets Response: " + String(httpCode));
    http.end();
  }
}

int Ultrasonic(byte trigPin, byte echoPin) {
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH); 
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000);
  return (duration == 0) ? 0 : duration / 58.2;
}

int getAverageDistance() {
  long sum = 0;
  for (int i = 0; i < 5; i++) { 
    sum += Ultrasonic(TRIG_PIN, ECHO_PIN); 
    delay(10); 
  }
  return sum / 5;
}
