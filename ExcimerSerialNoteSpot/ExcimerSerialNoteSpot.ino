//Laser Harp
//Jake Hillard Spring 2016

  
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
    int lightThreshold = 10000; //Threshhold for triggering the second pulse.
    double currPos;
   
    //Filter Variables. 
    double currRatio = 0;
    int noteThreshold = 6; //Number of times Excimer must see the note before sending it as MIDI
    int buffCounter = 0;  //The current number of times Excimer has seen a note.

    //Note Scaling Etc Variables
    int numNotes = 7; 
    //Using scale Increasing C Major
    //C-D-E-F-G-A-B-[C]
    //https://en.wikipedia.org/wiki/Scale_(music)
    double NoteMin= 11.5; 
    double NoteMax= 21;
    double NoteSpan = NoteMax-NoteMin;
    int NoteSpot;
    
    

    
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
        NoteSpot = ceil((CurrPosition-NoteMin)/NoteSpan);
        Serial.println(NoteSpot); 


    
            
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

