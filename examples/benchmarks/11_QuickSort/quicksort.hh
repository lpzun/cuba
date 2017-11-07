#include <iostream>
#include <pthread.h>
#define max 20
using namespace std;

namespace sort {
class QuickSort {
	int *A;
	int n;

public:
	int high;
	int low;
	QuickSort(int n1, int *a) :
			high(0), low(0) {
		n = n1;
		A = a;
	}
	QuickSort(QuickSort *obj) :
			high(0), low(0) {

		this->n = obj->n;
		this->A = obj->A;
	}

	void quicksort(int, int);
	int partition(int, int);

	static void* thread_function(void *ptr) {
		QuickSort* q = static_cast<QuickSort*>(ptr);
		q->quicksort(q->low, q->high);
	}
};

void QuickSort::quicksort(int low, int high) {
	int piv_index;
	pthread_t th1, th2;
	QuickSort *q1, *q2;
	if (low < high) {
		piv_index = partition(low, high);

		q1 = new QuickSort(this);
		q2 = new QuickSort(this);

		q1->low = low;
		q1->high = piv_index - 1;
		q2->low = piv_index + 1;
		q2->high = high;

		void *obj1 = reinterpret_cast<void *>(q1);
		void *obj2 = reinterpret_cast<void *>(q2);

		pthread_create(&th1, NULL, QuickSort::thread_function, (void *) obj1);
		pthread_create(&th2, NULL, QuickSort::thread_function, (void *) obj2);

		pthread_join(th1, NULL);
		pthread_join(th2, NULL);
	}

}

int QuickSort::partition(int low, int high) {
	int i, j, pivot;

	pivot = A[low];
	i = low + 1;
	j = high;

	while (i <= j) {
		while (A[i] <= pivot)
			i++;

		while (A[j] > pivot)
			j--;

		if (i < j) {
			int temp = A[i];
			A[i] = A[j];
			A[j] = temp;
		}
	}

	A[low] = A[j];
	A[j] = pivot;

	return j;
}

int test() {
	int no;
	cout << "Enter the size of array: " << endl;
	cin >> no;
	QuickSort *q;
	int arr[max];

	for (int i = 0; i < no; i++) {
		cout << "Enter data: " << endl;
		cin >> arr[i];
	}

	q = new QuickSort(no, arr);
	cout << "Before Sorting: " << endl;
	for (int i = 0; i < no; ++i)
		cout << arr[i] << " ";
	cout << endl;
	q->low = 0;
	q->high = no - 1;
	void *ptr = reinterpret_cast<void*>(q);
	QuickSort::thread_function(ptr);

	cout << "After Sorting: " << endl;
	for (int i = 0; i < no; ++i)
		cout << arr[i] << " ";
}
}
