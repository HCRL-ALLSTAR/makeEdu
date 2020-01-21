#ifndef MQTT_WRAPPER_H
#define MQTT_WRAPPER_H

#include <Arduino.h>
#include <WiFiClient.h>
#include "SYSTEM/macros.hpp"
#include "PubSubClient/PubSubClient.h"
#include "PubSubClient/PubSubClient.cpp"

class mqttWrapper
{
private:
    WiFiClient client;
    PubSubClient mqtt;
    /* data */

    char *server;
    uint16_t port;
    char *userName;
    char *password;

    boolean isStart = false;
    String subscribeList[MAX_SIZE];
    String publishList[MAX_SIZE];

    void add2List(String *list, const char *topic);
    void printList(String *list);

public:
    mqttWrapper(/* args */);
    ~mqttWrapper();
    void begin(const char *server, uint16_t port, MQTT_CALLBACK_SIGNATURE);
    void update();
    void reconnect();
    void startSubscribe(const char *topic);
    void publish(const char *topic, const char *payload);
    void setUser(const char *username, const char *password);

    void printSubscribeTopic();
    void printpublishTopic();

    boolean getStatus();
    char *getServer();
    int getPort();
    char *getUsername();
};

mqttWrapper::mqttWrapper(/* args */)
{
    this->mqtt = PubSubClient(client);
}

mqttWrapper::~mqttWrapper()
{
}

void mqttWrapper::begin(const char *server, uint16_t port, MQTT_CALLBACK_SIGNATURE)
{
    this->server = (char *)server;
    this->port = port;
    this->mqtt.setServer(this->server, this->port);
    this->mqtt.setCallback(callback);
    this->isStart = true;
}
void mqttWrapper::update()
{
    if (this->isStart)
    {
        if (!this->mqtt.connected())
        {
            this->reconnect();
        }
        this->mqtt.loop();
    }
}
void mqttWrapper::reconnect()
{
    int Index = 0;
    while (!this->mqtt.connected())
    {
        Sprintln("Attempting MQTT connection...");
        String clientId = "ESP8266Client-";
        clientId += String(random(0xffff), HEX);

        if (int(this->userName) == 0 && int(this->password) == 0 ? this->mqtt.connect(clientId.c_str()) : this->mqtt.connect(clientId.c_str(), this->userName, this->password))
        {
            Sprintln("Connected");

            this->mqtt.publish("start", "Hello world");
            this->mqtt.subscribe("intopic");

            while (this->subscribeList[Index].length() != 0)
            {
                char buffer[512];
                this->subscribeList[Index].toCharArray(buffer, subscribeList[Index].length() + 1);
                this->mqtt.subscribe(buffer);
                Sprintln("Start Subscribe =>" + String(buffer));
                Index++;
            }
        }
        else
        {
            Sprint("failed, rc=");
            Sprint(this->mqtt.state());
            Sprintln();
            delay(100);
        }
    }
}
void mqttWrapper::startSubscribe(const char *topic)
{
    this->add2List(this->subscribeList, topic);
}
void mqttWrapper::publish(const char *topic, const char *payload)
{
    this->add2List(this->publishList, topic);
    if (!this->mqtt.connected())
    {
        this->reconnect();
    }
    this->mqtt.publish(topic, payload);
}
void mqttWrapper::setUser(const char *username, const char *password)
{
    this->userName = (char *)username;
    this->password = (char *)password;
}

void mqttWrapper::printSubscribeTopic()
{
    this->printList(this->subscribeList);
}
void mqttWrapper::printpublishTopic()
{
    this->printList(this->publishList);
}

boolean mqttWrapper::getStatus()
{
    return this->mqtt.connected();
}
char *mqttWrapper::getServer()
{
    return this->server;
}
int mqttWrapper::getPort()
{
    return this->port;
}
char *mqttWrapper::getUsername()
{
    // if (int(this->userName) == 0)
    // {
    //     return "Unknown";
    // }
    // else
    // {
    //     return this->userName;
    // }
    return int(this->userName) == 0 ? (char *)("Invalid Username") : this->userName;
}

void mqttWrapper::add2List(String *list, const char *topic)
{
    int Index = 0;
    boolean isAdded = false;
    while (!isAdded)
    {
        if (!list[Index].equals(topic))
        {
            if (list[Index].length() == 0)
            {
                list[Index] = topic;
                isAdded = !isAdded;
                Sprintln("Topic Added : " + String(list[Index]));
            }
            else
            {
                Index += 1;
            }
        }
        else
        {
            break;
        }
    }
}
void mqttWrapper::printList(String *list)
{
    int Index = 0;
    while (list[Index].length() != 0)
    {
        Sprintln(String(Index) + " : " + list[Index]);
        Index += 1;
    }
}

#endif