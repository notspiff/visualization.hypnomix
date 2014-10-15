/* 
 * http://www.drdobbs.com/cpp/a-simple-and-efficient-fft-implementatio/199500857 * http://www.dspdimension.com/admin/dft-a-pied/
 */

#include <stdio.h>
#include <math.h>

#include "fft.h"


#define FFT_FORWARD 1
#define FFT_INVERSE -1

#define SWAP(a, b) tmpr = a; a = b; b = tmpr


static void fftPerform(float data[], const int len, int sign)
{
	unsigned long n, mmax, m, j, istep, i;
	double wtmp, wr, wpr, wpi, wi, theta;
	float tmpr, tmpi;

	n = len << 1;
	j = 1;
	for(i = 1; i < n; i += 2) {
		if(j > i) {
			SWAP(data[j-1], data[i-1]);
			SWAP(data[j], data[i]);
		}
		m = len;
		while(m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}

	mmax = 2;
	while(n > mmax) {
		istep = mmax << 1;
		theta = sign * (-6.28318530717959 / mmax);
		wtmp = sin(0.5 * theta);
		wpr = -2.0 * wtmp * wtmp;
		wpi = sin(theta);
// printf("theta=%f wtmp=%f wpr=%f wpi=%f\n", theta, wtmp, wpr, wpi);
		wr = 1.0;
		wi = 0.0;
		for(m = 1; m < mmax; m += 2) {
			for(i = m; i <= n; i += istep) {
				j = i + mmax;
				tmpr = wr * data[j-1] - wi * data[j];
				tmpi = wr * data[j] + wi * data[j-1];
				data[j-1] = data[i-1] - tmpr;
				data[j] = data[i] - tmpi;
				data[i-1] += tmpr;
				data[i] += tmpi;
			}
			wtmp = wr;
			wr += wr * wpr - wi * wpi;
			wi += wi * wpr + wtmp * wpi;
		}
		mmax = istep;
	}
}

void fft(float data[], const int len)
{
	fftPerform(data, len, FFT_FORWARD);
}


void ifft(float data[], const int len)
{
	int i;
	const float factor = 1.0 / (float)len;

	fftPerform(data, len, FFT_INVERSE);
	/* scaling and filtering very low float values */
	for(i = 0; i < len*2; i++) {
		if(data[i] > 0.000001f) {
			data[i] *= factor;
		} else {
			data[i] = 0.0f;
		}
	} 
}


void fftDump(float data[], const int len)
{
	int i;

	for(i = 0; i < len; i++) {
		printf("%f, %f\n", data[i*2], data[i*2+1]);
	}
}
