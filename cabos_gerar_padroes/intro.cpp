#include "Problema.h"

int main(int argc, char *argv[]) {

	char *inst;
	if (argc < 2)
		inst = "problema.txt";
	else {
		if (argc < 3)
			inst = argv[1];
		else {
			cout << "Argumentos demais" << endl;
			exit(0);
		}
	}


	Problema Prob(inst);
	Prob.Rodar_Packing();
	Prob.ImprimirPadrao_Packing();
	/*Prob.Rodar_Cortes();
	Prob.ImprimirPadrao_Corte();*/

	system("pause");
	return 0;
}