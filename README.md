
Bro::PF_RING
============

This plugin provides native `PF_RING <http://www.ntop.org>` support for Bro.

Bro-pkg Installation
--------------------

Make sure you have the PF_RING library installed and then run::

	bro-pkg install xxx/bro-pf_ring

Manual Installation
-------------------

Follow PF_RING's instructions to get its kernel module, drivers and userspace libraries installed,
then use the following commands to configure and build the plugin.

After building bro from the sources, change to the "aux/plugins/pf_ring" directory and run::

        ./configure --with-pfring=<PF_RING base directory> --bro-dist=<path to bro sources>
        make && sudo make install

If everything built and installed correctly, you should see this::

    # bro -N Bro::PF_RING
    Bro::PF_RING - Packet acquisition via PF_RING

You may run Bro as unprivileged user.

Usage
-----

Once installed, you can use PF_RING interfaces/ports by prefixing them
with ``pf_ring::`` on the command line. For example, to use PF_RING to
monitor interface ``eth1``::

    bro -i pf_ring::eth1

To use it in production with multiple Bro processes, use a
configuration similar to this in node.cfg::

    [worker-1]
    type=worker
    host=localhost
    lb_method=custom
    lb_procs=<number of processes, like 16>
    interface=pf_ring::<interface name, like eth1>

