#ifndef FFT2_H
#define FFT2_H

#include <complex>
#include <vector>

template <class T>
class FFT2
{

private:
	const unsigned dimW, dimH;
	const unsigned logDimW, logDimH;


public:

	FFT2();
	FFT2(unsigned lgW, unsigned lgH) :
		logDimW(lgW),
		logDimH(lgH),
		dimW(1 << lgW),
		dimH(1 << lgH) {}
	~FFT2() {}

	template<class OTHER>
	void operator()(std::vector<std::vector<std::complex<OTHER>>>& A);
};

#include "FFT2.cpp"

#endif // !FFT2_H