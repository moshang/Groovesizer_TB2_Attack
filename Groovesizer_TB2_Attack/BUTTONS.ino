void checkSwitches()
{
  // https://blog.adafruit.com/2009/10/20/example-code-for-multi-button-checker-with-debouncing/
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;

  if (millis() < lasttime)
  {
    // we wrapped around, lets just try again
    lasttime = millis();
  }

  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return;
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();

  for (index = 0; index < NUMBUTTONS; index++) // when we start, we clear out the "just" indicators
  {
    justreleased[index] = 0;
    justpressed[index] = 0;


    currentstate[index] = digitalRead(buttons[index]);   // read the button

    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        // just released
        justreleased[index] = 1;
      }
      pressed[index] = !currentstate[index];  // remember, digital HIGH means NOT pressed
    }

    previousstate[index] = currentstate[index];   // keep a running tally of the buttons
  }
}

void clearJust()
{
  for (byte index = 0; index < NUMBUTTONS; index++) // when we start, we clear out the "just" indicators
  {
    justreleased[index] = 0;
    justpressed[index] = 0;
  }
}

void handlePresses()
{
  if (pressed[13] && justpressed[14]) // fine adjust increment
  {
    if ((*adjustValue + increment) <= upperLimit)
      *adjustValue += increment;
    else
      *adjustValue = lowerLimit;

    incDecSpecials();
    shiftL = true;
    clearJust();
  }

  if (pressed[14] && justpressed[13]) // fine adjust decrement
  {
    if ((*adjustValue - increment) >= lowerLimit)
      *adjustValue -= increment;
    else
      *adjustValue = upperLimit;

    incDecSpecials();
    shiftR = true;
    clearJust();
  }

  if (shiftL && justreleased[13])
  {
    shiftL = false;
    valueChange = true;
    clearJust();
  }
  else if (shiftR && justreleased[14])
  {
    shiftR = false;
    valueChange = true;
    clearJust();
  }
  else
  {
    switch (mainMenu)
    {
      case 0: // KIT
        if (justreleased[14] && menu == 0)
        {
          if (kitLoadSave == 0) // LOAD
          {
            menu = 150;
            gotoRootDir();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else // SAVE
          {
            gotoRootDir();
            menu = 160;
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          valueChange = true;
          clearJust();
        }
        for (int i = 0; i < 6; i++)
        {
          if (justpressed[whiteKey[i]])
          {
            if (menu != 101)
            {
              hit(i, voice[i].accentVel);
              adjustVoice = i;
              lockPot(5);
              justpressed[whiteKey[i]] = false; // prevent false retriggers
            }
            else
            {
              hit(adjustVoice, voice[adjustVoice].accentVel);
              justpressed[whiteKey[i]] = false; // prevent false retriggers
              lockPot(5);
            }
            valueChange = true;
          }
        }
        break;
      case 1: // SEQUENCER
        if (justreleased[14] && menu == 0)
        {
          if (seqLoadSave == 0) // LOAD
          {
            menu = 250;
            gotoRootDir();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else // SAVE
          {
            gotoRootDir();
            menu = 260;
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          valueChange = true;
          clearJust();
        }
        break;
      case 2: // SETTINGS
        if (justreleased[14] && menu == 0)
        {
          if (settingsConfirm)
          {
            savePrefs();
            settingsConfirm = false;
            updateMenu();
            lcd.setCursor(0, 1);
            lcd.print("    Saved!      ");
            arrowAnimation = false;
          }
          else
          {
            settingsConfirm = true;
            updateMenu();
            lcd.setCursor(0, 1);
            lcd.print("    Overwrite?  ");
          }
          clearJust();
        }
        if (justreleased[13] && menu == 0)
        {
          settingsConfirm = false;
          updateMenu();
          clearJust();
        }
        for (int i = 0; i < 6; i++)
        {
          if (justpressed[whiteKey[i]])
          {
            hit(i, voice[i].accentVel);
            justpressed[whiteKey[i]] = false; // prevent false retriggers
          }
        }
        break;
    }
    switch (menu)
    {
      case 100: // KIT VOICE SETUP
        if (justreleased[14] && !shiftL && !shiftR) // enter key
        {
          menu = 101;
          gotoRootDir();
          getDirCount();
          assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          valueChange = true;
          clearJust();
        }
        break;
      case 101: // KIT LOAD VOICE
        if (justreleased[14] && !shiftL && !shiftR) // enter key
        {
          if (!inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            justEnteredFolder = true;
          }
          clearJust();
        }
        if (justreleased[13] && !shiftL && !shiftR) // back key
        {
          if (inFolder)
          {
            setFolder();
            getDirCount();
          }
          else
          {
            menu = 100;
            valueChange = true;
            lcd.clear();
          }
          clearJust();
        }
        break;
      case 150: // LOAD KIT
        if (justreleased[14] && !shiftL && !shiftR) // enter
        {
          if (!inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            justEnteredFolder = true;
          }
          lockPot(5);
          clearJust();
        }
        if (justreleased[13] && !shiftL && !shiftR) // back
        {
          if (inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else
          {
            menu = 0;
            valueChange = true;
            lcd.clear();
            updateMenu();
          }
          lockPot(5);
          clearJust();
        }
        break;
      case 160: // SAVE KIT
        if (justreleased[14] && !shiftL && !shiftR) // enter
        {
          if (!inFolder)
          {
            setFolder();
            justEnteredFolder = true;
          }
          else if (saveConfirm)
          {
            saveConfirm = false;
            saveKit();
          }
          else
          {
            if (strcmp(fileName, saveName) == 0)
              saveKit();
            else
            {
              saveConfirm = true;
              valueChange = true;
            }
          }
          lockPot(5);
          clearJust();
        }
        if (justreleased[13] && !shiftL && !shiftR) // back
        {
          if (saveConfirm)
            saveConfirm = false;
          else if (inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else
          {
            menu = 0;
            valueChange = true;
            lcd.clear();
            updateMenu();
          }
          lockPot(5);
          clearJust();
        }
        break;
      case 200: // SEQUENCER TRIGGER
        if (justpressed[13])
        {
          seqReset();
          seqRunning = !seqRunning;
          sendMidiStartStop();
          valueChange = true;
        }
        for (byte i = 0; i < 8; i++)
        {
          if (justpressed[whiteKey[i]])
          {
            sourceSeq = currentSeq;
            if (midiTriggerOut)
              midiA.sendNoteOn(midiTrigger[i], 127, midiTriggerChannel);
            if (seqRunning)
            {
              cuedSeq = i;
            }
            else
            {
              currentSeq = i;
              cuedSeq = i;
            }
            valueChange = true;
            clearJust();
            longPress = millis();
            copied = false;
            destinationSeq = i;
          }
          if (pressed[whiteKey[i]])
          {
            if ((millis() - longPress) > 500 && !copied)
              copySeq();
          }
        }

        if (justpressed[1]) // BEAT REPEAT
        {
          beatRepeat = true;
          clearJust();
        }
        if (justreleased[1])
        {
          beatRepeat = false;
        }
        if (justpressed[3]) // FLANGER
        {
          flanger = true;
          clearJust();
        }
        if (justreleased[3])
        {
          flanger = false;
        }
        if (pressed[6]) // GRAIN FX HOLD
        {
          grainPlay = true;
        }
        if (justreleased[6])
        {
          grainPlay = false;
          grainPitch = 255;
          lockPot(5);
        }
        if (pressed[8]) // GRAIN FX RYTHM
        {
          grainRhythm = true;
        }
        if (justreleased[8])
        {
          grainPlay = false;
          grainRhythm = false;
          grainPitch = 255;
          lockPot(5);
        }
        if (pressed[10]) // GRAIN FX RYTHM 2
        {
          grainRhythm2 = true;
        }
        if (justreleased[10])
        {
          grainPlay = false;
          grainRhythm2 = false;
          grainPitch = 255;
          lockPot(5);
        }
        break;
      case 210: // SEQUENCER EDIT
        if (justpressed[10])
        {
          if (!seqRunning)
            valueChange = true; // so that it will show the labels for pot0 (sample start) and pot1 (pitch)
          else if (record)
          {
            record = false;
            metronome = false;
            valueChange = true; // so that it will show the "Record" tool tip
          }
          clearJust();
        }
        if (justreleased[10] && !record)
        {
          valueChange = true; // so that it will show the labels for pot0 (sample start) and pot1 (pitch)
          clearJust();
        }
        if (justpressed[13])
        {
          if (!pressed[10])
          {
            seqReset();
            seqRunning = !seqRunning;
            sendMidiStartStop();
          }
          if (pressed[10])
          {
            if (!seqRunning)
            {
              seqReset();
              seqRunning = true;
              sendMidiStartStop();
            }
            record = true;
            metronome = true;
            metroIndex = 255;
          }
          else
          {
            record = false;
            metronome = false;
          }
          clearJust();
          valueChange = true;
        }
        if (justpressed[14])
        {
          showAutoLabel = true;
          valueChange = true; // so that it will show the labels for pot0 (sample start) and pot1 (pitch)
          clearJust();
        }
        if (justreleased[14])
        {
          showAutoLabel = false;
          valueChange = true; // so that it will stop showing the labels for pot0 (sample start) and pot1 (pitch)
          clearJust();
        }
        for (int i = 0; i < 8; i++)
        {
          if (justpressed[whiteKey[i]])
          {
            if (!record)
            {
              if (pressed[14])
              {
                if (i < 6)
                {
                  adjustVoice = i;
                  hit(i, voice[i].standardVel);
                }
                else if (i == 6)
                  seqRow = (seqRow > 0) ? seqRow - 1 : 3;
                else if (i == 7)
                  seqRow = (seqRow < 3) ? seqRow + 1 : 0;
                shiftR = true;
                showAutoLabel = false;
                valueChange = true;
                clearJust();
              }
              else
              {
                if (pressed[1])
                  sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)] = 2; // accent velocity
                else if (pressed[3])
                {
                  if (sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)] == 0 || sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)] == 4)
                    sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)] = 3; // standard flam
                  else
                    sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)] = 4; // accent flam
                }
                else if (sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)] == 0)
                  sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)] = 1; // standard velocity
                else // clear step
                  sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)] = 0;

                clearJust();
                valueChange = true;
              }
            }
            else // we're recording
            {
              if (i < 6)
              {
                if (!beforeTargetStep)
                {
                  if (!pressed[1] && !pressed[3]) // STANDARD
                  {
                    adjustVoice = i;
                    sequence[currentSeq].seq[i][targetStep] = 1; // standard velocity
                    hit(i, voice[i].standardVel);
                    voice[i].nextFlam = 255; // off
                  }
                  else if (pressed[1] && !pressed[3]) // ACCENT
                  {
                    adjustVoice = i;
                    sequence[currentSeq].seq[i][targetStep] = 2; // accent velocity
                    hit(i, voice[i].accentVel);
                    voice[i].nextFlam = 255; // off
                  }
                  else if (pressed[3] && !pressed[1]) // STANDARD FLAM
                  {
                    adjustVoice = i;
                    sequence[currentSeq].seq[i][targetStep] = 3; // standard velocity
                    hit(i, voice[i].standardVel);
                    scheduleFlam(i, voice[i].standardVel);
                  }
                  else if (pressed[1] && pressed[3]) // ACCENT FLAM
                  {
                    adjustVoice = i;
                    sequence[currentSeq].seq[i][targetStep] = 4; // accent velocity
                    hit(i, voice[i].accentVel);
                    scheduleFlam(i, voice[i].accentVel);
                  }
                }
                else // it's before the target step, so take measures to prevent a double hit
                {
                  if (!pressed[1] && !pressed[3]) // STANDARD
                  {
                    adjustVoice = i;
                    sequence[currentSeq].seq[i][targetStep] = 5; // placeholder for standard velocity
                    hit(i, voice[i].standardVel);
                    voice[i].nextFlam = 255; // off
                  }
                  else if (pressed[1] && !pressed[3]) // ACCENT
                  {
                    adjustVoice = i;
                    sequence[currentSeq].seq[i][targetStep] = 6; // placeholder for standard velocity
                    hit(i, voice[i].accentVel);
                    voice[i].nextFlam = 255; // off
                  }
                  else if (pressed[3] && !pressed[1]) // STANDARD FLAM
                  {
                    adjustVoice = i;
                    sequence[currentSeq].seq[i][targetStep] = 7; // placeholder for standard flam
                    hit(i, voice[i].standardVel);
                    scheduleFlam(i, voice[i].standardVel);
                  }
                  else if (pressed[1] && pressed[3]) // ACCENT FLAM
                  {
                    adjustVoice = i;
                    sequence[currentSeq].seq[i][targetStep] = 8; // placeholder for accent flam
                    hit(i, voice[i].accentVel);
                    scheduleFlam(i, voice[i].accentVel);
                  }
                }
                justpressed[whiteKey[i]] = false;
              }
            }
          }
        }
        if (pressed[14])
        {
          if (justpressed[1])
          {
            for (byte i = 0; i < 6; i++)
              clearSeq(currentSeq, i);
            clearJust();
            showAutoLabel = false;
            valueChange = true;
          }
          if (justpressed[3])
          {
            clearSeq(currentSeq, adjustVoice);
            clearJust();
            showAutoLabel = false;
            valueChange = true;
          }
        }
        break;
      case 250: // LOAD SEQ BANK
        if (justreleased[14] && !shiftL && !shiftR) // enter
        {
          if (!inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
            justEnteredFolder = true;
          }
          lockPot(5);
          clearJust();
        }
        if (justreleased[13] && !shiftL && !shiftR) // back
        {
          if (inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else
          {
            menu = 0;
            valueChange = true;
            lcd.clear();
            updateMenu();
          }
          lockPot(5);
          clearJust();
        }
        break;
      case 260: // SAVE SEQ BANK
        if (justreleased[14] && !shiftL && !shiftR) // enter
        {
          if (!inFolder)
          {
            setFolder();
            justEnteredFolder = true;
          }
          else if (saveConfirm)
          {
            saveConfirm = false;
            saveSeqBank();
          }
          else
          {
            if (strcmp(fileName, saveName) == 0)
              saveSeqBank();
            else
            {
              saveConfirm = true;
              valueChange = true;
            }
          }
          lockPot(5);
          clearJust();
        }
        if (justreleased[13] && !shiftL && !shiftR) // back
        {
          if (saveConfirm)
            saveConfirm = false;
          else if (inFolder)
          {
            setFolder();
            getDirCount();
            assignIncrementButtons(&dirChoice, 1, dirCount, 1);
          }
          else
          {
            menu = 0;
            valueChange = true;
            lcd.clear();
            updateMenu();
          }
          lockPot(5);
          clearJust();
        }
        break;

    }
  }
}

void assignIncrementButtons(int *adjVar, int low, int up, int inc) // variable address, lower limit, upper limit, increment
{
  adjustValue = adjVar;
  lowerLimit = low;
  upperLimit = up;
  increment = inc;
}

void incDecSpecials()
{
  // special cases
  if (adjustValue == &dirChoice && dirCount != 0)
    prepNextChoice();

  else if (adjustValue == &mainMenu)
    updateMenu();

  else if (adjustValue == &voice[adjustVoice].increment)
    voice[adjustVoice].defaultIncrement = voice[adjustVoice].increment;

  else if (adjustValue == &seqBpm)
    bpmChange(seqBpm);

  // evaluate seperately for the sake of the else
  if (adjustValue == &menuChoice)
  {
    lockTimer = millis();
    switch (mainMenu)
    {
      case 0: // synth
        menu = kitMenu[*adjustValue];
        break;
      case 1: // arp
        menu = seqMenu[*adjustValue];
        break;
      case 2: // sequencer
        menu = settingsMenu[*adjustValue];
        break;
    }
    updateMenu();
  }
  else
  {
    valueChange = true;
    lockPot(5);
  }
}
