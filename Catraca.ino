/*
  [+] CATRACA
  [+] CRIADO: JONATAN GOMES
  [+] DATA:   21/11/2019
  
*/

// CHAMA AS BIBLIOTECAS
#include <SPI.h>
#include <MFRC522.h>
#include <Ethernet.h>
 
#define SS_PIN 6
#define RST_PIN 9
#define ACCESS_DELAY 3000
#define DENIED_DELAY 2000
#define PINRELE 2 // RELÉ PARA LIBERAÇÃO DE CATRACA
#define LED_R 4
#define LED_G 3
#define BUZZER 5

MFRC522 mfrc522(SS_PIN, RST_PIN);   // CRIA A UMA INSTANCIA DE CONEXAO

String content= "";
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED}; // MAC DA PLACA
IPAddress ip(); // ENDEREÇO IP DA PLACA
//char server[] = ""; // ENDEREÇO IP DO SERVIDOR
IPAddress server();

EthernetClient client; // INICIA O MODULO ETHERNET COMO CLIENTE

void setup() 
{
  digitalWrite(PINRELE, HIGH); // MANTEM O RELE LIGADO
  Serial.begin(9600);   // INICIALIZA A COMUNIÇÃO
  SPI.begin();          // INICIALIZA O PROTOCOLO
  mfrc522.PCD_Init();   // INICIA O MODULO MFR552

  Ethernet.begin(mac, ip);
  Serial.print("IP:  ");
  Serial.println(Ethernet.localIP()); // EXIBE O IP DA PLACA

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(PINRELE, OUTPUT);
}
void loop() 
{
    // VERIFICAÇÃO DE CARTOES
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // SELECIONA O CARTAO
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  // MONITORA O ID DE CARTÕES
 // Serial.print("UID tag :");
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     
     
     // ADICIONA VALORES DO CARTAO NA VARIAVEL CONTENT
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
     content.toUpperCase(); // MANTEM LETRAS MAIUSCULAS
     content.trim(); // REMOVE OS ESPAÇOS
}

  if (client.connect(server, 80)) 
    {
        delay(100);
        
        Serial.println(); 
        Serial.println("CONECTADO AO SERVIDOR");
        client.print("GET placa1/index.php?tag=" + (content) + "&log=placa2"); // ENVIA A INFORMAÇÃO PARA O SERVIDOR
        client.println(" HTTP/1.1"); 
        client.println("Host: "); 
        client.println("Connection: close"); 
        client.println(); 
        String c = client.readStringUntil('&');   // FAZ A LEITURA DOS DADOS DO SERVER 
        client.stop();
        Serial.println(content);
        Serial.println(c);
        
        // FAZ VERIFICAÇÃO SE O USUARIO ESTÁ LIBERADO
        if(c.indexOf("ok") != -1){
          libera();
        }else{
          bloqueia();  
        }
    }else{ // CASO NAO CONECTA NO SERVIDOR FECHA CONEXAO
      Serial.println();
      Serial.print("CONEXAO FECHADA");
    }
    content = ""; // ZERA A VARIAVEL PARA NOVA VERIFICAÇÃO
}

// REALIZA LIBERAÇÃO DO RELE
void libera(){
  Serial.println();  
  Serial.println("LIBERADO");
  digitalWrite(PINRELE, LOW); // LIBERA A CATRACA CORTANDO A ENERGIA
  digitalWrite(LED_G, HIGH);
  tone(BUZZER, 495);
  delay(500);
  noTone(BUZZER);
  delay(500);
  tone(BUZZER, 528);
  noTone(BUZZER);
  delay(ACCESS_DELAY);
  digitalWrite(PINRELE, HIGH); 
  digitalWrite(LED_G, LOW);
}

// BLOQUEIA A CATRACA
void bloqueia(){
  Serial.println();
  Serial.println("BLOQUEADO");   
  digitalWrite(LED_R, HIGH); 
  tone(BUZZER, 495);
  delay(DENIED_DELAY);
  noTone(BUZZER);
  digitalWrite(LED_R, LOW);
}
