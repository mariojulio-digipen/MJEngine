#ifndef IFFT2_H
#define IFFT2_H

#include <complex>
#include <vector>

template <class T>
class IFFT2
{

private:
	const unsigned dimW, dimH;
	const unsigned logDimW, logDimH;

public:
	IFFT2();
	IFFT2(unsigned lgW, unsigned lgH) :
		logDimW(lgW),
		logDimH(lgH),
		dimW(1 << lgW),
		dimH(1 << lgH) {}
	~IFFT2() {}

	template<class OTHER>
	void operator()(std::vector<std::vector<std::complex<OTHER>>>& A);
};

#include "IFFT2.cpp"

#endif // !IFFT2_H