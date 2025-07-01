require 'hexdump'
require 'socket'
# load "ethernetFrame.rb"
require_relative 'ethernetFrame'
require_relative 'ippacket'
require_relative 'udpdatagram'

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
INTERFACE_NAME = 'eth0'

IFREQ_SIZE = 0x0028

IFINDEX_SIZE = 0x0004

SIOCGIFINDEX = 0x8933

socket = Socket.open(:PACKET, :RAW)

ifreq = [INTERFACE_NAME].pack("a#{IFREQ_SIZE}")

socket.ioctl(SIOCGIFINDEX, ifreq)

index = ifreq[Socket::IFNAMSIZ, IFINDEX_SIZE]

ETH_P_ALL = 0x0300

SOCKADDR_LL_SIZE = 0x0014

sockaddr_ll = [Socket::AF_PACKET].pack('s')
sockaddr_ll << [ETH_P_ALL].pack('s')
sockaddr_ll << index
sockaddr_ll << ("\x00" * (SOCKADDR_LL_SIZE - sockaddr_ll.length))

socket.bind(sockaddr_ll)
# socket = UDPSocket.new
# socket.bind('172.27.91.114', 20)

BUFFER_SIZE = 1024

UDP_PROTOCOL = 0x11

loop do
  # data = socket.recv(BUFFER_SIZE)
  data = socket.recv(BUFFER_SIZE)
  Hexdump.dump(data)
  # frame = EthernetFrame.new(data)
  
  
  # message, sender = socket.recvfrom(BUFFER_SIZE)
  # port = sender[1]
  # host = sender[2]

  # socket.send(message.upcase, 0 , host, port)
  # puts "-------"
  # next unless frame.ip_packet.protocol == UDP_PROTOCOL && frame.ip_packet.udp_datagram.destination_port == 20

  # puts "Ethernet:"
  # puts "  Source MAC: #{frame.source_mac}"
  # puts "  Destination MAC: #{frame.destination_mac}"
  # puts
  # puts "IP:"
  # puts "  Version: #{frame.ip_packet.version}"
  # puts "  IHL: #{frame.ip_packet.ihl}"
  # puts "  DSCP: #{frame.ip_packet.dscp}"
  # puts "  ECN: #{frame.ip_packet.ecn}"
  # puts "  Total Length: #{frame.ip_packet.total_length}"
  # puts "  Identification: #{frame.ip_packet.identification}"
  # puts "  Flags: #{frame.ip_packet.flags}"
  # puts "  Fragment Offset: #{frame.ip_packet.fragment_offset}"
  # puts "  Time To Live: #{frame.ip_packet.time_to_live}"
  # puts "  Protocol: #{frame.ip_packet.protocol}"
  # puts "  Header Checksum: #{frame.ip_packet.header_checksum}"
  # puts "  Source IP Address: #{frame.ip_packet.source_ip_address}"
  # puts "  Destination IP Address: #{frame.ip_packet.destination_ip_address}"
  # puts
  # puts "UDP:"
  # puts "  Source Port: #{frame.ip_packet.udp_datagram.source_port}"
  # puts "  Destination Port: #{frame.ip_packet.udp_datagram.destination_port}"
  # puts "  Length: #{frame.ip_packet.udp_datagram.length}"
  # puts "  Checksum: #{frame.ip_packet.udp_datagram.checksum}"
  # puts "  Body: #{frame.ip_packet.udp_datagram.body}"
  # puts

  # UDPSocket.new.send("Hello",
  # 0,
  # "172.27.91.114",
  # frame.ip_packet.udp_datagram.source_port)
  
end