#pragma once
#include <ilcp/cp.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <list>
#include <vector>

using namespace std;

struct Tipo_Viga {
	int tempo_cura, n_comprimentos, n_barras;
	vector<int> demandas;
	vector<double> comprimentos;
};

class Problema
{
private:
	IloEnv env;
	IloModel model;
	IloInt W, V, Gamma;
	IloNumArray b;
	vector<Tipo_Viga> Tipos_de_Viga;


	IloInt M, C, T;
	IloInt e, k;
	vector<double> FORMAS;
	vector<double> L;
	IloNumArray l;
	IloIntArray d;
	IloIntArray estoque;
	IloNum Maior_Forma, Menor_Forma;
	IloInt Maior_Qc;
	IloNumArray Menor_tamanho;
	//Variaveis de Decisão Dependentes do Gerados de Packing
	IloIntVar gamma; //Variável auxiliar para gerar apenas maximais
	IloIntVar c;
	IloNumVar cap;

	//Variaveis de Decisão Dependentes do Gerador de Cortes
	IloIntVar w;
	IloIntVarArray A;
	IloNum Maior_Barra;

	//Variaveis de decisão dependetes do gerador de traspasse
	//Não há adicional apenas redefinição
	IloNumVar folga;
	IloNum epsilon;
	
	list<list<double>> Padroes;

	const char* nome_instancia;
	


	void Resolver_Cortes();
	void Resolver_Packing();
	
	void Iniciar_Modelo_Cortes();
	void Iniciar_Modelo_Packing();

	void Splicing_Model_Initiate();
	void Splicing_Solve();

public:

	Problema(const char* filename);
	
	void Rodar_Cortes();
	void ImprimirPadrao_Corte();

	void Rodar_Packing();
	void ImprimirPadrao_Packing();
	
	void Rodar_Spl();
	void ImprimirPadrao_Splicing();
};

