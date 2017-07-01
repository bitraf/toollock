 

int outputPin = D0;
int outputValue = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(outputPin, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  
  // put your main code here, to run repeatedly:
  digitalWrite(outputPin, HIGH);
  digitalWrite(D4, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(3000);
  digitalWrite(outputPin, LOW);
  digitalWr ite(D4, LOW);    // turn the LED off by making the voltage LOW
  delay(3000);
}
