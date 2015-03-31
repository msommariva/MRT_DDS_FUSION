#include <iostream>

#include "DdsArchLib.h"

#include "GlobalTracksListener.h"
#include "TracksSourcesDataListener.h"
#include "TrackFusion.h"

int main()
{
	DdsArchLibrary::DdsArchLib* lib = DdsArchLibrary::DdsArchLib::Instance();

	TracksSourcesDataListener* tsdl = new TracksSourcesDataListener();
	GlobalTracksListener* gtl = new GlobalTracksListener(tsdl);



 	gtl->Start();

	//gtl.WaitForFinish();

	system("pause");

	delete gtl;
	delete tsdl;
	delete lib;
	


 	return 0;
}