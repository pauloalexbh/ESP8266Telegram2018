/*******************************************************************
 *  Based in the example cod by Brian Lough.                       *
 *  It uses Telegram and an PMX5700DP to read a wather colun       *
 *                                                                 *
 *  written by Paulo Campos                                        *
 *******************************************************************/
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Initialize Wifi connection to the router
char ssid[] = "XXXXXX";     // your network SSID (name)
char password[] = "YYYYYY"; // your network key

// Initialize Telegram BOT
#define BOTtoken "XXXXXXXXX:XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"  // your Bot Token (Get from Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

const int AnaPin = A0;
int AnaRead = 0;
int Percent = 0;
int sensorMin = 0;
int sensorMax = 1024;


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if (text == "/nivel") {
      AnaRead = analogRead(AnaPin);
      Percent = map(AnaRead, sensorMin, sensorMax, 0, 100);
      Serial.print("Leitura do sensor: ");
      Serial.print(AnaRead);
      Serial.println(".");
      Serial.print("Percentual de volume: ");
      Serial.print(Percent);
      Serial.println(" %.");
    }

    if (text == "/ReplyKeyboard") {
      String keyboardJson = "[[\"/nivel\", \"/start\"],[\"/InlineKeyboard\"]]";
      bot.sendMessageWithReplyKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson, true);
    }
    
    if (text == "/InlineKeyboard") {
      String keyboardJson = "[[\{ \"nivel\" : \"start\", \"url\" : \"https://www.google.com\" \} ]]";
      bot.sendMessageWithInlineKeyboard(chat_id, "Choose from one of the following options", "", keyboardJson);
    }

    if (text == "/start") {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Reply Keyboard Markup example.\n\n";
      welcome += "/nivel : to switch the Led ON\n";
      welcome += "/start : to switch the Led OFF\n";
      welcome += "/InlineKeyboard : Returns current status of LED\n";
      welcome += "/ReplyKeyboard : returns the reply keyboard\n";
      bot.sendMessage(chat_id, welcome, "Markdown");
    }
  }
}

void setup() {
  Serial.begin(115200);

  // Set WiFi to station mode and disconnect from an AP if it was Previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);

  // attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    Bot_lasttime = millis();
  }
}
