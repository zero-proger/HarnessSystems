#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <GyverEncoder.h>

// пины энкодера
#define ENC_A D5   // GPIO пин для вывода A энкодера 27
#define ENC_B D4  // GPIO пин для вывода B энкодера 26
#define ENC_SW D3 // GPIO пин для вывода нажатия энкодера 23

Encoder enc(ENC_A, ENC_B, ENC_SW, TYPE2); // для работы без кнопки и сразу выбираем тип

// пины шагового мотора
#define STEPPER_DIR D8  // D8
#define STEPPER_STEP D7 // D7
#define STEPPER_STEPS 200 // количество шагов на оборот

// LCD дисплей 1602 I2C
#define LCD_ADDRESS 0x27
#define LCD_COLUMNS 16
#define LCD_LINES 2

LiquidCrystal_I2C LCD(LCD_ADDRESS, LCD_COLUMNS, LCD_LINES);

u_int16_t stepperSpeed = 1200;
bool stepperEnabled = true;
bool step = true;

u_int64_t Timer0 = millis();
u_int64_t Timer1 = millis();
u_int64_t Timer2 = micros();

#define _DEBUG
#ifdef _DEBUG
#define PRINT(x) Serial.print(x)
#define PRINTLN(x) Serial.println(x)
#endif

void updateLCD(void);

void setup(void)
{
  Serial.begin(9600);
  LCD.init();
  LCD.backlight();
  pinMode(ENC_A, INPUT);
  pinMode(ENC_B, INPUT);
  pinMode(ENC_SW, INPUT);
  pinMode(STEPPER_STEP, OUTPUT);
  pinMode(STEPPER_DIR, OUTPUT);
}

void moveSteps(int steps, int direction, int speed)
{
  // Установить направление вращения
  digitalWrite(STEPPER_DIR, direction);
  int SPM = speed * STEPPER_STEPS;
  int SPS = SPM / 60;
  int del = 1000 / SPS;
  // Отправить необходимое количество импульсов STEP
  for (int i = 0; i < steps; i++)
  {
    digitalWrite(STEPPER_STEP, HIGH);
    delayMicroseconds(del * 1000 / 2); 
    digitalWrite(STEPPER_STEP, LOW);
    delayMicroseconds(del * 1000 / 2); 
  }
}

void loop()
{
  enc.tick();

  if (stepperEnabled)
  {
    moveSteps(200, HIGH, stepperSpeed);
  }

  if (enc.isLeft())
  {
    PRINT("LEFT: ");
    PRINTLN(stepperSpeed);
    stepperSpeed += 10;
    if (stepperSpeed > 1000)
    {
      stepperSpeed = 1000;
    }
  }
  
  if (enc.isRight())
  {
    stepperSpeed -= 10;
    PRINT("RIGHT: ");
    PRINTLN(stepperSpeed);
    if (stepperSpeed < 100)
    {
      stepperSpeed = 100;
    }
  }

  // Обработка нажатия кнопки
  if (enc.isPress() && millis() - Timer0 > 10)
  {
    Timer0 = millis();
    Serial.println("Button pressed!");
    stepperEnabled = !stepperEnabled;
  }

  if (millis() - Timer1 > 300)
  {
    Timer1 = millis();
    updateLCD();
  }
}

void updateLCD()
{
  PRINTLN("UPDATE LCD");
  LCD.clear();
  LCD.print("Stepper Speed:");
  LCD.setCursor(0, 1);
  LCD.print(stepperSpeed);
  LCD.print(" rmp/s ");
  LCD.print(stepperEnabled == 1 ? "ON" : "OFF");
}