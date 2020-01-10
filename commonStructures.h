#ifndef COMMONSTRUCTURES_H_INCLUDED
#define COMMONSTRUCTURES_H_INCLUDED



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
	bool notDuplicate; //omogucava korisniku da doda dogadjaj ako se posumnja da je dogadjaj mozda vec dodan

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


#endif // COMMONSTRUCTURES_H_INCLUDED
