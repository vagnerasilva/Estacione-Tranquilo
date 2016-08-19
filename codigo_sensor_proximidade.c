/*
  Hardware Livre USP - Mastertech
  Enviando mensagens por um servidor
*/
#include <SPI.h>
#include <Ethernet.h>

const int outPin = 2; 
const int inPin = 3;

// Utilize o endereço MAC que está na etiqueta branca da
// sua Galielo
byte mac[] = { 0x98, 0x4F, 0xEE, 0x05, 0x5F, 0x89 };

// IP do servidor e porta do servidor.
//IPAddress server(192,168,1,32);
char server[] = "0.tcp.ngrok.io";
int port = 15003;

EthernetClient client;
String line = "";

// Marcadores da string line
int lendo_valor = false;

// Variaveis para delay de envio de informacoes
const long interval = 0;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  pinMode(outPin, OUTPUT_FAST);
  pinMode(inPin, INPUT_FAST);
  system("ifup eth0");
  Serial.println("Tentando obter um IP:");
  while (!Ethernet.begin(mac)) {
    Serial.println("Erro ao conectar");
  }
  Serial.print("Meu endereco:");
  Serial.println(Ethernet.localIP());

  // Reserva espaço para a string
  line.reserve(10);
}

void loop() {

 long duration, inches, cm;

  /*Ativa o trig com um pulso em alto*/
  fastDigitalWrite(outPin, LOW);
  waitMicros(2);
  fastDigitalWrite(outPin, HIGH);
  waitMicros(10);
  fastDigitalWrite(outPin, LOW);

  /*espera por um sinal alto e guarda o tempo que demorou*/
  duration = pulseIn(inPin, HIGH); // calls fastGpioPciDigitalRead

  /*converte o tempo para centimetros*/
  cm = microsecondsToCentimeters(duration);
  

  Serial.print(cm);
  Serial.print("cm");
  Serial.println();
  
  delay(100);

  
  // Realiza a conexao e leitura do servidor
  if (client.connected()) {
    if (client.available()) {
      char c = client.read();
      line += c;
      if (c == '\n')
        line = "";

      if (line.endsWith("{")) {
        lendo_valor = true;
        line = "";
      }
      if (lendo_valor) {
        if (c == '}') {
          int valor = cm;
          lendo_valor = false;
          Serial.print("Recebendo: ");
          Serial.println(valor);
        }
      }
    }
  } else {
    Serial.println("Conectando ao servidor");
    delay(1000);
    client.connect(server, port);
  }

  // Realiza o envio de informacoes
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (client.connected()) {
      int value = cm;
      client.println(value);
      Serial.print("Enviando: ");
      Serial.println(value);
    }
  }
}

void waitMicros(int val)
{
  unsigned long a = micros();
  unsigned long b = micros();
  while((b-a) < val)
  {
    b = micros();
    if(a>b)
    {
      break;
    }
  }
}

long microsecondsToInches(long microseconds)
{
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds)
{
  return microseconds / 29 / 2;
}