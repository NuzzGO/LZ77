#include <string>
#include <fstream>
#include <tuple>
struct Parametry
{
	std::string plik_wej_sciezka, plik_wyj_sciezka;
	int slownik;
	int tryb = 0;
};
class Algorithm
{
protected:
	int D_param;
	std::string Name_file_in, Name_file_out, Data;
public:
	Algorithm();
	Algorithm(std::string, std::string, int param);//przypisanie zmiennych
	~Algorithm() {};
	virtual std::string LZ77(const std::string& input) = 0;
	bool Open();//Otwarcie plikow w trybie binarnym
	bool Save();//zapis binarny danych
};
class Compression : public Algorithm
{
public:
	Compression(std::string, std::string, int param);//konstruktor otwiera plik wejsciowy i zapisuje dane do pliku wyjsciwoego.
	~Compression() {};
	std::string LZ77(const std::string& input);
	std::pair<int, int> GetMatch(const std::string& dictionary, const std::string& lookahead_buffer);//szuka najwiekszego dopasowania
};
class Decompression : public Algorithm
{
public:
	Decompression(std::string, std::string, int param);//konstruktor otwiera plik wejsciowy i zapisuje dane do pliku wyjsciwoego.
	~Decompression() {};
	std::string LZ77(const std::string& input);
};

bool Odczytaj(int argc, char* argv[], Parametry& param);//Odczytuje parametry i tryb pracy.
int B2D(std::string bin);//zamienia lcizbe binarna na dziesiatna
std::string D2B(int dec, int nb_bits = -1);//zamienia liczbe dziesiatna na binarna