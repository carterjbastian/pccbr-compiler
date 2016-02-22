#include "codegen.h"
//#include "../quadgen/quadgen.h"
#include "../util/symtab.h"

#include <string.h>
#include <stdio.h>

#define   MAX_TEMP_COUNT  1000000

extern quad_t list;
extern int tempCount;
quad_t list_end;

static const char *label_prefix = "L_";
static const char *temp_prefix = "T";

const int START_POS   12;
int cur_pos;

int create_constants(FILE *fp, symhashtable_t *scope, curr_pos);
int add_constant(FILE *fp, symbol_t symbol);

/*
 * The function to generate assembly from quads
 */
int generate_assembly(FILE *fp, quad_t ir, symboltable_t *table) {
  // Local vars
  quad_t quad;
  symhashtable_t *scope;
  symnode_t *node;
  int tab_size;

  curr_pos = START_POS;

  /* Set up */
  // Stack
  fprintf(fp, ".pos 0\n");
  fprintf(fp, "\tirmovl stack, %%esp\n");

  /* Constants */
  fprintf(fp, "\n");
  fprintf(fp, ".pos %x\n", START_POS);
  fprintf(fp, "L_CONSTANTS:\n");
  // For each constant in root of symbol table
    // Give it current address
    // Generate assembly for it
    // Increment curr pos

  /* Globals */
  // Loop through
  fprintf(fp, "\n");
  fprintf(fp, ".pos %x\n", ((curr_pos / 4) * 4) + 4);
  fprintf(fp, "L_GLOBALS:\n");
  scope = table->root;
  tab_size = scope->size;
  
  // Loop through each bucket in the global scope
  for (int i = 0; i < tab_size; i++) {
    // Is the bucket occupied?
    if (scope->table[i] != NULL) {
      // Check each node in the bucket
      for (node = scope->table[i]; node != NULL; node = node->next) {

        if (node->type != FUNC_INT_LT && node->type != FUNC_VOID_LT) {
          // Set globals address to curr_pos
          // Increment curr_pos
          fprintf(fp, "\tPROCESSING GLOBAL VAR %s\n", node->name);
        }
        
      }
    }
  }


  /* Temps */




  // Jump to global assignment section
  fprintf(fp, "\tjmp GLOBAL_ASSIGNMENTS\n");

  // Label for calling main
  fprintf(fp, "CALL_MAIN:\n");
  // Call the main function
  fprintf(fp, "\tcall L_MAIN\n");

  // halt
  fprintf(fp, "\thalt\n\n");

  // LABEL FOR GLOBAL ASSIGNMENT
  fprintf(fp, "GLOBAL_ASSIGNMENTS:\n");

  /* while Loop for quads */
  for (quad = ir; quad != NULL; quad = quad->next) {
    /* The switch statement to deal with quads */
    switch(quad->op) {
      /* CASE 0: NULL_QOP */
      case NULL_QOP:
        //fprintf(fp, "\tnop\n");
        break;

      /* CASE 1: ASSN_QOP */
      case ASSN_QOP :
        fprintf(fp, "\tASSN_QOP\n");
        break;

      /* CASE 2: LABEL_QOP */
      case LABEL_QOP :
        if (strcmp(quad->operand1->name, "L_MAIN") == 0) {
          fprintf(fp, "\tjmp CALL_MAIN\n");
        }
        fprintf(fp, "%s:\n", quad->operand1->name);
        break;

      /* CASE 3: RETURN_QOP */
      case RETURN_QOP :
        fprintf(fp, "\tret\n");
        break;


      default :
        fprintf(fp, "Go Fuck Yourself\n");

    }
  }
  /* Wrap up */

  // String definitions

  // .pos for stack
  fprintf(fp, ".pos 0x%x\n", 0x10c);

  // stack location
  fprintf(fp, "stack:\n");
  fprintf(fp, "\t.long 0x%08x\n", 0xFFFFFFFF);
  return 0;
}


int create_constants(FILE *fp, symhashtable_t *scope, curr_pos) {
  symbol_t symbol;
  int num_bytes = 0;

  for (int i = 0; i < scope->size; i++) {
    for (symbol = scope->table[i]; symbol != NULL; symbol = symbol->next) {
      if (symbol->vType = CONST_VT)
        num_bytes += add_constant(fp, symbol);
    }
  }

  return num_bytes;
}

int add_constant(FILE *fp, symbol_t symbol) {
  switch (symbol->type):
    case INT_LT:
      fprintf(fp, "# Creation of int %s\n", symbol->name);
      fprintf(fp, "\t.long 0x%x\n", symbol->val);
      break;

    case STR_LT:
      fprintf(fp, "# Creation of string for %s\n", symbol->name);
      char *clean_str = calloc(strlen(symbol->name) - 1, 1);
      sscanf(symbol->name, "#c%s", clean_str);
      for (int i = 0; i < strlen(clean_str); i++) {
        fprintf("\t.byte 0x%02x\n", clean_str[i]);
      }
      fprintf("\t.byte 0x00\n");
      break;

    default:
      break;
}



