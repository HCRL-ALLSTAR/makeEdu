#ifndef MQTT_TASK_H
#define MQTT_TASK_H

#include <Arduino.h>
#include <WiFiClient.h>
#include "MqttWrapper/PubSubClient/PubSubClient.h"
#include "System/SystemDefaults.hpp"
#include "System/SystemMacros.hpp"
#include "MqttWrapper/MqttWrapper.h"

class MqttTask
{
private:
    MqttWrapper wrapper;
    TaskHandle_t updateHandle;

    boolean isNewTopic = false;

    static void updateCode(void *);
    void update();
    char *username;
    char *password;

public:
    MqttTask(/* args */);
    ~MqttTask();
    void begin(const char *Server, int Port, MQTT_CALLBACK_SIGNATURE);
    void startSubscribe(const char *Topic);
    void publish(const char *Topic, const char *Payload);
    void setUser(const char *username, const char *password);

    void printSubscribeTopic();
    void printpublishTopic();
    boolean getStatus();
    char *getServer();
    int getPort();
    char *getUsername();
    char *getPassword();
};

MqttTask::MqttTask(/* args */)
{
}

MqttTask::~MqttTask()
{
}

/*
    begin Mqtt Connection
    Server  -> Mqtt Server Address
    Port    -> Mqtt Server Port 
    MQTT_CALLBACK_SIGNATURE -> Callback Function in form 
    Example :
    void callback(char *Topic, byte *Paylaod, unsigned int Length)
    {
        Paylaod[Length] = '\0';
        String topic_str = Topic, payload_str = (char *)Paylaod;
        Serial.println("[" + topic_str + "]: " + payload_str);
    }
*/
void MqttTask::begin(const char *Server, int Port, MQTT_CALLBACK_SIGNATURE)
{
    this->wrapper.Begin(Server, Port, callback);
    this->update();
}

void MqttTask::update()
{
    xTaskCreate(updateCode, "MQTT UPDATE TASK", Default_Task_Stack, this, 1, &updateHandle);
}

void MqttTask::updateCode(void *pv)
{
    MqttTask *task = (MqttTask *)(pv);
    for (;;)
    {
        if (task->isNewTopic || task->wrapper.SubscribeTopic[0].length() == 0)
        {
            //vTaskDelete(task->updateHandle);
            task->wrapper.Update();
            task->isNewTopic = false;
        }
        if (task->wrapper.isConnected() && !task->isNewTopic)
        {
            task->wrapper.Update();
        }
        TaskDelay(delay_Time);
    }
}
/*
    *** Please Add After begin Connection ***
    Start Subscribe to Your Topic 
*/
void MqttTask::startSubscribe(const char *Topic)
{
    this->wrapper.StartSubscribe(Topic);
    this->isNewTopic = true;
}

/*  
    Start publish To Topic 
*/
void MqttTask::publish(const char *Topic, const char *Payload)
{
    this->wrapper.Publish(Topic, Payload);
}

/*
    Set Your username And password if you need
    username -> username in Mqtt Broker
    password -> password in Mqtt Broker
*/
void MqttTask::setUser(const char *username, const char *password)
{
    this->username = (char *)username;
    this->password = (char *)password;
    this->wrapper.SetUser(this->username, this->password);
}

/*
    Print All Subscribe Topic
*/
void MqttTask::printSubscribeTopic()
{
    this->wrapper.PrintSubscribeTopic();
}
/*
    Print All publish Topic 
*/
void MqttTask::printpublishTopic()
{
    this->wrapper.PrintPublishTopic();
}

/*
    Get Current Mqtt Connection Status
*/
boolean MqttTask::getStatus()
{
    return this->wrapper.isConnected();
}

/*
    Get Mqtt Server Address
*/
char *MqttTask::getServer()
{
    return this->wrapper.GetServer();
}

/*
    Get Mqtt Server Port
*/
int MqttTask::getPort()
{
    return this->wrapper.GetPort();
}
char *MqttTask::getUsername()
{
    return this->username;
}
char *MqttTask::getPassword()
{
    return this->password;
}

#endif
