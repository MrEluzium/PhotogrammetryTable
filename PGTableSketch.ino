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

// DATA
int turnStep = 0;

class GlobalStateMachine{
  private:
    int currentState = 0;                // 0 - Waiting for a button pressed; 1 - Reading potentiometer values; 2 - Show stored value and set state to 0;

  public:
    void set(int state){
      currentState = state;

      if (globalStateLogger == true){
          Serial.print("Global state changed to: ");
          Serial.println(state);
        }
    }

    int get(){
      return currentState;
    }
};

class ButtonStateMachine{
  private:
    int pin = 0;
    int currentState = 0;                    // 0 - LOW; 1 - Pressed once; 2 - Long pressed;
    int currentFlag = 0;                     // Saving previous state until unpress button. Used to fix updating before unpress.
    long buttonCounter = 0;                  // Shows how long the button has been pressed. Used to assign long press state.

  public:
    ButtonStateMachine(int aPin){
       pin = aPin;
    }

    void updateState(){
      int _buttonInput = digitalRead(pin);

      if (_buttonInput == HIGH && currentFlag != 2){  
        currentFlag = 1;

        if (buttonCounter == longPressFactor/globalDelay){
          currentState = 2;
          currentFlag = 2;
        } else{
          buttonCounter++;
        }

      } else if (_buttonInput == LOW){

        if(currentState == 0 && currentFlag == 1){
          currentState = 1;
        }
        
        currentFlag = 0;
        buttonCounter = 0;
      } 
      
      if (buttonStateLogger == true){
        Serial.print("buttonState: ");
        Serial.print(currentState);
        Serial.print(" | buttonFlag: ");
        Serial.print(currentFlag);
        Serial.print(" | buttonCounter: ");
        Serial.println(buttonCounter);
      }
    }

    int getState(){                               // Return current button state. 0 - LOW; 1 - Pressed once; 2 - Long pressed;
      return currentState;
    }

    void used(){                             // Makes the state machine know that a button has been used. Resets the state.
      currentState = 0;
    }
};

GlobalStateMachine globalState = GlobalStateMachine();
ButtonStateMachine mainButton = ButtonStateMachine(buttonPin);


void setup(){
  pinMode(buttonPin, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  Serial.begin(9600);                   // Initialize serial port data transmission at 9600 baud
}

void loop(){
  delay(globalDelay);

  switch (globalState.get()) {

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

  if(mainButton.getState() == 2){
    diodeSetColor(255, 0, 0);
    delay(1000);
    globalState.set(0); 
    mainButton.used();
  }

  mainButton.updateState();
}

/* globalState = 0 */
void waitingBigin(){
  diodeSetColor(0, 255, 127);
  if (mainButton.getState() == 1) {   
    mainButton.used();
    globalState.set(1); 
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

  if (mainButton.getState() == 1) {    
    mainButton.used();

    turnStep = _degree;
    globalState.set(2); 
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
  globalState.set(0);
}

void diodeSetColor(int red, int green, int blue){
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}