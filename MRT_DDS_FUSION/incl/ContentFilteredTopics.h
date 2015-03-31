#ifndef _CONTENTFILTEREDTOPICS_H_
#define _CONTENTFILTEREDTOPICS_H_

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <set>

#include "DdsArchLib.h" 
#include "DataDefinitions.h"
#include "TrackFilter.h"
#include "TrackFusion.h"

class FilteredTrackListener: public DDSDataReaderListener 
{
private:

	static const int maxNoUpdateTimeSeconds = 10;
	
	AD_TRACK* refTrack;

	std::set<TRACK_ID,compareTrackIDs>										CorrelatedTracksSet;
	std::set<TRACK_ID,compareTrackIDs>::iterator							CorrelatedTracksSetIter;
	boost::unordered::unordered_map< TRACK_ID, boost::posix_time::ptime >	TracksTimeStamp;

	bool isThreadRunning;
	boost::thread*	m_thread;
	boost::mutex	m_SetMutex;

	TrackFusion*	fusor;

public:
	
	FilteredTrackListener(TrackFusion*, AD_TRACK*);
	void on_data_available(DDSDataReader *reader);

	void Start();
	void WaitForFinish();
	void operator()();
};

class ArchFilteredTrackSubject
{
private:
	DDSDataReader*				data_reader;
	FilteredTrackListener*		listener;
	DDSContentFilteredTopic*	cft;
	TracksSourcesDataListener*	dataSource;
	AD_TRACK*					refTrack;

public:
	ArchFilteredTrackSubject(AD_TRACK*, TracksSourcesDataListener*, FilteredTrackListener*);
	~ArchFilteredTrackSubject();

	AD_TRACK* getReferenceTrack() const;
	void UpdateTrackFilter(AD_TRACK*);
};





#endif