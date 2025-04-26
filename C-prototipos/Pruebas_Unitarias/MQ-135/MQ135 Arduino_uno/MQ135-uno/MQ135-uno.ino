void setup() {
  Serial.begin(9600);
  Serial.println("MQ135 Proteus Library by Satyam Singh");

}

void loop() {
  int raw_data=analogRead (A0);
  Serial.print ("CO2 raw Value = ");
  Serial.print (raw_data);
  Serial.println("");
  delay(1000);

}
