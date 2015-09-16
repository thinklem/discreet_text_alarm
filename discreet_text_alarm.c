// Discreet Text Alarm System
// Receive text when person enters and doesn't push button
// Function sendText code and variables borrowed from MakerBee on instructables.com
// 9/14/15

// Enter your own values here:
#define TEMBOO_ACCOUNT ""                           // your Temboo account name 
#define TEMBOO_APP_KEY_NAME ""                      // your Temboo app key name
#define TEMBOO_APP_KEY  ""                          // your Temboo app key
#define TWILIO_ACCOUNT_SID ""                       // the Account SID from your Twilio account
#define TWILIO_AUTH_TOKEN ""                        // the Auth Token from your Twilio account
#define TWILIO_NUMBER ""                            // your Twilio phone number, e.g., "+1 555-222-1212"
#define RECIPIENT_NUMBER ""                         // the number to which the SMS should be sent, e.g., "+1 555-222-1212"

// Include statements:
#include <Bridge.h>
#include <Temboo.h>

const int buttonPin = 2;            // pin number of the pushbutton
const int doorPin = 4;              // pin number of door sensor
const int ledPin = 13;              // pin number of the LED
const int waitTime = 20;            // wait time in seconds before alarm is active
const int codeTime = 20;            // wait time in seconds before text is sent

boolean doorState = HIGH;           // state of the door sensor
boolean buttonState = LOW;          // state of the pushbutton
boolean ledState = LOW;             // state of the LED

void setup() {
  pinMode(buttonPin, INPUT);        // set buttonPin to INPUT
  pinMode(doorPin, INPUT);          // set doorPin to INPUT
  pinMode(ledPin, OUTPUT);          // set ledPin to OUTPUT

  Bridge.begin();
}

void sendText()
{
  // Send an SMS via a Twilio account and Temboo
      
  // we need a Process object to send a Choreo request to Temboo
  TembooChoreo SendSMSChoreo;
  
  // invoke the Temboo client
  // NOTE that the client must be reinvoked and repopulated with
  // appropriate arguments each time its run() method is called.
  SendSMSChoreo.begin();
      
  // set Temboo account credentials
  SendSMSChoreo.setAccountName(TEMBOO_ACCOUNT);
  SendSMSChoreo.setAppKeyName(TEMBOO_APP_KEY_NAME);
  SendSMSChoreo.setAppKey(TEMBOO_APP_KEY);
  
  // identify the Temboo Library choreo to run (Twilio > SMSMessages > SendSMS)
  SendSMSChoreo.setChoreo("/Library/Twilio/SMSMessages/SendSMS");
  
  // set the required choreo inputs
  // see https://www.temboo.com/library/Library/Twilio/SMSMessages/SendSMS/ 
  // for complete details about the inputs for this Choreo
  
  // the first input is a your AccountSID
  SendSMSChoreo.addInput("AccountSID", TWILIO_ACCOUNT_SID);
      
  // next is your Auth Token
  SendSMSChoreo.addInput("AuthToken", TWILIO_AUTH_TOKEN);
   
  // next is your Twilio phone number
  SendSMSChoreo.addInput("From", TWILIO_NUMBER);
      
  // next, what number to send the SMS to
  SendSMSChoreo.addInput("To", RECIPIENT_NUMBER);
  
  // finally, the text of the message to send
  SendSMSChoreo.addInput("Body", "INTRUDER ALERT!");
      
  // tell the Process to run and wait for the results. The 
  // return code (returnCode) will tell us whether the Temboo client 
  // was able to send our request to the Temboo servers
  unsigned int returnCode = SendSMSChoreo.run();
  
  SendSMSChoreo.close();
}

void loop() {
  // Read button
  buttonState = digitalRead(buttonPin);
  if(buttonState == HIGH)
  {
    
    // Wait waitTime before reading door sensor
    for(int i=0; i < (waitTime * 2); i++)
    {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      delay(500);
    }

    // Read door sensor
    while(doorState == HIGH)
    {
      doorState = digitalRead(doorPin);
      delay(5);
    }
    doorState = HIGH;

    // Wait codeTime and read button
    for(int j=0; j < (codeTime * 2); j++)
    {
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
      for(int k=0; k < 100; k++)
      {
        buttonState = digitalRead(buttonPin);
        delay(5);
        if(buttonState == HIGH)
        {
          ledState = LOW;
          digitalWrite(ledPin, ledState);
          goto afterText;
        }
      }
    }

    sendText();

    // Bypass sendText if button pressed
    afterText:
    delay(3000);
  }
  
}
