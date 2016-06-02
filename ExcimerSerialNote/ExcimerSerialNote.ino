//Laser Harp
//Jake Hillard Spring 2016

    #include <stdio.h>
    
    const int handPin = 14;  // Analog input is AIN0 (Teensy3 pin 14, next to LED)
    const int LED1 = 13;         // output LED connected on Arduino digital pin 13
    const int clockPin = 0;
    const int clockPinA = 15;
    int clockThreshold =  50000;// For making the clock signal digitally aquired 
    const int flagPin = 1; //digital output pin for Oscilloscope to see. 
    const int flagPin2 = 2; //Clock Aquisition Debug.
    const int b1 = 4;
    const int b2 = 5;
    const int b3 = 6;
    const int b4 = 7;
    const int channel = 1;

    //Clock Aquisition Variables
    unsigned long curTime = 0;
    unsigned long handTime = 0;
    unsigned long lastTime = 0;
    bool clockHigh = 0; //Prevent clock thinking a full cycle has passed.
    unsigned long noteTime = 0;
    int analogVal = 0;
    bool noteFound = 0; 
    //Dark Mode: When you're in a really dark room)
    //int lightThreshold = 10000; //Threshhold for triggering the second pulse.
    //Light Mode: When its still dark, but there is some ambient light
    int lightThreshold = 10000; //Threshhold for triggering the second pulse.
    
    double currPosition;
    double doubleZero = 0;
   
    //Filter Variables. 
    double currRatio = 0;
    int noteThreshold = 6; //Number of times Excimer must see the note before sending it as MIDI
    int buffCounter = 0;  //The current number of times Excimer has seen a note.

    //Note Scaling Etc Variables
    double numNotes = 7; 
    //Using scale Increasing C Major
    //C-D-E-F-G-A-B-[C]
    //https://en.wikipedia.org/wiki/Scale_(music)
    double NoteMin= 56; 
    double NoteMax= 82;
    double NoteSpan = NoteMax-NoteMin;
    int noteSpot;

    //MIDI Note Constants
    int Octave = 4;
    int noteNumber;
    unsigned long  noteStartedTime;

    //Current Note Plan: Make Note play for 4 seconds and fade slowly out after being pressed. Only start new note if it
    //sees a new _different_ note being played.
    
    

    
    void setup() {    // ==============================================================
      pinMode(LED1,OUTPUT);       // enable digital output for turning on LED indicator
      pinMode(clockPin, INPUT_PULLDOWN);
      pinMode(b1, INPUT_PULLUP);
      pinMode(b2, INPUT_PULLUP);
      pinMode(b3, INPUT_PULLUP);
      pinMode(b4, INPUT_PULLUP);
      
      pinMode(flagPin2, OUTPUT);
      pinMode(flagPin, OUTPUT);
      analogReference(INTERNAL);  // set analog reference to internal ref
      analogReadRes(16);          // Teensy 3.0: set ADC resolution to this many bits
     
      digitalWrite(LED1,HIGH);   delay(1000);   // LED on for 1 second
      digitalWrite(LED1,LOW);

     // usbMIDI.sendNoteOn(61, 99, channel);
      delay(2000);
      Serial.begin(9600);
      Serial.println("Excimer Booted");
    //  usbMIDI.sendNoteOn(61, 0, channel);
      
      
    } // ==== end setup() ===========

    void loop() {  // ================================================================ 
        if(buffCounter>noteThreshold){
          buffCounter = 0;
          //playNote()
        }
        trackHand();
        currPosition = currRatio;
        //if(currPosition != doubleZero) Serial.println(currPosition);
        
        noteSpot = int(ceil((currPosition-NoteMin)/(NoteSpan)*numNotes))%int(numNotes)+1;
        if(currPosition != doubleZero) PlaySpot(noteSpot);
        //Breaks up position into Notes. NoteMax Term Normalizes 0-100 as the note Position.
        //numNotes, mod 7, and ceil() buckets our positions into specific notes to play.
        //Plus one is to shift from 0-6 to 1-7
        
        //if(currPosition != doubleZero) Serial.println(noteSpot); 
           // }
       // }
        
        if(!digitalRead(b1)){
          digitalWrite(LED1, HIGH);
          //delay(5);
          digitalWrite(LED1,LOW);
        }
        if (!digitalRead(b2)){
          digitalWrite(LED1, HIGH);
          //delay(5);
          digitalWrite(LED1,LOW);
        }
        if(!digitalRead(b4)){
        }
        /*if(!digitalRead(b3)){
          digitalWrite(LED1, HIGH);
          delay(5);
          digitalWrite(LED1, LOW);
          usbMIDI.sendNoteOn(61, 99, channel +1 );  // 61 = C#4
        }else{usbMIDI.sendNoteOn(61, 0, channel +1);}  // 61 = C#4
         */
         // MIDI Controllers should discard incoming MIDI messages.
      //  while (usbMIDI.read()) {
      //  }
      
        
      }

void trackHand(){


       if(analogRead(clockPinA)>clockThreshold ){  //finds the time at the rising clock edge.
          if (!clockHigh){
            lastTime = curTime;
            curTime = micros();
            clockHigh = 1; 

            digitalWrite(flagPin2, HIGH);
            delay(1);
            digitalWrite(flagPin2, LOW);
            }
        }else{clockHigh = 0;}

        if (analogRead(handPin) > lightThreshold){ // Finds Rising edge of Hand Clock Pulse
          handTime = micros();
          digitalWrite(flagPin, HIGH);
          delay(1);
          digitalWrite(flagPin, LOW);
        }
        
        if (handTime > curTime){  //Hand in Beam
          currRatio = double(handTime-curTime)/double(curTime-lastTime)*100;
          //Serial.println(currRatio);
        }else{ //No Hand in Beam
          currRatio = 0;
          buffCounter = 0;
        }

}

void PlaySpot(int noteSpot){
  //C-D-E-F-G-A-B-[C]
  //1-2-3-4-5-6-7
  //0-2-4-5-7-9-11
  int intermNum;
  int lastNoteNumber;
  //Serial.println(noteSpot);
  switch(noteSpot){
    case 1: intermNum = 0;
      break; 
    case 2: intermNum = 2;
      break;
    case 3: intermNum = 4;
      break;
    case 4: intermNum = 5;
      break;
    case 5: intermNum = 7;
      break;
    case 6: intermNum = 9;
      break;
    case 7: intermNum = 11;
      break;
    default: intermNum = 0;
      break;
  }

  
  lastNoteNumber=noteNumber;
  noteNumber = 12*Octave + intermNum;
  if(lastNoteNumber!=noteNumber){// && (micros()>noteStartedTime+4*1000000){
    noteStartedTime=micros();
    PlayNote(noteNumber);
  } 
}

void PlayNote(int noteNumber){
  Serial.println(noteNumber);
}


