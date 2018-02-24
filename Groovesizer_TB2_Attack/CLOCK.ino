// *** 96PPQ INTERNAL CLOCK ***

void clockHandler()
{
  if (ticks == 0 || ticks == (24 + sequence[currentSeq].swing))
  {
    doStep();
  }

  else
  {
    if (seqRunning && sequence[currentSeq].interpolate)
    {
      for (int i = 0; i < 6; i++)
      {
        if (sequence[currentSeq].pAuto[i] && !sequence[currentSeq].editingAuto[i])
        {
          int nextStep = (playhead + 1) % sequence[currentSeq].seqLength;
          int targetPitch = (sequence[currentSeq].pitchAuto[i][nextStep] == 255) ? voice[i].defaultIncrement : sequence[currentSeq].pitchAuto[i][nextStep] << 3;
          if (ticks < (24 + sequence[currentSeq].swing))
            voice[i].increment = map(ticks, 0, (24 + sequence[currentSeq].swing), voice[i].increment, targetPitch);
          else
            voice[i].increment = map(ticks, (24 + sequence[currentSeq].swing), 47, voice[i].increment, targetPitch);
        }
      }
    }
  }

  // *** MIDI CLOCK OUT ***
  if (midiClockOut && clockOutCounter == 0)
    midiA.sendRealTime(midi::Clock); // send a midi clock pulse

  if (clockOutCounter < 3)
    clockOutCounter++;
  else
    clockOutCounter = 0;

  // *** RECORD ***
  if (record)
  {
    if (ticks < 12)
    {
      targetStep = (playhead - 1) % sequence[currentSeq].seqLength;
      beforeTargetStep = false;
    }
    else if (ticks < 24)
    {
      targetStep = playhead % sequence[currentSeq].seqLength;
      beforeTargetStep = true;
    }
    else if (ticks < 36)
    {
      targetStep = (playhead != 0) ? (playhead - 1) % sequence[currentSeq].seqLength : sequence[currentSeq].seqLength - 1;
      beforeTargetStep = false;
    }
    else
    {
      targetStep = playhead % sequence[currentSeq].seqLength;
      beforeTargetStep = true;
    }
  }


  // *** METRONOME ***
  if (ticks == 0)
  {
    if (metroCounter % 2 == 0)
    {
      metroIndex = 0;
      valueChange = true; // so we can update the LCD
    }

    if (metroCounter % 8 == 0)
    {
      metroIncrement = 2; // octave up
      metroCounter = 0;
      metroIndex = 0;
    }
    else
      metroIncrement = 1; // normal octave
    metroCounter++;
  }

  // *** FLAM ***
  for (int i = 0; i < 6; i++)
  {
    if (voice[i].nextFlam != 255)
    {
      if (flamTicks == voice[i].nextFlam)
      {
        hit(i, voice[i].flamVelocity);
        scheduleFlam(i, voice[i].flamVelocity);
      }
    }
  }

  // *** GRAIN FX ***
  if (grainRhythm)
  {
    if (ticks == 1) // offset of 1 tick so the grain buffer has time to fill
      grainPlay = true;
    else if (ticks == (25 + sequence[currentSeq].swing))
      grainPlay = false;
  }
  if (grainRhythm2)
  {
    if (ticks == 1)
    {
      grabBuffer = true;

      if ((ticks % 12) == 1)
      {
        grabBuffer = false;
        grainPlay = !grainPlay;
        int tmp = 1023 / sequence[currentSeq].seqLength;
        grainPitch = realStep * tmp;
      }
    }
  }

  // *** FLANGE FX ***
  static int8_t flangeAdvance = 1;
  flangeDelay += flangeAdvance;
  if (flangeDelay == 100)
    flangeAdvance = -1;
  else if (flangeDelay == 0)
    flangeAdvance = 1;


  ticks++;
  if (!(syncIn && receivingClock))
  {
    if (ticks == 48)
      ticks = 0;
  }

  flamTicks++;
  if (flamTicks == 96)
    flamTicks = 0;
}

void bpmChange(int BPM)
{
  bpm = BPM;
  Timer4.setPeriod(60000000 / bpm / 96).start(); // set the timer to the new bpm
}
