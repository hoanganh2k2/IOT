
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

WiFiClient wifiClient;

const char *ssid = "IoT";
const char *password = "68686868";

// Cấu hình server 
const char *host = "192.168.206.1";   
const int   serverPort   = 81;  // Cổng của server
const char *postEndpoint = "/iotproject/postdemo.php";

//=======================================================================
//   =======================================================================
//                    MAIN
// =======================================================================
int value, real_value;
#define ledr D5
// Led Red into D5
#define ledg D6
// Led Green into D6
#define ledy D7
// Led Yelow into D7
#define relay D3
// Relay or máy bơm into D3
#define cbmua D2
// Cảm biến mưa into D2

void setup() {
  delay(1000);
  Serial.begin(9600);

  // Tắt chế độ Access Point và chuyển sang chế độ Station
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);

  // Kết nối tới WiFi
  WiFi.begin(ssid, password);
  Serial.println();
  Serial.print("Connecting");

  // Đợi kết nối
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Nếu kết nối thành công hiển thị địa chỉ IP trong màn hình nối tiếp
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Địa chỉ IP được gán cho ESP

  // Dùng chân D1-GPIO5 dể làm cổng đọc digital, còn chân A0 vào thì không cần khai báo
  pinMode(ledr,OUTPUT);
  pinMode(ledy,OUTPUT);
  pinMode(ledg,OUTPUT);
  pinMode(relay,OUTPUT);
  pinMode(cbmua, INPUT);
}

void loop() {
  real_value = 0;
  // Lấy giá trị trung bình của 10 lần để được giá trị chính xác nhất
  for(int i = 0; i <10; i++){
    real_value += analogRead(A0);
    delay(10);
  }
  value = real_value/10;

// Set thang giá trị đàu và giá trị cuối để đưa giá trị về thang từ 0 - 100.
  int percent = map(value, 432, 1023, 0, 100);
  // Tính giá trị phần trăm thực. Chuyển điện thế từ 3.3v (khô) thành 3.3v (ẩm)
  percent = 100 - percent;

  Serial.print("Độ ẩm là: ");
  Serial.print(percent);
  Serial.println('%');

  int status_rain = digitalRead(cbmua);

  if(status_rain == LOW){
    digitalWrite(ledr, LOW);
    digitalWrite(ledg, LOW);
    digitalWrite(ledy, HIGH);
    digitalWrite(relay,LOW);
    Serial.println("Trời đang mưa");
  }else{
    Serial.println("Trời không mưa");
    if(percent < 60){
      digitalWrite(ledr, HIGH);
      digitalWrite(ledg, LOW);
      digitalWrite(ledy, LOW);
      digitalWrite(relay,HIGH);
      Serial.println("Độ ẩm thấp, máy bơm đang hoạt động");
      //chạy
    }else if(percent > 80){
      // tắt 
      digitalWrite(ledr, LOW);
      digitalWrite(ledg, LOW);
      digitalWrite(ledy, HIGH);
      digitalWrite(relay,LOW);
      Serial.println("Độ ẩm cao, máy bơm không hoạt động");
    }else{
      digitalWrite(ledr, LOW);
      digitalWrite(ledg, HIGH);
      digitalWrite(ledy, LOW);
      if(digitalRead(relay) == LOW)
        Serial.println("Độ ẩm bình thường, máy bơm không hoạt động");
      else 
        Serial.println("Độ ẩm bình thường, máy bơm đang hoạt động");
    }
  }

  Serial.print("Giá trị analog: ");
  Serial.print(value);
  Serial.print(" && Giá trị digital (ledr): ");
  Serial.println(digitalRead(ledr));
  delay(1000);

  HTTPClient http;    //Khai báo đối tượng của lớp HTTPClient

  String ADCData, station, maybom, postData;

  // trạng thái máy bơm
  if(digitalRead(relay) == LOW) maybom = "Máy bơm đang tắt";
  else maybom = "Máy bơm đang bật";

  // trạng thái mưa
  if(status_rain == HIGH) station = "Trời không mưa";
  else station = "Trời đang mưa";

  // //Đăng dữ liệu
  postData = "doam=" + String(percent) + "&trangthaimua=" + station + "&trangthaibom=" + maybom;
  Serial.println(postData);

  String url = "http://" + host + ":" + String(serverPort) + postEndpoint; //http://192.168.206.1:81/iotproject/postdemo.php
  http.begin(wifiClient, url);
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");

  int httpCode = http.POST(postData);   //Gửi yêu cầu
  String payload = http.getString();    //Nhận tải trọng phản hồi

  Serial.print("HTTP code: ");
  Serial.println(httpCode);
  Serial.print("Response: ");
  Serial.println(payload);

  http.end();
  
  delay(3000);
}
