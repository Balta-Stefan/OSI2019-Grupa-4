#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdlib>

//global variable: vector<struct session> sessions

struct loginInfo
{
	std::string username, password;
};

struct session
{
	std::string sessionID, userName, userID;
	int userRank; //0 za obicne korisnike, 1 za administratore
};

struct quiz
{
	std::vector<std::string> questions, answers;
	std::vector<int> rightAnswers;
};

struct quiz4Players
{
	std::vector<std::string> questions, answers;
};

struct eventList
{	//sadrzi osnovne informacije za pregled dogadjaja na glavnoj stranici
	//slika, ostaviti za kasnije
	std::string eventName;
	std::string shortDescription;
	unsigned short startHour, startMinute, endHour, endMinute, startMonth, startDay, startYear, endMonth, endDay, endYear; //informacije o pocetku i zavrsetu dogadjaja 
	std::string location; //mjesto odrzavanja dogadjaja 
	std::vector<std::string>specialRequirements; //posebni zahtjevi dogadjaja 
};

struct eventFilter
{	//koristi se za filtriranje dogadjaja 
	unsigned short startHour, startMinute, endHour, endMinute, startMonth, startDay, startYear, endMonth, endDay, endYear; //informacije o pocetku i zavrsetu dogadjaja 
	std::string location; //mjesto odrzavanja dogadjaj 
	std::string category;
};

struct event
{	//daje informacije o konkretnom dogadjaju

	//dodati sliku kasnije
	struct eventList data; //sadrzi i samu strukturu eventList
	std::string description; //tekst koji opisuje dogadjaj
	std::string category;

	//i-ti indeksi vektora ispod su u vezi
	std::vector<std::string>userNames; //korisnicka imena 
	std::vector<std::string>comments; //sadrzi komentare od korisnika iz vektora userNames
	std::vector<std::string>commentIDs; //sadrzi identifikatore komentara
};

struct newEvent
{	//za dodavanje novog dogadjaja 
	struct eventList;
	std::string category;
	std::string description; //opis dogadjaja 
	bool notDuplicate; //omogucava korisniku da doda dogadjaj ako se posumnja da je dogadjaj mozda vec dodan
};


bool authenticate(std::string sessionID);
bool isAdmin(std::string sessionID);
std::string login(std::string username, std::string password);
std::string eventComment(std::string comment, std::string eventName, std::string sessionID);
std::string banUser(std::string userName);
struct quiz getQuizInfo();
void editQuiz(struct quiz &newQuiz);
std::vector<std::string> getCategories();
void changeCategories(std::vector<std::string> newCategories);
struct quiz4Players getQuestions4Player();
int checkPlayersAnswers(struct quiz &playerQandA);
std::string removeComment(std::string commentID, std::string eventName);
struct event getEvent(std::string eventName);
std::vector<eventList> getEvents(struct eventFilter &filter);
std::string addEvent(struct newEvent &event2Add);
char genRandomChar();
std::string genRandomString(std::string&);
bool checkSessionID(std::string&);
std::string logOut(std::string sessionID);



#endif // BACKEND_H_INCLUDED
