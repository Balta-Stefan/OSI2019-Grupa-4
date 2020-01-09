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

//VAZNA NAPOMENA, ZABRANITI DODAVANJE DOGADJAJA SA ISTIM IMENOM


unsigned short MAXQUESTIONS = 10; //max broj pitanja za kviz
unsigned short MAXANSWERS = 3; //max broj odgovora
std::string sessionID = "";
int userRank = 0;
bool loggedIn = false;

std::string successfulLogOut = "Uspjesno odjavljivanje.";


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


void greska()
{
	std::cout << "greska" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	system("cls");
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




void dodavanjeDogadjaja()
{
	//backend gives answer via a text file called addEventAnswer.txt

	newEvent tempEvent;
	tempEvent.sessionID = sessionID;
	tempEvent.notDuplicate = false;

	std::cout << "Ime dogadjaja: " << std::endl;
	std::cin.ignore();
	std::getline(std::cin, tempEvent.eventData.eventName);
	std::cout << "Ime je: " << tempEvent.eventData.eventName << std::endl;
	//std::cin >> tempEvent.eventData.eventName;
	std::cout << "Kratak opis:" << std::endl;
	//std::cin.ignore();
	std::getline(std::cin, tempEvent.eventData.shortDescription);
	std::cout << "Kratki opis je: " << tempEvent.eventData.shortDescription << std::endl;

	std::cout << "Pocetak (sat)" << " ";
	std::cin >> tempEvent.eventData.startHour;
	std::cout << std::endl;

	std::cout << "Pocetak (minuta)" << " ";
	std::cin >> tempEvent.eventData.startMinute;
	std::cout << std::endl;

	std::cout << "Pocetak (mjesec)" << " ";
	std::cin >> tempEvent.eventData.startMonth;
	std::cout << std::endl;

	std::cout << "Pocetak (dan)" << " ";
	std::cin >> tempEvent.eventData.startDay;
	std::cout << std::endl;

	std::cout << "Pocetak (godina)" << " ";
	std::cin >> tempEvent.eventData.startYear;
	std::cout << std::endl;

	std::cout << "Kraj (sat)" << " ";
	std::cin >> tempEvent.eventData.endHour;
	std::cout << std::endl;

	std::cout << "Kraj (minuta)" << " ";
	std::cin >> tempEvent.eventData.endMinute;
	std::cout << std::endl;

	std::cout << "Kraj (mjesec)" << " ";
	std::cin >> tempEvent.eventData.endMonth;
	std::cout << std::endl;

	std::cout << "Kraj (dan)" << " ";
	std::cin >> tempEvent.eventData.endDay;
	std::cout << std::endl;

	std::cout << "Kraj (godina)" << " ";
	std::cin >> tempEvent.eventData.endYear;
	std::cout << std::endl;


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

	for (int i = 0; i < 10; i++)
	{
		std::cout << "Pitanje " << i + 1 << ": " << kviz.questions[i] << std::endl;
		std::cout << "1)" << kviz.answers[i * 3] << std::endl;
		std::cout << "2)" << kviz.answers[i * 3 + 1] << std::endl;
		std::cout << "3)" << kviz.answers[i * 3 + 2] << std::endl;

		short answer;
		std::cin >> answer;
		tempKviz.answers.push_back(kviz.answers[(answer - 1) + i * 3]);
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
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			validAnswers.close();
			remove("correctAnswers.txt");
			return;
		}
	}





}

void getEvent(std::string eventName)
{

	while (true)
	{
		std::ofstream file("getEvent.txt");
		file << eventName;
		file.close();

		event tempEvent;
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
			std::ifstream response("getEventResponse.bin", std::ios::binary);

			if (response.is_open())
			{
				cereal::BinaryInputArchive iarchive(response);
				iarchive(tempEvent);
				response.close();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				remove("getEventResponse.bin");
				break;
			}
		}



		system("cls");
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
				std::cout << "komentar ne smije biti duži od 400 znakova" << std::endl;
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

void pregledDogadjaja()
{

	system("cls");
	eventsFilter tempStruct;
	short tempChar;

	std::cout << "Pregledati prosle dogadjaje (0/1)?" << std::endl;
	std::cin >> tempChar;
	if (tempChar == 1)
		tempStruct.pastEvents = true;

	std::cout << "Pregledati danasnje dogadjaje (0/1)?" << std::endl;
	std::cin >> tempChar;
	if (tempChar == 1)
		tempStruct.todayEvents = true;


	std::cout << "Pregledati buduce dogadjaje (0/1)?" << std::endl;
	std::cin >> tempChar;
	if (tempChar == 1)
		tempStruct.futureEvents = true;

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

	//ispisati dogadjaje
	while (true)
	{
		system("cls");


		for (unsigned int i = 0; i < events.size(); i++)
		{
			std::cout << i + 1 << std::endl;
			std::cout << "Ime: " << events[i].data.eventName << std::endl;
			std::cout << "Kratak opis: " << std::endl << events[i].data.shortDescription << std::endl;
			std::cout << "Lokacija: " << events[i].data.location << std::endl;
			std::cout << "Kategorija: " << events[i].category << std::endl;
			if (events[i].data.specialRequirements.size() > 0)
			{
				std::cout << "Posebni zahtjevi: " << std::endl;

				for (unsigned int j = 0; j < events[i].data.specialRequirements.size() - 1; j++)
					std::cout << events[i].data.specialRequirements[j] << ", ";
				std::cout << events[i].data.specialRequirements.back() << std::endl;
			}

		}

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

void pregledVlastitihDogadaja()
{
	system("cls");
	std::ofstream file("ownEventsRequest.txt");
	file << sessionID;
	file.close();


	std::vector<std::string> ownEvents;
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

	while (true)
	{
		for (unsigned int i = 0; i < ownEvents.size(); i++)
			std::cout << i + 1 << ")" << ownEvents[i] << std::endl;

		std::cout << std::endl;
		int response;
		std::cout << "1)Pregledaj dogadjaj" << std::endl;
		std::cout << "2)Izlaz" << std::endl;
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
	std::cout << "0)Izlaz" << std::endl;
	std::cout << "1)Pregled dogadjaja" << std::endl;
	std::cout << "2)Igranje kviza" << std::endl;

	if (loggedIn)
	{
		std::cout << "3)Dodavanje dogadjaja" << std::endl;
		std::cout << "4)Pregled vlastitih dogadjaja" << std::endl;
	}
	else
		std::cout << "3)Prijava" << std::endl;

	if (userRank == 1)
	{
		std::cout << "5)Banovanje korisnika" << std::endl;
		std::cout << "6)Izmjena kategorija" << std::endl;
		std::cout << "7)Izmjena kviza" << std::endl;
	}


}

//		std::this_thread::sleep_for(std::chrono::milliseconds(200));



int main()
{
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
		case 3:
			if (loggedIn)
				dodavanjeDogadjaja();
			else
				logIn();
			break;
		case 2:
			igrajKviz();
			break;
		case 4:
			if (!loggedIn)
				logIn();
			else
				pregledVlastitihDogadaja();
			break;
		case 5:
			if (loggedIn && userRank == 1)
				banovanjeKorisnika();
			else
				greska();
			break;

		case 6:
			if (loggedIn && userRank == 1)
				izmjenaKategorija();
			else
				greska();
			break;
		case 7:
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
