//Default baud speed for communication
#define BAUD 9600


int brightness = 0;    // how bright the LED is

void setup(){
  Serial.begin(BAUD);
  
  pinMode(9, OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(13,OUTPUT);
}

void loop(){
  //String input;
  //If any input is detected in arduino
  if(Serial.available() > 0){

    digitalWrite(13, HIGH);
    brightness = Serial.read();
    if(brightness>0){
    analogWrite(9, brightness);
    analogWrite(10, brightness);
    analogWrite(11, brightness);   
    analogWrite(3, brightness);
    analogWrite(5, brightness);
    analogWrite(6, brightness);
    }
    //Serial.print(brightness);

    //delay(1000);
    
    }
   
    
  
}

