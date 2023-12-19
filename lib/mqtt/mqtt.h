#ifndef _CEK_WIFICLIENT_H_
#define _CEK_WIFICLIENT_H_

#include <WiFi.h>
#include "PubSubClient.h"


void receivedCallback(char* topic, byte* payload, unsigned int length);

class MqttClient {
public:
    MqttClient();
    void setClient(Client& espClient);
    void mqttconnect(); 
    void loop();

    bool publish(const char* topic, float payload);
private:
    void initMqtt();
private:
    PubSubClient client;

    const char* mqtt_server = "broker.hivemq.com";
};

#endif