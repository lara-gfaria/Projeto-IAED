/* iaed24 - ist1106059 - project */

#ifndef PARSER_H_
#define PARSER_H_

#include "project.h"
#include "cars.h"
#include "parks.h"
#include "dates.h"

/* Funções Parser */
char* read_string(char *input);
int read_int(char *input);
float read_float(char *input);
Park *read_park_name(char *input, System *sys);

#endif
