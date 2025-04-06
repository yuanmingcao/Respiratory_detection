#if 0   //该成1，并copy到main.cpp中   这是标准版

#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"
#include <Arduino.h>
#include "ADS1X15.h"
#include <Wire.h>
const char *ssid = "HUAWEI-XGH3MH_HiLink";  //WIFI名字
const char *password = "suzhou57#2308";    
AsyncWebServer server(80);

const int Led_pin = 4;
const int analogInPin_35 = 35; // 设置模拟输入引脚
int f_date = 0, s_date = 0, d_date = 0, voltage = 0;
using namespace ADS1X15;
ADS1015<TwoWire> ads(Wire);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>test</title>
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script> 
<body>
<div style="display: flex; flex-direction: column; justify-content: center; align-items: center; width: 500px; height: 800px; margin: 0 auto; border: 1px solid #ccc; padding: 10px;">
<div style="display: flex; 
			justify-content: center; 
			align-items: center; 
			width: 100%;
			height: 5%;">
  <h1>Smart Mask</h1>
</div>

<div style="display: flex; 
			justify-content: center; 
			align-items: center; 
			width: 100%;
			height: 35%;
			margin-left: 35px
			">
      <img src="https://imgos.cn/2024/08/10/66b77c6b84690.png" alt="Description" style="width: 45%; height: 80%; margin-bottom: 5px;"/>
	  
	  <div style="display: flex; 
			flex-direction: column; /* 修改为垂直分布 */
			justify-content: center; 
			align-items: center; 
			width: 150%;
			height: 35%;">
		<div style="display: flex; 
			justify-content: center; 
			align-items: center; 
			width: 80%;
			height: 120%;
			margin-left: 35px;
			">
			<img src="https://imgos.cn/2024/08/10/66b77bdc99a94.png" alt="Description" style="width: 60%; height: 80%; margin-bottom: 5px; "/>
			<div style="display: flex; 
						flex-direction: column;
						justify-content: center; 
						/*align-items: center;  */
						width: 100%;
						height: 50%;
						margin-bottom: 10px;
						">
				<h3 style="margin: 0; padding: 0;">APnea</h3> <!-- 移除了默认的外边距 -->
				<h3 style="margin: 0; padding: 0;"id="times">00 times</h3> <!-- 移除了默认的外边距 -->

			</div>
		</div>
		<div style="display: flex; 
			justify-content: center; 
			align-items: center; 
			width: 80%;
			height: 120%;
			margin-right: 35px;
			">
			<img src="https://imgos.cn/2024/08/10/66b77c4547caa.png" alt="Description" style="width: 60%; height: 70%; margin-bottom: 5px;"/>
			<div style="display: flex; 
						flex-direction: column;
						justify-content: center; 
						/*align-items: center; */
						width: 100%;
						height: 50%;
						margin-bottom: 10px;
						">
				<h3 style="margin: 0; padding: 0;">Period</h3>
				<h3 style="margin: 0; padding: 0;"id="time">0.0 second</h3>
			</div>
		</div>
    </div>
</div>
<div style="display: flex; 
			justify-content: center; 
			align-items: center; 
			height: 10%; 
			width: 100%;">
      <img src="https://imgos.cn/2024/08/10/66b77bdd24055.png" alt="Description" style="width: 10%; height: 85%; margin-right: 35px;"/>
      <img src="https://imgos.cn/2024/08/10/66b77bdda64d9.png" alt="Description" style="width: 10%; height: 85%; margin-right: 35px;"/>
      <img src="https://imgos.cn/2024/08/10/66b77bddac280.png" alt="Description" style="width: 10%; height: 85%; margin-right: 35px;"/>
      <img src="https://imgos.cn/2024/08/10/66b77be266ac9.png" alt="Description" style="width: 10%; height: 85%; margin-right: 35px;"/>
      <img src="https://imgos.cn/2024/08/10/66b77be25e790.png" alt="Description" style="width: 10%; height: 85%; margin-right: 5px; "/>
    </div>
    <canvas id="myChart1" width="450" height="200"></canvas>
    <canvas id="myChart2" width="450" height="200"></canvas>
</div>
  <script>
    // 创建第一个图表
    const ctx1 = document.getElementById('myChart1').getContext('2d');
    const chart1 = new Chart(ctx1, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          label: 'Sensor Data 1',
          data: [],
          fill: false,
          borderColor: 'rgb(255, 99, 132)',
          tension: 0.1
        }]
      },
      options: {
        scales: {
          y: {
            beginAtZero: true,
            ticks: {
              min: 0,
              max: 250,
              stepSize: 5
            }
          }
        }
      }
    });

    // 创建第二个图表
    const ctx2 = document.getElementById('myChart2').getContext('2d');
    const chart2 = new Chart(ctx2, {
      type: 'line',
      data: {
        labels: [],
        datasets: [{
          label: 'Sensor Data 2',
          data: [],
          fill: false,
          borderColor: 'rgb(54, 162, 235)',
          tension: 0.1
        }]
      },
      options: {
        scales: {
          y: {
            beginAtZero: true,
            ticks: {
              min: 0,
              max: 250,
              stepSize: 5
            }
          }
        }
      }
    });

    function fetchData(index) {
 
      return function() {
        const sensorUrl = `http://192.168.3.13:80/sensor${index + 1}`;  /* 这里也要改网关 */
        return fetch(sensorUrl)
          .then(response => response.text())
          .then(data => {
            if(index!=2){
              const chart = index === 0 ? chart1 : chart2;
              const labels = chart.data.labels;
              const datasets = chart.data.datasets;

            // 确保数据点数组不超过200个
              while (labels.length > 80) {
                labels.shift(); // 删除最旧的标签
                datasets[0].data.shift(); // 删除最旧的数据点
              }

            // 添加新的标签和数据点
              labels.push(new Date().toLocaleTimeString());
              datasets[0].data.push(parseFloat(data));
              chart.update();
            }
            else if(index==2){
              document.getElementById("times").textContent = data + ' times'; 
            }

          })
          .catch(error => console.error(error));
      };
    }
    setInterval(fetchData(0), 100); // 传感器1  100ms
    setInterval(fetchData(1), 100); // 传感器2  100ms
    setInterval(fetchData(2), 1000); // 传感器2 1s
  </script>
</body>
)rawliteral";
int batteryValue = 0;
int ledValue = 0, HX_flag = 0, HX_start_time = 0, HX_time = 0;
int ADC_flag = 0;
int adc0 = 100;
int adc1;
int adcThreshold = 40;
int hx_begin = 0, num = 0;
float volts0;
float volts1;
unsigned long previousMillis = 0; // 保存上一次检查时间的变量
const long interval = 5000;       // 2秒的时间间隔
bool ledState = LOW;              // LED的状态
int time_tick = 0;
IPAddress staticIP(192, 168, 3, 13);   //先cmd IPCONFIG查网关   改第三个数值  这是网址  
IPAddress gateway(192, 168, 3, 1);   //先cmd IPCONFIG查网关   改第三个数值  这是网址 
IPAddress subnet(255, 255, 255, 0);
IPAddress dns(8, 8, 8, 8);

void setup()
{
  pinMode(Led_pin, OUTPUT); // 设置LED引脚为输出模式
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send_P(200, "text/html", index_html); });

  server.on("/sensor1", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    int sensorValue = adc1-100;
    request->send(200, "text/plain", String(sensorValue)); });
  server.on("/sensor2", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    int sensorValue = adc0-100;

    request->send(200, "text/plain", String(sensorValue)); });
  server.on("/sensor3", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    request->send(200, "text/plain", String(num));} );
  server.begin();
  ads.begin();
  ads.setGain(Gain::TWOTHIRDS_6144MV);
  ads.setDataRate(Rate::ADS1015_250SPS);
  delay(5000);
  HX_start_time = millis();
}
int time_start = 0;
int time_end = 0;
int breathCount = 0;                           // 呼吸次数计数器
unsigned long lastBreathTime = 0;              // 记录上一次检测到呼吸的时间
unsigned long breathIntervalThreshold = 1000;  // 呼吸间隔的阈值，单位为毫秒
unsigned long breathDetectionThreshold = 10.0; // 呼吸检测的阈值，根据你的湿度传感器进行调整
void loop()
{

  adc0 = ads.readADCSingleEnded(0);
  volts0 = ads.computeVolts(adc0);
  adc1 = ads.readADCSingleEnded(1);
  volts1 = ads.computeVolts(adc1);
  Serial.print(0);
  Serial.print(",");
  Serial.print(500);
  Serial.print(",");
  Serial.print(adc0 - 100);
  Serial.print(",");
  Serial.print(HX_time);
  Serial.print(",");
  
  Serial.println(adc1);
  if (hx_begin >= 1 && (adc0 - 100) > 0)
  {
    unsigned long currentMillis = millis(); // 获取当前时间
    if (hx_begin == 1)
    {
      previousMillis = currentMillis;
      hx_begin = 2;
      Serial.println("开始计时");
    }
    else
    {
      if (currentMillis - previousMillis >= interval)
      {
        digitalWrite(Led_pin, ledState);
        ledState = !ledState; // 切换LED状态
        Serial.println("检测不到呼吸");
        
      }
    }
  }
  else
  {
    // HX_times++;
  }
  if(HX_flag==1&&(adc0 - 100) > 0){
    HX_flag=0;
    num++;
    HX_time = (millis() - HX_start_time) / 1000;
  }
  if ((adc0 - 100) < 0)
  {
    HX_flag = 1;
    if (num > 5)
    {
      hx_begin = 1;
    }
  }
  delay(50); // 稍作延迟
}

#endif