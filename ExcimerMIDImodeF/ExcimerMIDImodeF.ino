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

    //Mode Filter Variable.      
    int filteredNote =0;
    const int BUFF_SIZE = 5;
    int Ari [BUFF_SIZE] = {13, 13, 13, 13, 13};
    int currentIndex =0;
    int SENTINEL_VAL = 13;
    int NUM_NOTES = 7;

    //Note Scaling Etc Variables
    double numNotes = 7; 
    //Using scale Increasing C Major
    //C-D-E-F-G-A-B-[C]
    //https://en.wikipedia.org/wiki/Scale_(music)
    double NoteMin= 56; 
    double NoteMax= 82;
    double NoteSpan = NoteMax-NoteMin;
    int noteSpot;
    int lastNoteNumber;

    //MIDI Note Constants
    int Octave = 4;
    int noteNumber;
    int lastPlayedNote;
    unsigned long  noteStartedTime;
    unsigned long noteHoldTime = 10*100000;

    //Current Note Plan: Make Note play for 4 seconds and fade slowly out after being pressed. Only start new note if it
    //sees a new _different_ note being played.
    
    

    
    void setup() {    // ==============================================================
      pinMode(LED1,OUTPUT);       // enable digital output for turning on LED indicator
      //pinMode(clockPinA, INPUT_PULLDOWN);
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

        //Serial.println(analogRead(clockPinA));//Debug, Use to set clockThreshold
        
        ///*
        trackHand();
        currPosition = currRatio;
        //if(currPosition != doubleZero) Serial.println(currPosition);
       ///* 
        //if(currPosition != doubleZero) Serial.println(currPosition);
        noteSpot = int(ceil((currPosition-NoteMin)/(NoteSpan)*numNotes))%int(numNotes+1) +1;
        if((currPosition>NoteMax) || (currPosition<NoteMin-7) || (currPosition == doubleZero)) noteSpot = 13; 
        PlaySpot(noteSpot);
        //Breaks up position into Notes. NoteMax Term Normalizes 0-100 as the note Position.
        //numNotes, mod 7, and ceil() buckets our positions into specific notes to play.
        //Plus one is to shift from 0-6 to 1-7

        //Serial.print("CurrPos:"); //Debug. If the numbers keep rising, it's an issue with the Secondary Photocell.
        //Serial.println(currPosition);
  

        
        //if(currPosition != doubleZero) Serial.println(noteSpot); 
           // }
       // }
        /*
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
          //delay(5);
          digitalWrite(LED1, LOW);
          usbMIDI.sendNoteOn(61, 99, channel +1 );  // 61 = C#4
        }else{usbMIDI.sendNoteOn(61, 0, channel +1);}  // 61 = C#4
         */
         // MIDI Controllers should discard incoming MIDI messages.
      //  while (usbMIDI.read()) {
      //  }
      
      //delay(1);  
      //*/
      }

void trackHand(){

       
       if(analogRead(clockPinA)>clockThreshold ){  //finds the time at the rising clock edge.
          if (!clockHigh){
            lastTime = curTime;
            curTime = micros();
            clockHigh = 1; 
            //Serial.println("Clocked!");

            digitalWrite(flagPin2, HIGH);
            delay(1);
            digitalWrite(flagPin2, LOW);
            }
        }else{clockHigh = 0;}

        if (analogRead(handPin) > lightThreshold){ // Finds Rising edge of Hand Clock Pulse
          handTime = micros();
          digitalWrite(flagPin, HIGH);
          delay(1); //Oscope Debug
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
    default: intermNum = 13; //13 is SENTINEL VALUE.
      break;
  }
  //PrintModeArray();
  if(intermNum!=13){
    Enqueue(intermNum);
    filteredNote = Mode();
    lastNoteNumber=noteNumber;
    //lastPlayedNote=noteNumber;
    noteNumber = 12*Octave + filteredNote;
  }else{ lastNoteNumber = 13;}
  
  //Serial.print("Note Played: ");
  //Serial.println(noteNumber);
  
  if((noteNumber != 13) && (lastNoteNumber==noteNumber)){
    noteStartedTime=micros();
  }
  if(micros()> noteStartedTime+noteHoldTime){
    //killNotes();
    usbMIDI.sendNoteOn(lastPlayedNote, 0, 1);
  }
  //Serial.print("LastNote: ");
  //Serial.println(lastNoteNumber);
  //Serial.print("Time till note Death");
  //Serial.println(noteStartedTime+noteHoldTime-micros());
  if((lastNoteNumber!=noteNumber) && (intermNum != 13)){// && (micros()>noteStartedTime+4*1000000){
    noteStartedTime=micros();
    
    usbMIDI.sendNoteOn(lastNoteNumber, 0, 1);
    //Serial.print("NoteOff:");
    //Serial.println(lastNoteNumber);
    PlayNote(noteNumber);
    lastPlayedNote = noteNumber;
    delay(200); //To Prevent MIDI Spamming
    lastTime = 0; //To reset Harp counter so it doesn't come online after delay and immediately start playing something dumb;
    //Serial.print("Note Played: ");
    //Serial.println(noteNumber);
  }  
  
}//End PlaySpot()

void PlayNote(int noteNumber){
  int channel=1;
  usbMIDI.sendNoteOn(noteNumber, 99, 1);
  Serial.print("MIDI: ");
  Serial.println(noteNumber);
  //delay(200);
}
void killNotes(){
  Serial.println("Kill Notes");
    for( int i =0; i <13; i++){
      usbMIDI.sendNoteOn(i+ 12*Octave, 0, 1);//Turns off Note (sets note Velocity to zero);
    } 
}
void Enqueue(int val){
  Ari[currentIndex] = val;
  currentIndex++;
  if(currentIndex>=BUFF_SIZE) currentIndex=currentIndex%BUFF_SIZE;
}

int Mode(){
  int noteFrequency [13] = {0,0,0,0,0,0,0,0,0,0,0,0,0}; //to allow for -1 sentinel value. (SENTINEL VAL has to be !-1, maybe 13?
  for(int i=0; i<sizeof(Ari)/sizeof(Ari[0]); i++){
     noteFrequency[Ari[i]]++;    //Iterates through all values once and tallies everytime a value shows up
  }
  int maxOccur = -1;
  int maxIndex =-1;
  for(int j=0; j<sizeof(noteFrequency)/sizeof(noteFrequency[0]); j++){ //Finds the maximum occurances for each index, then returns the index
    //(The index of this array is the noteNumber).
    if(noteFrequency[j]>maxOccur){
      maxOccur = noteFrequency[j];
      maxIndex = j;
    } 
  }
  return maxIndex; 
}


void PrintModeArray(){
  for(int i=0; i<sizeof(Ari)/sizeof(Ari[0]); i++){
     Serial.print(Ari[i]);
     Serial.print(",");
     }
  Serial.println("<^-^-^-Circular Array Values");
  Serial.print("Mode: ");
  Serial.println(Mode());   
  }


