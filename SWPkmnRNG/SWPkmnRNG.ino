#include <WiFi.h>
#include <WebServer.h>
#include <SPIFFS.h>
#include <ArduinoOTA.h> 
#include <WebSocketsServer.h> 
#include "switch_ESP32.h"
#include "index_html.h" 

//************************IMPORTANT***************************************
//INPUT YOUR WIFI CREDENTIALS HERE!!!
const char* ssid = "";         
const char* password = ""; 

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81); 

NSGamepad gamepad;

bool isMacroRunning = false;
bool isClosingGame = false;
bool hasConnected = false; 
bool abortMacro = false; 
String macroPayload = ""; 

String activeProfileName = "";
unsigned long currentMacroStartTime = 0;

bool hUP = false, hDOWN = false, hLEFT = false, hRIGHT = false;
unsigned long currentPhaseStart = 0;
unsigned long currentPhaseTargetMicros = 0;

const unsigned long MIN_HARDWARE_HOLD_MS = 80; 

struct VButton { uint8_t nsBtn; bool logicalState; bool physState; unsigned long unlockTime; };
VButton vA = {NSButton_A, false, false, 0}; VButton vB = {NSButton_B, false, false, 0};
VButton vX = {NSButton_X, false, false, 0}; VButton vY = {NSButton_Y, false, false, 0}; 
VButton vPLUS = {NSButton_Plus, false, false, 0}; VButton vHOME = {NSButton_Home, false, false, 0};

bool l_UP=false, l_DOWN=false, l_LEFT=false, l_RIGHT=false;
bool p_UP=false, p_DOWN=false, p_LEFT=false, p_RIGHT=false;
unsigned long u_UP=0, u_DOWN=0, u_LEFT=0, u_RIGHT=0;

void executeRNGMacro(); void executeCloseGame(); void clickButton(uint8_t button, unsigned long holdTime, unsigned long releaseBuffer);
void processVirtualController(); void abortableDelay(unsigned long ms); void cleanupMacro();

void handleRoot() { server.send(200, "text/html", index_html); }

void handleStatus() { 
  if (isMacroRunning) {
    String resp = "BUSY:" + activeProfileName + ":" + String(millis() - currentMacroStartTime);
    server.send(200, "text/plain", resp);
  } else if (isClosingGame) {
    server.send(200, "text/plain", "BUSY:TEARDOWN:0");
  } else {
    server.send(200, "text/plain", "IDLE"); 
  }
}

void handlePrepare() { if (server.hasArg("seq")) { macroPayload = server.arg("seq"); } server.send(200, "text/plain", "READY"); }

void handleExecute() { 
  activeProfileName = server.hasArg("p") ? server.arg("p") : "Unknown";
  currentMacroStartTime = millis();
  isMacroRunning = true; 
  server.send(200, "text/plain", "GO"); 
}

void handleCloseGame() { server.send(200, "text/plain", "CLOSING"); isClosingGame = true; }
void handleStop() { abortMacro = true; server.send(200, "text/plain", "ABORTED"); }


void handleReboot() {
  server.send(200, "text/plain", "REBOOTING");
  delay(500); // Give the ESP32 half a second to actually send the HTTP response
  ESP.restart(); // Nuke the board
}

void handleGetProfiles() {
  if (SPIFFS.exists("/profiles.json")) {
    File file = SPIFFS.open("/profiles.json", "r");
    server.streamFile(file, "application/json");
    file.close();
  } else {
    server.send(200, "application/json", "{}");
  }
}

void handleSaveProfiles() {
  if (server.hasArg("plain")) {
    File file = SPIFFS.open("/profiles.json", "w");
    if (file) {
      file.print(server.arg("plain"));
      file.close();
      server.send(200, "text/plain", "SAVED");
    } else {
      server.send(500, "text/plain", "SPIFFS ERROR");
    }
  } else {
    server.send(400, "text/plain", "NO DATA");
  }
}

void applyVButton(VButton &btn, bool wantPressed) { if (wantPressed && !btn.logicalState) { btn.unlockTime = millis() + MIN_HARDWARE_HOLD_MS; } btn.logicalState = wantPressed; }
void applyVDPad(bool &logical, unsigned long &unlock, bool wantPressed) { if (wantPressed && !logical) { unlock = millis() + MIN_HARDWARE_HOLD_MS; } logical = wantPressed; }

//websocket handling for controller
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  if (type == WStype_TEXT) {
    String s = "";
    for(size_t i = 0; i < length; i++) {
        s += (char)payload[i];
    }
    
    applyVButton(vA, s.indexOf("A") != -1); applyVButton(vB, s.indexOf("B") != -1); 
    applyVButton(vX, s.indexOf("X") != -1); applyVButton(vY, s.indexOf("Y") != -1); 
    applyVButton(vPLUS, s.indexOf("PLUS") != -1); applyVButton(vHOME, s.indexOf("HOME") != -1);
    applyVDPad(l_UP, u_UP, s.indexOf("UP") != -1); applyVDPad(l_DOWN, u_DOWN, s.indexOf("DOWN") != -1); 
    applyVDPad(l_LEFT, u_LEFT, s.indexOf("LEFT") != -1); applyVDPad(l_RIGHT, u_RIGHT, s.indexOf("RIGHT") != -1);
  }
}

//handles controller input from the website - only works when a macro or restart isn't happening
void processVirtualController() {
  if (isMacroRunning || isClosingGame) return; 
  bool changed = false;
  VButton* btns[] = {&vA, &vB, &vX, &vY, &vPLUS, &vHOME};
  for (int i=0; i<6; i++) {
    bool target = btns[i]->logicalState || (millis() < btns[i]->unlockTime);
    if (target != btns[i]->physState) { if (target) gamepad.press(btns[i]->nsBtn); else gamepad.release(btns[i]->nsBtn); btns[i]->physState = target; changed = true; }
  }
  bool tU = l_UP || (millis() < u_UP); bool tD = l_DOWN || (millis() < u_DOWN); bool tL = l_LEFT || (millis() < u_LEFT); bool tR = l_RIGHT || (millis() < u_RIGHT);
  if (tU != p_UP || tD != p_DOWN || tL != p_LEFT || tR != p_RIGHT) { gamepad.dPad(tU, tD, tL, tR); p_UP = tU; p_DOWN = tD; p_LEFT = tL; p_RIGHT = tR; changed = true; }
  if (changed) gamepad.write();
}

void abortableDelay(unsigned long ms) { 
  unsigned long start = millis(); 
  while (millis() - start < ms) { 
    if (abortMacro) return; 
    server.handleClient(); 
    webSocket.loop(); 
    ArduinoOTA.handle(); 
    gamepad.loop(); 
    delay(1); 
  } 
}

void clickButton(uint8_t button, unsigned long holdTime, unsigned long releaseBuffer) { if (abortMacro) return; gamepad.press(button); gamepad.write(); abortableDelay(holdTime); if (abortMacro) return; gamepad.release(button); gamepad.write(); abortableDelay(releaseBuffer); }

//precise wait for macro inputs
void preciseWaitUntil(unsigned long startTimeMicros, unsigned long durationMicros) { 
  while ((micros() - startTimeMicros) < durationMicros) { 
    if (abortMacro) return; 
    unsigned long elapsed = micros() - startTimeMicros; 
    unsigned long remaining = durationMicros - elapsed; 
    if (remaining > 50000) { 
      server.handleClient(); 
      webSocket.loop(); 
      ArduinoOTA.handle(); 
      delay(1); 
    } 
    gamepad.loop(); 
  } 
}


//ensures macro isn't holding any buttons when ending or aborted
void cleanupMacro() { 
  gamepad.releaseAll(); gamepad.dPad(false, false, false, false); gamepad.write(); 
  hUP = false; hDOWN = false; hLEFT = false; hRIGHT = false; 
  
  vA.unlockTime = 0; vB.unlockTime = 0; vX.unlockTime = 0; vY.unlockTime = 0; 
  vPLUS.unlockTime = 0; vHOME.unlockTime = 0;
  u_UP = 0; u_DOWN = 0; u_LEFT = 0; u_RIGHT = 0;
}

void setup() {
  Serial.begin(115200); 
  if (!SPIFFS.begin(true)) { Serial.println("SPIFFS Mount Failed"); }
  WiFi.mode(WIFI_STA); WiFi.begin(ssid, password); 
  int retries = 0; while (WiFi.status() != WL_CONNECTED && retries < 20) { delay(500); retries++; }
  
  ArduinoOTA.setHostname("Switch-RNG-Engine");
  ArduinoOTA.begin();

  server.on("/", handleRoot); 
  server.on("/prepare", handlePrepare); 
  server.on("/execute", handleExecute); 
  server.on("/close_game", handleCloseGame); 
  server.on("/stop", handleStop); 
  server.on("/status", handleStatus); 
  server.on("/get_profiles", HTTP_GET, handleGetProfiles);
  server.on("/save_profiles", HTTP_POST, handleSaveProfiles);
  server.on("/reboot", handleReboot); // V76: Register Reboot Endpoint
  server.begin();
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  
  gamepad.begin(); USB.begin(); 
}

void loop() { 
  server.handleClient(); 
  webSocket.loop(); 
  ArduinoOTA.handle(); 
  gamepad.loop(); 
  processVirtualController(); 
  
  if (isMacroRunning) { executeRNGMacro(); isMacroRunning = false; } 
  if (isClosingGame) { executeCloseGame(); isClosingGame = false; } 
}

void executeCloseGame() { abortMacro = false; gamepad.releaseAll(); gamepad.dPad(false, false, false, false); gamepad.write(); clickButton(NSButton_Home, 100, 500); clickButton(NSButton_X, 100, 500); clickButton(NSButton_A, 100, 1500); }

//runs compiled macro string from webpage
void executeRNGMacro() {
  abortMacro = false; 
  gamepad.releaseAll(); gamepad.dPad(false, false, false, false); gamepad.write(); 
  hUP = false; hDOWN = false; hLEFT = false; hRIGHT = false; 
  
  vA.physState = false; vB.physState = false; vX.physState = false; vY.physState = false; 
  vPLUS.physState = false; vHOME.physState = false;
  p_UP = false; p_DOWN = false; p_LEFT = false; p_RIGHT = false;

  if (!hasConnected) { clickButton(NSButton_A, 50, 1000); hasConnected = true; } else { abortableDelay(1050); }
  if (abortMacro) { cleanupMacro(); return; }

  int startIdx = 0;
  while (startIdx < macroPayload.length()) {
    if (abortMacro) { cleanupMacro(); return; }
    int pipeIdx = macroPayload.indexOf('|', startIdx);
    if (pipeIdx == -1) pipeIdx = macroPayload.length();
    String cmdStr = macroPayload.substring(startIdx, pipeIdx);
    startIdx = pipeIdx + 1;

    if (cmdStr.length() == 0) continue;
    char type = cmdStr.charAt(0);
    
    if (type == 'S') { currentPhaseTargetMicros = strtoul(cmdStr.substring(2).c_str(), NULL, 10); currentPhaseStart = micros(); } 
    else if (type == 'E') { preciseWaitUntil(currentPhaseStart, currentPhaseTargetMicros); }
    else if (type == 'W') { abortableDelay(cmdStr.substring(2).toInt()); } 
    else {
      int c2 = cmdStr.indexOf(':', 2); String btnStr = (c2 == -1) ? cmdStr.substring(2) : cmdStr.substring(2, c2);
      uint8_t nsBtn = 0; bool isDPad = false; bool dU=false, dD=false, dL=false, dR=false;
      if (btnStr == "A") nsBtn = NSButton_A; else if (btnStr == "B") nsBtn = NSButton_B; else if (btnStr == "X") nsBtn = NSButton_X; else if (btnStr == "Y") nsBtn = NSButton_Y; else if (btnStr == "PLUS") nsBtn = NSButton_Plus; else if (btnStr == "HOME") nsBtn = NSButton_Home; else if (btnStr == "UP") { isDPad = true; dU = true; } else if (btnStr == "DOWN") { isDPad = true; dD = true; } else if (btnStr == "LEFT") { isDPad = true; dL = true; } else if (btnStr == "RIGHT"){ isDPad = true; dR = true; }

      if (type == 'D') {
        if (isDPad) { if(dU) hUP=true; if(dD) hDOWN=true; if(dL) hLEFT=true; if(dR) hRIGHT=true; gamepad.dPad(hUP, hDOWN, hLEFT, hRIGHT); gamepad.write(); }
        else { gamepad.press(nsBtn); gamepad.write(); }
      }
      else if (type == 'U') {
        if (isDPad) { if(dU) hUP=false; if(dD) hDOWN=false; if(dL) hLEFT=false; if(dR) hRIGHT=false; gamepad.dPad(hUP, hDOWN, hLEFT, hRIGHT); gamepad.write(); }
        else { gamepad.release(nsBtn); gamepad.write(); }
      }
      else if (type == 'P') { 
        unsigned long dur = cmdStr.substring(c2 + 1).toInt();
        if (isDPad) { gamepad.dPad(dU||hUP, dD||hDOWN, dL||hLEFT, dR||hRIGHT); gamepad.write(); abortableDelay(dur); gamepad.dPad(hUP, hDOWN, hLEFT, hRIGHT); gamepad.write(); abortableDelay(50); } else { clickButton(nsBtn, dur, 50); }
      } 
      else if (type == 'H') { 
        unsigned long dur = cmdStr.substring(c2 + 1).toInt();
        if (isDPad) { gamepad.dPad(dU||hUP, dD||hDOWN, dL||hLEFT, dR||hRIGHT); gamepad.write(); abortableDelay(dur); gamepad.dPad(hUP, hDOWN, hLEFT, hRIGHT); gamepad.write(); } else { gamepad.press(nsBtn); gamepad.write(); abortableDelay(dur); gamepad.release(nsBtn); gamepad.write(); }
      }
      else if (type == 'M') { 
        unsigned long dur = cmdStr.substring(c2 + 1).toInt(); unsigned long start = millis();
        while (millis() - start < dur) { if (abortMacro) return; if (isDPad) { gamepad.dPad(dU||hUP, dD||hDOWN, dL||hLEFT, dR||hRIGHT); gamepad.write(); abortableDelay(20); gamepad.dPad(hUP, hDOWN, hLEFT, hRIGHT); gamepad.write(); abortableDelay(20); } else { clickButton(nsBtn, 20, 20); } }
      }
    }
  }
  cleanupMacro();
}