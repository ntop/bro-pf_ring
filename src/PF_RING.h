// See the file "COPYING" in the main distribution directory for copyright.

#ifndef IOSOURCE_PKTSRC_PF_RING_SOURCE_H
#define IOSOURCE_PKTSRC_PF_RING_SOURCE_H

extern "C" {
#include <stdio.h>
#include <pfring.h>
}

#include "iosource/PktSrc.h"

namespace iosource {
namespace pktsrc {

class PF_RINGSource : public iosource::PktSrc {
public:
	/**
	 * Constructor.
	 *
	 * path: Name of the interface to open.
	 *
	 * is_live: Must be true (the pf_ring source doesn't support offline operation).
	 */
	PF_RINGSource(const std::string& path, bool is_live, const std::string& kind);

	/**
	 * Destructor.
	 */
	virtual ~PF_RINGSource();

	static PktSrc* InstantiatePF_RING(const std::string& path, bool is_live);

protected:
	// PktSrc interface.
	virtual void Open();
	virtual void Close();
	virtual bool ExtractNextPacket(Packet* pkt);
	virtual void DoneWithPacket();
	virtual bool PrecompileFilter(int index, const std::string& filter);
	virtual bool SetFilter(int index);
	virtual void Statistics(Stats* stats);

private:
	Properties props;
	std::string kind;

	int current_filter;
	unsigned int num_discarded;

	pfring *pd;

	struct pcap_pkthdr current_hdr;
};

}
}

#endif
