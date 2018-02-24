void doStep()
{
  if (seqRunning)
  {
    if (playhead == 0)
    {
      if (currentSeq != cuedSeq)
        currentSeq = cuedSeq;
      cueNextSeq();
    }
    for (int i = 0; i < 6; i++) // for each voice
    {
      if (sequence[currentSeq].ssAuto[i]) // update sample start from sequencer
      {
        if (sequence[currentSeq].startAuto[i][playhead] != 255)
          voice[i].sampleStart = sequence[currentSeq].startAuto[i][playhead] << 5;
        else // the sequencer value is 255 / unassigned
          voice[i].sampleStart = voice[i].defaultSampleStart;
      }
      if (sequence[currentSeq].pAuto[i]) // update pitch from sequencer
      {
        if (!sequence[currentSeq].editingAuto[i])
        {
          if (sequence[currentSeq].pitchAuto[i][playhead] != 255)
          {
            voice[i].increment = sequence[currentSeq].pitchAuto[i][playhead] << 3;
            autoVal[2] = sequence[currentSeq].pitchAuto[i][playhead];
            if (autoChannel[i]) // ie. autoChannel[i] != 0
              midiA.sendControlChange(autoCC[i], constrain(sequence[currentSeq].pitchAuto[i][playhead], 0, 127), autoChannel[i]);
          }
          else // the sequencer value is 255 / unassigned
          {
            voice[i].increment = voice[i].defaultIncrement;
            if (autoChannel[i])
              midiA.sendControlChange(autoCC[i], constrain(voice[i].defaultIncrement >> 3, 0, 127), autoChannel[i]);
          }
        }
        else // if we're busy editing automation
        {
          if (autoChannel[i])
            midiA.sendControlChange(autoCC[i], autoVal[i], autoChannel[i]);
        }
      }
      if (sequence[currentSeq].seq[i][playhead])
      {
        switch (sequence[currentSeq].seq[i][playhead])
        {
          case 1:
            hit(i, voice[i].standardVel);
            voice[i].nextFlam = 255; // off
            break;
          case 2:
            hit(i, voice[i].accentVel);
            voice[i].nextFlam = 255; // off
            break;
          case 3:
            hit(i, voice[i].standardVel);
            scheduleFlam(i, voice[i].standardVel);
            break;
          case 4:
            hit(i, voice[i].accentVel);
            scheduleFlam(i, voice[i].accentVel);
            break;
          case 5: // placeholder for standard velocity (prevents double triggers when recording)
            sequence[currentSeq].seq[i][playhead] = 1;
            break;
          case 6: // placeholder for accent velocity
            sequence[currentSeq].seq[i][playhead] = 2;
            break;
          case 7: // placeholder for standard flam
            sequence[currentSeq].seq[i][playhead] = 3;
            break;
          case 8: // placeholder for accent
            sequence[currentSeq].seq[i][playhead] = 4;
            break;
        }
      }
    }
  }
  realStep = (realStep + 1) % sequence[currentSeq].seqLength;
  if (!beatRepeat)
    playhead = realStep;
}

void seqReset()
{
  realStep = 0;
  playhead = 0;
  ticks = 0;
  metroCounter = 0;
  inPulseCounter = 0;
}

void clearSeq(int thisSequence, int voice)
{
  for (int i = 0; i < 32; i++)
    sequence[thisSequence].seq[voice][i] = 0;
  clearStartAuto(thisSequence, voice);
  clearPitchAuto(thisSequence, voice);
}

void clearStartAuto(int thisSequence, int voice)
{
  for (int i = 0; i < 32; i++)
    sequence[thisSequence].startAuto[voice][i] = 255;
}

void clearPitchAuto(int thisSequence, int voice)
{
  for (int i = 0; i < 32; i++)
    sequence[thisSequence].pitchAuto[voice][i] = 255;
}

void ssAutoOff(int thisSequence, int voice) // turn off sample start automation
{
  sequence[thisSequence].ssAuto[voice] = false;
}

void pAutoOff(int thisSequence, int voice) // turn off pitch automation
{
  sequence[thisSequence].pAuto[voice] = false;
}

void scheduleFlam(int vce, int velo)
{
  if ((velo - voice[vce].flamDecay) > 0)
  {
    voice[vce].flamVelocity = velo - voice[vce].flamDecay;
    if (voice[vce].nextFlam == 255) // for the first flam
      voice[vce].nextFlam = (flamTicks + voice[vce].flamTime) % 96;
    else
      voice[vce].nextFlam = (voice[vce].nextFlam + voice[vce].flamTime) % 96;
  }
  else
    voice[vce].nextFlam = 255; // off
}

void copySeq() // copy the current sequence to another location
{
  for (byte i = 0; i < 6; i++) // for each voice
  {
    for (byte j = 0; j < 32; j++)
    {
      sequence[destinationSeq].seq[i][j] = sequence[sourceSeq].seq[i][j];
      sequence[destinationSeq].startAuto[i][j] = sequence[sourceSeq].startAuto[i][j];
      sequence[destinationSeq].pitchAuto[i][j] = sequence[sourceSeq].pitchAuto[i][j];
    }
    sequence[destinationSeq].ssAuto[i] = sequence[sourceSeq].ssAuto[i];
    sequence[destinationSeq].pAuto[i] = sequence[sourceSeq].pAuto[i];
    sequence[destinationSeq].seqLength = sequence[sourceSeq].seqLength;
    sequence[destinationSeq].swing = sequence[sourceSeq].swing;
    seqBpm = seqBpm;
    sequence[destinationSeq].interpolate = sequence[sourceSeq].interpolate;
  }
  copied = true;
}

void cueNextSeq()
{
  switch (bankMode)
  {
    case 0: // do nothing
      break;
    case 1: // loop 2
      if ((currentSeq % 2) == 0) // for even numbered sequences
        cuedSeq = currentSeq + 1;
      else
        cuedSeq = currentSeq - 1;
      break;
    case 2: // loop 4
      if (currentSeq < 3)
        cuedSeq = currentSeq + 1;
      else if (currentSeq == 3)
        cuedSeq = 0;
      else if (currentSeq < 7)
        cuedSeq = currentSeq + 1;
      else if (currentSeq == 7)
        cuedSeq = 4;
      break;
    case 3:
      if (currentSeq < 7)
        cuedSeq = currentSeq + 1;
      else
        cuedSeq = 0;
      break;
    case 4:
      do
        cuedSeq = random(0, 8); // upper bound is not included in random()
      while (cuedSeq == currentSeq); // if the random function delivers the currentSeq we'll get stuck
      break;
  }
}
