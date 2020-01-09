

#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include "commonStructures.h"
#include "backEnd.h"


unsigned short MAXQUESTIONS = 10;
unsigned short MAXANSWERS = 3;

static const char alphanum[] =
"0123456789"
"!@#$%^&*"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";
int stringLength = sizeof(alphanum) - 1;


std::vector<struct session> sessions;
std::string fileOpeningError = "Greska,zahtjev nije moguce ispuniti.",
successfulLogin = "Uspjesno prijavljivanje.", unsuccessfulLogin = "Neuspjesno prijavljivanje.", userBanned = "Ovaj nalog je banovan.",
successfulBan = "Korisnik uspjesno banovan.", unsuccessfulBan = "Korisnik sa takvim imenom ne postoji.", alreadyBanned = "Korisnik je vec banovan.",
commentAdded = "Komentar uspjesno dodan.",
unsuccessfulComment = "Neuspjesno brisanje komentara.", successfulComment = "Komentar uspjesno obrisan.", successfulLogOut = "Uspjesno odjavljivanje.",
unsuccessfulLogOut = "Neuspjesno odjavljivanje.",
successfulEvent = "Dogadjaj uspjesno dodan.", alreadyExistingEvent = "Dogadjaj sa ovim imenom vec postoji.";

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
		if (i.sessionID == sessionID && i.userRank == 1)
			return true;
	return false;
}

std::vector<std::string> getOwnEvents(std::string& sessionID)
{
	session* tmp = authenticate(sessionID);
	std::string userID = (*tmp).userID, fline, feventName, fuserID;
	std::vector<std::string> ownEvents;
	std::ifstream userEventFile("korisnikDogadjaj.txt");
	while (std::getline(userEventFile, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, feventName, '-');
		std::getline(iss, fuserID, '\n');
		if (fuserID == userID)
			ownEvents.push_back(feventName);
	}
	userEventFile.close();
	return ownEvents;
}

loginConfirmation login(std::string username, std::string password)
{
	loginConfirmation tempConfirmation;
	tempConfirmation.sessionID = "";
	tempConfirmation.message = "";
	tempConfirmation.userRank = 0;

	std::string fuserID, fusername, fpassword, fnumber, fline;
	std::ifstream users("korisnici.txt");
	if (users.is_open() == false)
	{
		tempConfirmation.message = fileOpeningError;
		return tempConfirmation;
	}

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
			if (fnum == 1)
			{
				tempConfirmation.message = userBanned;
				return tempConfirmation;
			}

			std::string sessionID;
			do
			{
				sessionID = "";
				genRandomString(sessionID);
			} while (checkSessionID(sessionID));
			struct session newSession;
			newSession.sessionID = sessionID;
			newSession.userID = fuserID;
			newSession.userName = fusername;
			newSession.userRank = fnum;
			sessions.push_back(newSession);
			tempConfirmation.sessionID = sessionID;
			tempConfirmation.message = successfulLogin;
			return tempConfirmation;
		}

	}
	users.close();
	std::ifstream admins("admini.txt");
	if (admins.is_open() == false)
	{
		tempConfirmation.message = fileOpeningError;
		return tempConfirmation;
	}

	while (std::getline(admins, fline))
	{
		std::stringstream iss(fline);
		std::getline(iss, fuserID, '-');
		std::getline(iss, fusername, '-');
		std::getline(iss, fpassword, '\n');
		if (fusername == username && fpassword == password)
		{
			std::string sessionID;
			do
			{
				sessionID = "";
				genRandomString(sessionID);
			} while (checkSessionID(sessionID));
			struct session newSession;
			newSession.sessionID = sessionID;
			newSession.userID = fuserID;
			newSession.userName = fusername;
			newSession.userRank = 1;
			sessions.push_back(newSession);

			tempConfirmation.userRank = 1;
			tempConfirmation.sessionID = sessionID;
			tempConfirmation.message = successfulLogin;
			return tempConfirmation;

		}
	}
	admins.close();
	tempConfirmation.message = unsuccessfulLogin;
	return tempConfirmation;
}

std::string eventComment(std::string comment, std::string eventName, std::string sessionID)
{
	std::string fileName = eventName + "\\komentari.txt", commentID = "";
	std::fstream comments(fileName, std::ios::app);
	if (!comments)
		return fileOpeningError;
	do
	{
		commentID = "";
		genRandomString(commentID);
	} while (!checkCommentID(commentID, eventName));
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
	int flag = 0;
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
			if (fnum == 1)
				return alreadyBanned;
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

void editQuiz(struct quiz& newQuiz)
{
	int i;
	int tempVar = newQuiz.questions.size();
	int tempVar2 = newQuiz.answers.size();

	std::ofstream quizQuestions("kvizPitanja.txt");
	for (i = 0; i < tempVar; i++)
		quizQuestions << newQuiz.questions[i] << "-";
	for (i = 0; i < tempVar2; i++)
		if (i == tempVar2 -1)
			quizQuestions << newQuiz.answers[i] << "\n";
		else
			quizQuestions << newQuiz.answers[i] << "-";
	quizQuestions.close();
	std::ofstream quizAnswers("kvizOdgovori.txt");
	for (i = 0; i < tempVar; i++)
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
	unsigned int i, j, flag = 0;
	for (i = 0; i < newCategories.size(); i++)
	{	
		for (j = 0; j < newCategories.size(); j++)
			if (newCategories[i] == newCategories[j])
			{
				flag++;
				if (flag != 1)
					newCategories.erase(newCategories.begin() + j);
			}
		flag = 0;

	}
	std::ofstream categories("kategorije.txt");
	if (newCategories.size() > 0)
	{
		for (i = 0; i < newCategories.size() - 1; i++)
			categories << newCategories[i] << "-";
		categories << newCategories[i];
	}

}

struct quiz4Players getQuestions4Player()
{
	struct quiz quizInfo;
	struct quiz4Players quizForPlayers;
	quizInfo = getQuizInfo();
	unsigned int i;
	for (i = 0; i < quizForPlayers.questions.size(); i++)
		quizForPlayers.questions.push_back(quizInfo.questions[i]);
	for (i = 0; i < quizForPlayers.answers.size(); i++)
		quizForPlayers.answers.push_back(quizInfo.answers[i]);
	return quizForPlayers;
}

int checkPlayersAnswers(struct quiz& playerQandA)
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
	std::string fileName = eventName + "\\komentari.txt", fline, fcommentID, fuserName, fcomment, fileText = "";
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
	commentsInput << fileText;
	commentsInput.close();
	return successfulComment;
}

struct event getEvent(std::string eventName)
{	
	std::ifstream userEvent("korisnikDogadjaj.txt");
	std::string fline, fuserID, fevent;
	int flag = 0;
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
	std::string longDescription = "", shortDescription = "", fcommentID, fuserName, fcomment;//ali kako signalizirati ako nisu dobro otvoreni, jer ne vraca funkcija poruku
	while (std::getline(longDescriptionFile, fline, '\n'))
		longDescription += fline + '\n';
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
	requestedEvent.data.location = fline;
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


std::string addEvent(struct newEvent& event2Add, std::string sessionID)
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
		if (fevent == event2Add.eventData.eventName)
			flag++;
	}
	if (flag != 0)
		return alreadyExistingEvent;
	userEvent.close();
	CreateDirectoryA((LPCSTR)event2Add.eventData.eventName.c_str(), NULL);
	std::ofstream dateAndLocation(event2Add.eventData.eventName + "\\datumiMjesto.txt");
	if (dateAndLocation.is_open() == false)
		return fileOpeningError;
	dateAndLocation << event2Add.eventData.startHour << "-" << event2Add.eventData.startMinute << "-" << event2Add.eventData.endHour << "-" << event2Add.eventData.endMinute << "-"
		<< event2Add.eventData.startDay << "-" << event2Add.eventData.startMonth << "-" << event2Add.eventData.startYear << "-"
		<< event2Add.eventData.endDay << "-" << event2Add.eventData.endMonth << "-" << event2Add.eventData.endYear << "-"
		<< event2Add.eventData.location << "-" << event2Add.category << "\n";
	dateAndLocation.close();
	std::fstream userEvent2("korisnikDogadjaj.txt", std::ios::app);
	if (userEvent2.is_open() == false)
		return fileOpeningError;
	session* session = authenticate(sessionID);
	userEvent2 << event2Add.eventData.eventName << "-" << (*session).userID << '\n';
	userEvent2.close();
	std::ofstream shortDescription(event2Add.eventData.eventName + "\\kratakOpis.txt");
	if (shortDescription.is_open() == false)
		return fileOpeningError;
	shortDescription << event2Add.eventData.shortDescription;
	shortDescription.close();
	std::ofstream longDescription(event2Add.eventData.eventName + "\\opis.txt");
	if (longDescription.is_open() == false)
		return fileOpeningError;
	longDescription << event2Add.description;
	longDescription.close();
	std::ofstream specialRequests(event2Add.eventData.eventName + "\\posebniZahtjevi.txt");
	if (specialRequests.is_open() == false)
		return fileOpeningError;
	for (auto& i : event2Add.eventData.specialRequirements)
		specialRequests << i << "-";
	return successfulEvent;
	std::ofstream comments(event2Add.eventData.eventName + "\\komentari.txt");

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
	for (unsigned int i = 0; i < sessions.size(); ++i)
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
	std::string fline, fcommentID, fuserName, fcomment;
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
std::vector<event> getFilteredEvents(struct eventsFilter& filter)
{

	//prihvaca strukturu eventFilter koja daje informacije o tome sta korisnik zeli da trazi, vraca se vektor sa strukturama od kojih svaka daje informacije o nekom dogadjaju
	std::ifstream userEventFile("korisnikDogadjaj.txt");
	std::string fline, feventName, fuserID;
	std::vector<event> events;
	event tmp;
	struct tm currentDate;
	time_t now = time(0);
	localtime_s(&currentDate, &now);
	if (!filter.pastEvents && !filter.futureEvents && filter.category == "" && !filter.todayEvents)
		return events;
	else if (!filter.pastEvents && !filter.futureEvents && filter.category == "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if (tmp.data.startDay == currentDate.tm_mday && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startYear == currentDate.tm_year + 1900)
				events.push_back(tmp);
		}
	}
	else if (!filter.futureEvents && !filter.pastEvents && !filter.todayEvents)
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if (tmp.category == filter.category)
				events.push_back(tmp);
		}
	}
	else if (!filter.pastEvents && !filter.todayEvents && filter.category == "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if (tmp.data.startYear > currentDate.tm_year + 1900 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth > currentDate.tm_mon + 1 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startDay > currentDate.tm_mday)
				events.push_back(tmp);
		}
	}
	else if (!filter.futureEvents && !filter.todayEvents && filter.category == "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if (tmp.data.startYear < currentDate.tm_year + 1900 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth < currentDate.tm_mon + 1 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startDay < currentDate.tm_mday)
				events.push_back(tmp);
		}
	}
	else if (filter.pastEvents && filter.futureEvents && filter.todayEvents && filter.category != "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if (tmp.category == filter.category)
				events.push_back(tmp);
		}
	}
	else if (filter.pastEvents && filter.futureEvents && filter.category != "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if ((tmp.data.startDay != currentDate.tm_mday && tmp.data.startMonth != currentDate.tm_mon + 1 && tmp.data.startYear != currentDate.tm_year + 1900) &&
				tmp.category == filter.category)
				events.push_back(tmp);
		}
	}
	else if (filter.pastEvents && filter.futureEvents && filter.todayEvents)
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			events.push_back(tmp);
		}
	}
	else if (filter.todayEvents && filter.pastEvents && filter.category != "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if ((tmp.data.startYear < currentDate.tm_year + 1900 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth < currentDate.tm_mon + 1 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startDay <= currentDate.tm_mday) &&
				tmp.category == filter.category)
				events.push_back(tmp);
		}
	}
	else if (filter.todayEvents && filter.futureEvents && filter.category != "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if ((tmp.data.startYear > currentDate.tm_year + 1900 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth > currentDate.tm_mon + 1 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startDay >= currentDate.tm_mday) &&
				tmp.category == filter.category)
				events.push_back(tmp);
		}
	}
	else if (filter.pastEvents && filter.category != "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if ((tmp.data.startYear < currentDate.tm_year + 1900 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth < currentDate.tm_mon + 1 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startDay < currentDate.tm_mday) &&
				tmp.category == filter.category)
				events.push_back(tmp);
		}
	}
	else if (filter.futureEvents && filter.category != "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if ((tmp.data.startYear > currentDate.tm_year + 1900 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth > currentDate.tm_mon + 1 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startDay > currentDate.tm_mday) &&
				tmp.category == filter.category)
				events.push_back(tmp);
		}
	}
	if (filter.todayEvents && filter.category != "")
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if ((tmp.data.startDay == currentDate.tm_mday && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startYear == currentDate.tm_year + 1900) &&
				tmp.category == filter.category)
				events.push_back(tmp);
		}
	}
	else if (filter.todayEvents && filter.pastEvents)
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if (tmp.data.startYear < currentDate.tm_year + 1900 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth < currentDate.tm_mon + 1 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startDay <= currentDate.tm_mday)
				events.push_back(tmp);
		}
	}
	else if (filter.todayEvents && filter.futureEvents)
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if (tmp.data.startYear > currentDate.tm_year + 1900 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth > currentDate.tm_mon + 1 ||
				tmp.data.startYear == currentDate.tm_year + 1900 && tmp.data.startMonth == currentDate.tm_mon + 1 && tmp.data.startDay >= currentDate.tm_mday)
				events.push_back(tmp);
		}
	}
	else if (filter.pastEvents && filter.futureEvents)
	{
		while (std::getline(userEventFile, fline))
		{
			std::stringstream iss(fline);
			std::getline(iss, feventName, '-');
			std::getline(iss, fuserID, '\n');
			tmp = getEvent(feventName);
			if (tmp.data.startDay != currentDate.tm_mday || tmp.data.startMonth != currentDate.tm_mon + 1 || tmp.data.startYear != currentDate.tm_year + 1900)
				events.push_back(tmp);
		}
	}
	userEventFile.close();
	return events;
}








void send()
{
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream logInFile("loginRequest.bin", std::ios::binary);
		std::ifstream addEventRequest("addEventRequest.bin", std::ios::binary);
		std::ifstream giveQuiz("quizRequest.txt");
		std::ifstream quizPlayerAnswers("quizAnswers.bin", std::ios::binary);
		std::ifstream ownEvents("ownEventsRequest.txt");
		std::ifstream logOutRequest("logOutRequest.txt");
		std::ifstream banRequest("banRequest.bin", std::ios::binary);
		std::ifstream changeCategoriesRequest("changeCategoriesRequest.txt");
		std::ifstream addCategories("addNewCategories.bin", std::ios::binary);
		std::ifstream checkEvent("getEvent.txt");
		std::ifstream addCommentRequest("addCommentRequest.bin", std::ios::binary);
		std::ifstream showEventsRequest("showEvents.txt");
		std::ifstream removeCommentRequest("removeCommentRequest.bin", std::ios::binary);
		std::ifstream editQuizRequest("editQuizRequest.txt");
		std::ifstream newQuiz("newQuiz.bin", std::ios::binary);
		std::ifstream pushEvents("checkEvents.bin", std::ios::binary);

		if (logInFile.is_open())
		{	
			loginInfo tempInfo;
			cereal::BinaryInputArchive iarchive(logInFile);
			iarchive(tempInfo);
			logInFile.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("loginRequest.bin");
			
			loginConfirmation tempConfirmation = login(tempInfo.username, tempInfo.password);
			
			std::cout << "session id is: " << tempConfirmation.sessionID << std::endl;
			std::ofstream response("loginRequestAnswer.bin", std::ios::binary);
			cereal::BinaryOutputArchive oarchive(response);
			oarchive(tempConfirmation);
			response.close();
		}
		else if (addEventRequest.is_open())
		{
			newEvent tempEvent;
			cereal::BinaryInputArchive iarchive(addEventRequest);
			iarchive(tempEvent);
			addEventRequest.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("addEventRequest.bin");

			std::string response;
			response = addEvent(tempEvent, tempEvent.sessionID);
			std::ofstream responseFile("addEventAnswer2.txt");
			responseFile << response;
			responseFile.close();
			rename("addEventAnswer2.txt", "addEventAnswer.txt");
		}
		else if (giveQuiz.is_open())
		{
			giveQuiz.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("quizRequest.txt");
			std::ofstream quizAnswer("quiz2.bin", std::ios::binary);

			quiz tempQuiz = getQuizInfo();
			cereal::BinaryOutputArchive oarchive(quizAnswer);
			oarchive(tempQuiz);

			quizAnswer.close();
			rename("quiz2.bin", "quiz.bin");
		}
		else if (quizPlayerAnswers.is_open())
		{	
			quiz playerAnswers;
			cereal::BinaryInputArchive iarchive(quizPlayerAnswers);
			iarchive(playerAnswers); //problem u ovoj liniji, otkriti sta
			quizPlayerAnswers.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("quizAnswers.bin");

			int correct = checkPlayersAnswers(playerAnswers);
			std::ofstream answer("correctAnswers.txt");
			answer << correct;
			answer.close();
		}
		else if (ownEvents.is_open())
		{
			std::string userSessionID;
			ownEvents >> userSessionID;
			ownEvents.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("ownEventsRequest.txt");

			std::vector<std::string> usersEventsList;
			std::ofstream response("ownEventsAnswer2.bin", std::ios::binary);

			usersEventsList = getOwnEvents(userSessionID);
			cereal::BinaryOutputArchive oarchive(response);
			oarchive(usersEventsList);

			response.close();
			rename("ownEventsAnswer2.bin", "ownEventsAnswer.bin");
		}
		else if (logOutRequest.is_open())
		{
			std::string userSessionID;
			logOutRequest >> userSessionID;
			logOutRequest.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("logOutRequest.txt");

			logOut(userSessionID);
		}
		else if (banRequest.is_open())
		{
			banUserStruct tempStruct;
			cereal::BinaryInputArchive iarchive(banRequest);
			iarchive(tempStruct);

			banRequest.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("banRequest.bin");
			std::string response;
			
			if (isAdmin(tempStruct.sessionID))
			{
				response = banUser(tempStruct.user2Ban);
			}
			else
				response = "niste administrator";

			std::ofstream responseFile("banRequestAnswer2.txt");
			responseFile << response;
			
			responseFile.close();
			rename("banRequestAnswer2.txt", "banRequestAnswer.txt");
			std::cout << "renamed" << std::endl;
		}
		else if (changeCategoriesRequest.is_open())
		{	
			changeCategoriesRequest.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("changeCategoriesRequest.txt");

			std::vector<std::string> categories = getCategories();

			std::ofstream response("changeCategoriesResponse.bin", std::ios::binary);
			cereal::BinaryOutputArchive oarchive(response);
			oarchive(categories);
			response.close();
		}
		else if (addCategories.is_open())
		{	
			std::vector<std::string> categories;
			std::string userSessionID, temp;
			cereal::BinaryInputArchive iarchive(addCategories);

			iarchive(categories);
			addCategories.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("addNewCategories.bin");
			

			userSessionID = categories[0];
			
			temp = categories.back();
			categories.back() = categories[0];
			categories[0] = temp;
			categories.pop_back();

			if (isAdmin(userSessionID))
				changeCategories(categories);
			
				
			
		}
		
		else if (checkEvent.is_open())
		{
			std::string eventName;
			std::getline(checkEvent, eventName);
			//checkEvent >> eventName;
			checkEvent.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("getEvent.txt");

			std::ofstream giveEvent("getEventResponse2.bin", std::ios::binary);
			//struct event getEvent(std::string eventName);
			event TempEvent = getEvent(eventName);

			cereal::BinaryOutputArchive oarchive(giveEvent);
			oarchive(TempEvent);


			giveEvent.close();
			rename("getEventResponse2.bin", "getEventResponse.bin");
		}
		else if (addCommentRequest.is_open())
		{	
			addComment tempStruct;
			cereal::BinaryInputArchive iarchive(addCommentRequest);
			iarchive(tempStruct);
			addCommentRequest.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("addCommentRequest.bin");
			

			std::string  response = eventComment(tempStruct.comment, tempStruct.eventName, tempStruct.sessionID);
			std::ofstream giveResponse("addCommentResponse2.txt");
			giveResponse << response;
			giveResponse.close();
			rename("addCommentResponse2.txt", "addCommentResponse.txt");
		}
		else if (showEventsRequest.is_open())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			showEventsRequest.close();
			remove("showEvents.txt");
		}
		else if (removeCommentRequest.is_open())
		{
			removeCommentStruct tempStruct;
			cereal::BinaryInputArchive iarchive(removeCommentRequest);
			iarchive(tempStruct);
			removeCommentRequest.close();
			remove("removeCommentRequest.bin");

			std::ofstream response("response2.txt");
			
			if (isAdmin(tempStruct.sessionID))
			{
				std::string answer = removeComment(tempStruct.commentID, tempStruct.eventName);
				response << answer;
			}
			else
			{
				response << "Niste administrator";
			}
			response.close();
			rename("response2.txt", "removeCommentAnswer.txt");
		}
		else if (editQuizRequest.is_open())
		{
			std::string temp;
			editQuizRequest >> temp;
			editQuizRequest.close();
			remove("editQuizRequest.txt");

			quizEdit tempStruct;
			if (isAdmin(temp))
			{
				tempStruct.data = getQuizInfo();
			}
			else
			{
				tempStruct.message = "niste administrator";
			}
			std::ofstream response("editQuizAnswer2.bin", std::ios::binary);
			cereal::BinaryOutputArchive oarchive(response);
			oarchive(tempStruct);

			response.close();
			rename("editQuizAnswer2.bin", "editQuizAnswer.bin");
		}
		else if (newQuiz.is_open())
		{
			quiz tempStruct;
			cereal::BinaryInputArchive iarchive(newQuiz);
			iarchive(tempStruct);
			
			newQuiz.close();
			remove("newQuiz.bin");

			editQuiz(tempStruct);
		}
		else if (pushEvents.is_open())
		{
			eventsFilter tempFilter;
			cereal::BinaryInputArchive iarchive(pushEvents);
			iarchive(tempFilter);

			pushEvents.close();
			remove("checkEvents.bin");

			std::ofstream response("eventsList2.bin", std::ios::binary);
			std::vector<event> events = getFilteredEvents(tempFilter);

			cereal::BinaryOutputArchive oarchive(response);
			oarchive(events);

			response.close();
			rename("eventsList2.bin", "eventsList.bin");
		}
		
	}
}

//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
int main()
{
	send();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

