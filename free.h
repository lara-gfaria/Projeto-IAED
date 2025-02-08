/* iaed24 - ist1106059 - project */

#ifndef FREE_H_
#define FREE_H_

#include "project.h"
#include "cars.h"
#include "parks.h"

/* Funções Free */
void free_park(Park *park, System *sys);
void remove_park_sys(Park *p, System *sys);
void free_dayPark(Park *p, System *sys);
void free_park_carMov(Car *car, Park *park, System *sys);
void free_date(Date *date);
void free_car(Car *car, System *sys);

#endif