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




std::string sessionID = "";
int userRank = 0;
bool loggedIn = false;

std::string successfulLogOut = "Uspjesno odjavljivanje.";

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
	bool notDuplicate{}; //omogucava korisniku da doda dogadjaj ako se posumnja da je dogadjaj mozda vec dodan

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(eventData, category, description, notDuplicate);
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


struct banUser
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

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(questions, answers);
	}
};



void greska()
{
	std::cout << "greska" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	system("cls");
}

void dodavanjeDogadjaja()
{
	//backend gives answer via a text file called addEventAnswer.txt

	newEvent tempEvent;
	tempEvent.sessionID = sessionID;
	tempEvent.notDuplicate = false;

	std::cout << "ime dogadjaja: ";
	std::cin >> tempEvent.eventData.eventName;
	std::cout << "kratak opis:" << std::endl;
	std::getline(std::cin, tempEvent.eventData.shortDescription);

	std::cout << "pocetak (sat)" << " ";
	std::cin >> tempEvent.eventData.startHour;
	std::cout << std::endl;

	std::cout << "pocetak (minuta)" << " ";
	std::cin >> tempEvent.eventData.startMinute;
	std::cout << std::endl;

	std::cout << "pocetak (mjesec)" << " ";
	std::cin >> tempEvent.eventData.startMonth;
	std::cout << std::endl;

	std::cout << "pocetak (dan)" << " ";
	std::cin >> tempEvent.eventData.startDay;
	std::cout << std::endl;

	std::cout << "pocetak (godina)" << " ";
	std::cin >> tempEvent.eventData.startYear;
	std::cout << std::endl;

	std::cout << "kraj (sat)" << " ";
	std::cin >> tempEvent.eventData.endHour;
	std::cout << std::endl;

	std::cout << "kraj (minuta)" << " ";
	std::cin >> tempEvent.eventData.endMinute;
	std::cout << std::endl;

	std::cout << "kraj (mjesec)" << " ";
	std::cin >> tempEvent.eventData.endMonth;
	std::cout << std::endl;

	std::cout << "kraj (dan)" << " ";
	std::cin >> tempEvent.eventData.endDay;
	std::cout << std::endl;

	std::cout << "kraj (godina)" << " ";
	std::cin >> tempEvent.eventData.endYear;
	std::cout << std::endl;


	std::cout << "lokacija: ";
	std::cin >> tempEvent.eventData.location;
	std::cout << std::endl;

	int brojZahtjeva;

	std::cout << "broj posebnih zahtjeva: ";
	std::cin >> brojZahtjeva;
	std::cout << std::endl;

	for (int i = 0; i < brojZahtjeva; i++)
	{
		std::string temp;
		std::cin >> temp;
		tempEvent.eventData.specialRequirements.push_back(temp);
	}

	std::cout << "kategorija: ";
	std::cin >> tempEvent.category;
	std::cout << std::endl;

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
			input >> message;

			input.close();
			remove("addEventAnswer.txt");
			std::cout << message;
		}

		input.close();
	}
}

void logIn()
{	
	std::string userName, password;
	char tempChar;
	const char ENTER = 13;

	std::cout << "unesite korisnicko ime: " << std::endl;
	std::getline(std::cin, userName);
	std::cout << std::endl;
	std::cout << "unesite lozinku: " << std::endl;



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

	std::cout << tempLoginInfo.message << std::endl;
	if (tempLoginInfo.sessionID.length() > 0)
	{
		loggedIn = true;
		userRank = tempLoginInfo.userRank;
		sessionID = tempLoginInfo.sessionID;
	}

	remove("loginRequestAnswer.bin");

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	system("cls");

}

void igrajKviz()
{	
	//backend gives answer via quiz.txt, format is: num of lines \n all questions then all answers (each entry is in new line)

	std::ofstream request("quizRequest.txt");
	request.close();
	

	quiz kviz;

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream answer;
		answer.open("quiz.txt", std::ios::in);
		if (answer.is_open())
		{
			cereal::BinaryInputArchive iarchive(answer);
			iarchive(kviz);

			answer.close();
			remove("quiz.txt");
			break;
		}
		answer.close();
	}

	quiz tempKviz;
	tempKviz.questions = kviz.questions;

	for (int i = 0; i < 3; i++)
	{
		std::cout << "Pitanje " << i + 1 << ": " << kviz.questions[i] << std::endl;
		std::cout << "1)" << kviz.answers[i * 4] << std::endl;
		std::cout << "2)" << kviz.answers[i * 4 + 1] << std::endl;
		std::cout << "3)" << kviz.answers[i * 4 + 2] << std::endl;

		short answer;
		std::cin >> answer;
		tempKviz.answers.push_back(kviz.answers[(answer-1)+i*3]);
	}
	



	//check validity of answers
	{
		std::ofstream giveAnswers("quizAnswers.bin", std::ios::binary);

		cereal::BinaryOutputArchive oarchive(giveAnswers);
		oarchive(tempKviz);
	}

	
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
		std::ifstream validAnswers("correctAnswers.txt");
		if (validAnswers.is_open())
		{
			int valid;
			validAnswers >> valid;
			std::cout << "broj ispravnih odgovora: " << valid << std::endl;
			validAnswers.close();
			remove("correctAnswers.txt");
			return;
		}
	}





}

void pregledDogadjaja()
{

}

void getEvent(std::string eventName)
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
			remove("getEventResponse.bin");
			break;
		}
	}
	

	

	std::cout << "ime dogadjaja: " << tempEvent.data.eventName << std::endl;
	std::cout << "trajanje dogadjaja: " << 
		tempEvent.data.startDay << "." << tempEvent.data.startMonth << "." << tempEvent.data.startYear << " " <<
		tempEvent.data.startHour << ":" << tempEvent.data.startMinute << "-" <<
		tempEvent.data.endDay << "." << tempEvent.data.endMonth << "." << tempEvent.data.endYear << " " <<
		tempEvent.data.endHour << ":" << tempEvent.data.endMinute << std::endl;
	std::cout << "lokacija: " << tempEvent.data.location << std::endl;
	if (tempEvent.data.specialRequirements.size() > 0)
	{
		std::cout << "posebni zahtjevi: " << std::endl;
		for (auto& i : tempEvent.data.specialRequirements)
			std::cout << i << std::endl;
	}

	std::cout << "kategorija: " << tempEvent.category << std::endl;
	std::cout << "opis: " << std::endl;
	std::cout << tempEvent.description << std::endl;

	for (unsigned int i = 0; i < tempEvent.userNames.size(); i++)
	{
		std::cout << tempEvent.userNames[i] << std::endl;
		std::cout << tempEvent.comments[i] << std::endl;
	}

	std::cout << "1)izlaz" << std::endl;
	std::cout << "2)dodaj komentar" << std::endl;

	while (true)
	{
		int answer;
		std::cin >> answer;

		if (answer == 1)
		{
			break;
		}
		else if (answer == 2)
		{	
			addComment komentar;
			//std::string eventComment(std::string comment, std::string eventName, std::string sessionID);
			std::cout << "unesite komentar:" << std::endl;
			getline(std::cin, komentar.comment);
			if (komentar.comment.length() > 400)
				std::cout << "komentar ne smije biti duži od 400 znakova" << std::endl;
			else
			{	
				komentar.eventName = eventName;
				komentar.sessionID = sessionID;

				std::ofstream file("addCommentRequest.bin", std::ios::binary);
				cereal::BinaryOutputArchive oarchive(file);
				oarchive(komentar);
				file.close();

				while (true)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(200));
					std::ifstream response("addCommentResponse.txt");
					if (response.is_open())
					{
						std::string answer;
						std::getline(response, answer);
						std::cout << "answer" << std::endl;
						response.close();
						remove("addCommentResponse.txt");
						break;
					}
				}
			}
		}
		else
		{
			std::cout << "greska" << std::endl;
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
			remove("ownEventsAnswer.bin");
			break;
		}
	}

	while (true)
	{
		int response;
		std::cout << "1)pregledaj dogadjaj" << std::endl;
		std::cout << "2)izlaz" << std::endl;
		std::cin >> response;

		if (response == 1)
		{
			unsigned int event;
			std::cout << "unesite redni broj dogadjaja" << std::endl;
			std::cin >> event;
			if ((event >= 0) && (event < ownEvents.size()))
			{
				getEvent(ownEvents[event]);
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
	banUser tempStruct;
	tempStruct.sessionID = sessionID;

	std::cout << "unesite ime korisnika za banovanje:" << std::endl;
	std::getline(std::cin, tempStruct.user2Ban);
	

	{
		std::ofstream file("banRequest.bin", std::ios::binary);
		cereal::BinaryOutputArchive oarchive(file);
		oarchive(tempStruct);
	}


	std::ifstream response("banRequestAnswer.txt");
	std::string message;
	response >> message;
	response.close();
	remove("banRequestAnswer.txt");

	std::cout << message << std::endl;

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));

	system("cls");
}





void izmjenaKategorija()
{
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
			remove("changeCategoriesResponse.bin");
			break;
		}
		response.close();
	}

	//modify


	while (true)
	{
		int counter = 1;
		int choice;
		for (auto& category : categories)
			std::cout << counter++ << ")" << category << std::endl;

		std::cout << "1)obrisi" << std::endl;
		std::cout << "2)dodaj" << std::endl;
		std::cout << "3)izlaz" << std::endl;
		std::cin >> choice;

		if (choice == 1)
		{
			int toRemove;
			std::cout << "unesite redni broj dogadjaja" << std::endl;
			std::cin >> toRemove;
			toRemove--;

			if ((toRemove >= 0) && (toRemove < categories.size()))
			{
				categories.erase(categories.begin() + toRemove);
			}
			else
				greska();

		}
		else if (choice == 2)
		{
			std::string newCategory;
			std::cout << "unesite ime nove kategorije:" << std::endl;
			std::getline(std::cin, newCategory);
		}
		else if (choice == 3)
			break;
		else
			greska();
			
	}

	{
		std::ofstream file("addNewCategories.bin", std::ios::binary);
		cereal::BinaryOutputArchive oarchive(file);
		oarchive(categories);
	}

	

}

void printMenu()
{
	system("cls");
	std::cout << "1)pregled događaja" << std::endl;
	std::cout << "2)dodavanje događaja" << std::endl;
	std::cout << "3)igranje kviza" << std::endl;

	if (loggedIn)
	{
		std::cout << "4)pregled vlastitih događaja" << std::endl;
		std::cout << "5)izlaz" << std::endl;
	}
	else
		std::cout << "4)prijava" << std::endl;

	if (userRank == 1)
	{
		std::cout << "6)banovanje korisnika" << std::endl;
		std::cout << "7)izmjena kategorija" << std::endl;
	}
	

}

//		std::this_thread::sleep_for(std::chrono::milliseconds(200));



int main()
{
	bool run = true;
	while (true)
	{
		printMenu();
		short input;
		std::cin >> input;
		switch (input)
		{
			case 1:
				pregledDogadjaja();
				break;
			case 2:
				dodavanjeDogadjaja();
				break;
			case 3:
				igrajKviz;
				break;
			case 4:
				if (!loggedIn)
					logIn();
				else
					pregledVlastitihDogadaja();
				break;
			case 5:
				if (loggedIn)
				{
					logOut();
					run = false;
				}
				else
					greska();
				break;
			case 6:
				if (loggedIn && userRank == 1)
					banovanjeKorisnika();
				else
					greska();

			case 7:
				if (loggedIn && userRank == 1)
					izmjenaKategorija();
				else
					greska();

			default:
				greska();
		}

	}

}
