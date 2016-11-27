//Ugly suit, ESP8266 wifi hotspot suit thing.
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include "FastLED.h"
#define DATA_PIN    12
#define LED_TYPE    WS2811
#define COLOR_ORDER RGB
#define NUM_LEDS    50
#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120
FASTLED_USING_NAMESPACE
CRGB leds[NUM_LEDS];
const char WiFiAPPSK[] = "uglysuit12345";

/////////////////////
// Pin Definitions //
/////////////////////
const int LED_PIN = 2; // Thing's onboard, green LED
const int ANALOG_PIN = A0; // The only analog pin on the Thing
const int DIGITAL_PIN = 12; // Digital pin to be read

WiFiServer server(80);
void initHardware();
void setupWiFi();

void setup() 
{
  delay(1000); // 3 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

Serial.begin(115200);
server.begin();
  // Start the server
Serial.println("Server started");

// Print the IP address
Serial.print("Use this URL to connect: ");
Serial.print("http://");
Serial.print(WiFi.localIP());
Serial.println("/");
}

typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void loop() {
// Check if a client has connected
WiFiClient client = server.available();
//if (!client) {
//return;
//}

// Wait until the client sends some data
//Serial.println("new client");
if(client.available()){
// Read the first line of the request
String request = client.readStringUntil('\r');
Serial.println(request);
client.flush();

// Fire Ze fucking missiles!
if (request.indexOf("/LB=1") != -1) {
nextPattern(0);
}
if (request.indexOf("/LB=2") != -1) {
nextPattern(1);
}
if (request.indexOf("/LB=3") != -1) {
nextPattern(2);
}
if (request.indexOf("/LB=4") != -1) {
nextPattern(3);
}
if (request.indexOf("/LB=5") != -1) {
nextPattern(4);
}
if (request.indexOf("/LB=6") != -1) {
nextPattern(5);
}
if (request.indexOf("/LB=7") != -1) {
Serial.print("Firing 7");
}
if (request.indexOf("/LB=8") != -1) {
Serial.print("Firing 8");
}
// Return the response
client.println("HTTP/1.1 200 OK");
client.println("Content-Type: text/html");
client.println(""); // do not forget this one
client.println("<!DOCTYPE HTML>");
client.println("<html>");
client.println("<body style=\"background-color:green;\">");
client.println("<br><br>");

client.println("<center><font face=\"helvetica\"><a style=\"font-size:80px\"><b>Ugly Suit Control Panel &trade;</b><br></font></a>");
client.println("<button style=\"height:100px;width:240px\" type=\"button\" onClick=\"parent.location='/LB=1'\"><a style=\"font-size:40px\">Rainbow</a></button><br>");
client.println("<button style=\"height:100px;width:240px\" type=\"button\" onClick=\"parent.location='/LB=2'\"><a style=\"font-size:40px\">Glitter Rainbow</a></button><br>");
client.println("<button style=\"height:100px;width:240px\" type=\"button\" onClick=\"parent.location='/LB=3'\"><a style=\"font-size:40px\">Confetti</a></button><br>");
client.println("<button style=\"height:100px;width:240px\" type=\"button\" onClick=\"parent.location='/LB=4'\"><a style=\"font-size:40px\">Sinelon</a></button><br>");
client.println("<button style=\"height:100px;width:240px\" type=\"button\" onClick=\"parent.location='/LB=5'\"><a style=\"font-size:40px\">Juggle</a></button><br>");
client.println("<button style=\"height:100px;width:240px\" type=\"button\" onClick=\"parent.location='/LB=6'\"><a style=\"font-size:40px\">BPM</a></button><br>");
//client.println("<button style=\"height:160px;width:400px\" type=\"button\" onClick=\"parent.location='/LB=7'\"><a style=\"font-size:60px\">Bank 7 FIRE!</a></button><br>");
//client.println("<button style=\"height:160px;width:400px\" type=\"button\" onClick=\"parent.location='/LB=8'\"><a style=\"font-size:60px\">Bank 8 FIRE!</a></button><br></center>");
client.println("");
client.println("</body>");
client.println("</html>");

delay(1);
Serial.println("Client disonnected");
Serial.println("");
}

 gPatterns[gCurrentPatternNumber]();
  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}



#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern(int a)
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = a;
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
