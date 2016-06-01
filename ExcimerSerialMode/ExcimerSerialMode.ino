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
                //40000
    int lightThreshold = 10000; //Threshhold for triggering the second pulse.
    int lowNotes = 0;
    int highNotes = 0;
    int dead = 0;
    int lasersOn = 0;
    double buttonNote = 0;
    double noteRatio = 0;
    int analogVal = 0;
    unsigned long curTime = 0;
    unsigned long handTime = 0;
    unsigned long lastTime = 0;
    bool clockHigh = 0; //Prevent clock thinking a full cycle has passed.
    unsigned long noteTime = 0;
    unsigned long noteLength = 5*100;
    unsigned long deadLength = 2*100*100;
    unsigned long prevNoteTime = 0;
    double doubZero = 0;


    
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
        
//        if(digitalRead(clockPin)){  //finds the time at the rising clock edge.
//          if (!clockHigh){
//            digitalWrite(flagPin2, HIGH);
//            lastTime = curTime;
//            curTime = micros();
//            clockHigh = 1; 
//            delay(1);
//            digitalWrite(flagPin2, LOW);
//          }
//        }else{clockHigh = 0;}

        if(analogRead(clockPinA)>clockThreshold ){  //finds the time at the rising clock edge.
          if (!clockHigh){
            digitalWrite(flagPin2, HIGH);
            lastTime = curTime;
            curTime = micros();
            clockHigh = 1; 
            delay(1);
            digitalWrite(flagPin2, LOW);
          }
        }else{clockHigh = 0;}

    //Serial.println(analogRead(clockPinA));
        
        
        analogVal = analogRead(handPin);
        //serial.println(analogVal);
        if (analogVal > lightThreshold){
          digitalWrite(flagPin, HIGH); //For the oscilloscope to track when we find the hand. 
          delay(1);
          digitalWrite(flagPin, LOW);
          handTime = micros();
        }
        Serial.println(analogVal);

        if (handTime > curTime){
          noteRatio = double(handTime-curTime)/double(curTime-lastTime);          
        }else{
          noteRatio = 0;
         // usbMIDI.sendNoteOn(noteRatio*100*3, 0, channel);
        }

        //if(lasersOn){
            //if (noteTime-prevNoteTime> noteLength){
          //      prevNoteTime = noteTime;
           //     noteTime = micros();
        
        if (noteRatio != doubZero){
            //Serial.println(int(noteRatio*100*3.5));
            //usbMIDI.sendNoteOn(int(noteRatio*100*3.5), 99, channel);
            dead = 0;
            //delay(100);
            //delay(1000);
        }else{
          if((noteTime-prevNoteTime> deadLength)&&!dead){
            dead = 1;
            //usbMIDI.sendNoteOn(61,0,channel);
          }
        }

    
        prevNoteTime = noteTime;
        noteTime = micros();

            
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

