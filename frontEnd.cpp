// FrontEnd.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <thread>
#include <chrono>
#include <cstdio>
#include <conio.h>
#include <cereal/archives/binary.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <iomanip>
#include <algorithm>
#include <sstream>

//VAZNA NAPOMENA, ZABRANITI DODAVANJE DOGADJAJA SA ISTIM IMENOM


unsigned short MAXQUESTIONS = 10; //max broj pitanja za kviz
unsigned short MAXANSWERS = 3; //max broj odgovora
std::string sessionID = "";
int userRank = 0;
bool loggedIn = false;

std::string successfulLogOut = "Uspjesno odjavljivanje.";



struct deleteEvent
{
	std::string sessionID, eventName;
	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(sessionID, eventName);
	}
};


struct eventsFilter
{
	bool todayEvents, futureEvents, pastEvents;
	std::string category;

	eventsFilter() { todayEvents = futureEvents = pastEvents = false; category = ""; }
	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(todayEvents, futureEvents, pastEvents, category);
	}
};

struct removeCommentStruct
{
	std::string sessionID, commentID, eventName;

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(sessionID, commentID, eventName);
	}
};

struct addComment
{
	std::string comment, eventName, sessionID;
	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(comment, eventName, sessionID);
	}
};

struct eventList
{
	std::string eventName;
	std::string shortDescription;
	unsigned short startHour, startMinute, endHour, endMinute, startMonth, startDay, startYear, endMonth, endDay, endYear;
	std::string location;
	std::vector<std::string>specialRequirements;

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(eventName, shortDescription, startHour, startMinute, endHour, endMinute, startMonth, startDay, startYear, endMonth, endDay, endYear, location, specialRequirements);
	}
};

struct event
{
	eventList data;
	std::string description;
	std::string category;


	std::vector<std::string>userNames;
	std::vector<std::string>comments;
	std::vector<std::string>commentIDs;


	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(data, description, category, userNames, comments, commentIDs);
	}

};



struct newEvent
{	//za dodavanje novog dogadjaja 
	eventList eventData;
	std::string category;
	std::string description; //opis dogadjaja 
	std::string sessionID;
	bool notDuplicate{}; //omogucava korisniku da doda dogadjaj ako se posumnja da je dogadjaj mozda vec dodan

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(eventData, category, description, notDuplicate, sessionID);
	}
};


struct loginInfo
{
	std::string username, password;
	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(username, password);
	}
};


struct banUserStruct
{
	std::string user2Ban, sessionID;

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(sessionID, user2Ban);
	}
};

struct loginConfirmation
{
	std::string sessionID, message;
	int userRank;

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(sessionID, message, userRank);
	}
};






struct quiz
{
	std::vector<std::string> questions, answers;
	std::vector<int> rightAnswers;

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(questions, answers, rightAnswers);
	}
};

struct quizEdit
{
	quiz data;
	std::string message;
	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(data, message);
	}
};


int countDigit(size_t n)
{
	int count = 0;
	while (n != 0) {
		n = n / 10;
		++count;
	}
	return count;
}


void greska()
{
	std::cout << "greska" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	system("cls");
}


void removeEvent(std::string eventForRemoval)
{
	//prikazati sve dogadjaje ako je korisnik administrator
	//ako korisnik nije administrator, onda prikazati samo vlastite dogadjaje

	if (loggedIn)
	{
		deleteEvent tempStruct;
		std::ofstream requestFile("deleteEvent2.bin", std::ios::binary);

		if (eventForRemoval == "")
		{
			std::string toDelete;
			std::cout << "Unesite ime dogadjaja koji zelite obrisati: " << std::endl;
			std::cin.ignore();
			std::getline(std::cin, toDelete); //mozda potreban cin.ignore()?
			tempStruct.eventName = toDelete;

		}
		else
			tempStruct.eventName = eventForRemoval;

		tempStruct.sessionID = sessionID;

		cereal::BinaryOutputArchive oarchive(requestFile);
		oarchive(tempStruct);
		requestFile.close();

		rename("deleteEvent2.bin", "deleteEvent.bin");

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			std::ifstream response("deleteEventResponse.txt");
			if (response.is_open())
			{
				std::string tempString;
				std::getline(response, tempString);

				std::cout << tempString << std::endl;
				break;
			}
		}

	}
	else
		greska();

	remove("deleteEventResponse.txt");
	std::string tempString;
	std::cout << "Unesite bilo sta za povratak:" << std::endl;
	std::cin >> tempString;
}


void izmjenaKviza()
{
	std::ofstream request("editQuizRequest2.txt");
	request << sessionID;
	request.close();
	rename("editQuizRequest2.txt", "editQuizRequest.txt");

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		std::ifstream quizData("editQuizAnswer.bin", std::ios::binary);
		if (quizData.is_open())
		{
			quizEdit tempStruct;
			cereal::BinaryInputArchive iarchive(quizData);
			iarchive(tempStruct);
			quizData.close();
			remove("editQuizAnswer.bin");



			if (tempStruct.message.length() > 0) //greska
			{
				std::cout << tempStruct.message << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				return;
			}
			else
			{
				while (true)
				{
					system("cls");

					for (unsigned int i = 0; i < tempStruct.data.questions.size(); i++)
					{
						std::cout << i + 1 << ")" << tempStruct.data.questions[i] << std::endl;
						char tempC = 'a';
						for (int j = 0; j < MAXANSWERS; j++)
							std::cout << "	" << tempC++ << ")" << tempStruct.data.answers[MAXANSWERS * i + j] << std::endl;

						std::cout << "Ispravni odgovor: " << tempStruct.data.answers[tempStruct.data.rightAnswers[i]] << std::endl; //ovo se kvari kada se uklanjaju elementi
						std::cout << std::endl;
					}
					std::cout << "1)Dodaj pitanje" << std::endl;
					std::cout << "2)Izbrisi pitanje" << std::endl;
					std::cout << "3)Izlaz" << std::endl;
					int choice;
					std::cin >> choice;
					std::cout << std::endl;
					if ((choice == 1) && (tempStruct.data.questions.size() < MAXQUESTIONS))
					{
						std::string question;
						short correctAnswer;
						int tempSize = tempStruct.data.answers.size() - 1;

						std::cout << "Unesite pitanje:" << std::endl; //mozda potreban cin.ignore()
						std::cin.ignore();
						std::getline(std::cin, question);
						tempStruct.data.questions.push_back(question);

						for (int i = 0; i < MAXANSWERS; i++)
						{
							std::string temp;
							std::cout << "Unesite odgovor " << i + 1 << ":" << std::endl; //mozda potreban cin.ignore()
							std::getline(std::cin, temp);
							tempStruct.data.answers.push_back(temp);
						}

						std::cout << "Odgovori na pitanja su numerisana od 1 do " << MAXANSWERS << std::endl;
						do
						{
							std::cout << "Odaberite ispravan odgovor za pitanje " << ":" << std::endl;
							std::cin >> correctAnswer;
						} while ((correctAnswer < 1) || (correctAnswer > MAXANSWERS));
						tempStruct.data.rightAnswers.push_back(tempSize + correctAnswer);



					}
					else if ((choice == 2) && (tempStruct.data.questions.size() > 0)) //brisanje pitanja, mozda neispravno
					{
						std::cout << "Koje pitanje obrisati? (pitanja su numerisana od 1)" << std::endl;
						unsigned short toDelete;
						std::cin >> toDelete;


						if ((toDelete > 0) && (toDelete <= tempStruct.data.questions.size()))
						{
							toDelete--;

							tempStruct.data.questions.erase(tempStruct.data.questions.begin() + toDelete);

							for (int i = 0; i < MAXANSWERS; i++)
								tempStruct.data.answers.erase(tempStruct.data.answers.begin() + toDelete * MAXANSWERS);
							for (unsigned int k = toDelete; k < tempStruct.data.rightAnswers.size(); k++)
								tempStruct.data.rightAnswers[k] -= MAXANSWERS;
							tempStruct.data.rightAnswers.erase(tempStruct.data.rightAnswers.begin() + toDelete); //ova linija je problem
						}
						else
							greska();
					}
					else if (choice == 3)
					{
						std::ofstream changeQuiz("newQuiz2.bin", std::ios::binary);
						quiz tempQuiz = tempStruct.data;
						cereal::BinaryOutputArchive oarchive(changeQuiz);
						oarchive(tempQuiz);

						changeQuiz.close();
						rename("newQuiz2.bin", "newQuiz.bin");
						return;
					}
					else
						greska();
				}
			}
		}
	}
}


void addEventTime(eventList& tempEvent)
{
	std::cout << "Pocetak (sat)" << " ";
	std::cin >> tempEvent.startHour;
	std::cout << std::endl;

	std::cout << "Pocetak (minuta)" << " ";
	std::cin >> tempEvent.startMinute;
	std::cout << std::endl;

	std::cout << "Pocetak (mjesec)" << " ";
	std::cin >> tempEvent.startMonth;
	std::cout << std::endl;

	std::cout << "Pocetak (dan)" << " ";
	std::cin >> tempEvent.startDay;
	std::cout << std::endl;

	std::cout << "Pocetak (godina)" << " ";
	std::cin >> tempEvent.startYear;
	std::cout << std::endl;

	std::cout << "Kraj (sat)" << " ";
	std::cin >> tempEvent.endHour;
	std::cout << std::endl;

	std::cout << "Kraj (minuta)" << " ";
	std::cin >> tempEvent.endMinute;
	std::cout << std::endl;

	std::cout << "Kraj (mjesec)" << " ";
	std::cin >> tempEvent.endMonth;
	std::cout << std::endl;

	std::cout << "Kraj (dan)" << " ";
	std::cin >> tempEvent.endDay;
	std::cout << std::endl;

	std::cout << "Kraj (godina)" << " ";
	std::cin >> tempEvent.endYear;
	std::cout << std::endl;
}


void dodavanjeDogadjaja()
{
	//backend gives answer via a text file called addEventAnswer.txt
	system("cls");

	newEvent tempEvent;
	tempEvent.sessionID = sessionID;
	tempEvent.notDuplicate = false;

	std::cout << "Ime dogadjaja: " << std::endl;
	std::cin.ignore();
	std::getline(std::cin, tempEvent.eventData.eventName);
	
	//std::cin >> tempEvent.eventData.eventName;
	std::cout << "Kratak opis:" << std::endl;
	//std::cin.ignore();
	std::getline(std::cin, tempEvent.eventData.shortDescription);
	

	addEventTime(tempEvent.eventData);


	std::cout << "Lokacija: ";
	std::cin.ignore();
	std::getline(std::cin, tempEvent.eventData.location);
	//std::cin >> tempEvent.eventData.location;
	std::cout << std::endl;

	int brojZahtjeva;

	std::cout << "Broj posebnih zahtjeva: ";
	std::cin >> brojZahtjeva;
	std::cout << std::endl;
	std::cin.ignore();

	for (int i = 0; i < brojZahtjeva; i++)
	{
		std::cout << "Unesite poseban zahtjev: " << std::endl;
		std::string temp;

		std::getline(std::cin, temp);
		tempEvent.eventData.specialRequirements.push_back(temp);
	}

	std::cout << "Kategorija: " << std::endl;
	//std::cin.ignore();
	std::getline(std::cin, tempEvent.category);
	//std::cin >> tempEvent.category;
	std::cout << std::endl;


	std::cout << "Unesite opis dogadjaja: " << std::endl;
	//std::cin.ignore();
	std::getline(std::cin, tempEvent.description);


	//send 2 server
	{
		std::fstream file("addEventRequest.bin", std::ios::binary | std::ios::out);

		cereal::BinaryOutputArchive oarchive(file);
		oarchive(tempEvent);
	}


	//answer from server
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream input;
		input.open("addEventAnswer.txt");
		if (input.is_open())
		{
			std::string message;
			std::getline(input, message);
			//input >> message;

			input.close();
			std::cout << std::endl;
			std::cout << message;
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			remove("addEventAnswer.txt");
			return;

		}

	}
}

void logIn()
{
	std::string userName, password;
	char tempChar;
	const char ENTER = 13;

	std::cout << "Unesite korisnicko ime: " << std::endl;
	std::cin.ignore();
	std::getline(std::cin, userName);
	std::cout << std::endl;
	std::cout << "Unesite lozinku: " << std::endl;



	while ((tempChar = _getch()) != ENTER)
	{
		if ((tempChar == 8 || tempChar == 127) && !password.empty()) //backspace
		{
			password.pop_back();
			std::cout << "\b \b";
		}
		else
		{
			password += tempChar;
			std::cout << '*';
		}

	}


	loginInfo login;
	login.password = password;
	login.username = userName;

	{
		std::fstream fajl;
		fajl.open("loginRequest.bin", std::ios::out | std::ios::binary);

		cereal::BinaryOutputArchive oarchive(fajl);
		oarchive(login);

		fajl.close();
	}

	loginConfirmation tempLoginInfo;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream fajl("loginRequestAnswer.bin", std::ios::in | std::ios::binary);
		if (fajl.is_open())
		{
			cereal::BinaryInputArchive iarchive(fajl);
			iarchive(tempLoginInfo);

			fajl.close();
			break;
		}

		fajl.close();

	}

	std::cout << std::endl;
	std::cout << tempLoginInfo.message << std::endl;

	if (tempLoginInfo.sessionID.length() > 0)
	{
		loggedIn = true;
		userRank = tempLoginInfo.userRank;
		sessionID = tempLoginInfo.sessionID;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	remove("loginRequestAnswer.bin");
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	system("cls");

}

void igrajKviz()
{
	//backend gives answer via quiz.txt, format is: num of lines \n all questions then all answers (each entry is in new line)
	system("cls");
	std::ofstream request("quizRequest2.txt");
	request.close();
	rename("quizRequest2.txt", "quizRequest.txt");

	quiz kviz;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream answer;
		answer.open("quiz.bin", std::ios::binary);
		if (answer.is_open())
		{
			cereal::BinaryInputArchive iarchive(answer);
			iarchive(kviz);

			answer.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("quiz.bin");
			break;
		}
		answer.close();
	}

	quiz tempKviz;
	tempKviz.questions = kviz.questions;

	for (int i = 0; i < MAXQUESTIONS; i++)
	{	
		size_t questionLength = kviz.questions[i].length();
		std::cout << "Pitanje " << i + 1 << ": " << std::endl;
		std::cout << std::string(questionLength, '-') << std::endl;
		
		std::cout << kviz.questions[i] << std::endl;
		std::cout << std::string(questionLength, '-') << std::endl;
		std::cout << "1)" << kviz.answers[i * 3] << std::endl;
		std::cout << "2)" << kviz.answers[i * 3 + 1] << std::endl;
		std::cout << "3)" << kviz.answers[i * 3 + 2] << std::endl;

		short answer;
		std::cout << std::endl;
		std::cout << "Unesite odgovor: ";
		std::cin >> answer;
		tempKviz.answers.push_back(kviz.answers[(answer - 1) + i * 3]);
		system("cls");
	}




	//check validity of answers
	{
		std::ofstream giveAnswers("quizAnswers.bin", std::ios::binary);

		cereal::BinaryOutputArchive oarchive(giveAnswers);
		oarchive(tempKviz);
		giveAnswers.close();
	}


	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream validAnswers("correctAnswers.txt");
		if (validAnswers.is_open())
		{
			int valid;
			validAnswers >> valid;
			std::cout << "Broj ispravnih odgovora: " << valid << std::endl;
			validAnswers.close();
			remove("correctAnswers.txt");
			break;
		}
	}


	std::string temp;
	std::cout << "Unesite bilo koju vrijednost za izlaz: ";
	std::cin >> temp;
	



}

void concreteEventTransaction(event& eventVar, std::string eventName)
{
	std::ofstream file("getEvent.txt");
	file << eventName;
	file.close();

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream response("getEventResponse.bin", std::ios::binary);

		if (response.is_open())
		{
			cereal::BinaryInputArchive iarchive(response);
			iarchive(eventVar);
			response.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("getEventResponse.bin");
			break;
		}
	}
}

void printEventData(event &tempEvent)
{
	system("cls");
	//pocetak ispisa podataka o dogadjaju
	std::cout << "Ime dogadjaja: " << tempEvent.data.eventName << std::endl;
	std::cout << "Trajanje dogadjaja: " <<
		tempEvent.data.startDay << "." << tempEvent.data.startMonth << "." << tempEvent.data.startYear << " " <<
		tempEvent.data.startHour << ":" << tempEvent.data.startMinute << "-" <<
		tempEvent.data.endDay << "." << tempEvent.data.endMonth << "." << tempEvent.data.endYear << " " <<
		tempEvent.data.endHour << ":" << tempEvent.data.endMinute << std::endl;
	std::cout << "Lokacija: " << tempEvent.data.location << std::endl;
	if (tempEvent.data.specialRequirements.size() > 0)
	{
		std::cout << "Posebni zahtjevi: " << std::endl;
		for (auto& i : tempEvent.data.specialRequirements)
			std::cout << i << std::endl;
	}

	std::cout << "Kategorija: " << tempEvent.category << std::endl;
	std::cout << "Opis: " << std::endl;
	std::cout << tempEvent.description << std::endl;

	std::cout << "Komentari:" << std::endl;
	for (unsigned int i = 0; i < tempEvent.userNames.size(); i++)
	{
		std::cout << i + 1 << ")";
		std::cout << tempEvent.userNames[i] << std::endl;
		std::cout << tempEvent.comments[i] << std::endl;
	}

	//kraj ispisa podataka o dogadjaju
}

void getEvent(std::string eventName)
{

	while (true)
	{
		

		event tempEvent;
		concreteEventTransaction(tempEvent, eventName);

		printEventData(tempEvent);


		
		std::cout << std::endl;

		std::cout << "1)Izlaz" << std::endl;
		std::cout << "2)Dodaj komentar" << std::endl;
		if (userRank == 1)
		{
			std::cout << "3)Ukloni komentar" << std::endl;
		}
		int answer;
		std::cin >> answer;
		std::cout << std::endl;

		if (answer == 1)
		{
			break;
		}
		else if (answer == 2)
		{
			addComment komentar;
			//std::string eventComment(std::string comment, std::string eventName, std::string sessionID);
			std::cout << "Unesite komentar:" << std::endl;
			std::cin.ignore();
			getline(std::cin, komentar.comment);
			if (komentar.comment.length() > 400)
				std::cout << "komentar ne smije biti du�i od 400 znakova" << std::endl;
			else
			{
				komentar.eventName = eventName;
				komentar.sessionID = sessionID;

				std::ofstream file("addCommentRequest2.bin", std::ios::binary);
				cereal::BinaryOutputArchive oarchive(file);
				oarchive(komentar);
				file.close();
				rename("addCommentRequest2.bin", "addCommentRequest.bin");

				while (true)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					std::ifstream response("addCommentResponse.txt");
					if (response.is_open())
					{
						std::string answer;
						//std::cin.ignore();
						std::getline(response, answer);
						std::cout << answer << std::endl;
						response.close();
						std::this_thread::sleep_for(std::chrono::milliseconds(100));
						remove("addCommentResponse.txt");
						break;
					}
				}
			}
		}
		else if ((answer == 3) && (userRank == 1))
		{
			unsigned int toRemove;
			std::cout << std::endl;
			std::cout << "Unesite redni broj komentara: " << std::endl;
			std::cin >> toRemove;
			toRemove--;
			if ((toRemove >= 0) && (toRemove < tempEvent.userNames.size()) && (tempEvent.userNames.size() > 0)) //problem ako nema komentara?
			{
				//std::string removeComment(std::string commentID, std::string eventName);
				removeCommentStruct tempStruct;
				tempStruct.sessionID = sessionID;
				tempStruct.eventName = eventName;
				tempStruct.commentID = tempEvent.commentIDs[toRemove];

				std::ofstream request("removeCommentRequest2.bin", std::ios::binary);
				cereal::BinaryOutputArchive oarchive(request);
				oarchive(tempStruct);

				request.close();
				rename("removeCommentRequest2.bin", "removeCommentRequest.bin");

				while (true)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					std::ifstream response("removeCommentAnswer.txt");
					if (response.is_open())
					{
						std::string answer;
						std::getline(response, answer);
						std::cout << answer << std::endl;
						response.close();
						remove("removeCommentAnswer.txt");
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
						break;
					}
				}

			}
			else
				greska();


		}
		else
		{
			std::cout << "Greska" << std::endl;
		}

	}




}

void insertionSort(std::vector<event>& events, int n)
{
	int i, key, j;
	event tempEvent;

	for (i = 1; i < n; i++)
	{
		key = events[i].data.startYear;
		tempEvent = events[i];
		j = i - 1;

		
		while (j >= 0 && events[j].data.startYear >= key)
		{	
			if (events[j].data.startYear == key)
			{
				if (events[j].data.startMonth > tempEvent.data.startMonth)
				{
					events[j + 1] = events[j];
					j = j - 1;
				}
				else if (events[j].data.startDay > tempEvent.data.startDay)
				{
					events[j + 1] = events[j];
					j = j - 1;
				}
				else if (events[j].data.startHour > tempEvent.data.startHour)
				{
					events[j + 1] = events[j];
					j = j - 1;
				}
				else if (events[j].data.startMinute > tempEvent.data.startMinute)
				{
					events[j + 1] = events[j];
					j = j - 1;
				}
				else
					break;
			}
			else
			{
				events[j + 1] = events[j];
				j = j - 1;
			}
			
		}
		//events[j + 1].data.startYear = key;
		events[j + 1] = tempEvent;
	}

}

void pregledDogadjaja()
{

	system("cls");
	eventsFilter tempStruct;
	char tempChar;


	bool sortByDate = false;

	std::cout << "Pregledati prosle dogadjaje (d/n)?" << std::endl;
	std::cin >> tempChar;
	if (tempChar == 'd')
		tempStruct.pastEvents = true;

	std::cout << "Pregledati danasnje dogadjaje (d/n)?" << std::endl;
	std::cin >> tempChar;
	if (tempChar == 'd')
		tempStruct.todayEvents = true;


	std::cout << "Pregledati buduce dogadjaje (d/n)?" << std::endl;
	std::cin >> tempChar;
	if (tempChar == 'd')
		tempStruct.futureEvents = true;

	std::cout << "Sortirati dogadjaje po datumu i vremenu (rastuci)? (d/n)" << std::endl;
	std:: cin >> tempChar;
	if (tempChar == 'd')
		sortByDate = true;

	std::ofstream getCategories("changeCategoriesRequest2.txt");
	getCategories.close();
	rename("changeCategoriesRequest2.txt", "changeCategoriesRequest.txt");

	std::vector<std::string> categories;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream getCategories("changeCategoriesResponse.bin", std::ios::binary);
		if (getCategories.is_open())
		{
			cereal::BinaryInputArchive iarchive(getCategories);
			iarchive(categories);

			getCategories.close();
			remove("changeCategoriesResponse.bin");
			break;
		}
	}

	std::cout << "Kategorije: " << std::endl;
	for (unsigned int i = 0; i < categories.size(); i++)
		std::cout << i + 1 << ")" << categories[i] << std::endl;

	std::cout << std::endl;
	unsigned short choice;
	while ((categories.size() > 0))
	{
		std::cout << "Koju kategoriju zelite odabrati? (0 za nijednu)" << std::endl;
		std::cin >> choice;
		if ((choice >= 0) && (choice <= categories.size()))
			break;
	}


	if (choice != 0)
		tempStruct.category = categories[choice - 1];


	std::ofstream request("checkEvents2.bin");
	cereal::BinaryOutputArchive oarchive(request);
	oarchive(tempStruct);
	request.close();
	rename("checkEvents2.bin", "checkEvents.bin");

	
	std::vector<event> events;
	

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream eventList("eventsList.bin", std::ios::binary);
		if (eventList.is_open())
		{
			cereal::BinaryInputArchive iarchive(eventList);
			iarchive(events);

			eventList.close();
			remove("eventsList.bin");
			break;
		}
	}

	//call insertion sort here
	if (sortByDate == true)
		insertionSort(events, events.size());

	//ispisati dogadjaje
	while (true)
	{
		system("cls");

		std::cout << std::string(30, '=') << std::endl;
		for (unsigned int i = 0; i < events.size(); i++)
		{	
			//dodati brojevi na size_t var-ove su pomjeraji zbog hardkodiranih stringova
			size_t nameLen = events[i].data.eventName.length() + 5;
			size_t shortDescriptionLen = events[i].data.shortDescription.length();
			size_t locationLen = events[i].data.location.length() + 10;
			size_t categoryLen = events[i].category.length() + 12;

			events[i].data.shortDescription.erase(std::remove(events[i].data.shortDescription.begin(), events[i].data.shortDescription.end(), '\n'),
				events[i].data.shortDescription.end());
			
			
			//std::cout << "***" << i + 1 << "." << "***" << std::endl;
			int numOfDigits = countDigit(i);
			int numOfUnderscores = nameLen / 2;
			if ((nameLen % 2) == 0)
				numOfUnderscores--;
			if (numOfDigits > 1)
				numOfUnderscores -= numOfDigits;

			std::cout << std::string(nameLen / 2, '_') << i + 1 << "." << std::string(numOfUnderscores, '_') << std::endl;

			std::cout << std::string(nameLen, '-') << "+" << std::endl;
			std::cout << "Ime: " << events[i].data.eventName << "|" << std::endl;
			std::cout << std::string(nameLen, '-') << "+" << std::endl;

			std::cout << "Kratak opis: " << std::endl;
			std::cout << std::string(shortDescriptionLen, '-') << "+" << std::endl;
			std::cout << events[i].data.shortDescription << " |" << std::endl;
			std::cout << std::string(shortDescriptionLen, '-') << "+" << std::endl;

			std::cout << std::string(locationLen, '-') << "+" << std::endl;
			std::cout << "Lokacija: " << events[i].data.location << "|" << std::endl;
			std::cout << std::string(locationLen, '-') << "+" << std::endl;


			std::cout << std::string(categoryLen, '-') << "+" << std::endl;
			std::cout << "Kategorija: " << events[i].category << "|" << std::endl;
			std::cout << std::string(categoryLen, '-') << "+" << std::endl;

			std::cout << std::string(5, '-') << "+" << std::endl;
			std::cout << "Trajanje: ";
			std::cout << events[i].data.startDay << "." << events[i].data.startMonth << "." << events[i].data.startYear << ". (";
			std::cout << events[i].data.startHour << ":";
			if (events[i].data.startMinute < 10)
				std::cout << "0";
			std::cout << events[i].data.startMinute << ") - ";
			std::cout << events[i].data.endDay << "." << events[i].data.endMonth << "." << events[i].data.endYear << ". (";
			std::cout << events[i].data.endHour << ":";
			if (events[i].data.endMinute < 10)
				std::cout << "0";
			std::cout << events[i].data.endMinute << ")" << std::endl;

			std::cout << std::string(5, '-') << "+" << std::endl;

			if (events[i].data.specialRequirements.size() > 0)
			{
				std::cout << std::string(18, '-') << "+" << std::endl;
				std::cout << "Posebni zahtjevi: |" << std::endl;
				std::cout << std::string(18, '-') << "+" << std::endl;

				for (unsigned int j = 0; j < events[i].data.specialRequirements.size() - 1; j++)
					std::cout << events[i].data.specialRequirements[j] << ", ";
				std::cout << events[i].data.specialRequirements.back() << std::endl;
			}
			std::cout << std::endl << std::endl;
			std::cout << std::string(16, '-') << "+" << std::endl;
			std::cout << "Opis dogadjaja: |" << std::endl;
			std::cout << std::string(16, '-') << "+" << std::endl;

			std::cout << events[i].description;
			std::cout << std::string(30, '=') << std::endl;

		}

		std::cout << std::endl << std::endl;
		std::cout << "1)Nazad" << std::endl;
		std::cout << "2)Odaberi dogadjaj" << std::endl;
		std::cout << std::endl;



		do
		{
			std::cin >> choice;
		} while ((choice < 1) || (choice > 2));

		if (choice == 1)
			return;
		else
		{
			unsigned short chooseEvent;
			do
			{
				std::cout << "Odaberite redni broj dogadjaja: " << std::endl;
				std::cin >> chooseEvent;
			} while ((chooseEvent < 1) || (chooseEvent > events.size()) && (events.size() > 0));

			std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			getEvent(events[chooseEvent - 1].data.eventName);
		}
	}

}



void logOut()
{
	std::ofstream fajl("logOutRequest.txt", std::ios::out);
	fajl << sessionID;
	fajl.close();

}


void editOwnEvent(std::string event2Edit)
{
	event tempEvent;
	
	concreteEventTransaction(tempEvent, event2Edit);

	//delete vector comments
	tempEvent.commentIDs.clear();
	tempEvent.comments.clear();
	tempEvent.userNames.clear();

	printEventData(tempEvent);
	
	char choice;
	std::cout << "Da li zelite obrisati dogadjaj? (d/n)" << std::endl;
	std::cin >> choice;

	if (choice == 'd')
	{
		removeEvent(event2Edit);
	}
	else if (choice == 'n')
	{
		std::cout << "Da li zelite izmijeniti kratki opis? (d/n)" << std::endl;
		std::cin >> choice;

		if (choice == 'd')
		{
			std::cout << "Unesite novi kratki opis:" << std::endl;
			std::string tempStr;
			std::cin.ignore();
			std::getline(std::cin, tempStr); //mozda treba cin.ignore()

			tempEvent.data.shortDescription = tempStr;
		}

		std::cout << "Da li zelite izmijeniti kategoriju? (d/n)" << std::endl;
		std::cin >> choice;

		if (choice == 'd')
		{
			std::cout << "Unesite novu kategoriju:" << std::endl;
			std::string tempStr;
			std::cin.ignore();
			std::getline(std::cin, tempStr); //mozda treba cin.ignore()

			tempEvent.category = tempStr;
		}

		std::cout << "Da li zelite izmijeniti vrijeme? (d/n)" << std::endl;
		std::cin >> choice;

		if (choice == 'd')
		{
			std::cout << "Unesite novo vrijeme:" << std::endl;
			addEventTime(tempEvent.data);
			

		}

		std::cout << "Da li zelite izmijeniti lokaciju? (d/n)" << std::endl;
		std::cin >> choice;

		if (choice == 'd')
		{
			std::cout << "Unesite novu lokaciju:" << std::endl;
			std::string tempStr;
			std::cin.ignore(); //cin ignore, mozda je greska
			std::getline(std::cin, tempStr);
			tempEvent.data.location = tempStr;

		}
		//send data to server
		std::ofstream outputFile("editOwnEvent2.bin", std::ios::binary);

		cereal::BinaryOutputArchive oarchive(outputFile);
		oarchive(tempEvent);
		outputFile.close();
		rename("editOwnEvent2.bin", "editOwnEvent.bin");

		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			std::ifstream response("editOwnEventResponse.txt");
			if (response.is_open())
			{
				std::string tempStr;
				std::getline(response, tempStr);
				std::cout << tempStr << std::endl;
				response.close();
				std::this_thread::sleep_for(std::chrono::milliseconds(1000));
				remove("editOwnEventResponse.txt");
				break;
			}
		}

	}




}

void pregledVlastitihDogadajaTransaction(std::vector<std::string>& ownEvents)
{
	std::ofstream file("ownEventsRequest.txt");
	file << sessionID;
	file.close();


	
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream response("ownEventsAnswer.bin", std::ios::binary);
		if (response.is_open())
		{
			cereal::BinaryInputArchive iarchive(response);
			iarchive(ownEvents);
			response.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("ownEventsAnswer.bin");
			break;
		}
	}
}

void pregledVlastitihDogadaja()
{
	
	std::vector<std::string> ownEvents;
	pregledVlastitihDogadajaTransaction(ownEvents);

	bool removed = false;

	while (true)
	{
		if (removed == true)
		{
			pregledVlastitihDogadajaTransaction(ownEvents);
			removed = false;
		}
			

		system("cls");
		for (unsigned int i = 0; i < ownEvents.size(); i++)
			std::cout << i + 1 << ")" << ownEvents[i] << std::endl;

		std::cout << std::endl;
		int response;
		std::cout << "1)Pregledaj dogadjaj" << std::endl;
		std::cout << "2)Izlaz" << std::endl;
		std::cout << "3)Promjena dogadjaja" << std::endl;
		std::cin >> response;

		if (response == 1)
		{
			unsigned int event;
			std::cout << "Unesite redni broj dogadjaja" << std::endl;
			std::cin >> event;
			if ((event > 0) && (event <= ownEvents.size()))
			{
				getEvent(ownEvents[event - 1]);
				system("cls");
			}
			else
				greska();

		}
		else if (response == 2)
		{
			break;
		}
		else if (response == 3)
		{
			unsigned int event;
			std::cout << "Unesite redni broj dogadjaja" << std::endl;
			std::cin >> event;
			if ((event > 0) && (event <= ownEvents.size()))
			{
				editOwnEvent(ownEvents[event - 1]);
				removed = true;
			}
				
			else
				greska();
		}
		else
			greska();

	}
}


void banovanjeKorisnika()
{
	banUserStruct tempStruct;
	tempStruct.sessionID = sessionID;

	std::cout << "Unesite ime korisnika za banovanje:" << std::endl;
	std::cin.ignore();
	std::getline(std::cin, tempStruct.user2Ban);


	{
		std::ofstream file("banRequest.bin", std::ios::binary);
		cereal::BinaryOutputArchive oarchive(file);
		oarchive(tempStruct);
	}

	std::string message;
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		std::ifstream response("banRequestAnswer.txt");
		if (response.is_open())
		{
			std::getline(response, message);
			response.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(300));
			remove("banRequestAnswer.txt");
			break;
		}
	}




	std::cout << message << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	system("cls");
}





void izmjenaKategorija()
{
	std::cout << "";
	system("cls");
	std::ofstream file("changeCategoriesRequest.txt");
	file << sessionID;
	file.close();


	std::vector<std::string> categories;


	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream response("changeCategoriesResponse.bin", std::ios::binary);
		if (response.is_open())
		{
			cereal::BinaryInputArchive iarchive(response);
			iarchive(categories);
			response.close();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			remove("changeCategoriesResponse.bin");
			break;
		}
		response.close();
	}

	//modify

	std::string temp = categories[0];
	categories[0] = sessionID;
	categories.push_back(temp);

	while (true)
	{
		system("cls");
		int counter = 1;
		int choice;
		for (auto& category : categories)
			if (category == categories[0])
				continue;
			else
				std::cout << counter++ << ")" << category << std::endl;

		std::cout << std::endl;
		std::cout << "1)Obrisi" << std::endl;
		std::cout << "2)Dodaj" << std::endl;
		std::cout << "3)Izlaz" << std::endl;
		std::cin >> choice;


		if (choice == 1)
		{
			unsigned int toRemove;
			std::cout << "Unesite redni broj kategorije" << std::endl;
			std::cin >> toRemove;


			if ((toRemove > 0) && (toRemove < categories.size()) && (categories.size() > 0))
			{
				categories.erase(categories.begin() + toRemove);
			}
			else
				greska();


		}
		else if (choice == 2)
		{
			std::string newCategory;
			std::cout << "Unesite ime nove kategorije:" << std::endl;
			std::cin.ignore();
			std::getline(std::cin, newCategory);
			categories.push_back(newCategory);
		}
		else if (choice == 3)
			break;

		else
			greska();

	}

	std::cin.ignore();
	{
		std::ofstream file("addNewCategories.bin", std::ios::binary);
		cereal::BinaryOutputArchive oarchive(file);
		oarchive(categories);
	}



}



void printMenu()
{
	system("cls");
	std::cout << std::setw(85) << "+ -------------------------Evenizer------------------------+" << std::endl;
	std::cout <<"			 | 0)Izlaz" << std::setw(51) << "|" << std::endl;
	std::cout <<"			 | 1)Pregled dogadjaja" << std::setw(39) << "|" << std::endl;
	std::cout <<"			 | 2)Igranje kviza" << std::setw(43) << "|" << std::endl;
	std::cout << "			 | 3)Informacije" << std::setw(45) << "|" << std::endl;
	
	if (loggedIn)
	{
		std::cout << "			 | 4)Dodavanje dogadjaja" << std::setw(37) << "|" << std::endl;
		std::cout << "			 | 5)Pregled vlastitih dogadjaja" << std::setw(29) << "|" << std::endl;
		std::cout << "			 | 6)Brisanje dogadjaja" << std::setw(38) << "|" << std::endl;
	}
	else
		std::cout <<"			 | 4)Prijava" << std::setw(49) << "|" << std::endl;

	if (userRank == 1)
	{
		std::cout << "			 | 7)Banovanje korisnika" <<  std::setw(37) << "|" << std::endl;
		std::cout << "			 | 8)Izmjena kategorija" <<  std::setw(38) << "|" << std::endl;
		std::cout << "			 | 9)Izmjena kviza" <<  std::setw(43) << "|" << std::endl;
	}

	
	std::cout << std::setw(85) << "+ ---------------------------------------------------------+" << std::endl;
}

//		std::this_thread::sleep_for(std::chrono::milliseconds(200));

void displayInfo(std::string info)
{
	system("cls");
	
	std::string input;
	std::cout << info << std::endl;
	
	std::cout << std::endl << std::endl;

	std::cout << "Unesite bilo sta za povratak u meni" << std::endl;
	std::cin >> input;
	
}

int main()
{
	std::ofstream getInfo("getInfo.txt");
	getInfo.close();
	std::string info;
	
	

	while (true)
	{
		std::ifstream getInfoResponse("getInfoResponse.txt");
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		if (getInfoResponse.is_open())
		{
			std::stringstream buffer;
			buffer << getInfoResponse.rdbuf();
			info = buffer.str();
			getInfoResponse.close();
			remove("getInfoResponse.txt");
			break;
		}
	}

	bool run = true;
	while (run)
	{
		printMenu();
		short input;
		std::cin >> input;
		switch (input)
		{
		case 0:
			run = false;
			if (loggedIn)
				logOut();
			break;
		case 1:
			pregledDogadjaja();
			break;
		case 2:
			igrajKviz();
			break;
		case 3:
			displayInfo(info);
			break;
		case 4:
			if (loggedIn)
				dodavanjeDogadjaja();
			else
				logIn();
			break;
		case 5:
			if (!loggedIn)
				logIn();
			else
				pregledVlastitihDogadaja();
			break;
		case 6:
			if (loggedIn && userRank == 1)
				removeEvent("");
			else
				greska();
			break;
		case 7:
			if (loggedIn && userRank == 1)
				banovanjeKorisnika();
			else
				greska();
			break;

		case 8:
			if (loggedIn && userRank == 1)
				izmjenaKategorija();
			else
				greska();
			break;
		case 9:
			if (loggedIn && userRank == 1)
				izmjenaKviza();
			else
				greska();
			break;

		default:
			greska();
		}

	}

}
