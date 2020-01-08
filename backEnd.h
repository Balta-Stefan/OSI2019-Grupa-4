#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <Windows.h>

//global variable: vector<struct session> sessions

struct loginConfirmation
{
	std::string sessionID, message;
	int userRank;
};


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


struct eventsFilter
{
	bool todayEvents, futureEvents, pastEvents;
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
	std::string sessionID;
};

struct newEvent
{	//za dodavanje novog dogadjaja 
	struct eventList eventList;
	std::string category;
	std::string description; //opis dogadjaja 
	bool notDuplicate; //omogucava korisniku da doda dogadjaj ako se posumnja da je dogadjaj mozda vec dodan
};


session* authenticate(std::string sessionID);
bool isAdmin(std::string sessionID);
loginConfirmation login(std::string username, std::string password);
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
std::string addEvent(struct newEvent &event2Add, std::string sessionID);
char genRandomChar();
std::string genRandomString(std::string&);
bool checkSessionID(std::string&);
std::string logOut(std::string sessionID);
bool checkCommentID(std::string&, std::string&);
std::vector<event> getFilteredEvents(struct eventsFilter &);



#endif // BACKEND_H_INCLUDED
