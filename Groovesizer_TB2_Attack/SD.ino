// *** WAVE FILE HEADER ***
typedef struct {
  char RIFF[4];
  int32_t chunk_size;
  char WAVE[4];
  char fmt_[4];
  int32_t subchunk1_size;
  int16_t audio_format;
  int16_t num_channels;
  int32_t sample_rate;
  int32_t byte_rate;
  int16_t block_align;
  int16_t bits_per_sample;
  char DATA[4];
  int32_t subchunk2_size;
} wave_header;

wave_header header[1]; // we have to create an array of 1 member here, because file.read() insists on reading to an array

void loadWave(int vce, char fName[])
{
  if (!file.open(fName))
  {
    lcd.setCursor(0, 1);
    lcd.print("Uhhh...Nope!    ");
  }
  else
  {
    getShortName(vce, fName);
    file.seekSet(0);
    file.read(header, 44);
    wrongFormat = false;
    // mono, 22kHz, 16bit ******** CHECKED *********
    if (header[0].num_channels == 1 && header[0].sample_rate <= 22050 && header[0].bits_per_sample == 16)
    {
      voice[vce].wavLength = (header[0].subchunk2_size / 2 > 5500) ? 5500 : header[0].subchunk2_size / 2;
      voice[vce].sampleEnd = voice[vce].wavLength;
      file.seekSet(44); // just after the header data
      clearBuffer();
      file.read(sdBuffer, voice[vce].wavLength * 2); // be have to double wavLength, because it's in 16bit ints, and we'll be reading bytes
      file.close();
      for (int i = 0; i < 5500; i++)
      {
        voice[vce].wave[i] = (sdBuffer[i] >> 4) + 2048;
      }
    }
    // stereo, 22.05kHz, 16bit or mono, 44.1kHz, 16bit ******** CHECKED *********
    else if ((header[0].num_channels == 2 && header[0].sample_rate <= 22050 && header[0].bits_per_sample == 16) || (header[0].num_channels == 1 && header[0].sample_rate <= 44100 && header[0].bits_per_sample == 16))
    {
      voice[vce].wavLength = (header[0].subchunk2_size / 4 > 5500) ? 5500 : header[0].subchunk2_size / 4;
      voice[vce].sampleEnd = voice[vce].wavLength;
      file.seekSet(44); // just after the header data
      byte reads = constrain(voice[vce].wavLength / 2750, 1, 2);
      for (byte j = 0; j < reads; j++)
      {
        uint16_t readNumBytes = (j == 0) ? 11000 : (voice[vce].wavLength - 2750) * 4;
        clearBuffer();
        file.read(sdBuffer, readNumBytes);
        for (int i = 0; i < 2750; i++)
        {
          voice[vce].wave[i + (j * 2750)] = (sdBuffer[i * 2] >> 4) + 2048;
        }
      }
      file.close();
      if (reads == 1)
      {
        for (int i = 2750; i < 5500; i++)
          voice[vce].wave[i] = 2048;  // write silence to the end of the voice
      }
    }
    // stereo, 44.1kHz, 16bit ******** CHECKED *********
    else if (header[0].num_channels == 2 && header[0].sample_rate <= 44100 && header[0].bits_per_sample == 16)
    {
      voice[vce].wavLength = (header[0].subchunk2_size / 8 > 5500) ? 5500 : header[0].subchunk2_size / 8;
      voice[vce].sampleEnd = voice[vce].wavLength;
      file.seekSet(44); // just after the header data
      byte reads = constrain(voice[vce].wavLength / 1375, 1, 4);
      for (byte j = 0; j < reads; j++)
      {
        uint16_t readNumBytes = (j == (reads - 1)) ? ((voice[vce].wavLength - (1375 * (reads - 1))) * 4) : 11000;
        clearBuffer();
        file.read(sdBuffer, readNumBytes);
        for (int i = 0; i < 1375; i++)
        {
          voice[vce].wave[i + (j * 1375)] = (sdBuffer[i * 4] >> 4) + 2048;
        }
      }
      file.close();
      while (reads < 4)
      {
        for (int i = 0; i < 1375; i++)
          voice[vce].wave[i + (reads * 1375)] = 2048;  // write silence to the end of the voice
        reads++;
      }
    }
    // mono, 44.1kHz, 24bit ******** CHECKED *********
    else if (header[0].num_channels == 1 && header[0].sample_rate <= 44100 && header[0].bits_per_sample == 24)
    {
      int32_t buffer32[1];
      voice[vce].wavLength = (header[0].subchunk2_size / 6 > 5500) ? 5500 : header[0].subchunk2_size / 6;
      voice[vce].sampleEnd = voice[vce].wavLength;
      file.seekSet(44); // just after the header data
      int reads = voice[vce].wavLength;
      for (int j = 0; j < reads; j++)
      {
        buffer32[0] = 0;
        file.seekSet(44 + (j * 6));
        file.read(buffer32, 3);
        if bitRead(buffer32[0], 23) // check if the value was negative and flip it
        {
          for (byte i = 0; i < 24; i++)
          {
            if bitRead(buffer32[0], i)
              bitClear(buffer32[0], i);
            else
              bitSet(buffer32[0], i);
          }
          buffer32[0] = buffer32[0] * -1;
        }
        voice[vce].wave[j] = (buffer32[0] >> 12)  + 2048;
      }
      file.close();
      while (reads < 5500)
      {
        voice[vce].wave[reads] = 2048;  // write silence to the end of the voice
        reads++;
      }
      file.close();
    }
    // stereo, 44.1kHz, 24bit
    else if (header[0].num_channels == 2 && header[0].sample_rate <= 44100 && header[0].bits_per_sample == 24)
    {
      int32_t buffer32[1];
      voice[vce].wavLength = (header[0].subchunk2_size / 12 > 5500) ? 5500 : header[0].subchunk2_size / 12;
      voice[vce].sampleEnd = voice[vce].wavLength;
      file.seekSet(44); // just after the header data
      int reads = voice[vce].wavLength;
      for (int j = 0; j < reads; j++)
      {
        buffer32[0] = 0;
        file.seekSet(44 + (j * 12));
        file.read(buffer32, 3);
        file.read(buffer32, 3);
        if bitRead(buffer32[0], 23) // check if the value was negative and flip it
        {
          for (byte i = 0; i < 24; i++)
          {
            if bitRead(buffer32[0], i)
              bitClear(buffer32[0], i);
            else
              bitSet(buffer32[0], i);
          }
          buffer32[0] = buffer32[0] * -1;
        }
        voice[vce].wave[j] = (buffer32[0] >> 12)  + 2048;
      }
      file.close();
      while (reads < 5500)
      {
        voice[vce].wave[reads] = 2048;  // write silence to the end of the voice
        reads++;
      }
    }

    else
    {
      wrongFormat = true;
      file.close();
    }
    voiceLoaded = true;
  }
}

void getShortName(int vce, char tempName[13])
{
  for (int i = 0; i < 8; i++)
  {
    if (tempName[i] != (char)46)
      voice[vce].shortName[i] = tempName[i];
    else
    {
      voice[vce].shortName[i] = NULL;
      break;
    }
  }
}

void gotoRootDir()
{
  inFolder = false;
  sd.chdir(); // change current directory to root
  sd.vwd()->rewind();
}

void getDirCount()
{
  sd.vwd()->rewind();
  tempCount = 0;
  while (file.openNext(sd.vwd(), O_READ))
  {
    //if (!macFile())
    tempCount++;
    file.close();
  }
  dirCount = tempCount;
  dirChoice = 0;
  sd.vwd()->rewind();
  dirChecked = true;
  valueChange = true;
}

void setFolder()
{
  dirCount = 0;
  dirChecked = false;
  switch (inFolder)
  {
    case true:
      inFolder = false;
      sd.chdir(); // change current directory to root
      sd.vwd()->rewind();
      clearJust();
      valueChange = true;
      break;

    case false:
      inFolder = true;
      sd.chdir(folderName); // change current directory to folderName
      sd.vwd()->rewind();
      clearJust();
      valueChange = true;
      break;
  }
}

void prepNextChoice()
{
  sd.vwd()->rewind();
  tempCount = 0;
  voiceLoaded = false;
  kitLoaded = false;
  seqLoaded = false;
}

void getVoice()
{
  if (dirCount == 0 && dirChecked == false)
  {
    getDirCount();
  }

  else
  {
    while (tempCount < dirChoice)
    {
      file.openNext(sd.vwd(), O_READ);
      tempCount++;
      file.getFilename(fileName);
      if (file.isDir())
      {
        folder = true;
        file.getFilename(folderName);
      }
      else
        folder = false;
      file.close();
    }
    if (tempCount == dirChoice)
    {
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(fileName);
      if (folder)
        arrow(strlen(fileName) + 4, 1);
      lcd.print("            "); // sloppy way to clear excess characters
    }
    if (!voiceLoaded)
    {
      if (checkExtension(".WAV"))
        loadWave(adjustVoice, fileName);
    }
  }
}

boolean checkExtension(char ext[]) // checks the extension of the file, use argument like ".WAV", ".TB2"
{
  byte nameLength = strlen(fileName);
  if (nameLength < 6)
    return false;
  else
  {
    char lastFour[5];
    for (byte i = 0; i < 5; i++)
      lastFour[i] = fileName[nameLength - 4 + i]; // create a string with only the last four chars
    if (strcmp(ext, lastFour) == 0)
      return true;
    else
      return false;
  }
}

void saveKit() // we separate this from writeKit() so we can call it from saveSeqBank()
{
  file.open(fileName, O_RDWR | O_CREAT); // create file if it doesn't exist and open the file for write
  lcd.setCursor(0, 1);
  writeKit();
  file.close();
  justSavedKit = true;
  valueChange = true;
}
void writeKit()
{
  // pack & write voice variables
  for (byte i = 0; i < 6; i++)
  {
    sdBuffer[1 + (i * 50)] = (int16_t)voice[i].defaultVolume; // 50 is arbitrarily more than the 19 we need - for future use
    sdBuffer[2 + (i * 50)] = (int16_t)voice[i].defaultLoseBits;
    for (byte j = 0; j < 9; j++)
      sdBuffer[3 + j + (i * 50)] = (int16_t)voice[i].shortName[j];
    sdBuffer[12 + (i * 50)] = (int16_t)voice[i].defaultSampleStart;
    sdBuffer[13 + (i * 50)] = (int16_t)voice[i].sampleStart;
    sdBuffer[14 + (i * 50)] = (int16_t)voice[i].sampleEnd;
    sdBuffer[15 + (i * 50)] = (int16_t)voice[i].pan;
    sdBuffer[16 + (i * 50)] = (int16_t)voice[i].velSStFactor;
    sdBuffer[17 + (i * 50)] = (int16_t)voice[i].velPitchFactor;
    sdBuffer[18 + (i * 50)] = (int16_t)voice[i].velBitFactor;
    sdBuffer[19 + (i * 50)] = (int16_t)voice[i].velVolFactor;
    sdBuffer[20 + (i * 50)] = (int16_t)voice[i].increment;
    sdBuffer[21 + (i * 50)] = (int16_t)voice[i].defaultIncrement;
  }
  if (file.write(sdBuffer, 11000) != -1) // note - we are writing 5500 2 byte ints from the sd buffer to 11000 bytes on the SD
  {
    if (file.sync())
    {
      lcd.print(".");
    }
  }
  // pack and write voice wav data
  for (byte k = 0; k < 6; k++)
  {
    for (int m = 0; m < 5500; m++)
      sdBuffer[m] = voice[k].wave[m];
    if (file.write(sdBuffer, 11000) != -1) // note - we are writing 5500 2 byte ints from the sd buffer to 11000 bytes on the SD
    {
      if (file.sync())
        lcd.print(".");
    }
  }
}

void loadKit() // separate this from readKit() so we can call it from loadSeqBank()
{
  file.open(fileName); // create file if it doesn't exist and open the file for write
  readKit();
  file.close();
  kitLoaded = true;
}

void readKit()
{
  // read and unpack voice variables
  if (file.read(sdBuffer, 11000) == 11000) // note - we are reading 11000 bytes to a buffer of 5500 2 byte ints
  {
    for (byte i = 0; i < 6; i++)
    {
      voice[i].defaultVolume = (int)sdBuffer[1 + (i * 50)];
      voice[i].defaultLoseBits = (int)sdBuffer[2 + (i * 50)];
      for (byte j = 0; j < 9; j++)
        voice[i].shortName[j] = (char)sdBuffer[3 + j + (i * 50)];
      voice[i].defaultSampleStart = (int)sdBuffer[12 + (i * 50)];
      voice[i].sampleStart = (int)sdBuffer[13 + (i * 50)];
      voice[i].sampleEnd = (int)sdBuffer[14 + (i * 50)];
      voice[i].pan = (int)sdBuffer[15 + (i * 50)];
      voice[i].velSStFactor = (int)sdBuffer[16 + (i * 50)];
      voice[i].velPitchFactor = (int)sdBuffer[17 + (i * 50)];
      voice[i].velBitFactor = (int)sdBuffer[18 + (i * 50)];
      voice[i].velVolFactor = (int)sdBuffer[19 + (i * 50)];
      voice[i].increment = (int)sdBuffer[20 + (i * 50)];
      voice[i].defaultIncrement = (int)sdBuffer[21 + (i * 50)];
    }
  }
  // read and unpack voice wav data
  for (byte k = 0; k < 6; k++)
  {
    if (file.read(sdBuffer, 11000) == 11000) // note - we are reading 11000 bytes to a buffer of 5500 2 byte ints
    {
      for (int m = 0; m < 5500; m++)
        voice[k].wave[m] = sdBuffer[m];
    }
  }
}

void getKit()
{
  while (tempCount < dirChoice && (file.openNext(sd.vwd(), O_READ)))
  {
    tempCount++;
    file.getFilename(fileName);
    if (file.isDir())
    {
      folder = true;
      file.getFilename(folderName);
    }
    else
      folder = false;
    file.close();
  }
  if (tempCount == dirChoice)
  {
    lcd.setCursor(0, 1);
    lcd.print("    ");
    lcd.print(fileName);
    if (folder)
      arrow(strlen(fileName) + 4, 1);
    lcd.print("            "); // sloppy way to clear excess characters
  }
  if (!kitLoaded)
  {
    if (checkExtension(".KIT"))
      loadKit();
  }
}

void prepSaveKit()
{
  if (dirCount == 0 && dirChecked == false)
  {
    static uint16_t tempCount = 0;
    while (file.openNext(sd.vwd(), O_READ))
    {
      file.close();
      dirCount++;
    }
    dirChoice = 0;
    sd.vwd()->rewind();
    dirChecked = true;
    valueChange = true;
    if (dirCount < 98)
    {
      numberName = dirCount + 1;
      if (numberName < 10)
      {
        sprintf(saveName, "0%d.KIT", numberName);
      }
      else
      {
        sprintf(saveName, "%d.KIT", numberName);
      }
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(saveName);
      lcd.print("      ");
      strcpy(fileName, saveName);
    }
    else
    {
      lcd.setCursor(0, 1);
      lcd.print("    Folder Full!");
    }
  }

  else
  {
    while (tempCount < dirChoice)
    {
      file.openNext(sd.vwd(), O_READ);
      tempCount++;
      file.getFilename(fileName);
      if (file.isDir())
      {
        folder = true;
        file.getFilename(folderName);
      }
      else
        folder = false;
      file.close();
    }
    if (tempCount == dirChoice)
    {
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(fileName);
      if (folder)
        arrow(strlen(fileName) + 4, 1);
      lcd.print("            "); // sloppy way to clear excess characters
    }
  }
}

void saveSeqBank()
{
  file.open(fileName, O_RDWR | O_CREAT); // create file if it doesn't exist and open the file for write


  // pack & write sequences
  for (byte h = 0; h < 8; h++) // for each sequence in the bank
  {
    for (byte i = 0; i < 6; i++) // for each voice
    {
      for (byte j = 0; j < 32; j++)
      {
        sdBuffer[(h * 680) + (i * 112) + j] = sequence[h].seq[i][j]; // 680 int allotment per sequence, 112 int allotment per voice
        sdBuffer[(h * 680) + (i * 112) + 32 + j] = sequence[h].startAuto[i][j];
        sdBuffer[(h * 680) + (i * 112) + 64 + j] = sequence[h].pitchAuto[i][j];
      }
      sdBuffer[(h * 680) + (i * 112) + 96] = sequence[h].ssAuto[i];
      sdBuffer[(h * 680) + (i * 112) + 97] = sequence[h].pAuto[i];
      sdBuffer[(h * 680) + (i * 112) + 98] = sequence[h].seqLength;
      sdBuffer[(h * 680) + (i * 112) + 99] = sequence[h].swing;
      sdBuffer[(h * 680) + (i * 112) + 100] = sequence[h].interpolate;
    }
  }
  sdBuffer[5498] = seqBpm;
  sdBuffer[5499] = bankMode;
  if (file.write(sdBuffer, 11000) != -1) // note - we are writing 5500 2 byte ints from the sd buffer to 11000 bytes on the SD
  {
    if (file.sync())
    {
      lcd.setCursor(0, 1);
      lcd.print(".");
    }
  }
  writeKit();
  file.close();
  justSavedBank = true;
  valueChange = true;
}

void loadSeqBank()
{
  file.open(fileName);
  // read and unpack sequence bank
  if (file.read(sdBuffer, 11000) == 11000) // note - we are reading 11000 bytes to a buffer of 5500 2 byte ints
  {
    for (byte h = 0; h < 8; h++) // for each sequence in the bank
    {
      for (byte i = 0; i < 6; i++) // for each voice
      {
        for (byte j = 0; j < 32; j++)
        {
          sequence[h].seq[i][j] = sdBuffer[(h * 680) + (i * 112) + j]; // 680 int allotment per sequence, 112 int allotment per voice
          sequence[h].startAuto[i][j] = sdBuffer[(h * 680) + (i * 112) + 32 + j];
          sequence[h].pitchAuto[i][j] = sdBuffer[(h * 680) + (i * 112) + 64 + j];
        }
        sequence[h].ssAuto[i] = sdBuffer[(h * 680) + (i * 112) + 96];
        sequence[h].pAuto[i] = sdBuffer[(h * 680) + (i * 112) + 97];
        sequence[h].seqLength = sdBuffer[(h * 680) + (i * 112) + 98];
        sequence[h].swing = sdBuffer[(h * 680) + (i * 112) + 99];
        sequence[h].interpolate = sdBuffer[(h * 680) + (i * 112) + 100];
      }
    }
    seqBpm = sdBuffer[5498];
    if (recallTempo && !(receivingClock && midiSync == 2) && bpm != seqBpm)
      bpmChange(seqBpm);
    bankMode = sdBuffer[5499];
  }
  if (recallKit)
    readKit();
  lcd.setCursor(0, 1);
  lcd.print("      Loaded!   ");
  file.close();
  seqLoaded = true;
}

void prepSaveBank()
{
  if (dirCount == 0 && dirChecked == false)
  {
    static uint16_t tempCount = 0;
    while (file.openNext(sd.vwd(), O_READ))
    {
      file.close();
      dirCount++;
    }
    dirChoice = 0;
    sd.vwd()->rewind();
    dirChecked = true;
    valueChange = true;
    if (dirCount < 98)
    {
      numberName = dirCount + 1;
      if (numberName < 10)
      {
        sprintf(saveName, "0%d.SEQ", numberName);
      }
      else
      {
        sprintf(saveName, "%d.SEQ", numberName);
      }
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(saveName);
      lcd.print("      ");
      strcpy(fileName, saveName);
    }
    else
    {
      lcd.setCursor(0, 1);
      lcd.print("    Folder Full!");
    }
  }

  else
  {
    while (tempCount < dirChoice)
    {
      file.openNext(sd.vwd(), O_READ);
      tempCount++;
      file.getFilename(fileName);
      if (file.isDir())
      {
        folder = true;
        file.getFilename(folderName);
      }
      else
        folder = false;
      file.close();
    }
    if (tempCount == dirChoice)
    {
      lcd.setCursor(0, 1);
      lcd.print("    ");
      lcd.print(fileName);
      if (folder)
        arrow(strlen(fileName) + 4, 1);
      lcd.print("            "); // sloppy way to clear excess characters
    }
  }
}

void getBank()
{
  while (tempCount < dirChoice && (file.openNext(sd.vwd(), O_READ)))
  {
    tempCount++;
    file.getFilename(fileName);
    if (file.isDir())
    {
      folder = true;
      file.getFilename(folderName);
    }
    else
      folder = false;
    file.close();
  }
  if (tempCount == dirChoice)
  {
    lcd.setCursor(0, 1);
    lcd.print("    ");
    lcd.print(fileName);
    if (folder)
      arrow(strlen(fileName) + 4, 1);
    lcd.print("            "); // sloppy way to clear excess characters
  }
  if (!seqLoaded)
  {
    if (checkExtension(".SEQ"))
      loadSeqBank();
  }
}

void clearBuffer()
{
  for (int j = 0; j < 5500; j++) // make sure the sdBuffer is clean, as we'll copy the whole buffer to the voice's wave array later
    sdBuffer[j] = 0;
}

void savePrefs()
{
  // pack preferences
  sdBuffer[0] = midiThruType;
  sdBuffer[1] = midiSync; // call setSyncType() when recalling
  sdBuffer[2] = midiChannel;
  for (byte i = 0; i < 6; i++)
    sdBuffer[3 + i] = voiceNoteNumber[i];
  for (byte i = 0; i < 6; i++)
    sdBuffer[9 + i] = autoChannel[i];
  for (byte i = 0; i < 6; i++)
    sdBuffer[15 + i] = autoCC[6];
  sdBuffer[21] = recallKit;
  sdBuffer[22] = recallTempo;
  sdBuffer[23] = waveShapeAmount;
  sdBuffer[24] = midiOut;
  sdBuffer[25] = midiTriggerOut;
  sdBuffer[26] = midiTriggerChannel;
  for (int i = 0; i < 8; i++)
    sdBuffer[27 + i] = midiTrigger[i];
  sdBuffer[35] = finalVolume;
  // sd.chdir(); // change current directory to root
  // write the preferences
  file.open("TB2ATTCK.SET", O_RDWR | O_CREAT); // create file if it doesn't exist and open the file for write
  if (file.write(sdBuffer, 100) != -1) // note - we are writing 50 2 byte ints from the sd buffer to 100 bytes on the SD
  {
    if (file.sync())
    {
      lcd.setCursor(8, 1);
      lcd.print("Settings Saved! ");
    }
  }
  file.close();
}

void loadPrefs()
{
  sd.chdir(); // change current directory to root
  // if we're in the root and we can't open the file, make it with savePreferences()
  if (!file.open("TB2ATTCK.SET"))
  {
    savePrefs();
  }
  else // load the preferences
  {
    if (file.read(sdBuffer, 100) == 100) // note - we are reading 100 bytes to a buffer of 50 2 byte ints
    {
      // unpack the preferences
      midiThruType = sdBuffer[0];
      checkThru();
      midiSync = sdBuffer[1]; // call setSyncType() when recalling
      midiChannel = sdBuffer[2];
      for (byte i = 0; i < 6; i++)
        voiceNoteNumber[i] = sdBuffer[3 + i];
      for (byte i = 0; i < 6; i++)
        autoChannel[i] = sdBuffer[9 + i];
      for (byte i = 0; i < 6; i++)
        autoCC[6] = sdBuffer[15 + i];
      recallKit = sdBuffer[21];
      recallTempo = sdBuffer[22];
      waveShapeAmount = sdBuffer[23];
      midiOut = sdBuffer[24];
      midiTriggerOut = sdBuffer[25];
      midiTriggerChannel = sdBuffer[26];
      for (int i = 0; i < 8; i++)
        midiTrigger[i] = sdBuffer[27 + i];
      finalVolume = constrain(sdBuffer[35], 0, 1023); // the constrain here to prevent crashed if the value has not been saved before (coming from earlier version of firmware) 
    }
    file.close();
  }
}

void loadDefault()
{
  // KICK
  for (int i = 0; i < 5500; i++)
    voice[0].wave[i] = KICK[i];
  voice[0].sampleEnd = 5499;
  voice[0].wavLength = 5499;

  // SNARE
  for (int i = 0; i < 5500; i++)
    voice[1].wave[i] = SNARE[i];
  voice[1].sampleEnd = 5499;
  voice[1].wavLength = 5499;

  // HAT CLOSED
  for (int i = 0; i < 5312; i++)
    voice[2].wave[i] = HAT_CL[i];
  voice[2].sampleEnd = 5311;
  voice[2].wavLength = 5311;

  // HAT OPEN
  for (int i = 0; i < 5500; i++)
    voice[3].wave[i] = HAT_OP[i];
  voice[3].sampleEnd = 5499;
  voice[3].wavLength = 5499;

  // FX1
  for (int i = 0; i < 5500; i++)
    voice[4].wave[i] = FX1[i];
  voice[4].sampleEnd = 5499;
  voice[4].wavLength = 5499;

  // FX2
  for (int i = 0; i < 5500; i++)
    voice[5].wave[i] = FX2[i];
  voice[5].sampleEnd = 5499;
  voice[5].wavLength = 5499;


  char* fileNames[] = {
    "KICK.wav",
    "SNARE.wav",
    "HAT_CL.wav",
    "HAT_OP.wav",
    "FX1.wav",
    "FX2.wav"
  };

  for (byte i = 0; i < 6; i++)
  {
    getShortName(i, fileNames[i]);
  }

  voice[2].pan = 256;
  voice[3].pan = 712;
  voice[4].pan = 0;
  voice[5].pan = 1023;
}
