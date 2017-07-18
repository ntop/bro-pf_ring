
Bro::PF_RING
============

This plugin provides native `PF_RING <http://www.ntop.org>` support for Bro.

Bro-pkg Installation
--------------------

Make sure you have the PF_RING library installed and then run:

    bro-pkg install ntop/bro-pf_ring

Manual Installation
-------------------

Follow PF_RING's instructions to get its kernel module, drivers and userspace libraries installed,
then use the following commands to configure and build the plugin.

After building bro from the sources, change to the "bro-pf_ring" directory and run:

    ./configure --with-pfring=<PF_RING base directory> --bro-dist=<path to bro sources>
    make && sudo make install

If everything built and installed correctly, you should see this:

    bro -N Bro::PF_RING
    Bro::PF_RING - Packet acquisition via PF_RING

Usage
-----

Once installed, you can use PF_RING interfaces/ports by prefixing them
with ``pf_ring::`` on the command line. For example, to use PF_RING to
monitor interface ``eth1``:

    bro -i pf_ring::eth1

To use it in production with multiple Bro processes, use a configuration 
similar to this in node.cfg (e.g. /usr/local/bro/etc/node.cfg):

    [worker-1]
    type=worker
    host=localhost
    interface=pf_ring::zc:eth1
    lb_method=pf_ring
    lb_procs=8
    pin_cpus=0,1,2,3,4,5,6,7

Where lb_procs is the number of processes for load balancing (e.g. RSS)
Now start the BroControl shell like:

    broctl

The first-time, perform an initial installation of the BroControl configuration:

    [BroControl] > install

Then start the Bro instances:

    [BroControl] > start

