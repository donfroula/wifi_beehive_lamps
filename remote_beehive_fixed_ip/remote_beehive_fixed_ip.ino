#include <ESP8266WiFi.h>

const char* ssid = "wifissid";
const char* password = "wifipassword";

//Static IP address configuration
IPAddress staticIP(192, 168, 1, 234);  //ESP static ip
IPAddress gateway(192, 168, 1, 1);     //IP Address of your WiFi Router (Gateway)
IPAddress subnet(255, 255, 255, 0);    //Subnet mask
IPAddress dns(8, 8, 8, 8);             //DNS

int builtinled = D0;
int whiteled = D1; 
int greenled = D2; 
int redled = D5; 
int beeper = D6; 

int whitevalue = LOW;
int greenvalue = LOW;
int redvalue = LOW;

int period = 15000;
unsigned long time_now = 0;
  
WiFiServer server(80);
 
void setup() {
  Serial.begin(115200);
  delay(10);
  
  pinMode(redled, OUTPUT);
  digitalWrite(redled, HIGH);
  delay(500);
  digitalWrite(redled, LOW);
  
  pinMode(greenled, OUTPUT);
  digitalWrite(greenled, HIGH);
  delay(500);
  digitalWrite(greenled, LOW);

  pinMode(whiteled, OUTPUT);
  digitalWrite(whiteled, HIGH);
  delay(500);
  digitalWrite(whiteled, LOW);

  pinMode(beeper, OUTPUT);
  digitalWrite(beeper, LOW);
  tone(beeper, 1000, 200);
  delay(200);

  pinMode(builtinled, OUTPUT);
  digitalWrite(builtinled, HIGH);
 
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.disconnect();  //Prevent connecting to wifi based on previous configuration
  WiFi.config(staticIP, gateway, subnet, dns);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(redled, HIGH);
    delay(250);
    Serial.print(".");
    digitalWrite(redled, LOW);
    delay(250);
  }
  Serial.println("");
  Serial.println("WiFi connected");
 
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
 
}
 
void loop() {

//Heartbeat "OK" flashing LED and WIFI connection loss flashing LED
  if((millis() - time_now > period)){
     time_now = millis();
     if (WiFi.status() == WL_CONNECTED){
        if(greenvalue == LOW){digitalWrite(greenled, HIGH); delay(5); digitalWrite(greenled, LOW);}    
        if(greenvalue == HIGH){digitalWrite(greenled, LOW); delay(50); digitalWrite(greenled, HIGH);} 
        }
      if (WiFi.status() != WL_CONNECTED){
        if(redvalue == LOW){digitalWrite(redled, HIGH); tone(beeper, 100, 150); delay(100); digitalWrite(redled, LOW);}    
        if(redvalue == HIGH){digitalWrite(redled, LOW); tone(beeper, 100, 150); delay(100); digitalWrite(redled, HIGH);} 
        }
     }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  // Match the request
 
  //int redvalue = LOW;
  if (request.indexOf("/REDLED=ON") != -1)  {
    digitalWrite(redled, HIGH);
    tone(beeper, 800, 250);
    delay(250);
    redvalue = HIGH;
  }
  if (request.indexOf("/REDLED=OFF") != -1)  {
    digitalWrite(redled, LOW);
    tone(beeper, 800, 150);
    delay(150);
    redvalue = LOW;
  }

    //int greenvalue = LOW;
  if (request.indexOf("/GREENLED=ON") != -1)  {
    digitalWrite(greenled, HIGH);
    tone(beeper, 1200, 250);
    delay(250);
    greenvalue = HIGH;
  }
  if (request.indexOf("/GREENLED=OFF") != -1)  {
    digitalWrite(greenled, LOW);
    tone(beeper, 1200, 150);
    delay(150);
    greenvalue = LOW;
  }

    //int whitevalue = LOW;
  if (request.indexOf("/WHITELED=ON") != -1)  {
    digitalWrite(whiteled, HIGH);
    tone(beeper, 1400, 250);
    delay(250);
    whitevalue = HIGH;
  }
  if (request.indexOf("/WHITELED=OFF") != -1)  {
    digitalWrite(whiteled, LOW);
    tone(beeper, 1400, 150);
    delay(150);
    whitevalue = LOW;
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
 
  client.print("Red Led is now: ");
 
  if(redvalue == HIGH) {
    client.print("On");
  } else if(redvalue == LOW){
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/REDLED=ON\"\"><button>Red On </button></a>");
  client.println("<a href=\"/REDLED=OFF\"\"><button>Red Off </button></a><br />");  
  client.println("<br><br>");
  client.println("<br><br>");


client.print("Green Led is now: ");
 
  if(greenvalue == HIGH) {
    client.print("On");
  } else if(greenvalue == LOW){
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/GREENLED=ON\"\"><button>Green On </button></a>");
  client.println("<a href=\"/GREENLED=OFF\"\"><button>Green Off </button></a><br />");  
  client.println("<br><br>");
  client.println("<br><br>");


  client.print("White Led is now: ");
 
  if(whitevalue == HIGH) {
    client.print("On");
  } else if(whitevalue == LOW){
    client.print("Off");
  }
  client.println("<br><br>");
  client.println("<a href=\"/WHITELED=ON\"\"><button>White On </button></a>");
  client.println("<a href=\"/WHITELED=OFF\"\"><button>White Off </button></a><br />");  
  client.println("<br><br>");
  client.println("<br><br>");
  client.println("</html>");

 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
 
}
