#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

static const char alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";
int stringLength = sizeof(alphanum) - 1;


char genRandomChar()  // Funkcija za generisanje random stringa.
{
	return alphanum[rand() % stringLength];
}

std::string genRandomString(std::string& userID)
{
	srand((unsigned)time(NULL));
	for (int z = 0; z < 32; z++)
	{
		userID += genRandomChar();
	}
	return userID;
}

bool checkUserID(std::string& userID)
{
	std::ifstream usersFile("korisnici.txt");
	std::string fline, fuserID, fusername, fpassword, fnumber;
	while (std::getline(usersFile, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fuserID, '-');
		std::getline(iss, fusername, '-');
		std::getline(iss, fpassword, '-');
		std::getline(iss, fnumber, '\n');
		if (fuserID == userID)
			return false;
	}
	return true;
	usersFile.close();

}

std::string registerUser(std::string username, std::string password)
{
	std::ifstream usersFileInput("korisnici.txt");
	std::string fline, fuserID, fusername, fpassword, fnumber,userID;
	if (usersFileInput.is_open() == false)
		return "Greska,zahtjev nije moguce ispuniti.";
	while (std::getline(usersFileInput, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fuserID, '-');
		std::getline(iss, fusername, '-');
		std::getline(iss, fpassword, '-');
		std::getline(iss, fnumber, '\n');
		if (fusername == username)
			return "Korisnik sa unesenim korisnickim imenom vec postoji.";
	}
	usersFileInput.close();
	std::ifstream adminsFile("admini.txt");
	while (std::getline(adminsFile, fline))
	{
		std::stringstream iss2(fline);
		std::getline(iss2, fuserID, '-');
		std::getline(iss2, fusername, '-');
		std::getline(iss2, fpassword, '\n');
		if (fusername == username)
			return "Korisnik sa unesenim korisnickim imenom vec postoji.";
	}
	adminsFile.close();
	do
	{
		userID = "";
		genRandomString(userID);
	} while (!checkUserID(userID));
	std::fstream usersFileOutput("korisnici.txt", std::ios::app);
	if (usersFileOutput.is_open() == false)
		return "Greska,zahtjev nije moguce ispuniti.";
	usersFileOutput << userID << "-" << username << "-" << password << "-" << "0" << '\n';
	usersFileOutput.close();
	return "Uspjesna registracija";
}
