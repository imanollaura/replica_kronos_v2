
#include "sidebook.h"
#include "time_management.h"
#include "esp_system.h"
#include <string>

//Inicializo mis variables
#define NTP_UPTE_INT 1800000	/* NTP Update interval (ms) */

//Las clases propias
Cacharro aparatejo = Cacharro();
MyTime Time = MyTime();

//El tiempo leído en el ntp y el estimado con el reloj interno
uint32_t ntpSecond;
uint32_t calc_time;

// Credenciales para el wifi casa
char* ssid = "TIM-73024334";
char* password = "RTEfTys2sfTu5sDY7ekGAdNd";

void setup() {

  Serial.begin(115200);
  //Nos conectamos a la WiFi e imprimimos la IP en el display
  aparatejo.con_wifi(ssid, password);
  
  Time.init_temp(); //Iniciamos la estructura date temporal para empezar con las comparaciones

  aparatejo.template_tft();  //La plantilla sobre la que escribiremos después
  
  //Iniciamos la conexión a la NTP  
  if (aparatejo.NTP) {
      aparatejo.NTP->begin();
      aparatejo.NTP->setUpdateInterval(NTP_UPTE_INT);
  } else {
      Serial.println("Failed to initialize NTPClient!");
  }

}


void loop() {

  //Si le toca actualizar desde el NTP cargamos el nuevo dato, reiniciamos la cuenta del reloj y actualizamos la estructura de fechas
  if (aparatejo.NTP->update()) {
    ntpSecond = aparatejo.NTP->getEpochTime();

    Time.setEpoch(ntpSecond); //Setter de tiempos
    Time.iniciarCronometro(); //Reinicio del reloj interno
    Time.dayTime(ntpSecond); //Actualización de la estructura Date en la clase MyTime

  } else {
    calc_time = Time.realTime(); //Corregimos el tiempo NTP + la cuenta del reloj interno desde la última actualización
    Time.dayTime(calc_time);
  }

  aparatejo.actualiza(Time); // Actualiza los tiempos en base al epoch
  Time.copy_temp();  // Guarda los datos nuevos en la variable temporal para próximas comparaciones
	delay(100);							
}


