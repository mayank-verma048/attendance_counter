

#include <LiquidCrystal.h>

LiquidCrystal lcd(7, 8, 9, 10, 11 , 12);
int inPin = 2;
int inPin2=3;
volatile int in,in2;
char charBuf[10],charBuf2[10];
volatile int lastVal=1,lastVal2=1;
volatile int lastValOut=1,lastVal2Out=1;
volatile int count=0,lastCount=0;
volatile int inStage=1,outStage=1;
int badCalibration=0;


void setup() { 
  lcd.begin(16, 2);
  lcd.setCursor(0,1);
  lcd.write("0");
  
  pinMode(inPin,INPUT);
  pinMode(inPin2,INPUT);
  
  //WARNING:Interrupts can be MISSED if ISR is too long! Think a million times before modifying ISRs. Especially avoid Serial printouts. They terribly slow down Arduino.
  //NOTE:If code doesn't work well, maybe ISR is already too long.
  attachInterrupt(0, pin_ISR, CHANGE);
  attachInterrupt(1, pin_ISR2, CHANGE);
  
  in = digitalRead(inPin);
  in2 = digitalRead(inPin2);
  
  //Tell it's bad calibration of both sensors don't detect IR emitter's light by this time.
  if(in!=1 || in2!=1){
   badCalibration=1;
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.write("Bad Calibration.");
   lcd.setCursor(0,1);
   lcd.write("Please Restart."); 
  }
  
}

void loop() {
}


//Detect object going in sequence
/*NOTE: 1.Designed for only one object at a time.
        2.Object jumping mid air shall lead to erratic output. In future timing can be taken into account to distinguish jumping from other patterns.
        3.Thin objects that cannot block both sensors are not handled. At best we can assume it passed through the sensors perfectly.*/
void patternIn(){

 //Stage 0 to 1: ?? --> 11
  if(in==1 && in2==1 && inStage==0){
    lastVal=1;
    lastVal2=1;
    inStage=1;
    return;
  }

 //Stage 1 to 2: 11 --> 01
  if(lastVal==1 && in==0 && lastVal2==1 && in2==1 && inStage==1){
    lastVal=0;
    lastVal2=1;
    inStage=2;
    
    outStage=0;
    
    return;
  }

 //Stage 2 to 3: 01 --> 00
  if(lastVal==0 && in==0 && lastVal2==1 && in2==0 && inStage==2){
    lastVal=0;
    lastVal2=0;
    inStage=3;
    return;
  }
 
 //Stage 3 to 4: 00 --> 10
  if(lastVal==0 && in==1 && lastVal2==0 && in2==0 && inStage==3){
    lastVal=1;
    lastVal2=0;
    inStage=4;
    return;
  }

 //Stage 4 to 5: 10 --> 11
  if(lastVal==1 && in==1 && lastVal2==0 && in2==1 && inStage==4){
    lastVal=1;
    lastVal2=1;
    count++;
    
    lcd.clear();
    lcd.begin(16, 2);
    lcd.setCursor(0,1);
    String(count).toCharArray(charBuf,10);
    lcd.write(charBuf);
    lastCount=count;
    inStage=1;

    outStage=1;
    
    return;
  }
  
  //The following pattern checks are for cases when the object goes to previous stages before completing the pattern.
  
  //Stage 2 to 1: 01 --> 11
  if(lastVal==0 && in==1 && lastVal2==1 && in2==1 && inStage==2){
    lastVal=1;
    lastVal2=1;
    inStage=1;
    
    outStage=1;
    
    return;
  }
  
  //Stage 3 to 2: 00 --> 01
  if(lastVal==0 && in==0 && lastVal2==0 && in2==1 && inStage==3){
    lastVal=0;
    lastVal2=1;
    inStage=2;
    return;
  }

  //Stage 4 to 3: 10 --> 00
  if(lastVal==1 && in==0 && lastVal2==0 && in2==0 && inStage==4){
    lastVal=0;
    lastVal2=0;
    inStage=3;
    return;
  }
  
}


//Detect object going out sequence
/*NOTE: 1.Designed for only one object at a time.
        2.Object jumping mid air shall lead to erratic output. In future timing can be taken into account to distinguish jumping from other patterns.
        3.Thin objects that cannot block both sensors are not handled. At best we can assume it passed through the sensors perfectly.*/
void patternOut(){

 //Stage 0 to 1: ?? --> 11
  if(in==1 && in2==1 && outStage==0){
    lastValOut=1;
    lastVal2=1;
    outStage=1;
    return;
  }
  
  //Stage 1 to 2: 11 --> 10
  if(lastValOut==1 && in==1 && lastVal2Out==1 && in2==0 && outStage==1){
    lastValOut=1;
    lastVal2Out=0;
    outStage=2;
    
    inStage=0;
    
    return;
  }
  
  //Stage 2 to 3: 10 --> 00
  if(lastValOut==1 && in==0 && lastVal2Out==0 && in2==0 && outStage==2){
    lastValOut=0;
    lastVal2Out=0;
    outStage=3;
    return;
  }

 //Stage 3 to 4: 00 --> 01
  if(lastValOut==0 && in==0 && lastVal2Out==0 && in2==1 && outStage==3){
    lastValOut=0;
    lastVal2Out=1;
    outStage=4;
    return;
  }

 //Stage 4 to 5: 01 --> 11
  if(lastValOut==0 && in==1 && lastVal2Out==1 && in2==1 && outStage==4){
    lastValOut=1;
    lastVal2Out=1;
    count--;
    
    lcd.clear();
    lcd.begin(16, 2);
    lcd.setCursor(0,1);
    String(count).toCharArray(charBuf,10);
    lcd.write(charBuf);
    lastCount=count;
    outStage=1;
    
    inStage=1;
    
    return;
  }
 
 //The following pattern checks are for cases when the object goes to previous stages before completing the pattern.

 //Stage 2 to 1: 10 --> 11
 if(lastValOut==1 && in==1 && lastVal2Out==0 && in2==1 && outStage==2){
    lastValOut=1;
    lastVal2Out=1;
    outStage=1;
    
    inStage=1;
    
    return;
  }
  
  //Stage 3 to 2: 00 --> 10
  if(lastValOut==0 && in==1 && lastVal2Out==0 && in2==0 && outStage==3){
    lastValOut=1;
    lastVal2Out=0;
    outStage=2;
    return;
  }
  
  //Stage 4 to 3: 01 --> 00
  if(lastValOut==0 && in==0 && lastVal2Out==1 && in2==0 && outStage==4){
    lastValOut=0;
    lastVal2Out=0;
    outStage=3;
    return;
  }
  
}

void pin_ISR(){
  
  if(badCalibration)return;
  
  in = digitalRead(inPin);
  patternIn();
  patternOut();
}

void pin_ISR2(){
  
  if(badCalibration)return;

  in2=digitalRead(inPin2);
  patternIn();
  patternOut();
}
