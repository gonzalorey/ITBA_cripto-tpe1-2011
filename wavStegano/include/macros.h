/* macros.h */

#ifndef MACROS_H_
#define MACROS_H_

/* a=target variable, b=bit number to act upon 0-n */
#define BIT_SET(a,b) ((a) |= (1<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1<<(b)))
#define BIT_GET(a,b) ((a) & (1<<(b)))


#define STR_NULL(x) (((x) == NULL)? "(null)" : (x))

#endif /* MACROS_H_ */
