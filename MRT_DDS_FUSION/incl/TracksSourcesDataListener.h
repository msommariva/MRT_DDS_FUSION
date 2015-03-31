#ifndef _TRACKSSOURCESDATA_LISTENER_H_
#define _TRACKSSOURCESDATA_LISTENER_H_

#include "DdsArchLib.h" 

#include <boost/thread.hpp>
#include <boost/unordered_map.hpp>

class TracksSourcesDataListener : public DdsArchLibrary::Observer
{

private:

	boost::unordered::unordered_map< DDS_UnsignedLong, AD_INFO >			m_info_map;
	boost::unordered::unordered_map< DDS_UnsignedLong, AD_PROPERTY >		m_property_map;
	boost::unordered::unordered_map< DDS_UnsignedLong, AD_STATUS >			m_status_map;

	boost::mutex	m_DataMutex;

public:

	TracksSourcesDataListener();
	~TracksSourcesDataListener() {}

	virtual void Update(DdsArchLibrary::ISubject*);

	bool get_System_INFO(const DDS_UnsignedLong SID, AD_INFO* info);
	bool get_System_PROPERTY(const DDS_UnsignedLong SID, AD_PROPERTY* prop);
	bool get_System_STATUS(const DDS_UnsignedLong SID, AD_STATUS* status);

};

#endif