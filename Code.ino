#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>

const char* ssid = "your_SSID";
const char* password = "your_PASSWORD";

WebServer server(80);

bool doorOpen = false; // Variable to track door state

void setup() {
  Serial.begin(115200);
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Print the IP address
  Serial.println(WiFi.localIP());

  // Initialize pins for LEDs
  pinMode(LED_BUILTIN, OUTPUT); // Default LED for demo
  pinMode(2, OUTPUT); // Fan LED
  pinMode(3, OUTPUT); // TV LED
  pinMode(4, OUTPUT); // Light LED
  pinMode(5, OUTPUT); // Door LED

  // Initial states
  digitalWrite(LED_BUILTIN, LOW); // Default LED off
  digitalWrite(2, LOW); // Fan LED off
  digitalWrite(3, LOW); // TV LED off
  digitalWrite(4, LOW); // Light LED off
  digitalWrite(5, LOW); // Door LED off

  // Route for root / web page
  server.on("/", HTTP_GET, [](){
    String html = "<html><head><style>body { font-family: Arial, Helvetica, sans-serif; }</style></head>";
    html += "<body><h1>Home Control</h1>";

    // Fan LED
    html += "<h2>Fan</h2><button onclick=\"toggleLED(2)\">Toggle Fan</button><br><br>";
    
    // TV LED
    html += "<h2>TV</h2><button onclick=\"toggleLED(3)\">Toggle TV</button><br><br>";
    
    // Light LED
    html += "<h2>Light</h2><button onclick=\"toggleLED(4)\">Toggle Light</button><br><br>";
    
    // Door LED (Open/Close)
    html += "<h2>Door</h2>";
    html += "<p>Status: <span id='doorStatus'>" + String(doorOpen ? "Open" : "Closed") + "</span></p>";
    html += "<button onclick=\"toggleDoor()\">Toggle Door</button><br><br>";

    // JavaScript functions
    html += "<script>";
    html += "function toggleLED(pin) { fetch('/led/' + pin, { method: 'POST' }); }";
    html += "function toggleDoor() { fetch('/door', { method: 'POST' }); }";
    html += "</script>";

    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  // Route to handle LED toggling
  server.on("/led/<pin>", HTTP_POST, [](){
    int pin = server.pathArg("pin").toInt();
    digitalWrite(pin, !digitalRead(pin));
    server.send(200, "text/plain", "LED toggled");
  });

  // Route to handle door toggling
  server.on("/door", HTTP_POST, [](){
    doorOpen = !doorOpen;
    digitalWrite(5, doorOpen ? HIGH : LOW); // Toggle door LED
    server.send(200, "text/plain", doorOpen ? "Open" : "Closed");
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
