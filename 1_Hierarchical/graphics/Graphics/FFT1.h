#ifndef FFT_H
#define FFT_H

#include <complex>
#include <cmath>
#include <vector>

// Base class
template<class REAL>
class FFT_Base
{
private:
	const unsigned lg_dimension, dimension;
	REAL sign2pi, norm;
	unsigned* bit_reverse;
	std::vector<std::complex<REAL>> transform;

	//FFT_Base& operator=(const FFT_Base&);

public:
	FFT_Base();
	FFT_Base(unsigned lg_dim, bool inverse);
	~FFT_Base(void);

	template<class OTHER>
	std::vector<std::complex<REAL>> operator()(OTHER* arr);
};


// Derived class for Fast Fourier Transform
template<class REAL>
class FFT : public FFT_Base<REAL>
{
public:
	FFT(unsigned lg_dim);
};


// Derived class for Inverse Fast Fourier Transform
template<class REAL>
class IFFT : public FFT_Base<REAL>
{
public:
	IFFT(unsigned lg_dim);
};

#include "FFT1.cpp"

#endif




///////////////////////////////////////

