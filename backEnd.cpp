#include "backEnd.h"

static const char alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";
int stringLength = sizeof(alphanum) - 1;


std::vector<struct session> sessions;
std::string fileOpeningError = "Greska,zahtjev nije moguce ispuniti.",
successfulLogin = "Uspjesno prijavljivanje.", unsuccessfulLogin = "Neuspjesno prijavljivanje.",
successfulBan = "Korisnik uspjesno banovan.", unsuccessfulBan = "Korisnik sa takvim imenom ne postoji.";

bool authenticate(std::string sessionID)
{
	//provjeriti da li globalni vektor zvan "sessions", koristiti for(auto &counter : sessions) petlju
		//sadrzi sessionID pa vratiti odgovarajuci bool
}

bool isAdmin(std::string sessionID)
{
	//isto kao funkcija authenticate, provjeriti da li je proslijedjeni sessionID od administratora
	//povratna vrijednost je bool i false je za neuspjesno logovanje, true u protivnom
}

std::string login(std::string username, std::string password)
{
	std::string fuserID, fusername, fpassword, fnumber, fline;
	std::ifstream users("korisnici.txt");
	if (users.is_open() == false)
		return fileOpeningError;
	while (std::getline(users, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fuserID, '-');
		std::getline(iss, fusername, '-');
		std::getline(iss, fpassword, '-');
		std::getline(iss, fnumber, '\n');
		int fnum = std::stoi(fnumber);
		if (fusername == username && fpassword == password)
		{
			std::string sessionID = "";
			do
			{
				genRandomString(sessionID);
			} while (checkSessionID(sessionID));
			struct session newSession;
			newSession.sessionID = sessionID;
			newSession.userID = fuserID;
			newSession.userName = fusername;
			newSession.userRank = fnum;
			sessions.push_back(newSession);
			return successfulLogin;
		}

	}
	users.close();
	std::ifstream admins("admini.txt");
	if (admins.is_open() == false)
		return fileOpeningError;
	while (std::getline(admins, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fuserID, '-');
		std::getline(iss, fusername, '-');
		std::getline(iss, fpassword, '\n');
		if (fusername == username && fpassword == password)
		{
			std::string sessionID = "";
			do
			{
				genRandomString(sessionID);
			} while (checkSessionID(sessionID));
			struct session newSession;
			newSession.sessionID = sessionID;
			newSession.userID = fuserID;
			newSession.userName = fusername;
			newSession.userRank = 1;
			sessions.push_back(newSession);
			return successfulLogin;
		}
	}
	admins.close();
	return unsuccessfulLogin;
}

std::string eventComment(std::string comment, std::string eventName)
{
	//staviti komentar comment u dogadjaj sa nazivom eventName
	//voditi racuna o formatu u fajlu komentari.txt
	//povratna vrijednost je poruka o uspjesnosti, koristiti neku globalnu promjenljivu
}

std::string banUser(std::string userName)
{
	std::string fuserID, fusername, fpassword, fnumber, fline, fileText = "";
	int flag=0;
	std::ifstream users("korisnici.txt");
	if (users.is_open() == false)
		return fileOpeningError;
	while (std::getline(users, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fuserID, '-');
		std::getline(iss, fusername, '-');
		std::getline(iss, fpassword, '-');
		std::getline(iss, fnumber, '\n');
		int fnum = std::stoi(fnumber);
		if (fusername == username)
		{
			if (fnum == 0)
			{
				fnumber = "1";
				flag = 1;
			}
				
		}
		fileText += fuserID + '-' + fusername + '-' + fpassword + '-' + fnumber + '\n';
	}
	users.close();
	if (flag == 1)
	{
		std::ofstream users("korisnici.txt");
		users << fileText;
		users.close();
		return successfulBan;
	}
	else
		return unsuccessfulBan;
}

struct quiz getQuizInfo()
{
	//POPUNITI STRUKTURU STRUCT QUIZ
	//ovo je funkcija koja pribavlja podatke o kvizu za administratora
	//vratiti tu popunjenu strukturu
}

void editQuiz(struct quiz newQuiz)
{
	//FUNKCIJA ZA ADMINISTRATORA, KORISTI SE ZA PROMJENU KVIZA
}

std::vector<std::string> getCategories()
{
	std::vector<std::string> categories;
	std::string tmp;
	std::ifstream fcategories("kategorije.txt");
	if (fcategories.is_open() == false)
	{
		categories.push_back(fileOpeningError);
		return categories;
	}
	while (!fcategories.eof())
	{
		std::getline(fcategories, tmp, '-');
		categories.push_back(tmp);
	}
	fcategories.close();
	return categories;
}

void changeCategories(std::vector<std::string> newCategories)
{
	unsigned int i;
	std::ofstream categories("kategorije.txt");
	for (i = 0; i < newCategories.size()-1; i++)
		categories << newCategories[i] << "-";
	categories << newCategories[i];
}

struct quiz4Players getQuestions4Player()
{
	//FUNKCIJA KOJA PRIBAVLJA PITANJA I PONUDJENE ODGOVORE ZA IGRACE
}

int checkPlayersAnswers(struct quiz)
{
	//FUNKCIJA KOJA PROVJERAVA KOLIKO JE ODGOVORA ISPRAVNO
	//VRATITI BROJ ISPRAVNIH ODGOVORA
}

std::string removeComment(std::string commentID, std::string eventName)
{
	//ADMINISTRATORSKA FUNKCIJA ZA UKLANJANJE KOMENTARA
	//U FOLDERU eventName SE NALAZI FAJL komentari.txt
	//UKLONITI KOMENTAR U REDU SA commentID
}


std::vector<eventList> getEvents(struct eventFilter)
{
	
	//prihvaca strukturu eventFilter koja daje informacije o tome sta korisnik zeli da trazi, vraca se vektor sa strukturama od kojih svaka daje informacije o nekom dogadjaju
	
}

struct event getEvent(std::string eventName)
{
	
	//prihvata sve informacije o konkretnom dogadjaju
	

}


std::string addEvent(struct newEvent &event2Add)
{
	
	//funkcija za dodavanje dogadjaja u bazu podataka
	//provjerava da li dogadjaj vec postoji uporedjivanjem imena i svih ostalih podataka (vrijeme, datum, mjesto i kategorija)
	//ako dogadjaj vec postoji, vratiti odgovarajucu poruku

	//povratna vrijednost je poruka o uspjesnosti
	

}

char genRandomChar()  // Funkcija za generisanje random stringa.
{

	return alphanum[rand() % stringLength];
}

std::string genRandomString(std::string& sessionID)
{
	for (int z = 0; z < 32; z++)
	{
		sessionID += genRandomChar();
	}
	return sessionID;
}

bool checkSessionID(std::string& sessionID)
{
	for (unsigned int i=0; i<sessions.size(); ++i)
		if (sessionID == sessions[i].sessionID)
			return true;
	return false;
}
