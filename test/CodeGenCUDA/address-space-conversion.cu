// RUN: %clang_cc1 %s -triple nvptx-- -fcuda-is-device -emit-llvm -o - | FileCheck %s

#include "../SemaCUDA/cuda.h"

#define N 32

extern __shared__ int x;


__global__ void explicit_address_space_cast(int* p) {
	// CHECK: @_Z27explicit_address_space_castPi
   __shared__ unsigned char x[N];

   for (unsigned int i=0; i<(N/4); i++) {
     ((unsigned int *)x)[i] = 0;
		// CHECK: addrspacecast
   }
}

__global__ void pointer_as_array_access() {
	// CHECK: @_Z23pointer_as_array_accessv
   __shared__ int A[10];
   int* p = A + 1;
   p[x] = 0;
	 // CHECK: addrspacecast
}

__device__ int* callee(int* p) {
	// CHECK: @_Z6calleePi
  return p;
}

__global__ void caller() {
	// CHECK: @_Z6callerv
  __shared__ int A[10];
  __shared__ int* p;
	p = A;
	// CHECK: addrspacecast

	((int*)A)[x] = 42;
	// CHECK: addrspacecast
	((int*)A)[0] = 15;
	// CHECK: addrspacecast

  int *np = callee(p);
	A[2] = 5;
	np[0] = 2;
}
