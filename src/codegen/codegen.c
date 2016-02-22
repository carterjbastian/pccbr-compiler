#include "codegen.h"
//#include "../quadgen/quadgen.h"
#include "../util/symtab.h"

#include <string.h>
#include <stdio.h>

#define   MAX_TEMP_COUNT  1000000
#define   REGISTER_COUNT  6

extern quad_t list;
extern int tempCount;
quad_t list_end;

static const char *label_prefix = "L_";
static const char *temp_prefix = "T";

const int START_POS = 12;
int curr_pos;

// Initialize register table
reg_t registers[] = {
  {"%%eax", NULL, 1}, {"%%ecx", NULL, 1}, {"%%edx", NULL, 1}, {"%%ebx", NULL, 1}, 
  {"%%esi", NULL, 1}, {"%%edi", NULL, 1} 
};

/*
 * The function to generate assembly from quads
 */
int generate_assembly(FILE *fp, quad_t ir, symboltable_t *table) {
  // Local vars
  quad_t quad;
  symhashtable_t *scope;
  symnode_t *node;
  int tab_size;
  int local_offset = -4;

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
  curr_pos = ((curr_pos / 4) * 4) + 4;
  fprintf(fp, "\n");
  fprintf(fp, ".pos %x\n", curr_pos);
  fprintf(fp, "L_GLOBALS:\n");
  scope = table->root;
  tab_size = scope->size;
  
  // Loop through each bucket in the global scope
  for (int i = 0; i < tab_size; i++) {
    // Is the bucket occupied?
    if (scope->table[i] != NULL) {
      // Check each node in the bucket
      for (node = scope->table[i]; node != NULL; node = node->next) {

        // Check if it's a global variable
        if (node->vType == LOCAL_VT && 
            (node->type != FUNC_INT_LT) && (node->type != FUNC_VOID_LT) ) {
          node->mem_location = curr_pos;
          curr_pos += 4;
          // Set globals address to curr_pos and increment curr_pos
          printf("\t\t#Putting global %s at memory location %08x\n", node->name, node->mem_location);
        }
        
      }
    }
  }


  /* Temps */
  curr_pos = ((curr_pos / 4) * 4) + 4;
  fprintf(fp, "\n");
  fprintf(fp, ".pos %x\n", curr_pos);
  fprintf(fp, "L_TEMPS:\n");

  // Loop through each bucket in the global scope
  for (int i = 0; i < tab_size; i++) {
    // Is the bucket occupied?
    if (scope->table[i] != NULL) {
      // Check each node in the bucket
      for (node = scope->table[i]; node != NULL; node = node->next) {

        // Check if it's a global variable
        if (node->vType == TEMP_VT) {
          node->mem_location = curr_pos;
          curr_pos += 4;
          // Set globals address to curr_pos and increment curr_pos
          printf("\t\t#Putting temp %s at memory location %08x\n", node->name, node->mem_location);
        }
        
      }
    }
  }



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
    int mem_loc1 = 0;
    int mem_loc2 = 0;
    int push_val = 0;
    int assn_val = 0;



    /* The switch statement to deal with quads */
    switch(quad->op) {

      /* CASE 0: NULL_QOP */
      case NULL_QOP:
        //fprintf(fp, "\tnop\n");
        break;


      /* CASE 1: ASSN_QOP */
      case ASSN_QOP :
        // Load in each variable's location from memory
        switch (quad->operand1->vType) {
          case TEMP_VT:
            mem_loc1 = quad->operand1->mem_location;
            break;

          case CONST_VT:
            mem_loc1 = quad->operand1->mem_location;
            break;

          case LOCAL_VT:
            // Does this local variable have a memory address?
            if (quad->operand1->mem_location) {
              mem_loc1 = quad->operand1->mem_location;
            } else {
            // If not, we need to give it one on the stack
              push_val = 1;
            }
            break;

          default :
            fprintf(fp, "#ERROR: unrecognized variable type...\n");
        }

        if (quad->operand2 == NULL) {
          fprintf(fp, "\timmovl 0x%08x, 0x0\n", mem_loc1);
          break;
        }

        // Load in each variable's location from memory
        switch (quad->operand2->vType) {
          case TEMP_VT:
            mem_loc2 = quad->operand2->mem_location;
            break;

          case CONST_VT:
            mem_loc2 = quad->operand2->mem_location;
            break;

          case LOCAL_VT:
            // Does this local variable have a memory address?
            if (quad->operand2->mem_location) {
              mem_loc2 = quad->operand2->mem_location;
            } else {
              if (quad->operand2->hasVal) {
                assn_val = quad->operand2->val;
                mem_loc2 = 0x0;
              } else {
                fprintf(fp, "#ERROR: second operand in ASSN_QOP should never be unassigned local\n");
              }
            }
            break;

          default :
            fprintf(fp, "#ERROR: unrecognized variable type...\n");
        }

        // We need to create a new value
        if (push_val) {
          if (mem_loc2 > 0) {
            fprintf(fp, "\tpushl (0x%08x)\n", mem_loc2);
            quad->operand1->mem_location = local_offset;
            local_offset -= 4;
            fprintf(fp, "\t\t#Putting local %s with offset from ebp of %d\n", quad->operand1->name, quad->operand1->mem_location);
          } else if (mem_loc2 < 0) {
            fprintf(fp, "\tpushl %d(%%ebp)\n", mem_loc2);
            quad->operand1->mem_location = local_offset;
            local_offset -= 4;
            fprintf(fp, "\t\t#Putting local %s with offset from ebp of %d\n", quad->operand1->name, quad->operand1->mem_location);
          } else {
            fprintf(fp, "\tpushl 0x%x\n", assn_val);
            quad->operand1->mem_location = local_offset;
            local_offset -= 4;
            fprintf(fp, "\t\t#Putting local %s with offset from ebp of %d\n", quad->operand1->name, quad->operand1->mem_location);
          }

        } else {
          if (mem_loc1 > 0) {
            if (mem_loc2 > 0) {
              fprintf(fp, "\tmmmovl 0x%08x, 0x%08x\n", mem_loc1, mem_loc2);
            } else if (mem_loc2 < 0) {
              fprintf(fp, "\tmmmovl 0x%08x, %d(esp)\n", mem_loc1, mem_loc2);
            } else {
              fprintf(fp, "\timmovl 0x%08x, 0x%08x\n", mem_loc1, assn_val);
            }
          } else {
            if (mem_loc2 > 0) {
              fprintf(fp, "\tmmmovl %d(esp), 0x%08x\n", mem_loc1, mem_loc2);
            } else if (mem_loc2 < 0) {
              fprintf(fp, "\tmmmovl %d(esp), %d(esp)\n", mem_loc1, mem_loc2);
            } else {
              fprintf(fp, "\timmovl %d(esp), 0x%08x\n", mem_loc1, assn_val);
            }
          }
        }
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

reg_t *get_available_register(FILE *fp, reg_t *registers) {
  for (int i = 0; i < REGISTER_COUNT; i++) {
    if (registers[i].available)
      return &registers[i];
  }
  
  // Take the last register and replace it 
  fprintf(fp, "#NO REGISTER AVAILABLE\n");
  return NULL;
}
