#include <Arduino.h>
#include "HCRL_EDU.h"

#define PUB_LIGHT_1 "M5/light1"
#define PUB_LIGHT_2 "M5/light2"
#define PUB_LIGHT_3 "M5/light3"
#define PUB_AIR "M5/air"
#define PUB_FAN "M5/fan"
#define PUB_ENV "M5/env"
#define PUB_PIR "M5/pir"

#define SUB_LIGHT_1 "Node/light1"
#define SUB_LIGHT_2 "Node/light2"
#define SUB_LIGHT_3 "Node/light3"
#define SUB_AIR "Node/air"
#define SUB_FAN "Node/fan"

#define KEY_TEMP "temp"
#define KEY_HUMI "humi"
#define KEY_PRESSURE "pres"
#define KEY_STATUS "st"
#define KEY_LEVEL "lv"
#define KEY_R "R"
#define KEY_G "G"
#define KEY_B "B"

void SubLight(byte *payload, unsigned int length, uint8_t *lightStatus, uint16_t *, uint8_t lightIndex);
void SubAir(byte *payload, unsigned int length);
void SubFan(byte *payload, unsigned int length);
void PubENV(const char *topic);
void PubPIR(const char *topic);
void PubLight(const char *topic, uint8_t lightStatu);
void PubAir(const char *topic);
void PubFan(const char *topic);
void callback(char *Topic, byte *Paylaod, unsigned int Length);

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

millisDelay pubDelay;
millisDelay ledRand;
HCRL_EDU hcrl;

#define FAN_INDEX 0
#define AIR_INDEX 1
#define LIGHT_1_INDEX 2
#define LIGHT_2_INDEX 3
#define LIGHT_3_INDEX 4

int data[6] = {0, 0, 0, 0, 0, 0}; //0-fan , 1-air , 2-4-light , 5-temp

void setup()
{
	Serial.begin(115200);
	hcrl.WiFi.begin(HCRL_WiFi_SSID, HCRL_WiFi_PASS);
	// hcrl.WiFi.begin();
	hcrl.MQTT.begin(HCRL_MQTT_SERVER, HCRL_MQTT_PORT, callback);
	hcrl.MQTT.startSubscribe("/test");
	hcrl.MQTT.startSubscribe(SUB_AIR);
	hcrl.MQTT.startSubscribe(SUB_FAN);
	hcrl.MQTT.startSubscribe(SUB_LIGHT_1);
	hcrl.MQTT.startSubscribe(SUB_LIGHT_2);
	hcrl.MQTT.startSubscribe(SUB_LIGHT_3);

	//Get Status
	Sprintln("WiFi SSID : " + String(hcrl.WiFi.getSSID()));
	Sprintln("WiFi Status : " + String(hcrl.WiFi.getStatus()));

	Sprintln("MQTT Server : " + String(hcrl.MQTT.getServer()));
	Sprintln("MQTT Port : " + String(hcrl.MQTT.getPort()));
	Sprintln("MQTT Username : " + String(hcrl.MQTT.getUsername()));
	Sprintln("MQTT Status : " + String(hcrl.MQTT.getStatus()));

	hcrl.Ui.begin();

	//UI
	hcrl.Ui.node_init(5);
	for (int i = 0; i < 5; i++)
	{
		char buffer[50];
		sprintf(buffer, "Item %d", i);
		hcrl.Ui.node_setTitle(i, (String)buffer, "abcdefghijkl");
		hcrl.Ui.node_setTitlePic(i, "/AIR/Air_GREEN.png", "/AIR/Air_GREEN_Hover.png");
	}

	hcrl.Ui.node_setAllTitleColor(WHITE, CYAN);

	hcrl.Ui.node_setType(FAN_INDEX, FAN);
	hcrl.Ui.node_setTitle(FAN_INDEX, "Fan", "Near Door");
	hcrl.Ui.node_setTitlePic(FAN_INDEX, "/FAN/Fan_BLUE.png", "/FAN/Fan_BLUE_Hover.png");

	hcrl.Ui.node_setType(AIR_INDEX, AIR);
	hcrl.Ui.node_setTitle(AIR_INDEX, "Air", " ");
	hcrl.Ui.node_setTitlePic(AIR_INDEX, "/AIR/Air_RED.png", "/AIR/Air_RED_Hover.png");

	hcrl.Ui.node_setType(LIGHT_1_INDEX, LIGHT);
	hcrl.Ui.node_setTitle(LIGHT_1_INDEX, "LIGHT", "1");
	hcrl.Ui.node_setTitlePic(LIGHT_1_INDEX, "/LIGHT/Li_YELLOW.png", "/LIGHT/Li_YELLOW_Hover.png");

	hcrl.Ui.node_setType(LIGHT_2_INDEX, LIGHT);
	hcrl.Ui.node_setTitle(LIGHT_2_INDEX, "LIGHT", "2");
	hcrl.Ui.node_setTitlePic(LIGHT_2_INDEX, "/LIGHT/Li_YELLOW.png", "/LIGHT/Li_YELLOW_Hover.png");

	hcrl.Ui.node_setType(LIGHT_3_INDEX, LIGHT);
	hcrl.Ui.node_setTitle(LIGHT_3_INDEX, "LIGHT", "3");
	hcrl.Ui.node_setTitlePic(LIGHT_3_INDEX, "/LIGHT/Li_YELLOW.png", "/LIGHT/Li_YELLOW_Hover.png");

	pubDelay.start(Sec2MS(3));
	ledRand.start(Sec2MS(1));

	hcrl.ANGLE.begin();
	hcrl.ENV.begin();
	hcrl.MOTION.begin();
	hcrl.RGB_LED.begin();
	hcrl.RGB_LED.setBrightness(10);

	hcrl.RGB_STRIP.begin();
	hcrl.RGB_STRIP.setBrightness(10);
}

void loop()
{
	temp = hcrl.ENV.getTemp();
	humi = hcrl.ENV.getHumi();
	pressure = hcrl.ENV.getPressure();

	motionStatus = hcrl.MOTION.getValue();

	light_1Status = hcrl.Ui.get_node_data(2);
	light_2Status = hcrl.Ui.get_node_data(3);
	light_3Status = hcrl.Ui.get_node_data(4);

	airStatus = hcrl.Ui.get_node_data(1);
	airTemp = hcrl.Ui.get_node_temp(1);

	fanLevel = hcrl.Ui.get_node_data(0);

	if (fanLevel > 0) //on
	{
		fanStatus = 1;
	}
	else if (fanLevel == 0) //off
	{
		fanStatus = 0;
	}
	if (ledRand.justFinished())
	{
		hcrl.RGB_LED.setPixelsColor(0, light_1RGB[0], light_2RGB[1], light_3RGB[2]);
		hcrl.RGB_LED.setPixelsColor(0, light_2RGB[0], light_2RGB[1], light_2RGB[2]);
		hcrl.RGB_LED.setPixelsColor(0, light_3RGB[0], light_3RGB[1], light_3RGB[2]);
		for (int i = 0; i < RGB_STRIP_PIXELS; i++)
		{
			hcrl.RGB_STRIP.setPixelsColor(i, (uint8_t)random(254), (uint8_t)random(254), (uint8_t)random(254));
		}
		ledRand.repeat();
	}

	if (pubDelay.justFinished()) //m5->Node-red
	{
		if (data[1] != airStatus || data[5] != airTemp)
		{
			data[1] = airStatus;
			data[5] = airTemp;
			PubAir(PUB_AIR);
		}
		PubENV(PUB_ENV);
		if (data[0] != fanLevel)
		{
			data[0] = fanLevel;
			PubFan(PUB_FAN);
		}
		PubPIR(PUB_PIR);
		if (data[2] != light_1Status || data[3] != light_2Status || data[4] != light_3Status)
		{
			data[2] = light_1Status;
			data[3] = light_2Status;
			data[4] = light_3Status;
			PubLight(PUB_LIGHT_1, light_1Status);
			PubLight(PUB_LIGHT_2, light_2Status);
			PubLight(PUB_LIGHT_3, light_3Status);
		}

		pubDelay.repeat();
	}

	hcrl.update();
	randomSeed(millis());
}

// Default Callback Function
void callback(char *Topic, byte *Paylaod, unsigned int Length)
{
	Paylaod[Length] = '\0';
	String topic_str = Topic, payload_str = (char *)Paylaod;
	Serial.println("[" + topic_str + "]: " + payload_str);
	if (topic_str.equals(SUB_LIGHT_1))
	{
		SubLight(Paylaod, Length, &light_1Status, light_1RGB, LIGHT_1_INDEX);
	}
	else if (topic_str.equals(SUB_LIGHT_2))
	{
		SubLight(Paylaod, Length, &light_2Status, light_2RGB, LIGHT_2_INDEX);
	}
	else if (topic_str.equals(SUB_LIGHT_3))
	{
		SubLight(Paylaod, Length, &light_3Status, light_3RGB, LIGHT_3_INDEX);
	}
	else if (topic_str.equals(SUB_AIR))
	{
		SubAir(Paylaod, Length);
	}
	else if (topic_str.equals(SUB_FAN))
	{
		SubFan(Paylaod, Length);
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
	StaticJsonDocument<1024> doc;
	deserializeJson(doc, payload, length);
	*lightStatus = doc[KEY_STATUS];
	lightRGB[0] = doc[KEY_R];
	lightRGB[1] = doc[KEY_G];
	lightRGB[2] = doc[KEY_B];
	hcrl.Ui.set_node_data(lightIndex, *lightStatus);
	if (lightStatus == 0)
	{
		hcrl.RGB_LED.setPixelsColor(lightIndex, 0, 0, 0);
	}
	else
	{
		hcrl.RGB_LED.setPixelsColor(lightIndex, lightRGB[0], lightRGB[1], lightRGB[2]);
	}
}

/*
{
    "st": boolean,	--> Status of each air conditioner
    "temp" : number	--> Target temperature
}
*/
void SubAir(byte *payload, unsigned int length)
{
	StaticJsonDocument<1024> doc;
	deserializeJson(doc, payload, length);

	airStatus = doc[KEY_STATUS];
	data[1] = airStatus;
	hcrl.Ui.set_node_data(1, airStatus);

	airTemp = doc[KEY_TEMP];
	data[5] = airTemp;
	hcrl.Ui.set_node_temp(1, airTemp);
}

/*
{
    "st": boolean,	--> Status of each fan
    "level": number	--> Fan level
}
*/
void SubFan(byte *payload, unsigned int length)
{
	StaticJsonDocument<1024> doc;
	deserializeJson(doc, payload, length);
	fanStatus = doc[KEY_STATUS];
	fanLevel = doc[KEY_LEVEL];
	hcrl.Ui.set_node_data(0, fanStatus);
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
void PubPIR(const char *topic)
{
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
	size_t size = 1024;
	DynamicJsonDocument docJson(size);
	char json[1024];
	docJson[KEY_STATUS] = fanStatus;
	docJson[KEY_LEVEL] = fanLevel;
	serializeJson(docJson, json);
	//Sprintln(String(topic) + " : " + String(json));
	hcrl.MQTT.publish(topic, json);
}