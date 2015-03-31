#include "TrackFilter.h"

tracks_filter_type::tracks_filter_type(TracksSourcesDataListener* data)
{
	DataSource = data;
}

/* Called when Custom Filter is created, or when parameters are changed */
DDS_ReturnCode_t tracks_filter_type::compile(void** new_compile_data,
        const char *expression, const DDS_StringSeq& parameters,
        const DDS_TypeCode* type_code, const char* type_class_name,
        void *old_compile_data) 
{
	struct cdata* cd = NULL;

	/* First free old data, if any */
	delete old_compile_data;

	/* We expect an expression of the form "%0 %1 <var>"
	* where %1 = "divides" or "greater-than"
	* and <var> is an integral member of the msg structure.
	* 
	* We don't actually check that <var> has the correct typecode,
	* (or even that it exists!). See example Typecodes to see 
	* how to work with typecodes.
	*
	* The compile information is a structure including the first filter
	* parameter (%0) and a function pointer to evaluate the sample
	*/

	/* Check form: */
	if (strncmp(expression, "%0 %1", 5) != 0) 
	{
		*new_compile_data = NULL;
		return DDS_RETCODE_BAD_PARAMETER;
	}

	/* Check that we have params */
	if (parameters.length() < 2) 
	{
		*new_compile_data = NULL;
		return DDS_RETCODE_BAD_PARAMETER;
	}

	cd = new cdata();

	cd->ref_x_val = static_cast<float>(atof(parameters[0]));
	cd->ref_y_val = static_cast<float>(atof(parameters[1]));

	cd->eval_func = track_pos_test;

	*new_compile_data = cd;
	return DDS_RETCODE_OK;
}

/* Called to evaluated each sample */
DDS_Boolean tracks_filter_type::evaluate(void* compile_data, const void* sample) 
{
    cdata* cd = (cdata*) compile_data;
	AD_TRACK* msg = (AD_TRACK*) sample;

	bool absolute = true;

	//if the received track is not in the ABSOLUTE coordinates we try to perform the conversion
	if(msg->CoordinatesTypePosition != ABSOLUTE_2D && msg->CoordinatesTypePosition != ABSOLUTE_3D)
	{
		//and the sensor location is needed, so we search inside the Sources Data structure
		absolute = TrackConversions::PolarToAbsolute(*msg,DataSource);
	}

	if(!absolute)
		return DDS_BOOLEAN_FALSE;


	if (cd->eval_func(msg->TargetPosition.x_lat_range, msg->TargetPosition.y_lon_azimut, cd->ref_x_val, cd->ref_y_val))
        return DDS_BOOLEAN_TRUE;
    else
		return DDS_BOOLEAN_FALSE;
}

void tracks_filter_type::finalize(void* compile_data) 
{
    if (compile_data != NULL)
        delete compile_data;
}

bool track_pos_test(float sample_x, float sample_y, float ref_x, float ref_y) 
{
    if(fabs(sample_x-ref_x) < 3 && fabs(sample_y-ref_y) < 3) 
		return true;
	else
		return false;
}