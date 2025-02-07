
#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>

WiFiClient wifiClient;

const char *ssid = "IoT";
const char *password = "68686868";

//Web/Server address to read/write from 
const char *host = "192.168.206.1";   

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
  WiFi.mode(WIFI_OFF);        //Ngăn chặn sự cố kết nối lại (mất quá nhiều thời gian để kết nối)
  delay(1000);
  WiFi.mode(WIFI_STA);        //Dòng này ẩn việc xem ESP là điểm phát wifi
  
  WiFi.begin(ssid, password);     //Kết nối với bộ định tuyến WiFi của bạn
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //Nếu kết nối thành công hiển thị địa chỉ IP trong màn hình nối tiếp
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Địa chỉ IP được gán cho ESP của bạn
  // đặt mã thiết lập của bạn ở đây, để chạy một lần:
  // Mở cổng Serial ở mức 9600
  Serial.begin(9600);
  // Dùng chân D1-GPIO5 dể làm cổng đọc digital, còn chân A0 vào thì không cần khai báo
  pinMode(ledr,OUTPUT);
  pinMode(ledy,OUTPUT);
  pinMode(ledg,OUTPUT);
  pinMode(relay,OUTPUT);
  pinMode(cbmua, INPUT);
}

void loop() {
  
  // Đặt mã chính của bạn ở đây để chạy lặp lại:
  // Dùng hàm for để cộng dồn 10 lần đọc giá trị của cảm biến, sau đó lấy trung bình để được giá trị chính xác nhất
  for(int i = 0; i <=9; i++){
    real_value += analogRead(A0);
  }
  value = real_value/10;
// Đặt lại giá trị 0 để cho vòng lặp mới 
  real_value = 0;
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
      digitalWrite(relay,LOW);
      Serial.println("Độ ẩm bình thường, máy bơm không hoạt động");
    }
  }

  Serial.print("Giá trị analog: ");
  Serial.print(value);
  Serial.print(" && ");
  Serial.print("Giá trị digital: ");
  Serial.println(digitalRead(5));
  delay(1000);

    HTTPClient http;    //Khai báo đối tượng của lớp HTTPClient

  String ADCData, station, maybom, postData;

  station = status_rain;
  if(digitalRead(relay) == LOW) maybom = "Máy bơm đang tắt";
  else maybom = "Máy bơm đang bật";

  if(status_rain == HIGH) station = "Trời không mưa";
  else station = "Trời đang mưa";

  // //Đăng dữ liệu
  postData = "doam=" + String(percent) + "&trangthaimua=" + station + "&trangthaibom=" + maybom;
  Serial.println(postData);
  http.begin(wifiClient, "http://192.168.206.1:81/iotproject/postdemo.php");              //Chỉ định đích yêu cầu
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");    //Chỉ định tiêu đề kiểu nội dung

  int httpCode = http.POST(postData);   //Gửi yêu cầu
  String payload = http.getString();    //Nhận tải trọng phản hồi

  Serial.println(httpCode);   //In mã trả về HTTP
  Serial.println(payload);    //In tải trọng phản hồi yêu cầu

  http.end();  //Đóng kết nối
  
  delay(3000);  //Đăng dữ liệu cứ sau 3 giây
}
