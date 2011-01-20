#define MAX_LAMPS  8

// constants won't change. They're used here to 
// set pin numbers:
const int button1 = 10;     // the number of the pushbutton pin
const int button2 = 11;
const int lampPin[] =  {2, 3, 4, 5, 6, 7, 8, 9};      // the number of the LED pin
int lampState[MAX_LAMPS];

#define DEADTIME  1  // Totzeit, in der die Lampen nicht reagieren (in Sekunden)

// Variables will change:
int buttonState1 = LOW;             // the current reading from the input pin
int buttonState2 = LOW;
int lastButtonState1 = LOW;   // the previous reading from the input pin
int lastButtonState2 = LOW;

long lastDebounceTime1 = 0;  // the last time the output pin was toggled
long lastDebounceTime2 = 0;

void setup() {
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  
 for(int i=0; i < MAX_LAMPS; i++) {
    digitalWrite(lampPin[i], LOW);
    lampState[i] = LOW;
    pinMode(lampPin[i], OUTPUT);
 }
 
  // start serial port at 9600 bps:
  Serial.begin(9600);
  Serial.print("Hello world.\n");
}

void button1pressed() 
{
    int lampOnFound = 0;
    // search a lamp that is on
    for(int i=0; i < MAX_LAMPS; i++)
     if (lampState[i] > 0) {
       lampOnFound = 1;
       break;
     }
  
  // When one lamp is on, set all to OFF
  for(int i=0; i < 2; i++)
    lampState[i] = ! lampOnFound;
  
    
      for(int i=0; i < MAX_LAMPS; i++) {
        Serial.print("lamp ");
        Serial.print(lampPin[i]);
        Serial.print(" ");
        Serial.print(lampState[i]);
        Serial.print("\n");
      }
      
}


void button2pressed() 
{
  int lampOnFound = 0;
  // search a lamp that is on
  for(int i=0; i < MAX_LAMPS; i++)
    if (lampState[i] > 0) {
      lampOnFound = 1;
      break;
    }
  
  // When one lamp is on, set all to OFF
  for(int i=0; i < MAX_LAMPS; i++)
    lampState[i] = ! lampOnFound;
  
    for(int i=0; i < MAX_LAMPS; i++) {
        Serial.print("lamp ");
        Serial.print(lampPin[i]);
        Serial.print(" ");
        Serial.print(lampState[i]);
        Serial.print("\n");
      }
      
}

void readButtons() {
  int reading;
  
  reading = digitalRead(button1);

  if(reading == 1) // PENS Crap-Code
  {
    delay(50);
    if(digitalRead(button1) == HIGH && lastButtonState1 == LOW)
    { 
      if ((millis() - lastDebounceTime1) / 1000 > DEADTIME) // DEADTIME! COOL OLLO STUFF inspirated by dome
      {
        lastDebounceTime1 = millis();
        
        Serial.println("button1 toggled");
        
        button1pressed();
        
        //buttonState1 = !buttonState1;
        //buttonHasChanged(1);
        
        lastButtonState1 = HIGH;
 
      }
      
    }
    
  }
  else
  {
    lastButtonState1 = LOW; 
  }


  reading = digitalRead(button2);

  if(reading == 1) // PENS Crap-Code
  {
    delay(50);
    if(digitalRead(button2) == 1 && lastButtonState2 == LOW)
    { 
      if ((millis() - lastDebounceTime2) / 1000 > DEADTIME)
      {
        lastDebounceTime2 = millis();
        
        Serial.println("button2 toggled");
        buttonState2 = !buttonState2;
        button2pressed();
        
        lastButtonState2 = HIGH;
      }
      
    }
  }
  else
  {
    lastButtonState2 = LOW; 
  }

}
  
void loop() {
  readButtons();
  
      
  for(int i=0; i < MAX_LAMPS; i++)
    digitalWrite(lampPin[i], lampState[i]);
}

