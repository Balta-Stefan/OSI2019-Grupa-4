#ifndef BACKEND_H_INCLUDED
#define BACKEND_H_INCLUDED
#include <string>
#include <vector>

//global variable: vector<struct session> sessions

struct loginInfo
{
    std::string username, password;
};

struct session
{
    std::string sessionID, userName, userID;
    char userRank; //0 za obicne korisnike, 1 za administratore
};

struct quiz
{
    std::string questions, answers;
    std::vector<int> rightAnswers;
};

struct quiz4Players
{
    std::string questions, answers;
};

bool authenticate(std::string sessionID);
bool isAdmin(std::string sessionID);
std::string login(std::string username, std::string password);
std::string eventComment(std::string comment, std::string eventName);
std::string banUser(std::string userName);
struct quiz getQuizInfo();
void editQuiz(struct quiz newQuic);
std::vector<std::string> getCategories();
void changeCategories(std::vector<std::string> newCategories);
struct quiz4Players getQuestions4Player();
int checkPlayersAnswers(struct quiz);
std::string removeComment(std::string commentID, std::string eventName);



#endif // BACKEND_H_INCLUDED
