#include "TrackConversions.h"

bool TrackConversions::PolarToAbsolute(AD_TRACK& track, TracksSourcesDataListener* dataSource)
{
	if(track.CoordinatesTypePosition != POLAR_2D || track.CoordinatesTypePosition != POLAR_3D)
	{
		return false;
	}
	else
	{
		DDS_UnsignedLong SID = track.SID;
		
		AD_PROPERTY prop;

		bool result = dataSource->get_System_PROPERTY(SID,&prop);

		if(false == result)
		{
			return false;
		}
		else
		{
			double sensorLat = DEGRAD*prop.SensorPosition.Latitude;
			double sensorLong = DEGRAD*prop.SensorPosition.Longitude;

			//formulas from www.movable-type.co.uk/scripts/latlong.html
			double trackRange = track.TargetPosition.x_lat_range;
			double trackAzimuth = DEGRAD*track.TargetPosition.y_lon_azimut;

			double trackLat = asin( sin(sensorLat) * cos(trackRange/earth_radius) + cos(sensorLat)*sin(trackRange/earth_radius) * cos(trackAzimuth) );
			double trackLong = sensorLong + atan2( sin(trackAzimuth) * sin(trackRange/earth_radius) * cos(sensorLat), cos(trackRange/earth_radius) - sin(sensorLat)*sin(trackLat) );

			if(track.CoordinatesTypePosition == POLAR_2D)
			{
				track.CoordinatesTypePosition = ABSOLUTE_2D;
			}
			else if (track.CoordinatesTypePosition == POLAR_3D)
			{
				track.CoordinatesTypePosition = ABSOLUTE_3D;
				track.TargetPosition.z_alt_elev = track.TargetPosition.x_lat_range * tan(DEGRAD * track.TargetPosition.z_alt_elev); 
			}

			track.TargetPosition.x_lat_range = (float) RADDEG*trackLat;
			track.TargetPosition.x_lat_range = (float) RADDEG*trackLong;

			return true;
		}

		
	}
}