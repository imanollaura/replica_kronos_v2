#ifndef SIDEBOOK_H
#define SIDEBOOK_H
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
// Load Wi-Fi library
#include <WiFi.h>
#include <Udp.h>
#include <WiFiUDP.h>
#include "time_management.h"

//Para el diccionario de coordenadas
#include <iostream>
#include <map>
#include <vector>


// pinouts from https://github.com/Xinyuan-LilyGO/TTGO-T-Display
#define TFT_MOSI 19
#define TFT_SCLK 18
#define TFT_CS 5
#define TFT_DC 16
#define TFT_RST 23
#define TFT_BL 4

#define NTP_DEFAULT_LOCAL_PORT 1337

#define SEVENZYYEARS 2208988800UL
#define NTP_PACKET_SIZE 48


class NTPClient{
  private:

    UDP*          _udp;
    bool _udpSetup = false;
    long          _timeOffset     = 0;
    unsigned long _currentEpoc    = 0;      // In s
    unsigned long _lastUpdate     = 0;      // In ms

    byte          _packetBuffer[NTP_PACKET_SIZE];

    unsigned long _updateInterval = 60000;  // In ms

    const char*   _poolServerName; // Default time server
    IPAddress     _poolServerIP;
    unsigned int _port;

    void          sendNTPPacket();

    
  public:
  
    NTPClient(UDP& udp, const char* poolServerName = "time.microcom.es");

    void begin(unsigned int port = 1337);

        /**
     * @return time in seconds since Jan. 1, 1970
     */
    unsigned long getEpochTime() const;

        /**
     * This should be called in the main loop of your application. By default an update from the NTP Server is only
     * made every 60 seconds. This can be configured in the NTPClient constructor.
     *
     * @return true on success, false on failure
     */
    bool update();

    /**
     * This will force the update from the NTP Server.
     *
     * @return true on success, false on failure
     */
    bool forceUpdate();

        /**
     * Set the update interval to another frequency. E.g. useful when the
     * timeOffset should not be set in the constructor
     */
    void setUpdateInterval(unsigned long updateInterval);

    void end();
};


class Cacharro{
  private:
    // constructor for data object named tft 
    Adafruit_ST7789 tft;
    WiFiUDP ntpUDP;

    //Coordenadas en las que escribiré por pantalla cada unidad
    uint8_t dict_coord[6][2] = {
      {50, 15*4}    // Segundos
      ,{25, 15*4}    // Minutos
      ,{0, 15*4}      // Horas
      ,{0, 15*6}       // Dias
      ,{25, 15*6}    // Meses
      ,{45, 15*6}     // Años
    };

  public:
    NTPClient* NTP; //Quiero que el NTP sea público para poder llamar a sus funciones

    Cacharro(); 
    
    void begin_tft(Adafruit_ST7789 tft);
    void escribir(String texto, int linea = 0, String color = "blanco", bool borrar = false);
    void con_wifi(char* usuario, char* contrasena);  //Conexión a WiFi

    void template_tft();  //Plantilla para escribir después la hora

    String num2str2(uint8_t num); //Conversión int a String

    void actualiza_valor(uint8_t pos, MyTime Time);  // Escribe en display los nuevos valores
    void actualiza(MyTime Time);   //Decide qué valor actualizar

};

#endif