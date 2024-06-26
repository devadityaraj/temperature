#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <LiquidCrystal.h>
#include <DHT.h>
#define DHTPIN D2
#define DHTTYPE DHT11
#define WIFI_SSID "Galgotias-H"
#define WIFI_PASSWORD ""
#define API_KEY "AIzaSyBvXip_P6i4NSTpx03L2gQW6Nc0sWUKUH8"
#define DATABASE_URL "https://room-temp-esp-default-rtdb.firebaseio.com/"
#define USER_EMAIL "adityaraj94505@gmail.com"
#define USER_PASSWORD "hotandcold"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

DHT dht(DHTPIN, DHTTYPE);

//RS to D8 (GPIO15)
//E to D7 (GPIO13)
//D4 to D6 (GPIO12)
//D5 to D5 (GPIO14)
//D6 to D4 (GPIO2)
//D7 to D0 (GPIO16)
//VSS to Ground
//VDD to 5V (or 3.3V, depending on your LCD)
//RW to Ground
//Vo to the wiper of the potentiometer (connect the other two ends to VSS and VDD)

LiquidCrystal lcd(15, 13, 12, 14, 2, 16);

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.begin(16, 2);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(D3, OUTPUT); //RED LED 
  pinMode(D1, OUTPUT); //WIFI LED
  //Using 1k resisitor to common ground to decraese the brightness.
  lcd.setCursor(0, 0);
    lcd.print("   Aditya Raj   ");
  lcd.setCursor(0, 1);
  lcd.print("ESP Temp Server");
    delay(1000);
    lcd.clear();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(D1, HIGH);
    Serial.print(".");
    delay(300);
    lcd.setCursor(0, 0);
    lcd.print("Wait WIFI");
    digitalWrite(D1, LOW);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  lcd.clear();
  lcd.setCursor(0, 0);
    lcd.print(WiFi.localIP());
    lcd.setCursor(0,1);
    lcd.print(WiFi.SSID());
    delay(800);
  Serial.println();
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.token_status_callback = tokenStatusCallback; 
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  lcd.clear();
  lcd.print("Started");
    delay(700);
    lcd.clear();
}
void loop() {
  String databasePath = "/DHT";  
  digitalWrite(D3, LOW);
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  Serial.println(temperature);
  Serial.println(humidity);
  lcd.clear();
  lcd.setCursor(0, 0);
    lcd.print(temperature);
    lcd.print("C  ");
    lcd.print(humidity);
    lcd.print("%");
  if (!isnan(temperature) && !isnan(humidity)) {
    FirebaseJson json;
    json.set("temperature", temperature);
     digitalWrite(LED_BUILTIN, LOW);
     delay(100);
    json.set("humidity", humidity);
    lcd.setCursor(0, 1);
    lcd.print("    Updating    ");
    delay(200);
    Serial.print("Updating DHT Data... ");
     digitalWrite(LED_BUILTIN, HIGH);
    if (Firebase.RTDB.setJSON(&fbdo, databasePath.c_str(), &json)) {
      Serial.println("Data updated successfully.");
      lcd.clear();
      lcd.setCursor(0, 0);
    lcd.print(" Update Sucess! ");
    lcd.setCursor(0,1);
    lcd.print("    FIREBASE    ");
    } else {
      Serial.println(fbdo.errorReason());
      digitalWrite(D3, HIGH);
      lcd.setCursor(0, 1);
    lcd.print("Upload Error");
      
    }
  } else {
    Serial.println("Failed to read DHT data.");
    digitalWrite(D3, HIGH);
    lcd.setCursor(0, 0);
    lcd.clear();
    lcd.print("Sensor error!");
      }
  delay(1000); 
}
