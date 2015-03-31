#include "TracksSourcesDataListener.h"

TracksSourcesDataListener::TracksSourcesDataListener()
{
	DdsArchLibrary::DdsArchLib::Instance()->getArchInfoSubject()->Attach(this);
	DdsArchLibrary::DdsArchLib::Instance()->getArchPropertySubject()->Attach(this);
	DdsArchLibrary::DdsArchLib::Instance()->getArchStatusSubject()->Attach(this);
}

void TracksSourcesDataListener::Update(DdsArchLibrary::ISubject* sub)
{
	if (sub == DdsArchLibrary::DdsArchLib::Instance()->getArchInfoSubject())
	{
		AD_INFO data;

		DdsArchLibrary::ValidData result = ((DdsArchLibrary::ArchInfoSubject*)sub)->getData(data);

		DDS_UnsignedLong received_SID = data.SID;

		if( result == DdsArchLibrary::newData)
		{
			m_DataMutex.lock();
			
			m_info_map[received_SID] = data;
			
			m_DataMutex.unlock();
		}
		else if( result == DdsArchLibrary::deleteData)			//e' un dato corretto non ancora gettato
		{
			m_DataMutex.lock();


			m_DataMutex.unlock();
		}

	}
	else if (sub == DdsArchLibrary::DdsArchLib::Instance()->getArchPropertySubject())
	{
		AD_PROPERTY data;

		DdsArchLibrary::ValidData result = ((DdsArchLibrary::ArchPropertySubject*)sub)->getData(data);

		DDS_UnsignedLong received_SID = data.SID;

		if( result == DdsArchLibrary::newData)
		{
			m_DataMutex.lock();
			
			m_property_map[received_SID] = data;
			
			m_DataMutex.unlock();
		}
		else if( result == DdsArchLibrary::deleteData)			//e' un dato corretto non ancora gettato
		{
			m_DataMutex.lock();


			m_DataMutex.unlock();
		}

	}
	else if (sub == DdsArchLibrary::DdsArchLib::Instance()->getArchStatusSubject())
	{
		AD_STATUS data;

		DdsArchLibrary::ValidData result = ((DdsArchLibrary::ArchStatusSubject*)sub)->getData(data);

		DDS_UnsignedLong received_SID = data.SID;

		if( result == DdsArchLibrary::newData)
		{
			m_DataMutex.lock();
			
			m_status_map[received_SID] = data;
			
			m_DataMutex.unlock();
		}
		else if( result == DdsArchLibrary::deleteData)			//e' un dato corretto non ancora gettato
		{
			m_DataMutex.lock();


			m_DataMutex.unlock();
		}

	}
}

bool TracksSourcesDataListener::get_System_INFO(const DDS_UnsignedLong SID, AD_INFO* info)
{
	bool found;

	m_DataMutex.lock();
	
	if(m_info_map.find(SID) == m_info_map.end())		//track is not contained yet
	{
		found = false;
	}
	else
	{
		AD_INFO_copy(info,&(m_info_map[SID]));
		found = true;
	}
	
	m_DataMutex.unlock();

	return found;
}

bool TracksSourcesDataListener::get_System_PROPERTY(const DDS_UnsignedLong SID, AD_PROPERTY* prop)
{
	bool found;

	m_DataMutex.lock();
	
	if(m_property_map.find(SID) == m_property_map.end())		//track is not contained yet
	{
		found = false;
	}
	else
	{
		AD_PROPERTY_copy(prop,&(m_property_map[SID]));
		found = true;
	}
	
	m_DataMutex.unlock();

	return found;
}

bool TracksSourcesDataListener::get_System_STATUS(const DDS_UnsignedLong SID, AD_STATUS* status)
{
	bool found;

	m_DataMutex.lock();
	
	if(m_status_map.find(SID) == m_status_map.end())		//track is not contained yet
	{
		found = false;
	}
	else
	{
		AD_STATUS_copy(status,&(m_status_map[SID]));
		found = true;
	}
	
	m_DataMutex.unlock();

	return found;
}