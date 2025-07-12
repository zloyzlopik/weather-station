# 🌤️ Погодная станция на NodeMCU ESP8266 + DHT22 + SSD1306
<img src="images/GIf oled.gif" alt="Погода станция" width="220" align="right" />

Простая погодная станция, которая:
- Подключается к Wi-Fi
- Получает данные о погоде через OpenWeatherMap API
- Отображает температуру, влажность, давление, скорость ветра
- Выводит время восхода и заката
- Использует дисплей SSD1306 (I2C)
- Может отображать данные с датчика DHT22

---

## 📦 Компоненты

| **Lolin NodeMCU v3** | ESP8266 (ESP-12F), Wi-Fi |

| **DHT22** | Датчик температуры и влажности |

| **SSD1306 OLED 128x64** | Дисплей I2C |

## 🧪 Схема подключения

<img src="images/wiring-diagram.png" width="600" /> 

### DHT22
| NodeMCU | DHT22     |
|--------|-----------|
| 3V3    | VCC       |
| GND    | GND       |
| D4     | DATA      |


### SSD1306 (I2C)
| NodeMCU | OLED      |
|--------|-----------|
| 3V3    | VCC       |
| GND    | GND       |
| D1     | SCL       |
| D2     | SDA       |

---

## 📦 Библиотеки проекта

Для работы проекта требуются следующие библиотеки:

| Название | Автор / Источник | Описание |
|----------|------------------|----------|
| [`ESP8266WiFi`](https://github.com/esp8266/Arduino ) | [ESP8266 Community](https://github.com/esp8266/Arduino ) | Работа с Wi-Fi для ESP8266 |
| [`ESP8266HTTPClient`](https://github.com/esp8266/Arduino ) | [ESP8266 Community](https://github.com/esp8266/Arduino ) | Выполнение HTTP-запросов |
| [`Wire`](https://www.arduino.cc/en/Reference/Wire ) | Arduino | Работа с шиной I2C |
| [`Adafruit_SSD1306`](https://github.com/adafruit/Adafruit_SSD1306 ) | [Adafruit Industries](https://github.com/adafruit ) | Управление OLED дисплеем SSD1306 |
| [`Adafruit_GFX`](https://github.com/adafruit/Adafruit-GFX-Library ) | [Adafruit Industries](https://github.com/adafruit ) | Графическая библиотека для отрисовки |
| [`DHT sensor library`](https://github.com/adafruit/DHT-sensor-library ) | [Adafruit Industries](https://github.com/adafruit ) | Чтение данных с DHT11/DHT22 |
| [`ArduinoJson`](https://arduinojson.org ) | [Benoit Blanchon](https://github.com/bblanchon/ArduinoJson ) | Парсинг JSON-ответов |


## 📁 Установка


1. Установите ядро **ESP8266** через Boards Manager:
   - `Tools → Board: "Boards Manager" → Поиск: "esp8266" → Установите "ESP8266 by ESP8266 Community"`
2. Установите библиотеки:
   - `ESP8266WiFi`
   - `ESP8266HTTPClient`
   - `Wire`
   - `Adafruit_SSD1306`
   - `Adafruit_GFX`
   - `DHT sensor library` 
   - `ArduinoJson` 

3.Скачайте и откройте [проект](sketch.ino) в Arduino IDE.

> 💡 Убедитесь, что вы выбрали правильную плату:  
> `Generic ESP8266 Module` или `LOLIN(WEMOS) D1 R2 & mini`

4. В скетче замените на ваши параметры:
   - `ssid` и `password`                   на свои логин и пароль wifi
   - `apiKey`                              на свой ключ с [OpenWeatherMap](https://openweathermap.org/api )
   - `const String city = "Moscow"`        на ваш город
   - `const String countryCode = "RU"`     на ISO-код вашей страны
  
6. Загрузите скетч на плату

---
