/* iaed24 - ist1106059 - project */

#ifndef CARS_H_
#define CARS_H_

#include "project.h"
#include "parser.h"
#include "parks.h"
#include "dates.h"
#include "free.h"
#include "sort.h"

/* Funções HashTable */
int hash(char *licence);
void init_hash_table(System *sys);
void insert_car_hash_table(Car *car, System *sys);
Car *find_car(char *licence, System *sys);

/* Funções Comando e */
void command_e(char *input, System *sys);
void create_car(char *input, Park *p, char *licence_str, System *sys);
void init_car(Car *car, char *licence);
int registe_entry(Car *car, Park *p, char *input, System *sys);
int registe_carMov(Car *car, Park *park, Date *date);
int licence_valid(char *licence);

/* Funções Comando s */
void command_s(char *input, System *sys);
int registe_exit(Car *car, Park *p, char *input, System *sys);
void update_cars_park(Park *park, Car *car);
void update_and_print_carMovs(Car *car, float price);
int realloc_list_dayPark(DayPark *dayPark);
float calcule_price(Car *car, Park *park);
float calcule_price_periods(int minutes, Park *park);
int calcule_minutes_same_day(Car *car);
int calcule_minutes_entryday(Date *date);
int calcule_minutes_exitday(Date *date);
int same_year(int entry_month, int exit_month, int entry_day, int exit_day);
int diff_year(int entry_year, int exit_year, int entry_month, 
int exit_month, int entry_day, int exit_day);
int days(Car *car);

/* Funções Comando v */
void command_v(char *input, System *sys);

/* Funções Comando f */
void command_f(char *input, System *sys);
void list_date_profit(Park *park);
void list_car_profit(Park *park, char *date, System *sys);
int date_before_sys(char *date, System *sys);

#endif