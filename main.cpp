#include <M5Cardputer.h>
#include <WiFi.h>
#include <Preferences.h>
#include <NimBLEDevice.h>

// 000 OS — Cardputer ADV starter firmware
// Functional modules: keyboard UI, saved accent themes, Wi-Fi scan,
// BLE discovery, IMU diagnostics and system information.
// Only inspect networks/devices you own or have permission to test.

Preferences prefs;

struct Theme { const char* name; uint16_t accent; };
Theme themes[] = {
  {"NEON GREEN", 0x07E0},
  {"CYBER BLUE", 0x05FF},
  {"VIOLET",     0xB81F},
  {"AMBER",      0xFD20},
  {"CRIMSON",    0xF986},
  {"WHITE",      0xFFFF}
};
const int THEME_COUNT = sizeof(themes) / sizeof(themes[0]);
int themeIndex = 0;
uint16_t accent() { return themes[themeIndex].accent; }
const uint16_t BG = 0x0000;
const uint16_t PANEL = 0x0841;

enum Page { HOME, WIFI_PAGE, BLE_PAGE, STYLE_PAGE, DIAG_PAGE, ABOUT_PAGE };
Page page = HOME;
int menuIndex = 0;
const char* menuItems[] = {"Wi-Fi Scanner", "Bluetooth Explorer", "O-STYLE Colours", "Device Diagnostics", "About 000"};
const int MENU_COUNT = sizeof(menuItems) / sizeof(menuItems[0]);
String statusLine = "SYSTEM READY";

void header(const char* title) {
  auto &d = M5Cardputer.Display;
  d.fillScreen(BG);
  d.setTextColor(accent(), BG);
  d.setTextSize(1);
  d.setCursor(5, 4); d.print("000 OS");
  d.setTextColor(0x7BEF, BG);
  d.setCursor(165, 4); d.print("CARDPUTER ADV");
  d.drawFastHLine(4, 17, d.width()-8, accent());
  d.setTextColor(accent(), BG);
  d.setCursor(5, 23); d.print(title);
}

void footer(const char* text = "W/S MOVE   E ENTER   Q BACK") {
  auto &d = M5Cardputer.Display;
  d.drawFastHLine(4, 119, d.width()-8, accent());
  d.setTextColor(0x7BEF, BG);
  d.setCursor(5, 124); d.print(text);
}

void drawHome() {
  header("MAIN MENU");
  auto &d = M5Cardputer.Display;
  for (int i=0; i<MENU_COUNT; i++) {
    int y = 36 + i*15;
    bool selected = (i == menuIndex);
    if (selected) d.fillRoundRect(4, y-2, d.width()-8, 14, 2, PANEL);
    d.setTextColor(selected ? accent() : 0xBDF7, selected ? PANEL : BG);
    d.setCursor(9, y); d.printf("%c %s", selected ? '>' : ' ', menuItems[i]);
  }
  footer();
}

void drawMessage(const char* title, const String& message, const char* foot = "Q BACK") {
  header(title);
  auto &d = M5Cardputer.Display;
  d.setTextColor(0xFFFF, BG);
  d.setCursor(5, 40);
  d.setTextWrap(true);
  d.print(message);
  d.setTextWrap(false);
  footer(foot);
}

void runWifiScan() {
  header("WI-FI SCANNER");
  auto &d = M5Cardputer.Display;
  d.setTextColor(0xFFFF, BG); d.setCursor(5, 39); d.print("Scanning 2.4 GHz networks...");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(150);
  int count = WiFi.scanNetworks(false, true);
  d.fillRect(0, 32, d.width(), 84, BG);
  d.setTextColor(accent(), BG); d.setCursor(5, 33); d.printf("Found: %d", count);
  if (count <= 0) {
    d.setTextColor(0xFFFF, BG); d.setCursor(5, 50); d.print("No networks found.");
  } else {
    int shown = count < 4 ? count : 4;
    for (int i=0; i<shown; i++) {
      int y = 49 + i*16;
      String ssid = WiFi.SSID(i);
      if (ssid.length() == 0) ssid = "<hidden SSID>";
      if (ssid.length() > 18) ssid = ssid.substring(0,18);
      d.setTextColor(0xFFFF, BG); d.setCursor(5, y); d.print(ssid);
      d.setTextColor(accent(), BG); d.setCursor(174, y); d.printf("%d dBm", WiFi.RSSI(i));
    }
  }
  WiFi.scanDelete();
  footer("Q BACK / E RESCAN");
  page = WIFI_PAGE;
}

void runBleScan() {
  header("BLUETOOTH EXPLORER");
  auto &d = M5Cardputer.Display;
  d.setTextColor(0xFFFF, BG); d.setCursor(5, 38); d.print("Scanning BLE for 4 seconds...");
  NimBLEDevice::init("");
  NimBLEScan* scanner = NimBLEDevice::getScan();
  scanner->setActiveScan(true);
  scanner->setInterval(45);
  scanner->setWindow(15);
  NimBLEScanResults results = scanner->getResults(4000, false);
  d.fillRect(0, 32, d.width(), 84, BG);
  d.setTextColor(accent(), BG); d.setCursor(5, 33); d.printf("Devices: %d", (int)results.getCount());
  int shown = results.getCount() < 4 ? results.getCount() : 4;
  for (int i=0; i<shown; i++) {
    const NimBLEAdvertisedDevice* dev = results.getDevice(i);
    int y = 49 + i*16;
    String name = dev->haveName() ? String(dev->getName().c_str()) : String("Unknown device");
    if (name.length() > 17) name = name.substring(0,17);
    d.setTextColor(0xFFFF, BG); d.setCursor(5, y); d.print(name);
    d.setTextColor(accent(), BG); d.setCursor(180, y); d.printf("%d", dev->getRSSI());
  }
  scanner->clearResults();
  footer("Q BACK / E RESCAN");
  page = BLE_PAGE;
}

void drawStyle() {
  header("O-STYLE / UI COLOUR");
  auto &d = M5Cardputer.Display;
  for (int i=0; i<THEME_COUNT; i++) {
    int y = 35 + i*13;
    bool selected = (i == themeIndex);
    d.fillCircle(11, y+3, 4, themes[i].accent);
    d.setTextColor(selected ? accent() : 0xBDF7, BG);
    d.setCursor(22, y); d.printf("%c %s", selected ? '>' : ' ', themes[i].name);
  }
  footer("W/S SELECT   E SAVE   Q BACK");
  page = STYLE_PAGE;
}

void drawDiagnostics() {
  header("DEVICE DIAGNOSTICS");
  auto &d = M5Cardputer.Display;
  float ax=0, ay=0, az=0;
  bool imuOK = M5.Imu.getAccel(&ax, &ay, &az);
  d.setTextColor(0xFFFF, BG);
  d.setCursor(5, 38); d.printf("Chip: ESP32-S3");
  d.setCursor(5, 52); d.printf("CPU: %u MHz", (unsigned int)getCpuFrequencyMhz());
  d.setCursor(5, 66); d.printf("Heap: %u KB", (unsigned int)(ESP.getFreeHeap()/1024));
  d.setCursor(5, 80); d.printf("Wi-Fi: %s", WiFi.status()==WL_CONNECTED ? "CONNECTED" : "IDLE");
  d.setCursor(5, 94);
  if (imuOK) d.printf("IMU: X%.1f Y%.1f Z%.1f", ax, ay, az); else d.print("IMU: unavailable");
  footer("Q BACK / E REFRESH");
  page = DIAG_PAGE;
}

void drawAbout() {
  drawMessage("ABOUT 000", "000 OS\nCardputer ADV edition\n\nTools operate locally. Wi-Fi/BLE results are observations only. Use only on devices and networks you are allowed to test.", "Q BACK");
  page = ABOUT_PAGE;
}

void enterSelected() {
  switch (menuIndex) {
    case 0: runWifiScan(); break;
    case 1: runBleScan(); break;
    case 2: drawStyle(); break;
    case 3: drawDiagnostics(); break;
    case 4: drawAbout(); break;
  }
}

void handleKey(char c) {
  if (page == HOME) {
    if (c=='w' || c=='W') { menuIndex = (menuIndex + MENU_COUNT - 1) % MENU_COUNT; drawHome(); }
    else if (c=='s' || c=='S') { menuIndex = (menuIndex + 1) % MENU_COUNT; drawHome(); }
    else if (c=='e' || c=='E' || c=='\n' || c=='\r') enterSelected();
  } else if (page == STYLE_PAGE) {
    if (c=='w' || c=='W') { themeIndex = (themeIndex + THEME_COUNT - 1) % THEME_COUNT; drawStyle(); }
    else if (c=='s' || c=='S') { themeIndex = (themeIndex + 1) % THEME_COUNT; drawStyle(); }
    else if (c=='e' || c=='E' || c=='\n' || c=='\r') { prefs.putInt("theme", themeIndex); drawStyle(); statusLine="THEME SAVED"; }
    else if (c=='q' || c=='Q') { page=HOME; drawHome(); }
  } else if (page == WIFI_PAGE) {
    if (c=='e' || c=='E') runWifiScan(); else if (c=='q' || c=='Q') { page=HOME; drawHome(); }
  } else if (page == BLE_PAGE) {
    if (c=='e' || c=='E') runBleScan(); else if (c=='q' || c=='Q') { page=HOME; drawHome(); }
  } else if (page == DIAG_PAGE) {
    if (c=='e' || c=='E') drawDiagnostics(); else if (c=='q' || c=='Q') { page=HOME; drawHome(); }
  } else if (page == ABOUT_PAGE) {
    if (c=='q' || c=='Q') { page=HOME; drawHome(); }
  }
}

void setup() {
  auto cfg = M5.config();
  M5Cardputer.begin(cfg, true);
  M5Cardputer.Display.setRotation(1);
  M5Cardputer.Display.setTextSize(1);
  prefs.begin("zeroos", false);
  themeIndex = prefs.getInt("theme", 0);
  if (themeIndex < 0 || themeIndex >= THEME_COUNT) themeIndex = 0;
  drawHome();
}

void loop() {
  M5Cardputer.update();
  if (M5Cardputer.Keyboard.isChange() && M5Cardputer.Keyboard.isPressed()) {
    auto keys = M5Cardputer.Keyboard.keysState();
    for (auto c : keys.word) handleKey(c);
    if (keys.enter) handleKey('\n');
  }
  delay(10);
}
