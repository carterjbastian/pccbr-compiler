#ifndef TYPES_H_
#define TYPES_H_

/* The full enumeration of types of variables in the C subset */
typedef enum { INT_LT,
               INT_ARRAY_LT,
               FUNC_INT_LT,
               FUNC_VOID_LT,
               STR_LT,
               VOID_LT,
               ERROR_LT} var_lookup_type;

#endif // TYPES_H_
