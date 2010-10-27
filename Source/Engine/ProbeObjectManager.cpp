#include "ProbeObjectManager.h"
#include "PlaneProbe.h"
#include "TunnelProbe.h"

namespace engine
{
	ProbeObjectManager probe_object_manager;

	void ProbeObjectManager::Initialize()
	{
		plane_probe = new PlaneProbe();
		tunnel_probe = new TunnelProbe();
	}

	void ProbeObjectManager::Deinitalize()
	{
		delete tunnel_probe;
		delete plane_probe;
	}
}