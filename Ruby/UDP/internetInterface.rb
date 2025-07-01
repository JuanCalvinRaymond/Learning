require 'socket'

# struct ifreq {
#   char ifr_name[IFNAMIZ];
#   union {
#     struct sockaddr ifr_addr;
#     struct sockaddr ifr_dstaddr;
#     struct sockaddr ifr_broadaddr;
#     struct sockaddr ifr_netmask;
#     struct sockaddr ifr_hwaddr;
#     short           ifr_flags;
#     int             ifr_ifindex;
#     int             ifr_metric;
#     int             ifr_mtu;
#     struct ifmap    ifr_map;
#     char            ifr_slave[IFNAMSIZ];
#     char            ifr_newname[IFNAMSIZ];
#     char           *ifr_data;
#   }
# };

IFREQ_SIZE = 0x0028

IFINDEX_SIZE = 0x0004

SIOCGIFINDEX = 0x8933

socket = Socket.open(:PACKET, :RAW)

ifreq = %w[eth0].pack("a#{IFREQ_SIZE}")

socket.ioctl(SIOCGIFINDEX, ifreq)

index = ifreq[Socket::IFNAMSIZ, IFINDEX_SIZE]