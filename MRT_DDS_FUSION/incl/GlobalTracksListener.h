#ifndef _GLOBALTRACKS_LISTENER_H_
#define _GLOBALTRACKS_LISTENER_H_

#include <boost/thread.hpp>
#include <boost/unordered_map.hpp>
#include <boost/fusion/adapted/std_pair.hpp>
#include <boost/fusion/include/std_pair.hpp>

#include "defines.h"
#include "DdsArchLib.h" 
#include "DataDefinitions.h"
#include "ContentFilteredTopics.h"
#include "TracksSourcesDataListener.h"
#include "TrackConversions.h"
#include "TrackFusion.h"

class GlobalTracksListener : public DdsArchLibrary::Observer
{


private:
	
	static const double deltaLat;

	TracksSourcesDataListener*	m_SourcesData;

	bool isThreadRunning;
	boost::thread*	m_thread;

	boost::mutex	m_TracksMapMutex;
	boost::unordered::unordered_map< TRACK_ID, std::pair<AD_TRACK,track_state> >	tracksMap;

	boost::unordered::unordered_map< TRACK_ID,FilteredTrackListener* >				filteredListenerMap;
	boost::unordered::unordered_map< TRACK_ID,ArchFilteredTrackSubject* >			filteredSubjectMap;
	

	TrackFusion* fusor;

public:

	GlobalTracksListener(TracksSourcesDataListener*);
	~GlobalTracksListener();

	void Start();
	void WaitForFinish();
	void operator()();

	virtual void Update(DdsArchLibrary::ISubject*);
};

#endif