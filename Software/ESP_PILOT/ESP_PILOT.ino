
//#include "Arduino.h"
//#include "user_interface.h"
#include "string.h"
#include "espnow.h"
#include <ESP8266WiFi.h>
#include <EEPROM.h>

#define RELEASE

#ifndef RELEASE
#define DEBUG
#endif

//variable definitions
#define USB_BAUDRATE 115200
#define DEBOUNCE_MSDELAY 30
#define FOO_OK 1
#define FOO_ERROR 0
#define ROW_COUNT 5
#define COL_COUNT 2
#define BUTTON_COUNT 10
#define EEPROM_SIZE 12

//pins definitons - TODO, dodać piny
#define PIN_ROW1 0   //GPIO0
#define PIN_ROW2 2   //GOIO2
#define PIN_ROW3 4   //GPIO4
#define PIN_ROW4 5   //GPIO5
#define PIN_ROW5 13  //GPIO13
#define PIN_COL1 15  //GPIO15
#define PIN_COL2 12  //GPIO12
#define PIN_LED 14   //GPIO14



//structures
typedef struct struct_message {
  uint8_t cmd;
  uint8_t data;
} struct_meessage;

//file scope variables
static const uint8_t row_Pins[] = { PIN_ROW1, PIN_ROW2, PIN_ROW3, PIN_ROW4, PIN_ROW5 };
static const uint8_t col_Pins[] = { PIN_COL1, PIN_COL2 };
static uint8_t lastPressedButton;
static uint8_t pressedButtonID;
static uint8_t broadcastAddress[] = { 0x84, 0xF3, 0xEB, 0xE4, 0x59, 0x8E };
static struct_message myData;
//84:F3:EB:E4:59:8E

//function prototypes
uint32_t initializePins();
void pilotEnterLightSleep(void);
uint8_t findPressedButton();
uint8_t sendMessage(uint8_t, uint8_t);
void sendAction(uint8_t, uint8_t);
void hw_wdt_enable(void);
void hw_wdt_disable(void);

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
#ifdef DEBUG
  Serial.print("Last Packet Send Status: ");
#endif
  if (sendStatus == 0) {
#ifdef DEBUG
    Serial.println("Delivery success");
#endif
  } else {
#ifdef DEBUG
    Serial.println("Delivery fail");
#endif
  }
}

void setup() {
  // enable light sleep
  initializePins();
  Serial.begin(USB_BAUDRATE);
  EEPROM.begin(EEPROM_SIZE);
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
#ifdef DEBUG
    Serial.println("Error initializing ESP-NOW");
#endif
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  delay(50);

  uint8_t eeprom_val = 0;
  EEPROM.get(0, eeprom_val);
  if (eeprom_val != 0)  //jeżeli pilot był wyłączony
  {
    myData.cmd = 5;
    myData.data = 5;
    if (esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData)) == 0) {
      uint8_t eeprom_val = 0;
      EEPROM.get(0, eeprom_val);
      if (eeprom_val != 0) {
        for (uint8_t led_blink_ct; led_blink_ct < 4; led_blink_ct++) {
          digitalWrite(PIN_LED, HIGH);
          delay(100);
          digitalWrite(PIN_LED, LOW);
          delay(100);
          EEPROM.put(0, 0);
          EEPROM.commit();
          EEPROM.end();
        }
      }
    }
  } else {
    myData.cmd = 6;
    myData.data = 6;
    if (esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData)) == 0) {
      for (uint8_t led_blink_ct; led_blink_ct < 6; led_blink_ct++) {
        digitalWrite(PIN_LED, HIGH);
        delay(50);
        digitalWrite(PIN_LED, LOW);
        delay(50);
        EEPROM.put(0, 1);
        EEPROM.commit();
        EEPROM.end();
      }
    }
  }


  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();

#ifdef DEBUG
  Serial.println();
  Serial.println("I`m alive");
  Serial.println("I`m alive");
  Serial.println("I`m alive");
#endif

#ifdef RELEASE
  Serial.end();
#endif
  delay(1);  //by some reason ESP is going into soft sleep after calling above wifi fucntions, this delay is here to trigger it before program counter reaches actual code
}

void loop() {

  EEPROM.begin(EEPROM_SIZE);
  uint8_t eeprom_val = 0;
  EEPROM.get(0, eeprom_val);
  EEPROM.end();
  if (eeprom_val == 0) {
    lastPressedButton = findPressedButton();
    if (lastPressedButton != 0) {
      digitalWrite(PIN_LED, HIGH);
    }

    delay(DEBOUNCE_MSDELAY);
    pressedButtonID = findPressedButton();
    if ((lastPressedButton == pressedButtonID) && (pressedButtonID != 0)) {
      while (findPressedButton() == lastPressedButton) {  //wait for button to be released
      sendMessage(1, pressedButtonID);
      delay(200);
      }
      sendMessage(2, pressedButtonID);
      delay(DEBOUNCE_MSDELAY);
    }
    digitalWrite(PIN_LED, LOW);
  }
  pilotEnterLightSleep();
}

void hw_wdt_disable(void) {
  *((volatile uint32_t *)0x60000900) &= ~(1);  // Hardware WDT OFF
}

void hw_wdt_enable(void) {
  *((volatile uint32_t *)0x60000900) |= 1;  // Hardware WDT ON
}

void sendAction(uint8_t action_type, uint8_t action_data) {
  myData.cmd = action_type;
  myData.data = action_data;
  esp_now_send(broadcastAddress, (uint8_t *)&myData, sizeof(myData));
}

uint8_t sendMessage(uint8_t message_type, uint8_t buttonID) {
  uint8_t message_data = 0;
  switch (buttonID) {
    case 1:
      message_data = 1;
      break;
    case 2:
      message_data = 2;
      break;
    case 3:
      message_data = 3;
      break;
    case 4:
      message_data = 4;
      break;
    case 5:
      message_data = 5;
      break;
    case 6:
      message_data = 1;
      message_type += 2;
      break;
    case 7:
      message_data = 2;
      message_type += 2;
      break;
    case 8:
      message_data = 3;
      message_type += 2;
      break;
    case 9:
      message_data = 4;
      message_type += 2;
      break;
    case 10:
      message_data = 5;
      message_type += 2;
      break;
    default:
#ifdef DEBUG
      Serial.println("Unexpected Error in function \"sendMessage\", buttonID variable has undefined value");
#endif
      return FOO_ERROR;
      break;
  }
  if ((message_type != 0) && (message_data != 0)) {
    sendAction(message_type, message_data);
  }

  return FOO_OK;
}

uint8_t findPressedButton() {
  uint8_t buttonID = 0;

  for (uint8_t i = 0; i < COL_COUNT; i++) {
    pinMode(col_Pins[i], INPUT_PULLUP);
  }

  //loop trhought collumns and rows to find first key pressed, then return it`s ID(number)
  for (uint8_t col = 0; col < COL_COUNT; col++) {
    pinMode(col_Pins[col], OUTPUT);
    digitalWrite(col_Pins[col], LOW);
    for (uint8_t row = 0; row < ROW_COUNT; row++) {
      if (digitalRead(row_Pins[row]) == LOW) {
        // col 1 row 3 = button3, col 2 row 3 = button 8
        buttonID = (col * 5) + (row + 1);
        break;
      }
    }
    pinMode(col_Pins[col], INPUT_PULLUP);
    if (buttonID != 0) break;
  }

  for (uint8_t i = 0; i < COL_COUNT; i++) {
    pinMode(col_Pins[i], OUTPUT);
    digitalWrite(col_Pins[i], LOW);
  }
  return buttonID;
}

void pilotEnterLightSleep(void) {
  WiFi.mode(WIFI_OFF);
  ESP.wdtFeed();
  delay(1);
  esp_now_deinit();

  //ESP.wdtDisable();  // Software WDT OFF
  yield();
  //hw_wdt_disable();
  yield();
#ifdef DEBUG
  Serial.flush();  //flushing any outcoming serial data before CPU halts
#endif
// actually enter light sleep:
// the special timeout value of 0xFFFFFFF triggers indefinite
// light sleep (until any of the GPIO interrupts above is triggered)
#ifdef DEBUG
  Serial.println("Entering Sleep");
#endif
  wifi_fpm_do_sleep(0xFFFFFFF);
  // the CPU will only enter light sleep on the next idle cycle, which
  // can be triggered by a short delay()
  delay(1);
#ifdef DEBUG
  Serial.println("Waking Up");
#endif
  //hw_wdt_enable();
  WiFi.mode(WIFI_STA);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  if (esp_now_init() != 0) {
#ifdef DEBUG
    Serial.println("Error initializing ESP-NOW");
#endif
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
}

uint32_t initializePins() {
  //initialize row pins as inputs with pull-up resistors
  for (uint8_t i = 0; i < ROW_COUNT; i++) {
    pinMode(row_Pins[i], INPUT_PULLUP);
    gpio_pin_wakeup_enable(row_Pins[i], GPIO_PIN_INTR_LOLEVEL);
  }
  //initialize col pins as outputs and set them to LOW state
  for (uint8_t i = 0; i < COL_COUNT; i++) {
    pinMode(col_Pins[i], OUTPUT);
    digitalWrite(col_Pins[i], LOW);
  }
  //initialize led pin as output and set it to low state
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  return FOO_OK;
}
