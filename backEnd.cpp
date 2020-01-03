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
successfulBan = "Korisnik uspjesno banovan.", unsuccessfulBan = "Korisnik sa takvim imenom ne postoji.",
commentAdded = "Komentar uspjesno dodan.",
unsuccessfulComment = "Neuspjesno brisanje komentara.", successfulComment="Komentar uspjesno obrisan.",successfulLogOut="Uspjesno odjavljivanje.",
unsuccessfulLogOut="Neuspjesno odjavljivanje.",
successfulEvent="Dogadjaj uspjesno dodan.",alreadyExistingEvent="Dogadjaj sa ovim imenom vec postoji.";

session* authenticate(std::string sessionID)
{
	for (auto& i : sessions)
		if (i.sessionID == sessionID)
			return &i;
	return nullptr;
}
bool isAdmin(std::string sessionID)
{
	for (auto& i : sessions)
		if (i.sessionID == sessionID&&i.userRank==1)
			return true;
	return false;
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
			std::string sessionID;
			do
			{
				sessionID="";
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
			std::string sessionID ;
			do
			{
				sessionID="";
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

std::string eventComment(std::string comment, std::string eventName, std::string sessionID)
{
	std::string fileName = eventName + "\\komentari.txt", commentID="";
	std::fstream comments(fileName,std::ios::app);
	if (!comments)
		return fileOpeningError;
	do
	{
		commentID = "";
		genRandomString(commentID);
	} while (!checkCommentID(commentID,eventName));
	comments << commentID << "-";
	for (auto& i : sessions)
		if (i.sessionID == sessionID)
			comments << i.userName << "-";
	comments << comment << '\n';
	return commentAdded;
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
		if (fusername == userName)
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
	struct quiz quizinfo;
	std::string question, answer, line;
	std::ifstream quizquestions("kvizPitanja.txt");
	std::getline(quizquestions, line);
	int i, k;
	std::stringstream iss(line);
	for (i = 0; i < 10; i++)
	{
		std::getline(iss, question, '-');
		quizinfo.questions.push_back(question);
	}
	for (k = 0; k < 30; k++)
	{
		if (k == 29) std::getline(iss, answer, '\n');
		else std::getline(iss, answer, '-');
		quizinfo.answers.push_back(answer);
	}
	quizquestions.close();
	std::ifstream quizanswers("kvizOdgovori.txt");
	std::getline(quizanswers, line);
	std::stringstream iss2(line);
	for (i = 0; i < 10; i++)
	{
		if (i == 9)
			std::getline(iss2, answer, '\n');
		else
			std::getline(iss2, answer, '-');
		for (k = 3 * i; k < 3 * i + 3; k++)
			if (quizinfo.answers[k] == answer)
				quizinfo.rightAnswers.push_back(k);
	}
	return quizinfo;
}

void editQuiz(struct quiz &newQuiz)
{
	int i;
	std::ofstream quizQuestions("kvizPitanja.txt");
	for (i = 0; i < 10; i++)
		quizQuestions << newQuiz.questions[i] << "-";
	for (i = 0; i < 30; i++)
		if (i == 29)
			quizQuestions << newQuiz.answers[i] << "\n";
		else
			quizQuestions << newQuiz.answers[i] << "-";
	quizQuestions.close();
	std::ofstream quizAnswers("kvizOdgovori.txt");
	for (i = 0; i < 10; i++)
		if (i == 9)
			quizAnswers << newQuiz.answers[newQuiz.rightAnswers[i]] << "\n";
		else
			quizAnswers << newQuiz.answers[newQuiz.rightAnswers[i]] << "-";
	quizAnswers.close();
	
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
	unsigned int i,j,flag=0;
	for (i = 0; i < newCategories.size(); i++)
	{
		for (j = 0; j < newCategories.size(); j++)
			if (newCategories[i] == newCategories[j])
			{
				flag++;
				if (flag != 1)
					newCategories.erase(newCategories.begin()+j);
			}
		flag = 0;

	}
	std::ofstream categories("kategorije.txt");
	for (i = 0; i < newCategories.size()-1; i++)
		categories << newCategories[i] << "-";
	categories << newCategories[i];
}

struct quiz4Players getQuestions4Player()
{
	struct quiz quizInfo;
	struct quiz4Players quizForPlayers;
	quizInfo = getQuizInfo();
	int i;
	for (i = 0; i < 10; i++)
		quizForPlayers.questions.push_back(quizInfo.questions[i]);
	for (i = 0; i < 30; i++)
		quizForPlayers.answers.push_back(quizInfo.answers[i]);
	return quizForPlayers;
}

int checkPlayersAnswers(struct quiz playerQandA)
{
	int k = 0, numOfCorrectAnswers = 0;
	struct quiz quizInfo = getQuizInfo();
	for (int i = 0; i < 10; i++)
	{
		while (playerQandA.questions[i] != quizInfo.questions[k])
			k++;
		if (playerQandA.answers[i] == quizInfo.answers[quizInfo.rightAnswers[k]]) numOfCorrectAnswers++;
		k = 0;

	}
	return numOfCorrectAnswers;
	
	//STRUKTURA playerQandA sadrzi samo pitanja i odgovore KORISNIKA,dok vektor rightAnswers ostaje prazan i zanemaruje se
}

std::string removeComment(std::string commentID, std::string eventName)
{
	std::string fileName = eventName + "\\komentari.txt",fline,fcommentID,fuserName,fcomment,fileText="";
	int flag = 0;
	std::ifstream comments(fileName);
	if (comments.is_open() == false)
		return fileOpeningError;
	while (std::getline(comments, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fcommentID, '-');
		std::getline(iss, fuserName, '-');
		std::getline(iss, fcomment, '\n');
		if (fcommentID != commentID)
		{
			fileText += fcommentID + "-" + fuserName + "-" + fcomment + '\n';
		}
		else
			flag++;
	}
	if (flag != 1)
		return unsuccessfulComment;
	comments.close();
	std::ofstream commentsInput(fileName);
	if (commentsInput.is_open() == false)
		return fileOpeningError;
	commentsInput<< fileText;
	commentsInput.close();
	return successfulComment;
}


std::vector<eventList> getEvents(struct eventFilter)
{
	
	//prihvaca strukturu eventFilter koja daje informacije o tome sta korisnik zeli da trazi, vraca se vektor sa strukturama od kojih svaka daje informacije o nekom dogadjaju
	
}

struct event getEvent(std::string eventName)
{
	std::ifstream userEvent("korisnikDogadjaj.txt");
	std::string fline,fuserID,fevent;
	int flag=0;
	while (std::getline(userEvent, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fevent, '-');
		std::getline(iss, fuserID, '\n');
		if (fevent == eventName)
			flag++;
	}
	if (flag == 0)
	{
		event emptyEvent;
		emptyEvent.category = "";
		return emptyEvent;
	}
	userEvent.close();
	std::ifstream longDescriptionFile(eventName + "\\opis.txt");//mozda bi trebalo provjeriti da li su svi fajlovi pravilno otvoreni
	event requestedEvent;
	std::string longDescription = "", shortDescription = "",fcommentID,fuserName,fcomment;//ali kako signalizirati ako nisu dobro otvoreni, jer ne vraca funkcija poruku
	while (std::getline(longDescriptionFile, fline,'\n'))
		longDescription += fline+'\n';
	requestedEvent.description = longDescription;
	longDescriptionFile.close();
	std::ifstream shortDescriptionFile(eventName + "\\kratakOpis.txt");
	while (std::getline(shortDescriptionFile, fline))
		shortDescription += fline + '\n';
	requestedEvent.data.shortDescription = shortDescription;
	shortDescriptionFile.close();
	std::ifstream dateAndLocationFile(eventName + "\\datumiMjesto.txt");
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.startHour = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.startMinute = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.endHour = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.endMinute = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.startDay = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.startMonth = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.startYear = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.endDay = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.endMonth = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.endYear = stoi(fline);
	std::getline(dateAndLocationFile, fline, '-');
	requestedEvent.data.location= fline;
	std::getline(dateAndLocationFile, fline, '\n');
	requestedEvent.category = fline;
	requestedEvent.data.eventName = eventName;
	dateAndLocationFile.close();
	std::ifstream specialRequirementsFile(eventName + "\\posebniZahtjevi.txt");
	while (std::getline(specialRequirementsFile, fline, '-'))
		requestedEvent.data.specialRequirements.push_back(fline);
	specialRequirementsFile.close();
	std::ifstream commentsFile(eventName + "\\komentari.txt");
	while (std::getline(commentsFile, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fcommentID, '-');
		std::getline(iss, fuserName, '-');
		std::getline(iss, fcomment, '\n');
		requestedEvent.commentIDs.push_back(fcommentID);
		requestedEvent.comments.push_back(fcomment);
		requestedEvent.userNames.push_back(fuserName);
	}
	commentsFile.close();
	return requestedEvent;
	

}


std::string addEvent(struct newEvent &event2Add, std::string sessionID)
{
	std::ifstream userEvent("korisnikDogadjaj.txt");
	if (userEvent.is_open() == false)
		return fileOpeningError;
	std::string fline, fuserID, fevent;
	int flag = 0;
	while (std::getline(userEvent, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fevent, '-');
		std::getline(iss, fuserID, '\n');
		if(fevent==event2Add.eventList.eventName)
			flag++;
	}
	if (flag != 0)
		return alreadyExistingEvent;
	userEvent.close();
	CreateDirectoryA((LPCSTR)event2Add.eventList.eventName.c_str(), NULL);
	std::ofstream dateAndLocation(event2Add.eventList.eventName+"\\datumiMjesto.txt");
	if (dateAndLocation.is_open() == false)
		return fileOpeningError;
	dateAndLocation << event2Add.eventList.startHour << "-" << event2Add.eventList.startMinute << "-" << event2Add.eventList.endHour << "-" << event2Add.eventList.endMinute << "-"
	<< event2Add.eventList.startDay << "-" << event2Add.eventList.startMonth << "-" << event2Add.eventList.startYear << "-"
	<< event2Add.eventList.endDay << "-" << event2Add.eventList.endMonth << "-" << event2Add.eventList.endYear << "-"
	<< event2Add.eventList.location << "-" << event2Add.category << "\n";
	dateAndLocation.close();
	std::fstream userEvent2("korisnikDogadjaj.txt",std::ios::app);
	if (userEvent2.is_open() == false)
		return fileOpeningError;
	session* session = authenticate(sessionID);
	userEvent2 << event2Add.eventList.eventName << "-" << (*session).userID << '\n';
	userEvent2.close();
	std::ofstream shortDescription(event2Add.eventList.eventName + "\\kratakOpis.txt");
	if (shortDescription.is_open() == false)
		return fileOpeningError;
	shortDescription << event2Add.eventList.shortDescription;
	shortDescription.close();
	std::ofstream longDescription(event2Add.eventList.eventName + "\\opis.txt");
	if (longDescription.is_open() == false)
		return fileOpeningError;
	longDescription << event2Add.description;
	longDescription.close();
	std::ofstream specialRequests(event2Add.eventList.eventName+"\\posebniZahtjevi.txt");
	if (specialRequests.is_open() == false)
		return fileOpeningError;
	for (auto& i : event2Add.eventList.specialRequirements)
		specialRequests << i << "-";
	return successfulEvent;
	

}

char genRandomChar()  // Funkcija za generisanje random stringa.
{

	return alphanum[rand() % stringLength];
}

std::string genRandomString(std::string& sessionID)
{
	srand((unsigned)time(NULL));
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

std::string logOut(std::string sessionID)
{
	unsigned i;
	for (i = 0; i < sessions.size(); i++)
		if (sessions[i].sessionID == sessionID)
		{
			std::vector<struct session>::iterator it = sessions.begin() + i;
			sessions.erase(it);
			return successfulLogOut;
		}
	return unsuccessfulLogOut;
}

bool checkCommentID(std::string& commentID, std::string& eventName)
{
	std::ifstream commentsFile(eventName + "\\komentari.txt");
	std:: string fline, fcommentID, fuserName, fcomment;
	while (std::getline(commentsFile, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fcommentID, '-');
		std::getline(iss, fuserName, '-');
		std::getline(iss, fcomment, '\n');
		if (fcommentID == commentID)
			return false;
	}
	commentsFile.close();
	return true;
}
