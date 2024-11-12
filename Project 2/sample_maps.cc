/*
 * Copyright (C) Mohsen Zohrevandi, 2017
 *               Rida Bazzi 2019
 * Do not share this file with anyone
 */
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "lexer.h"
#include <unordered_map>
#include <set>
#include <string>

using namespace std;

set<string> NT = {"A","B","C"};
set<string> T = {"a","b","c"};

// ordered universe of all symbols
vector<string> universe = {"#", "$", "c", "a", "b"};    

// unordered map of all FIRST sets this needs to be initialized. 
// See the function initializeFIRST()
unordered_map<string, set<string>> FIRST; 

//function that initializes all FIRST sets
void initializeFIRST()
{
	FIRST["#"] = {"#"};
	FIRST["$"] = {"$"};
	FIRST["a"] = {"a"};
	FIRST["b"] = {"b"};
	FIRST["c"] = {"c"};
	FIRST["A"] = {};
	FIRST["B"] = {};
	FIRST["C"] = {};
}

void printSetInUniverseOrder(set<string> st)
{
	// to ensure they are printed in universe order, we 
	// iterate over the universe and print every element
	// that appears in the given set
	for (string s : universe)
		if (st.find(s) != st.end())
			cout << s << " ";
	cout << endl;
}

int main ()
{
	initializeFIRST();

	FIRST["testset"] = {"b", "c"};

	cout << "printing testset" << endl;
	printSetInUniverseOrder(FIRST["testset"]);

	cout << "printing testset after inserting epsilon" << endl;
	FIRST["testset"].insert("#"); 		// adding epsilon to set
	printSetInUniverseOrder(FIRST["testset"]);

	for (auto nt : NT)
	{
		cout << "FIRST" << "[" << nt << "]" << " = ";
		printSetInUniverseOrder(FIRST[nt]);
	}

	for (auto u : universe)
	{
		cout << "FIRST" << "[" << u << "]" << " = ";
		printSetInUniverseOrder(FIRST[u]);
	}

	return 0;
}

