#include "Problema.h"
#include <algorithm>

#define EPSILON 0.0005

Problema::Problema(const char* filename){
	ifstream instancia(filename, ifstream::in);
	if (instancia.fail()) {
		cerr << "     Arquivo \"" << filename << "\" nao encontrado." << endl;
		getchar();
		exit(1);
	}

	/*Leitura dos dados e alocação*/

	nome_instancia = filename;
	instancia >> C >> M >> T; //Só usa para padroes de empacotamento
	instancia >> W >> V; //Só Usa para padroes de corte

	L.resize(M); //vai ser vetor unique
	FORMAS.resize(M);
	Tipos_de_Viga.resize(C);
	Maior_Forma = 0;
	Menor_Forma = INT_MAX;

	//Ler vetor das formas e seus tamanhos
	for (int i = 0; i < M; i++) {
		instancia >> FORMAS[i];
		if (FORMAS[i] > Maior_Forma)
			Maior_Forma = FORMAS[i];
		if ((FORMAS[i] < Menor_Forma))
			Menor_Forma = FORMAS[i];
	}

	L = FORMAS;
	sort(L.begin(), L.end());

	/*Calculo do numero de tamanhos de forma diferente e seu vetor*/
	auto iterador_auxiliar = unique(L.begin(), L.end());//Tirar duplicados
	L.resize(distance(L.begin(), iterador_auxiliar));//Mudar o tamanho

	Gamma = L.size();//Gamma é o tamanho do vetor de comprimentos únicos de forma

	/*Para cada tipo de viga ler os n_tamanhos, tamanhos, demandas, cura e n_barras 
	necessárias*/
	Maior_Qc = 0;
	Menor_tamanho = IloNumArray(env, C);
	for (int i = 0; i < C; i++) {
		Menor_tamanho[i] = 100;

		instancia >> Tipos_de_Viga[i].tempo_cura 
			>> Tipos_de_Viga[i].n_comprimentos
			>> Tipos_de_Viga[i].n_barras;
		Tipos_de_Viga[i].demandas.resize(Tipos_de_Viga[i].n_comprimentos);
		Tipos_de_Viga[i].comprimentos.resize(Tipos_de_Viga[i].n_comprimentos);

		if (Tipos_de_Viga[i].n_comprimentos > Maior_Qc)
			Maior_Qc = Tipos_de_Viga[i].n_comprimentos;

		

		//Ler comprimentos
		for (int k = 0; k < Tipos_de_Viga[i].n_comprimentos; k++) {
			instancia >> Tipos_de_Viga[i].comprimentos[k];
			if (Tipos_de_Viga[i].comprimentos[k] < Menor_tamanho[i])
				Menor_tamanho[i] = Tipos_de_Viga[i].comprimentos[k];
		}

		//Demandas dos comprimentos
		for (int k = 0; k < Tipos_de_Viga[i].n_comprimentos; k++)
			instancia >> Tipos_de_Viga[i].demandas[k];

	}

	b = IloNumArray(env, W + V); //Alocar vetor com os tamanhos únicos de barras

	Maior_Barra = 0; //Será usado para calcular os upper bounds das variáveis de decisão

	for (int i = 0; i < W + V; i++) {
		instancia >> b[i];
		cout << b[i] << " ";
		if (b[i] > Maior_Barra)
			Maior_Barra = b[i];
	}

	estoque = IloIntArray(env, W + V);
	for (int i = 0; i < W + V; i++) {
		instancia >> estoque[i];
		cout << estoque[i] << " ";
	}

	instancia >> epsilon;
	cout << endl << epsilon << endl;

	cout << endl;
	instancia.close();
}

void Problema::Iniciar_Modelo_Cortes() {
	/*Alocar variáveis*/
	model = IloModel(env);
	w = IloIntVar(env, 0, W + V - 1, "w");
	cap = IloNumVar(env, 0, Maior_Barra, "cap");
	A = IloIntVarArray(env, Gamma + V);
	
	/*Preste atenção que aqui voce fez um range diferente para cada tipo de barra
	para ser proporcional ao tamanho da barra em relação a maior barra. A ideia seria:
	Quantas barras dessa cabem na maior barra no máximo?
	*/
	for (IloInt i = 0; i < Gamma + V; i++) {
		if (i < Gamma) 
			A[i] = IloIntVar(env, 0, Maior_Barra / L[i]);
		
		else 
			A[i] = IloIntVar(env, 0, Maior_Barra / b[W + i - Gamma]);
		
	}

	IloExpr soma1(env), soma2(env), aaa(env);

	for (int i = 0; i < Gamma; i++) {
		soma1 += L[i] * A[i];
		cout << L[i] << endl;
	}

	for (int i = 0; i < V; i++) {
		soma2 += b[W + i] * A[Gamma + i];
		cout << b[W + i] << endl;
	}

	model.add(soma1 + soma2 == cap);

	model.add(cap  <= b[w]);

	model.add(cap >= FLT_EPSILON);

	soma1.end();
	soma2.end();

	for (int i = Gamma; i < Gamma + V; i++){
		for (int j = Gamma; j < Gamma + V; j++) {
			if(i != j)
				model.add(IloIfThen(env, A[i] > 0, A[j] == 0));
		}
	}

	for (int i = Gamma; i < Gamma + V; i++) 
		model.add(IloIfThen(env, w >= W, A[i] == 0));

}

void Problema::Resolver_Cortes() {
	//Passar por cada solução e a guarda em uma lista de padrões que será impressa após
	try {

		IloCP cp(model);
		cp.propagate();
		cp.setParameter(IloCP::LogVerbosity, IloCP::Quiet);
		cout << cp.domain(A) << endl;
		//cp.exportModel("modelo.cpo");
		cp.startNewSearch();
		while (cp.next()) {
			list<double> auxiliar;
			auxiliar.push_back(cp.getValue(w));
			auxiliar.push_back(cp.getValue(cap));

			for (int i = 0; i < Gamma + V; i++)
				auxiliar.push_back(cp.getValue(A[i]));

			Padroes.push_back(auxiliar);
		}

	}
	catch (IloException& ex) {
		env.out() << "Error: " << ex << endl;

	}
	env.end();

	
}

void Problema::Rodar_Cortes() {
	/*Iniciar variáveis e montar o modelo*/
	Iniciar_Modelo_Cortes();
	/*Resolver o modelo em si*/
	Resolver_Cortes();
}

void Problema::ImprimirPadrao_Corte() {
	/*Imprimir padroes no arquivo*/
	string arquivo_saida = ".cutpat";
	stringstream ss;
	ss << nome_instancia << arquivo_saida;
	arquivo_saida = ss.str();

	ofstream saida(arquivo_saida);
	saida << Padroes.size() << endl;
	for(auto elemento: Padroes){
		for (auto gene : elemento)
			saida << gene << " ";
		saida << endl;
	}
	saida.close();

}

void Problema::Iniciar_Modelo_Packing() {
	/*Alocar variáveis*/
	model = IloModel(env);
	c = IloIntVar(env, 0, C - 1, "c");
	A = IloIntVarArray(env, Maior_Qc, 0, 100);
	gamma = IloIntVar(env, 0, Gamma - 1, "gamma");
	cap = IloNumVar(env, 0, Maior_Forma, "cap");

	for (int i = 0; i < C; i++){
		for (int k = Tipos_de_Viga[i].n_comprimentos; k < Maior_Qc; k++){
			model.add(IloIfThen(env, c == i, A[k] == 0));
		}
		IloExpr soma(env);
		for (int k = 0; k < Tipos_de_Viga[i].n_comprimentos; k++) {
			soma += Tipos_de_Viga[i].comprimentos[k] *
				A[k];
		}

		//model.add(IloIfThen(env, c == i, Menor_Forma - Menor_tamanho[i] + FLT_EPSILON <= soma));
		for (int m = 0; m < Gamma; m++)
		{
			model.add(IloIfThen(env, c == i && m == gamma, L[m] - Menor_tamanho[i] + FLT_EPSILON <= soma));
			model.add(IloIfThen(env, c == i && m == gamma, soma <= L[m]));
			model.add(IloIfThen(env, c == i && m == gamma, cap == soma));
		}
	}
}

void Problema::Rodar_Packing() {
	/*Iniciar variáveis e montar o modelo*/
	Iniciar_Modelo_Packing();
	/*Resolver o modelo em si*/
	Resolver_Packing();
}

void Problema::Resolver_Packing() {
	//Passar por cada solução e a guarda em uma lista de padrões que será impressa após
	try {

		IloCP cp(model);
		cp.propagate();
		cp.setParameter(IloCP::LogVerbosity, IloCP::Quiet);
		cout << cp.domain(A) << endl;

		cp.startNewSearch();
		while (cp.next()) {
			list<double> auxiliar;
			auxiliar.push_back(cp.getValue(c));

			auxiliar.push_back(cp.getValue(cap));

			for (int i = 0; i < Tipos_de_Viga[cp.getValue(c)].n_comprimentos; i++)
				auxiliar.push_back(cp.getValue(A[i]));

			Padroes.push_back(auxiliar);
		}

	}
	catch (IloException& ex) {
		env.out() << "Error: " << ex << endl;
	}
	env.end();
}

void Problema::ImprimirPadrao_Packing() {

	/*Imprimir padroes no arquivo*/
	string arquivo_saida = ".pat";
	stringstream ss;
	ss << nome_instancia << arquivo_saida;
	arquivo_saida = ss.str();

	ofstream saida(arquivo_saida);

	saida << Padroes.size() + 1 << endl << "0 0 ";
	for (int i = 0; i < Tipos_de_Viga[0].n_comprimentos; i++)
		saida << "0 ";
	saida << endl;

	for (auto elemento : Padroes) {
		for (auto gene : elemento)
			saida << gene << " ";
		saida << endl;
	}
	saida.close();

}

void Problema::Splicing_Model_Initiate() {
	model = IloModel(env);
	A = IloIntVarArray(env, V, 0, 2);


	for (int i = 0; i < V; i++) {
		stringstream ss;
		ss << "A[" << i << "]";
		string s_aux = ss.str();
		A[i].setName(s_aux.c_str());
	}


	gamma = IloIntVar(env, 0, Gamma - 1, "gamma");
	folga = IloNumVar(env, 0, 10000, "folga");

	IloExpr soma(env), soma2(env);

	for (int i = 0; i < V; i++)
		soma += A[i] * b[W + i];

	for (int m = 0; m < Gamma; m++) {
		model.add(IloIfThen(env, m == gamma, soma >= L[m] + epsilon));
		model.add(IloIfThen(env, m == gamma, folga == soma - L[m]));
	}
	soma.end();

	for (int i = 0; i < V; i++)
		soma2 += A[i];
	model.add(soma2 == 2);

	soma2.end();
}

void Problema::Splicing_Solve() {
	//Passar por cada solução e a guarda em uma lista de padrões que será impressa após
	try {

		IloCP cp(model);
		//cp.exportModel("modelo.cpo");
		cp.propagate();
		cp.setParameter(IloCP::LogVerbosity, IloCP::Quiet);
		cout << cp.domain(A) << endl;
		
		cp.startNewSearch();
		while (cp.next()) {
			list<double> auxiliar;
			auxiliar.push_back(cp.getValue(gamma));

			auxiliar.push_back(cp.getValue(folga));

			for (int i = 0; i < V; i++)
				auxiliar.push_back(cp.getValue(A[i]));

			Padroes.push_back(auxiliar);
		}

	}
	catch (IloException& ex) {
		env.out() << "Error: " << ex.getMessage() << endl;
	}
	env.end();
}

void Problema::Rodar_Spl()
{
	Splicing_Model_Initiate();
	Splicing_Solve();
}

void Problema::ImprimirPadrao_Splicing() {

	string arquivo_saida = ".spl";
	stringstream ss;
	ss << nome_instancia << arquivo_saida;
	arquivo_saida = ss.str();

	ofstream saida(arquivo_saida);
	saida << Padroes.size() << endl;
	for (auto elemento : Padroes) {
		for (auto gene : elemento)
			saida << gene << " ";
		saida << endl;
	}
	saida.close();
}
