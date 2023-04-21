#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <algorithm>
#include <tuple>
#include <sstream>
#include "ClassAndFunctions.h"
using namespace std;


Algorithm::Algorithm() : Name_file_in(""), Name_file_out(""), D_param(20) {};
Algorithm::Algorithm(string IN, string OUT, int param) : Name_file_in(IN), Name_file_out(OUT), D_param(param) {}
bool Algorithm::Open()//Otwarcie plikow w trybie binarnym
{
	ifstream File_in;
	File_in.open(Name_file_in, ios_base::binary | ios::in); //otwarcie pliku
	if (!File_in)//jesli nie mozna otworzyc pliku
	{
		return false;
	}
	int n = 1024;
	char bufor[1024];
	while (true)
	{
		File_in.read(bufor, n);
		for (int i = 0; i < File_in.gcount(); i++)
		{
			Data = Data + bufor[i]; //zapis danych
		}
		if (File_in.gcount() < 1024)
		{
			break;
		}
	}
	File_in.close();//zamkniecie pliku
	return true;
};
bool Algorithm::Save()//zapis binarny danych
{
	ofstream File_out;
	File_out.open(Name_file_out, ios_base::binary | ios::out); //otawrcie pliku do zapisu
	if (!File_out) //jesli nie mozna otworzyc pliku
	{
		return EXIT_FAILURE;
	}
	for (int i = 0; i < Data.length(); i++)//zapis danych
	{
		File_out.put(Data[i]); 
	}
	File_out.close();//zamkniecei pliku
};
Compression::Compression(string IN, string OUT, int param) : Algorithm(IN, OUT, param)//konstruktor otwiera plik wejsciowy i zapisuje dane do pliku wyjsciwoego.
{
	Open();
	Data = LZ77(Data);
	Save();
};
Decompression::Decompression(string IN, string OUT, int param) : Algorithm(IN, OUT, param)//konstruktor otwiera plik wejsciowy i zapisuje dane do pliku wyjsciwoego.
{
	Open();
	Data = LZ77(Data);
	Save();
};
bool Odczytaj(int argc, char* argv[], Parametry& param)//Odczytuje parametry i tryb pracy.
{
	string param_in = "-i";
	string param_out = "-o";
	string tryb_k = "-k";
	string tryb_d = "-d";
	string param_s = "-s";
	param.tryb = 0;
	int i;
	for (i = 1; i < argc; ++i) //Szuka parametrów i nazw plików
	{
		if (argv[i] == param_in)
		{
			param.plik_wej_sciezka = argv[++i];
		}
		else if (argv[i] == param_out)
		{
			param.plik_wyj_sciezka = argv[++i];
		}
		else if (argv[i] == param_s)
		{
			istringstream  g(argv[++i]);
			g >> param.slownik;
		}
		else if (argv[i] == tryb_d)
		{
			param.tryb = 2;
		}
		else if (argv[i] == tryb_k)
		{
			param.tryb = 1;
		}
	}
	if (param.plik_wej_sciezka == "" || param.plik_wyj_sciezka == ""  || param.tryb == 0)
	{
		throw 5;
	}
	return true;
};
int B2D(std::string bin)//zamienia lcizbe binarna na dziesiatna
{
	int dec = 0;
	int n = bin.size();
	int k = 1;
	for (int i = 0; i < n; i++)
	{
		if (bin[n - 1 - i] == '1')
			dec += k;
		k *= 2;
	}
	return dec;
};
std::string D2B(int dec, int nb_bits)//zamienia liczbe dziesiatna na binarna
{
	string bin;
	while (dec != 0) //dla wszystkich bitów
	{
		if (dec % 2 == 0) //dla 0
		{
			bin = "0" + bin;
			dec /= 2;
		}
		else //dla 1
		{
			bin = "1" + bin;
			dec = (dec - 1) / 2;
		}
	}

	if (nb_bits > 0 && bin.size() < nb_bits) //wype³nienie bitami do pe³nej jednostki
	{
		string fill(nb_bits - bin.size(), '0');
		return fill + bin;
	}
	return bin;
};
std::pair<int, int> Compression::GetMatch(const std::string& dictionary, const std::string& lookahead_buffer)//szuka najwiekszego dopasowania, zwraca  pozycje i dlugosc dopasowania
{
	int p = 0; 
	int n = 0;

	int dict_size = dictionary.size(); //wielksoc slownika
	int buffer_size = lookahead_buffer.size(); //wiekosc bufora

	for (int q = 0; q < dictionary.size(); q++)
	{
		int local_n = 0;
		while (local_n < std::min(buffer_size, dict_size - q) && dictionary[q + local_n] == lookahead_buffer[local_n])   //sprawdzenie warunków, czy mieœcimy siê w przedziale
			local_n++;
		if (local_n == dict_size - q)
		{
			int buffer_index = 0;
			while (local_n < buffer_size && lookahead_buffer[buffer_index] == lookahead_buffer[local_n])
			{
				buffer_index++;
				local_n++;
			}
		}
		if (local_n > n)
		{
			n = local_n;
			p = dict_size - q;
		}
	}
	return std::make_pair(p, n); //zwraca pare miejsca i d³ugoœci
};
std::string Compression::LZ77(const std::string& input)
{
	size_t cursor = 0; //pozycja
	size_t input_size = input.size(); //wejœcie

	int dictionary_size = pow(2, D_param); //wielkoœc s³ownika
	int buffer_size = pow(2, 8) + 2; //wielkosc bufora

	string output, output_bin; //wyjœcie
	size_t output_bin_cursor = 0; //cursor wyjœcia

	while (cursor < input_size) //dopóki nie dojdziemy do koñca pliku 
	{
		string dictionary = input.substr(std::max(0, (int)cursor - dictionary_size), std::min(cursor, (size_t)dictionary_size)); //ustalenie s³ownika
		string buffer = input.substr(cursor, buffer_size); //ustalenie bufora

		pair<int, int> x = GetMatch(dictionary, buffer); //najwiêksze doposowanie

		int p = x.first; 
		int n = x.second; 
		if (n < 3) //jesli przepisywany bajt
		{
			output_bin += "0";
			output_bin += D2B((int)(unsigned char)input[cursor], 8);
			cursor++;
		}
		else //podmianka
		{
			output_bin += "1";
			output_bin += D2B(p - 1, D_param);
			output_bin += D2B(n - 3, 8);
			cursor += n;
		}
		int output_bin_size = output_bin.size();
		while (output_bin_size - output_bin_cursor >= 8) //przepisanie 0 i 1 na bajty
		{
			string c = output_bin.substr(output_bin_cursor, 8);
			output += (char)B2D(c);
			output_bin_cursor += 8;
		}
		if (output_bin_size > output_bin.max_size() - 100)
		{
			output_bin.erase(0, cursor);
			output_bin_cursor = 0;
		}
	}
	output_bin.erase(0, output_bin_cursor);
	if (!output_bin.empty()) //wyrównianie do pe³nego ci¹gu
	{
		string fill_byte(8 - output_bin.size(), '0');
		output_bin += fill_byte;
		string c = output_bin.substr(0, 8);
		output += (char)B2D(c);
	}
	return output;
};
std::string Decompression::LZ77(const std::string& input)
{
	size_t input_size = input.size();
	size_t input_cursor = 0;
	size_t input_bin_cursor = 0;
	string input_bin;
	while (input_cursor < input_size && input_bin.size() < input_bin.max_size() - 100)
	{
		input_bin += D2B((int)(unsigned char)input[input_cursor], 8);
		input_cursor++;
	}
	string output;
	while (!input_bin.empty())
	{
		char type = input_bin[input_bin_cursor];
		input_bin_cursor++;
		if (input_bin_cursor > input_bin.size() || input_bin.size() - (int)input_bin_cursor < 8)
			return output;
		if (type == '0') //przepisywanie danych
		{
			string c = input_bin.substr(input_bin_cursor, 8);
			output += (char)B2D(c);
			input_bin_cursor += 8;
		}
		else //szukanie odpowiedniego podci¹gu
		{
			int p = B2D(input_bin.substr(input_bin_cursor, D_param)) + 1;
			input_bin_cursor += D_param;
			int n = B2D(input_bin.substr(input_bin_cursor, 8)) + 3;
			input_bin_cursor += 8;
			size_t match_begin = output.size() - p; 
			for (int k = 0; k < n; k++)
				output += output[match_begin + k];
		}
		if (input_bin.size() - (int)input_bin_cursor < 100 && input_cursor < input_size)//przepisanie danych, podciagu 010101.. itp. na bajty
		{
			input_bin.erase(0, input_bin_cursor);
			input_bin_cursor = 0;

			while (input_cursor < input_size && input_bin.size() < input_bin.max_size() - 100)
			{
				input_bin += D2B((int)(unsigned char)input[input_cursor], 8);
				input_cursor++;
			}
		}
	}
	return output;
};