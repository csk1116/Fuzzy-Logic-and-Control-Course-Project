int brightness = 255;    // how bright the LED is


 void setup()  {
  
  pinMode(9, OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
}

void loop()  {

  // set the brightness of pin 9:

  analogWrite(9, brightness);
  analogWrite(10, brightness);
  analogWrite(11, brightness);   
  analogWrite(3, brightness);
  analogWrite(5, brightness);
  analogWrite(6, brightness);
 
  

  //delay(10);                           

}
