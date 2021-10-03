// ANALOG PINS
#define POT 0                           // Potentiometer input (A0)

// DIGITAL PINS
#define redPin 12
#define greenPin 11
#define bluePin 10

#define buttonPin 2

// SETUP
const int globalDelay = 30;             // Delay at each loop() iteration. Must be set at least on 10.
const int longPressFactor = 230;        // Number that determines amount of iterations to consider the button is long pressed. Calculated by the formula: Amount of iterations = longPressFactor / globalDelay

const bool globalStateLogger = true;
const bool potentiometerLogger = true;
const bool buttonStateLogger = true;
 
// STATES
int globalState = 0;                    // 0 - Waiting for a button pressed; 1 - Reading potentiometer values; 2 - Show stored value and set state to 0;
int buttonState = 0;                    // 0 - LOW; 1 - Pressed once; 2 - Long pressed;
int buttonFlag = 0;

// COUNTERS
long buttonCounter = 0;

// DATA
int turnStep = 0;


void setup()
{
  pinMode(buttonPin, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  Serial.begin(9600);                   // Initialize serial port data transmission at 9600 baud
}

void loop()
{
  delay(globalDelay);

  switch (globalState) {

    case 0:
      waitingBigin();
      break;

    case 1:
      readingPotentiometer();
      break;
    
    case 2:
      showSavedValue();
      break;
  }

  if(buttonState == 2){
    diodeSetColor(255, 0, 0);
    delay(1000);
    setGlobalState(0);
    buttonUsed();
  }

  updateButtonState();
}

/* globalState = 0 */
void waitingBigin(){
  diodeSetColor(0, 255, 127);
  if (buttonState == 1) {   
    buttonUsed();
    setGlobalState(1); 
  }

}

/* globalState = 1 */
void readingPotentiometer(){
  int _val = analogRead(POT);           // Reading value from a potentiometer
  int _per = map(_val, 0, 1023, 0, 100); // Converting to percent
  int _colour = map(_val, 0, 1023, 1, 255); // Converting to rgb value
  int _degree = map(_val, 0, 1023, 5, 45); // Converting to degrees

  diodeSetColor(_colour, 255-_colour, 0);

  if (potentiometerLogger == true)
  {
    Serial.print("Analog Reading: ");   
    Serial.print(_val);                   // Printing raw analog values to the serial port
    Serial.print(" | Percentage: ");     
    Serial.print(_per);
    Serial.print("%");
    Serial.print(" | Degrees: ");     
    Serial.print(_degree);
    Serial.print("°");
    Serial.print(" | Colour: ");     
    Serial.println(_colour);
  }

  if (buttonState == 1) {    
    buttonUsed();

    turnStep = _degree;
    setGlobalState(2); 
  }
}

/* globalState = 2 */
void showSavedValue(){
  diodeSetColor(255, 165, 0);
  Serial.println("");
  Serial.print(" Final degrees: ");     
  Serial.print(turnStep);
  Serial.println("°");
  delay(1000);
  setGlobalState(0);
}

void updateButtonState(){
  int _buttonInput = digitalRead(buttonPin);

  if (_buttonInput == HIGH && buttonFlag != 2){  
    buttonFlag = 1;

    if (buttonCounter == longPressFactor/globalDelay){
      buttonState = 2;
      buttonFlag = 2;
    } else{
      buttonCounter++;
    }

  } else if (_buttonInput == LOW){

    if(buttonState == 0 && buttonFlag == 1){
      buttonState = 1;
    }
    
    buttonFlag = 0;
    buttonCounter = 0;
  } 
  
  if (buttonStateLogger == true){
    Serial.print("buttonState: ");
    Serial.print(buttonState);
    Serial.print(" | buttonFlag: ");
    Serial.print(buttonFlag);
    Serial.print(" | buttonCounter: ");
    Serial.println(buttonCounter);
  }
  
}

void buttonUsed(){
  buttonState = 0;
}

void setGlobalState(int state){
  globalState = state;

  if (globalStateLogger == true)
    {
      Serial.print("Global state changed to: ");
      Serial.println(state);
    }
  
}

void diodeSetColor(int red, int green, int blue){
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}