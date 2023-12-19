#include "mqtt.h"


MqttClient::MqttClient()
{
  
}

void MqttClient::setClient(Client &espClient)
{
  client.setClient(espClient);
  initMqtt();
}

void MqttClient::mqttconnect()
{
  /* Loop until reconnected */
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client_ev!";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      /* subscribe topic with default QoS 0*/
      //client.subscribe(LED_TOPIC);
    } else {
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      /* Wait 5 seconds before retrying */
      delay(5000);
    }
  }
}

void MqttClient::loop()
{
  client.loop();
}

//1 char for the sign, 1 char for the decimal dot, 4 chars for the value & 1 char for null termination
constexpr size_t BUFFER_SIZE = 7;
bool MqttClient::publish(const char *topic, float payload)
{
  char buffer[BUFFER_SIZE]; 
  dtostrf(payload, BUFFER_SIZE - 1 /*width, including the decimal dot and minus sign*/, 2 /*precision*/, buffer);
  return client.publish(topic, buffer, BUFFER_SIZE);
}

void receivedCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message received: ");
  Serial.println(topic);
 
  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void MqttClient::initMqtt()
{
  //
   /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);
}
