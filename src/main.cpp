#include <ESP8266WiFi.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include <time.h>

// --- Wi-Fi Settings ---
const char *ssid = "ВАША_НАЗВА_WIFI";
const char *password = "ВАШ_ПАРОЛЬ_WIFI";

// --- Hardware Settings ---
// ИСПРАВЛЕНО: Изменено MD_MAX7219 на MD_MAX72XX
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 2

#define CLK_PIN 14  // Hardware SPI Clock (GPIO14)
#define DATA_PIN 13 // Hardware SPI Data (GPIO13)
#define CS_PIN 2    // Chip Select Line (GPIO2)

// Инициализация дисплея
MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// --- Настройка времени для Украины ---
const char *TZ_INFO = "EET-2EEST,M3.5.0/3,M10.5.0/4";

void setup()
{
  Serial.begin(115200);

  display.begin();
  display.setIntensity(2);
  display.displayClear();

  // НАЛАШТУВАННЯ АНІМАЦІЇ: текст повзе вліво (PA_SCROLL_LEFT), швидкість 80мс, пауза 1 секунда
  display.displayText("WiFi", PA_CENTER, 150, 500, PA_SCROLL_LEFT, PA_SCROLL_LEFT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }

  configTime(TZ_INFO, "pool.ntp.org", "time.nist.gov");
}

void loop()
{
  static uint32_t lastNTPCheck = 0;
  static char timeString[9] = ""; // Буфер для "  20:52  "

  // Анімація Parola має викликатися якомога частіше в loop
  if (display.displayAnimate())
  {
    // Коли попередня анімація завершилася, формуємо новий рядок із часом
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);

    if (timeinfo->tm_year > 70)
    {
      // Додаємо пробіли з боків, щоб текст гарно випливав і запливав
      strftime(timeString, sizeof(timeString), " %H:%M ", timeinfo);
      display.setTextBuffer(timeString);
    }
    else
    {
      display.setTextBuffer("Sync");
    }

    display.displayReset(); // Перезапуск анімації з новим часом
  }
}
