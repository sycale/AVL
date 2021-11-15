#include <omp.h>
#include <iostream>
#include <experimental/random>
#include <chrono>

using namespace std::chrono;


double scalParallel(double* v1, double *v2, int n) {
	double s = 0.0;
	int i;

    #pragma omp parallel for shared(v1, v2) private(i) reduction (+: s) schedule (static)
	for (i = 0; i < n; i++) {
		s += v1[i] * v2[i];
	}
	return s;
}

double scal(double* v1, double* v2, int n) {
    double s = 0.0;

    for(int i = 0; i < n; i++) {
        s += v1[i] * v2[i];
    }

    return s;
}

double* getRandomVector(int size) {
    double* v = new double[size];

    for(int i = 0; i < size; i++) {
        v[i] = std::experimental::randint(0, 10);
    }

    return v;
}

void printArr(double* arr, int size) {
    for(int i = 0; i < size; i++) {
        std::cout << "Arr[" << i << "] - " << arr[i] << std::endl;
    }
}

int main() {
    int size;

    std::cout << "Array size:";

    std::cin >> size;

    double* v1 = getRandomVector(size);
    double* v2 = getRandomVector(size);

    // printArr(v1, size);
    // printArr(v2, size);

    auto start = high_resolution_clock::now();

    double resPar = scalParallel(v1, v2, size);

    auto stop = high_resolution_clock::now();

    auto parallelDuration = duration_cast<microseconds>(stop - start);

    start = high_resolution_clock::now();

    double resDef = scal(v1, v2, size);

    stop = high_resolution_clock::now();

    auto defaultDuration = duration_cast<microseconds>(stop - start);

    std::cout << "Default - " << resDef << std::endl << "Par - " << resPar << std::endl;
    std::cout << "Parallel - " << parallelDuration.count()  << std::endl << "Default - " << defaultDuration.count() << std::endl;

    delete v1;
    delete v2;
}
