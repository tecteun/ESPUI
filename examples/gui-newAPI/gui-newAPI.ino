#include <DNSServer.h>
#include <ESPUI.h>

const byte DNS_PORT = 53;
IPAddress apIP( 192, 168, 1, 1 );
DNSServer dnsServer;

#if defined(ESP32)
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

// true for verbose, false for quiet
ESPUIClass ESPUI( Verbosity::VerboseJSON );

const char* ssid = "ESPUI";
const char* password = "espui";
const char* hostname = "EspuiTest";

void numberCall( Control* sender, int type ) {
  Serial.println( sender->value );
}

void textCall( Control* sender, int type ) {
  Serial.print("Text: ID: ");
  Serial.print(sender->id);
  Serial.print(", Value: ");
  Serial.println( sender->value );}

void slider( Control* sender, int type ) {
  Serial.print("Slider: ID: ");
  Serial.print(sender->id);
  Serial.print(", Value: ");
  Serial.println( sender->value );}

void buttonCallback( Control* sender, int type ) {
  switch ( type ) {
    case B_DOWN:
      Serial.println( "Button DOWN" );
      break;

    case B_UP:
      Serial.println( "Button UP" );
      break;
  }
}

void buttonExample( Control* sender, int type ) {
  switch ( type ) {
    case B_DOWN:
      Serial.println( "Status: Start" );
      ESPUI.updateControl( "Status:", "Start" );
      break;

    case B_UP:
      Serial.println( "Status: Stop" );
      ESPUI.updateControl( "Status:", "Stop" );
      break;
  }
}
void padExample( Control* sender, int value ) {
  switch ( value ) {
    case P_LEFT_DOWN:
      Serial.print( "left down" );
      break;

    case P_LEFT_UP:
      Serial.print( "left up" );
      break;

    case P_RIGHT_DOWN:
      Serial.print( "right down" );
      break;

    case P_RIGHT_UP:
      Serial.print( "right up" );
      break;

    case P_FOR_DOWN:
      Serial.print( "for down" );
      break;

    case P_FOR_UP:
      Serial.print( "for up" );
      break;

    case P_BACK_DOWN:
      Serial.print( "back down" );
      break;

    case P_BACK_UP:
      Serial.print( "back up" );
      break;

    case P_CENTER_DOWN:
      Serial.print( "center down" );
      break;

    case P_CENTER_UP:
      Serial.print( "center up" );
      break;
  }

  Serial.print( " " );
  Serial.println( sender->id );
}

void switchExample( Control* sender, int value ) {
  switch ( value ) {
    case S_ACTIVE:
      Serial.print( "Active:" );
      break;

    case S_INACTIVE:
      Serial.print( "Inactive" );
      break;
  }

  Serial.print( " " );
  Serial.println( sender.id );
}

void otherSwitchExample( Control* sender, int value ) {
  switch ( value ) {
    case S_ACTIVE:
      Serial.print( "Active:" );
      break;

    case S_INACTIVE:
      Serial.print( "Inactive" );
      break;
  }

  Serial.print( " " );
  Serial.println( sender->id );
}

void setup( void ) {
  Serial.begin( 115200 );

#if defined(ESP32)
  WiFi.setHostname( hostname );
#else
  WiFi.hostname( hostname );
#endif

  // try to connect to existing network
  WiFi.begin( ssid, password );
  Serial.print( "\n\nTry to connect to existing network" );

  {
    uint8_t timeout = 5;

    // Wait for connection, 2.5s timeout
    do {
      delay( 500 );
      Serial.print( "." );
      timeout--;
    } while ( timeout && WiFi.status() != WL_CONNECTED );

    // not connected -> create hotspot
    if ( WiFi.status() != WL_CONNECTED ) {
      Serial.print( "\n\nCreating hotspot" );

      WiFi.mode( WIFI_AP );
      WiFi.softAPConfig( apIP, apIP, IPAddress( 255, 255, 255, 0 ) );
      WiFi.softAP( ssid );

      timeout = 5;

      do {
        delay( 500 );
        Serial.print( "." );
        timeout--;
      } while ( timeout );
    }
  }

  dnsServer.start( DNS_PORT, "*", apIP );

  Serial.println( "\n\nWiFi parameters:" );
  Serial.print( "Mode: " );
  Serial.println( WiFi.getMode() == WIFI_AP ? "Station" : "Client" );
  Serial.print( "IP address: " );
  Serial.println( WiFi.getMode() == WIFI_AP ? WiFi.softAPIP() : WiFi.localIP() );

  ESPUI.addControl( ControlType::Label, "Status:", "Stop", ControlColor::Turquoise );
  ESPUI.addControl( ControlType::Text, "Text Test:", "a Text Field", ControlColor::Alizarin, Control::noParent, &textCall );
  ESPUI.addControl( ControlType::Label, "Millis:", "0", ControlColor::Emerald, Control::noParent );
  ESPUI.addControl( ControlType::Button, "Push Button", "Press", ControlColor::Peterriver, Control::noParent, &buttonCallback );
  ESPUI.addControl( ControlType::Button, "Other Button", "Press", ControlColor::Wetasphalt, Control::noParent, &buttonExample );
  ESPUI.addControl( ControlType::PadWithCenter, "Pad with center", "", ControlColor::Sunflower, Control::noParent, &padExample );
  ESPUI.addControl( ControlType::Pad, "Pad without center", "", ControlColor::Carrot, Control::noParent, &padExample );
  ESPUI.addControl( ControlType::Switcher, "Switch one", "", ControlColor::Alizarin, Control::noParent, &switchExample );
  ESPUI.addControl( ControlType::Switcher, "Switch two", "", ControlColor::None, Control::noParent, &otherSwitchExample );
  ESPUI.addControl( ControlType::Slider, "Slider one", "30", ControlColor::Alizarin, Control::noParent, &slider );
  ESPUI.addControl( ControlType::Slider, "Slider two", "100", ControlColor::Alizarin, Control::noParent, &slider );
  ESPUI.addControl( ControlType::Number, "Number:", "50", ControlColor::Alizarin, Control::noParent, &numberCall );

  /*
   * .begin loads and serves all files from PROGMEM directly.
   * If you want to serve the files from SPIFFS use ESPUI.beginSPIFFS
   * (.prepareFileSystem has to be run in an empty sketch before)
   */

  /*
   * Optionally you can use HTTP BasicAuth. Keep in mind that this is NOT a
   * SECURE way of limiting access.
   * Anyone who is able to sniff traffic will be able to intercept your password
   * since it is transmitted in cleartext. Just add a username and password,
   * for example begin("ESPUI Control", "username", "password")
   */
  ESPUI.begin( "ESPUI Control" );
}

void loop( void ) {
  dnsServer.processNextRequest();

  static long oldTime = 0;
  static bool switchi = false;

  if ( millis() - oldTime > 5000 ) {
    ESPUI.updateControl( "Millis:", String( millis() ) );
    switchi = !switchi;
    ESPUI.updateControl( "Switch one", switchi ? "1" : "0" );
    oldTime = millis();
  }
}

