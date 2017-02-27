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
//incluir biblioteca DHT
#include <DHT.h>
//incluir biblioteca BH1750
#include <BH1750.h>
//incluir biblioteca studio
#include <stdio.h>
//incluir biblioteca stdlib
#include <stdlib.h>
//incluir biblioteca string
#include <string.h>

//Define a ligação ao pino de dados do sensor DHT11
#define DHTPIN A3
//Define o tipo de sensor DHT utilizado
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);
//Define as ligações aos pinos de dados do sensor BH1750
#define SCL A5
#define SDA A4
//Tipo de sensor de Luminosidade utilizado
BH1750 lightMeter; 

//nRF24L01(+) rádio conectado a placa
RF24 radio(7,8);

//Rede utilizada neste rádio
RF24Network network(radio);

//variável usada para receber os dados da porta serial
String val;         

//Endereço deste nó
uint16_t this_node = 00;

//Endereço do outro nó
uint16_t other_node = 01;

//Frequência para enviar os dados para a outra unidade
const unsigned long interval = 1000; //ms

//
unsigned long last_sent;

//Status de inicialização do nó
unsigned int node_status = 9;

//Valor da variável na inicialização
bool is_sensor_controlled = false;

//Estrutura do pacote
struct payload_t {
  unsigned int source_id;
  unsigned int cmd;
};

void setup(void)
{
  //Início da comunicação serial
  Serial.begin(9600);
  //Inicialização do sensor BH1750
  lightMeter.begin(); 
  //inicialização do sensor DHT11
  dht.begin();
  //Imprime na tela na inicialização
  Serial.println("RF24Network/examples/helloworld_tx/");
  //Inicialização do protocolo SPI
  SPI.begin();
  //Inicialização do rádio
  radio.begin();
  //Inicialização da rede e definição do canal e do endereço do nó
  network.begin(/*channel*/ 90, /*node address*/ this_node);
}

void loop(void)
{
  //Lê o valor do sensor DHT11 e o armazena na variável t
  int t = (int)dht.readTemperature();
  //Imprime na tela o valor do sensor
  Serial.print("temp");
  Serial.println(t);

  //Lê o valor do sensor BH1750 e o armazena na variável lux
  int lux = lightMeter.readLightLevel();
  //Imprime na tela o valor do sensor
  Serial.print("Lux: ");
  Serial.println(lux);

  //Se a porta serial estiver disponível leia o valor dela e armazene-o na variável val
  if( Serial.available() )
  {;}
  val = Serial.readString();
  //Imprime na tela o valor da variável
  Serial.print("val: ");
  Serial.println(val);
  //Atualiza a rede regularmente
  network.update();

  unsigned long now = millis();
  if ( now - last_sent >= interval  )
  {
    last_sent = now;
    //Se os dois primeiros elementos da val são 1 e 5 a variável is_sensor_controlled vira verdadeira
    if (val[0] == '1' && val[1] == '5') {
      is_sensor_controlled = true;
    //Se os dois primeiros elementos da val são 1 e 6 a variável is_sensor_controlled vira falsa
    } else if(val[0] == '1' && val[1] == '6'){
      is_sensor_controlled = false;
    }
    //Se is_sensor_controlled for verdadeira o comando será enviado para o nó 01
    if (is_sensor_controlled) {      
      other_node = 01;
      //Se o valor da variável lux for menor que 50 envie o status 2 para o nó 01
      if (lux < 50){
        node_status = 2;
      //Senão envie o status 2 
      } else {
        node_status = 3;
      }
      //Se o valor da variável t for maior que 26 envie o status 1 para o nó 01
      if (t > 26){
        node_status = 1;
      //Senão envie o status 0
      } else {
        node_status = 0;
      }
      //Imprima na tela "[SENSOR]"
      Serial.print("[SENSOR] ");
    }
    //Imprima na tela o nó e o status enviados
    Serial.print("Sending...");
    Serial.print(node_status);
    Serial.print(" to ");
    Serial.print(other_node);
    Serial.print("...");
    //Se o primeiro elemento da variável val for 1 envie os comandos para o nó 1
    if (val[0] == '1') {
      other_node = 01;
      //Se o segundo elemento da variável val for 1 envie  status 1 para o nó 01
      if (val[1] == '1'){
        node_status = 1;
      //Se o segundo elemento da variável val for 0 envie  status 0 para o nó 01
      }else if (val[1] == '0'){
        node_status = 0;
      }
    }
    //Se o primeiro elemento da variável val for 1 envie os comandos para o nó 01
    if (val[0] == '1') {
      other_node = 01;
      //Se o segundo elemento da variável val for 2 envie  status 2 para o nó 01
      if (val[1] == '2'){
        node_status = 2;
       //Se o segundo elemento da variável val for 3 envie  status 3 para o nó 01
      }else if (val[1] == '3'){
        node_status = 3;
      }
    }
    //Imprime na tela "step 5"
    Serial.println("step 5");
    //Monta o pacote
    payload_t payload = { this_node, node_status };
    //Cabeçalho do pacote
    RF24NetworkHeader header(/*to node*/ other_node);
    //envia o pacote 
    bool ok = network.write(header,&payload,sizeof(payload));
    //Se o status do nó for menor que 9 imprime "ok"
    if (node_status < 9)
      Serial.println("ok.");
    //Senão imprime "failed"
    else
      Serial.println("failed.");
  }
  }
