// RUN: %clang_cc1 %s -ffake-address-space-map -emit-llvm -o - | FileCheck %s

#define NULL ((void*)0)

void null_pointer_implicit_conversion(int i, __global int *A) {
	// CHECK: null_pointer_implicit_conversion
	__global int *b;

	b = i > 42 ? A : NULL;

	if (b != NULL)
	  A[0] = b[5];
	// CHECK: null
}
