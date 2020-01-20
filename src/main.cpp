#include <Arduino.h>
#include <HCRL_Edu.h>

HCRL_Edu hcrl;

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

//Make Json
void SubLight();
void SubAir();
void SubFan();
void PubENV();
void PubPIR();
void PubLight(const char *lightTopic);
void PubAir();
void PubFan();
void callback(char *Topic, byte *Paylaod, unsigned int Length);

void setup(void)
{
	//Serial.begin(115200);
	hcrl.Ui.begin();
	hcrl.WiFi.Begin(HCRL_WiFi_SSID, HCRL_WiFi_PASS);
	hcrl.MQTT.begin(HCRL_MQTT_SERVER, HCRL_MQTT_PORT, callback);
	hcrl.MQTT.startSubscribe("/test");
	hcrl.Ui.node_init(5);
	for (int i = 0; i < 5; i++)
	{
		char buffer[50];
		sprintf(buffer, "Item %d", i);
		hcrl.Ui.node_setTitle(i, (String)buffer, "abcdefghijkl");
		hcrl.Ui.node_setTitlePic(i, "/AIR/Air_GREEN.png", "/AIR/Air_GREEN_Hover.png");
	}

	hcrl.Ui.node_setAllTitleColor(WHITE, CYAN);

	hcrl.Ui.node_setType(0, FAN);
	hcrl.Ui.node_setTitle(0, "Fan", "Near Door");
	hcrl.Ui.node_setTitlePic(0, "/FAN/Fan_BLUE.png", "/FAN/Fan_BLUE_Hover.png");

	hcrl.Ui.node_setType(1, AIR);
	hcrl.Ui.node_setTitle(1, "Air", " ");
	hcrl.Ui.node_setTitlePic(1, "/AIR/Air_RED.png", "/AIR/Air_RED_Hover.png");

	hcrl.Ui.node_setType(2, LIGHT);
	hcrl.Ui.node_setTitle(2, "LIGHT", "Overall");
	hcrl.Ui.node_setTitlePic(2, "/LIGHT/Li_YELLOW.png", "/LIGHT/Li_YELLOW_Hover.png");
}
void loop(void)
{
	hcrl.MQTT.publish("/testPub", "Hello From M5");
	hcrl.update();
}

void callback(char *Topic, byte *Paylaod, unsigned int Length)
{
	Paylaod[Length] = '\0';
	String topic_str = Topic, payload_str = (char *)Paylaod;
	Serial.println("[" + topic_str + "]: " + payload_str);
}

/*
{
    "st": boolean,  --> status of eachlight 
    "R": number,	--> Red
    "G": number,	--> Green
    "B": number		--> Blue
}
*/
void SubLight()
{
}

/*
{
    "st": boolean,	--> Status of each air conditioner
    "temp" : number	--> Target temperature
}
*/
void SubAir()
{
}

/*
{
    "st": boolean,	--> Status of each fan
    "level": number	--> Fan level
}
*/
void SubFan()
{
}

/*
{
    "temp": number,		--> Current Temperature
    "humi": number,		--> Curremt HUmidity
    "pressure":number	--> Current Pressure
}
*/
void PubENV()
{
}

/*
{
    "st": boolean	--> Current Value of Motion Sensor
}
*/
void PubPIR()
{
}

/*
{
    "st": boolean	-->light status
}
*/
void PubLight(const char *lightTopic)
{
}

/*
{
    "st": boolean,	--> Status of each air conditioner
    "temp" : number	--> Target temperature
}
*/
void PubAir()
{
}

/*
{
    "st": boolean,	--> Status of each fan
    "level": number	--> Fan level
}
*/
void PubFan()
{
}