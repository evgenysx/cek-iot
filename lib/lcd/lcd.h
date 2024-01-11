
#ifndef _CEK_ESP_LCD_H_
#define _CEK_ESP_LCD_H_

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4); 

void initLCD(){
  #define I2C_SDA 21
  #define I2C_SCL 22

  Wire.begin(I2C_SDA, I2C_SCL);

  lcd.init();
  lcd.clear();
  lcd.backlight();
}
#endif