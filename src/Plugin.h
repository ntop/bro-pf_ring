#ifndef BRO_PLUGIN_BRO_PF_RING
#define BRO_PLUGIN_BRO_PF_RING

#include <plugin/Plugin.h>

namespace plugin {
namespace Bro_PF_RING {

class Plugin : public ::plugin::Plugin
{
protected:
	// Overridden from plugin::Plugin.
	virtual plugin::Configuration Configure();
};

extern Plugin plugin;

}
}

#endif
