# HUMMINGBIRD FCU MATH FUNCTIONS

## `math_functs.h`

Extra math functions such as:

* [Fast square root](https://en.wikipedia.org/wiki/Fast_inverse_square_root)
* [Range constrain (template)](https://github.com/ArduPilot/ardupilot/blob/00cfc1932fe98452ede016ea9f9f799d10ea9fb8/libraries/AP_Math/AP_Math.cpp#L287)
* [Safe square root (template)](https://github.com/ArduPilot/ardupilot/blob/00cfc1932fe98452ede016ea9f9f799d10ea9fb8/libraries/AP_Math/AP_Math.cpp#L71)
* [Safe arcsine (template)](https://github.com/ArduPilot/ardupilot/blob/00cfc1932fe98452ede016ea9f9f799d10ea9fb8/libraries/AP_Math/AP_Math.cpp#L50)

## `matrices_h`

A matrix object is definied by it's rows and columns. When a matrix object is created, the array is allocated on the heap (RAM2 for Teensy 4.1) with the C++ 'new' keyword as an array of pointers. See [this resource](https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/) to learn more.

## `matrix_math.h`

USES SINGLE-PRECISION FLOATS!
 * 
The library assumes the 2D array is one long dynamic array in memory (row major). The array indexing may be a bit strange, but since we allocated one long array in memory, it sould help a bit with cache performance. Seebelow for an example of this.

I give the following site credit for the matrix math algorithms. It was a significant inspiration and resource for algorithms!

[http://www.mymathlib.com/matrices/](http://www.mymathlib.com/matrices/)

Vector operations include:

* Fill
* Addition
* Accumulate (in-place)
* Subtract

Matrix operations include:

* Fill
* Transpose
* Square matrix transpose (in-place)
* Addition
* Add identity (I + A)
* Subtract identity (I - A)
* Accumulate (in-place)
* Subtraction
* Subtract accumulate (in-place)
* Negate (-1 * A)
* Matrix-vector multiply
* Matrix-matrix multiply
* Inversion via Cholesky decomposition
* Special multiplication (A * B^T)

How to Allocate and Access a 2D Array:

```cpp
float *A = new float[rows * cols];  // Allocate 2D array
for (size_t i = 0; i < rows; i++)  // Set value
{
    for (size_t j = 0; j < cols; j++)
    {
        *(A + i*cols + j) = 0.0f;  // Row-major
    }
}
```

### Resources

* [https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/](https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/)
* [https://eli.thegreenplace.net/2015/memory-layout-of-multi-dimensional-arrays](https://eli.thegreenplace.net/2015/memory-layout-of-multi-dimensional-arrays)

## `vectors.h`

A vector object is definied by it's rows/length. When a vector object is created, the array is allocated on the heap (RAM2 for Teensy 4.1) with the C++ 'new' keyword. See [this resource](https://www.techiedelight.com/dynamic-memory-allocation-in-c-for-2d-3d-array/) to learn more.

How to Allocate and Access a 1D Array (vector):

```cpp
float *b = new float[rows];  // Allocate 1D aray/vector
for (size_t i = 0; i < rows; i++)
    b[i] = 0.0f;  // Or, use: *(b + i)
```
