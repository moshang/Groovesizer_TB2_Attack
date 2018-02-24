void loop() {
  midiA.read();
  checkForClock();
  checkSwitches(); // gets the current state of the buttons - defined in BUTTONS
  handlePresses(); // what to do with button presses - defined in BUTTONS
  getPots(); // update the pot values - defined in POTS
  getMenu(); // defined in UI
  adjustValues(); // defined in POTS
  updateValues(); // defined in UI - only executes if the variable valueChange is set to true
  arrowAnim(); // animate the arrow
}
