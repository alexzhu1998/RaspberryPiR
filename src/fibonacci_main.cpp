// #include "Fibonacci.h"
extern "C" {
#include <wiringPi.h>
}
#include <stdio.h>
extern "C" {
#include <string.h>
}
#include <R.h>
#include <Rdefines.h>
#include <vector>

// #define ledPin 0
// int fibonacci_main(int x) {
//   Fibonacci fib;
//   return fib.getFibonacciNumber(x);
// }

// void double_me(int x) {
//   // Doubles the value at the memory location pointed to by x
//   x = x + x;
// }

extern "C" {
    void helloWorld(char const **greeting) {
        *greeting = "Hello World!";
    }
}

extern "C" {
    void blink (int num) {
        printf("Program is starting ... \n");
        wiringPiSetup();
        int ledPin = 0;
        pinMode(ledPin,OUTPUT);
        // printf("Using pin %d\n", ledPin);
        while(1) {
            digitalWrite(ledPin, HIGH);
            printf("Led turned on >>>\n");
            delay(num);
            digitalWrite(ledPin, LOW);
            printf("Led turned off <<<\n");
            delay(num);
        }
    }
}

extern "C"{
    void setup(void) {
        wiringPiSetup();
    }
}

extern "C" {
    int conversion (char const **e) {
        if (strcmp(*e,"GPIO17") == 0) {
            return 0;
        } else {
            printf("Extension Not Valid\n");
            return -1;
        }
        return -1;
    }
}

extern "C" {
    void setPin(char const **e, char const **type) {
        int ledPin = conversion(e);
        if (ledPin == -1) return;
        if (strcmp(*type,"OUTPUT") == 0) {
            pinMode(ledPin,OUTPUT);
        } else if (strcmp(*type,"INPUT") == 0) {
            pinMode(ledPin,INPUT);
        } else {
            printf("Type not valid!\n");
        }

    }
}
extern "C" {
    void (char const **e, char const **level) {
        int ledPin = conversion(e);
        if (ledPin == -1) return;
        digitalWrite(ledPin,HIGH);
    }
}

extern "C" {
    void output(char const **e, int on) {
        int ledPin = conversion(e);
        if (ledPin == -1) return;
        if (on == 1) {
            digitalWrite(ledPin, HIGH);
        } else {
            digitalWrite(ledPin, LOW);
        }

    }
}

// extern "C" {
//   void addOneToVector(int* n, double* vector) {
//     for (int i = 0; i < *n; ++i)
//       vector[i] += 1.0;
//   }
// }
// extern "C" {
//   SEXP helloA1() {
//     printf("hello World\n");
//     return (R_NilValue);
//   }
// }
//
// extern "C" SEXP conv(SEXP a, SEXP b) {
//   int i, j, na, nb, nab;
//   double *xa, *xb, *xab;
//   SEXP ab;
//   PROTECT(a = AS_NUMERIC(a));
//   PROTECT(b = AS_NUMERIC(b));
//   na = LENGTH(a);
//   nb = LENGTH(b);
//   nab = na +nb -1;
//   PROTECT(ab = NEW_NUMERIC(nab));
//   xa = NUMERIC_POINTER(a);
//   xb = NUMERIC_POINTER(b);
//   xab = NUMERIC_POINTER(ab);
//   for (i = 0; i < nab; i++) {
//     xab[i] = 0.0;
//   }
//   for (i = 0; i < na; i++) {
//     for (j = 0; j < nb; j++) {
//       xab[i+j] += xa[i]*xb[j];
//     }
//   }
//   UNPROTECT(3);
//   return(ab);
// }

// SEXP add(SEXP a, SEXP b) {
//   SEXP result = PROTECT(allocVector(REALSXP, 1));
//   REAL(result)[0] = asReal(a) + asReal(b);
//   UNPROTECT(1);
//
//   return result;
// }
