#ifndef _TRACKCONVERSIONS_H_
#define _TRACKCONVERSIONS_H_

#include "defines.h"
#include "DdsArchLib.h"
#include "TracksSourcesDataListener.h"

class TrackConversions
{
public:
	static bool PolarToAbsolute(AD_TRACK& track, TracksSourcesDataListener* dataSource);
};

#endif