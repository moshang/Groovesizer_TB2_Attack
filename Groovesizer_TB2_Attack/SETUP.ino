void setup() {
  // *** DEBUG ***
  Serial.begin(9600);

  // *** LCD ***
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // create custom characters
  lcd.createChar(2, Tee1);
  lcd.createChar(3, Tee2);
  lcd.createChar(4, Bee1);
  lcd.createChar(5, Bee2);
  lcd.createChar(6, Two1);
  lcd.createChar(7, Two2);

  lcd.setCursor(4, 0);
  lcd.print("ATTACK!");
  lcd.setCursor(4, 1);
  lcd.print("Ver.");
  lcd.setCursor(8, 1);
  lcd.print(versionNumber);
  showTB2(0);

  // *** BUTTONS ***
  // Make input & enable pull-up resistors on switch pins
  for (byte i = 0; i < NUMBUTTONS; i ++)
    pinMode(buttons[i], INPUT_PULLUP);

  // *** SD CARD ***
  if (!sd.begin(chipSelect, SPI_FULL_SPEED))
  {
    lcd.setCursor(0, 1);
    lcd.print("*SD Unavailable*");
  }

  // *** TIMERS ***
  Timer3.attachInterrupt(audioHandler).setFrequency(22050).start(); // start the audio interrupt at 22.05kHz
  Timer4.attachInterrupt(clockHandler).setPeriod(60000000 / bpm / 96).start(); // start the 96ppq internal clock (in microseconds
  Timer5.attachInterrupt(displayHandler).setFrequency(10).start(); // start the display handler)

  // this is a cheat - enable the DAC
  analogWrite(DAC0, 0);
  analogWrite(DAC1, 0);

  // *** MIDI ***
  midiA.begin(MIDI_CHANNEL_OMNI);
  midiA.setHandleNoteOn(HandleNoteOn);  // these callback functions are defined in MIDI
  midiA.setHandleNoteOff(HandleNoteOff);
  //midiA.setHandleControlChange(HandleCC);
  midiA.setHandleClock(HandleClock);
  midiA.setHandleStart(HandleStart);
  midiA.setHandleStop(HandleStop);

  // *** LOAD DEFAULT KIT ***
  loadDefault();

  // *** POTS ***
  getPots();
  lockPot(5);

  // *** SEQUENCER ***
  for (int i = 0; i < 8; i++) // 8 sequences
  {
    for (int j = 0; j < 6; j++) // 6 voices
    {
      for (int k = 0; k < 32; k++)
        clearStartAuto(i, j);
      clearPitchAuto(i, j);
    }
  }

  // *** PREFERENCES ***
  loadPrefs();

  // *** WAVESHAPER ***
  createWaveShaper();
}

