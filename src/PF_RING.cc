#include "bro-config.h"

#include "PF_RING.h"
#include "pf_ring.bif.h"

using namespace iosource::pktsrc;

PF_RINGSource::~PF_RINGSource()
	{
	Close();
	}

PF_RINGSource::PF_RINGSource(const std::string& path, bool is_live, const std::string& arg_kind)
	{
	if ( ! is_live )
		Error("pf_ring source does not support offline input");

	kind = arg_kind;
	current_filter = -1;
	props.path = path;
	props.is_live = is_live;
	}

void PF_RINGSource::Open()
	{
	int flags = 0;
	std::string iface = props.path;

	flags |= PF_RING_PROMISC;
	flags |= PF_RING_TIMESTAMP; /* forcing timestamping (sw if hw ts is not available) */
	flags |= PF_RING_DNA_SYMMETRIC_RSS;

	pd = pfring_open(iface.c_str(), 9056, flags);

	if ( ! pd )
		{
		Error(errno ? strerror(errno) : "invalid interface");
		return;
		}

	if ( pfring_set_socket_mode(pd, recv_only_mode) != 0 )
		{
		Error(errno ? strerror(errno) : "unable to set socket mode");
		pfring_close(pd);
		pd = NULL;
		return;
		}

	if ( pfring_enable_ring(pd) != 0 )
		{
		Error(errno ? strerror(errno) : "unable to enable socket");
		pfring_close(pd);
		pd = NULL;
		return;
		}

	props.netmask = 0xffffff00;
	props.selectable_fd = pfring_get_selectable_fd(pd);
	props.is_live = true;
	props.link_type = DLT_EN10MB;
	props.hdr_size = GetLinkHeaderSize(props.link_type);

	num_discarded = 0;

	Opened(props);
	}

void PF_RINGSource::Close()
	{
	if ( ! pd )
		return;

	pfring_close(pd);
	pd = NULL;

	Closed();
	}

bool PF_RINGSource::ExtractNextPacket(Packet* pkt)
	{
	struct pfring_pkthdr ph;
	u_char *data;
	int rc;

	memset(&ph, 0, sizeof(ph));

	if ( ! pd )
		return false;

	while ( true )
		{
		if ( (rc = pfring_recv(pd, &data, 0 /* len = 0 (zero-copy) */, &ph, 0 /* !wait XXX what about wait with breakloop? */ )) <= 0 )
			return false;

		current_hdr.ts = ph.ts;
		current_hdr.caplen = ph.caplen;
		current_hdr.len = ph.len;

		pkt->ts = current_hdr.ts.tv_sec + double(current_hdr.ts.tv_usec) / 1e6;
		pkt->hdr = &current_hdr;
		pkt->data = data;

		if ( ApplyBPFFilter(current_filter, &current_hdr, data) )
			break;

		++num_discarded;
		}

	return true;
	}

void PF_RINGSource::DoneWithPacket()
	{
	}

bool PF_RINGSource::PrecompileFilter(int index, const std::string& filter)
	{
	return PktSrc::PrecompileBPFFilter(index, filter);
	}

bool PF_RINGSource::SetFilter(int index)
	{
	current_filter = index;
	return true;
	}

void PF_RINGSource::Statistics(Stats* s)
	{
	pfring_stat ps;

	if ( ! pd || pfring_stats(pd, &ps) != 0)
		{
		s->received = s->link = s->dropped = 0;
		return;
		}

	s->received = ps.recv - num_discarded;
	s->link = ps.recv;
	s->dropped = ps.drop;
	}

iosource::PktSrc* PF_RINGSource::InstantiatePF_RING(const std::string& path, bool is_live)
	{
	return new PF_RINGSource(path, is_live, "pf_ring");
	}

