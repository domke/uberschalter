#define MAX_LAMPS  8

// constants won't change. They're used here to 
// set pin numbers:
const int button1 = 10;     // the number of the pushbutton pin
const int button2 = 11;
const int lampPin[] =  {2, 3, 4, 5, 6, 7, 8, 9};      // the number of the LED pin
int lampState[MAX_LAMPS];

#define DEADTIME  1  // Totzeit, in der die Lampen nicht reagieren (in Sekunden)

// Serial fields
int CMD_MAX = 128;
char myCmd[128];
int port;

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

void printLampState() {
  
      for(int i=0; i < MAX_LAMPS; i++) {
        Serial.print("lamp ");
        Serial.print(( i + 1 ));        
        Serial.print(" : ");
        Serial.print(lampState[i]);
        Serial.print("    (Pin ");        
        Serial.print(lampPin[i]);
        Serial.print(")");
        Serial.print("\n");
      }
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
  
  printLampState();
      
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
  
  printLampState();
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
  
    //delay needed to have a chance to get the whole message
  delay(50);
  clearCmdArray();
 
  int inputSize = readFromSerialIntoCmdArray();
  
  if (inputSize > 0)
  {
    //debug
    Serial.print("receiced: ");
    Serial.println(myCmd);
    
    int checkCmd = checkCmdArrayForPrefix();
    if(checkCmd == 0){
       Serial.println("if you dont know what to do type \"ollpehelp\"");
       sendNackOverSerial();
       return; 
    }
    //check for write command
    if (myCmd[5] == 'w')
    {
        port = decodePort(myCmd[6]);
        if (port < 0)
        {
          Serial.println("NACK");  
        }
        else
        {
            if (myCmd[7] == 'h')
            {
              lampState[port] = HIGH;
              Serial.println("HIGH");
              sendAckOverSerial();
            }
            else if (myCmd[7] == 'l'){
              lampState[port] = LOW;
              Serial.println("LOW");  
              sendAckOverSerial();
            }
            else
            {
              sendNackOverSerial();
            }
            
            printLampState();
        }
    }
    else if (myCmd[5] == 'r')
    {
      port = decodePort(myCmd[6]);      
      Serial.println(lampState[port]);
    }
    //check for ping command
    else if(myCmd[5] == 'p' 
            && myCmd[6] == 'i' 
            && myCmd[7] == 'n' 
            && myCmd[8] == 'g')
    {
       sendPingAckOverSerial();
    }
    else if(myCmd[5] == 'h' 
            && myCmd[6] == 'e' 
            && myCmd[7] == 'l' 
            && myCmd[8] == 'p')
    {
       sendHelpOverSerial();
    }
    else
    {
      //no write command
       sendNackOverSerial(); 
    }
  }    
      
  for(int i=0; i < MAX_LAMPS; i++)
    digitalWrite(lampPin[i], lampState[i]);
}


// -------------- Serial Stuff ------------------

void clearCmdArray(){
  //clear the cmd array
  for (int i = 0; i < CMD_MAX; i++){
    myCmd[i] = '\0';
  }
}

//returns number of read bytes
int readFromSerialIntoCmdArray(){ 
  //read from the serial buffer and flush
  int inputSize = Serial.available();
  
  //give serial a chance to receive all bytes
  if(inputSize > 0){
    delay(100);
    inputSize = Serial.available();
  }
  
  if(inputSize > 0 && inputSize < CMD_MAX){
    Serial.print("inputSize: ");
    Serial.println(inputSize);
    for (int i = 0; i < inputSize; i++){
      myCmd[i] = Serial.read();
    }
  }else if(inputSize >= CMD_MAX){
   Serial.flush();
     Serial.println("too much data, flush");
  }
  return inputSize;
}

//check if command has the required prefix
int checkCmdArrayForPrefix(){
   if (myCmd[0] == 'o'
     && myCmd[1] == 'l'
     && myCmd[2] == 'l'
     && myCmd[3] == 'p'
     && myCmd[4] == 'e'){
      return 1;
   }
  return 0;
}

void sendAckOverSerial(){
  Serial.println("ACK");
}

void sendNackOverSerial(){
  Serial.println("NACK");
}

void sendPingAckOverSerial(){
  Serial.println("PACK");
}

void sendHelpOverSerial()
{
  Serial.println("----help is coming----");
  Serial.println("all commands must be prefixed with \"ollpe\"");
  Serial.println("----commands----");
  Serial.println("w1h\t set port 1 high");
  Serial.println("w1l\t set port 1 low");
  Serial.println("r1\t returns binary state of port 1 (values 0,1)");
  Serial.println("ping\t returns \"PACK\"");
  Serial.println("help\t prints this help");
  Serial.println("----help end----");
}

//converts a char with the port number 0-6
//to the arduino port numbers
int decodePort(char c)
{
 switch(c)
  {
   case '1':
     return 0;
   case '2':
     return 1;
   case '3':
     return 2;
   case '4':
     return 3;
   case '5':
     return 4;
   case '6':
     return 5;
   case '7':
     return 6;
   case '8':
     return 7;
   default:
     return -1;
  } 
}

