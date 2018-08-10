#include "Problema.h"

int main(int, const char *[]) {
	Problema Prob("problema.txt");
	Prob.Rodar_Packing();
	Prob.ImprimirPadrao_Packing();
	/*Prob.Rodar_Cortes();
	Prob.ImprimirPadrao_Corte();*/

	system("pause");
	return 0;
}