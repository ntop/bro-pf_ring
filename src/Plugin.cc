#include "Plugin.h"
#include "PF_RING.h"

namespace plugin { namespace Bro_PF_RING { Plugin plugin; } }

using namespace plugin::Bro_PF_RING;

plugin::Configuration Plugin::Configure()
	{
	AddComponent(new ::iosource::PktSrcComponent("PF_RINGReader", "pf_ring", ::iosource::PktSrcComponent::LIVE, ::iosource::pktsrc::PF_RINGSource::InstantiatePF_RING));

	plugin::Configuration config;
	config.name = "Bro::PF_RING";
	config.description = "Packet acquisition via PF_RING";
	config.version.major = 1;
	config.version.minor = 0;
	return config;
	}
