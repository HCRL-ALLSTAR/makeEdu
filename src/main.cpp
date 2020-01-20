#include <Arduino.h>
#include <HCRL_Edu.h>

HCRL_Edu hcrl;
void callback(char *Topic, byte *Paylaod, unsigned int Length)
{
	Paylaod[Length] = '\0';
	String topic_str = Topic, payload_str = (char *)Paylaod;
	Serial.println("[" + topic_str + "]: " + payload_str);
}

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
