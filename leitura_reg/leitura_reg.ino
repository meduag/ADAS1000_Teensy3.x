#include <SPI.h>

//configuracion datos del dac
const int CS_PIN = 10;

SPISettings settingsSPI(1000000, MSBFIRST, SPI_MODE3); // Figure 2 - pg 11

unsigned char valueToWrite[4] = {0, 0, 0, 0};
unsigned char readDatain[4] = {0, 0, 0, 0};

int f = 1;
int g = 1;

void setup() {
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);
  delay(1000);

  SPI.begin();
  //Serial.begin(2000000);
  Serial.begin(115200);
}

void loop() {

  if (f == 1) {
    delay(500);
    Serial.println("\n\nWRITE >> CMREFCTL");
    SPI_Write(1, 5, 0b111000000000000000001011); // Original - teste //SPI_Write(1, 5, 0b110000001010101010001001);
    Serial.println("FIN WRITE");
    f = 0;
  }

  delay(100);

  if (g == 1) {
    delay(500);
    Serial.println("\n\nREAD >> CMREFCTL");
    SPI_Read(0, 5, 0); // Original - teste //SPI_Write(1, 5, 0b110000001010101010001001);
    Serial.println("FIN READ");
    g = 0;
  }

  delay(200);
  f = 1;
  g = 1;

}// Fin setup


/****************************************************************************************/
void SPI_Write(bool R_W, int adress, unsigned long data) {

  separa_bytes(R_W, adress, data);

  SPI.beginTransaction(settingsSPI);
  digitalWrite(CS_PIN, LOW);

  for (int j = 0; j < 4; j++) {
    SPI.transfer(valueToWrite[j]);
  }

  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();

} // fin spi write


/****************************************************************************************/
void SPI_Read(bool R_W2, int adress2, unsigned long data2) {

  Serial.print("Data WRITE do READ ------------------------------------- \n");
  Serial.print("RW: ");
  Serial.print(R_W2);
  Serial.print("   Add:");
  Serial.print(adress2);
  Serial.print("   Data2: ");
  Serial.println(data2);

  SPI_Write(R_W2, adress2, data2);
  //valueToWrite[0] = (R_W2 << 7 ) | (adress2 << 0); // monta o byte con RW e ADD >>> MSB >>> 2kHz
  //readDatain[0] = valueToWrite[0];

  for (int j = 0; j < 4; j++) {
    readDatain[j] = 0;
  }

  SPI.beginTransaction(settingsSPI);
  digitalWrite(CS_PIN, LOW);

  //SPI.transfer(valueToWrite[0]);

  for (int j = 1; j < 4; j++) {
    Serial.print("SPI.transfer: ");
    Serial.println(SPI.transfer(0), BIN);
    //readDatain[j] = SPI.transfer(0);
  }

  digitalWrite(CS_PIN, HIGH);
  SPI.endTransaction();

  Serial.println("----------- Leitura do REG -----------");
  print_datain(1);
  delay(100);

}

/****************************************************************************************/
void separa_bytes(bool R_W3, int adress3, unsigned long data3) {

  for (int j = 0; j < 4; j++) {
    valueToWrite[j] = 0;
  }

  valueToWrite[0] = (R_W3 << 7 ) | (adress3 << 0); // monta o byte con RW e ADD >>> MSB >>> 2kHz

  // Extrai o MSB de data
  for (int i = 16; i < 24; i++) {
    bool rb = bitRead(data3, i);
    valueToWrite[1] = (rb << (i - 16)) | valueToWrite[1];
  }

  valueToWrite[2] = highByte(data3); //extrai o segundo byte
  valueToWrite[3] = lowByte(data3); //extrai o primeiro byte >>> LSB

  print_datain(0);
}

void print_datain(int sel) {

  if (sel == 0) {
    Serial.print("Data WRITE ------------------------------------- \n");
    Serial.print("Byte 1: ");
    Serial.print(valueToWrite[0], BIN);
    Serial.print("\t Byte 2: ");
    Serial.print(valueToWrite[1], BIN);
    Serial.print("\t Byte 3: ");
    Serial.print(valueToWrite[2], BIN);
    Serial.print("\t Byte 4: ");
    Serial.println(valueToWrite[3], BIN);
  } else {
    Serial.print("Data READ ------------------------------------- \n");
    Serial.print("Byte 1: ");
    Serial.print(readDatain[0], BIN);
    Serial.print("\t Byte 2: ");
    Serial.print(readDatain[1], BIN);
    Serial.print("\t Byte 3: ");
    Serial.print(readDatain[2], BIN);
    Serial.print("\t Byte 4: ");
    Serial.println(readDatain[3], BIN);
  }
}
