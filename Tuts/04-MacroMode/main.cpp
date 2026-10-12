#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <iomanip>

#include "PS2Math.h"

using std::cout;

#define QWALIGN __attribute__((aligned(16)))

/** Dump a vector to standard output.
*/
void DumpVector(float V[]) {

	cout << std::setprecision(6);
	cout << "V[0] = " << V[0] << "\tV[1] = " << V[1] <<
		"\tV[2] = " << V[2] << "\tV[3]= " << V[3] << std::endl;
}

/** Dump a matrix on standard output.
	 This function is used to dump a two dimensional array
	 to standard output.
*/
void DumpMatrix(float Mat[][4]) {

	cout << std::setprecision(6);
	cout << "[" << Mat[0][0] << "\t" << Mat[0][1] << "\t" << 
					 Mat[0][2] << "\t" << Mat[0][3] << "]\n";
	cout << "[" << Mat[1][0] << "\t" << Mat[1][1] << "\t" << 
					Mat[1][2] << "\t" << Mat[1][3] << "]\n";
	cout << "[" << Mat[2][0] << "\t" << Mat[2][1] << "\t" << 
					Mat[2][2] << "\t" << Mat[2][3] << "]\n";
	cout << "[" << Mat[3][0] << "\t" << Mat[3][1] << "\t" << 
					Mat[3][2] << "\t" << Mat[3][3] << "]\n";
}


int main() {

	//Gain exclusive access to the vector units.
	int VU0; 
	VU0 = open("/dev/ps2vpu0", O_RDWR);

	//Declare some vector to do some examples.
	float V1[4] QWALIGN = {1.0f, 0.0f, 1.0f, 1.0f};
	float V2[4] QWALIGN = {2.0f, 2.0f, 1.0f, 1.0f};
	float V3[4] QWALIGN = {0};

	//Sum two vectors
	asm __volatile__(" 

	lqc2 vf1, 0(%0)   
	lqc2 vf2, 0(%1)    
	vadd.xyzw vf3, vf1, vf2  
	sqc2 vf3, 0(%2)     
	
	": : "r" (V1), "r" (V2), "r" (V3));
	cout << "Vector Sum:\n";
	DumpVector(V3);

	//Dot products, notice that is not so efficien to use VU0 
	//to compute dot product because an horizontal addition 
	//of the first three terms must be done.
	asm __volatile__(" 

		lqc2 vf1, 0(%0)   
		lqc2 vf2, 0(%1)    
		vmul.xyzw	vf3, vf1, vf2  
		vaddy.x		vf3, vf3, vf3
		vaddz.x		vf3, vf3, vf3
		sqc2 vf3, 0(%2)     
	
	": : "r" (V1), "r" (V2), "r" (V3));

	cout << "Dot Product = " << V3[0] << std::endl;

	//Thanks to vopmula and vopsub cross product is a breeze in VU0
	asm __volatile__(" 

		lqc2 vf1, 0(%0)   
		lqc2 vf2, 0(%1)    
		vopmula.xyz ACC, vf1, vf2
		vopmsub.xyz vf3, vf2, vf1
		sqc2 vf3, 0(%2)    
	
	": : "r" (V1), "r" (V2), "r" (V3));
	cout << "Cross Product V1 . V2= " << V3[0] << std::endl;
	DumpVector(V3);

	//Now declare two matrix to make some matrix operations.
	float mat1[4][4] QWALIGN = {{1, 2, 3, 4}, 
										 {5, 6, 7, 8}, 
										 {9, 10, 11, 12}, 
										 {13, 14, 15, 16}};
	float mat2[4][4] QWALIGN = {{0.1, 0.2, 0.3, 0.4}, 
										 {0.5, 0.6, 0.7, 0.8}, 
										 {0.9, 1.0, 1.1, 1.2}, 
										 {1.3, 1.4, 1.5, 1.6}};
	
	/** This is the algorithm to multiply two matrix toghether mat1 . mat2
	    and store the result back to mat1.
		 Thanks to the broadcast operation only 16 operation are sufficient
		 to do the operation, more complex is code written with some other
		 SIMD technologies as for SSE of PIII processors.
	*/
		asm __volatile__(" 

		lqc2		vf1,		0(%0)
		lqc2		vf2,		16(%0)
		lqc2		vf3,		32(%0)
		lqc2		vf4,		48(%0)
		lqc2		vf5,		0(%1)
		lqc2		vf6,		16(%1)
		lqc2		vf7,		32(%1)
		lqc2		vf8,		48(%1)

		#Acual multiplicatoin code is here
		vmulax	ACC,		vf5,		vf1
		vmadday	ACC,		vf6,		vf1
		vmaddaz	ACC,		vf7,		vf1
		vmaddw	vf9,		vf8,		vf1

		vmulax	ACC,		vf5,		vf2
		vmadday	ACC,		vf6,		vf2
		vmaddaz	ACC,		vf7,		vf2
		vmaddw	vf10,		vf8,		vf2

		vmulax	ACC,		vf5,		vf3
		vmadday	ACC,		vf6,		vf3
		vmaddaz	ACC,		vf7,		vf3
		vmaddw	vf11,		vf8,		vf3

		vmulax	ACC,		vf5,		vf4
		vmadday	ACC,		vf6,		vf4
		vmaddaz	ACC,		vf7,		vf4
		vmaddw	vf12,		vf8,		vf4

		sqc2		vf9,		0(%0)
		sqc2		vf10,		16(%0)
		sqc2		vf11,		32(%0)
		sqc2		vf12,		48(%0)

	": : "r" (mat1), "r" (mat2));
	cout << "Matrix multiplication\n";
	DumpMatrix(mat1);

	//Now vector transformation using column vector notation.
	//This is absolutely inefficent.
	asm __volatile__(" 

		#Store the matrix into $8-$11 registers 
		lqc2		vf8,		0(%0)
		lqc2		vf9,		16(%0)
		lqc2		vf10,		32(%0)
		lqc2		vf11,		48(%0)
		#Load the vector
		lqc2		vf12,		0(%1)

		#Now multiply first row of the matrix for the vector 
		vmul.xyzw  vf8,   vf8,      vf12
		#Now the remaining ones.
		vmul.xyzw  vf9,   vf9,      vf12
		vmul.xyzw  vf10,  vf10,     vf12
		vmul.xyzw  vf11,  vf11,     vf12
			
		#we must now perform horizontal add of the component, each one
		#into a different component, store result in memory then 
		#into general register to do the swizzling
		sqc2			vf8,   0(%2)
		lq			   $8,	 0(%2)
		sqc2			vf9,   0(%2)
		lq			   $9,	 0(%2)
		sqc2			vf10,  0(%2)
		lq			   $10,	 0(%2)
		sqc2			vf11,  0(%2)
		lq			   $11,	 0(%2)

		#swizzle
		pextlw     $12,	$9,		$8
		pextuw     $13,	$9,		$8
		pextlw     $14,	$11,		$10
		pextuw     $15,	$11,		$10

		pcpyld     $8,		$14,		$12
		pcpyud     $9,		$12,		$14
		pcpyld     $10,	$15,		$13
		pcpyud     $11,	$13,		$15

		#Store again data into VU0 registers
		sq			  $8,		0(%2)
		lqc2		  vf8,	0(%2)
		sq			  $9,		0(%2)
		lqc2		  vf9,	0(%2)
		sq			  $10,	0(%2)
		lqc2		  vf10,	0(%2)
		sq			  $11,	0(%2)
		lqc2		  vf11,	0(%2)

		#finally add the values together
		vadd.xyzw  vf8,   vf8,   vf9
		vadd.xyzw  vf10,  vf10,  vf11
		vadd.xyzw  vf8,   vf8,   vf10
		sqc2		  vf8,   0(%2)

	": : "r" (mat1), "r" (V2), "r" (V3));
	cout << "Transformed vertex\n";
	DumpVector(V3);

	//Another example of transforming column vector, this time not using
	//swizzling with multimedia instruction.
	asm __volatile__(" 

		#Store the matrix into $8-$11 registers 
		lqc2		vf8,		0(%0)
		lqc2		vf9,		16(%0)
		lqc2		vf10,		32(%0)
		lqc2		vf11,		48(%0)
		#Load the vector
		lqc2		vf12,		0(%1)

		#Now multiply first row of the matrix for the vector 
		vmul.xyzw  vf8,   vf8,      vf12
		#Now the remaining ones.
		vmul.xyzw  vf9,   vf9,      vf12
		vmul.xyzw  vf10,  vf10,     vf12
		vmul.xyzw  vf11,  vf11,     vf12
		
		#now perform horizontal add each of them in different component
		vaddy.x	  vf8,   vf8,   vf8
		vaddx.y	  vf9,   vf9,   vf9
		vaddx.z	  vf10,  vf10,  vf10
		vaddx.w	  vf11,  vf11,  vf11
		vmr32		  vf1,   vf0  #this store constant (0, 0, 1.0, 0) in vf1

		vaddz.x	  vf8,   vf8,   vf8
		vaddz.y	  vf9,   vf9,   vf9
		vaddy.z	  vf10,  vf10,  vf10
		vaddy.w	  vf11,  vf11,  vf11
		vmr32		  vf2,   vf1  #this store constant (0, 1.0, 0, 0) in vf2

		vaddw.x	  vf8,   vf8,   vf8
		vaddw.y	  vf9,   vf9,   vf9
		vaddw.z	  vf10,  vf10,  vf10
		vaddz.w	  vf11,  vf11,  vf11
		vmr32		  vf3,   vf2  #this store constant (1.0, 0, 0, 0) in vf3

		#Multiply and add in accumulator to know the result of the operation.
		vmula.xyzw  ACC,   vf11,  vf0
		vmadda.xyzw ACC,   vf10,  vf1
		vmadda.xyzw ACC,   vf9,   vf2
		vmadd.xyzw  vf12,  vf8,   vf3

		sqc2		  vf12,   0(%2)

	": : "r" (mat1), "r" (V2), "r" (V3));
	cout << "Transformed vertex\n";
	DumpVector(V3);


	// Now transpose the matrix using multimedia registers.
	asm __volatile__(" 

		lq			  $8,		0(%0)
		lq			  $9,		16(%0)
		lq			  $10,	32(%0)
		lq			  $11,	48(%0)

		#First part of the swizzle (consult tutorial)
		pextlw     $12,	$9,		$8
		pextuw     $13,	$9,		$8
		pextlw     $14,	$11,		$10
		pextuw     $15,	$11,		$10

		#Second part of the swizzle (consult tutorial)
		pcpyld     $8,		$14,		$12
		pcpyud     $9,		$12,		$14
		pcpyld     $10,	$15,		$13
		pcpyud     $11,	$13,		$15

		sq			  $8,		0(%0)
		sq			  $9,		16(%0)
		sq			  $10,	32(%0)
		sq			  $11,	48(%0)

	": : "r" (mat1));
	cout << "Matrix transposing\n";
	DumpMatrix(mat1);

	/** This is the algorithm to transform a row vector.
	    It can be noticed how the algorith is simple comparing
		 to the preceding ones.
		 only four instructions are needed to transform a single vertex
		 at the cost of transposing transformation matrix before 
		 vertex transformation.
	*/
		asm __volatile__(" 

		lqc2		vf1,		0(%0)
		lqc2		vf2,		16(%0)
		lqc2		vf3,		32(%0)
		lqc2		vf4,		48(%0)
		lqc2		vf5,		0(%1)
		vmulax	ACC,		vf1,		vf5
		vmadday	ACC,		vf2,		vf5
		vmaddaz	ACC,		vf3,		vf5
		vmaddw	vf9,		vf4,		vf5
		sqc2		vf9,		0(%2)

	": : "r" (mat1), "r" (V2), "r" (V3));
	cout << "Transformed vertex\n";
	DumpVector(V3);
	
	//Finally release the VU0
	close(VU0);
 };
