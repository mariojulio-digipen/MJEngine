#ifndef DA_TRICK
#define DA_TRICK

#include "FFT1.h"


/*

DFT of a Frequency bin Fk:

	where	Fk = SUM(n=[0,N-1] | xn*e^(-i*2*PI*k*n/N))

*/

template<class REAL>
FFT_Base<REAL>::FFT_Base()
{
}

template<class R>
FFT_Base<R>::FFT_Base(unsigned lg_dim, bool inv) :
	lg_dimension(lg_dim),
	dimension(1 << lg_dim),
	sign2pi(2 * ((inv ? R(4) : R(-4)) * std::atan(R(1)))), // to compute the -2PI factor
	norm(inv ? R(1) / R(dimension) : R(1)) // the 1/N factor
{
	bit_reverse = new unsigned[dimension];
	for (unsigned i = 0; i < dimension; ++i)
	{
		unsigned r = 0;
		for (unsigned j = 0, n = i; j < lg_dimension; ++j, n >>= 1)
		{
			r = (r << 1) | (n & 0x01);
		}
		bit_reverse[i] = r;
	}
	//transform = new std::complex<R>[dimension];
	transform.resize(dimension);
}


template<class R>
FFT_Base<R>::~FFT_Base(void)
{
	//delete[] transform;
	delete[] bit_reverse;
}

template<class R>
template<class OTHER>
std::vector<std::complex<R>> FFT_Base<R>::operator()(OTHER* in)
{

	for (unsigned p = 0; p < dimension; ++p)
		transform[p] = std::complex<R>(in[bit_reverse[p]]);

	for (unsigned h = 0; h < lg_dimension; ++h)
	{
		unsigned m = (1 << h); // 2^h
		unsigned n = (m << 1); // 2m

		std::complex<R> twiddle = std::exp(std::complex<R>(0, sign2pi / n)); // (Real, Imaginary)
		for (unsigned k = 0; k < dimension; k += n)
		{
			std::complex<R> twiddle_j(1);
			for (unsigned j = 0; j < m; ++j)
			{
				std::complex<R> e_Fmkj = twiddle_j * transform[k + j + m];

				std::complex<R> z = transform[k + j] - e_Fmkj;
				transform[k + j] = transform[k + j] + e_Fmkj;
				transform[k + j + m] = z;

				twiddle_j *= twiddle;

			}
		}
	}

	// add the norm to each xn
	for (unsigned p = 0; p < dimension; p++)
	{
		transform[p] *= norm;
	}

	return transform;
}

template<class REAL>
FFT<REAL>::FFT(unsigned lg_dim) : FFT_Base<REAL>(lg_dim, false)
{

}

template<class REAL>
IFFT<REAL>::IFFT(unsigned lg_dim) : FFT_Base<REAL>(lg_dim, true)
{

}

#endif