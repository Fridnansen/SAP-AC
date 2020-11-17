#ifndef RANDOM_H
#define RANDOM_H

//Random Number Generation in C++11

#include <random>

std::default_random_engine & global_urng();
void randomize();
int numAleatorio(int minimo, int maximo);
double numAleatorioReal(double minimo, double maximo);

#endif // RANDOM_H
