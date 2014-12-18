// RUN: %clang_cc1 -triple x86_64-apple-darwin -fsyntax-only -verify  %s
// rdar://18716393

extern int a[] __attribute__((weak));
int b[] = {8,13,21};
struct {
  int x[10];
} c;
const char str[] = "text";

void ignore() {
  if (!a) {}
}
void test() {
  if (!b) {} // expected-warning {{address of array 'b' will always evaluate to 'true'}}
  if (b == 0) {} // expected-warning {{comparison of array 'b' equal to a null pointer is always false}}
  if (!c.x) {} // expected-warning {{address of array 'c.x' will always evaluate to 'true'}}
  if (c.x == 0) {} // expected-warning {{comparison of array 'c.x' equal to a null pointer is always false}}
  if (!str) {} // expected-warning {{address of array 'str' will always evaluate to 'true'}}
  if (0 == str) {} // expected-warning {{comparison of array 'str' equal to a null pointer is always false}}
}

int array[2];
int test1()
{
  if (!array) { // expected-warning {{address of array 'array' will always evaluate to 'true'}}
    return array[0];
  } else if (array != 0) { // expected-warning {{comparison of array 'array' not equal to a null pointer is always true}}
    return array[1];
  }
  if (array == 0) // expected-warning {{comparison of array 'array' equal to a null pointer is always false}}
    return 1;
  return 0;
}

#define NULL (void*)0

int test2(int* pointer, char ch, void * pv) {
   if (!&pointer) {  // expected-warning {{address of 'pointer' will always evaluate to 'true'}}
     return 0;
   }

   if (&pointer) {  // expected-warning {{address of 'pointer' will always evaluate to 'true'}}
     return 0;
   }

   if (&pointer == NULL) {} // expected-warning {{comparison of address of 'pointer' equal to a null pointer is always false}}

   if (&pointer != NULL) {} // expected-warning {{comparison of address of 'pointer' not equal to a null pointer is always true}}

   return 1;
}

void test3() {
   if (array) { } // expected-warning {{address of array 'array' will always evaluate to 'true'}}
   if (array != 0) {} // expected-warning {{comparison of array 'array' not equal to a null pointer is always true}}
   if (!array) { } // expected-warning {{address of array 'array' will always evaluate to 'true'}}
   if (array == 0) {} // expected-warning {{comparison of array 'array' equal to a null pointer is always false}}

   if (array[0] &&
       array) {} // expected-warning {{address of array 'array' will always evaluate to 'true'}}

   if (array[0] ||
       array) {} // expected-warning {{address of array 'array' will always evaluate to 'true'}}

   if (array[0] &&
       !array) {} // expected-warning {{address of array 'array' will always evaluate to 'true'}}
   if (array[0] ||
       !array) {} // expected-warning {{address of array 'array' will always evaluate to 'true'}}

   if (array && // expected-warning {{address of array 'array' will always evaluate to 'true'}}
       array[0]) {}
   if (!array || // expected-warning {{address of array 'array' will always evaluate to 'true'}}
       array[0]) {}

   if (array ||  // expected-warning {{address of array 'array' will always evaluate to 'true'}}
       (!array && array[0])) {} // expected-warning {{address of array 'array' will always evaluate to 'true'}}
 }

// rdar://19256338
#define SAVE_READ(PTR, RESULT) if( (PTR) && *(PTR) ) *RESULT=*PTR;
 
// Source
typedef unsigned char Boolean;
struct HTTPClientPrivate
{
   Boolean readSuspended;
};
typedef struct HTTPClientPrivate * HTTPClientRef;
static void _HTTPClientErrorHandler( HTTPClientRef me)
{
  Boolean result;
  SAVE_READ(&me->readSuspended, &result);
}
