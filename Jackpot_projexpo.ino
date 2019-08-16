#include <WiFi.h>

const int ledpin = 13; // pwm pins
const int ledpin1 = 12;
const int ledpin2 = 14;

const int gnd = 27; // ground pins
const int gnd1 = 26;
const int gnd2 = 25;


const int freq= 5000;

const int ledChannel=0; // pwm channels
const int ledChannel1=1;
const int ledChannel2=2;

const int resolution= 8;

// network credentials
const char* ssid     = "akshay";
const char* password = "nanomicro";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output2State = "off";

// Assign output variables to GPIO pins
const int output2 = 2;

void setup() {
  // put your setup code here, to run once:
ledcSetup(ledChannel,freq,resolution);
ledcAttachPin(ledpin,ledChannel);
ledcSetup(ledChannel1,freq,resolution);
ledcAttachPin(ledpin1,ledChannel1);
ledcSetup(ledChannel2,freq,resolution);
ledcAttachPin(ledpin2,ledChannel2);

randomSeed(analogRead(0));

Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output2, OUTPUT);
  pinMode(gnd,OUTPUT);
  pinMode(gnd1,OUTPUT);
  pinMode(gnd2,OUTPUT);
  
  // Set outputs to LOW
  digitalWrite(output2, LOW);
  digitalWrite(gnd, LOW);
  digitalWrite(gnd1, LOW);
  digitalWrite(gnd2, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}


void loop() {
   int a = random(85,255); // alloting random numbers for speed of 3 motors
   int b = random(85,255);
   int c = random(85,255);
  // put your main code here, to run repeatedly:
   WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // turns the GPIOs on and off
            if (header.indexOf("GET /2/on") >= 0) {
              Serial.println("GPIO 2 on");
              output2State = "on";
              ledcWrite(ledChannel,a); // running 3 motors at random speeds
              ledcWrite(ledChannel1,b);
              ledcWrite(ledChannel2,c);
              
              digitalWrite(output2, HIGH);
              delay(5000);

              ledcWrite(ledChannel,0); // automatic switch off
              ledcWrite(ledChannel1,0);
              ledcWrite(ledChannel2,0);
              
            } else if (header.indexOf("GET /2/off") >= 0) {
              Serial.println("GPIO 2 off");
              output2State = "off";
              ledcWrite(ledChannel,0);
              ledcWrite(ledChannel1,0);
              ledcWrite(ledChannel2,0);
              digitalWrite(output2, LOW);
            } 
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #b10c2d; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body bgcolor='black'><h1 style='color:#b10c2d;'>Jackpot!!!</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p style='color:#b10c2d;'>Press the button to spin your luck</p>");
            // If the output2State is off, it displays the ON button       
            if (output2State=="off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            } 
               
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }

}
