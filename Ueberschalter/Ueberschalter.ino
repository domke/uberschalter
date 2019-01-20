
#include <avr/interrupt.h>
#include <avr/io.h> 

/****************************************************************************
 *  RS485 Communication
 ****************************************************************************/
 
#define UART_UWE 2 /* (PD2) Pin for UART WRITE ENABLE */
#define RS485_PRINT(line)   { digitalWrite(UART_UWE, HIGH); Serial.print(line); }
#define RS485_PRINTLN(line) { digitalWrite(UART_UWE, HIGH); Serial.println(line); }

/****************************************************************************
 *  Hardware configuration
 ****************************************************************************/
#define MAX_LAMPS 10
// constants won't change. They're used here to
// set pin numbers:
const int button1 = 7; // the number of the pushbutton pin
const int button2 = 8;
const int lampPin[] = {9, 10, 11, 12, 13, 14, 15, 16, 17, 18}; // the number of the LED pin
int lampState[MAX_LAMPS];

#define DEADTIME 1 // Totzeit, in der die Lampen nicht reagieren (in Sekunden)

// Serial fields
int CMD_MAX = 128;
char myCmd[128];
int port;

// Variables will change:
int buttonState1 = LOW; // the current reading from the input pin
int buttonState2 = LOW;
int lastButtonState1 = LOW; // the previous reading from the input pin
int lastButtonState2 = LOW;

long lastDebounceTime1 = 0; // the last time the output pin was toggled
long lastDebounceTime2 = 0;

/* Reset Write Enable pin, when all data is transmitted */
ISR(USART0_TXC)
{ 
  digitalWrite(UART_UWE, LOW);
}

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
  pinMode(UART_UWE, OUTPUT);
  RS485_PRINT("Hello world.\n");
}

void printLampState() {
  
  RS485_PRINT("states ");
  
   for(int i=0; i < MAX_LAMPS; i++) {
     RS485_PRINT(lampState[i]);
   }
      
   RS485_PRINT("\n");
}

void button1pressed()
{
    // Only control the lamps for the hardware hacking area: aka lamp 5 & 6
    int lampOnFound = 0;
    // search a lamp that is on
    for(int i=4; i < MAX_LAMPS; i++)
     if (lampState[i] > 0) {
       lampOnFound = 1;
       break;
     }
  
  // When one lamp is on, set all to OFF
  // When no lamp off, set all to ON
  for(int i=4; i < MAX_LAMPS; i++) // only handle lamp 5 & 6 in array 4 & 5
    lampState[i] = ! lampOnFound;
  
  printLampState();
      
}

void button2pressed()
{
  // button 2 conroll all lamps of the room
  int lampOnFound = 0;
  // search a lamp that is on
  for(int i=0; i < 4; i++)
    if (lampState[i] > 0) {
      lampOnFound = 1;
      break;
    }
  
  // When one lamp is on, set all to OFF
  // When no lamp off, set all to ON
  for(int i=0; i < 4; i++) // only handle lamp 1,2,3,4 in array: 0-3
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
        
        RS485_PRINTLN("button1 toggled");
        
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
        
        RS485_PRINTLN("button2 toggled");
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
    RS485_PRINT("receiced: ");
    RS485_PRINTLN(myCmd);
    
    int checkCmd = checkCmdArrayForPrefix();
    if(checkCmd == 0){
       RS485_PRINTLN("if you dont know what to do type \"ollpehelp\"");
       sendNackOverSerial();
       return;
    }
    //check for write command
    if (myCmd[5] == 'w')
    {
        port = decodePort(myCmd[6]);
        if (port < 0)
        {
          RS485_PRINTLN("NACK");
        }
        else
        {
          if (((millis() - lastDebounceTime1) / 1000 > DEADTIME)||((millis() - lastDebounceTime2) / 1000 > DEADTIME))
            {
              if (myCmd[7] == 'h')
              {
                 lampState[port] = HIGH;
                 RS485_PRINTLN("HIGH");
                 //sendAckOverSerial();
              }
              else if (myCmd[7] == 'l'){
                lampState[port] = LOW;
                RS485_PRINTLN("LOW");
                //sendAckOverSerial();
              }
              else
              {
                sendNackOverSerial();
                return;  
              }
            
              printLampState();
              sendAckOverSerial();
              
           }
        }
    }
    else if (myCmd[5] == 'r'
              && myCmd[6] == 'a')
    {
      printLampState();
      sendAckOverSerial();
    }
    else if (myCmd[5] == 'r')
    {
      port = decodePort(myCmd[6]);
      RS485_PRINTLN(lampState[port]);
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
    RS485_PRINT("inputSize: ");
    RS485_PRINTLN(inputSize);
    for (int i = 0; i < inputSize; i++){
      myCmd[i] = Serial.read();
    }
  }else if(inputSize >= CMD_MAX){
   Serial.flush();
     RS485_PRINTLN("too much data, flush");
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
  RS485_PRINTLN("ACK");
}

void sendNackOverSerial(){
  RS485_PRINTLN("NACK");
}

void sendPingAckOverSerial(){
  RS485_PRINTLN("PACK");
}

void sendHelpOverSerial()
{
  RS485_PRINTLN("----help is coming----");
  RS485_PRINTLN("all commands must be prefixed with \"ollpe\"");
  RS485_PRINTLN("----commands----");
  RS485_PRINTLN("w1h\t set port 1 high");
  RS485_PRINTLN("w1l\t set port 1 low");
  RS485_PRINTLN("r1\t returns binary state of port 1 (values 0,1)");
  RS485_PRINTLN("ra\t returns binary state of all ports (values 0,1)");
  RS485_PRINTLN("ping\t returns \"PACK\"");
  RS485_PRINTLN("help\t prints this help");
  RS485_PRINTLN("----help end----");
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
