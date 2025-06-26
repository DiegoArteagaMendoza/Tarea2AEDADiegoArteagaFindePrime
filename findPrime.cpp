#include <iostream>
#include <vector>
#include <atomic>
#include <thread>
#include <chrono>
#include <cmath>

// variables atomicas
std::atomic<long> nextNumber(1);
std::atomic<int> primeCount(0);

// es primo?
bool isPrime(long n) {
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;

    for (long i = 3; i * i <= n; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

// Función que ejecuta cada hilo
void worker(long MILLON) {
    while (true) {
        long num = nextNumber.fetch_add(1, std::memory_order_relaxed);
        if (num > MILLON)
            break;
        if (isPrime(num)) {
            primeCount.fetch_add(1, std::memory_order_relaxed);
        }
    }
}

int main() {
    const long MILLON = 1000000;

    std::cout << "Buscando primos hasta " << MILLON << "...\n";

    for (int numThreads : {1, 2, 4, 8, 16}) {
        nextNumber.store(1);
        primeCount.store(0);

        auto start = std::chrono::high_resolution_clock::now();

        std::vector<std::thread> threads;
        for (int i = 0; i < numThreads; ++i) {
            threads.emplace_back(worker, MILLON);
        }

        for (auto &t : threads) {
            t.join();
        }

        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double>(end - start).count();

        std::cout << "Hilos: " << numThreads
                  << " | Primos encontrados: " << primeCount.load()
                  << " | Tiempo: " << duration << " segundos\n";
    }

    return 0;
}