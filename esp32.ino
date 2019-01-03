#include "DHT.h"
#include <IOXhop_FirebaseESP32.h>
#if defined(ESP8266)
#include <ESP8266WiFi.h>  //ESP8266 Core WiFi Library         
#else
#include <WiFi.h>      //ESP32 Core WiFi Library    
#endif
 
#if defined(ESP8266)
#include <ESP8266WebServer.h> //Local WebServer used to serve the configuration portal
#else
#include <WebServer.h> //Local WebServer used to serve the configuration portal ( https://github.com/zhouhan0126/WebServer-esp32 )
#endif
 
#include <DNSServer.h> //Local DNS Server used for redirecting all requests to the configuration portal ( https://github.com/zhouhan0126/DNSServer---esp32 )
#include <WiFiManager.h>   // WiFi Configuration Magic ( https://github.com/zhouhan0126/WIFIMANAGER-ESP32 ) >> https://github.com/tzapu/WiFiManager (ORIGINAL)


const int PIN_AP = 2;
#define DHTTYPE DHT22
#define LED_BUILTIN 5
#define FIREBASE_HOST "esp32-8e66c.firebaseio.com/"
#define FIREBASE_AUTH "CDa7d3pp48liAdIM39NUp2MpdkOcAmw7sDGr0mX2"
DHT dht(4, DHTTYPE);
float h,t,f;
void setup() {
  Serial.begin(115200);
  pinMode(PIN_AP, INPUT);
  // put your setup code here, to run once:
  dht.begin();
  pinMode(LED_BUILTIN,OUTPUT);
  //declaração do objeto wifiManager
  WiFiManager wifiManager;
 
  //utilizando esse comando, as configurações são apagadas da memória
  //caso tiver salvo alguma rede para conectar automaticamente, ela é apagada.
//  wifiManager.resetSettings();
 
//callback para quando entra em modo de configuração AP
  wifiManager.setAPCallback(configModeCallback); 
//callback para quando se conecta em uma rede, ou seja, quando passa a trabalhar em modo estação
  wifiManager.setSaveConfigCallback(saveConfigCallback); 
 
//cria uma rede de nome ESP_AP com senha 12345678
  wifiManager.autoConnect("ESP_AP","12345678");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); 
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
  if(Firebase.getInt("State")==1){
  if ( digitalRead(PIN_AP) == HIGH ) 
   {
      WiFiManager wifiManager;
      if(!wifiManager.startConfigPortal("ESP_AP", "12345678") )
      {
        Serial.println("Falha ao conectar");
        delay(2000);
        ESP.restart();
      }
   }
   if (Firebase.failed()) {
      Serial.print("setting /number failed:");
      Serial.println(Firebase.error());  
      return;
   }
  h=dht.readHumidity();
  t=dht.readTemperature();
  f=dht.readTemperature(true);
    if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    digitalWrite(LED_BUILTIN,LOW);
    return;
  }else{
  Firebase.setFloat("Humidity",h);
  Firebase.setFloat("Temperature",t);
  Firebase.pushFloat("/LogHumidity",h);
  Firebase.pushFloat("/LogTemperature",t);
  Firebase.pushFloat("/LogFarenheit",f);
  digitalWrite(LED_BUILTIN,HIGH);
  }
  }else
  {
    Serial.println("Dispositivo calcelado!");
  }
}
//callback que indica que o ESP entrou no modo AP
void configModeCallback (WiFiManager *myWiFiManager) {  
//  Serial.println("Entered config mode");
  Serial.println("Entrou no modo de configuração");
  Serial.println(WiFi.softAPIP()); //imprime o IP do AP
  Serial.println(myWiFiManager->getConfigPortalSSID()); //imprime o SSID criado da rede
 
}
//callback que indica que salvamos uma nova rede para se conectar (modo estação)
void saveConfigCallback () {
  Serial.println("Configuração salva");
}
