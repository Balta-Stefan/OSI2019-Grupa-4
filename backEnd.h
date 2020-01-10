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




struct session
{
	std::string sessionID, userName, userID;
	int userRank; //0 za obicne korisnike, 1 za administratore
	
};



struct quiz4Players
{
	std::vector<std::string> questions, answers;
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
//std::vector<std::string> getOwnEvents(std::string& sessionID);
std::string removeEvent(deleteEvent &);
int DeleteDirectory(const std::string &refcstrRootDirectory, bool bDeleteSubdirectories = true);



#endif // BACKEND_H_INCLUDED
