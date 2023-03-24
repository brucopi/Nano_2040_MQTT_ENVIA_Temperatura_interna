
//##################MQTT envia 

#include <MQTT.h>
#include <MQTTClient.h>
#include <WiFiNINA.h>

#include <Arduino_LSM6DSOX.h>

char ssid[] = "";        // your network SSID (name)
char pass[] = "";
int status = WL_IDLE_STATUS;     // the Wifi radio's status

WiFiClient wifiClient;
MQTTClient mqttClient;  

#define BROKER_IP    "mqtt.eclipseprojects.io"
#define DEV_NAME     "bruno"
#define MQTT_USER    "bruno"
#define MQTT_PW      "bruno"
#define TOPIC_SUBSCRIBE "FIT_bruno"   //Informe um Tópico único. Caso sejam usados tópicos em duplicidade, o último irá eliminar o anterior.

unsigned long lastMillis = 0;

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
  conectawifi();

  // MQTT brokers usually use port 8883 for secure connections.
  mqttClient.begin(BROKER_IP, 1883, wifiClient);
  conectamqtt();
  //inicializa o sensor de temperatura da placa
if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
  
}

void loop() {
  // put your main code here, to run repeatedly:
 mqttClient.loop();
    if (!mqttClient.connected()) {
      conectawifi();
      conectamqtt();
     
      printData();
      
    } 
     if (millis() - lastMillis > 5000) {
    lastMillis = millis();
    enviaValores();
     }
}


 void conectawifi()
  {
     // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to network: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network:
      status = WiFi.begin(ssid, pass);
  
      // wait 10 seconds for connection:
      delay(15000);
    }
  
    // you're connected now, so print out the data:
    Serial.println("You're connected to the network");
    
    Serial.println("----------------------------------------");
    printData();
    Serial.println("----------------------------------------");
  }

  void printData() {
    Serial.println("Board Information:");
    // print your board's IP address:
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);
  
    Serial.println();
    Serial.println("Network Information:");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
  
    // print the received signal strength:
    long rssi = WiFi.RSSI();
    Serial.print("signal strength (RSSI):");
    Serial.println(rssi);
  
    byte encryption = WiFi.encryptionType();
    Serial.print("Encryption Type:");
    Serial.println(encryption, HEX);
    Serial.println();
  }

 void conectamqtt()
  {
    Serial.print("\nconnectando MQTT...");
    while (!mqttClient.connect(DEV_NAME));//, MQTT_USER, MQTT_PW)) 
    {
      Serial.print(".");
      delay(1000);
    }
    Serial.println("\nMQTT connected!");
    Serial.print("BROKER_IP=");
    Serial.println(BROKER_IP);
    Serial.print("DEV_NAM=");
    Serial.println(DEV_NAME);
    Serial.print("MQTT_USER=");
    Serial.println(MQTT_USER);
    Serial.print("MQTT_PW=");
    Serial.println(MQTT_PW);
    
  }


     void enviaValores()
     {
       if (IMU.temperatureAvailable())
        {
          int temperature_deg = 0;
          IMU.readTemperature(temperature_deg);

          Serial.print("LSM6DSOX Temperature = ");
          Serial.print(temperature_deg);
          Serial.println(" °C");
            mqttClient.publish(TOPIC_SUBSCRIBE,String(temperature_deg));
        }
     }

