//#include "painlessMesh.h"
#include "Arduino.h"
#include "ESP.h"
#include "user_interface.h" // include required for LIGHT_SLEEP_T, among others

#define DEBUG 0

#define MESH_PREFIX "LOCAL_MESH"
#define MESH_PASSWORD "LOCALMESHMOTHERFUCKER"
#define MESH_PORT 2137


//settings definitions
#define USB_BAUDRATE 115200

//pins definitons - TODO, dodać piny
#define PIN_ROW1 0 //GPIO0
#define PIN_ROW2 2 //GOIO2
#define PIN_ROW3 4 //GPIO4
#define PIN_ROW4 5 //GPIO5
#define PIN_ROW5 13 //GPIO13
#define PIN_COL1 15 //GPIO15
#define PIN_COL2 12 //GPIO12
#define PIN_LED 14 //GPIO14

//util definitions
#define FOO_OK 1
#define FOO_ERROR 0
#define ROW_COUNT 5
#define COL_COUNT 2
#define BUTTON_COUNT 10
#define DEFAULT_DEBOUNCE_TIME 100

//file scope variables
static const uint8_t row_Pins[] = {PIN_ROW1, PIN_ROW2, PIN_ROW3, PIN_ROW4, PIN_ROW5};
static const uint8_t col_Pins[] = {PIN_COL1, PIN_COL2};

uint32_t initializePins();
void pilotEnterLightSleep();
uint32_t findPressedButton();

void setup() {
  // enable light sleep
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T);
  wifi_fpm_open();
  Serial.begin(USB_BAUDRATE);
  initializePins();
  Serial.println("I`m alive");
  Serial.println("I`m alive");
  
  Serial.println("I`m alive");
  //pilotEnterLightSleep();
  //mesh.setDebugMsgTypes(ERROR | STARTUP);
  //mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);

}

void loop() {
  findPressedButton();
}

uint32_t findPressedButton()
{

  for(uint8_t i = 0; i < COL_COUNT; i++)
  {
    pinMode(col_Pins[i], OUTPUT);
    digitalWrite(col_Pins[i], HIGH);
    digitalWrite(PIN_LED, LOW);
  }
  
  //loop trhought collumns and rows to find first key pressed, then return it`s ID(number)
  for(uint8_t col = 0; col < COL_COUNT; col++)
  {
    digitalWrite(col_Pins[col], LOW);
    delay(10);
    for(uint8_t row = 0; row < ROW_COUNT; row++)
    {
      /*
      Serial.print("Col: ");
      Serial.print(col);
      Serial.print("  Row: ");
      Serial.println(row);
      */
      if(digitalRead(row_Pins[row]) == LOW)
      {
        // col 1 row 3 = button3, col 1 row 3 = button 6 
        pinMode(col_Pins[col], INPUT_PULLUP);
        digitalWrite(PIN_LED, HIGH);
        uint8_t buttonID = (col*5) + (row+1);
        #ifdef DEBUG
          Serial.print("Pressed Button ID: ");
          Serial.println(buttonID);
/*
          for(uint8_t row = 0; row < ROW_COUNT; row++)
          {
            uint8_t value = 0;
            Serial.print("Row");
            Serial.print(row);
            Serial.print(" State:");
            switch(digitalRead(row_Pins[row]))
            {
              case 0:
                value = 0;
                break;
              case 1:
                value = 1;
                break;
              default:
                break;
            }
            Serial.println(value);
          }
          */
        #endif
        return (uint32_t)buttonID;
      }
      else
      {
        #ifdef DEBUG

        //Serial.println("Nothing found\n");
        /*
          for(uint8_t row = 0; row < ROW_COUNT; row++)
          {
            uint8_t value = 0;
            Serial.print("Row");
            Serial.print(row);
            Serial.print(" State:");
            switch(digitalRead(row_Pins[row]))
            {
              case 0:
                value = 0;
                break;
              case 1:
                value = 1;
                break;
              default:
                break;
            }
            Serial.println(value);
          }
          */
        #endif
      }
    }
    digitalWrite(col_Pins[col], HIGH);
  }

  return FOO_ERROR;
}

void pilotEnterLightSleep()
{
  Serial.flush(); //flushing any outcoming serial data before CPU halts
  // actually enter light sleep:
  // the special timeout value of 0xFFFFFFF triggers indefinite
  // light sleep (until any of the GPIO interrupts above is triggered)
  wifi_fpm_do_sleep(0xFFFFFFF);
  // the CPU will only enter light sleep on the next idle cycle, which
  // can be triggered by a short delay()
  delay(10);
}

uint32_t initializePins()
{
  //pinMode(4, OUTPUT);
  //digitalWrite(4, HIGH);
  //initialize row pins as inputs with pull-up resistors
  for(uint8_t i = 0; i < ROW_COUNT; i++)
  {
    pinMode(row_Pins[i], INPUT_PULLUP);
    if(row_Pins[i] != 4) gpio_pin_wakeup_enable(row_Pins[i], GPIO_PIN_INTR_LOLEVEL); //TODO pin 4 is somehow pulled low on NodeMCU
    //gpio_pin_wakeup_enable(row_Pins[i], GPIO_PIN_INTR_LOLEVEL);
  }
  //initialize col pins as outputs and set them to LOW state
  for(uint8_t i = 0; i < COL_COUNT; i++)
  {
    pinMode(col_Pins[i], OUTPUT);
    digitalWrite(col_Pins[i], LOW); 
  }
  //initialize led pin as output and set it to low state
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  return FOO_OK;
}
