#include "Problema.h"

int main(int, const char *[]) {
	Problema Prob("problema.txt");
	Prob.Rodar();
	Prob.ImprimirPadrao();

	system("pause");
	return 0;
}