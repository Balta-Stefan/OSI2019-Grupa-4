#ifndef COMMONSTRUCTURES_H_INCLUDED
#define COMMONSTRUCTURES_H_INCLUDED
struct banUserStruct
{
	std::string user2Ban, sessionID;

	template <class Archive>
	void serialize(Archive& ar)
	{
		ar(sessionID, user2Ban);
	}
};


#endif // COMMONSTRUCTURES_H_INCLUDED
