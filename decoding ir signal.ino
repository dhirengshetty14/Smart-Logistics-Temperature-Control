//Decoding the IR signal for air-conditioner
//We decode the IR signal sent by the remote controller of air-conditioner. We press the buttons ON and OFF and the IR Receiver will receive this signal which after that is //decoded in order to send it back again afterwards.

#define IRpin_PIN PIND
#define IRpin 6
#define MAXPULSE 65000
#define RESOLUTION 20

//the array where the IR signal will be saved
uint16_t pulses[100][2];
uint8_t currentpulse = 0;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Ready to decode IR!");
}

void loop(void) {
    uint16_t highpulse, lowpulse;
    highpulse = lowpulse = 0;
  
    //reading the signal
    while (IRpin_PIN & (1 << IRpin)) {
      
      highpulse++;
      delayMicroseconds(RESOLUTION);

      if ((highpulse >= MAXPULSE) && (currentpulse != 0)) {
        printpulses(); //method for printing results
        currentpulse=0;
        return;
      }
    }

    pulses[currentpulse][0] = highpulse;
    //reading of signal
    while (! (IRpin_PIN & _BV(IRpin))) {

      lowpulse++;
      delayMicroseconds(RESOLUTION);
      if ((lowpulse >= MAXPULSE) && (currentpulse != 0)) {
        printpulses(); //method for printing results
        currentpulse=0;
        return;
      }
    }
    pulses[currentpulse][1] = lowpulse;

    currentpulse++;
}
//method for printing the decoding of IR signal
void printpulses(void) {
 
  Serial.println("int IRsignal[] = {");
  for (uint8_t i = 0; i < currentpulse-1; i++) {
    Serial.print("\t");
    Serial.print(pulses[i][1] * RESOLUTION / 10, DEC);
    Serial.print(", ");
    Serial.print(pulses[i+1][0] * RESOLUTION / 10, DEC);
    Serial.println(",");
  }
  Serial.print("\t");
  Serial.print(pulses[currentpulse-1][1] * RESOLUTION / 10, DEC);
  Serial.print(", 0};");
}