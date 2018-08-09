#pragma once
#include <ilcp/cp.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <list>

using namespace std;


class Problema
{
private:
	IloEnv env;
	IloModel model;
	IloInt W, V, Gamma;
	IloNum epsilon;
	IloIntVar w;
	IloIntVarArray A;
	IloNum Maior_Barra;
	const char* nome_instancia;
	IloNumArray b, L;
	void Iniciar_Modelo();
	void Resolver();
	
	list<list<int>> Padroes;

public:
	Problema(const char* filename);
	
	void Rodar();
	void ImprimirPadrao();
	~Problema();
};

