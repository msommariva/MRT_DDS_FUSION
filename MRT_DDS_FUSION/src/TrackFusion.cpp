#include "TrackFusion.h" 

TrackFusion::TrackFusion(boost::unordered::unordered_map< TRACK_ID, std::pair<AD_TRACK,track_state> >* tracks)
{
	globalTracksMap = tracks;
	m_thread = NULL;
	isThreadRunning = false;

}

TrackFusion::~TrackFusion()
{
	if(m_thread!=NULL) 
	{
		WaitForFinish();
		delete m_thread;
	}
}

void TrackFusion::Start()
{
	isThreadRunning = true;
	m_thread = new boost::thread(boost::ref(*this));
}
	
void TrackFusion::WaitForFinish()
{
	isThreadRunning = false;
	if(m_thread!=NULL) 
		m_thread->join();
}
	
void TrackFusion::operator()()
{
	trackIter iter;

	while(isThreadRunning)
	{
		m_TracksMapMutex.lock();

		for(TracksMapIter = tracksMap.begin(); TracksMapIter != tracksMap.end(); ++TracksMapIter)
		{
			std::string print;
			int num = 0;
			std::cout << "fused tracks: ";
			for(iter = (trackIter&)(TracksMapIter->first.begin()); iter != (trackIter&)(TracksMapIter->first.end()); ++(trackIter&)iter)
			{
				num++;
				print += *iter;
				print += " ";
 			}

			std::cout <<  num << " " << print << std::endl;
 		}

		m_TracksMapMutex.unlock();

		boost::this_thread::sleep(boost::posix_time::seconds(3));	
	}
}

void TrackFusion::addCorrelatedTracks(std::set<TRACK_ID,compareTrackIDs> set)
{
	m_TracksMapMutex.lock();

	tracksMap[set] = boost::posix_time::second_clock::local_time();

	m_TracksMapMutex.unlock();
}