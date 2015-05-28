OpenCLIPP - OpenCL Integrated Performance Primitives
=========================================================

OpenCLIPP is a library providing processing primitives (image processing and computer vision primitives) implemented with OpenCL for fast execution on dedicated computing devices like GPUs.

It was designed to be simple to use and to have low overhead.

Two interfaces are provided :

 - C Interface similar to the Intel IPP and NVIDIA NPP libraries      
      
 - C++ Interface

Requirements
------------

An OpenCL SDK is required to build the library.
OpenCL SDKs are available for download from Intel, AMD and NVIDIA


How to build
------------

 - On Windows : Use the .sln file with Visual Studio 2012
 - Other platforms : Use make with the provided makefiles:
```
user@machine ~/OpenCLIPP# make
```

How to use it - C++
-------------------

``` C++
#include "OpenCLIPP.hpp"

// Initialize OpenCL
COpenCL CL;    
COpenCL::SetClFilesPath("/path/to/cl files/");

// Fill a SImage structure with image information
SImage SomeImage = {width, height, step, channels, type);

// Create an image in the device
//  Image data is not sent to the device
Image Img1(CL, SomeImage, Image1DataPtr);

// Create a second image in the device
Image Img2(CL, SomeImage, Image2DataPtr);

// Create an OpenCL program to do arithmetic operations
Arithmetic arithmetic(CL);

// Will add 10 to each pixel value and put the result in Img2 in the device
//  Img1 is automatically sent to the device, before the calculation, since it was not sent previously.
//  The memory transfer and calculation are done asynchronously.
arithmetic.Add(Img1, Img2, 10);

// Ask to read Img2, after previous operations are done.
//  true as argument means we want to wait for the transfer to finish.
//  After this statement, the result of the computation will be in Image2DataPtr.
Img2.Read(true);

// The image now contains the result of the addition
```

How to use it - C
-----------------

``` C
#include <OpenCLIPP.h>

/* Variables */
ocipContext Context = NULL;
ocipError Error = CL_SUCCESS;
ocipImage Img1, Img2;

/* Initialize OpenCL */
Error = ocipInitialize(&Context, NULL, CL_DEVICE_TYPE_ALL);
ocipSetCLFilesPath("/path/to/cl files/");

/* Fill a SImage structure with image information */
SImage SomeImage = {width, height, step, channels, type};

/* Create an image in the device
   Image data is not sent to the device */
Error = ocipCreateImage(&Img1, SomeImage, Image1DataPtr, CL_MEM_READ_ONLY);

/* Create a second image in the device */
Error = ocipCreateImage(&Img2, SomeImage, Image2DataPtr, CL_MEM_WRITE_ONLY);

/* Will add 10 to each pixel value of Img1 and put the result in Img2 in the device
    Img1 is automatically sent to the device, before the calculation, since it was not sent previously.
    The memory transfer and calculation are done asynchronously. */
Error = ocipAddC(Img1, Img2, 10);

/* Ask to read Img2, after previous operations are done.
   After this statement, the result of the computation will be in Image2DataPtr. */
Error = ocipReadImage(Img2);

/* Free images on the device */
Error = ocipReleaseImage(Img1);
Error = ocipReleaseImage(Img2);

/* Uninitialize */
Error = ocipUninitialize(Context);
```

Error handling - C++
--------------------

When using the C++ interface, errors are reported using C++ exceptions : an instance of cl::Error is thrown
The cl::Error will contain :

 - a numerical error code (cl::Error.err()) that can be translated to text using COpenCL::ErrorName()
 - optionally an explanatory string (cl::Error.what())

Error handling - C
------------------

Most functions of the C interface return a ocipError.
When no error occured, the value returned will be CL_SUCCESS (0). When an error occurs, a negative value will be returned. The error value can be translated to text using ocipGetErrorName().

Similarity with IPP & NPP
-------------------------

OpenCLIPP provides a C interface that can is similar to the popular libraries Intel IPP and NVIDIA NPP.
Except unlike these libraries, the OpenCLIPP library tracks images in the computing device using objects that know size, datatype and number of channels of the image. This leads to simpler function signatures than IPP and NPP (less arguments and no need for complex suffixes).

Typical primitive signature
``` C
/*IPP*/			IppStatus ippiAbsDiffC_8u_C1R(const Ipp8u* pSrc,  int srcStep,   Ipp8u* pDst, int dstStep,  IppiSize roiSize,  int value);
/*NPP*/			NppStatus nppiAbsDiffC_8u_C1R(const Npp8u* pSrc1, int nSrc1Step, Npp8u* pDst, int nDstStep, NppiSize oSizeROI, Npp8u nConstant);
/*OpenCLIPP*/	ocipError ocipAbsDiffC(ocipImage Source, ocipImage Dest, float value);
```


Supported image types
---------------------

The library supports the following image types:

- 2 dimensions, no size restrictions (size restrictions may be present for some primitives, like <16megapixels for FFT)
- Signed or Unsigned integer of 8, 16 or 32 bits
- Floating point 32 and 64 bits
- 1, 2, 3 or 4 channels (all channels must be the same type)
	
Supported platforms
-------------------

Supported OpenCL platforms :

- nVidia
- AMD
- Intel (tested only on CPU)
	
Supported Operating Systems :

- Windows 7+
- Linux (tested on Ubuntu with AMD)
	

Currently implemented primitives
--------------------------------

- Arithmetic and Logic
- LUT
- Morphology
- Transform (Mirror, Flip & Transpose)
- Resize and Rotate
- Datatype conversion & value scaling
- Treshold
- Filters (Blur, Sharpen, Sobel, Median, etc.)
- Histogram
- Pattern matching
- Statistical reductions
- Blob labeling
- Integral scan
- FFT (using external library clFFT)
- ...

Performance
-----------

Performance is significantly higher than IPP when operating on big (2048x2048+) images and using a high end GPU (expect 3 to 10x faster for most operations).
Performance is comparable and often slightly better than NPP on the same GPU (sometimes as much as 2x faster).
The library has the advantage of being able to run on AMD GPUs that usually provide significantly better OpenCL performance than NVIDIA GPUs.
No device specific optimization has been done so performance of many primitives can be further improved.

Authors
------------

- Antoine W. Campagna 
- Gao Chen

Call for contributions
----------------------

We will welcome comments and contributions to improve the library.
Please send us your contributions via pull requests.


Comments or questions
---------------------

- [Vision team](mailto:vision@crvi.ca)

License
-------

The OpenCLIPP is free for personal and commercial use.
It is released under the LGPL license, see the file named 'LICENSE' for details.

Publications
------------

If you are using the library in published work, please refer to the following paper:

M. Akhloufi, A. Campagna, "OpenCLIPP: OpenCL Integrated Performance Primitives library for computer vision applications", Proc. SPIE Electronic Imaging 2014, Intelligent Robots and Computer Vision XXXI: Algorithms and Techniques, P. 9025-31, San Francisco, CA, USA, February 2014.

