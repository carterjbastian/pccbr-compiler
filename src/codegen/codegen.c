#include "codegen.h"
#include "../quadgen/quadgen.h"
#include "../util/symtab.h"
#include <string.h>
#include <stdio.h>

#define   MAX_TEMP_COUNT  1000000

extern quad_t list;
quad_t list_end;

static const char *label_prefix = "L_";
static const char *temp_prefix = "T";



