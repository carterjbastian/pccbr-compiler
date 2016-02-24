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
int last_stomped = 0;

// Initialize register table
reg_t registers[] = {
  {"%eax", NULL, 1}, {"%ecx", NULL, 1}, {"%edx", NULL, 1}, {"%ebx", NULL, 1}, 
  {"%esi", NULL, 1}, {"%edi", NULL, 1} 
};

int create_constants(FILE *fp, symhashtable_t *scope, int curr_pos);
int add_constant(FILE *fp, symnode_t *symbol);

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
  int firstLabel = 1;
  curr_pos = START_POS;

  /* Set up */
  // Stack
  fprintf(fp, ".pos 0x0\n");
  fprintf(fp, "\tirmovl stack, %%esp\n");
  fprintf(fp, "\tjmp GLOBAL_ASSIGNMENTS\n");

  /* Constants */
  fprintf(fp, "\n");
  fprintf(fp, ".pos 0x%x\n", START_POS);
  fprintf(fp, "L_CONSTANTS:\n");
  // For each constant in root of symbol table
    // Give it current address
    // Generate assembly for it
    // Increment curr pos
  curr_pos += create_constants(fp, table->root, curr_pos);

  /* Globals */
  // Loop through
  curr_pos = ((curr_pos / 4) * 4) + 4;
  fprintf(fp, "\n");
  fprintf(fp, ".pos 0x%x\n", curr_pos);
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
          // Mark this value as a global
          node->vType = GLOBAL_VT;
          // Set globals address to curr_pos and increment curr_pos
          fprintf(fp, "\t\t#Putting global %s at memory location %08x\n", node->name, node->mem_location);
        }
        
      }
    }
  }


  /* Temps */
  curr_pos = ((curr_pos / 4) * 4) + 4;
  fprintf(fp, "\n");
  fprintf(fp, ".pos 0x%x\n", curr_pos);
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
          fprintf(fp, "\t\t#Putting temp %s at memory location %08x\n", node->name, node->mem_location);
        }
        
      }
    }
  }



  // Jump to global assignment section
  curr_pos = ((curr_pos / 4) * 4) + 4;
  fprintf(fp, "\n");
  fprintf(fp, ".pos 0x%x\n", curr_pos);


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
    reg_t *reg1 = NULL;
    reg_t *reg2 = NULL;
    reg_t *truth_reg = NULL;
    reg_t *false_reg = NULL;
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
        reg1 = get_available_register(fp, registers);

        // Load in each variable's location from memory
        switch (quad->operand1->vType) {
          case TEMP_VT:
            mem_loc1 = quad->operand1->mem_location;
            break;

          case CONST_VT:
            mem_loc1 = quad->operand1->mem_location;
            break;

          case LOCAL_VT:
            quad->operand1->absolute_address = 0;
            
            // Does this local variable have a memory address?
            if (quad->operand1->mem_location) {
              mem_loc1 = quad->operand1->mem_location;
            } else {
            // If not, we need to give it one on the stack
              push_val = 1;
            }
            break;

          case GLOBAL_VT:
            mem_loc1 = quad->operand1->mem_location;
            break;

          default :
            fprintf(fp, "#ERROR: unrecognized variable type...\n");
        }

        if (quad->operand2 == NULL) {
          fprintf(fp, "\tirmovl 0x0, %s\n", reg1->name); 
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, mem_loc1);
          reg1->available = 1;
          break;
        }

        // Load in each variable's location from memory
        switch (quad->operand2->vType) {
          case TEMP_VT:
            if (quad->operand2->mem_location) {
              mem_loc2 = quad->operand2->mem_location;
              
            } else {
              if (quad->operand2->hasVal) {
                assn_val = quad->operand2->val;
                mem_loc2 = 0x0;
              } else {
                fprintf(fp, "#ERROR: second operand in ASSN_QOP should never be unassigned temp\n");
              }
            }
            break;

          case CONST_VT:
            mem_loc2 = quad->operand2->mem_location;
            break;

          case LOCAL_VT:
            // Does this local variable have a memory address?
            quad->operand2->absolute_address = 0;
            if (quad->operand2->mem_location) {
              mem_loc2 = quad->operand2->mem_location;
            } else {
              if (quad->operand2->hasVal) {
                assn_val = quad->operand2->val;
                mem_loc2 = 0x0;
                quad->operand2->absolute_address = 1;
              } else {
                fprintf(fp, "#ERROR: second operand in ASSN_QOP should never be unassigned local\n");
              }
            }
            break;

          case GLOBAL_VT:
            mem_loc2 = quad->operand2->mem_location;
            break;

          default :
            fprintf(fp, "#ERROR: unrecognized variable type...\n");
        }

        // We need to create a new value
        if (push_val) {
          if (quad->operand2->absolute_address) {
            if (mem_loc2 == 0) {
              fprintf(fp, "\tirmovl 0x%08x, %s\n", assn_val, reg1->name);
              fprintf(fp, "\tpushl %s\n", reg1->name);
              quad->operand1->mem_location = local_offset;
              local_offset -= 4;
              fprintf(fp, "\t\t#Putting local %s with offset from ebp of %d\n", quad->operand1->name, quad->operand1->mem_location);
            } else {
              fprintf(fp, "\tmrmovl 0x%08x, %s\n", mem_loc2, reg1->name);
              fprintf(fp, "\tpushl %s\n", reg1->name);
              quad->operand1->mem_location = local_offset;
              local_offset -= 4;
              fprintf(fp, "\t\t#Putting local %s with offset from ebp of %d\n", quad->operand1->name, quad->operand1->mem_location);
            }
          } else {
            fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", mem_loc2, reg1->name);
            fprintf(fp, "\tpushl %s\n", reg1->name);
            quad->operand1->mem_location = local_offset;
            local_offset -= 4;
            fprintf(fp, "\t\t#Putting local %s with offset from ebp of %d\n", quad->operand1->name, quad->operand1->mem_location);
          }

        } else {
          if (quad->operand1->absolute_address) {
            if (quad->operand2->absolute_address) {
              if (mem_loc2 == 0) {
                fprintf(fp, "\tirmovl 0x%08x, %s\n", assn_val, reg1->name);
                fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, mem_loc1);

              } else {
                fprintf(fp, "\tmrmovl 0x%08x, %s\n", mem_loc2, reg1->name);
                fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, mem_loc1);
              }
            } else {
              fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", mem_loc2, reg1->name);
              fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, mem_loc1);
            }
          } else {
            if (quad->operand2->absolute_address) {
              if (mem_loc2 == 0) {
                fprintf(fp, "\tirmovl 0x%08x, %s\n", assn_val, reg1->name);
                fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, mem_loc1);
              } else {
                fprintf(fp, "\tmrmovl 0x%08x, %s\n", mem_loc2, reg1->name);
                fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, mem_loc1);
              }
            } else {
              fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", mem_loc2, reg1->name);
              fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, mem_loc1);
            }
          }
        }
        reg1->available = 1;
        //fprintf(fp, "\tASSN_QOP\n");
        break;

      /* CASE 2: LABEL_QOP */
      case LABEL_QOP :
        if (firstLabel == 1) {
          fprintf(fp, "\tjmp CALL_MAIN\n");
          firstLabel = 0;
        }
        /*
        if (strcmp(quad->operand1->name, "L_MAIN") == 0) {
          fprintf(fp, "\tjmp CALL_MAIN\n");
        }
        */
        fprintf(fp, "\n%s:\n", quad->operand1->name);
        if (strcmp(quad->operand1->name, "L_MAIN") == 0)
          fprintf(fp, "\trrmovl %%esp, %%ebp\n");
        break;

      /* CASE 3: RETURN_QOP */
      case RETURN_QOP :
        // Pop off the local variables by setting %esp to %ebp
        fprintf(fp, "\t\t#Popping off local variables\n");
        fprintf(fp, "\trrmovl %%ebp, %%esp\n");
        local_offset = -4; // WARNING: WILL THIS CAUSE PROBLEMS?
        
        // Return to the proper place in the code
        fprintf(fp, "\tret\n");
        break;


      /* CASE 4: PRE_CALL_QOP */
      case PRE_CALL_QOP:
        // Push all of the general purpose registers
          // NOTE: This can be optimized!
        fprintf(fp, "\t\t#Caller-save storing registers\n");
        for (int i = 0; i < REGISTER_COUNT; i++) 
          fprintf(fp, "\tpushl %s\n", registers[i].name);

        fprintf(fp, "\t\t#Storing the caller's ebp\n");
        // Push the current frame's base pointer
        fprintf(fp, "\tpushl %%ebp\n");
        fprintf(fp, "\t\t#Pushing %d arguments for the function\n", quad->operand1->func_arg_count); 
        break;


      /* CASE 5: ARG_QOP */
      case ARG_QOP :
        // Argument will be in a temporary
        reg1 = get_available_register(fp, registers);
 
        // Load it to a register 
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand1->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand1->mem_location, reg1->name);
        }
        // push it onto the stack
        fprintf(fp, "\tpushl %s\n", reg1->name);
        
        // Free register 1
        reg1->available = 1;
        break;


      /* CASE 6: POST_PARAMS_QOP */
      case POST_PARAMS_QOP :
        // Set the base pointer for the next frame to esp - 4
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        fprintf(fp, "\t\t#Setting the callee's base pointer\n");
        // Subtract four from the ESP and store the resulting address in a reg
        fprintf(fp, "\tirmovl 0x4, %s\n", reg1->name);
        fprintf(fp, "\trrmovl %%esp, %s\n", reg2->name);
        fprintf(fp, "\tsubl %s, %s\n", reg1->name, reg2->name);

        // Set the base pointer to that value
        fprintf(fp, "\trrmovl %s, %%ebp\n", reg2->name);

        // Modify the scope in such a way that  

        // Call the function
        fprintf(fp, "\tcall %s\n", quad->operand1->func_label);
        
        // Free the registers
        reg1->available = 1;
        reg2->available = 1;
        break;


      /* CASE 7: POST_CALL_QOP */
      case POST_RET_QOP :
        // Pop off the parameters to get to the old base pointer
        fprintf(fp, "\t\t#Popping off %d parameters\n", quad->operand2->func_arg_count);
        for (int i = 0; i < quad->operand2->func_arg_count; i++)
          fprintf(fp, "\tpopl %%esi\n");

        // Pop the EBP into the right place
        fprintf(fp, "\t\t#Restoring Caller's EBP\n");
        fprintf(fp, "\tpopl %%ebp\n");


        // Do we need to save the return value? It will be in %eax
        if (quad->operand1 != NULL) {
          fprintf(fp, "\t\t#Saving return value from the function call\n");
          
          if (quad->operand1->absolute_address) {
            fprintf(fp, "\trmmovl %%eax, 0x%08x\n", quad->operand1->mem_location);
          } else {
            fprintf(fp, "\trmmovl %%eax, %d(%%ebp)\n", quad->operand1->mem_location);
          }
        }


        // Pop the registers in order (caller save)
        fprintf(fp, "\t\t#Restoring Caller's Registers\n");  
        for (int i = REGISTER_COUNT - 1; i >= 0; i--)
          fprintf(fp, "\tpopl %s\n", registers[i].name);

        break;

      case ADD_QOP :
        // NOTE: we assume both operands and target and will be temporaries
        fprintf(fp, "\t\t#Adding operation\n");

        // We'll need two registers
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        // Load operands into registers
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Do the adding
        fprintf(fp, "\taddl %s, %s\n", reg2->name, reg1->name);

        // Put the result into the proper place in memory
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }

        // Free the registers
        reg1->available = 1;
        reg2->available = 1;

        break;

      case SUB_QOP :
        // NOTE: we assume both operands and target and will be temporaries
        fprintf(fp, "\t\t#Subtraction operation\n");

        // We'll need two registers
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        // Load operands into registers
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Do the adding
        fprintf(fp, "\tsubl %s, %s\n", reg2->name, reg1->name);

        // Put the result into the proper place in memory
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }
        // Free the registers
        reg1->available = 1;
        reg2->available = 1;

        break;

      case MULL_QOP :
        // NOTE: we assume both operands and target and will be temporaries
        fprintf(fp, "\t\t#Multiply operation\n");

        // We'll need two registers
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        // Load operands into registers
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Do the adding
        fprintf(fp, "\tmull %s, %s\n", reg2->name, reg1->name);

        // Put the result into the proper place in memory
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }

        // Free the registers
        reg1->available = 1;
        reg2->available = 1;

        break;

      case DIV_QOP :
        // NOTE: we assume both operands and target and will be temporaries
        fprintf(fp, "\t\t#Divide operation\n");

        // We'll need two registers
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        // Load operands into registers
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Do the adding
        fprintf(fp, "\tdivl %s, %s\n", reg2->name, reg1->name);

        // Put the result into the proper place in memory
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }

        // Free the registers
        reg1->available = 1;
        reg2->available = 1;

        break;

      case MOD_QOP :
        // NOTE: we assume both operands and target and will be temporaries
        fprintf(fp, "\t\t#Mod operation\n");

        // We'll need two registers
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        // Load operands into registers
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Do the adding
        fprintf(fp, "\tmodl %s, %s\n", reg2->name, reg1->name);


        // Put the result into the proper place in memory
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }

        // Free the registers
        reg1->available = 1;
        reg2->available = 1;

        break;

      case GOTO_QOP:
        fprintf(fp, "\tjmp %s\n", quad->operand1->name);
        break;

      case IF_FALSE_QOP:
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        // Get the value to be tested in a register
        fprintf(fp, "\t\t#IF_FALSE operation\n");

        if (quad->operand1->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand1->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand1->mem_location, reg1->name);
        }

        fprintf(fp, "\tirmovl 0x0, %s\n", reg2->name);

        // Move conditionally based on the result
        fprintf(fp, "\taddl %s, %s\n", reg1->name, reg2->name);
        fprintf(fp, "\tje %s\n", quad->operand2->name);

        // Free the registers
        reg1->available = 1;
        reg2->available = 1;

        break;


      case IF_TRUE_QOP:
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        // Get the value to be tested in a register
        fprintf(fp, "\t\t#IF_FALSE operation\n");

        if (quad->operand1->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand1->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand1->mem_location, reg1->name);
        }

        fprintf(fp, "\tirmovl 0x0, %s\n", reg2->name);

        // Move conditionally based on the result
        fprintf(fp, "\taddl %s, %s\n", reg1->name, reg2->name);
        fprintf(fp, "\tjne %s\n", quad->operand2->name);

        // Free the registers
        reg1->available = 1;
        reg2->available = 1;

        break;


      case EQ_QOP:
        fprintf(fp, "\t\t#EQ Conditional Evaluation\n");
        
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        truth_reg = get_available_register(fp, registers);
        false_reg = get_available_register(fp, registers);

        // Load in the values
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Load in truth and false
        fprintf(fp, "\tirmovl 0x1, %s\n", truth_reg->name);
        fprintf(fp, "\tirmovl 0x0, %s\n", false_reg->name);

        // Assume the comparison to be false (result goes into the false register)
        fprintf(fp, "\tsubl %s, %s\n", reg2->name, reg1->name);

        // Do the comparison and conditionally assign
        fprintf(fp, "\tcmove %s, %s\n", truth_reg->name, false_reg->name);

        // Move the results into the results memory location
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }

        // Free Registers
        reg1->available = 1;
        reg2->available = 1;
        truth_reg->available = 1;
        false_reg->available = 1;

        break;


      case LT_QOP:
        fprintf(fp, "\t\t#LT Conditional Evaluation\n");
        
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        truth_reg = get_available_register(fp, registers);
        false_reg = get_available_register(fp, registers);

        // Load in the values
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Load in truth and false
        fprintf(fp, "\tirmovl 0x1, %s\n", truth_reg->name);
        fprintf(fp, "\tirmovl 0x0, %s\n", false_reg->name);

        // Assume the comparison to be false (result goes into the false register)
        fprintf(fp, "\tsubl %s, %s\n", reg2->name, reg1->name);

        // Do the comparison and conditionally assign
        fprintf(fp, "\tcmovl %s, %s\n", truth_reg->name, false_reg->name);

        // Move the results into the results memory location
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }

        // Free Registers
        reg1->available = 1;
        reg2->available = 1;
        truth_reg->available = 1;
        false_reg->available = 1;

        break;


      case LTE_QOP:
        fprintf(fp, "\t\t#LTE Conditional Evaluation\n");
        
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        truth_reg = get_available_register(fp, registers);
        false_reg = get_available_register(fp, registers);

        // Load in the values
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Load in truth and false
        fprintf(fp, "\tirmovl 0x1, %s\n", truth_reg->name);
        fprintf(fp, "\tirmovl 0x0, %s\n", false_reg->name);

        // Assume the comparison to be false (result goes into the false register)
        fprintf(fp, "\tsubl %s, %s\n", reg2->name, reg1->name);

        // Do the comparison and conditionally assign
        fprintf(fp, "\tcmovle %s, %s\n", truth_reg->name, false_reg->name);

        // Move the results into the results memory location
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }

        // Free Registers
        reg1->available = 1;
        reg2->available = 1;
        truth_reg->available = 1;
        false_reg->available = 1;

        break;


      case GT_QOP:
        fprintf(fp, "\t\t#GT Conditional Evaluation\n");
        
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        truth_reg = get_available_register(fp, registers);
        false_reg = get_available_register(fp, registers);

        // Load in the values
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Load in truth and false
        fprintf(fp, "\tirmovl 0x1, %s\n", truth_reg->name);
        fprintf(fp, "\tirmovl 0x0, %s\n", false_reg->name);

        // Assume the comparison to be false (result goes into the false register)
        fprintf(fp, "\tsubl %s, %s\n", reg2->name, reg1->name);

        // Do the comparison and conditionally assign
        fprintf(fp, "\tcmovg %s, %s\n", truth_reg->name, false_reg->name);

        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }

        // Free Registers
        reg1->available = 1;
        reg2->available = 1;
        truth_reg->available = 1;
        false_reg->available = 1;

        break;


      case GTE_QOP:
        fprintf(fp, "\t\t#GTE Conditional Evaluation\n");
        
        reg1 = get_available_register(fp, registers);
        reg2 = get_available_register(fp, registers);

        truth_reg = get_available_register(fp, registers);
        false_reg = get_available_register(fp, registers);

        // Load in the values
        if (quad->operand2->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand2->mem_location, reg1->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand2->mem_location, reg1->name);
        }

        if (quad->operand3->absolute_address) {
          fprintf(fp, "\tmrmovl 0x%08x, %s\n", quad->operand3->mem_location, reg2->name);
        } else {
          fprintf(fp, "\tmrmovl %d(%%ebp), %s\n", quad->operand3->mem_location, reg2->name);
        }

        // Load in truth and false
        fprintf(fp, "\tirmovl 0x1, %s\n", truth_reg->name);
        fprintf(fp, "\tirmovl 0x0, %s\n", false_reg->name);

        // Assume the comparison to be false (result goes into the false register)
        fprintf(fp, "\tsubl %s, %s\n", reg2->name, reg1->name);

        // Do the comparison and conditionally assign
        fprintf(fp, "\tcmovge %s, %s\n", truth_reg->name, false_reg->name);

        // Move the results into the results memory location
        if (quad->operand1->absolute_address) {
          fprintf(fp, "\trmmovl %s, 0x%08x\n", reg1->name, quad->operand1->mem_location);
        } else {
          fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", reg1->name, quad->operand1->mem_location);
        }

        // Free Registers
        reg1->available = 1;
        reg2->available = 1;
        truth_reg->available = 1;
        false_reg->available = 1;

        break;


      default :
        fprintf(fp, "#Go Fuck Yourself\n");

    }
  }
  /* Wrap up */

  // String definitions

  // .pos for stack
  fprintf(fp, "\n.pos 0x%x\n", 0x800);

  // stack location
  fprintf(fp, "stack:\n");
  fprintf(fp, "\t.long 0x%08x\n", 0xFFFFFFFF);
  return 0;
}

int create_constants(FILE *fp, symhashtable_t *scope, int curr_pos) {
  symnode_t *symbol;
  int num_bytes = 0;

  for (int i = 0; i < scope->size; i++) {
    for (symbol = scope->table[i]; symbol != NULL; symbol = symbol->next) {
      if (symbol->vType == CONST_VT) {
        symbol->mem_location = curr_pos + num_bytes;
        num_bytes += add_constant(fp, symbol);
      }
    }
  }

  return num_bytes;
}

int add_constant(FILE *fp, symnode_t *symbol) {
  int bytes;
  switch (symbol->type) {
    case INT_LT:
      fprintf(fp, "# Creation of int %s at location 0x%08x\n", symbol->name, symbol->mem_location);
      fprintf(fp, "\t.long 0x%08x\n", symbol->val);
      bytes = 4;
      break;

    case STR_LT:
      fprintf(fp, "# Creation of string for %s\n", symbol->name);
      char *clean_str = calloc(strlen(symbol->name) - 1, 1);
      sscanf(symbol->name, "#c%s", clean_str);
      bytes = 1;
      for (int i = 0; i < strlen(clean_str); i++) {
        bytes += 1;
        fprintf(fp, "\t.byte 0x%02x\n", clean_str[i]);
      }
      fprintf(fp, "\t.byte 0x00\n");
      break;

    default:
      break;
    }
    return bytes;
}

/*
 * TO DO: Figure out how to save registers' values if issues are caused
 */
reg_t *get_available_register(FILE *fp, reg_t *registers) {
  int to_stomp = 0;
  for (int i = 0; i < REGISTER_COUNT; i++) {
    if (registers[i].available) {
      registers[i].available = 0;
      return &registers[i];
    }
  }
  
  // Take the last register and replace it
/*  for (int i = 0; i < REGISTER_COUNT; i++) {
    if (registers[i].contents->mem_location > 0) {
      fprintf(fp, "\t\t#Clearing out register %d\n", i);
      fprintf(fp, "\trmmovl %s, 0x%08x\n", registers[i].name, registers[i].contents->mem_location);
      // CLEAR OUT THE REGISTER
      registers[i].available = 0;
      return &registers[i];
    }
  } */
  /*
  fprintf(fp, "#Clearing out the first register\n");
  fprintf(fp, "\trmmovl %s, %d(%%ebp)\n", registers[0].name, registers[0].contents->mem_location);
  registers[0].available = 0;
  */
  to_stomp = ((last_stomped + 1) % REGISTER_COUNT);
  last_stomped = to_stomp;
  fprintf(fp, "\t\t#Stomping on register %d\n", to_stomp);
  registers[to_stomp].available = 0;
  return &registers[to_stomp];
}



