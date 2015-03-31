#ifndef _TRACK_FUSION_H_
#define _TRACK_FUSION_H_

#include <boost/thread.hpp>
#include <boost/unordered_map.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <set>

#include "DataDefinitions.h"

class TrackFusion
{

private:
	typedef std::set<TRACK_ID,compareTrackIDs>::iterator  trackIter;

	bool isThreadRunning;
	boost::thread*	m_thread;

	boost::mutex	m_TracksMapMutex;

	//map of correlated tracks sets
	boost::unordered::unordered_map< std::set<TRACK_ID,compareTrackIDs>, boost::posix_time::ptime >				tracksMap;
	boost::unordered::unordered_map< std::set<TRACK_ID,compareTrackIDs>, boost::posix_time::ptime >::iterator	TracksMapIter;

	//global map data
	boost::unordered::unordered_map< TRACK_ID, std::pair<AD_TRACK,track_state> >*								globalTracksMap;

	trackIter iter;


public:

	TrackFusion(boost::unordered::unordered_map< TRACK_ID, std::pair<AD_TRACK,track_state> >*);
	~TrackFusion();

	void Start();
	void WaitForFinish();
	void operator()();

	void addCorrelatedTracks(std::set<TRACK_ID,compareTrackIDs>);

};

#endif