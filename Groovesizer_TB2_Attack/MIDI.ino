void HandleNoteOn(byte channel, byte note, byte velocity)
{
  if (channel == midiChannel)
  {
    midiVelocity = velocity;
    for (byte i = 0; i < 6; i++)
    {
      if (note == voiceNoteNumber[i])
        hit(i, midiVelocity);
    }
  }
  if (channel == midiTriggerChannel)
  {
    for (byte i = 0; i < 8; i++)
    {
      if (note == midiTrigger[i])
      {
        if (seqRunning)
        {
          cuedSeq = i;
        }
        else
        {
          currentSeq = i;
          cuedSeq = i;
        }
      }
    }
  }
}
void HandleNoteOff(byte channel, byte note, byte velocity)
{
}

void HandleClock(void) // what to do for 24ppq clock pulses
{
  if (syncIn)
  {
    lastClock = millis();
    receivingClock = true;
    static unsigned long lastClockInMicros = 0;
    if ((inPulseCounter % 12) == 0 && !seqJustStarted) // on each 8th except the first
    {
      ticks = 0;
    }
    else
      seqJustStarted = false;

    clockInMicros = micros();
    if (inPulseCounter != 0)
    {
      pulseMicros = micros() - lastClockInMicros;
      tmpBpm[inPulseCounter % 24] = 60000000 / (pulseMicros * 24);
    }
    if (inPulseCounter > 4)
    {
      int totalBpm = 0;
      for (byte i = 0; i < 24; i++)
        totalBpm += tmpBpm[i];

      if (bpm != totalBpm / 24 + 1)
      {
        bpmChange(totalBpm / 24 + 1); // +1 here because for some reason the BPM reading is off by one
        //  if (menu == 230)
        //    showValue(5, 1, bpm);
      }
    }
    inPulseCounter++;
    lastClockInMicros = clockInMicros;
  }
}

void HandleStart(void)
{
  if (syncIn)
  {
    seqReset();
    seqRunning = true;
    seqJustStarted = true;
  }
}

void HandleStop(void)
{
  if (syncIn)
  {
    seqRunning = false;
    seqReset();
  }
}

void checkForClock()
{
  if (receivingClock)
  {
    if ((millis() - lastClock) > 300)
      receivingClock = false;
  }
}

void checkThru()
{
  if (midiThruType == 0)
    midiA.turnThruOff();
  else if (midiThruType == 1)
    midiA.turnThruOn(midi::Full);
  else if (midiThruType == 2)
    midiA.turnThruOn(midi::DifferentChannel); // in conjunction with MIDI_CHANNEL_OMNI, this only allows system messages though like clock, stop, start
}

void setSyncType()
{
  switch (midiSync)
  {
    case 0:
      syncIn = false;
      midiClockOut = 0;
      break;
    case 1:
      syncIn = false;
      midiClockOut = 1;
      break;
    case 2:
      syncIn = true;
      midiClockOut = 0;
      break;
  }
}

void sendMidiStartStop()
{
  if (seqRunning)
  {
    if (midiClockOut)
      midiA.sendRealTime(midi::Start); // send a midi clock start signal
  }
  if (!seqRunning)
  {
    if (midiClockOut)
      midiA.sendRealTime(midi::Stop); // send a midi clock start signal
  }
}

