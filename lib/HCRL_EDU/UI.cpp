// Copyright (c) M5Stack. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include "UI.h"

#define BLK_PWM_CHANNEL 7 // LEDC_CHANNEL_7

UI::UI() : isInited(0)
{
  this->timer = timerBegin(0, 80, true); //timer 0, div 80
  timerAttachInterrupt(timer, esp_restart, true);
  timerAlarmWrite(timer, 3000000, false); //set time in us
  timerAlarmEnable(timer);                //enable interrupt
}

//init UI
void UI::begin(bool LCDEnable, bool SDEnable, bool SerialEnable)
{
  // Correct init once
  if (isInited == true)
  {
    return;
  }
  else
  {
    isInited = true;
  }

  // UART
  if (SerialEnable == true)
  {
    if (!Serial)
      Serial.begin(115200);
    Serial.flush();
    delay(50);
    Serial.print("M5Stack initializing...");
  }

  // LCD INIT
  if (LCDEnable == true)
  {
    Lcd.begin(backgroundColor);
  }

  // TF Card
  if (SDEnable == true)
  {
    SD.begin(TFCARD_CS_PIN, SPI, 40000000);
  }

  if (!SPIFFS.begin(true))
  {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  for (int i = 0; i < 32; i++)
  {
    this->data[i] = 0;
    this->temp_data[i] = 25;
    this->last_temp_data[i] = -1;
    this->last_data[i] = -1;
    this->EN[i] = false;
    this->title1stColor[i] = WHITE;
    this->title2ndColor[i] = WHITE;
    this->titleSize[i] = 2;
    node_setTitlePic(i, "/dummy.png", "/dummy.png");
  }

  if (SerialEnable == true)
  {
    Serial.println("OK");
  }

  power.begin();
}

//update all UI components
void UI::update()
{
  timerWrite(this->timer, 0);
  // //Button update
  BtnA.read();
  BtnB.read();
  BtnC.read();
  //control
  if (BtnA.wasPressed())
  {
    if (panel != MAIN && sub_panel == MAIN)
    {
      Lcd.fillRect(0, 0, 320, 218, backgroundColor);
      this->m.lastLevel = -1;
    }
    if (panel == STAT)
    {
      this->panel = MAIN;
    }
    if (panel == CONT)
    {
      if (sub_panel == MAIN)
      {
        this->c_panel.lastIndex = -1;
        this->panel = MAIN;
      }
      else if (sub_panel == AIRCONT)
      {
        this->temp_data[c_panel.index] -= 1;
      }
    }
    if (panel == SETT)
    {
      this->panel = MAIN;
    }
  }

  if (BtnB.pressedFor(RETURN_MS))
  {
    if (panel == MAIN)
    {
    }
    else if (panel == STAT)
    {
    }
    else if (panel == CONT)
    {
      if (type[c_panel.index] == AIR && sub_panel == MAIN)
      {
        Lcd.setTextColor(backgroundColor);
        Lcd.setTextSize(2);
        Lcd.setCursor(5, 221);
        Lcd.print(m.leftText);
        Lcd.setCursor(219, 221);
        Lcd.print(m.rightText);
        this->sub_panel = AIRCONT;
        this->hold = true;
      }
    }
    else if (panel == SETT)
    {
    }
  }
  else if (BtnB.wasReleased())
  {
    if (panel == MAIN)
    {
      Lcd.fillRect(0, 0, 320, 218, backgroundColor);
      this->m.lastLevel = -1;
      if (m_panel.select == 0b100)
      { //status
        this->panel = STAT;
      }
      else if (m_panel.select == 0b010)
      { //control
        this->panel = CONT;
        this->last_data[c_panel.index] = -1;
        this->last_temp_data[c_panel.index] = -1;
      }
      else
      { //settings
        this->panel = SETT;
      }
      this->m_panel.lastSel = 0b000;
    }
    else if (panel == STAT)
    {
    }
    else if (panel == CONT)
    {
      if (sub_panel == MAIN)
      {
        if (type[c_panel.index] == FAN)
        {
          this->data[c_panel.index]++;
          if (data[c_panel.index] == 4)
            this->data[c_panel.index] = 0;
        }
        else
          this->data[c_panel.index] = !data[c_panel.index];
      }
      else if (sub_panel == AIRCONT && !hold)
      {
        Lcd.setTextColor(backgroundColor);
        Lcd.setTextSize(2);
        Lcd.setCursor(5, 221);
        Lcd.print(m.leftText);
        Lcd.setCursor(219, 221);
        Lcd.print(m.rightText);
        this->sub_panel = MAIN;
      }
      else
        this->hold = false;

      if (type[c_panel.index] == LIGHT || data[c_panel.index] == 0 || data[c_panel.index] == 1)
        this->c_panel.lastIndex = -1;
      this->m.lastLevel = -1;
      this->last_data[c_panel.index] = -1;
      this->last_temp_data[c_panel.index] = -1;
    }
    else if (panel == SETT)
    {
      this->st_panel.st_data[m.cursor] += 1;
      if (st_panel.st_data[m.cursor] == 5){
        if (m.cursor > 0)
          this->st_panel.st_data[m.cursor] = 0;
        else
          this->st_panel.st_data[m.cursor] = 1;
      }
      this->st_panel.change = true;
    }
  }

  if (BtnC.pressedFor(RETURN_MS))
  {
    if (panel == CONT)
    {
      if (sub_panel == MAIN)
      {
        this->c_panel.index = 0;
        this->hold2 = true;

        this->last_data[c_panel.index] = -1;
        this->last_temp_data[c_panel.index] = -1;
      }
      else if (sub_panel == AIRCONT)
      {
      }
    }
  }
  else if (BtnC.wasPressed())
  {
    if (panel == SETT)
    {
      this->m.cursor += 1;
      if (m.cursor == 3)
        this->m.cursor = 0;
      this->st_panel.change2 = true;
    }
    if (panel == MAIN)
    {
      this->m_panel.select >>= 1;
      if (m_panel.select == 0)
        this->m_panel.select = 0b100;
    }
  }
  else if (BtnC.wasReleased())
  {
    if (panel == CONT && !hold2)
    {
      if (sub_panel == MAIN)
      {
        this->c_panel.index += 1;
        if (c_panel.index == c_panel.size)
          this->c_panel.index = 0;

        this->last_data[c_panel.index] = -1;
        this->last_temp_data[c_panel.index] = -1;
      }
      else if (sub_panel == AIRCONT)
      {
        this->temp_data[c_panel.index] += 1;
      }
    }
    else
      this->hold2 = false;
  }
  //main
  main();
}

void UI::setBrightness(uint8_t brightness)
{
  ledcWrite(BLK_PWM_CHANNEL, brightness);
}

void UI::main()
{
  if (panel == MAIN)
  {
    main_panel();
  }
  else if (panel == STAT)
  {
    stat_panel();
  }
  else if (panel == CONT)
  {
    cont_panel();
  }
  else if (panel == SETT)
  {
    sett_panel();
  }

  menu_disp(); //menu display don't change
}

void UI::main_panel()
{
  m.leftBtn_enable = false;
  m.midBtn_enable = true;
  m.rightBtn_enable = true;
  st_panel.change = true;

  //box
  if (m_panel.select != m_panel.lastSel)
  {
    Lcd.setTextColor(m_panel.titleColor);
    Lcd.setCursor(10, 10);
    Lcd.setTextSize(m_panel.titleSize);
    Lcd.print(m_panel.title);

    if (m_panel.select & 0b100)
      Lcd.fillRoundRect(159 - 135, 50, 120, 120, 15, m_panel.selFillColor);
    else
      Lcd.fillRoundRect(159 - 135, 50, 120, 120, 15, m_panel.fillColor);
    if (m_panel.select & 0b010)
      Lcd.fillRoundRect(159 + 15, 50, 120, 120, 15, m_panel.selFillColor);
    else
      Lcd.fillRoundRect(159 + 15, 50, 120, 120, 15, m_panel.fillColor);
    if (m_panel.select & 0b001)
      Lcd.fillRoundRect(159 - 75, 180, 150, 30, 15, m_panel.selFillColor);
    else
      Lcd.fillRoundRect(159 - 75, 180, 150, 30, 15, m_panel.fillColor);

    if (m_panel.select & 0b100)
      Lcd.drawRoundRect(159 - 135, 50, 120, 120, 15, m_panel.selLineColor);
    else
      Lcd.drawRoundRect(159 - 135, 50, 120, 120, 15, m_panel.lineColor);
    if (m_panel.select & 0b010)
      Lcd.drawRoundRect(159 + 15, 50, 120, 120, 15, m_panel.selLineColor);
    else
      Lcd.drawRoundRect(159 + 15, 50, 120, 120, 15, m_panel.lineColor);
    if (m_panel.select & 0b001)
      Lcd.drawRoundRect(159 - 75, 180, 150, 30, 15, m_panel.selLineColor);
    else
      Lcd.drawRoundRect(159 - 75, 180, 150, 30, 15, m_panel.lineColor);
    this->m_panel.lastSel = m_panel.select;

    //text
    Lcd.setTextColor(m_panel.lineColor);
    Lcd.setTextSize(2);
    Lcd.setCursor(159 - 70, 190);
    Lcd.print("  Settings  ");
    Lcd.setCursor(159 - 133, 105);
    Lcd.print("  Status ");
    Lcd.setCursor(159 + 23, 105);
    Lcd.print(" Control ");
  }
}

void UI::stat_panel()
{

  m.leftBtn_enable = true;
  m.midBtn_enable = false;
  m.rightBtn_enable = false;

  Lcd.setTextColor(sa_panel.titleColor);
  Lcd.setCursor(10, 10);
  Lcd.setTextSize(sa_panel.titleSize);
  Lcd.print(sa_panel.title);

  Lcd.setTextSize(2);
  Lcd.setCursor(0, 40);
  Lcd.printf(" WiFi SSID: ");
  if (wifi_ssid != last_wifi_ssid)
  {
    Lcd.fillRect(Lcd.getCursorX(), Lcd.getCursorY(), 200, 20, backgroundColor);
    this->last_wifi_ssid = wifi_ssid;
  }
  Lcd.println(wifi_ssid);
  Lcd.printf(" WiFi Status: ");
  if (wifi_status != last_wifi_status)
  {
    Lcd.fillRect(Lcd.getCursorX(), Lcd.getCursorY(), 200, 20, backgroundColor);
    this->last_wifi_status = wifi_status;
  }
  Lcd.println(wifi_status);
  Lcd.println("");
  Lcd.setTextColor(RED);
  Lcd.printf(" Temperature: ");
  if (temp != last_temp)
  {
    Lcd.fillRect(Lcd.getCursorX(), Lcd.getCursorY(), 200, 20, backgroundColor);
    this->last_temp = temp;
  }
  Lcd.printf("%.2f\n", temp);
  Lcd.setTextColor(GREEN);
  Lcd.printf(" Humidity: ");
  if (humid != last_humid)
  {
    Lcd.fillRect(Lcd.getCursorX(), Lcd.getCursorY(), 200, 20, backgroundColor);
    this->last_humid = humid;
  }
  Lcd.printf("%.2f\n", humid);
  Lcd.setTextColor(AQUA);
  Lcd.printf(" Pressure: ");
  if (pa != last_pa)
  {
    Lcd.fillRect(Lcd.getCursorX(), Lcd.getCursorY(), 200, 20, backgroundColor);
    this->last_pa = pa;
  }
  Lcd.printf("%.2f\n\n", pa);
  Lcd.setTextColor(sa_panel.titleColor);
  Lcd.printf(" MQTT IP: ");
  if (mqtt_ip != last_mqtt_ip)
  {
    Lcd.fillRect(Lcd.getCursorX(), Lcd.getCursorY(), 200, 20, backgroundColor);
    this->last_mqtt_ip = mqtt_ip;
  }
  Lcd.println(mqtt_ip);
  Lcd.printf(" MQTT Status: ");
  if (mqtt_status != last_mqtt_status)
  {
    Lcd.fillRect(Lcd.getCursorX(), Lcd.getCursorY(), 200, 20, backgroundColor);
    this->last_mqtt_status = mqtt_status;
  }
  Lcd.println(mqtt_status);
  Lcd.setTextColor(YELLOW);
  Lcd.printf(" PIR: ");
  if (motion != last_motion)
  {
    Lcd.fillRect(Lcd.getCursorX(), Lcd.getCursorY(), 200, 20, backgroundColor);
    this->last_motion = motion;
  }
  Lcd.printf("%d\n", motion);
  if (power.isCharging())
    Lcd.setTextColor(GREEN);
  else
    Lcd.setTextColor(CYAN);
  Lcd.printf(" Battery Level: ");
  if (power.getBatteryLevel() != m.lastLevel)
  {
    Lcd.fillRect(Lcd.getCursorX(), Lcd.getCursorY(), 200, 20, backgroundColor);
  }
  Lcd.printf("%d ", power.getBatteryLevel());
  if (power.isCharging())
    Lcd.printf("Charge\n");
}

void UI::cont_panel()
{
  if (sub_panel == MAIN)
  {
    this->m.leftText = m.BACK;
    this->m.rightText = m.NEXT;
  }
  else if (sub_panel == AIRCONT)
  {
    this->m.leftText = m.MINUS;
    this->m.rightText = m.PLUS;
  }

  m.leftBtn_enable = true;
  m.midBtn_enable = true;
  m.rightBtn_enable = true;

  Lcd.setTextColor(c_panel.titleColor);
  Lcd.setCursor(10, 10);
  Lcd.setTextSize(c_panel.titleSize);
  Lcd.print(c_panel.title);

  if (c_panel.index != c_panel.lastIndex)
  {

    if (c_panel.index - 1 >= 0)
    {
      Lcd.fillRoundRect(160 - 65 - 10 - 130 - 10, 120 - 65, 130, 130, 5, c_panel.fillColor);
      Lcd.drawRoundRect(160 - 65 - 10 - 130 - 10, 120 - 65, 130, 130, 5, c_panel.lineColor);
    }
    else
    {
      Lcd.fillRoundRect(160 - 65 - 10 - 130 - 10, 120 - 65, 130, 130, 5, backgroundColor);
      Lcd.drawRoundRect(160 - 65 - 10 - 130 - 10, 120 - 65, 130, 130, 5, backgroundColor);
    }

    if (c_panel.index + 1 < c_panel.size)
    {
      Lcd.fillRoundRect(160 - 65 + 10 + 130 + 10, 120 - 65, 130, 130, 5, c_panel.fillColor);
      Lcd.drawRoundRect(160 - 65 + 10 + 130 + 10, 120 - 65, 130, 130, 5, c_panel.lineColor);
    }
    else
    {
      Lcd.fillRoundRect(160 - 65 - 10 + 130 + 10, 120 - 65, 130, 130, 5, backgroundColor);
      Lcd.drawRoundRect(160 - 65 - 10 + 130 + 10, 120 - 65, 130, 130, 5, backgroundColor);
    }

    Lcd.fillRoundRect(160 - 75, 120 - 75, 150, 150, 5, c_panel.fillColor);
    Lcd.drawRoundRect(160 - 75, 120 - 75, 150, 150, 5, c_panel.lineColor);

    uint8_t offset_x;
    if (!type[c_panel.index])
    {
      offset_x = 0;
    }
    else
    {
      offset_x = 18;
    }
    if (!data[c_panel.index])
      Lcd.drawPngFile(SPIFFS, titlePic[c_panel.index], 160 - 75 + 22 - offset_x, 120 - 75 + 3);
    else
      Lcd.drawPngFile(SPIFFS, titlePic_Hover[c_panel.index], 160 - 75 + 22 - offset_x, 120 - 75 + 3);

    if (type[c_panel.index] == FAN)
    {
      Lcd.setTextColor(m.lineColor);
      Lcd.setTextSize(2);
      Lcd.setCursor(160 - 75 + 113 - 1, 120 - 75 + 12);
      Lcd.print(c_panel.up);
      Lcd.setCursor(160 - 75 + 113 - 1, 120 - 75 + 3 + 26 + 57);
      Lcd.print(c_panel.down);

      Lcd.drawRoundRect(160 - 75 + 113, 120 - 75 + 3 + 26, 33, 17, 5, c_panel.lineColor);
      Lcd.drawRoundRect(160 - 75 + 113, 120 - 75 + 3 + 26 + 19, 33, 17, 5, c_panel.lineColor);
      Lcd.drawRoundRect(160 - 75 + 113, 120 - 75 + 3 + 26 + 38, 33, 17, 5, c_panel.lineColor);
    }
    else if (type[c_panel.index] == AIR)
    {
      Lcd.setTextColor(m.lineColor);
      Lcd.setTextSize(2);
      Lcd.setCursor(160 - 75 + 113 - 1, 120 - 75 + 12);
      Lcd.print(c_panel.up);
      Lcd.setCursor(160 - 75 + 113 - 1, 120 - 75 + 3 + 26 + 57);
      Lcd.print(c_panel.down);

      Lcd.drawRoundRect(160 - 75 + 113, 120 - 75 + 3 + 26, 33, 51, 5, c_panel.lineColor);
    }

    Lcd.setTextSize(titleSize[c_panel.index]);
    Lcd.setCursor(160 - 75 + 5, 120 + 75 - 10 - 10 - 20);
    Lcd.print(title_1st[c_panel.index]);
    Lcd.setTextColor(title2ndColor[c_panel.index]);
    Lcd.setCursor(160 - 75 + 5, 120 + 75 - 10 - 10);
    Lcd.print(title_2nd[c_panel.index]);

    this->c_panel.lastIndex = c_panel.index;
  }
  if (type[c_panel.index] == FAN)
  {
    if (data[c_panel.index] != last_data[c_panel.index])
    {
      if (data[c_panel.index] == 0)
      {
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 2, 33 - 4, 17 - 4, 3, c_panel.fillColor);
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 19 + 2, 33 - 4, 17 - 4, 3, c_panel.fillColor);
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 38 + 2, 33 - 4, 17 - 4, 3, c_panel.fillColor);
      }
      else if (data[c_panel.index] == 1)
      {
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 2, 33 - 4, 17 - 4, 3, c_panel.fillColor);
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 19 + 2, 33 - 4, 17 - 4, 3, c_panel.fillColor);
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 38 + 2, 33 - 4, 17 - 4, 3, c_panel.lineColor);
      }
      else if (data[c_panel.index] == 2)
      {
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 2, 33 - 4, 17 - 4, 3, c_panel.fillColor);
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 19 + 2, 33 - 4, 17 - 4, 3, c_panel.lineColor);
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 38 + 2, 33 - 4, 17 - 4, 3, c_panel.lineColor);
      }
      else if (data[c_panel.index] == 3)
      {
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 2, 33 - 4, 17 - 4, 3, c_panel.lineColor);
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 19 + 2, 33 - 4, 17 - 4, 3, c_panel.lineColor);
        Lcd.fillRoundRect(160 - 75 + 113 + 2, 120 - 75 + 3 + 26 + 38 + 2, 33 - 4, 17 - 4, 3, c_panel.lineColor);
      }

      this->last_data[c_panel.index] = data[c_panel.index];
    }
  }
  else if (type[c_panel.index] == AIR)
  {
    if (data[c_panel.index] != last_data[c_panel.index])
    {
      this->last_data[c_panel.index] = data[c_panel.index];
    }
    if (temp_data[c_panel.index] != last_temp_data[c_panel.index])
    {

      Lcd.setTextColor(backgroundColor);
      Lcd.setTextSize(2);
      Lcd.setCursor(160 - 75 + 113 + 5, 120 - 75 + 3 + 26 + 19);
      Lcd.print(last_temp_data[c_panel.index]);

      Lcd.setTextColor(m.lineColor);
      Lcd.setTextSize(2);
      Lcd.setCursor(160 - 75 + 113 + 5, 120 - 75 + 3 + 26 + 19);
      Lcd.print(temp_data[c_panel.index]);

      this->last_temp_data[c_panel.index] = temp_data[c_panel.index];
    }
  }
  else if (type[c_panel.index] == LIGHT)
  {
    if (data[c_panel.index] != last_data[c_panel.index])
    {
      this->last_data[c_panel.index] = data[c_panel.index];
    }
  }
}

void UI::sett_panel()
{

  m.leftBtn_enable = true;
  m.midBtn_enable = true;
  m.rightBtn_enable = true;
  // Lcd.drawRect(0,0,)

  Lcd.setTextColor(st_panel.titleColor);
  Lcd.setCursor(10, 10);
  Lcd.setTextSize(st_panel.titleSize);
  Lcd.print(st_panel.title);

  Lcd.setTextSize(2);
  Lcd.setTextColor(st_panel.lineColor);
  Lcd.setCursor(50, 63);
  Lcd.print("LCD Brightness");
  Lcd.setTextColor(st_panel.lineColor);
  Lcd.setCursor(50, 118);
  Lcd.print("LED Brightness");
  Lcd.setTextColor(st_panel.lineColor);
  Lcd.setCursor(50, 173);
  Lcd.print("STRIP Brightness");
  if(st_panel.change2){
    if (m.cursor == 0)
    {
      Lcd.fillRect(0, 154, 320, 56, backgroundColor);
      Lcd.fillRect(0, 42, 320, 56, st_panel.selFillColor);
    }
    else if (m.cursor == 1)
    {
      Lcd.fillRect(0, 42, 320, 56, backgroundColor);
      Lcd.fillRect(0, 98, 320, 56, st_panel.selFillColor);
    }
    else if (m.cursor == 2)
    {
      Lcd.fillRect(0, 98, 320, 56, backgroundColor);
      Lcd.fillRect(0, 154, 320, 56, st_panel.selFillColor);
    }
  }

  if (st_panel.change || st_panel.change2)
  {
    if (st_panel.st_data[0] == 1)
    {
      Lcd.setBrightness(1);
      Lcd.fillRect(230 + 20, 70, 10, 10, st_panel.fillColor);
      if(m.cursor == 0)
      {
        Lcd.fillRect(245 + 20, 65, 10, 15, st_panel.selFillColor);
        Lcd.fillRect(260 + 20, 60, 10, 20, st_panel.selFillColor);
        Lcd.fillRect(275 + 20, 55, 10, 25, st_panel.selFillColor);
      }
      else
      {
        Lcd.fillRect(245 + 20, 65, 10, 15, backgroundColor);
        Lcd.fillRect(260 + 20, 60, 10, 20, backgroundColor);
        Lcd.fillRect(275 + 20, 55, 10, 25, backgroundColor);
      }
    }
    else if (st_panel.st_data[0] == 2) //default
    {
      Lcd.setBrightness(80);
      Lcd.fillRect(230 + 20, 70, 10, 10, st_panel.fillColor);
      Lcd.fillRect(245 + 20, 65, 10, 15, st_panel.fillColor);
    }
    else if (st_panel.st_data[0] == 3)
    {
      Lcd.setBrightness(160);
      Lcd.fillRect(230 + 20, 70, 10, 10, st_panel.fillColor);
      Lcd.fillRect(245 + 20, 65, 10, 15, st_panel.fillColor);
      Lcd.fillRect(260 + 20, 60, 10, 20, st_panel.fillColor);
    }
    else if (st_panel.st_data[0] == 4) //max
    {
      Lcd.setBrightness(255);
      Lcd.fillRect(230 + 20, 70, 10, 10, st_panel.fillColor);
      Lcd.fillRect(245 + 20, 65, 10, 15, st_panel.fillColor);
      Lcd.fillRect(260 + 20, 60, 10, 20, st_panel.fillColor);
      Lcd.fillRect(275 + 20, 55, 10, 25, st_panel.fillColor);
    }
    if (st_panel.st_data[1] == 0)
    {
      this->led_brightness = 0;
      if (m.cursor == 1)
      {
        Lcd.fillRect(230 + 20, 70 + 56, 10, 10, st_panel.selFillColor);
        Lcd.fillRect(245 + 20, 65 + 56, 10, 15, st_panel.selFillColor);
        Lcd.fillRect(260 + 20, 60 + 56, 10, 20, st_panel.selFillColor);
        Lcd.fillRect(275 + 20, 55 + 56, 10, 25, st_panel.selFillColor);
      }
      else
      {
        Lcd.fillRect(230 + 20, 70 + 56, 10, 10, backgroundColor);
        Lcd.fillRect(245 + 20, 65 + 56, 10, 15, backgroundColor);
        Lcd.fillRect(260 + 20, 60 + 56, 10, 20, backgroundColor);
        Lcd.fillRect(275 + 20, 55 + 56, 10, 25, backgroundColor);
      }
    }
    if (st_panel.st_data[1] == 1)
    {
      this->led_brightness = 10;
      Lcd.fillRect(230 + 20, 70 + 56, 10, 10, st_panel.fillColor);
      if (m.cursor == 1)
      {
        Lcd.fillRect(245 + 20, 65 + 56, 10, 15, st_panel.selFillColor);
        Lcd.fillRect(260 + 20, 60 + 56, 10, 20, st_panel.selFillColor);
        Lcd.fillRect(275 + 20, 55 + 56, 10, 25, st_panel.selFillColor);
      }
      else
      {
        Lcd.fillRect(245 + 20, 65 + 56, 10, 15, backgroundColor);
        Lcd.fillRect(260 + 20, 60 + 56, 10, 20, backgroundColor);
        Lcd.fillRect(275 + 20, 55 + 56, 10, 25, backgroundColor);
      }
    }
    else if (st_panel.st_data[1] == 2) //default
    {
      this->led_brightness = 80;
      Lcd.fillRect(230 + 20, 70 + 56, 10, 10, st_panel.fillColor);
      Lcd.fillRect(245 + 20, 65 + 56, 10, 15, st_panel.fillColor);
    }
    else if (st_panel.st_data[1] == 3)
    {
      this->led_brightness = 160;
      Lcd.fillRect(230 + 20, 70 + 56, 10, 10, st_panel.fillColor);
      Lcd.fillRect(245 + 20, 65 + 56, 10, 15, st_panel.fillColor);
      Lcd.fillRect(260 + 20, 60 + 56, 10, 20, st_panel.fillColor);
    }
    else if (st_panel.st_data[1] == 4) //max
    {
      this->led_brightness = 255;
      Lcd.fillRect(230 + 20, 70 + 56, 10, 10, st_panel.fillColor);
      Lcd.fillRect(245 + 20, 65 + 56, 10, 15, st_panel.fillColor);
      Lcd.fillRect(260 + 20, 60 + 56, 10, 20, st_panel.fillColor);
      Lcd.fillRect(275 + 20, 55 + 56, 10, 25, st_panel.fillColor);
    }
    if (st_panel.st_data[2] == 0)
    {
      this->strip_brightness = 0;
      if (m.cursor == 2)
      {
        Lcd.fillRect(230 + 20, 70 + 56 + 56, 10, 10, st_panel.selFillColor);
        Lcd.fillRect(245 + 20, 65 + 56 + 56, 10, 15, st_panel.selFillColor);
        Lcd.fillRect(260 + 20, 60 + 56 + 56, 10, 20, st_panel.selFillColor);
        Lcd.fillRect(275 + 20, 55 + 56 + 56, 10, 25, st_panel.selFillColor);
      }
      else
      {
        Lcd.fillRect(230 + 20, 70 + 56 + 56, 10, 10, backgroundColor);
        Lcd.fillRect(245 + 20, 65 + 56 + 56, 10, 15, backgroundColor);
        Lcd.fillRect(260 + 20, 60 + 56 + 56, 10, 20, backgroundColor);
        Lcd.fillRect(275 + 20, 55 + 56 + 56, 10, 25, backgroundColor);
      }
    }
    else if (st_panel.st_data[2] == 1)
    {
      this->strip_brightness = 10;
      Lcd.fillRect(230 + 20, 70 + 56 + 56, 10, 10, st_panel.fillColor);
      if (m.cursor == 2)
      {
        Lcd.fillRect(245 + 20, 65 + 56 + 56, 10, 15, st_panel.selFillColor);
        Lcd.fillRect(260 + 20, 60 + 56 + 56, 10, 20, st_panel.selFillColor);
        Lcd.fillRect(275 + 20, 55 + 56 + 56, 10, 25, st_panel.selFillColor);
      }
      else
      {
        Lcd.fillRect(245 + 20, 65 + 56 + 56, 10, 15, backgroundColor);
        Lcd.fillRect(260 + 20, 60 + 56 + 56, 10, 20, backgroundColor);
        Lcd.fillRect(275 + 20, 55 + 56 + 56, 10, 25, backgroundColor);
      }
    }
    else if (st_panel.st_data[2] == 2) //default
    {
      this->strip_brightness = 80;
      Lcd.fillRect(230 + 20, 70 + 56 + 56, 10, 10, st_panel.fillColor);
      Lcd.fillRect(245 + 20, 65 + 56 + 56, 10, 15, st_panel.fillColor);
    }
    else if (st_panel.st_data[2] == 3)
    {
      this->strip_brightness = 160;
      Lcd.fillRect(230 + 20, 70 + 56 + 56, 10, 10, st_panel.fillColor);
      Lcd.fillRect(245 + 20, 65 + 56 + 56, 10, 15, st_panel.fillColor);
      Lcd.fillRect(260 + 20, 60 + 56 + 56, 10, 20, st_panel.fillColor);
    }
    else if (st_panel.st_data[2] == 4) //max
    {
      this->strip_brightness = 255;
      Lcd.fillRect(230 + 20, 70 + 56 + 56, 10, 10, st_panel.fillColor);
      Lcd.fillRect(245 + 20, 65 + 56 + 56, 10, 15, st_panel.fillColor);
      Lcd.fillRect(260 + 20, 60 + 56 + 56, 10, 20, st_panel.fillColor);
      Lcd.fillRect(275 + 20, 55 + 56 + 56, 10, 25, st_panel.fillColor);
    }
    st_panel.change = false;
    if(st_panel.change2)
      this->st_panel.change2 = false;
  }
}

void UI::menu_disp()
{
  //battery indicator
  batteryUpdate();
  //box
  if (m.fillColor != BLACK)
  {
    if (m.leftBtn_visible)
      Lcd.fillRoundRect(0, 219, 105, 19, 8, m.fillColor);
    if (m.midBtn_visible)
      Lcd.fillRoundRect(107, 219, 105, 19, 8, m.fillColor);
    if (m.rightBtn_visible)
      Lcd.fillRoundRect(214, 219, 105, 19, 8, m.fillColor);
  }
  if (m.leftBtn_visible)
    Lcd.drawRoundRect(0, 219, 105, 19, 8, m.lineColor);
  if (m.midBtn_visible)
    Lcd.drawRoundRect(107, 219, 105, 19, 8, m.lineColor);
  if (m.rightBtn_visible)
    Lcd.drawRoundRect(214, 219, 105, 19, 8, m.lineColor);
  //text
  if (m.leftBtn_visible && m.leftBtn_enable)
    Lcd.setTextColor(m.lineColor);
  else
    Lcd.setTextColor(backgroundColor);
  Lcd.setTextSize(2);
  Lcd.setCursor(5, 221);
  Lcd.print(m.leftText);

  if (m.midBtn_visible && m.midBtn_enable)
    Lcd.setTextColor(m.lineColor);
  else
    Lcd.setTextColor(backgroundColor);
  Lcd.setTextSize(2);
  Lcd.setCursor(112, 221);
  Lcd.print(m.midText);

  if (m.rightBtn_visible && m.rightBtn_enable)
    Lcd.setTextColor(m.lineColor);
  else
    Lcd.setTextColor(backgroundColor);
  Lcd.setTextSize(2);
  Lcd.setCursor(219, 221);
  Lcd.print(m.rightText);
}

void UI::batteryUpdate()
{
  Lcd.drawRoundRect(270, 8, 38, 17, 5, m.battFillColor);
  uint8_t currentLevel = power.getBatteryLevel();

  if (currentLevel > 100)
    currentLevel = 100;
  if (power.isCharging())
  {
    this->m.battFillColor = m.charingBattFillColor;
    if (revert)
    {
      this->m.lastLevel = -1;
      this->revert = false;
    }
  }
  else if (currentLevel <= 25)
  {
    this->m.battFillColor = m.lowBattFillColor;
  }
  else
  {
    this->m.battFillColor = m.defaultBattFillColor;
    this->revert = true;
  }
  if (currentLevel != m.lastLevel)
  {
    Lcd.fillRoundRect(272, 10, 34, 13, 3, m.fillColor);
    Lcd.fillRoundRect(272, 10, 34 * currentLevel / 100, 13, 3, m.battFillColor);
    this->m.lastLevel = currentLevel;
  }
}

/*Node initializing
          size:     number of node to be used
          default:  1
          max:      32
*/
void UI::node_init(uint8_t size)
{
  this->node_size = size;
  for (int i = 0; i < size; i++)
  {
    this->EN[i] = true;
    this->data[i] = 0;
  }
  this->c_panel.size = size;
}
/*set the title of node at specific index
  title_1st: Title on first Line
  title_2nd: Title on second Line
  default: no change
  charater limit: 12
  *** Index must not exceed init size
*/
void UI::node_setTitle(uint8_t index, String title_1st, String title_2nd)
{
  if (EN[index])
  {
    if (title_1st != "")
      this->title_1st[index] = title_1st;
    if (title_2nd != "")
      this->title_2nd[index] = title_2nd;
  }
}
/*set node type for different control
  FAN   -> 4 level control 
  AIR   -> on/off and temperature control
  LIGHT -> on/off
*/
void UI::node_setType(uint8_t index, uint8_t typeSelect)
{
  if (EN[index])
  {
    this->type[index] = typeSelect;
  }
}
/*set node title picture at specific index
  put png file in data folder and put the path here
  example: "/test1.png"

  base_path   -> path for off/default status picture
  hover_path  -> path for on/selected status picture

  *** 103 x 103 pixels image
*/
void UI::node_setTitlePic(uint8_t index, const char *base_path, const char *hover_path)
{
  if (EN[index])
  {
    this->titlePic[index] = (char *)base_path;
    this->titlePic_Hover[index] = (char *)hover_path;
  }
}

//set node title color at specific index
void UI::node_setTitleColor(uint8_t index, uint16_t title_1st, uint16_t title_2nd)
{
  if (EN[index])
  {
    this->title1stColor[index] = title_1st;
    this->title2ndColor[index] = title_2nd;
  }
}

//set all node title color
void UI::node_setAllTitleColor(uint16_t title_1st, uint16_t title_2nd)
{
  for (int i = 0; i < c_panel.size; i++)
  {
    this->title1stColor[i] = title_1st;
    this->title2ndColor[i] = title_2nd;
  }
}

//set wifi ssid
void UI::set_wifi_ssid(char *ssid)
{
  this->wifi_ssid = ssid;
}
//set wifi status
void UI::set_wifi_status(bool status)
{
  if (status)
    this->wifi_status = "Connected";
  else
    this->wifi_status = "Disconnected";
}
//set mqtt ip address
void UI::set_mqtt_ip(char *ip)
{
  this->mqtt_ip = ip;
}
//set mqtt status
void UI::set_mqtt_status(bool status)
{
  if (status)
    this->mqtt_status = "Connected";
  else
    this->mqtt_status = "Disconnected";
}
//set temperature
void UI::set_temp(float temp)
{
  this->temp = temp;
}
//set humidity
void UI::set_humid(float humid)
{
  this->humid = humid;
}
//set pressure
void UI::set_pa(float pressure)
{
  this->pa = pressure;
}
//set pir value
void UI::set_motion(int motion)
{
  this->motion = motion;
}

//get Method
uint8_t UI::get_node_data(int index)
{
  return data[index];
}
int8_t UI::get_node_temp(int index)
{
  return temp_data[index];
}

void UI::set_node_data(int index, uint8_t newData)
{
  this->data[index] = newData;
  if (index == c_panel.index && last_data[index] != newData && type[index] == AIR)
  {
    this->c_panel.lastIndex = -1;
    this->last_temp_data[index] = -1;
  }
  else if (type[index] == FAN && newData == 0 && last_data[index] != 0)
  {
    this->c_panel.lastIndex = -1;
  }
  else if (type[index] == FAN && newData != 0 && last_data[index] == 0)
  {
    this->c_panel.lastIndex = -1;
  }
  else if (type[index] == LIGHT && newData != last_data[index] && index == c_panel.index)
  {
    this->c_panel.lastIndex = -1;
  }
}
void UI::set_node_temp(int index, uint8_t newTemp)
{
  this->temp_data[index] = newTemp;
}

uint8_t UI::get_strip_data()
{
  return st_panel.st_data[1];
}