#include "DataDefinitions.h"

bool operator==(AD_TRACK const& t1, AD_TRACK const& t2)
{
	return t1.SID == t2.SID && t1.TID == t2.TID;
}

TRACK_ID calculate_track_ID(const AD_TRACK& t)
{
	std::string s1(boost::lexical_cast<std::string>(t.SID)); 
	std::string s2(boost::lexical_cast<std::string>(t.TID));

	TRACK_ID out = s1 + "_" + s2;

	return out;
}

bool compareTrackIDs::operator()(const TRACK_ID& t1, const TRACK_ID& t2) const
{
	std::string s_SID1(t1.substr(0,t1.find('_')));
	std::string s_TID1(t1.substr(t1.find('_')+1,t1.length()-1));

	int a = t1.length();

	std::string s_SID2(t2.substr(0,t2.find('_')));
	std::string s_TID2(t2.substr(t2.find('_')+1,t2.length()-1));

	int b = t2.length();

	unsigned int SID1 = boost::lexical_cast<unsigned int>(s_SID1); 
	unsigned int TID1 = boost::lexical_cast<unsigned int>(s_TID1); 

	unsigned int SID2 = boost::lexical_cast<unsigned int>(s_SID2); 
	unsigned int TID2 = boost::lexical_cast<unsigned int>(s_TID2); 

	if(SID1 < SID2) return true;
	else if(SID1 == SID2)
	{
		if(TID1 < TID2) return true;
		else return false;
	}
	else return false;
}