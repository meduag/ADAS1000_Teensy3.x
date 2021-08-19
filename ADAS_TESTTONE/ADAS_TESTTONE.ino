#include "SPI.h"
#include "ADAS1000.h"
#include <stdio.h>
#include <stdlib.h>

// Pinos do Chip Select
#define CS_Master 10
#define CS_Slave 9

// Para selecionar Master ou Slave
#define CS_0 0  // Master
#define CS_1 1  // Slave

// Para habilitar resincronização ou nao
#define Con_resync 1
#define Sem_resync 0

// Para escrever ou ler o Reg em questao
#define Read 0
#define Write 1
#define Frame 2

// Numero de Bytes na msg
const byte num_bytes = 4;

// Pinos dos botões
const int bt1 = 0;
const int bt2 = 4;

int nreg = 0;


int t = 5;

int def_reg[] =
{
  1, 2, 3, 155, 229, 6, 115, 8, 41, 161, 11, 522, 13, 14, 15
};

bool only_print = true;

//Configuraçoes do Barramento SPI
SPISettings conf_tx(2000000, MSBFIRST, SPI_MODE0);
SPISettings conf_rx(2000000, MSBFIRST, SPI_MODE0);

/********************************************************************/
/********************************************************************/
void setup() {
  pinMode(CS_Master, OUTPUT);
  digitalWrite(CS_Master, HIGH);

  pinMode(CS_Slave, OUTPUT);
  digitalWrite(CS_Slave, HIGH);

  SPI.begin();
  Serial.begin(2000000);
}

/********************************************************************/
/********************************************************************/
void loop() {
  if (Serial.available()) {
    char data = Serial.read();
    Serial.readString(); // Tira o resto do buffer  - hace un flush forzado

    /*************************************************************************************************************/
    if (data == '1') {
      delay(300);

      if (only_print)
        Serial.println("1-Resync & 2-W & 3-Read");

      SPI_write(Write, ADAS1000_ECGCTL, 0x8100003E, Con_resync, CS_0, true);
      delayMicroseconds(15);

      SPI_read(ADAS1000_ECGCTL, Sem_resync, CS_0, true);

      if (only_print)
        Serial.println("\n");

      for (int reg_address = 1; reg_address < 16; reg_address++) {
        if (only_print) {
          Serial.print("A[0x0");
          Serial.print(reg_address, HEX);
          Serial.print("]\t");
          Serial.print(" - ");
        }

        SPI_read(reg_address, Sem_resync, CS_0, true);

        if (def_reg[reg_address - 1] == nreg) {
          if (only_print)
            Serial.println("- OK");
          nreg = 0;
        } else {
          if (only_print)
            Serial.println("- REG CHANGE");
        }
      }
    }// fin data 1

    /*************************************************************************************************************/
    if (data == '2') { // Test Tone
      SPI_write(Write, ADAS1000_ECGCTL, 0x81F800FE, Con_resync, CS_0, true);
      delayMicroseconds(15);
      SPI_read(ADAS1000_ECGCTL, Sem_resync, CS_0, true);
      Serial.println();

      delayMicroseconds(15);

      SPI_write(Write, ADAS1000_CMREFCTL, 0x8500000A, Sem_resync, CS_0, true);
      delayMicroseconds(15);
      SPI_read(ADAS1000_CMREFCTL, Sem_resync, CS_0, true);
      Serial.println();

      delayMicroseconds(15);

      SPI_write(Write, ADAS1000_TESTTONE, 0x88F84006, Sem_resync, CS_0, true);
      delayMicroseconds(15);
      SPI_read(ADAS1000_TESTTONE, Sem_resync, CS_0, true);
      Serial.println();

      delayMicroseconds(15);

      SPI_write(Write, ADAS1000_FILTCTL, 0x8B000000, Sem_resync, CS_0, true);
      delayMicroseconds(15);
      SPI_read(ADAS1000_FILTCTL, Sem_resync, CS_0, true);
      Serial.println();

      delayMicroseconds(15);

      SPI_write(Write, ADAS1000_FRMCTL, 0x8A079010, Sem_resync, CS_0, true);
      delayMicroseconds(15);
      SPI_read(ADAS1000_FRMCTL, Sem_resync, CS_0, true);
      Serial.println();

      // começa a leitura
      data = '0';
      int para = 0;
      SPI_write(Frame, ADAS1000_FRAMES, 0x40000000, Sem_resync, CS_0, true);

      while (data == '0') {
        if (Serial.available()) {
          data = Serial.read();
          Serial.readString(); // Tira o resto do buffer  - hace un flush forzado
        }

        SPI_read_only(0, Sem_resync, CS_0, true);

        if (data != '0') {
          data = '0';
          para = 1;
        }

        if (nreg == 200 || nreg == 240) {
          Serial.println("");
          if (para == 1) {
            data = '4';
          }
        }

      }// fin while

    }// fin data 2

    /*************************************************************************************************************/
    if (data == '3') { // ECG
      bool ptr = false;
      SPI_write(Write, ADAS1000_ECGCTL, 0x81F800BE, Con_resync, CS_0, ptr);
      delayMicroseconds(15);
      SPI_read(ADAS1000_ECGCTL, Sem_resync, CS_0, ptr);
      Serial.println();

      delayMicroseconds(15);

      SPI_write(Write, ADAS1000_CMREFCTL, 0x85E0000A, Sem_resync, CS_0, ptr);
      delayMicroseconds(15);
      SPI_read(ADAS1000_CMREFCTL, Sem_resync, CS_0, ptr);
      Serial.println();

      delayMicroseconds(15);

      SPI_write(Write, ADAS1000_FRMCTL, 0x8A079000, Sem_resync, CS_0, ptr);
      delayMicroseconds(15);
      SPI_read(ADAS1000_FRMCTL, Sem_resync, CS_0, ptr);
      Serial.println();

      delayMicroseconds(15);

      // começa a leitura
      data = '0';
      int para = 0;
      SPI_write(Frame, ADAS1000_FRAMES, 0x40000000, Sem_resync, CS_0, ptr);
      //int contador = 0;

      while (data == '0') {
        if (Serial.available()) {
          data = Serial.read();
          Serial.readString(); // Tira o resto do buffer  - hace un flush forzado
        }

        SPI_read_only(0, Sem_resync, CS_0, true);

        if (data != '0') {
          data = '0';
          para = 1;
        }

        if (nreg == 128 || nreg == 176 || nreg == 192) {
          Serial.println("");
          if (para == 1) {
            data = '4';
          }
        }
        
      }// fin while

    }// fin data 2


  }// fin serial ena

}// fin loop

/****************************************************************************/
/* Funcion de escritura SPI
*****************************************************************************/
void SPI_write(int R_W, int adress, unsigned long data, bool rs, int cs, bool only_print_w) {

  // Array de Bytes para enviar
  unsigned char valueToWrite[num_bytes] =  { 0, 0, 0, 0 };


  if (R_W == 0) {
    valueToWrite[0] = 0x7F & adress;
  }

  if (R_W == 1) { // para escribir
    valueToWrite[0] = 0x80 | adress;
  }

  if (R_W == 2) { // para escribir
    valueToWrite[0] = adress;
  }

  valueToWrite[1] = (unsigned char) ((data & 0xFF0000) >> 16);
  valueToWrite[2] = (unsigned char) ((data & 0x00FF00) >> 8);
  valueToWrite[3] = (unsigned char) ((data & 0x0000FF) >> 0);

  // Printo antes de enviar para conferir
  if (only_print_w)
    Serial.print("W[");

  for (int i = 0; i < num_bytes; i++) {
    if (valueToWrite[i] < 16)  {
      if (only_print_w)
        Serial.print("0");
    }
    if (only_print_w)
      Serial.print(valueToWrite[i], HEX);
    // Serial.print(" ");
  }

  if (only_print_w)
    Serial.print("] - R[");


  // Antes de enviar confiro se tem que fazer o Resync
  if (rs == 1) {
    SPI_resync(cs);
  }

  // Envio o reg dependendo do Adas Master = 0; ou Slave = 1
  SPI.beginTransaction(conf_tx);

  // Master
  if (cs == 0) {

    digitalWrite(CS_Master, LOW); // Habilita comunicação
    for (byte i = 0; i < num_bytes; i++) {
      // Escreve os 32 Bits
      SPI.transfer(valueToWrite[i]);
    }
    digitalWrite(CS_Master, HIGH); // disable clock again*/

  }

  // Slave
  if (cs == 1) {

    digitalWrite(CS_Slave, LOW); // Habilita comunicação
    for (byte i = 0; i < num_bytes; i++) {
      // Escreve os 32Bits
      SPI.transfer(valueToWrite[i]);
    }
    digitalWrite(CS_Slave, HIGH); // disable clock again*/

  }

  SPI.endTransaction();
}

/****************************************************************************/
/* Funcion de lectura SPI
*****************************************************************************/
void SPI_read(int adress, bool rs, int cs, bool only_print_r) {

  // Array de Bytes para recibir
  unsigned char readCmd[num_bytes] =  { 0, 0, 0, 0 };

  // Primeiro envio uma leitura
  SPI_write(Read, adress, 0, rs, cs, only_print_r);

  // Leio o reg dependendo do Adas Master = 0; ou Slave = 1
  SPI.beginTransaction(conf_rx);

  // Master
  if (cs == 0) {

    digitalWrite(CS_Master, LOW); // enable shifting
    for (int i = 0; i < num_bytes; i++) {
      readCmd[i] = SPI.transfer(0);
    }
    digitalWrite(CS_Master, HIGH); // disable clock again*/

  }

  // Slave
  if (cs == 1) {

    digitalWrite(CS_Slave, LOW); // enable shifting
    for (int i = 0; i < num_bytes; i++) {
      readCmd[i] = SPI.transfer(0);
    }
    digitalWrite(CS_Slave, HIGH); // disable clock again*/

  }

  SPI.endTransaction();

  /********* Imprime los valores recebidos *********/
  //String reg = "";
  nreg = 0;

  for (int i = 0; i < num_bytes; i++) {
    if (readCmd[i] < 16) {
      if (only_print_r)
        Serial.print("0");
    }
    if (only_print_r)
      Serial.print(readCmd[i], HEX);
    nreg +=  readCmd[i];
  }
  //Serial.print(nreg);
  if (only_print_r)
    Serial.print("]");
}

/****************************************************************************/
/* Funcion de lectura SPI escribiendo solo una vez
*****************************************************************************/
void SPI_read_only(int adress, bool rs, int cs, bool only_print_r) {

  // Array de Bytes para recibir
  unsigned char readCmd[num_bytes] =  { 0, 0, 0, 0 };

  // Leio o reg dependendo do Adas Master = 0; ou Slave = 1
  SPI.beginTransaction(conf_rx);

  // Master
  if (cs == 0) {

    digitalWrite(CS_Master, LOW); // enable shifting
    for (int i = 0; i < num_bytes; i++) {
      readCmd[i] = SPI.transfer(0);
    }
    digitalWrite(CS_Master, HIGH); // disable clock again*/

  }

  // Slave
  if (cs == 1) {

    digitalWrite(CS_Slave, LOW); // enable shifting
    for (int i = 0; i < num_bytes; i++) {
      readCmd[i] = SPI.transfer(0);
    }
    digitalWrite(CS_Slave, HIGH); // disable clock again*/

  }

  SPI.endTransaction();

  /********* Imprime los valores recebidos *********/
  //String reg = "";
  nreg = 0;
  nreg = readCmd[0];


  for (int i = 0; i < num_bytes; i++) {
    if (readCmd[i] < 16) {
      if (only_print_r)
        Serial.print("0");
    }
    if (only_print_r)
      Serial.print(readCmd[i], HEX);
  }
  //Serial.print(nreg);
  if (only_print_r)
    Serial.print(";");
}


/*****************************************************************/
// Funcion de resync
/*****************************************************************/
void SPI_resync(int cs) {
  SPI.beginTransaction(conf_tx); // Habilita e mantem a comunicação
  // Master
  if (cs == 0)  {
    delayMicroseconds(10);

    // Resync con 64 clocks
    for (int g = 0; g < 8; g++) {
      SPI.transfer(0xFF);
    }

  }

  // Slave
  if (cs == 1)  {
    // CS = HIGH para funcionar o resync
    delayMicroseconds(10);

    // Resync con 64 clocks
    for (int g = 0; g < 8; g++) {
      SPI.transfer(0xFF);
    }

    //SPI_write(Read, ADAS1000_ECGCTL, 0, Sem_resync, CS_1);
  }

  SPI.endTransaction();
}
