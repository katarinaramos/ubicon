//---------------------------------------------------------------------------------------------
//Programa do módulo central para receber comandos dos usuários e enviá-los aos módulos remotos
//Autora: Katarina Ramos
//---------------------------------------------------------------------------------------------
//incluir biblioteca RF24Network
#include <RF24Network.h>
//incluir biblioteca RF24
#include <RF24.h>
//incluir biblioteca SPI
#include <SPI.h>

//Porta ligada ao pino IN1  modulo
int porta_rele1 = 2;
//Porta ligada ao pino IN2  modulo
int porta_rele2 = 3; 

//nRF24L01(+) rádio conectado a placa
RF24 radio(7,8);           

//Rede utilizada neste rádio     
RF24Network network(radio);      
//Endereço deste nó
const uint16_t this_node = 01;    
//Endereço do outro nó
const uint16_t other_node = 00;        
//Estrutura do pacote
struct payload_t {                 
  unsigned int source_id;
  unsigned int cmd;
};
//Inicialização da variável ultimo_comando
unsigned int ultimo_comando = 9 ;

void setup(void)
{
  //Início da comunicação serial
  Serial.begin(9600);
  //Imprime na tela na inicialização
  Serial.println("RF24Network/examples/helloworld_rx/");
 
  //Define pinos para o relé como saída 
  pinMode(porta_rele1, OUTPUT); 
  pinMode(porta_rele2, OUTPUT);
  //Inicia as portas do relé HIGH
  digitalWrite(porta_rele1, HIGH);
  digitalWrite(porta_rele2, HIGH);
  //Inicialização do protocolo SPI
  SPI.begin();
  //Inicialização do rádio
  radio.begin();
  //Inicialização da rede e definição do canal e do endereço do nó
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop(void){
  //Atualiza a rede regularmente
  network.update();                  
  //Enquanto a rede estiver disponível leia os pacotes enviados
  while ( network.available() ) {     
    RF24NetworkHeader header;        
    payload_t payload;
    network.read(header,&payload,sizeof(payload));
    //Se o ultimo_comando for diferente do payload.cmd execute os comandos
    if ( ultimo_comando != payload.cmd){
      //Imprime na tela
      Serial.print("Received from ");
      Serial.print(payload.source_id);
      Serial.print(" cmd ");
      Serial.println(payload.cmd);
      //Se o payload.cmd for igual a 1 ligue a porta do relé
      if ( payload.cmd == 1){
        digitalWrite(porta_rele1, LOW);
       //Se o payload.cmd for igual a 0 desligue a porta do relé
      } else if (payload.cmd == 0){
        digitalWrite(porta_rele1, HIGH);
      }
      //Se o payload.cmd for igual a 2 ligue a porta do relé
      if( payload.cmd == 2){
        digitalWrite(porta_rele2, LOW);
        //Se o payload.cmd for igual a 3 desligue a porta do relé
      } else if (payload.cmd == 3){
        digitalWrite(porta_rele2, HIGH);
      }
    } 
    ultimo_comando = payload.cmd;  
  }
}
