#include <iostream>
#include <conio.h>
#include <fstream>
#include "ClassAndFunctions.h"

using namespace std;

int main(int argc, char* argv[])
{
	Parametry parametry;
	try {
		Odczytaj(argc, argv, parametry);
		if (parametry.tryb == 1)
		{
			Algorithm* abc = new Compression(parametry.plik_wej_sciezka, parametry.plik_wyj_sciezka,parametry.slownik);
		}
		else if (parametry.tryb == 2)
		{
			Algorithm* bcd = new Decompression(parametry.plik_wej_sciezka, parametry.plik_wyj_sciezka,parametry.slownik);
		}
	}
	catch (int error)
	{
		cout << "Usage: " << argv[0] << "-i <input> -o <output> -s <slownik> -k kompresja -d dekompresja \n";
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}