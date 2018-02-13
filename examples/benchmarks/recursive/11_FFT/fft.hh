#include <iostream>

#define border 128

template<class iter, class outputIter>
inline void FFT(iter first, iter last, outputIter res) {
	int n = std::distance(first, last);
	int N = n / 2;
	const double PI = 3.14159265358979323846;
	if (n != 2) {

		complex* temp1 = new complex[N];
		complex* temp2 = new complex[N];
		complex* out1 = new complex[N];
		complex* out2 = new complex[N];
		for (int i = 0; i < N; ++i) {
			temp1[i] = *first;
			++first;
			temp2[i] = *first;
			++first;
		}
		const complex J(0, 1);
		complex w = exp(-2.0 * PI * J / (double) n);
		complex wk = 1;

		if (n >= border) {
			std::thread t2([temp2,out2,&N]() {FFT(temp2,temp2+N,out2);});
			FFT(temp1, temp1 + N, out1);
			delete[] temp1;
			t2.join();
			delete[] temp2;
		}

		else {
			FFT(temp1, temp1 + N, out1);
			FFT(temp2, temp2 + N, out2);
			delete[] temp1;
			delete[] temp2;
		}

		for (int k = 0; k < N; k++) {
			*res = (out1[k] + wk * out2[k]);
			wk *= w;
			++res;
		}
		wk = 1;
		for (int k = 0; k < N; k++) {
			*res = (out1[k] - wk * out2[k]);
			wk *= w;
			++res;
		}
		delete[] out1;
		delete[] out2;
	}

	else {
		complex y1 = *first;
		++first;
		complex y2 = *first;
		*res = (y1 + y2);
		++res;
		*res = (y1 - y2);
	}
}

template<class iter, class outputIter>
inline void IFFT(iter first, iter last, outputIter res) {
	int n = std::distance(first, last);
	int N = n / 2;
	double s = .5;
	const double PI = 3.14159265358979323846;
	if (n != 2) {
		complex* temp1 = new complex[N];
		complex* temp2 = new complex[N];
		complex* out1 = new complex[N];
		complex* out2 = new complex[N];
		for (int i = 0; i < N; ++i) {
			temp1[i] = *first;
			++first;
			temp2[i] = *first;
			++first;
		}
		const complex J(0, 1);
		complex w = exp(2.0 * PI * J / (double) n);
		complex wk = 1.0;

		if (n >= border) {
			std::thread t2([temp2,out2,&N]() {IFFT(temp2,temp2+N,out2);});
			IFFT(temp1, temp1 + N, out1);
			delete[] temp1;
			t2.join();
			delete[] temp2;
		}

		else {
			IFFT(temp1, temp1 + N, out1);
			IFFT(temp2, temp2 + N, out2);
			delete[] temp1;
			delete[] temp2;
		}

		for (int k = 0; k < N; k++) {
			*res = s * (out1[k] + wk * out2[k]);
			wk *= w;
			++res;
		}
		wk = 1.0;
		for (int k = 0; k < N; k++) {
			*res = s * (out1[k] - wk * out2[k]);
			wk *= w;
			++res;
		}
		delete[] out1;
		delete[] out2;
	}

	else {
		complex y1 = *first;
		++first;
		complex y2 = *first;
		*res = s * (y1 + y2);
		++res;
		*res = s * (y1 - y2);
	}
}

