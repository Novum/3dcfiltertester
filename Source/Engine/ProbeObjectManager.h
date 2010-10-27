#pragma once

namespace engine
{
	class ProbeObject;	

	class ProbeObjectManager
	{
	public:
		void Initialize();
		void Deinitalize();
		ProbeObject *GetPlaneProbe() const { return plane_probe; }
		ProbeObject *GetTunnelProbe() const { return tunnel_probe; }
	private:
		ProbeObject *plane_probe;
		ProbeObject *tunnel_probe;
	};

	extern ProbeObjectManager probe_object_manager;
}