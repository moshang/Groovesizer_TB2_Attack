// *** CUSTOM CHARACTERS ***
//boolean arrowAnimation = false;
byte arrowX = 0;
byte arrowY = 0;
byte arrowFrame = 0;

byte arrow1[8] = {
  0b00000,
  0b00100,
  0b01100,
  0b11111,
  0b01100,
  0b00100,
  0b00000,
  0b00000
};

byte arrow2[8] = {
  0b00000,
  0b00000,
  0b01100,
  0b11111,
  0b01100,
  0b00000,
  0b00000,
  0b00000
};

byte arrow3[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b11111,
  0b00100,
  0b00000,
  0b00000,
  0b00000
};

byte arrow4[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte Tee1[8] = {
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b10101,
  0b10101,
  0b10101,
  0b00100
};

byte Tee2[8] = {
  0b00100,
  0b00100,
  0b00100,
  0b00100,
  0b01110,
  0b01110,
  0b01110,
  0b01110
};

byte Bee1[8] = {
  0b11110,
  0b11110,
  0b11111,
  0b11111,
  0b01011,
  0b01001,
  0b01011,
  0b01110
};

byte Bee2[8] = {
  0b01110,
  0b01011,
  0b01001,
  0b01011,
  0b11111,
  0b11111,
  0b11110,
  0b11110
};

byte Two1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b11011,
  0b11011
};

byte Two2[8] = {
  0b10011,
  0b00111,
  0b01110,
  0b11100,
  0b11001,
  0b11111,
  0b11111,
  0b11111
};

byte Note[8] = {
  0b00100,
  0b00110,
  0b00101,
  0b00100,
  0b10100,
  0b01000,
  0b10100,
  0b00000
};

byte Rest[8] = {
  0b11111,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b11111,
  0b00000
};

byte AccentFlam[8] = {
  0b11100,
  0b10100,
  0b11100,
  0b10111,
  0b00100,
  0b00110,
  0b00100,
  0b00000
};

byte Occupied[8] = {
  0b11111,
  0b11011,
  0b10101,
  0b01110,
  0b10101,
  0b11011,
  0b11111,
  0b00000
};

void customCharacters()
{
  lcd.createChar(0, arrow1);
  lcd.createChar(1, arrow2);
  lcd.createChar(2, arrow3);
  lcd.createChar(3, arrow4);
  lcd.createChar(4, Note);
  lcd.createChar(5, Rest);
  lcd.createChar(6, AccentFlam);
  lcd.createChar(7, Occupied);
}

void showValue(byte h, byte v, int number) // a function to help us deal with displaying numbers of varying lengths
{
  if (number < 10)
  {
    lcd.setCursor(h, v);
    lcd.print(number);
    lcd.print("   ");
  }
  else if (number < 100)
  {
    lcd.setCursor(h, v);
    lcd.print(number);
    lcd.print("  ");
  }
  else
  {
    lcd.setCursor(h, v);
    lcd.print(number);
    lcd.print(" ");
  }
}

void showTB2(byte x) // horizontal position
{
  lcd.setCursor(x, 0);
  lcd.write(2);
  lcd.write(4);
  lcd.write(6);
  lcd.setCursor(x, 1);
  lcd.write(3);
  lcd.write(5);
  lcd.write(7);
}

void arrow(byte x, byte y)
{
  arrowAnimation = true;
  arrowX = x;
  arrowY = y;
}

void arrowAnim()
{
  static byte lastArrowFrame = 255; // can't initialize to 0
  byte arrowMap[10] = {0, 0, 0, 0, 0, 1, 2, 3, 2, 1}; // the sequence of frames for the arrow animation
  if (arrowAnimation && arrowFrame != lastArrowFrame)
  {
    lastArrowFrame = arrowFrame;
    lcd.setCursor(arrowX, arrowY);
    switch (arrowMap[arrowFrame])
    {
      case 0:
        lcd.write(byte(0));
        break;
      case 1:
        lcd.write((byte) 1);
        break;
      case 2:
        lcd.write(2);
        break;
      case 3:
        lcd.write(3);
        break;
    }
  }
}

void displayHandler()
{
  arrowFrame = (arrowFrame < 9) ? arrowFrame + 1 : 0; // advance the arrow animation frame
  uiRefresh = true; // allow a screen refresh
}

