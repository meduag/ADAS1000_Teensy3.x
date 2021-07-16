#include "SPI.h"

const byte chipselect = 10;
const byte num_bytes = 4;
int t = 1;

int f = 0;

SPISettings conf(1000000, MSBFIRST, SPI_MODE3);

void setup() {
  pinMode(chipselect, OUTPUT);
  digitalWrite(chipselect, HIGH);

  SPI.begin();
  Serial.begin(115200);
}

void loop() {
  SPI_write(1, 5, 0x85E0000B);
  //if (f < 3) {
  /*  SPI_write(1, 1, 0xA2);
    delay(10);
    SPI_write(1, 1, 0x22);
    delay(10);*/
  // f = f + 1;
  //}

  //delayMicroseconds(2);
  //SPI_write(1, 5, 0xF0F0F0F0);
  SPI_read(5);
  //delayMicroseconds(4);
  //delayMicroseconds(20);

  /*agora = millis();
    while (millis() < agora + delayt) {}
    SPI_write(0, 1, 0);
    agora = millis();
    while (millis() < agora + delayt) {}*/
}

void SPI_write(bool R_W, int adress, unsigned long data) {

  unsigned char valueToWrite[num_bytes] = {0, 0, 0, 0};

  valueToWrite[0] = (R_W << 7 ) | (adress << 0); // monta o byte con RW e ADD >>> MSB >>> 2kHz

  // Extrai o MSB de data
  for (int i = 16; i < 24; i++) {
    bool rb = bitRead(data, i);
    valueToWrite[1] = (rb << (i - 16)) | valueToWrite[1];
  }

  valueToWrite[2] = highByte(data); //extrai o segundo byte
  valueToWrite[3] = lowByte(data); //extrai o primeiro byte >>> LSB

  /* Serial.println("Write SPI");
    for (byte i = 0; i < num_bytes; i++) {
     Serial.print("valueToWrite[");
     Serial.print(i);
     Serial.print("]: ");
     Serial.println(valueToWrite[i], BIN);
    }*/

  SPI.beginTransaction(conf);
  digitalWrite(chipselect, LOW); // enable shifting
  delayMicroseconds(t);
  for (byte i = 0; i < num_bytes; i++) {
    SPI.transfer(valueToWrite[i]);
    //delayMicroseconds(1);
  }
  delayMicroseconds(t);
  digitalWrite(chipselect, HIGH); // disable clock again*/
  SPI.endTransaction();
}

void SPI_read(int adress) {

  unsigned char readCmd[num_bytes] = {0, 0, 0, 0};

  SPI_write(0, adress, 0);

  delayMicroseconds(5);

  SPI.beginTransaction(conf);
  digitalWrite(chipselect, LOW); // enable shifting
  delayMicroseconds(t);
  for (int i = 0; i < num_bytes; i++) {
    readCmd[i] = SPI.transfer(0);
    //delayMicroseconds(1);
  }
  delayMicroseconds(t);
  digitalWrite(chipselect, HIGH); // disable clock again*/
  SPI.endTransaction();

  Serial.println("Read SPI");
  byte i;
  for (i = 0; i < num_bytes; i++) {
    Serial.print("readCmd[");
    Serial.print(i);
    Serial.print("]: ");
    Serial.println(readCmd[i], BIN);
  }
}
