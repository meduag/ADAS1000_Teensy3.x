#include "SPI.h"
#include "ADAS1000.h"

#define ini '<'
#define fin '>'

#define Con_Resync 1
#define Sem_Resync 0

#define Read 0
#define Write 1

const byte chipselect = 10;
const byte num_bytes = 4;

const char bnt1 = 1;
const char bnt2 = 1;

const int bt1 = 0;
const int bt2 = 4;
int t = 5;

boolean sal = false;

unsigned long tt = 10000000;;
unsigned long tempo_atual = 0;
unsigned long tempo_final = tt;


SPISettings conf_tx(2000000, MSBFIRST, SPI_MODE2);
SPISettings conf_rx(2000000, MSBFIRST, SPI_MODE3);

void setup() {
  pinMode(chipselect, OUTPUT);
  digitalWrite(chipselect, HIGH);

  pinMode(bt1, INPUT);
  pinMode(bt2, INPUT);

  SPI.begin();
  Serial.begin(2000000);
}

void loop() {
  if (digitalRead(bt1) == 0) {
    delay(300);
    Serial.println("Resync & Write");
    SPI_write(Write, ADAS1000_ECGCTL, 0x81F800BE, Con_Resync);
    delayMicroseconds(15);
    SPI_read(ADAS1000_ECGCTL, Sem_Resync, true);
  }

  if (digitalRead(bt2) == 0) {
    delay(300);
    Serial.println("1-Resync & 2-W & 3-Read");
    SPI_write(Write, ADAS1000_ECGCTL  , 0x81F800BE, Con_Resync);
    SPI_read(ADAS1000_ECGCTL, Sem_Resync, true);

    SPI_write(Write, ADAS1000_CMREFCTL , 0x85E0000A, Sem_Resync);
    SPI_read(ADAS1000_CMREFCTL, Sem_Resync, true);

    SPI_write(Write, ADAS1000_FRMCTL , 0x8A001181, Sem_Resync);
    SPI_read(ADAS1000_FRMCTL, Sem_Resync, true);
    Serial.println("\n");

    delayMicroseconds(1);

    // Sequencia para leitura >> Stream ECG
    SPI_read(ADAS1000_FRAMES, Sem_Resync, true);

    while (digitalRead(bt1) == 1) {
      SPI_read(0, Sem_Resync, false);
    }

    // Finaliza a comunicação SPI
    SPI.endTransaction();
    delay(300);
  }

  if (Serial.available()) {
    char data = Serial.read();
    Serial.readString(); // Tira o resto do buffer  - hace un flush forzado

    //Serial.print("DataIn: ");
    //Serial.println(data.charAt(0));

    // Btn1 -----*
    if (data == '1') {
      //delay(300);
      Serial.println("Resync & Write <");
      SPI_write(Write, ADAS1000_ECGCTL, 0x81F800BE, Con_Resync);
      delayMicroseconds(15);
      Serial.write('>');
      SPI_read(ADAS1000_ECGCTL, Sem_Resync, true);
      Serial.write('<');
      Serial.write('<');
      Serial.write('<');
    }

    // Btn2 -----*
    if (data == '2') {
      //delay(300);
      Serial.println("1-Resync & 2-W & 3-Read");
      SPI_write(Write, ADAS1000_ECGCTL  , 0x81F800BE, Con_Resync);
      SPI_read(ADAS1000_ECGCTL, Sem_Resync, true);

      SPI_write(Write, ADAS1000_CMREFCTL , 0x85E0000A, Sem_Resync);
      SPI_read(ADAS1000_CMREFCTL, Sem_Resync, true);

      SPI_write(Write, ADAS1000_FRMCTL , 0x8A001181, Sem_Resync);
      SPI_read(ADAS1000_FRMCTL, Sem_Resync, true);
      //Serial.println("-");

      delayMicroseconds(1);

      // Sequencia para leitura >> Stream ECG
      SPI_read(ADAS1000_FRAMES, Sem_Resync, true);

      tempo_atual = micros();
      tempo_final = tempo_atual + tempo_final;

      sal = false;
      while (sal == false) {
        // para sair por tempo
        if (micros() > tempo_final) {
          sal = true;
          tempo_final = tt;
        } else {
          Serial.write('>');
          SPI_read(0, Sem_Resync, false);
        }
      }
      Serial.write('<');
      Serial.write('<');
      Serial.write('<');
      sal = false;


      // Finaliza a comunicação SPI
      SPI.endTransaction();
      delay(300);
    }

    data = '0';

  }
}// end Loops


/****************************************************************************/
/* Funcion de escritura SPI
*****************************************************************************/
void SPI_write(bool R_W, int adress, unsigned long data, bool rs) {

  unsigned char valueToWrite[num_bytes] = {0, 0, 0, 0};

  if (R_W == 1) {
    valueToWrite[0] = 0x80 | adress;  // Write bit and register address.
  } else {
    valueToWrite[0] = 0x7F & adress;
  }

  valueToWrite[1] = (unsigned char)((data & 0xFF0000) >> 16);
  valueToWrite[2] = (unsigned char)((data & 0x00FF00) >> 8);
  valueToWrite[3] = (unsigned char)((data & 0x0000FF) >> 0);

  /*Serial.print("W[");
    for (int i = 0; i < num_bytes; i++) {
    if (valueToWrite[i] < 16) {
      Serial.print("0");
    }
    Serial.print(valueToWrite[i], HEX);
    Serial.print(" ");
    }
    Serial.print("]<");*/

  if (rs == 1) {
    SPI_resync();
  }

  digitalWrite(chipselect, LOW); // enable CS
  
  SPI.beginTransaction(conf_tx);
  for (byte i = 0; i < num_bytes; i++) {
    SPI.transfer(valueToWrite[i]);
  }

  digitalWrite(chipselect, HIGH); // disable CS
  SPI.endTransaction();

}

/****************************************************************************/
/* Funcion de lectura SPI
*****************************************************************************/
void SPI_read(int adress, bool rs, boolean b1b2) {

  // Valores por default
  unsigned char readCmd[num_bytes] = {0, 0, 0, 0};

  if (b1b2 == true) {
    SPI_write(Read, adress, 0, rs);
  }

  SPI.beginTransaction(conf_rx);
  
  digitalWrite(chipselect, LOW); // enable CS
  // Escribo 0 e leio o valor de resposta
  for (int i = 0; i < num_bytes; i++) {
    readCmd[i] = SPI.transfer(0);
  }
  digitalWrite(chipselect, HIGH); // disable CS
  
  SPI.endTransaction();

  //Serial.print("R[");
  for (int i = 0; i < num_bytes; i++) {
    if (readCmd[i] < 16) {
      Serial.print("0");
    }
    Serial.print(readCmd[i], HEX);
    //Serial.print(" ");
  }
  //Serial.write("]<");
  Serial.println();
}

/****************************************************************************/
/* Funcion de lectura SPI
*****************************************************************************/
void SPI_resync() {
  // Primeiro só manda os 64 ciclos de clock
  SPI.beginTransaction(conf_tx);

  delayMicroseconds(10);

  // Transmite os 64 ciclos de clock
  for (int g = 0; g  < 8; g++) {
    SPI.transfer(0xFF);
  }

  // Finaliza a sincronização com um pedido d leitura
  SPI.transfer(0x01);
  digitalWrite(chipselect, LOW); // enable shifting
  digitalWrite(chipselect, HIGH); // enable shifting
  SPI.endTransaction();
}

int c_timer() {
  int v_sal = 0;
  if (tempo_final < micros()) {
    v_sal = 1;
    tempo_final = 0;
  }
  return v_sal;
}
