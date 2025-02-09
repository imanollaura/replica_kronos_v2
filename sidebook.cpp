
#include "Arduino.h"
#include "sidebook.h"



/**************************************************************************************************************************************

Construye un objeto de la clase Cacharro que construye un objeto tft para el display y uno NTPClient para recibir los datos

***************************************************************************************************************************************/
Cacharro::Cacharro()
  : tft(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST), NTP(nullptr){ 
    /*Esta linea inicializa ya las clases 
    que van a servir en la clase que se quiere construir. Para la clase NTP tenemos un puntero que aún no apunta a ningún sitio porque aún no
    la hemos construido. Por eso su argumento es " nullptr "*/

  this->begin_tft(tft);
  delay(100);

  WiFiUDP ntpUDP;
  this->NTP = new NTPClient(this->ntpUDP);
} 

/*****************************************************************************************************************************************

Inicializa el display en negro, orientación vertical

*****************************************************************************************************************************************/
void Cacharro::begin_tft(Adafruit_ST7789 tft){

  pinMode(TFT_BL, OUTPUT);      // TTGO T-Display enable Backlight pin 4
  digitalWrite(TFT_BL, HIGH);   // T-Display turn on Backlight
  this->tft.init(135, 240);        
  this->tft.setRotation(1);        // Configura la orientación de la pantalla
  delay(100); // Allow time for display hardware to initialize
  tft.fillScreen(ST77XX_BLACK);
}

/**********************************************************************************************************************************

Escribe texto en una linea, de un color concreto. "Borrar" hace referencia a la posibilidad de borrar to lo anterior antes de
escribir el texto nuevo.

**********************************************************************************************************************************/
void Cacharro::escribir(String texto, int linea, String color, bool borrar){
  if(borrar) this->tft.fillScreen(ST77XX_BLACK);

  this->tft.setCursor(0,linea * 15);  //Son 5 píxeles por linea
  this->tft.setTextWrap(false);

  if (color == "negro") {
        this->tft.setTextColor(ST77XX_BLACK);
    } else if (color == "blanco") {
        this->tft.setTextColor(ST77XX_WHITE);
    } else if (color == "rojo") {
        this->tft.setTextColor(ST77XX_RED);
    } else if (color == "verde") {
        this->tft.setTextColor(ST77XX_GREEN);
    } else if (color == "azul") {
        this->tft.setTextColor(ST77XX_BLUE);
    } else if (color == "cian") {
        this->tft.setTextColor(ST77XX_CYAN);
    } else if (color == "magenta") {
        this->tft.setTextColor(ST77XX_MAGENTA);
    } else if (color == "amarillo") {
        this->tft.setTextColor(ST77XX_YELLOW);
    } else {
        this->tft.setTextColor(ST77XX_WHITE);
    }
  this->tft.print(texto);
}

/************************************************************************************************************************************

Conecta a la red WiFi y lo comunica por el display

************************************************************************************************************************************/
void Cacharro::con_wifi(char* usuario, char* contrasena){
  WiFi.begin(usuario, contrasena);
  this->escribir("Conectando...", true);
  while(WiFi.status() != WL_CONNECTED){
    this->escribir("    .", 2, "negro");
    delay(500);
    this->escribir("    .", 2);
    delay(100);
    yield(); // Allow ESP32 to process tasks
  }
  this->escribir("¡Conectado!", 3, "verde");
}

/***************************************************************************************************************************************

Vamos a escribir una plantilla para el display donde después escribiremos las cosas

***************************************************************************************************************************************/

void Cacharro::template_tft(){

  // Para utilizar en sprintf e imprimir textos en el display
  char texto_ip[25];

  //Ponemos la pantalla negra
  this->tft.fillScreen(ST77XX_BLACK);

  //EL TEXTO
  this->escribir("IKER, CARAPAPA!", 1, "cian", false);
  sprintf(texto_ip, "IP:%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1],WiFi.localIP()[2], WiFi.localIP()[3]);
  this->escribir(texto_ip, 2, "verde", false);

  this->escribir("Hora: ", 3, "azul", false);

  this->escribir("Fecha: ", 5, "rojo", false);
 
  //LOS SEPARADORES
  this->tft.setTextColor(ST77XX_BLUE);
  this->tft.setCursor(15 ,15 * 4);  //Son 5 píxeles por linea
  this->tft.print(":");
  this->tft.setCursor(40,15*4); 
  this->tft.print(":");

  this->tft.setTextColor(ST77XX_RED);
  this->tft.setCursor(15, 15*6);
  this->tft.print("/");
  this->tft.setCursor(40, 15*6);
  this->tft.print("/");

  this->tft.setTextColor(ST77XX_YELLOW);
  this->tft.setCursor(0, 15*8);
  this->tft.print("UN BESITO A MI JULIA BONITA");
}

/***************************************************************************************************************************************

Toma como entrada la posición que quiere actualizar e introduce el nuevo valor en sitio en el display. 
  0: segundos
  1: minutos
  2: horas
  3: dias
  4: meses
  5: años
***************************************************************************************************************************************/

void Cacharro::actualiza_valor(uint8_t pos, MyTime Time){
  char year[5];

  // Consulto en mi "diccionario" las coordenadas en las que quiero imprimir cada unidad 
  this->tft.setCursor(this->dict_coord[pos][0], this->dict_coord[pos][1]);

  switch(pos){
    case 0:
      this->tft.setTextColor(ST77XX_BLACK);
      this->tft.print(num2str2(Time.temp_date.Second));
      this->tft.setCursor(this->dict_coord[pos][0], this->dict_coord[pos][1]);
      this->tft.setTextColor(ST77XX_WHITE);
      this->tft.print(num2str2(Time.date.Second));
      break;

    case 1:
      this->tft.setTextColor(ST77XX_BLACK);
      this->tft.print(num2str2(Time.temp_date.Minute));
      this->tft.setCursor(this->dict_coord[pos][0], this->dict_coord[pos][1]);
      this->tft.setTextColor(ST77XX_WHITE);
      this->tft.print(num2str2(Time.date.Minute));
      break;

    case 2:
      this->tft.setTextColor(ST77XX_BLACK);
      this->tft.print(num2str2(Time.temp_date.Hour));
      this->tft.setCursor(this->dict_coord[pos][0], this->dict_coord[pos][1]);
      this->tft.setTextColor(ST77XX_WHITE);
      this->tft.print(num2str2(Time.date.Hour));
      break;

    case 3:  //Junto con el día cambiará el día de la semana
      this->tft.setTextColor(ST77XX_BLACK);
      this->tft.print(num2str2(Time.temp_date.Day));
      this->tft.setCursor(this->dict_coord[pos][0], this->dict_coord[pos][1]);
      this->tft.setTextColor(ST77XX_WHITE);
      this->tft.print(num2str2(Time.date.Day));
      this->tft.setCursor(0, 7*15);
      this->tft.setTextColor(ST77XX_BLACK);
      this->tft.print("ES " + Time.temp_date.DiaSemana + ", MELON");
      this->tft.setCursor(0, 7*15);
      this->tft.setTextColor(ST77XX_MAGENTA);
      this->tft.print("ES " + Time.date.DiaSemana + ", MELON");
      break;

    case 4:
      this->tft.setTextColor(ST77XX_BLACK);
      this->tft.print(num2str2(Time.temp_date.Month));
      this->tft.setCursor(this->dict_coord[pos][0], this->dict_coord[pos][1]);
      this->tft.setTextColor(ST77XX_WHITE);
      this->tft.print(num2str2(Time.date.Month));
      break;

    case 5:
      this->tft.setTextColor(ST77XX_BLACK);
      sprintf(year, "%d", Time.temp_date.Year);
      this->tft.setCursor(this->dict_coord[pos][0], this->dict_coord[pos][1]);
      this->tft.print(year);
      this->tft.setTextColor(ST77XX_WHITE);
      sprintf(year, "%d", Time.date.Year);
      this->tft.print(year);
      break;    

  }
}

/**************************************************************************************************************************************

Queremos actualizar únicamente los valores necesarios. Es decir, con el año se actualiza todo pero con la hora únicamente minutos y segundos

***************************************************************************************************************************************/

void Cacharro::actualiza(MyTime Time){
  int8_t i = 6;          // Si no hay nada que actualizar lo dejamos todo como está (y así no parpadea)

  if (Time.temp_date.Year != Time.date.Year) i = 5;
  else if (Time.temp_date.Month != Time.date.Month) i = 4;
  else if (Time.temp_date.Day != Time.date.Day) i = 3;
  else if (Time.temp_date.Hour != Time.date.Hour) i = 2;
  else if (Time.temp_date.Minute != Time.date.Minute) i = 1;
  else if (Time.temp_date.Second != Time.date.Second) i = 0;

  if(i != 6){ 
    for(i; i >= 0; i--){
      actualiza_valor(i, Time);
    }
  }
}

/*************************************************************************************************************************

Recibe un número entero y lo devuelve como String. Si el número es de una cifra le añade un 0 para forzar las dos cifras

************************************************************************************************************************/

String Cacharro::num2str2(uint8_t num){
  char str[3];  //Necesitamos reservar una posición extra para el fina del string
  if(num <= 9){
    sprintf(str, "0%d", num);
  } else sprintf(str, "%d", num);
  return str;
}

/***********************************************************************************************************************+++

Construye el objeto NTPClient

***************************************************************************************************************************/

NTPClient::NTPClient(UDP& udp,const char* poolServerName){
  this->_udp = &udp;
  this->_poolServerName = poolServerName;
}

void NTPClient::begin(unsigned int port){
  this->_port = port;
  this->_udp->begin(this->_port);
  this->_udpSetup = true;
}

/*****************************************************************************************************************************

Lee el Epoch

*****************************************************************************************************************************/
unsigned long NTPClient::getEpochTime() const{
    return this->_timeOffset + // User offset
    this->_currentEpoc + // Epoch returned by the NTP server
    ((millis() - this->_lastUpdate) / 1000); // Time since last update
}

/***************************************************************************************************************************

Mira si es la primera vez que se conecta el servidor o si ya es hora de actualizar el reloj a través del NTP

***************************************************************************************************************************/
bool NTPClient::update() {
  if ((millis() - this->_lastUpdate >= this->_updateInterval)     // Update after _updateInterval
    || this->_lastUpdate == 0) {                                // Update if there was no update yet.
    if (!this->_udpSetup || this->_port != NTP_DEFAULT_LOCAL_PORT) this->begin(this->_port); // setup the UDP client if needed
    return this->forceUpdate();
  }
  return false;   // return false if update does not occur
}

/**************************************************************************************************************************

Actualiza la NTP

**************************************************************************************************************************/

bool NTPClient::forceUpdate() {

  // flush any existing packets
  while(this->_udp->parsePacket() != 0)
    this->_udp->flush();

  this->sendNTPPacket();

  // Wait till data is there or timeout...
  byte timeout = 0;
  int cb = 0;
  do {
    delay ( 10 );
    cb = this->_udp->parsePacket();
    if (timeout > 100) {
    	Serial.println("Timeout from NTP Server");
    	return false; // timeout after 1000 ms
    }
    timeout++;
  } while (cb == 0);

  this->_lastUpdate = millis() - (10 * (timeout + 1)); // Account for delay in reading the time

  this->_udp->read(this->_packetBuffer, NTP_PACKET_SIZE);

  unsigned long highWord = word(this->_packetBuffer[40], this->_packetBuffer[41]);
  unsigned long lowWord = word(this->_packetBuffer[42], this->_packetBuffer[43]);
  // combine the four bytes (two words) into a long integer
  // this is NTP time (seconds since Jan 1 1900):
  unsigned long secsSince1900 = highWord << 16 | lowWord;

  this->_currentEpoc = secsSince1900 - SEVENZYYEARS;

  return true;  // return true after successful update
}

void NTPClient::sendNTPPacket() {
    memset(this->_packetBuffer, 0, NTP_PACKET_SIZE);
    this->_packetBuffer[0] = 0b11100011;  // LI, Version, Mode
    this->_packetBuffer[1] = 0;           // Stratum, or type of clock
    this->_packetBuffer[2] = 6;           // Polling Interval
    this->_packetBuffer[3] = 0xEC;        // Peer Clock Precision

    this->_packetBuffer[12]  = 49;
    this->_packetBuffer[13]  = 0x4E;
    this->_packetBuffer[14]  = 49;
    this->_packetBuffer[15]  = 52;

    Serial.println("Enviando paquete NTP...");

    if (this->_poolServerName) {
        Serial.print("Servidor NTP: ");
        Serial.println(this->_poolServerName);
        this->_udp->beginPacket(this->_poolServerName, 123);
    } else {
        Serial.print("Dirección IP NTP: ");
        Serial.println(this->_poolServerIP);
        this->_udp->beginPacket(this->_poolServerIP, 123);
    }

    int bytesWritten = this->_udp->write(this->_packetBuffer, NTP_PACKET_SIZE);
    Serial.print("Bytes enviados: ");
    Serial.println(bytesWritten);

    this->_udp->endPacket();
}


void NTPClient::setUpdateInterval(unsigned long updateInterval) {
  this->_updateInterval = updateInterval;
}

void NTPClient::end() {
  this->_udp->stop();

  this->_udpSetup = false;
}
