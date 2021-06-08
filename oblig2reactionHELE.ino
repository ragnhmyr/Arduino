#include<Servo.h> //use Servo library

int runde=1;
int poeng1=0;
int poeng2=0;

int pin1=2; //player 1
int button1=0;
unsigned long finishTime1=0;
bool pushed1=false;

int pin2=10; //player 2
int button2=0;
unsigned long finishTime2=0;
bool pushed2=false;

bool bothPushed=false;

int pinLed=6; //where the LED is 
int resetButton=11; //where the Reset button is

int foundWinner=false; //haven't found a winner yet

//LEDs to keep track of points
int ledPlayer1[]={3,4,5};
int ledPlayer2[]={7,8,9};

//Servo to raise winner-flag
int pinServo=12; //pin for Servo
Servo myServo;

//Note frequencies 
//frequency paa de ulike notene
//frequencies paa de ulike notene funnet paa: https://www.arduino.cc/en/Tutorial/BuiltInExamples/toneMelody 
//ja vi elsker-noter funnet paa: http://lakkegata.no/noter/JaViElsker_Klarinett1.jpg 
//delt opp i 5 deler for aa faa en storre pause mellom hver del slik notene viser
unsigned int h1=247; //kanskje 233
unsigned  int c=261;
unsigned  int d=294;
unsigned  int e=329;
unsigned  int f=349;
unsigned  int g=392;
unsigned  int a=440;
unsigned  int h=493;
unsigned  int c2=523;
unsigned int d2=587;
unsigned int e2=659;
unsigned int f2=698;
unsigned  long dur=1000;
//startsignal: prove h1, h1, h1, h4
const int piezoPin=A0; //pinen paa arduinoen funker med A0
int pauseBetweenNotes=500;

//notene til 'Ja vi elsker' delt opp i 5 for aa inkludere storre pauser
unsigned int noter1[]={g,f,e,d,c,d,e,f,g,a,g,f,e};
unsigned long varighet1[]={dur*1.5,dur/8,dur,dur,dur,dur,dur,dur,dur*1.5,dur/8,dur,dur,dur*2.5};

unsigned int noter2[]={a,g,f,e,d,e,f,g,g,a,a,h,c2};
unsigned long varighet2[]={dur*1.5,dur/8,dur,dur,dur,dur,dur,dur,dur*1.5,dur/8,dur,dur,dur*2.5};

unsigned int noter3[]={c2,c2,h,h,a,a,g,e,a,a,g,g,f};
unsigned long varighet3[]={dur*1.5,dur/8,(dur/8)*1.5,dur/16,(dur/8)*1.5,dur/16,dur*2,dur*2,dur*1.5,dur/8,dur,dur,dur*2};

unsigned int noter4[]={g,g,g,a,a,h,h,c2,c2,c2,h,c2,d2};
unsigned long varighet4[]={(dur/8)*1.5,dur/16,dur,dur,dur,dur,dur*2,dur*2,dur*1.5,dur/8,dur,dur,dur*2};

unsigned int noter5[]={d2,d2,d2,e2,f2,e2,d2,c2,h,a,g,a,a,h,c2};
unsigned long varighet5[]={(dur/8)*1.5,dur/16,dur,dur,dur,dur,dur*2,dur,(dur/8)*1.5,dur/16,dur*1.5,dur/8,dur,dur,dur*2.5};


void setup() {
  // put your setup code here, to run once:
  pinMode(pin1,INPUT); //player 1 in pin 2
  pinMode(pin2,INPUT); //player 2 in pin 4
  pinMode(pinLed,OUTPUT); //LED
  pinMode(resetButton,INPUT); //Reset button
  for (int i=0;i<3;i++){
    pinMode(ledPlayer1[i],OUTPUT); //set all LEDs to output
    pinMode(ledPlayer2[i],OUTPUT);
  }
  pinMode(piezoPin,OUTPUT);
  myServo.attach(pinServo);
  Serial.begin(9600); //initialize serial communication at 9600 bits per second
}


void loop() {
  // put your main code here, to run repeatedly:
  if (digitalRead(resetButton)){ //if start/reset button is pushed
    runde=1; //reset round 
    poeng1=0;
    poeng2=0; //reset points
    writePointsLED(poeng1,poeng2); //display zero points
    myServo.write(90);
    foundWinner=false;
    while (foundWinner==false && !digitalRead(resetButton)){
      newGame();
    }
  }
}

void newGame(){
  //millisDelay(1000);
  Serial.print("Round ");
  Serial.println(runde);
  //millisDelay(1000);
  Serial.println("Ready");
  //millisDelay(1000);
  Serial.println("Set");
  //millisDelay(1000);
  Serial.println("Go!");
  playStartSignal();
  int randomNumber = random(2,10);
  int startLight=randomNumber*1000; //the LED will light up after 2-10 seconds
  //millisDelay(startLight);
  
  long startTime=millis();
  bool timePassed=false;
  bool early1=false;
  bool early2=false;
  while (timePassed==false){
    long currentTime=millis();
    if (digitalRead(pin1)==HIGH){
      early1=true;
    }
    if (digitalRead(pin2)==HIGH){
      early2=true;
    }
    if (currentTime-startTime>=startLight){
      timePassed=true;
    }
  }
  minusPoints(early1,early2); // make minus points if any player has pushed button too early
  if (early1 || early2){ //if one or both has pushed too early
    Serial.println("Too early!");
    writePointsSerial(poeng1,poeng2);
    writePointsLED(poeng1,poeng2);
    return; //start same round again
  }
  digitalWrite(pinLed,HIGH);
  //unsigned long currentTime=millis(); //start time
  
  while (bothPushed==false){
    button1=digitalRead(pin1);
    button2=digitalRead(pin2);
    if (button1==HIGH && pushed1==false){
      finishTime1=millis();
      pushed1=true;
    }
    if (button2==HIGH && pushed2==false){
      finishTime2=millis();
      pushed2=true;
    }
    if (pushed1 && pushed2){
      bothPushed=true; //both must have pushed button to end the round, to compare times
    }
//    if (pushed1 || pushed2){ //if one has pushed
//      bothPushed=true; //end round
//    }
  }
  bothPushed=false; //reset variable
  pushed1=false; //reset variable
  pushed2=false; //reset variable
  
  int roundWinner = winner(finishTime1,finishTime2);
  if (roundWinner==1){
    poeng1++;
  }
  else if (roundWinner==2){
    poeng2++;
  }

  digitalWrite(pinLed,LOW);
  writePointsSerial(poeng1,poeng2);
  writePointsLED(poeng1,poeng2);
  millisDelay(1000);

  runde++;
  if (poeng1==3 || poeng2==3){
    writeWinnerSerial(poeng1,poeng2);
    //writePointsSerial(poeng1,poeng2);
    writePointsLED(poeng1,poeng2);
    winFlag(poeng1,poeng2);
    playJaViElsker();
    foundWinner=true;

  }
}

void minusPoints(bool player1,bool player2){
  if (player1){
    poeng1--; //minus 1 point
    if (poeng1<0){
      poeng1=0;
    }
  }
  if (player2){
    poeng2--;
    if (poeng2<0){
      poeng2=0;
    }
  }
}

int winner(unsigned long time1,unsigned long time2){
  if (time1<time2){
    return 1; //player 1 wins
  }
  else if (time2<time1){
    return 2; //player 2 wins
  }
  else {
    return 0;
  }
}

void writePointsSerial(int player1, int player2){
  Serial.print("Player 1: ");
  Serial.println(player1);
  Serial.print("Player 2: ");
  Serial.println(player2);
}

void writePointsLED(int player1, int player2){
  //first turn all LEDs off to remove points
  for (int h=0; h<3;h++){
    digitalWrite(ledPlayer1[h],LOW);
    digitalWrite(ledPlayer2[h],LOW);
  }
  for (int i=0; i<player1;i++){
    digitalWrite(ledPlayer1[i],HIGH); //turn points ON
  }
  for (int j=0; j<player2;j++){
    digitalWrite(ledPlayer2[j],HIGH); //turn points ON
  }
}

void writeWinnerSerial(int player1, int player2){
  Serial.print("GAME FINISHED \nThe winner is.... ");
  if (player1>player2){
    Serial.println("PLAYER 1");
  }
  else{
    Serial.println("PLAYER 2");
  }
  Serial.println("CONGRATULATIONS!");
  writePointsSerial(player1,player2);
}

void millisDelay(long period){
  long startTime=millis();
  bool check = false;
  while (check==false){
    long currentTime=millis();
    if (currentTime-startTime>=period){
      check=true;
    }
  }
}

void winFlag(int player1,int player2){ //left,right. The flag rises for player which has 3 points
  //under 90 grader en vei, over 90 grader andre veien
  int pause=700; //5 intervaller, slik at flagget er 'til topps' paa etter forste setning i sang
  if (player1==3){
    for (int i=90; i>=35; i-=5){
      myServo.write(i);
      delay(pause);
    }
  }
  else if (player2==3){
    for (int i=90; i<=145; i+=5){
      myServo.write(i);
      delay(pause);
    }
  }

  else {
    myServo.write(90);
  }
  }

void playStartSignal(){
  tone(piezoPin,h1);
  delay(pauseBetweenNotes);
  noTone(piezoPin);
  delay(pauseBetweenNotes);
  tone(piezoPin,h1);
  delay(pauseBetweenNotes);
  noTone(piezoPin);
  delay(pauseBetweenNotes);
  tone(piezoPin,h1);
  delay(pauseBetweenNotes);
  noTone(piezoPin);
  delay(pauseBetweenNotes);
  tone(piezoPin,h);
  delay(pauseBetweenNotes);
  noTone(piezoPin);
}

void playJaViElsker(){
  //forste del
  for (int i=0; i<(sizeof(noter1)/sizeof(noter1[0]));i++){
    tone(piezoPin,noter1[i],varighet1[i]);
    delay(pauseBetweenNotes);
    noTone(piezoPin);
  }
  noTone(piezoPin);
  delay(dur);

  //andre del
  for (int i=0; i<(sizeof(noter2)/sizeof(noter2[0]));i++){
    tone(piezoPin,noter2[i],varighet2[i]);
    delay(pauseBetweenNotes);
    noTone(piezoPin);
  }
  noTone(piezoPin);
  delay(dur);

  //tredje del
  for (int i=0; i<(sizeof(noter3)/sizeof(noter3[0]));i++){
    tone(piezoPin,noter3[i],varighet3[i]);
    delay(pauseBetweenNotes);
    noTone(piezoPin);
  }
  noTone(piezoPin);
  delay(dur);

  //fjerde del 
  for (int i=0; i<(sizeof(noter4)/sizeof(noter4[0]));i++){
    tone(piezoPin,noter4[i],varighet4[i]);
    delay(pauseBetweenNotes);
    noTone(piezoPin);
  }
  noTone(piezoPin);
  delay(dur);

  //femte og siste del 
   for (int i=0; i<(sizeof(noter5)/sizeof(noter5[0]));i++){
    tone(piezoPin,noter5[i],varighet5[i]);
    delay(pauseBetweenNotes);
    noTone(piezoPin);
  }
  noTone(piezoPin);
}
