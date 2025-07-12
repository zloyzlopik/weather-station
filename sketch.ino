#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>
#include <DHT.h>

// Подключение битмапов из отдельного файла graphics.h
#include "graphics.h" // 

// === Настройки Wi-Fi ===
const char* ssid = "Имя вашей сети Wi-Fi";      // Имя вашей сети Wi-Fi
const char* password = "Пароль от вашей сети";        // Пароль от вашей сети

// === Настройки OpenWeatherMap ===
const String city = "Город";           // Город
const String countryCode = "RU";          // Код страны
const String apiKey = "Ваш API ключ"; // Ваш API ключ
const String apiUrl = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&appid=" + apiKey + "&units=metric&lang=ru";

// === OLED дисплей ===
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// === DHT22 датчик ===
#define DHTPIN 2      // GPIO2 = D4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// Переменные для хранения данных
float internetTemp = 0;
float internetHumidity = 0;
float sensorTemp = 0;
float sensorHumidity = 0;
float internetPressure = 0;     // Давление
float internetWindSpeed = 0;    // Скорость ветра
unsigned long sunriseTime = 0;
unsigned long sunsetTime = 0;

unsigned long lastUpdateTime = 0;
const long updateInterval = 120000; // Обновление раз в 2 минуты

// === Функция получения данных о погоде с защитой от ошибок ===
void getWeatherData(float *temp, float *humidity, float *pressure, float *windSpeed, unsigned long *sunrise, unsigned long *sunset) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi не подключен");
    return;
  }

  WiFiClient client;
  HTTPClient http;

  client.setTimeout(10000); // увеличенный таймаут

  bool success = false;
  for (int i = 0; i < 3 && !success; i++) {
    http.begin(client, apiUrl);
    int httpResponseCode = http.GET();

    Serial.print("HTTP Response Code: ");
    Serial.println(httpResponseCode);

    if (httpResponseCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println("Ответ сервера:");
      Serial.println(payload);

      DynamicJsonDocument doc(2048);
      DeserializationError error = deserializeJson(doc, payload);

      if (!error) {
        *temp = doc["main"]["temp"];
        *humidity = doc["main"]["humidity"];
        *pressure = doc["main"]["pressure"];        // гПа
        *windSpeed = doc["wind"]["speed"];         // м/с
        *sunrise = doc["sys"]["sunrise"];
        *sunset = doc["sys"]["sunset"];

        success = true;
      } else {
        Serial.print("Ошибка парсинга JSON: ");
        Serial.println(error.c_str());
      }
    } else {
      Serial.println("Не удалось получить данные, пробую ещё раз...");
      delay(2000);
    }
    http.end();
    delay(1000);
  }

  if (!success) {
    Serial.println("Не удалось получить данные после нескольких попыток.");
    *temp = 0;
    *humidity = 0;
    *pressure = 0;
    *windSpeed = 0;
    *sunrise = 0;
    *sunset = 0;
  }
}

// === Чтение данных с DHT22 ===
void readSensorData(float *temp, float *humidity) {
  *humidity = dht.readHumidity();
  *temp = dht.readTemperature();

  if (isnan(*humidity) || isnan(*temp)) {
    Serial.println("Ошибка чтения с DHT22!");
    *humidity = 0;
    *temp = 0;
  }

  Serial.print("С датчика: T=");
  Serial.print(*temp);
  Serial.print(" H=");
  Serial.println(*humidity);
}

// === Перевод Unix времени в HH:MM ===
String unixToTime(unsigned long unixTime) {
  unixTime += 10800; // UTC+3
  long hours = (unixTime % 86400L) / 3600;
  long minutes = (unixTime % 3600) / 60;
  char buffer[10];
  sprintf(buffer, "%02d:%02d", hours, minutes);
  return String(buffer);
}

// === Отображение температуры и влажности (Интернет) ===
void showInternetData(float temp, float humidity) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Balakovo");

  if (temp == 0 && humidity == 0) {
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println("No Internet Data");
  } else {
    display.setTextSize(2);
    display.setCursor(0, 20);
    display.print(temp);
    display.println(" C");

    display.setCursor(0, 40);
    display.print(humidity);
    display.println(" %");
  }

  display.display();
}

// === Отображение давления и скорости ветра ===
void showExtraData(float pressure, float windSpeed) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Balakovo");

  if (pressure == 0 && windSpeed == 0) {
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println("No Weather Data");
  } else {
    float pressure_mmHg = pressure / 1.33322;

    display.setTextSize(1);
    display.setCursor(0, 20);
    display.print("Pressure: ");
    display.print(pressure_mmHg, 1);
    display.println(" mmHg");

    display.setCursor(0, 32);
    display.print("Wind: ");
    display.print(windSpeed);
    display.println(" m/s");
  }

  display.display();
}

// === Отображение температуры и влажности (датчик) ===
void showSensorData(float temp, float humidity) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Sensor");

  display.setTextSize(2);
  display.setCursor(0, 20);
  display.print(temp);
  display.println(" C");

  display.setCursor(0, 40);
  display.print(humidity);
  display.println(" %");

  display.display();
}

// === Отображение восхода ===
void showSunrise(unsigned long sunrise) {
  display.clearDisplay();

  // Отображаем битмап восхода
  display.drawBitmap(0, 0, sunriseBitmap, 128, 42, SSD1306_WHITE);

  // Центрируем текст
  String timeStr = "Sunrise: " + unixToTime(sunrise);
  uint8_t textSize = 1;
  uint8_t charWidth = 6 * textSize;
  int16_t strWidth = timeStr.length() * charWidth;
  int16_t textX = (SCREEN_WIDTH - strWidth) / 2;

  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(textX, 50);
  display.println(timeStr);

  display.display();
}

// === Отображение заката ===
void showSunset(unsigned long sunset) {
  display.clearDisplay();

  // Отображаем битмап заката
  display.drawBitmap(0, 0, sunsetBitmap, 128, 42, SSD1306_WHITE);

  // Центрируем текст
  String timeStr = "Sunset: " + unixToTime(sunset);
  uint8_t textSize = 1;
  uint8_t charWidth = 6 * textSize;
  int16_t strWidth = timeStr.length() * charWidth;
  int16_t textX = (SCREEN_WIDTH - strWidth) / 2;

  display.setTextSize(textSize);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(textX, 50);
  display.println(timeStr);

  display.display();
}

// === setup() ===
void setup() {
  Serial.begin(115200);

  dht.begin(); // Инициализация DHT22

  // Подключение к Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Подключение к Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nПодключено!");

  // Инициализация OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Остановить программу
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("loading...");
  display.display();

  // Получаем начальные данные с сайта
  getWeatherData(&internetTemp, &internetHumidity, &internetPressure, &internetWindSpeed, &sunriseTime, &sunsetTime);
  lastUpdateTime = millis();
}

// === loop() ===
void loop() {
  unsigned long currentMillis = millis();

  // === Обновляем данные с сайта  ===
  if (currentMillis - lastUpdateTime >= updateInterval) {
    lastUpdateTime = currentMillis;
    getWeatherData(&internetTemp, &internetHumidity, &internetPressure, &internetWindSpeed, &sunriseTime, &sunsetTime);
  }

  // === Чтение с датчика каждый раз ===
  readSensorData(&sensorTemp, &sensorHumidity);

  // === Показываем данные поочерёдно ===

  
  showSensorData(sensorTemp, sensorHumidity);
  delay(5000);
  
  showInternetData(internetTemp, internetHumidity);
  delay(5000);

  showExtraData(internetPressure, internetWindSpeed);
  delay(5000);

  showSunrise(sunriseTime);
  delay(5000);

  showSunset(sunsetTime);
  delay(5000);
}
