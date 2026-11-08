#include <Arduino.h>
#include <Adafruit_SPIFlash.h>
#include <bluefruit.h>
#include <Wire.h>

// Onboard Hardware Profiles
Adafruit_FlashTransport_QSPI flashTransport;

// TMP102 I2C Definitions
#define TMP102_ADDR 0x48  // Default I2C address for TMP102
#define CONFIG_REG  0x01  // Configuration register address
#define TEMP_REG    0x00  // Temperature register address

// Custom BLE Service and Characteristic definitions
BLEService        loggerService = BLEService(0x1234); 
BLECharacteristic dataChar      = BLECharacteristic(0x5678);
volatile bool     isConnected   = false; 

// Function Prototypes for PlatformIO Compiler Scope
void shutdownOnboardFlash();
void setupBLE();
void disableAllUnusedPins();
void setTMP102Sleep(bool sleepMode);
float readTMP102Temperature();
void connect_callback(uint16_t conn_handle);
void disconnect_callback(uint16_t conn_handle, uint8_t reason);

void connect_callback(uint16_t conn_handle) {
  (void)conn_handle;
  isConnected = true;
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason) {
  (void)conn_handle;
  (void)reason;
  isConnected = false;
}

void shutdownOnboardFlash() {
  flashTransport.begin();
  flashTransport.runCommand(0xB9); 
  flashTransport.end();
}

void setupBLE() {
  Bluefruit.begin();
  Bluefruit.autoConnLed(false); // Keeps the status LED off
  Bluefruit.setTxPower(-8);     // Set TX power to -8 dBm for low power
  
  // Link the connection event callbacks
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Initialize custom service profile
  loggerService.begin();
  dataChar.setProperties(CHR_PROPS_READ | CHR_PROPS_NOTIFY);
  dataChar.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  dataChar.setFixedLen(4); 
  dataChar.begin();

  // Clear data and set snappy 30ms intervals to maximize connection catch chance
  Bluefruit.Advertising.clearData();
  Bluefruit.Advertising.setInterval(48, 48); 
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.setName("XIAO_Logger");
}

void disableAllUnusedPins() {
  pinMode(LED_RED, OUTPUT);   digitalWrite(LED_RED, HIGH);   // Force OFF
  pinMode(LED_GREEN, OUTPUT); digitalWrite(LED_GREEN, HIGH); // Force OFF
  pinMode(LED_BLUE, OUTPUT);  digitalWrite(LED_BLUE, HIGH);  // Force OFF

  // Clamp unused P0 edge pins to Ground (Leaving out P0.02 and P0.03 if used for sensor!)
  uint8_t p0_unused[] = {28, 29};
  for(uint8_t i=0; i<sizeof(p0_unused); i++) {
    NRF_P0->PIN_CNF[p0_unused[i]] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                                    (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos);
  }

  // Clamp unused P1 edge pins to Ground
  uint8_t p1_unused[] = {11, 12, 13, 15};
  for(uint8_t i=0; i<sizeof(p1_unused); i++) {
    NRF_P1->PIN_CNF[p1_unused[i]] = (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos) |
                                    (GPIO_PIN_CNF_PULL_Pulldown << GPIO_PIN_CNF_PULL_Pos);
  }
}

void setTMP102Sleep(bool sleepMode) {
  Wire.beginTransmission(TMP102_ADDR);
  Wire.write(CONFIG_REG);
  if (sleepMode) {
    Wire.write(0x01); // Shutdown Mode (<0.5uA)
  } else {
    Wire.write(0x00); // Continuous Mode
  }
  
  Wire.write(0x60); 
  Wire.endTransmission();
}

float readTMP102Temperature() {
  Wire.beginTransmission(TMP102_ADDR);
  Wire.write(TEMP_REG);
  Wire.endTransmission();
  
  Wire.requestFrom(TMP102_ADDR, 2);
  if (Wire.available() >= 2) {
    uint8_t msb = Wire.read();
    uint8_t lsb = Wire.read();
    int16_t raw_temp = (msb << 4) | (lsb >> 4);
    if (raw_temp & 0x800) {
      raw_temp |= 0xF000; 
    }
    return raw_temp * 0.0625;
  }
  return -999.0; 
}

void setup() {
  // SAFETY BOOT WINDOW: Flashes the Red LED for 20 seconds on startup.
  pinMode(LED_RED, OUTPUT);
  for (int i = 0; i < 40; i++) {
    digitalWrite(LED_RED, LOW);  // ON
    delay(250);
    digitalWrite(LED_RED, HIGH); // OFF
    delay(250);
  }

  // Optimize Power Rails & Peripherals
  NRF_POWER->DCDCEN = 1;
  NRF_POWER->TASKS_LOWPWR = 1; 
  sd_power_mode_set(NRF_POWER_MODE_LOWPWR); 
  
  NRF_UART0->ENABLE = 0; // Turn off hardware UART
     
  shutdownOnboardFlash();
  disableAllUnusedPins();
  setupBLE();

  // Initialize Wire, put sensor to sleep, then clamp lines matching pull-ups
  Wire.begin();
  setTMP102Sleep(true);
  Wire.end();
  
  pinMode(PIN_WIRE_SDA, INPUT_PULLUP);
  pinMode(PIN_WIRE_SCL, INPUT_PULLUP);
}

void loop() {
  // 1. Wake up and sample sensor
  pinMode(PIN_WIRE_SDA, INPUT); 
  pinMode(PIN_WIRE_SCL, INPUT);
  
  Wire.begin();
  setTMP102Sleep(false); 
  delay(30);             
  
  float currentTemp = readTMP102Temperature();
  
  setTMP102Sleep(true);  
  Wire.end();            
  
  // Secure against hardware pull-up resistor current leaks
  pinMode(PIN_WIRE_SDA, INPUT_PULLUP); 
  pinMode(PIN_WIRE_SCL, INPUT_PULLUP);

  // Write data to our GATT Characteristic profile
  dataChar.write(&currentTemp, sizeof(currentTemp));

  // If a device connects during the advertising block, stall deep sleep
  if (isConnected) {
    while(isConnected) {
      // Fast tracking pulse while actively dumping data
      digitalWrite(LED_RED, LOW);  delay(10);              
      digitalWrite(LED_RED, HIGH); delay(90); 
    }
    // Re-initialize code stack options fresh right after losing the link
    setupBLE();
  }

  // 2. Transmit brief 500ms connection window beacon
  Bluefruit.Advertising.start(0); 
  delay(500); 
  
  // Final safety check to see if an incoming connection happened right at the deadline
  if (isConnected) return;

  // 3. Close the door on background scheduler tasks
  sd_softdevice_disable(); 

  // 4. Fire indicator LED flash
  digitalWrite(LED_RED, LOW);  delay(20);              
  digitalWrite(LED_RED, HIGH); 
  
  // 5. Deep System Sleep for 59.45 seconds
  delay(59450); 
}


