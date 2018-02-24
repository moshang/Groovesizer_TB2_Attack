// *** UI ***
void updateMenu()
{
  arrowAnimation = false;
  switch (menu)
  {
    case 0: // SPLASH/MAIN
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("MAIN            ");
      switch (mainMenu)
      {
        case 0:
          lcd.setCursor(0, 1);
          lcd.print("Kit           ");
          if (kitLoadSave == 0) // load
          {
            lcd.setCursor(11, 1);
            lcd.print("Load      ");
          }
          else // save
          {
            lcd.setCursor(11, 1);
            lcd.print("Save      ");
          }
          arrow(15, 1);
          break;
        case 1:
          lcd.setCursor(0, 1);
          lcd.print("Sequencer       ");
          lcd.setCursor(11, 0);
          lcd.print("Bank ");
          if (seqLoadSave == 0) // load
          {
            lcd.setCursor(11, 1);
            lcd.print("Load      ");
          }
          else // save
          {
            lcd.setCursor(11, 1);
            lcd.print("Save      ");
          }
          arrow(15, 1);
          break;
        case 2: // SETTINGS
          lcd.setCursor(0, 1);
          lcd.print("Settings   Save ");
          if (!settingsConfirm)
            arrow(15, 1);
          break;
      }
      break;
    case 100: // KIT VOICE SETUP
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("KIT     ");
      lcd.setCursor(0, 1);
      lcd.print("Voice Setup     ");
      break;
    case 110: // KIT DEFAULTS
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("KIT     ");
      lcd.setCursor(0, 1);
      lcd.print("Defaults        ");
      break;
    case 120: // KIT VELOCITY MODULATION
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("KIT     ");
      lcd.setCursor(0, 1);
      lcd.print("Velocity Mod    ");
      break;
    case 200: // SEQUENCER TRIGGER
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SEQUENCER       ");
      lcd.setCursor(0, 1);
      lcd.print("Trigger         ");
      break;
    case 210: // SEQUENCER EDIT
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SEQUENCER       ");
      lcd.setCursor(0, 1);
      lcd.print("Edit            ");
      break;
    case 220: // SEQUENCER SETTINGS
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SEQUENCER       ");
      lcd.setCursor(0, 1);
      lcd.print("Settings        ");
      break;
    case 300: // SETTINGS MIDI GENERAL
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SETTINGS        ");
      lcd.setCursor(0, 1);
      lcd.print("MIDI General    ");
      break; // SETTINGS VOICE
    case 310:
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SETTINGS        ");
      lcd.setCursor(0, 1);
      lcd.print("MIDI Voice      ");
      break;
    case 320: // SETTINGS MIDI TRIGGER
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SETTINGS        ");
      lcd.setCursor(0, 1);
      lcd.print("MIDI Trigger    ");
      break;
    case 330: // SETTINGS SEQUENCER
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SETTINGS        ");
      lcd.setCursor(0, 1);
      lcd.print("Sequencer       ");
      break;
          case 340: // SETTINGS SEQUENCER
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("SETTINGS        ");
      lcd.setCursor(0, 1);
      lcd.print("Shaper & Volume ");
      break;
  }
}

void updateValues()
{
  if (uiRefresh && valueChange && menu != 0)
  {
    arrowAnimation = false;
    uiRefresh = false;
    valueChange = false;
    switch (menu)
    {
      case 100: // KIT PAGE 1
        lcd.setCursor(0, 0);
        lcd.print("Vol BM      Vc: ");
        showValue(15, 0, adjustVoice + 1);
        showValue(0, 1, voice[adjustVoice].defaultVolume >> 2);
        showValue(4, 1, 12 - voice[adjustVoice].defaultLoseBits);
        for (int i = 0; i < (8 - strlen(voice[adjustVoice].shortName)); i++)
        {
          lcd.setCursor(7 + i, 1);
          lcd.print(" ");
        }
        lcd.setCursor(7 + (8 - strlen(voice[adjustVoice].shortName)), 1);
        lcd.print(voice[adjustVoice].shortName);
        arrow(15, 1);
        break;
      case 101: // KIT LOAD VOICE
        lcd.setCursor(0, 0);
        lcd.print("Vc:");
        showValue(3, 0, adjustVoice + 1);
        if (inFolder)
        {
          lcd.setCursor(5, 0);
          lcd.print(folderName);
          lcd.print("        ");
          if (justEnteredFolder)
          {
            lcd.setCursor(0, 1);
            lcd.print("Load Voice [P4] ");
          }
        }
        else
        {
          lcd.setCursor(5, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        getVoice();
        if (wrongFormat)
        {
          lcd.setCursor(0, 1);
          lcd.print("Unsupported File");
        }
        break;
      case 110: // KIT PAGE 2
        lcd.setCursor(0, 0);
        lcd.print("SSt Ptc Pan End ");
        showValue(15, 0, adjustVoice + 1);
        showValue(0, 1, map(voice[adjustVoice].defaultSampleStart, 0, voice[adjustVoice].sampleEnd, 0, 255));
        showValue(4, 1, (voice[adjustVoice].defaultIncrement >> 3) - 64);
        showValue(8, 1, (voice[adjustVoice].pan >> 3) - 64);
        showValue(12, 1, map(voice[adjustVoice].sampleEnd, voice[adjustVoice].sampleStart, 5499, 0, 255));
        lcd.setCursor(15, 1);
        lcd.print((char)94);
        break;
      case 120: // KIT VELOCITY MODULATION
        lcd.setCursor(0, 0);
        lcd.print("SSt Ptc BM  Vol ");
        showValue(15, 0, adjustVoice + 1);
        showValue(0, 1, voice[adjustVoice].velSStFactor >> 2);
        showValue(4, 1, (voice[adjustVoice].velPitchFactor >> 3) - 64);
        showValue(8, 1, voice[adjustVoice].velBitFactor >> 2);
        showValue(12, 1, voice[adjustVoice].velVolFactor >> 2);
        lcd.setCursor(15, 1);
        lcd.print((char)94);
        break;
      case 150: // KIT LOAD
        lcd.setCursor(0, 0);
        lcd.print("Kit: ");
        if (inFolder)
        {
          lcd.setCursor(4, 0);
          lcd.print(folderName);
          lcd.print("        ");
          if (justEnteredFolder)
          {
            lcd.setCursor(0, 1);
            lcd.print("Load Kit   [P4] ");
          }
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        getKit();
        break;
      case 160: // KIT SAVE
        lcd.setCursor(0, 0);
        lcd.print("Kit: ");

        if (inFolder)
        {
          lcd.setCursor(4, 0);
          lcd.print(folderName);
          lcd.print("        ");
        }
        else
        {
          lcd.setCursor(4, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        prepSaveKit();
        if (saveConfirm)
        {
          lcd.setCursor(0, 1);
          lcd.print("    Overwrite?  ");
        }
        if (justSavedKit)
        {
          lcd.setCursor(0, 1);
          lcd.print("    Saved!      ");
          justSavedKit = false;
        }
        break;
      case 200: // SEQUENCE TRIGGER
        lcd.setCursor(0, 0);
        lcd.print("1 2 3 4 5 6 7 8 ");
        if (seqRunning)
        {
          arrow((currentSeq * 2) + 1, 0);
          if (cuedSeq != currentSeq)
          {
            lcd.setCursor((cuedSeq * 2) + 1, 0);
            lcd.print((char)127);
          }
        }
        else
        {
          lcd.setCursor((cuedSeq * 2) + 1, 0);
          lcd.print((char)127);
        }
        lcd.setCursor(0, 1);
        if (seqRunning)
          lcd.print("Stop   Lp:  Bt: ");
        else
          lcd.print("Play   Lp:  Bt: ");
        lcd.setCursor(10, 1);
        lcd.print(loopMode[bankMode]);
        if (seqRunning)
          showValue(15, 1, (realStep / 4) + 1);
        break;
      case 210: // SEQUENCE EDIT
        lcd.setCursor(0, 0);
        for (int i = 0; i < 8; i++)
        {
          lcd.setCursor(i * 2, 0);
          if (sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)])
          {
            switch (sequence[currentSeq].seq[adjustVoice][i + (seqRow * 8)])
            {
              case 1:
                lcd.write(4);
                break;
              case 2:
                lcd.print("A");
                break;
              case 3:
                lcd.print("F");
                break;
              case 4:
                lcd.write(6);
                break;
            }
          }
          else
            lcd.write(5);
          lcd.print(" ");
        }
        if (pressed[1] && (unlockedPot(0) || unlockedPot(1)))
        {
          lcd.setCursor(0, 0);
          lcd.print("Std Acc         ");
          showValue(0, 1, voice[adjustVoice].standardVel);
          showValue(4, 1, voice[adjustVoice].accentVel);
          lcd.setCursor(8, 1);
          lcd.print("        ");
        }
        else if (pressed[3] && (unlockedPot(0) || unlockedPot(1)))
        {
          lcd.setCursor(0, 0);
          lcd.print("FTm FDc         ");
          showValue(0, 1, voice[adjustVoice].flamTime);
          showValue(4, 1, voice[adjustVoice].flamDecay);
          lcd.setCursor(8, 1);
          lcd.print("        ");
        }
        else if (pressed[10] && !record)
        {
          lcd.setCursor(0, 1);
          lcd.print("Record          ");
        }
        else if (record)
        {
          for (byte i = 0; i < 8; i++)
          {
            lcd.setCursor(i * 2, 0);
            if (i <= (playhead / 4))
              lcd.write(7);
            else
              lcd.write(5);
          }
          lcd.setCursor(0, 1);
          lcd.print("Recording       ");
        }
        else if (pressed[14] && showAutoLabel)
        {
          lcd.setCursor(0, 1);
          lcd.print("SSt Ptc         ");
        }
        else
        {
          lcd.setCursor(0, 1);
          if (seqRunning)
            lcd.print("Stop   ");
          else
            lcd.print("Play   ");
          lcd.setCursor(7, 1);
          lcd.print("Vc:");
          lcd.setCursor(10, 1);
          lcd.print(adjustVoice + 1);
          lcd.setCursor(12, 1);
          lcd.print("Rw:");
          lcd.setCursor(15, 1);
          lcd.print(seqRow + 1);
        }

        break;
      case 220:
        lcd.setCursor(0, 0);
        lcd.print("Len Int Swi BPM ");
        showValue(0, 1, sequence[currentSeq].seqLength);
        lcd.setCursor(4, 1);
        if (sequence[currentSeq].interpolate)
          lcd.print("On  ");
        else
          lcd.print("Off ");
        showValue(8, 1, sequence[currentSeq].swing);
        showValue(12, 1, bpm);
        break;
      case 250: // SEQUENCER BANK LOAD
        lcd.setCursor(0, 0);
        lcd.print("Bank: ");
        if (inFolder)
        {
          lcd.setCursor(6, 0);
          lcd.print(folderName);
          lcd.print("        ");
          if (justEnteredFolder)
          {
            lcd.setCursor(0, 1);
            lcd.print("Load Kit   [P4] ");
          }
        }
        else
        {
          lcd.setCursor(6, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        getBank();
        break;
      case 260: // SEQUENCER BANK SAVE
        lcd.setCursor(0, 0);
        lcd.print("Bank: ");

        if (inFolder)
        {
          lcd.setCursor(6, 0);
          lcd.print(folderName);
          lcd.print("        ");
        }
        else
        {
          lcd.setCursor(6, 0);
          lcd.print("Root");
          lcd.print("        ");
          lcd.setCursor(0, 1);
          lcd.print("Set Folder [P4] ");
        }
        prepSaveBank();
        if (saveConfirm)
        {
          lcd.setCursor(0, 1);
          lcd.print("    Overwrite?  ");
        }
        if (justSavedBank)
        {
          lcd.setCursor(0, 1);
          lcd.print("    Saved!      ");
          justSavedBank = false;
        }
        break;
      case 300: // SETTINGS MIDI GENERAL
        lcd.setCursor(0, 0);
        lcd.print("Out Chn Thr Sync");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        if (midiOut)
        {
          lcd.setCursor(0, 1);
          lcd.print("Yes ");
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.print("No  ");
        }
        showValue(4, 1, midiChannel);
        lcd.setCursor(8, 1);
        switch (midiThruType)
        {
          case 0:
            lcd.print("Off ");
            break;
          case 1:
            lcd.print("All ");
            break;
          case 2:
            lcd.print("Clk ");
            break;
        }
        lcd.setCursor(12, 1);
        switch (midiSync)
        {
          case 0:
            lcd.print("None");
            break;
          case 1:
            lcd.print("Mst");
            break;
          case 2:
            lcd.print("Slv");
            break;
        }
        break;
      case 310: // SETTINGS MIDI VOICE
        lcd.setCursor(0, 0);
        lcd.print("Vce ACh ACC Note");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        showValue(0, 1, settingsVoice + 1);
        if (autoChannel[settingsVoice] == 0)
        {
          lcd.setCursor(4, 1);
          lcd.print("Off");
        }
        else
          showValue(4, 1, autoChannel[settingsVoice]);
        showValue(8, 1, autoCC[settingsVoice]);
        showValue(12, 1, voiceNoteNumber[settingsVoice]);
        break;
      case 320: // SETTINGS MIDI TRIGGER
        lcd.setCursor(0, 0);
        lcd.print("Trg Chn No. Note");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        if (midiTriggerOut)
        {
          lcd.setCursor(0, 1);
          lcd.print("Yes ");
        }
        else
        {
          lcd.setCursor(0, 1);
          lcd.print("No  ");
        }
        showValue(4, 1, midiTriggerChannel);
        showValue(8, 1, editTrigger + 1);
        showValue(12, 1, midiTrigger[editTrigger]);
        break;
      case 330: // SETTINGS SEQUENCER
        lcd.setCursor(0, 0);
        lcd.print("Load    Kit BPM ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        lcd.setCursor(8, 1);
        if (recallKit)
          lcd.print("Yes");
        else
          lcd.print("No");
        lcd.setCursor(12, 1);
        if (recallTempo)
          lcd.print("Yes");
        else
          lcd.print("No");
        break;
              case 340: // SETTINGS SEQUENCER
        lcd.setCursor(0, 0);
        lcd.print("Shp Vol         ");
        lcd.setCursor(0, 1);
        lcd.print("                ");
        showValue(0, 1, waveShapeAmount);
        showValue(4, 1, finalVolume >> 2);        
        break;
    }
  }
}


