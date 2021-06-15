#define BTN_PIN 4   // кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define BTN_PIN2 5   // кнопка подключена сюда (BTN_PIN --- КНОПКА --- GND)
#define STRIP_PIN 2     // пин ленты
#define NUMLEDS 8      // кол-во светодиодов
#define COLOR_DEBTH 3
#define COLOR_ORDER GRB
#include <microLED.h>
#include <FastLEDsupport.h> // вкл поддержку
#include <AnalogKey.h>
#include <GyverButton.h>

// ленты
microLED<NUMLEDS, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip;
microLED<0, STRIP_PIN, MLED_NO_CLOCK, LED_WS2818, ORDER_GRB, CLI_AVER> strip2;

// кнопки
GButton butt1(BTN_PIN);
GButton butt2(BTN_PIN2);

// сообщение в азбуке Морзе
int lettersMorze[] = {
  800, 800, 800, 1,
  2000, 2000, 2000, 1,
  800, 800, 800, 1
};

// иные глобальные переменные
int modeLed = 0; // режим подсветки по умолчанию
bool enableBreathing = false; // включить или выключить плавную смену интенсивности подсветки
int siz = sizeof(lettersMorze) / 2; // всего длина массива для перезапуска поздравления
int startMorze = 1; // шаг начала поздравления
int step = 0; // шаг отображения букв поздравления (элемент массива)
int longPause = 1; // шаг отображения пазы поздравления
bool nextStep = true; // следующий шаг поздравления после пазузы (нужно для одинаковой длины пауз и включения подсветки)
bool nextColor = false; // циклическая смена цвета

DEFINE_GRADIENT_PALETTE( heatmap_gp ) {   // делаем палитру огня
  0,     0,  0,  0,     // black
  128,   255,  0,  0,   // red
  224,   255, 255,  0,  // bright yellow
  255,   255, 255, 255  // full white
};
CRGBPalette16 fire_p = heatmap_gp;
void setup() {
  Serial.begin(9600);
  Serial.println(sizeof(lettersMorze) / 2);
  pinMode(4, INPUT_PULLUP);
}

void loop() {
  static byte count = 0;
  static int count2 = 0;
  count++;
  count2 += 10;
  butt1.tick(); // опрос кнопок
  butt2.tick();
  if (modeLed == 0 || modeLed == 1 || modeLed == 2 || modeLed == 3) {
    strip2.setBrightness(200);
  } else {
    strip.setBrightness(200);
  }
  if (butt1.isStep()) { // проверка на длинный клик
    modeLed = 0;
  }

  if (butt1.hasClicks()) { // проверка на короткий клик первой кнопкой
    int clk = butt1.getClicks();
    switch (clk) {
      case 1:
        modeLed = 1;
        break;
      case 2:
        modeLed = 2;
        break;
      case 3:
        modeLed = 3;
        break;
      case 4:
        modeLed = 4;
        break;
      case 5:
        modeLed = 5;
        break;
      case 6:
        modeLed = 6;
        break;
      case 9:
        modeLed = 10;
        break;
    }
  }
  
  if (butt2.hasClicks()) { // проверка на короткий клик второй кнопкой
    int clk2 = butt2.getClicks();
    switch (clk2) {
      case 1:
        enableBreathing = true;
        break;
      case 2:
        enableBreathing = false;
        break;
      default:
        enableBreathing = false;
        break;
        //        if (!enableBreathing) {
        //          enableBreathing = true;
        //        } else {
        //          enableBreathing = false;
        //        }
    }
  }

  if (modeLed == 10) {
    Serial.println("morze");
    // начало поздравления
    switch (startMorze) {
      case 1:
        strip.fillGradient(0, 8, mRed, mWhite);
        startMorze++;
        strip.show();
        delay(1000);
        break;
      case 2:
        strip.fillGradient(0, 8, mWhite, mBlue);
        startMorze++;
        strip.show();
        delay(800);
        break;
      case 3:
        strip.fillGradient(0, 8, mBlue, mYellow);
        startMorze++;
        strip.show();
        delay(600);
        break;
      case 4:
        strip.fillGradient(0, 8, 0, 0);
        startMorze++;
        strip.show();
        delay(2000);
        break;
    }
    // вывод букв в формате телеграфного кода
    if (startMorze == 5) {
      // белый цвет - конец поздравления
      if (step == siz) {
        Serial.println("reload");
        strip.fillGradient(0, 8, mWhite, mWhite);
        strip.show();
        nextStep = false;
        step = 0;
        startMorze = 1;
        delay(5000);
      }
      // вывод буквы
      if (lettersMorze[step] != 1 && nextStep == true) {
        Serial.println("signal");
        if (nextColor == false) {
          strip.fillGradient(0, 8, mRed, mRed);
        } else {
          strip.fillGradient(0, 8, mPurple, mPurple);
        }
        strip.show();
        nextStep = false;
        delay(lettersMorze[step]);
      }
      // вывод паузы между буквами
      if (lettersMorze[step] == 1) {
        Serial.println("pause long");
        if (longPause == 1) {
          longPause++;
          strip.fillGradient(0, 8, 0, 0);
          strip.show();
          if (nextColor == false) {
            nextColor = true;
          } else {
            nextColor = false;
          }
          delay(200);
        }
        if (longPause == 2) {
          longPause++;
          strip.fillGradient(0, 8, 0, 0);
          strip.show();
          delay(1000);
        }
        if (longPause == 3) {
          longPause = 1;
          strip.fillGradient(0, 8, 0, 0);
          strip.show();
          nextStep = true;
          ++step;
          delay(lettersMorze[step]);
        }
      }
      // вывод паузы между словом
      if (lettersMorze[step] == 2) {
        Serial.println("next word");
        if (longPause == 1) {
          longPause++;
          strip.fillGradient(0, 8, 0, 0);
          strip.show();
          if (nextColor == false) {
            nextColor = true;
          } else {
            nextColor = false;
          }
          delay(200);
        }
        if (longPause == 2) {
          longPause++;
          strip.fillGradient(0, 8, mYellow, mYellow);
          strip.show();
          delay(6000);
        }
        if (longPause == 3) {
          longPause = 1;
          strip.fillGradient(0, 8, 0, 0);
          strip.show();
          nextStep = true;
          ++step;
          delay(lettersMorze[step]);
        }
      }
      // вывод паузы между единичным знаком (точкой или тире)
      if (nextStep == false) {
        int k = step - 1;
        Serial.println("pause");
        strip.fillGradient(0, 8, 0, 0);
        strip.show();
        nextStep = true;
        delay(lettersMorze[step]);
        ++step;
      }
    }
  }

  if (modeLed == 0) {
    step = 0;
    startMorze = 1;
    Serial.println("start");
    strip2.begin();
    for (int i = 0; i < 50; i++) {
      strip2.send(CRGBtoData(ColorFromPalette(PartyColors_p, count + i * 10, 255, LINEARBLEND)));
    }
    strip2.end();
    delay(40);
  }

  if (modeLed == 1) {
    step = 0;
    startMorze = 1;
    Serial.println("Single");
    strip.begin();
    for (int i = 0; i < 50; i++) {
      strip2.send(CRGBtoData(ColorFromPalette(RainbowStripeColors_p, count + i * 5, 255, LINEARBLEND)));
    }
    strip.end();
    delay(40);
  }

  if (modeLed == 2) {
    step = 0;
    startMorze = 1;
    Serial.println("Double");
    strip.begin();
    for (int i = 0; i < 50; i++) {
      strip2.send(CRGBtoData(ColorFromPalette(RainbowColors_p, inoise8(i * 20, count2), 255, LINEARBLEND)));
    }
    strip.end();
    delay(35);
  }

  if (modeLed == 3) {
    step = 0;
    startMorze = 1;
    Serial.println("Triple");
    static int count = 0;
    count += 10;
    strip.begin();
    for (int i = 0; i < 50; i++) {
      strip.send(CRGBtoData(ColorFromPalette(fire_p, inoise8(i * 25, count), 255, LINEARBLEND)));
    }
    strip.end();
    delay(30);
  }

  if (modeLed == 4) {
    step = 0;
    startMorze = 1;
    Serial.println("runningDots");
    if (enableBreathing)
    {
      breathing();
      strip.show();
    }
    static byte counter = 0;
    // перемотка буфера со сдвигом (иллюзия движения пикселей)
    for (int i = 0; i < NUMLEDS - 1; i++) strip.leds[i] = strip.leds[i + 1];

    // каждый третий вызов - последний пиксель красным, иначе чёрным
    if (counter % 3 == 0) strip.leds[NUMLEDS - 1] = mRed;
    else strip.leds[NUMLEDS - 1] = mBlack;
    counter++;
    delay(120);
  }

  if (modeLed == 5) {
    step = 0;
    startMorze = 1;
    Serial.println("colorCycle");
    static byte counter = 0;
    strip.fill(mWheel8(counter));
    counter += 3;
    delay(70);
  }

  if (modeLed == 6) {
    step = 0;
    startMorze = 1;
    Serial.println("filter");
    static int rev = 1;
    if (rev == 1) {
      static int counter = 7;
      strip.clear();
      strip.fill(0, counter, mRed);
      counter--;
      if (counter < -2) {
        counter = 7;
        rev = 0;
      }
    }
    if (rev == 0) {
      static int counter = -1;
      strip.fill(0, counter, mBlue);
      counter++;
      if (counter >= NUMLEDS) {
        counter = 0;
        rev = 1;
      }
    }
    delay(1000);
  }

  if (modeLed == 0 || modeLed == 1 || modeLed == 2 || modeLed == 3 ) {
    if (modeLed != 10) {
      strip2.show();
    }
  } else {
    if (modeLed != 10) {
      strip.show();
    }
  }

}

void breathing() {
  static int dir = 1;
  static int bright = 0;
  bright += dir * 5;    // 5 - множитель скорости изменения

  if (bright > 255) {
    bright = 255;
    dir = -1;
  }
  if (bright < 0) {
    bright = 0;
    dir = 1;
  }
  if (modeLed == 0 || modeLed == 1 || modeLed == 2 || modeLed == 3) {
    strip2.setBrightness(bright);
  } else {
    strip.setBrightness(bright);
  }

}
