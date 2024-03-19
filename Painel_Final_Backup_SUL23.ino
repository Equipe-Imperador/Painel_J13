/*

  Painel - Arquivo únificado para recebimento de dados pela CAN, e envio dos mesmos pela Serial para o display.
  Tratamento de Velocidade e RPM recebidos.
  Criado em 20/10/2020 por Rossi para implementação no J12 até novembro de 2023 e servir como base para o J13.  

*/

#include <CAN.h>

#define TX_GPIO_NUM   5  // Conecta no CTX
#define RX_GPIO_NUM   18  // Conecta no CRX


#define RXPIN 16
#define TXPIN 17
#define EA1 26

char dataRecebida [9];

String rpm_string = "";
int RPM;

String velocidade_string = "";
int VEL;

String temp_string = "";
int temperatura = 0;

String bat_string = "";
float tensao_bateria = 0.0;

String s_arq = "";
int n_arquivo = 0;

String s_hora = "";
int hora = 0;

String s_minuto = "":
int minuto = 0;

char freio_char;
bool freio;
int freio_i = 0;

char box_char;
bool box = false;
int BOX = 0;


long timer;
long timer_CAN;


void setup() {
//
//
  Serial.begin(115200);
  Serial.println("Comecando: ");

  pinMode(RXPIN, INPUT);
  pinMode(TXPIN, OUTPUT);

  Serial1.begin(115200, SERIAL_8N1, RXPIN, TXPIN);
//
//
  CAN.setPins (RX_GPIO_NUM, TX_GPIO_NUM);

  if (!CAN.begin (500E3)) {
    Serial.println ("Inicialização da CAN falhou.");
    while (1);
  }
  else {
    Serial.println ("CAN Iniciada");
  }

//
//
  delay(500);

}

void loop() {
  

  Envio_para_display();
  canReceiver();


}

void Envio_para_display(){
    if(millis()-timer>100)
  {
  

    sendIntRPM(RPM);
    sendIntVEL(VEL);
    sendIntTEMP(temperatura);
    sendFloatTENSAO(tensao_bateria);
    sendIntFreio(freio_i);
    sendIntBOX(BOX);


    timer = millis();
  }
}

void sendIntRPM(int numberToSend){

  Serial1.write(0x5A); 
  Serial1.write(0xA5); 
  Serial1.write(2+1+2); 
  Serial1.write(0x82);
  Serial1.write(0x10);
  Serial1.write((byte)0x00); 
  Serial1.write(highByte(numberToSend)); 
  Serial1.write(lowByte(numberToSend)); 
  

}

void sendIntVEL(int numberToSend){

  Serial1.write(0x5A); 
  Serial1.write(0xA5); 
  Serial1.write(2+1+2); 
  Serial1.write(0x82);
  Serial1.write(0x11);
  Serial1.write((byte)0x00); 
  Serial1.write(highByte(numberToSend)); 
  Serial1.write(lowByte(numberToSend)); 
  

}

void sendIntTEMP(int numberToSend){

  Serial1.write(0x5A); 
  Serial1.write(0xA5); 
  Serial1.write(2+1+2); 
  Serial1.write(0x82);
  Serial1.write(0x12);
  Serial1.write((byte)0x00); 
  Serial1.write(highByte(numberToSend)); 
  Serial1.write(lowByte(numberToSend)); 
  

}

void sendIntFreio(int numberToSend){

  Serial1.write(0x5A); 
  Serial1.write(0xA5); 
  Serial1.write(2+1+2); 
  Serial1.write(0x82);
  Serial1.write(0x14);
  Serial1.write((byte)0x00); 
  Serial1.write(highByte(numberToSend)); 
  Serial1.write(lowByte(numberToSend)); 
  

}

void sendIntBOX(int numberToSend){

  Serial1.write(0x5A); 
  Serial1.write(0xA5); 
  Serial1.write(2+1+2); 
  Serial1.write(0x82);
  Serial1.write(0x15);
  Serial1.write((byte)0x00); 
  Serial1.write(highByte(numberToSend)); 
  Serial1.write(lowByte(numberToSend)); 
  

}

void sendFloatTENSAO(float Value){

  Serial1.write(0x5A); 
  Serial1.write(0xA5); 
  Serial1.write(0X07); 
  Serial1.write(0x82);
  Serial1.write(0x13);
  Serial1.write((byte)0x00); 

  byte hex [4] = {0};

  FloatToHex(Value, hex);

  Serial1.write(hex[3]); 
  Serial1.write(hex[2]); 
  Serial1.write(hex[1]); 
  Serial1.write(hex[0]);

}

void FloatToHex(float f, byte*hex)
{
  byte* f_byte = reinterpret_cast<byte*>(&f);
  memcpy(hex, f_byte,4);
}


void canReceiver() {

  int i;
  int packetSize = CAN.parsePacket();

  
  if (packetSize) {

    timer_CAN = millis();

    Serial.print ("Received ");
  
    if (CAN.packetExtended()) {
      Serial.print ("extended ");
    }

    if (CAN.packetRtr()) {

      Serial.print ("RTR ");
    }

    Serial.print ("packet with id 0x");
    Serial.print (CAN.packetId(), HEX);

    if (CAN.packetRtr()) {
      Serial.print (" and requested length ");
      Serial.println (CAN.packetDlc());
    } else {
      Serial.print (" and length ");
      Serial.println (packetSize);


      while (CAN.available()) {
        dataRecebida[i] = ((char)CAN.read());
        Serial.print ("CAN: ");
        Serial.println(dataRecebida[i]);
        i++;
      }
      i = 0;

      //Formato: R0000V00
      if (dataRecebida[0] == 'R'){
        rpm_string = "";
        rpm_string.concat(dataRecebida[1]);
        rpm_string.concat(dataRecebida[2]);
        rpm_string.concat(dataRecebida[3]);
        rpm_string.concat(dataRecebida[4]);

        RPM = rpm_string.toInt();

        velocidade_string = "";

        velocidade_string.concat(dataRecebida[5]);
        velocidade_string.concat(dataRecebida[6]);

        VEL = velocidade_string.toInt();

        freio_char = dataRecebida[7];

        freio_char == '1' ? freio = true : freio = false;
        if(freio==true){
          freio_i = 1;
        }
        else{
          freio_i = 0;
        }

        Serial.print("Freio send: ");
        Serial.println(freio_i);

      }
      //Formato: C000111X (Criticos ; Bateria 3 digitos ; Temperatura 3 digitos ; Chamada BOX 1 digito )
      else if(dataRecebida[0] == 'C')
      {
        bat_string = "";
        bat_string.concat(dataRecebida[1]);
        bat_string.concat(dataRecebida[2]);
        bat_string.concat(dataRecebida[3]);

        tensao_bateria = (bat_string.toInt()/10.0);

        temp_string = "";
        temp_string.concat(dataRecebida[4]);
        temp_string.concat(dataRecebida[5]);
        temp_string.concat(dataRecebida[6]);

        temperatura = temp_string.toInt();

        box_char = dataRecebida[7];

        box_char == '1' ? box = true : box = false;
        if(box==true){
          BOX = 1;
        }
        else{
          BOX = 0;
        }

        Serial.print("BOX send: ");
        Serial.println(BOX);

        Serial.print("Tensão: ");
        Serial.println(tensao_bateria);
        Serial.println(bat_string);
        Serial.print("Temperatura: ");
        Serial.println(temperatura);
        Serial.println(temp_string);

      }
      else if(dataRecebida[0] == 'S'){
        s_arq = "":
        s_arq.concat(dataRecebida[1]);
        s_arq.concat(dataRecebida[2]);
        n_arquivo = s_arq.toInt();

        s_hora = "":
        s_hora.concat(dataRecebida[3]);
        s_hora.concat(dataRecebida[4]);
        hora = s_hora.toInt();

        s_minuto = "";
        s_minuto.concat(dataRecebida[5]);
        s_minuto.concat(dataRecebida[6]); 
        minuto = s_minuto.toInt();
      }
      Serial.println();
    }
    Serial.println();
  }

  else{

    if(millis() - timer_CAN>=500){
      temperatura = 500;
      tensao_bateria = 50.0;
    }
  }

}

