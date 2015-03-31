#ifndef _TRACKFILTER_H_
#define _TRACKFILTER_H_

#include <math.h>
#include "defines.h"
#include "DdsArchLib.h" 
#include "TracksSourcesDataListener.h"
#include "TrackConversions.h"

/* Custom compile data */
struct cdata 
{
    float ref_x_val;
	float ref_y_val;
	
    bool (*eval_func)(float, float, float, float);
};

bool track_pos_test(float, float, float, float);

class tracks_filter_type : public DDSContentFilter 
{
private:
	TracksSourcesDataListener* DataSource;

public:

	tracks_filter_type(TracksSourcesDataListener*);

    virtual DDS_ReturnCode_t compile(void** new_compile_data,
            const char *expression, const DDS_StringSeq& parameters,
            const DDS_TypeCode* type_code, const char* type_class_name,
            void *old_compile_data);

    virtual DDS_Boolean evaluate(void* compile_data, const void* sample);

    virtual void finalize(void* compile_data);
};

#endif