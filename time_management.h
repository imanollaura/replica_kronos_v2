#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>

typedef struct{
  uint32_t Year;
  uint8_t Ano; // Años desde el último bisiesto
  uint16_t Bisiestos;
  bool fl_bisiesto;

  uint8_t Month;

  uint8_t Day;
  uint32_t TotalDays;
  uint16_t DayOfYear;
  uint8_t WeekDay;
  String DiaSemana;

  uint8_t Hour;
  uint32_t HourOfYear;

  uint8_t Minute;
  uint16_t MinuteOfDay;

  uint8_t Second;
  uint32_t SecondOfDay;
}Date;

class MyTime{
  private:
    
    uint16_t inicio;    //Inicio del cronómetro
    uint32_t ntp_epoch = 0; //Inicializamos epoch

  public:
    Date date;
    Date temp_date;
    
    MyTime();

    void iniciarCronometro();
    uint16_t cronometro();

    void setEpoch(uint32_t epoch); 

    uint32_t realTime();

    void dayTime(uint32_t epoch);

    bool hora_cambio(uint32_t epoch);

    void copy_temp();
    void init_temp();
    void dia_semana();
};

int* calcula_fecha(int, int*);


