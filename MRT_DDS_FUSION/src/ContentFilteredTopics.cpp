#include "ContentFilteredTopics.h"

ArchFilteredTrackSubject::ArchFilteredTrackSubject(AD_TRACK* traccia, TracksSourcesDataListener* data, FilteredTrackListener* list)
{
	DDS_ReturnCode_t		retcode;
	DDS_StringSeq			parameters(2);
	DDSDomainParticipant*	participant;

	cft			= NULL;
	data_reader = NULL;
	listener	= list;
	dataSource	= data;

	refTrack	= traccia;

	participant = DdsArchLibrary::DdsArchLib::Instance()->getPartecipant();

	TRACK_ID id = calculate_track_ID(*traccia);

	// Create and register custom filter
	std::string FilterName = "TracksFilter_"+id;
	std::string TopicName = FilterName+"_topic";

	tracks_filter_type* custom_filter = new tracks_filter_type(data);
	retcode = participant->register_contentfilter(FilterName.c_str(),custom_filter);

	if (retcode != DDS_RETCODE_OK) 
	{
		std::cout << "Failed to register custom content filter" << std::endl;
		return;
	}

	std::string x = boost::lexical_cast<std::string>(traccia->TargetPosition.x_lat_range); 
	std::string y = boost::lexical_cast<std::string>(traccia->TargetPosition.y_lon_azimut);

	const char* param_list[] = { x.c_str(), y.c_str() };
	parameters.from_array(param_list, 2);

	// Create content filtered topic 
	
	cft = participant->create_contentfilteredtopic_with_filter(TopicName.c_str(), 
		DdsArchLibrary::DdsArchLib::Instance()->getArchTrackSubject()->getTopic(), 
		"%0 %1", parameters, FilterName.c_str());

	if (cft == NULL) 
	{
		std::cout << "create_contentfilteredtopic error" << std::endl;
		return;
	}

	data_reader = participant->create_datareader(
		cft,
		DDS_DATAREADER_QOS_DEFAULT,    /* QoS */
		listener,                      /* Listener */
		DDS_DATA_AVAILABLE_STATUS);

	if (data_reader == NULL) 
	{
		std::cout << "Unable to create data reader." << std::endl;
		return;
	}
}

ArchFilteredTrackSubject::~ArchFilteredTrackSubject()
{
	DDSDomainParticipant*	participant;
	participant = DdsArchLibrary::DdsArchLib::Instance()->getPartecipant();

	if (data_reader != NULL) participant->delete_datareader(data_reader);
	if (cft != NULL) participant->delete_contentfilteredtopic(cft);
	
}

void ArchFilteredTrackSubject::UpdateTrackFilter(AD_TRACK* newTrack)
{
	refTrack = newTrack;

	DDS_StringSeq oldParameters;
	cft->get_expression_parameters(oldParameters);

	DDS_String_free(oldParameters[0]);
	DDS_String_free(oldParameters[1]);

	std::string x = boost::lexical_cast<std::string>(newTrack->TargetPosition.x_lat_range); 
	std::string y = boost::lexical_cast<std::string>(newTrack->TargetPosition.y_lon_azimut);

	oldParameters[0] = DDS_String_dup(x.c_str());
	oldParameters[1] = DDS_String_dup(y.c_str());
	
	int retcode = cft->set_expression_parameters(oldParameters);
	if(retcode != DDS_RETCODE_OK)
	{
		std::cout << "Unable to change filters" << std::endl;
		return;
	}
}

AD_TRACK* ArchFilteredTrackSubject::getReferenceTrack() const
{
	return refTrack;
}

FilteredTrackListener::FilteredTrackListener(TrackFusion* tf, AD_TRACK* track)
{
	refTrack = track;
	isThreadRunning = false;
	m_thread = NULL;

	fusor = tf;
}

void FilteredTrackListener::Start()
{
	isThreadRunning = true;
	m_thread = new boost::thread(boost::ref(*this));
}

void FilteredTrackListener::WaitForFinish()
{
	isThreadRunning = false;
	if(m_thread!=NULL) 
		m_thread->join();
}

void FilteredTrackListener::operator()()
{
	typedef boost::unordered::unordered_map< TRACK_ID, boost::posix_time::ptime > unordered_map;

	while(isThreadRunning)
	{
		std::string print;
		print = calculate_track_ID(*refTrack) + " has generated set: ";

		m_SetMutex.lock();

		for(CorrelatedTracksSetIter = CorrelatedTracksSet.begin(); CorrelatedTracksSetIter != CorrelatedTracksSet.end(); ++CorrelatedTracksSetIter)
		{
			print += *CorrelatedTracksSetIter;
			print += " ";
 		}

		if(CorrelatedTracksSet.size())
			fusor->addCorrelatedTracks(CorrelatedTracksSet);

		//check every track validity based on the last received update timestamp
		boost::posix_time::ptime timeNow = boost::posix_time::second_clock::local_time();

		for(unordered_map::iterator it = TracksTimeStamp.begin(); it != TracksTimeStamp.end(); ++it)
		{
			boost::posix_time::time_duration lastUpdate = timeNow - it->second;
			
			if(lastUpdate.seconds() > maxNoUpdateTimeSeconds)
				CorrelatedTracksSet.erase(it->first);
		}
		
		m_SetMutex.unlock();

		std::cout << print << std::endl;

		boost::this_thread::sleep(boost::posix_time::seconds(3));	
	}
}

void FilteredTrackListener::on_data_available(DDSDataReader *reader)
{
	AD_TRACKDataReader *ArchTrack_reader = NULL;
	AD_TRACKSeq data_seq;
	DDS_SampleInfoSeq info_seq;
	DDS_ReturnCode_t retcode;
	int i;

	ArchTrack_reader = AD_TRACKDataReader::narrow(reader);
	if (ArchTrack_reader == NULL) {
		printf("DataReader narrow error\n");
		return;
	}

	retcode = ArchTrack_reader->take(data_seq, info_seq);


	if (retcode == DDS_RETCODE_NO_DATA) 
	{
		return;
	} 
	else if (retcode != DDS_RETCODE_OK) 
	{
		printf("take error %d\n", retcode);
		return;
	}

	for (i = 0; i < data_seq.length(); ++i) 
	{
		//controllare qui info_seq[i].instance_state per le cancellazioni
		if (info_seq[i].valid_data)
		{
			//if((refTrack->SID != data_seq[i].SID) || (refTrack->TID != data_seq[i].TID))
			{
				//std::cout << "Timestamp " << refTrack->TimeStamp.sec << " REF " << refTrack->SID << " " << refTrack->TID << " is near to " << data_seq[i].SID << " " << data_seq[i].TID << " "; 

				TRACK_ID id = calculate_track_ID(data_seq[i]);

				m_SetMutex.lock();
				
				CorrelatedTracksSet.insert(id);
				TracksTimeStamp[id] = boost::posix_time::second_clock::local_time();

				m_SetMutex.unlock();
			}

		}	
	}

	retcode = ArchTrack_reader->return_loan(data_seq, info_seq);

}

