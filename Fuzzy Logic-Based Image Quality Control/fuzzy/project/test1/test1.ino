int brightness = 0;    // how bright the LED is
int fadeAmount = 1;    // how many points to fade the LED by

 
void setup()  {
  // declare pin 9 to be an output:
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
 
  // change the brightness for next time through the loop:

  brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:

  if (brightness == 0 || brightness == 255) {

    fadeAmount = -fadeAmount ;

  }    
  // wait for 30 milliseconds to see the dimming effect   

  delay(100);                           

}

