#ifndef DA_TRICK3
#define DA_TRICK3

#include "IFFT2.h"
#include "FFT1.h"
#include <iostream>

template<class R>
IFFT2<R>::IFFT2()
{
}

template<class T>
template<class OTHER>
inline void IFFT2<T>::operator()(std::vector<std::vector<std::complex<OTHER>>>& A)
{
	/*
	computes the 2D DFT of the matrix A. The 2D matrix A is assumed
	to have M rows, and N columns, indexed in row major fashion. As a result of calling
	this function, A is overwritten with the DFT of A.
	*/
	unsigned int ASize = A.size();
	if ((ASize & (ASize - 1)) != 0)
		throw std::exception("Rows dimensions must be powers of two. Nothing done");

	for (unsigned i = 0; i < ASize; i++)
	{
		unsigned int ARSize = A[i].size();
		if ((ARSize & (ARSize - 1)) != 0)
			throw std::exception("Columns dimensions must be powers of two. Nothing done");
	}

	// Transform rows
	for (unsigned i = 0; i < dimW; i++)
	{
		// transform columns from one row
		IFFT<OTHER> ifft(logDimH);	// vectors of length 2^logDimH
									//std::vector<OTHER> Cj(dimW);
		std::vector<std::complex<OTHER>> Cj(dimH);
		for (unsigned j = 0; j < dimH; j++)
			Cj[j] = A[j][i];

		std::vector<std::complex<OTHER>> DFT = ifft(&Cj[0]);
		for (unsigned j = 0; j < dimH; j++)
			A[j][i] = DFT[j];

	}

	// Transform columns
	for (unsigned i = 0; i < dimH; i++)
	{
		// transform columns from one row
		IFFT<OTHER> ifft(logDimW);	// vectors of length 2^logDimH
		std::vector<std::complex<OTHER>> Cj(dimW);
		for (unsigned j = 0; j < dimW; j++)
			Cj[j] = A[i][j];

		std::vector<std::complex<OTHER>> DFT = ifft(&Cj[0]);
		for (unsigned j = 0; j < dimW; j++)
			A[i][j] = DFT[j];

	}

}

#endif