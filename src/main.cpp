#include <Arduino.h>
#include "HCRL_EDU.h"

void SubLight(byte *payload, unsigned int length, uint8_t *lightStatus, uint16_t *, uint8_t lightIndex);
void SubAir(byte *payload, unsigned int length);
void SubFan(byte *payload, unsigned int length);
void SUbStrip(byte *payload, unsigned int length);

void PubENV(const char *topic);
void PubMOTION(const char *topic);
void PubLight(const char *topic, uint8_t lightStatu);
void PubAir(const char *topic);
void PubFan(const char *topic);
void PubStrip(const char *topic);
void callback(char *Topic, byte *Paylaod, unsigned int Length);

//define M5 Publish Topic
#define PUB_LIGHT_1 "M5/light1"
#define PUB_LIGHT_2 "M5/light2"
#define PUB_LIGHT_3 "M5/light3"
#define PUB_AIR "M5/air"
#define PUB_FAN "M5/fan"
#define PUB_ENV "M5/env"
#define PUB_MOTION "M5/motion"
#define PUB_STRIP "M5/Strip"

#define SUB_LIGHT_1 "Node/light1"
#define SUB_LIGHT_2 "Node/light2"
#define SUB_LIGHT_3 "Node/light3"
#define SUB_AIR "Node/air"
#define SUB_FAN "Node/fan"
#define SUB_STRIP "Node/Strip"

#define AIR_GREEN_ICON_PATH "/AIR/Air_GREEN.png"
#define AIR_GREEN_HOVER_ICON_PATH "/AIR/Air_GREEN_Hover.png"
#define AIR_RED_ICON_PATH "/AIR/Air_RED.png"
#define AIR_RED_HOVER_ICON_PATH "/AIR/Air_RED_Hover.png"
#define FAN_ICON_PATH "/FAN/Fan_BLUE.png"
#define FAN_HOVER_ICON_PATH "/FAN/Fan_BLUE_Hover.png"
#define LIGHT_ICON_PATH "/LIGHT/Li_YELLOW.png"
#define LIGHT_HOVER_ICON_PATH "/LIGHT/Li_YELLOW_Hover.png"

#define KEY_TEMP "temp"
#define KEY_HUMI "humi"
#define KEY_PRESSURE "pres"
#define KEY_STATUS "st"
#define KEY_LEVEL "lv"
#define KEY_R "R"
#define KEY_G "G"
#define KEY_B "B"

#define FAN_INDEX 0
#define AIR_INDEX 1
#define LIGHT_1_INDEX 2
#define LIGHT_2_INDEX 3
#define LIGHT_3_INDEX 4

float temp;
float humi;
float pressure;

uint8_t motionStatus;

uint8_t light_1Status;
uint8_t light_2Status;
uint8_t light_3Status;

uint16_t light_1RGB[3];
uint16_t light_2RGB[3];
uint16_t light_3RGB[3];

uint8_t airStatus;
uint8_t airTemp;

uint8_t fanStatus;
uint8_t fanLevel;

uint8_t RGB_Strip;

uint8_t rgbStripStatus;

millisDelay pubDelay;
HCRL_EDU hcrl;

/*
    last variable data store 
*/
int data[7] = {0, 0, 0, 0, 0, 0, 0}; //0-fan , 1-air , 2-4-light , 5-temp , 6-rgb_strip

void setup()
{
    Serial.begin(115200);

    // hcrl.WiFi.begin("WIFIIV", "0245760494");
    // hcrl.MQTT.begin("192.168.1.127", HCRL_MQTT_PORT, callback);
    /*
        begin wifi connection
    */
    hcrl.WiFi.begin(HCRL_WiFi_SSID, HCRL_WiFi_PASS);

    /*
        begin mqtt connection 
    */
    hcrl.MQTT.begin(HCRL_MQTT_SERVER, HCRL_MQTT_PORT, callback);

    /*
        subscribe zone 
        start subscribe Topic from Node-RED
        "Node/light1"
        "Node/light2" 
        "Node/light3"
        "Node/air"
        "Node/fan"
    */
    hcrl.MQTT.startSubscribe(SUB_AIR);
    hcrl.MQTT.startSubscribe(SUB_FAN);
    hcrl.MQTT.startSubscribe(SUB_LIGHT_1);
    hcrl.MQTT.startSubscribe(SUB_LIGHT_2);
    hcrl.MQTT.startSubscribe(SUB_LIGHT_3);

    delay(100);

    /*
        begin display render 
    */
    hcrl.Ui.begin();
    //UI

    /*  
        define number of item in control
    */
    hcrl.Ui.node_init(5);
    for (int i = 0; i < 5; i++)
    {
        char buffer[50];
        sprintf(buffer, "Item %d", i);
        hcrl.Ui.node_setTitle(i, (String)buffer, "abcdefghijkl");
        hcrl.Ui.node_setTitlePic(i, AIR_RED_ICON_PATH, AIR_GREEN_HOVER_ICON_PATH);
    }

    /*
        set color of UI 
    */
    hcrl.Ui.node_setAllTitleColor(WHITE, CYAN);

    /*
        set UI index 0 to fan node and fan mode
    */
    hcrl.Ui.node_setType(FAN_INDEX, FAN);
    hcrl.Ui.node_setTitle(FAN_INDEX, "Fan", "Near Door");
    hcrl.Ui.node_setTitlePic(FAN_INDEX, FAN_ICON_PATH, FAN_HOVER_ICON_PATH);

    /*
        set UI index 1 to light node and air mode
    */
    hcrl.Ui.node_setType(AIR_INDEX, AIR);
    hcrl.Ui.node_setTitle(AIR_INDEX, "Air", " ");
    hcrl.Ui.node_setTitlePic(AIR_INDEX, AIR_RED_ICON_PATH, AIR_RED_HOVER_ICON_PATH);

    /*
        set UI index 2 to light node and light mode
    */
    hcrl.Ui.node_setType(LIGHT_1_INDEX, LIGHT);
    hcrl.Ui.node_setTitle(LIGHT_1_INDEX, "LIGHT", "1");
    hcrl.Ui.node_setTitlePic(LIGHT_1_INDEX, LIGHT_ICON_PATH, LIGHT_HOVER_ICON_PATH);

    /*
        set UI index 3 to light node and light mode
    */
    hcrl.Ui.node_setType(LIGHT_2_INDEX, LIGHT);
    hcrl.Ui.node_setTitle(LIGHT_2_INDEX, "LIGHT", "2");
    hcrl.Ui.node_setTitlePic(LIGHT_2_INDEX, LIGHT_ICON_PATH, LIGHT_HOVER_ICON_PATH);

    /*
        set UI index 4 to light node and light mode
    */
    hcrl.Ui.node_setType(LIGHT_3_INDEX, LIGHT);
    hcrl.Ui.node_setTitle(LIGHT_3_INDEX, "LIGHT", "3");
    hcrl.Ui.node_setTitlePic(LIGHT_3_INDEX, LIGHT_ICON_PATH, LIGHT_HOVER_ICON_PATH);

    /*
        set interval of publish to mqtt broker is 3 sec
    */
    pubDelay.start(Sec2MS(3));

    /*
        begin motion sensor
    */
    hcrl.MOTION.begin();

    /*
        begin env sensor
    */

    hcrl.ENV.begin();

    /*
        begin RGB led sensor
    */
    hcrl.LED.begin();

    /*
        begin RGB strip sensor 
    */
    hcrl.STRIP.begin();

    /*
        set default RGB led brightness
    */
    hcrl.LED.setBrightness(10);

    /*
        set default RGB strip brightness
    */
    hcrl.STRIP.setBrightness(10);
}
void loop()
{
    /*
        get current temperature and store in variable 
    */
    temp = hcrl.ENV.getTemp();

    /*
        get current humidity and store in variable 
    */
    humi = hcrl.ENV.getHumi();

    /*
        get current pressure and store in variable 
    */
    pressure = hcrl.ENV.getPressure();

    /*
        get current motion value  and store in variable 
    */
    motionStatus = hcrl.MOTION.getValue();

    /*
        get current data from ui node use index of each item to access data
    */
    light_1Status = hcrl.Ui.get_node_data(LIGHT_1_INDEX);
    light_2Status = hcrl.Ui.get_node_data(LIGHT_2_INDEX);
    light_3Status = hcrl.Ui.get_node_data(LIGHT_3_INDEX);

    airStatus = hcrl.Ui.get_node_data(AIR_INDEX);
    airTemp = hcrl.Ui.get_node_temp(AIR_INDEX);

    fanLevel = hcrl.Ui.get_node_data(FAN_INDEX);

    RGB_Strip = hcrl.Ui.get_strip_data();

  
    /*
        set RGB strip to fix color 
    */
    for (uint8_t i = 0; i < STRIP_NUM; i++)
    {
        hcrl.STRIP.set_RGB(i, 200, 0, 200);
        hcrl.STRIP.set_status(i, 1);
    }

    hcrl.LED.set_RGB(0, light_1RGB[0], light_1RGB[1], light_1RGB[2]);
    hcrl.LED.set_RGB(1, light_2RGB[0], light_2RGB[1], light_2RGB[2]);
    hcrl.LED.set_RGB(2, light_3RGB[0], light_3RGB[1], light_3RGB[2]);
    hcrl.LED.set_status(0, light_1Status);
    hcrl.LED.set_status(1, light_2Status);
    hcrl.LED.set_status(2, light_3Status);

    /*
        M5 Publish Zone 
        check if data is changed will publish to mqtt server 
    */
    if (fanLevel > 0) //on
    {
        fanStatus = 1;
    }
    else if (fanLevel == 0) //off
    {
        fanStatus = 0;
    }

    if (data[1] != airStatus || data[5] != airTemp)
    {
        data[1] = airStatus;
        data[5] = airTemp;
        PubAir(PUB_AIR);
    }

    if (data[0] != fanLevel)
    {
        data[0] = fanLevel;
        PubFan(PUB_FAN);
    }
    if (data[2] != light_1Status || data[3] != light_2Status || data[4] != light_3Status)
    {
        data[2] = light_1Status;
        data[3] = light_2Status;
        data[4] = light_3Status;
        PubLight(PUB_LIGHT_1, light_1Status);
        PubLight(PUB_LIGHT_2, light_2Status);
        PubLight(PUB_LIGHT_3, light_3Status);
    }

    if (pubDelay.justFinished()) //m5->Node-red
    {
        PubENV(PUB_ENV);
        PubMOTION(PUB_MOTION);
        /*
            repeat pubDelay interval 
        */
        pubDelay.repeat();
    }

    /*
        update data of ui and mqtt 
    */
    hcrl.update();
}

// Default Callback Function
void callback(char *Topic, byte *Paylaod, unsigned int Length)
{
    Paylaod[Length] = '\0';
    String topic_str = Topic, payload_str = (char *)Paylaod;
    Serial.print("[" + topic_str + "]: " + payload_str);

    if (topic_str.equals(SUB_LIGHT_1))
    {
        SubLight(Paylaod, Length, &light_1Status, light_1RGB, LIGHT_1_INDEX);
        //Serial.printf(" Successfully set %d %d %d\n", light_1RGB[0], light_1RGB[1], light_1RGB[2]);
    }
    else if (topic_str.equals(SUB_LIGHT_2))
    {
        SubLight(Paylaod, Length, &light_2Status, light_2RGB, LIGHT_2_INDEX);
        //Serial.printf(" Successfully set %d %d %d\n", light_2RGB[0], light_2RGB[1], light_2RGB[2]);
    }
    else if (topic_str.equals(SUB_LIGHT_3))
    {
        SubLight(Paylaod, Length, &light_3Status, light_3RGB, LIGHT_3_INDEX);
        //Serial.printf(" Successfully set %d %d %d\n", light_3RGB[0], light_3RGB[1], light_3RGB[2]);
    }
    else if (topic_str.equals(SUB_AIR))
    {
        SubAir(Paylaod, Length);
        //Serial.println("");
    }
    else if (topic_str.equals(SUB_FAN))
    {
        SubFan(Paylaod, Length);
        //Serial.println("");
    }
}

/*
{
    "st": boolean,  --> status of eachlight 
    "R": number,	--> Red
    "G": number,	--> Green
    "B": number		--> Blue
}
*/
void SubLight(byte *payload, unsigned int length, uint8_t *lightStatus, uint16_t *lightRGB, uint8_t lightIndex)
{
    /*
        deserialize byte payload to arduinoJSON 
        and get value from specification key 
    */
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload, length);
    *lightStatus = doc[KEY_STATUS];
    lightRGB[0] = doc[KEY_R];
    lightRGB[1] = doc[KEY_G];
    lightRGB[2] = doc[KEY_B];

    /*
        set data in UI with data and UI index 
    */
    hcrl.Ui.set_node_data(lightIndex, *lightStatus);
}

/*
{
    "st": boolean,	--> Status of each air conditioner
    "temp" : number	--> Target temperature
}
*/
void SubAir(byte *payload, unsigned int length)
{
    /*
        deserialize byte payload to arduinoJSON 
        and get value from specification key 
    */
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload, length);

    airStatus = doc[KEY_STATUS];
    data[1] = airStatus;
    airTemp = doc[KEY_TEMP];
    data[5] = airTemp;

    hcrl.Ui.set_node_data(AIR_INDEX, airStatus);
    hcrl.Ui.set_node_temp(AIR_INDEX, airTemp);
}

/*
{
    "st": boolean,	--> Status of each fan
    "level": number	--> Fan level
}
*/
void SubFan(byte *payload, unsigned int length)
{
    /*
        deserialize byte payload to arduinoJSON 
        and get value from specification key 
    */
    StaticJsonDocument<1024> doc;
    deserializeJson(doc, payload, length);
    fanStatus = doc[KEY_STATUS];
    fanLevel = doc[KEY_LEVEL];

    hcrl.Ui.set_node_data(FAN_INDEX, fanStatus);
}

/*
{
    "temp": number,		--> Current Temperature
    "humi": number,		--> Curremt HUmidity
    "pressure":number	--> Current Pressure
}
*/
void PubENV(const char *topic)
{
    /*
       seriallize data to string and send to mqtt broker
        
    */
    size_t size = 1024;
    DynamicJsonDocument docJson(size);
    char json[1024];
    docJson[KEY_TEMP] = temp;
    docJson[KEY_HUMI] = humi;
    docJson[KEY_PRESSURE] = pressure;
    serializeJson(docJson, json);

    //Sprintln(String(topic) + " : " + String(json));
    hcrl.MQTT.publish(topic, json);
}

/*
{
    "st": boolean	--> Current Value of Motion Sensor
}
*/
void PubMOTION(const char *topic)
{
    /*
       seriallize data to string and send to mqtt broker
        
    */
    size_t size = 1024;
    DynamicJsonDocument docJson(size);
    char json[1024];
    docJson[KEY_STATUS] = motionStatus;
    serializeJson(docJson, json);
    //Sprintln(String(topic) + " : " + String(json));
    hcrl.MQTT.publish(topic, json);
}

/*
{
    "st": boolean	-->light status
}
*/
void PubLight(const char *topic, uint8_t lightStatus)
{
    /*
       seriallize data to string and send to mqtt broker
        
    */
    size_t size = 1024;
    DynamicJsonDocument docJson(size);
    char json[1024];
    docJson[KEY_STATUS] = lightStatus;
    serializeJson(docJson, json);
    //Sprintln(String(topic) + " : " + String(json));
    hcrl.MQTT.publish(topic, json);
}

/*
{
    "st": boolean,	--> Status of each air conditioner
    "temp" : number	--> Target temperature
}
*/
void PubAir(const char *topic)
{
    /*
       seriallize data to string and send to mqtt broker
        
    */
    size_t size = 1024;
    DynamicJsonDocument docJson(size);
    char json[1024];
    docJson[KEY_STATUS] = airStatus;
    docJson[KEY_TEMP] = airTemp;
    serializeJson(docJson, json);
    //Sprintln(String(topic) + " : " + String(json));
    hcrl.MQTT.publish(topic, json);
}

/*
{
    "st": boolean,	--> Status of each fan
    "level": number	--> Fan level
}
*/
void PubFan(const char *topic)
{
    /*
       seriallize data to string and send to mqtt broker
        
    */
    size_t size = 1024;
    DynamicJsonDocument docJson(size);
    char json[1024];
    docJson[KEY_STATUS] = fanStatus;
    docJson[KEY_LEVEL] = fanLevel;
    serializeJson(docJson, json);
    //Sprintln(String(topic) + " : " + String(json));
    hcrl.MQTT.publish(topic, json);
}

void PubStrip(const char *topic)
{
    /*
       seriallize data to string and send to mqtt broker
        
    */
    size_t size = 1024;
    DynamicJsonDocument docJson(size);
    char json[1024];
    docJson[KEY_STATUS] = RGB_Strip;
    serializeJson(docJson, json);
    hcrl.MQTT.publish(topic, json);
}
