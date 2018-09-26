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

	try
	{
		Problema Prob(inst);
		Prob.Rodar_Packing();
		Prob.ImprimirPadrao_Packing();
	}
	catch (const std::exception&e)
	{
		cout << e.what() << endl;
	}


	try
	{
		Problema Prob(inst);
		Prob.Rodar_Cortes();
		Prob.ImprimirPadrao_Corte();
	}
	catch (const std::exception&e)
	{
		cout << e.what() << endl;
	}
	
	try
	{
		Problema Prob(inst);
		Prob.Rodar_Spl();
		Prob.ImprimirPadrao_Splicing();
	}
	catch (const std::exception&e)
	{
		cout << e.what() << endl;
	}

	//system("pause");
	return 0;
}