void getPots() // read the current pot values
{
  for (byte i = 0; i < 5; i++)
  {
    pot[i] = analogRead(i);
  }

  // see if it's time to lock the pots
  if (lockTimer != 0 && (millis() - lockTimer) > 500) // the pots lock after 500ms
  {
    lockPot(5);
    if (menu != 0)
    {
      valueChange = true;
    }
  }
}

void lockPot(byte Pot) // values of 0 - 4 for each pot and 5 for all
{
  if (Pot == 5)
  {
    for (byte i = 0; i < 5; i++)
      potLock[i] = pot[i];
    lockTimer = 0;
  }
  else
    potLock[Pot] = pot[Pot];

  for (byte i = 0; i < 6; i++)
    sequence[currentSeq].editingAuto[i] = false;
}

boolean unlockedPot(byte Pot) // check if a pot is locked or not
{
  if (potLock[Pot] == 9999)
  {
    if (abs(pot[Pot] - lastPotValue[Pot]) > 10) // the threshold value is 10
    {
      lastPotValue[Pot] = pot[Pot];
      lockTimer = millis();
      if (Pot < 4)
        valueChange = true;
    }
    return true;
  }
  else if (abs(potLock[Pot] - pot[Pot]) > 10) // the threshold value is 10
  {
    potLock[Pot] = 9999;
    lockTimer = millis();
    lastPotValue[Pot] = pot[Pot];
    return true;
  }
  else
    return false;
}

void getMenu()
{
  switch (mainMenu)
  {
    case 0: // KIT
      menuPages = 4;
      if (unlockedPot(4)) // select the menu page
      {
        assignIncrementButtons(&menuChoice, 0, menuPages - 1, 1);
        int tmp = 1023 / menuPages;
        menuChoice = constrain(pot[4] / tmp, 0, menuPages - 1);
        menu = kitMenu[menuChoice];
        if (menu != lastMenu)
        {
          lastMenu = menu;
          for (byte i = 0; i < 4; i++)
            lockPot(i);
          updateMenu();
          if (splash)
          {
            customCharacters();
            splash = false;
          }
        }
        else if (lastMainMenu != mainMenu)
        {
          lastMainMenu = mainMenu;
          lockPot[5];
        }
      }
      break;

    case 1: // SEQ
      menuPages = 4;
      if (unlockedPot(4)) // select the menu page
      {
        assignIncrementButtons(&menuChoice, 0, menuPages - 1, 1);
        int tmp = 1023 / menuPages;
        menuChoice = constrain(pot[4] / tmp, 0, menuPages - 1);
        menu = seqMenu[menuChoice];
        if (menu != lastMenu)
        {
          lastMenu = menu;
          for (byte i = 0; i < 4; i++)
            lockPot(i);
          updateMenu();
        }
      }
      break;

    case 2: // SETTINGS
      menuPages = 6;
      if (unlockedPot(4)) // select the menu page
      {
        assignIncrementButtons(&menuChoice, 0, menuPages - 1, 1);
        int tmp = 1023 / menuPages;
        menuChoice = constrain(pot[4] / tmp, 0, menuPages - 1);
        menu = settingsMenu[menuChoice];
        if (menu != lastMenu)
        {
          lastMenu = menu;
          for (byte i = 0; i < 4; i++)
            lockPot(i);
          updateMenu();
        }
      }
      break;
  }
}

void adjustValues()
{
  switch (menu)
  {
    case 0: // main
      if (unlockedPot(0))
      {
        if (splash)
        {
          customCharacters();
          splash = false;
          updateMenu();
        }
        else
        {
          assignIncrementButtons(&mainMenu, 0, 2, 1);
        }

        if (pot[0] < 1023 / 3) // there are 4 mainMenu items
        {
          if (mainMenu != 0)
          {
            mainMenu = 0;
            updateMenu();
          }
        }
        else if (pot[0] < (1023 / 3) * 2)
        {
          if (mainMenu != 1)
          {
            mainMenu = 1;
            updateMenu();
          }
        }
        else
        {
          if (mainMenu != 2)
          {
            mainMenu = 2;
            updateMenu();
            settingsConfirm = false;
          }
        }
      }
      else if (unlockedPot(3))
      {
        if (splash)
        {
          customCharacters();
          splash = false;
          mainMenu = 0;
          updateMenu();
        }
        else
        {
          //assignIncrementButtons(&synPatchLoadSave, 0, 1, 1);
        }
      }
      switch (mainMenu)
      {
        case 0: // Kit
          if (unlockedPot(3))
          {
            if (pot[3] < 512)
            {
              if (kitLoadSave != 0)
              {
                kitLoadSave = 0;
                updateMenu();
              }
            }
            else
            {
              if (kitLoadSave != 1)
              {
                kitLoadSave = 1;
                updateMenu();
              }
            }
          }
          break;
        case 1: // Sequencer
          if (unlockedPot(3))
          {
            //assignIncrementButtons(&seqBankLoadSave, 0, 1, 1);

            if (pot[3] < 512)
            {
              if (seqLoadSave != 0)
              {
                seqLoadSave = 0;
                updateMenu();
              }
            }
            else
            {
              if (seqLoadSave != 1)
              {
                seqLoadSave = 1;
                updateMenu();
              }
            }
          }
          break;
      }
      break;

    case 100: // KIT VOICE SETUP
      if (unlockedPot(0))
      {
        assignIncrementButtons(&voice[adjustVoice].defaultVolume, 0, 1023, 4);
        if (voice[adjustVoice].defaultVolume != pot[0])
          voice[adjustVoice].defaultVolume = pot[0];
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&voice[adjustVoice].defaultLoseBits, 0, 11, 1);
        int tmp = constrain(pot[1] / 93, 0, 11);
        if (voice[adjustVoice].defaultLoseBits != tmp)
          voice[adjustVoice].defaultLoseBits = tmp;
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&adjustVoice, 0, 5, 1);
        int tmp = constrain(pot[3] / 204, 0, 5);
        if (adjustVoice != tmp)
          adjustVoice = tmp;
      }
      break;

    case 101: // KIT LOAD VOICE
      if (unlockedPot(3) && dirCount != 0)
      {
        justEnteredFolder = false;
        assignIncrementButtons(&dirChoice, 1, dirCount, 1);
        //dirChoice = map(pot[3], 0, 1023, 1, dirCount);
        dirChoice = constrain(pot[3] / (1023 / dirCount), 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          lastDirChoice = dirChoice;
          prepNextChoice();
        }
      }
      break;

    case 110: // KIT DEFAULTS
      if (unlockedPot(0)) // sample start
      {
        assignIncrementButtons(&voice[adjustVoice].defaultSampleStart, 0, voice[adjustVoice].sampleEnd, 16);
        int tmp = map(pot[0], 0, 1023, 0, voice[adjustVoice].sampleEnd);
        if (voice[adjustVoice].sampleStart != tmp)
        {
          voice[adjustVoice].sampleStart = tmp;
          voice[adjustVoice].defaultSampleStart = tmp;
        }
      }

      if (unlockedPot(1)) // pitch
      {
        assignIncrementButtons(&voice[adjustVoice].defaultIncrement, 0, 1023, 8);
        if (voice[adjustVoice].defaultIncrement != pot[1])
        {
          voice[adjustVoice].increment = pot[1];
          voice[adjustVoice].defaultIncrement = pot[1];
        }
      }

      if (unlockedPot(2)) // pan
      {
        assignIncrementButtons(&voice[adjustVoice].pan, 0, 1023, 8);
        if (voice[adjustVoice].pan != pot[2])
          voice[adjustVoice].pan = pot[2];
      }

      if (unlockedPot(3)) // sample end
      {
        assignIncrementButtons(&voice[adjustVoice].sampleEnd, voice[adjustVoice].sampleStart, voice[adjustVoice].wavLength, 16);
        int tmp = map(pot[3], 0, 1023, voice[adjustVoice].sampleStart, voice[adjustVoice].wavLength);
        if (voice[adjustVoice].sampleEnd != tmp)
          voice[adjustVoice].sampleEnd = tmp;
      }
      break;
    case 120: // KIT VELOCITY MODULATION
      if (unlockedPot(0)) // velocity to sample start
      {
        assignIncrementButtons(&voice[adjustVoice].velSStFactor, 0, 1023, 4);
        if (voice[adjustVoice].velSStFactor != pot[0])
          voice[adjustVoice].velSStFactor = pot[0];
      }
      if (unlockedPot(1)) // velocity to pitch
      {
        assignIncrementButtons(&voice[adjustVoice].velPitchFactor, 0, 1023, 8);
        if (voice[adjustVoice].velPitchFactor != pot[1])
          voice[adjustVoice].velPitchFactor = pot[1];
      }
      if (unlockedPot(2)) // velocity to bit muncher
      {
        assignIncrementButtons(&voice[adjustVoice].velBitFactor, 0, 1023, 4);
        if (voice[adjustVoice].velBitFactor != pot[2])
          voice[adjustVoice].velBitFactor = pot[2];
      }
      if (unlockedPot(3)) // velocity to volume
      {
        assignIncrementButtons(&voice[adjustVoice].velVolFactor, 0, 1023, 4);
        if (voice[adjustVoice].velVolFactor != pot[3])
          voice[adjustVoice].velVolFactor = pot[3];
      }
      break;
    case 150: // LOAD KIT
      if (unlockedPot(3) && dirCount != 0)
      {
        int potSteps = 1023 / dirCount;
        dirChoice = constrain((pot[3] / potSteps) + 1, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          kitLoaded = false;
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
          justEnteredFolder = false;
        }
      }
      break;
    case 160: // SAVE KIT
      if (unlockedPot(3) && dirCount != 0)
      {
        int potSteps = 1023 / dirCount;
        dirChoice = constrain((pot[3] / potSteps) + 1, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
        }
      }
      break;
    case 200: // SEQUENCE TRIGGER
      if (unlockedPot(2))
      {
        assignIncrementButtons(&bankMode, 0, 4, 1);
        int tmpMode = constrain(pot[2] / 205, 0, 4);
        if (bankMode != tmpMode)
          bankMode = tmpMode;
      }
      if (unlockedPot(3)) // GRAIN FX PITCH
      {
        if (grainPitch != pot[3])
          grainPitch = pot[3];
      }
      break;
    case 210: // SEQUENCE EDIT
      if (unlockedPot(0)) // update write sample start automation values 0 - 128
      {
        static boolean functionLock = false;
        if (pressed[14])
        {
          shiftR = true;
          int tmp = map(pot[0], 0, 1023, 0, voice[adjustVoice].sampleEnd);
          sequence[currentSeq].startAuto[adjustVoice][playhead] = tmp >> 5;
          voice[adjustVoice].sampleStart = tmp;
          sequence[currentSeq].ssAuto[adjustVoice] = true;
          functionLock = true;
        }
        else if (pressed[1])
        {
          int tmp = pot[0] >> 3;
          if (voice[adjustVoice].standardVel != tmp)
            voice[adjustVoice].standardVel = tmp;
          functionLock = true;
        }
        else if (pressed[3])
        {
          int tmp = constrain(pot[0] / (1023 / 96) , 1, 96);
          if (voice[adjustVoice].flamTime != tmp)
            voice[adjustVoice].flamTime = tmp;
          functionLock = true;
        }
        else
        {
          if (functionLock)
          {
            lockPot(0);
            functionLock = false;
          }
          else
          {
            int tmp = map(pot[0], 0, 1023, 0, voice[adjustVoice].sampleEnd);
            if (voice[adjustVoice].sampleStart != tmp)
            {
              sequence[currentSeq].ssAuto[adjustVoice] = false;
              voice[adjustVoice].sampleStart = tmp;
            }
          }
        }
      }
      if (unlockedPot(1)) // update write pitch automation values 0 - 128
      {
        static boolean functionLock = false;
        if (pressed[14])
        {
          shiftR = true;
          voice[adjustVoice].increment = pot[1];
          sequence[currentSeq].pitchAuto[adjustVoice][playhead] = pot[1] >> 3;
          sequence[currentSeq].pAuto[adjustVoice] = true;
          sequence[currentSeq].editingAuto[adjustVoice] = true;
          autoVal[adjustVoice] = constrain(pot[1] >> 3, 0, 127);
          functionLock = true;
        }
        else if (pressed[1])
        {
          int tmp = pot[1] >> 3;
          if (voice[adjustVoice].accentVel != tmp)
            voice[adjustVoice].accentVel = tmp;
          functionLock = true;
        }
        else if (pressed[3])
        {
          int tmp = constrain(32 - (pot[1] / (1023 / 33)), 0, 32);
          if (voice[adjustVoice].flamDecay != tmp)
            voice[adjustVoice].flamDecay = tmp;
          functionLock = true;
        }
        else
        {
          if (functionLock)
          {
            lockPot(1);
            functionLock = false;
          }
          else
          {
            if (voice[adjustVoice].increment != pot[1])
            {
              sequence[currentSeq].pAuto[adjustVoice] = false;
              voice[adjustVoice].increment = pot[1];
            }
          }
        }
      }
      if (unlockedPot(2)) // voice we're editing
      {
        int tmp = constrain(pot[2] / 170, 0, 5); // 170 = 1023 / 6(voices)
        if (adjustVoice != tmp)
          adjustVoice = tmp;
      }
      if (unlockedPot(3)) // sequence row
      {
        int tmp = constrain(pot[3] / 256, 0, 3); // 256 = 1023 / 4(rows)
        if (seqRow != tmp)
          seqRow = tmp;
      }
      break;
    case 220: // sequence settings
      if (unlockedPot(0))
      {
        assignIncrementButtons(&sequence[currentSeq].seqLength, 1, 32, 1);
        int tmp = constrain(pot[0] / (1023 / 32), 1, 32);
        if (sequence[currentSeq].seqLength != tmp)
          sequence[currentSeq].seqLength = tmp;
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&sequence[currentSeq].interpolate, 0, 1, 1);
        int tmp = constrain(pot[1] / 512, 0, 1);
        if (sequence[currentSeq].interpolate != tmp)
          sequence[currentSeq].interpolate = !sequence[currentSeq].interpolate;
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&sequence[currentSeq].swing, 0, 8, 1);
        int tmp = constrain(pot[2] / (1023 / 9), 0, 8);
        if (sequence[currentSeq].swing != tmp)
          sequence[currentSeq].swing = tmp;
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&seqBpm, 20, 320, 1);
        int tmp = constrain(pot[3] / (1023 / 321) , 20, 320);
        if (seqBpm != tmp)
        {
          seqBpm = tmp;
          bpmChange(tmp);
        }
      }
      break;
    case 250: // LOAD SEQ BANK
      if (unlockedPot(3) && dirCount != 0)
      {
        int potSteps = 1023 / dirCount;
        dirChoice = constrain((pot[3] / potSteps) + 1, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          seqLoaded = false;
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
          justEnteredFolder = false;
        }
      }
      break;
    case 260: // SAVE SEQ BANK
      if (unlockedPot(3) && dirCount != 0)
      {
        int potSteps = 1023 / dirCount;
        dirChoice = constrain((pot[3] / potSteps) + 1, 1, dirCount);
        if (dirChoice != lastDirChoice)
        {
          lastDirChoice = dirChoice;
          sd.vwd()->rewind();
          tempCount = 0;
        }
      }
      break;

    case 300: // SETTINGS MIDI GENERAL
      if (unlockedPot(0))
      {
        assignIncrementButtons(&midiOut, 0, 1, 1);
        int tmp = (pot[0] < 512) ? 0 : 1;
        if (midiOut != tmp)
        {
          midiOut = tmp;
          valueChange;
        }
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&midiChannel, 1, 16, 1);
        int tmp = constrain(pot[1] / (1023 / 16), 1, 16);
        if (midiChannel != tmp)
        {
          midiChannel = tmp;
          valueChange;
        }
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&midiThruType, 0, 2, 1);
        byte items = 3;
        int tmp = (pot[2] / (1022 / items) < (items - 1)) ? pot[2] / (1022 / items) : items - 1;
        if (midiThruType != tmp)
        {
          midiThruType = tmp;
          checkThru();
          valueChange;
        }
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&midiSync, 0, 2, 1);
        int tmp = (pot[3] / (1027 / 3));
        if (midiSync != tmp)
        {
          midiSync = tmp;
          setSyncType();
        }
      }
      break;
    case 310: // SETTINGS MIDI VOICE
      if (unlockedPot(0))
      {
        assignIncrementButtons(&settingsVoice, 0, 5, 1);
        int tmp = (pot[0] / (1027 / 6));

        if (settingsVoice != tmp)
          settingsVoice = tmp;
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&autoChannel[settingsVoice], 0, 16, 1);
        int tmp = constrain((pot[1] / (1024 / 17)), 0, 16);
        if (tmp != autoChannel[settingsVoice])
          autoChannel[settingsVoice] = tmp;
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&autoCC[settingsVoice], 0, 127, 1);
        int tmp = (pot[2] / (1024 / 128));
        if (tmp != autoCC[settingsVoice])
          autoCC[settingsVoice] = tmp;
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&voiceNoteNumber[settingsVoice], 0, 127, 1);
        int tmp = pot[3] >> 3;
        if (tmp != voiceNoteNumber[settingsVoice])
          voiceNoteNumber[settingsVoice] = tmp;
      }
      break;
    case 320: // SETTINGS TRIGGER MIDI
      if (unlockedPot(0))
      {
        assignIncrementButtons(&midiTriggerOut, 0, 1, 1);
        if (pot[0] < 512)
          midiTriggerOut = 0;
        else
          midiTriggerOut = 1;
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&midiTriggerChannel, 1, 16, 1);
        int tmp = (pot[1] / (1024 / 16)) + 1;
        if (tmp != midiTriggerChannel)
          midiTriggerChannel = tmp;
      }
      if (unlockedPot(2))
      {
        assignIncrementButtons(&editTrigger, 0, 7, 1);
        int tmp = pot[2] / (1024 / 8);
        if (tmp != editTrigger)
          editTrigger = tmp;
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&midiTrigger[editTrigger], 0, 127, 1);
        int tmp = pot[3] >> 3;
        if (tmp != midiTrigger[editTrigger])
          midiTrigger[editTrigger] = tmp;
      }
      break;
    case 330: // SETTINGS SEQUENCER
      if (unlockedPot(2))
      {
        assignIncrementButtons(&recallKit, 0, 1, 1);
        if (pot[2] < 512)
          recallKit = 0;
        else
          recallKit = 1;
      }
      if (unlockedPot(3))
      {
        assignIncrementButtons(&recallTempo, 0, 1, 1);
        if (pot[3] < 512)
          recallTempo = 0;
        else
          recallTempo = 1;
      }
      break;
    case 340: // SETTINGS SHAPER & VOLUME
      if (unlockedPot(0))
      {
        assignIncrementButtons(&waveShapeAmount, 1, 15, 1);
        int tmp = constrain(pot[0] / (1023 / 16), 1, 15);
        if (waveShapeAmount != tmp)
          waveShapeAmount = tmp;
        createWaveShaper();
      }
      if (unlockedPot(1))
      {
        assignIncrementButtons(&finalVolume, 0, 1023, 4);
        if (finalVolume != pot[1])
          finalVolume = pot[1];
      }
      break;
  }
}
