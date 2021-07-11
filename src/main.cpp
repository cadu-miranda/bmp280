/*
  :: BMP280 ::

  Tensão de Operação: 1.7 - 3.6 V
  Consumo de Corrente: 2.7 µA
  Faixa de Temperatura: -40 à 85 °C
  Precisão da Temperatura: ±1.0 °C
  Faixa de Medição da Pressão: 300 - 1100 hPa (equiv. +9000 à -500 m
  acima/abaixo do nível do mar) Precisão: ±0.12 hPa (equiv. ±1 m) Interfaces:
  I2C e SPI Dimensões: 15 x 12 x 2,3 mm (sem os pinos)
*/

#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_BMP280 bmp;

const unsigned int intervalo = 10000;
unsigned long millisAnterior = 0UL;
byte estado = 0;

byte aAtilde[8] = {0b01110, 0b00000, 0b01110, 0b00001,
                   0b01111, 0b10001, 0b01111};

// Protótipo das funções

void mostraInicio(void);
void mostraTemperatura(void);
void mostraPressao(void);
void mostraAltitude(void);

void setup(void) {

  Serial.begin(9600);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);

  lcd.createChar(0, aAtilde);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  if (!bmp.begin(0x76)) {

    Serial.println("Sensor BMP280 não identificado! Verifique as conexões e/ou "
                   "o endereço I2C.");
    while (1)
      ;
  }

  mostraInicio();
}

void loop(void) {

  unsigned long millisAtual = millis();

  if ((millisAtual - millisAnterior) >= intervalo) {

    millisAnterior = millisAtual;

    switch (estado) {

    case 0:

      mostraTemperatura();
      estado++;
      break;

    case 1:

      mostraPressao();
      estado++;
      break;

    case 2:

      mostraAltitude();
      estado = 0;
    }
  }
}

void mostraTemperatura(void) {

  float temperatura = bmp.readTemperature();

  if (temperatura < (-40.0f) || temperatura > 85.0f) {

    lcd.setCursor(0, 0);
    lcd.print("  Temperatura:  ");
    lcd.setCursor(0, 1);
    lcd.print("       - ");
    lcd.setCursor(9, 1);
    lcd.print((char)223);
    lcd.print("C");
    lcd.print("                ");
  }

  else {

    lcd.setCursor(0, 0);
    lcd.print("  Temperatura:  ");
    lcd.setCursor(0, 1);
    lcd.print("    ");
    lcd.print(temperatura, 1);
    lcd.print("  ");
    lcd.setCursor(9, 1);
    lcd.print((char)223);
    lcd.print("C");
    lcd.print("                ");
  }
}

void mostraPressao(void) {

  int pressao = (bmp.readPressure() / 100) + 50;

  if (pressao < 300.0f || pressao > 1100.0f) {

    lcd.setCursor(0, 0);
    lcd.print("    Press");
    lcd.setCursor(9, 0);
    lcd.write(byte(0));
    lcd.print("o:      ");
    lcd.setCursor(0, 1);
    lcd.print("      - hPa     ");
  }

  else {

    lcd.setCursor(0, 0);
    lcd.print("    Press");
    lcd.setCursor(9, 0);
    lcd.write(byte(0));
    lcd.print("o:      ");
    lcd.setCursor(0, 1);
    lcd.print("    ");
    lcd.print(pressao);
    lcd.print(" hPa");
    lcd.print("                ");
  }
}

void mostraAltitude(void) {

  int altitude = bmp.readAltitude(1024.2f);

  lcd.setCursor(0, 0);
  lcd.print("  Alt. aprox.:  ");
  lcd.setCursor(0, 1);
  lcd.print("     ");
  lcd.print(altitude);
  lcd.print(" m");
  lcd.print("                ");
}

void mostraInicio(void) {

  lcd.setCursor(0, 0);
  lcd.print(" Monitoramento  ");
  lcd.print("                ");

  lcd.setCursor(0, 1);
  lcd.print("     BMP280     ");
  lcd.print("                ");
}
