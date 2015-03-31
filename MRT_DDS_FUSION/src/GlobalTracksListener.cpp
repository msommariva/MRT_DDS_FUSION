#include "GlobalTracksListener.h"

const double GlobalTracksListener::deltaLat = 2.5/(60.0*1852.216);

GlobalTracksListener::GlobalTracksListener(TracksSourcesDataListener* data)
{
	m_thread = NULL;
	isThreadRunning = false;

	m_SourcesData = data;

	fusor = new TrackFusion(&tracksMap);
	fusor->Start();

	DdsArchLibrary::DdsArchLib::Instance()->getArchTrackSubject()->Attach(this);
}

GlobalTracksListener::~GlobalTracksListener()
{
	typedef boost::unordered::unordered_map< TRACK_ID, ArchFilteredTrackSubject* > subjects_unordered_map;
	typedef boost::unordered::unordered_map< TRACK_ID, FilteredTrackListener* > listeners_unordered_map;

	for(subjects_unordered_map::iterator it = filteredSubjectMap.begin(); it != filteredSubjectMap.end(); ++it)
	{
		delete it->second;
	}

	for(listeners_unordered_map::iterator it = filteredListenerMap.begin(); it != filteredListenerMap.end(); ++it)
	{
		it->second->WaitForFinish();
		delete it->second;
	}

	if(m_thread!=NULL) 
	{
		WaitForFinish();
		delete m_thread;
	}

	delete fusor;
}

void GlobalTracksListener::Start()
{
	isThreadRunning = true;
	m_thread = new boost::thread(boost::ref(*this));
}

void GlobalTracksListener::WaitForFinish()
{
	isThreadRunning = false;
	if(m_thread!=NULL) 
		m_thread->join();
}

void GlobalTracksListener::operator()()
{
	typedef boost::unordered::unordered_map< TRACK_ID, std::pair<AD_TRACK,track_state> > unordered_map;

	while(isThreadRunning)
	{
		m_TracksMapMutex.lock();
		std::cout << std::endl << "Tracks update" << std::endl << std::endl;

		for(unordered_map::iterator it = tracksMap.begin(); it != tracksMap.end(); ++it)
		{
 			if(it->second.second == NEW_TRACK)
			{
				AD_TRACK* tr = &(it->second.first); 
				std::string id = it->first;

				//Store the filtered track listener
				FilteredTrackListener* listener = new FilteredTrackListener(fusor, tr);
				filteredListenerMap.insert(std::make_pair(id,listener));
				listener->Start();
				
				//Store the filtered subject
				ArchFilteredTrackSubject* fTrackSubject = new ArchFilteredTrackSubject(tr,m_SourcesData,listener);
				filteredSubjectMap.insert(std::make_pair(id,fTrackSubject));

				it->second.second = ANALYZED_TRACK;
			}
			//TODO: manca gestione update traccia con update del filtro
			else if(it->second.second == ANALYZED_TRACK)
			{
				AD_TRACK* tr = &(it->second.first); 
				std::string id = it->first;
				
				ArchFilteredTrackSubject* subject = filteredSubjectMap[id];

				AD_TRACK* ref = subject->getReferenceTrack();

				float temp = cos(DEGRAD*ref->TargetPosition.x_lat_range);
				
				if(fabs(tr->TargetPosition.x_lat_range - ref->TargetPosition.x_lat_range) > deltaLat ||
					((temp > 0.0001) && (fabs(tr->TargetPosition.y_lon_azimut - ref->TargetPosition.y_lon_azimut) > (deltaLat / temp))) )
				{
					subject->UpdateTrackFilter(tr);
				}
			}
			else if(it->second.second == DELETE_TRACK)
			{
				
			}
		}

		m_TracksMapMutex.unlock();
		
		boost::this_thread::sleep(boost::posix_time::seconds(3));	
	}
}

void GlobalTracksListener::Update(DdsArchLibrary::ISubject* sub)
{
	if (sub == DdsArchLibrary::DdsArchLib::Instance()->getArchTrackSubject())
	{
		AD_TRACK data;

		DdsArchLibrary::ValidData result = ((DdsArchLibrary::ArchTrackSubject*)sub)->getData(data);

		std::string trackID = calculate_track_ID(data);

		if( result == DdsArchLibrary::newData)
		{
			bool absolute = true;

			//if the received track is not in the ABSOLUTE coordinates we try to perform the conversion
			if(data.CoordinatesTypePosition != ABSOLUTE_2D && data.CoordinatesTypePosition != ABSOLUTE_3D)
			{
				//and the sensor location is needed, so we search inside the Sources Data structure
				absolute = TrackConversions::PolarToAbsolute(data,m_SourcesData);
			}

			//if the conversion was needed, but it wasn't succesful the track is ignored
			//otherwise is analyzed and stored in the tracks map
			if(absolute)
			{
				m_TracksMapMutex.lock();

				if(tracksMap.find(trackID) == tracksMap.end())		//track is not contained yet
				{
					std::pair<AD_TRACK,track_state> p(data,NEW_TRACK);
					tracksMap.insert(std::make_pair(trackID,p));
				}
				else												//update of an already contained track
				{
					AD_TRACK_copy(&tracksMap[trackID].first, &data);
				}
				
				m_TracksMapMutex.unlock();
			}

		}
		else if( result == DdsArchLibrary::deleteData)			//e' un dato corretto non ancora gettato
		{
			m_TracksMapMutex.lock();

			tracksMap[trackID].second = DELETE_TRACK;

			m_TracksMapMutex.unlock();
		}

	}
}