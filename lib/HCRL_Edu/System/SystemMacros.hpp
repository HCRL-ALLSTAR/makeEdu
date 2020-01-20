#ifndef System_Macros
#define System_Macros

#define delay_Time 200
#define TaskDelay(Time) (delay(Time / portTICK_PERIOD_MS))
#define Sprintln(payload) (Serial.println(payload))
#define Sprint(payload) (Serial.print(payload))
#endif