
//settings definitions
#define USB_BAUDRATE 9600

//pins definitons - TODO, dodać piny
#define PIN_ROW1 0 //GPIO0
#define PIN_ROW2 2 //GOIO2
#define PIN_ROW3 5 //GPIO5
#define PIN_ROW4 4 //GPIO4
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
static uint8_t lastPressedButtonID = 0;

typedef struct Button
{
  uint8_t ID;
  uint8_t timeElapsedSincePush;
  uint8_t isDebounced;
}Button;

Button buttons[BUTTON_COUNT] = {0};

//function prototypes
uint32_t findPressedButton();
uint32_t checkButtonsTiming();
uint32_t initializePins();
uint32_t readConfigFromEEPROM();
uint32_t debounceButton();


void setup() {
  initializePins();
  initializeButtons();
}

void loop() {
  findPressedButton();
  //if(Button
  //debounceButton(lastPressedButtonID);
  //if(Button.
}

uint32_t findPressedButton()
{
  
  for(uint8_t i = 0; i < COL_COUNT; i++)
  {
    digitalWrite(col_Pins[i], HIGH);
  }
  
  //loop trhought collumns and rows to find first key pressed, then return it`s ID(number)
  for(uint8_t col = 0; col < COL_COUNT; col++)
  {
    digitalWrite(col_Pins[col], LOW);
    for(uint8_t row = 0; row < ROW_COUNT; row++)
    {
      if(digitalRead(row_Pins[row]) == LOW)
      {
        // col 1 row 3 = button3, col 1 row 3 = button 6 
        uint8_t buttonID = (col+1) * (row+1);
        lastPressedButton = buttonID;
      }
    }
    digitalWrite(col_Pins[col], HIGH);
  }

  return FOO_OK;
}

uint32_t initializePins()
{
  //initialize row pins as inputs with pull-up resistors
  for(uint8_t i = 0; i < ROW_COUNT; i++)
  {
    pinMode(row_Pins[i], INPUT_PULLUP);
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

uint32_t initializeButtons()
{
  for(uint8_t i = 0; i < BUTTON_COUNT; i++)
  {
    Button[i].ID = (i+1);
    Button[i].timeElapsedSincePush = 0;
    Button[i].isDebounced = 1;
  }

  return FOO_OK;
}

uint32_t readConfigFromEEPROM()
{
  
}

uint32_t checkButtonsTiming()
{
  
}

uint32_t debounceButton(uint8_t button_ID)
{
  
}
