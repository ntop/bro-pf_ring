#include "bro-config.h"
#include "stdlib.h"

#include "PF_RING.h"

cluster_type get_cluster_type()
	{
	if (getenv("PCAP_PF_RING_USE_CLUSTER_PER_FLOW"))
		return cluster_per_flow;
	else if (getenv("PCAP_PF_RING_USE_CLUSTER_PER_FLOW_2_TUPLE"))
		return cluster_per_flow_2_tuple;
	else if (getenv("PCAP_PF_RING_USE_CLUSTER_PER_FLOW_4_TUPLE"))
		return cluster_per_flow_4_tuple;
	else if (getenv("PCAP_PF_RING_USE_CLUSTER_PER_FLOW_TCP_5_TUPLE"))
		return cluster_per_flow_tcp_5_tuple;
	else if (getenv("PCAP_PF_RING_USE_CLUSTER_PER_FLOW_5_TUPLE"))
		return cluster_per_flow_5_tuple;
	else
		return cluster_per_flow_4_tuple; //Round robin never makes sense for bro
	}

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
	char *app_name = (char *) "bro";

	flags |= PF_RING_PROMISC;
	flags |= PF_RING_TIMESTAMP; /* forcing timestamping (sw if hw ts is not available) */
	flags |= PF_RING_DNA_SYMMETRIC_RSS;

	pd = pfring_open(iface.c_str(), 9056, flags);

	if ( ! pd )
		{
		Error(errno ? strerror(errno) : "invalid interface");
		return;
		}

	if ( pfring_set_application_name(pd, app_name) != 0 )
		{
		Error(errno ? strerror(errno) : "unable to set name");
		pfring_close(pd);
		pd = NULL;
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

	char *cluster_id = getenv("PCAP_PF_RING_CLUSTER_ID");
	if ( cluster_id )
		{
		cluster_type cluster_mode = get_cluster_type();
		if ( pfring_set_cluster(pd, atoi(cluster_id), cluster_mode) )
			{
			Error(errno ? strerror(errno) : "unable to set cluster");
			pfring_close(pd);
			pd = NULL;
			return;
			}
		}

	props.netmask = NETMASK_UNKNOWN;
	props.selectable_fd = pfring_get_selectable_fd(pd);
	props.is_live = true;
	props.link_type = DLT_EN10MB;

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

		if ( ! ApplyBPFFilter(current_filter, &current_hdr, data) ) 
			{
			++num_discarded;
			continue;
			}
		else
			{
			pkt->Init(props.link_type, &current_hdr.ts, current_hdr.caplen, current_hdr.len, data);
			return true;
			}
		}

	return false;
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

