#ifndef _DataDefinitions_H_
#define _DataDefinitions_H_

#include <string>
#include <boost/lexical_cast.hpp>
#include "DdsArchLib.h"

typedef std::string TRACK_ID;

enum track_state {NEW_TRACK, ANALYZED_TRACK, DELETE_TRACK};

bool operator==(AD_TRACK const&, AD_TRACK const&);

TRACK_ID calculate_track_ID(const AD_TRACK&);


struct compareTrackIDs
{
	bool operator() (const TRACK_ID&, const TRACK_ID&) const;
};



#endif