function initWM(a, b) {
  screen = new Screen();
  w = new Window();
  screen.addWindow(w);
  return a + '...' + b + screen.windowCount;
}
