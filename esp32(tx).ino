#include <WiFi.h>
#include <esp_now.h>

uint8_t receiverMAC[] = {0xA4,0xCF,0x12,0xED,0xC9,0xB7};

typedef struct {
  int x1;
  int y1;
  int x2;
  int y2;
} Data;

Data data;

class Kalman {
  public:
  float err_measure = 2;
  float err_estimate = 2;
  float q = 0.01;
  float current_estimate = 0;
  float last_estimate = 0;
  float kalman_gain = 0;

  float updateEstimate(float mea){
    kalman_gain = err_estimate/(err_estimate + err_measure);
    current_estimate = last_estimate + kalman_gain*(mea-last_estimate);
    err_estimate = (1.0 - kalman_gain)*err_estimate + fabs(last_estimate-current_estimate)*q;
    last_estimate = current_estimate;
    return current_estimate;
  }
};

Kalman kx1,ky1,kx2,ky2;

void setup(){

Serial.begin(115200);

WiFi.mode(WIFI_STA);

esp_now_init();

esp_now_peer_info_t peerInfo={};
memcpy(peerInfo.peer_addr,receiverMAC,6);
peerInfo.channel=0;
peerInfo.encrypt=false;

esp_now_add_peer(&peerInfo);

}

void loop(){

int j1x = analogRead(32);
int j1y = analogRead(33);
int j2x = analogRead(34);
int j2y = analogRead(35);

data.x1 = kx1.updateEstimate(j1x);
data.y1 = ky1.updateEstimate(j1y);
data.x2 = kx2.updateEstimate(j2x);
data.y2 = ky2.updateEstimate(j2y);

esp_now_send(receiverMAC,(uint8_t*)&data,sizeof(data));

delay(20);
}
