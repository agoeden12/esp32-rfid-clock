#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <JC_Button.h>

#define onboard 2
#define CHECK_IN 13
#define CHECK_OUT 12

#define RFID_SDA 4
#define RFID_SCK 18
#define RFID_MOSI 23
#define RFID_MISO 19
#define RFID_RST 5

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_SCL 22
#define OLED_SDA 21

const char *ssid = ""; // Enter network SSID
const char *password = ""; // Enter network Password

const String api_url = "https://us-central1-evt-hours-api.cloudfunctions.net";

MFRC522 mfrc522(RFID_SDA, RFID_RST);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT);
Button check_in(CHECK_IN);
Button check_out(CHECK_OUT);
WiFiClient wifiClient;
HTTPClient httpClient;
static boolean displayState = false;
static byte displayTimeout = 0;

String readUid()
{
  int state = 0;
  while (state < 25)
  {
    delay(200);
    // Look for new cards
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
    {
      break;
    }
    state++;
  }
  if (state >= 25)
    return "";

  //Show UID on serial monitor
  String CardID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++)
  {
    CardID += mfrc522.uid.uidByte[i];
  }

  return CardID;
}

void sendPost(String uid, String endpoint)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("Sending");
  display.println("Request");
  display.display();

  String url = api_url + "/shop/" + endpoint + "?id=" + uid;
  httpClient.begin(url);
  httpClient.addHeader("Content-Length", "0");

  int httpResponseCode = httpClient.POST("");

  if (httpResponseCode == 201)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.println("Successful");
    display.display();
  }
  else
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.println("Failed");
    display.println("Try Again");
    display.display();
  }

  httpClient.end();
}

void checkIn()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("Check In");
  display.println("Scan Card");
  display.display();

  String uid = readUid();
  if (!uid.isEmpty())
  {
    sendPost(uid, "checkIn");
  }
  else
  {
    display.clearDisplay();
    display.display();
  }
}

void checkOut()
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.println("Check Out");
  display.println("Scan Card");
  display.display();

  String uid = readUid();
  if (!uid.isEmpty())
  {
    sendPost(uid, "checkOut");
  }
  else
  {
    display.clearDisplay();
    display.display();
  }
}

void connectToWifi()
{
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to Network.");
}

void setup()
{
  // Set Pinouts
  pinMode(onboard, OUTPUT);
  check_in.begin();
  check_out.begin();

  // Init Serial and SPI
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();

  // Init WIFI
  connectToWifi();

  // Init I2C Display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.display();

  //Finish setup and show with LED
  digitalWrite(onboard, HIGH);
}

void loop()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    connectToWifi();
  }

  if (displayState && displayTimeout > 25)
  {
    displayState = false;
    display.clearDisplay();
    display.display();
    digitalWrite(onboard, LOW);
  }

  check_in.read();
  check_out.read();

  if (displayState)
  {
    
      Serial.println("Display Active");
    if (check_in.wasPressed())
      checkIn();
    else if (check_out.wasPressed())
      checkOut();
    else
      displayTimeout++;
  }
  else if (!displayState && check_in.wasPressed() && check_out.wasPressed())
  {
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.println("Scan card");
    display.display();

    String uid = readUid();
    
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.setTextColor(WHITE);
    display.println(uid);
    display.display();

    delay(30000);
    display.clearDisplay();
    display.display();
  }
  else
  {
    if (check_in.wasPressed() || check_out.wasPressed())
    {
      Serial.println("Button Pressed");
      displayState = true;
      displayTimeout = 0;
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.setTextColor(WHITE);
      display.println("CheckIn  >>");
      display.println("-----------");
      display.println("-----------");
      display.println("CheckOut >>");
      display.display();
      digitalWrite(onboard, HIGH);
    }
  }

  delay(200);
}
