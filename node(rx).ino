#include <ESP8266WiFi.h>
#include <espnow.h>

typedef struct {
  int x1;
  int y1;
  int x2;
  int y2;
} Data;

Data data;

// motor pins
#define ENA1 D5
#define IN1 D1
#define IN2 D2

#define ENA2 D6
#define IN3 D7
#define IN4 D8

#define ENA3 D4
#define IN5 D0
#define IN6 D3

void driveMotor(int en,int a,int b,int spd){

if(spd>20){
digitalWrite(a,HIGH);
digitalWrite(b,LOW);
}
else if(spd<-20){
digitalWrite(a,LOW);
digitalWrite(b,HIGH);
spd=-spd;
}
else{
analogWrite(en,0);
return;
}

spd=constrain(abs(spd),0,1023);
analogWrite(en,spd);
}

void OnDataRecv(uint8_t *mac,uint8_t *incomingData,uint8_t len){

memcpy(&data,incomingData,sizeof(data));

int m1 = data.x1 - 1900;
int m2 = data.y1 - 1900;
int m3 = data.x2 - 1900;

int rotate = data.y2 - 1900;

m1 += rotate;
m2 += rotate;
m3 += rotate;

Serial.print("M1 ");Serial.print(m1);
Serial.print(" M2 ");Serial.print(m2);
Serial.print(" M3 ");Serial.println(m3);

driveMotor(ENA1,IN1,IN2,m1);
driveMotor(ENA2,IN3,IN4,m2);
driveMotor(ENA3,IN5,IN6,m3);
}

void setup(){

Serial.begin(115200);

pinMode(IN1,OUTPUT);
pinMode(IN2,OUTPUT);
pinMode(IN3,OUTPUT);
pinMode(IN4,OUTPUT);
pinMode(IN5,OUTPUT);
pinMode(IN6,OUTPUT);

pinMode(ENA1,OUTPUT);
pinMode(ENA2,OUTPUT);
pinMode(ENA3,OUTPUT);

WiFi.mode(WIFI_STA);

esp_now_init();
esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
esp_now_register_recv_cb(OnDataRecv);

Serial.println("Receiver Ready");
}

void loop(){}
