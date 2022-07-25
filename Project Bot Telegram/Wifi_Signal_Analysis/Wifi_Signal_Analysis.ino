//#include <TelegramCertificate.h>
//#include <UniversalTelegramBot.h>


#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Initialize Wifi connection to the router
char ssid[] = "RUMAH THERAPY DETOKS";     // diisi nama wifi
char password[] = "buangdarah"; // diisi password wifi

// Initialize Telegram BOT
#define BOTtoken "5464991538:AAFgYACBBV1cx77Ws9pzw5FIN9WRn55CESE" // diisi Token Bot (Dapat dari Telegram Botfather)

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

//Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

int LEDWIFI = 2;

String SSIDWIFI[100],RSSIWIFI[100],TOTALSSIDRSSI,SIGNALINDICATOR;


void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";
        
    //Cek Command untuk setiap aksi
    if (text == "/MulaiScanWifi") {  
      String scanning = "Proses scan sedang berlangsung tunggu beberapa saat.\n";
      bot.sendMessage(chat_id, scanning, "Markdown");
      STAMODE();
      APCLIENT();
      bot.sendMessage(chat_id, TOTALSSIDRSSI, "Markdown");
      TOTALSSIDRSSI = "";
    }
  }
}

void STAMODE(){
  //mode scan wifi
  // Set WiFi to station mode and disconnect from an AP if it was Previously
  // connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    TOTALSSIDRSSI += " Jumlah Wifi Terdeteksi : " + String(n) + "\n";
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
//      Serial.print(WiFi.SSID(i));
      SSIDWIFI[i] = WiFi.SSID(i);
      Serial.print(SSIDWIFI[i]);
      Serial.print(" (");
//      Serial.print(WiFi.RSSI(i));
      RSSIWIFI[i] = String(WiFi.RSSI(i));
      Serial.print(RSSIWIFI[i]);
      Serial.print(")");
      if((WiFi.RSSI(i)>=-57)&&(WiFi.RSSI(i)<=-10)){
        SIGNALINDICATOR = "EXCELLENT";
      }
      if((WiFi.RSSI(i)>=-75)&&(WiFi.RSSI(i)<=-58)){
        SIGNALINDICATOR = "GOOD";
      }
      if((WiFi.RSSI(i)>=-85)&&(WiFi.RSSI(i)<=-76)){
        SIGNALINDICATOR = "FAIR";
      }
      if((WiFi.RSSI(i)>=-95)&&(WiFi.RSSI(i)<=-86)){
        SIGNALINDICATOR = "POOR";
      }
//      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      TOTALSSIDRSSI += String(i+1) +". Nama Wifi             : " + SSIDWIFI[i] + "\n    Kuat Sinyal(RSSI) : " + RSSIWIFI[i] + "dBm\n" + "    Kategori                 : "+SIGNALINDICATOR +"\n\n";
      delay(10);
    }
//    SSIDWIFI = WiFi.SSID();
  }
  Serial.println("");
}

void APCLIENT(){
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(LEDWIFI, LOW);
    delay(500);
    digitalWrite(LEDWIFI, HIGH);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  String IP = WiFi.localIP().toString();
}


void setup() {
//  EEPROM.begin(512);
  pinMode(LEDWIFI, OUTPUT);
  digitalWrite(LEDWIFI, LOW);
  
  Serial.begin(115200);

  client.setInsecure();
  APCLIENT();  
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    lastTimeBotRan = millis();
  }
}
