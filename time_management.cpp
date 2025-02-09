# include "time_management.h"

//La clase no tiene que hacer nada al iniciarse
MyTime::MyTime() = default;

//Mide el tiempo en un momento específico para poder contar desde ahí
void MyTime::iniciarCronometro(){
  this->inicio = millis()/1000; 
}

//Cuenta el tiempo desde que se inició el cronómetro
uint16_t MyTime::cronometro(){
  return millis()/1000 - this->inicio;
}

void MyTime::setEpoch(uint32_t epoch){
  this->ntp_epoch = epoch;
}

//Estima el epoch añadiendo la medida del cronómetro a la última actualización por el NTP
uint32_t MyTime::realTime(){
  return ntp_epoch + this->cronometro();
}

/*************************************************************************************************************

Obtiene, a partir del epoch, cada una de las medidas de tiempo.

*************************************************************************************************************/
void MyTime::dayTime(uint32_t epoch){

  //Vamos a generar unos punteros que nos lleven a la distribución de días primero en los ciclos de año bisiesto y luego en cada mes del año
  int dias_anos[] = {365, 365, 366, 365, 0};
  int* pt_dias_anos = dias_anos;

  int dias_meses[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0};
  int b_dias_meses[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 0}; // Años bisiestos
  int* pt_dias_meses; //Elegiremos más tarde cuál de los dos es el vector

  int* ret_ano;         //Aquí recibiremos los resultados cuando calculemos fechas
  int* ret_mes;

  //Como nuestro horario es UTC + 1
  epoch += 3600;


  //EL AÑO: es el año en el que se empezó a contar (1970) más 4 veces la cantidad de ciclos bisestos que hayan pasado más los años individuales 
  //desde entonces.

  this->date.TotalDays = epoch/86400;        // Dias desde el 01/01/1970
  this->date.Bisiestos = this->date.TotalDays/(4*365+1);

  ret_ano = calcula_fecha(this->date.TotalDays - this->date.Bisiestos*(4*365+1), pt_dias_anos);  // Metemos los días desde el último ciclo bisiesto
  this->date.Ano = *ret_ano;            // Año desde el último ciclo bisiesto
  this->date.Year = 1970 + 4*this->date.Bisiestos + this->date.Ano;
  ret_ano++;   //Avanzamos el puntero para obtener el otro valor
  this->date.DayOfYear = *ret_ano;           // Dias del presente año


  // Si es bisiesto o no los meses tienen duraciones distintas. Vamos a escribir un flag para no repetir más adelante
  if(this->date.Year % 4 == 0){  
    this->date.fl_bisiesto = true;
  } else {
    this->date.fl_bisiesto = false;
  }

  if(this->date.fl_bisiesto){
    pt_dias_meses = b_dias_meses;
  } else {
    pt_dias_meses = dias_meses;
  }

  // La hora del año. Me servirá para saber si es horario de verano
  this->date.HourOfYear = (epoch - (this->date.Bisiestos*(365*4+1) + this->date.Ano*365)*86400) / 3600;
  if(hora_cambio(epoch)) epoch += 3600; //Me dice si cambiar el horario de verano

  // El mes y el día
  ret_mes = calcula_fecha(this->date.DayOfYear, pt_dias_meses);
  this->date.Month = *ret_mes + 1;
  ret_mes++;
  this->date.Day = *ret_mes + 1;

  //El día de la semana es trivial (el +3 viene de que el 1/1/1970 fue jueves)
  this->date.WeekDay = (this->date.TotalDays + 3) % 7;

  //La hora
  this->date.SecondOfDay = epoch % 86400;
  this->date.Second = this->date.SecondOfDay % 60;
  this->date.MinuteOfDay = this->date.SecondOfDay / 60;
  this->date.Minute = this->date.MinuteOfDay % 60;
  this->date.Hour = this->date.MinuteOfDay / 60;

  this->dia_semana();

  delay(100);  

}


/*************************************************************************************************************************

Convertimos el día de la semana numérico en texto: 1-> Lunes, 2-> Martes...

*************************************************************************************************************************/
void MyTime::dia_semana(){
  switch(this->date.WeekDay){

    case 0:
      this->date.DiaSemana = "LUNES";
      break;
    case 1:
      this->date.DiaSemana = "MARTES";
      break;
    case 2:
      this->date.DiaSemana = "MIERCOLES";
      break;
    case 3:
      this->date.DiaSemana = "JUEVES";
      break;
    case 4:
      this->date.DiaSemana = "VIERNES";
      break;
    case 5:
      this->date.DiaSemana = "SABADO";
      break;
    case 6:
      this->date.DiaSemana = "DOMINGO";
      break;

  }
}


/*************************************************************************************************************************+

Calculamos la hora del año a la que ocurre el cambio a horario de verano. Esto ocurre el último domingo de marzo a las
2 de la mañana así que cada año es diferente.

***************************************************************************************************************************/

bool MyTime::hora_cambio(uint32_t epoch){
  uint8_t m_31 = 31 + 28 +31;  // Dia del año a 31 de marzo 
  uint8_t bis = 0;
  uint8_t suma_bis = 0;   
  uint16_t hora_cambio;
  uint16_t fin_cambio;
  bool fl_ret = false;


  if(this->date.fl_bisiesto == true) m_31++; bis = 1; // Si este es año bisiesto
  if(this->date.Ano > 1) suma_bis = 1; //Sumaremos un día si ya ha pasado el año bisiesto

  //Queremos saber la hora del año que corresponde al último domingo de marzo a las 2 de la mañana. Cambio de hora
  // el +4 es porque el 1/1/1970 fue jueves
  hora_cambio = ((31 - (this->date.Bisiestos*(365*4+1) + (this->date.Ano)*365 + suma_bis + m_31 + 4) % 7) + bis + 28 + 31 -1) * 24 + 2;

  // En 2025 el intervalo ha sido desde el 30 de marzo a las 3 hasta las 2 del 26 de octubre. Todos los años son las mismas horas
  //                     horas-> 1/abril                  hasta el 26/10 00:00              
  fin_cambio = hora_cambio +  21 + 24             + (30 + 31 + 30+ 31 + 31+ 30 + 25)*24    + 2;

  if (this->date.HourOfYear >= hora_cambio && this->date.HourOfYear <= fin_cambio){
    fl_ret = true;
  }   

  return fl_ret;
}

/******************************************************************************************************************

Cargo los datos en temp_date para poder compararlos en el nuevo ciclo

*****************************************************************************************************************/

void MyTime::copy_temp(){
  this->temp_date.Second = this->date.Second;
  this->temp_date.Minute = this->date.Minute;
  this->temp_date.Hour = this->date.Hour;
  
  this->temp_date.Day = this->date.Day;
  this->temp_date.Month = this->date.Month;
  this->temp_date.Year = this->date.Year;

  this->temp_date.DiaSemana = this->date.DiaSemana;
}

/**********************************************************************************************************************

Inicializa la date temporal al comienzo del programa para poder comparar después

************************************************************************************************************************/

void MyTime::init_temp(){
  this->temp_date.Second = 0;
  this->temp_date.Minute = 0;
  this->temp_date.Hour = 0;
  
  this->temp_date.Day = 0;
  this->temp_date.Month = 0;
  this->temp_date.Year = 0; 

  this->temp_date.DiaSemana = "";
}

/****************************************************************************************************************

Toma el numero total de días y un puntero a un vector con los números totales de días por ciclo i.e [31, 28, 31, ...., 0]
y nos devuelve el número de mes o año y el día concreto. El 0 al final del vector indica el final del mismo. No es necesario
pero evitará datos extraños en caso de error.

*****************************************************************************************************************/

int* calcula_fecha(int dia, int* finales){
  int suma_finales = 0;
  bool finvec = false;
  int contador = 0;
  int* ret = new int[2];

  while (!finvec){
    if (*finales == 0) finvec = true;
    else{
      suma_finales += *finales;
      if (dia <= suma_finales){
        ret[0] = contador;
        ret[1] = dia - suma_finales + *finales;
        finvec = true;
        break;
      }
    }
    contador++;
    finales++;
  }
  return ret;
}


