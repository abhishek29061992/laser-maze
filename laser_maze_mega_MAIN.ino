/*
commands sent to master:
'q'= a cut detected
'w'=timer begin (start button pressed)
'e'=timer pause (stop button pressed)
*/
int m[14]= {30,31,32,33,34,35,36,37,38,39,40,41,42,43}; // mosfet pin map: lights on 43,42,30, fog on 41
int b1=20; //int b1=26;  //connected to connector 'sensors_3(+sw.)' RJ45 jack: START button //changed to 20 for interrupt
int b2=21; //int b2=27;  //connected to connector 'sensors_3(+sw.)' RJ45 jack: //changed to 21 for interrupt
int b3=28;  //not connected
int b4=29; //not connected
int s[16]={A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15}; //analog pins
unsigned long threslow[16], threshigh[16]; // threshold data array
int i,j,k,l;  //general counters
char startb=0;
char stopb=0;
void startpressed();
void stoppressed();
void thresholdcheck();

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
char state=0;
char state2=0;
//char cut=0;


//--------------------------------------------------

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  for (i=0; i<14; i++){
      pinMode(m[i],OUTPUT); }
  //for (i=0; i<16; i++){
  //    pinMode(s[i],INPUT); }
  for (i=0; i<16; i++){
      threslow[i]=0;
      threshigh[i]=0; }
      
      
          digitalWrite(m[0],HIGH); digitalWrite(m[12],HIGH);digitalWrite(m[13],HIGH); //switch on ambient lights
        digitalWrite(m[12],HIGH);  //fog release
        delay(2000);
      digitalWrite(m[12],LOW);
      
      // setting LDR threshold:
         for (i=2; i<11; i++){       // switch off lasers
      digitalWrite(m[i],LOW); }
      
      for (i=0; i<1000; i++)    //accumulate values
      {
        for (j=0; j<16; j++){
          threslow[j]+=analogRead(s[j]);
        }
      }      
      
         for (i=2; i<11; i++){       // switch on lasers
      digitalWrite(m[i],HIGH); }
      
       for (i=0; i<1000; i++)    //accumulate values
      {
        for (j=0; j<16; j++){
          threshigh[j]+=analogRead(s[j]);
        }
      }
      
      for (i=0; i<16; i++){    //averaging 
      threslow[i]=threslow[i]/16;                // NOTE: shift the thresholds a bit!
      threshigh[i]=(threshigh[i]/16)+50;  }
      
      
attachInterrupt(2, startpressed, RISING); //interrupt trigger
attachInterrupt(3, stoppressed, RISING); //interrupt trigger
      
}

//---------------------------------------------------
void loop()
{
  
  
  
  if (state==1) //reset cmd from master: ambient lights off, lasers on, release fog, timer reset cmd,
  {
    if (state2==0)
    {   
    digitalWrite(m[0],LOW); digitalWrite(m[12],LOW);digitalWrite(m[13],LOW); //ambient light
    for (k=1;k<11;k++) {digitalWrite(m[k],HIGH); }//lasers
       digitalWrite(m[12],HIGH);  //fog
        delay(2000);
      digitalWrite(m[12],LOW);
      Serial1.write('a'); //timer rst
      state2=1;
      startb=0; //just to be sure; start button pressed flag
    }
    else {    
    if (startb) {state=2; state2=2; startb=0;} //start button pressed
    
    }
  }
  
  
  if (state==2) //Begin game: timer start cmd, cuts report
  {     
    if (state2==2) {Serial1.write('s'); Serial.write('w');state2=3;}// start timer cmd
    thresholdcheck();
    if (stopb) {state=3; state2=3; stopb=0;}
  }
  
  
  if (state==3) //end game: pause timer cmd, ambient lights on, lasers off
  {
    if (state2==3) {Serial1.write('d'); Serial.write('e'); state2=0;}// pause timer cmd
    digitalWrite(m[0],HIGH); digitalWrite(m[12],HIGH);digitalWrite(m[13],HIGH); //ambient light
    for (k=1;k<11;k++) {digitalWrite(m[k],LOW); }//lasers    
  } 
  
}


//----------------------------------------------------------------------

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read(); 
   
    if (inChar == 'a') { //reset command from master
      int state=1;
    }
    
    if (inChar=='b')    //release fog for 2s
     {  digitalWrite(m[12],HIGH);
        delay(2000);
      digitalWrite(m[12],LOW);
     }
    }
}

//------------------------------------------------------

void thresholdcheck() //to scan adc and report a cut
{
  for (l=0; l<16 ;l++)
 {
  if (analogRead(s[l])<threshigh[l]) Serial.write('q');
 }     
  //if (cut) { Serial.write('q'); cut=0;}
}

//--------------------------------------------------------
void startpressed()
{
  startb=1; //start button pressed flag
}

void stoppressed()
{
  stopb=1; //stop button pressed flag
}

