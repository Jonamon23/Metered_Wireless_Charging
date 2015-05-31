//Simple code that reads a pin and mirrors its status to another pin
void setup() {
  pinMode(2, INPUT);
  pinMode(13, OUTPUT);

}

void loop() {
  if (digitalRead(2) == HIGH) {
    digitalWrite(13, LOW);
  }
  else
    digitalWrite(13, HIGH);
}
