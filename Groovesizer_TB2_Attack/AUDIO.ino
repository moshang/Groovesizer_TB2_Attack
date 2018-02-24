void audioHandler()
{
  int32_t sample = 2048;
  int32_t sampleL = 0;
  int32_t sampleR = 0;


  for (int i = 0; i < 6; i++)
  {
    if (voice[i].playing)
    {
      sample = voice[i].wave[voice[i].sampleIndex] - 2048;
      sample = bitMunch(i, sample);
      sample = (sample * (voice[i].defaultVolume * voice[i].currentVolume) >> 10) >> 10;
      sampleL += (sample * (1023 - voice[i].pan)) >> 10;
      sampleR += (sample * voice[i].pan) >> 10;

      voice[i].index += (voice[i].increment * 2);

      voice[i].sampleIndex = voice[i].tempSampleStart + (voice[i].index >> 10);

      if (voice[i].sampleIndex >= voice[i].sampleEnd)
      {
        voice[i].sampleIndex = voice[i].tempSampleStart;
        voice[i].index = 0;
        if (!voice[i].looping)
          voice[i].playing = false;
      }
    }
  }

  // *** METRONOME ***

  if (metroIndex < 255 && metronome)
  {
    sampleL += metroClick[metroIndex];
    sampleR += metroClick[metroIndex];
    metroIndex = (metroIndex < 237) ? metroIndex + metroIncrement : 255;
  }

  // *** GRAIN FX ***
  int16_t grainL;
  int16_t grainR;

  if (!grainPlay)
  {
    grainL = sampleL;
    grainR = sampleR;
    if (menu == 200 && (!grainRhythm2 || grabBuffer)) // only use the sdBuffer for storing grains if we're on the SEQUENCE TRIGGER page
    {
      sdBuffer[grainIndexL] = sampleL;
      sdBuffer[grainIndexR] = sampleR;
      grainIndexL++;
      grainIndexR = grainIndexL + 256;
    }
  }
  else // playback the grain
  {
    grainL = constrain(sdBuffer[grainIndexL] + sampleL, -2047, 2047);
    grainR = constrain(sdBuffer[grainIndexR] + sampleR, -2017, 2047);
    grainAcc += (grainPitch * 4);
    grainIndexL = grainAcc >> 10;
    grainIndexR = grainIndexL + 256;
  }

  // *** FLANGE FX ***
  if (menu == 200)
  {
    // fill the flange buffer
    int16_t flangeWriteIndexL = flangeIndex + flangeOffsetL;
    int16_t flangeWriteIndexR = flangeIndex + flangeOffsetR;
    sdBuffer[flangeWriteIndexL] = grainL;
    sdBuffer[flangeWriteIndexR] = grainR;
    if (flanger)
    {
      int16_t flangeReadIndexL = (flangeIndex - flangeDelay) + flangeOffsetL;
      int16_t flangeReadIndexR = (flangeIndex - flangeDelay) + flangeOffsetR;

      grainL = constrain(grainL + (sdBuffer[flangeReadIndexL] * -1), -2047, 2047);
      grainR = constrain(grainR + (sdBuffer[flangeReadIndexR] * -1), -2047, 2047);
      /*
            grainL = (grainL < sdBuffer[flangeReadIndexL]) ? grainL + ((sdBuffer[flangeReadIndexL] - grainL) / 2) : sdBuffer[flangeReadIndexL] + ((grainL - sdBuffer[flangeReadIndexL]) / 2);
            grainR = (grainR < sdBuffer[flangeReadIndexR]) ? grainR + ((sdBuffer[flangeReadIndexR] - grainR) / 2) : sdBuffer[flangeReadIndexR] + ((grainL - sdBuffer[flangeReadIndexR]) / 2);
            */
    }
    flangeIndex++;

  }
  
  // *** APPLY OFFSET ***
  grainL += 2048;
  grainR += 2048;
  
  // *** WAVESHAPER ***
  int16_t shaperL = waveShaper[constrain(grainL, 0, 4095)];
  int16_t shaperR = waveShaper[constrain(grainR, 0, 4095)];

  // *** FINAL VOLUME ***
  int16_t finalL = constrain(((int(shaperL - 2048) * finalVolume) >> 10) + 2028, 0, 4095);
  int16_t finalR = constrain(((int(shaperR - 2048) * finalVolume) >> 10) + 2028, 0, 4095);

  // *** DAC OUTPUT ***
  // write to DAC0
  dacc_set_channel_selection(DACC_INTERFACE, 0);
  dacc_write_conversion_data(DACC_INTERFACE, finalL);
  // write to DAC1
  dacc_set_channel_selection(DACC_INTERFACE, 1);
  dacc_write_conversion_data(DACC_INTERFACE, finalR);
}


void hit(int vce, int vel)
{
  handleVelocity(vce, vel);
  voice[vce].playing = true;
  voice[vce].sampleIndex = voice[vce].sampleStart;
  voice[vce].tempSampleStart = voice[vce].sampleStart;
  voice[vce].index = 0;
  if (midiOut)
    midiA.sendNoteOn(voiceNoteNumber[vce], vel, midiChannel);
}

int bitMunch(int vce, int sample)
{
  sample = sample >> constrain(voice[vce].defaultLoseBits + voice[vce].velLoseBits, 0, 11);
  sample = sample << constrain(voice[vce].defaultLoseBits + voice[vce].velLoseBits, 0, 11);
  return sample;
}

void handleVelocity(int vce, int velo)
{
  // volume
  voice[vce].currentVolume = (127 - ((voice[vce].velVolFactor * (127 - velo)) >> 10)) << 3;

  // sample start
  if (!(seqRunning && sequence[currentSeq].ssAuto[vce]))
  {
    if (velo == 127)
      voice[vce].sampleStart = voice[vce].defaultSampleStart;
    else
      voice[vce].sampleStart = map(((127 - velo) * voice[vce].velSStFactor) >> 7, 0, 1023, voice[vce].defaultSampleStart, voice[vce].sampleEnd);
  }
  // pitch
  if (!(seqRunning && sequence[currentSeq].pAuto[vce]))
  {
    voice[vce].increment = constrain(voice[vce].defaultIncrement + ((velo * (voice[vce].velPitchFactor - 512)) >> 7), 0, 1023);
  }
  // bit muncher
  voice[vce].velLoseBits = map((velo * voice[vce].velBitFactor) >> 7, 0, 1023, 0, 11);
}
