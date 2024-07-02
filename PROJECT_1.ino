

/*  GHI CHÚ CHÂN KẾT NỐI TRÊN ESP8266
Relay. D3              relay máy bơm
Btn.   D7                   
Soil.  A0               cảm biến độ ẩm đất
Rain.   D5               cảm biến mưa
SDA.   D2                        2 chân LCD I2C
SCL.   D1
Temp.  D4                          cảm biến DHT11
*/

// Chèn Thư viện sử dụng
#include <LiquidCrystal_I2C.h> 
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

//  thiết lập LCD I2C
LiquidCrystal_I2C lcd(0x3F, 16, 2);



char auth[] = "37urfWC3cvSTuF2gg2WGuzbccclqwgOC";  //mã ĐỒ ÁN 1 trên Blynk( Auth token)
char ssid[] = "nn";  // WIFI SSID
char pass[] = "11112222";  // WIFI Password

DHT dht(D4, DHT11);//( chân kết nối ,loại cảm biến)  D4 DHT11 cảm biến nhiệt độ , độ ẩm
BlynkTimer timer;

//Định nghĩa các chân cảm biến 
#define soil A0     //A0 là cảm biến đất
#define R D5      //D5 là cẩm biến mưa
int R_ToggleValue;
void checkPhysicalButton();
int relay1State = LOW;
int pushButton1State = HIGH;
#define RELAY_PIN_1       D3   //D3 relay
#define PUSH_BUTTON_1     D7   //D7 nút nhấn
#define VPIN_BUTTON_1    V12    // Bơm

//tạo biến lưu giá trị
double T, P;
char status;



void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  pinMode(R, INPUT);

 pinMode(RELAY_PIN_1, OUTPUT);
 digitalWrite(RELAY_PIN_1, LOW);
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);
  digitalWrite(RELAY_PIN_1, relay1State);


  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);     // khởi động blynk
  dht.begin();

  lcd.setCursor(0, 0);
  lcd.print("...DO_AN_1...    ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(11, 1);
  lcd.print("W:OFF");
  //Call the function
  timer.setInterval(100L, soilMoistureSensor);
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(500L, checkPhysicalButton);
}


//Hàm cảm biến DHT11
void DHT11sensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

}


//Hàm cảm biến độ ẩm đất
void soilMoistureSensor() {

int value = analogRead(soil);
  value = map(value, 1024, 0, 0, 100);
 
  Blynk.virtualWrite(V3,value);
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(value);
  lcd.print(" ");

}


//Hàm cảm biến mưa
void Rsensor() {
  bool value = digitalRead(R);
  if (!value) {
    Blynk.logEvent("pirmotion","WARNNG! RAINING!"); // khi có mưa sẽ cảnh báo điện thoại
    WidgetLED LED(V5);
     
 LED.on();
  } else {
    WidgetLED LED(V5);
    
    LED.off();
  }  
  }

BLYNK_WRITE(V6)
{
 R_ToggleValue = param.asInt();  
}


BLYNK_CONNECTED() {
  // yêu cầu trạng thái mới nhất từ ​​máy chủ
  Blynk.syncVirtual(VPIN_BUTTON_1);
}

BLYNK_WRITE(VPIN_BUTTON_1) {
  relay1State = param.asInt();
  digitalWrite(RELAY_PIN_1, relay1State);
}
// Hàm kiểm tra nút nhấn
void checkPhysicalButton()
{
  if (digitalRead(PUSH_BUTTON_1) == LOW) {
    // nếu chân D7 đang mức thấp
    if (pushButton1State != LOW) {

      // kiểm tra relay
      relay1State = !relay1State;
      digitalWrite(RELAY_PIN_1, relay1State);

      // cập nhật nút nhấn trên blynk
      Blynk.virtualWrite(VPIN_BUTTON_1, relay1State);
    }
    pushButton1State = LOW;
  } else {
    pushButton1State = HIGH;
  }
}


void loop() {
  Blynk.run();// chạy thư viện blynk
  timer.run();// chạy timer blynk




    if (R_ToggleValue)
    {
    lcd.setCursor(5, 1);
    lcd.print("R:ON ");
      Rsensor();
      }
     else
     {
    lcd.setCursor(5, 1);
    lcd.print("R:OFF");
    WidgetLED LED(V5);
    LED.off();
     }

if (relay1State == HIGH)
{
  lcd.setCursor(11, 1);
  lcd.print("W:ON ");
  }
  else if (relay1State == LOW)
  {
    lcd.setCursor(11, 1);
    lcd.print("W:OFF");
    }
     
      

  }
