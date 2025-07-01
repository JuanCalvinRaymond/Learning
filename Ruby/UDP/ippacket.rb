class IPPacket
  attr_reader :bytes

  def initialize(bytes)
    @bytes = bytes
  end

  def version
    bytes[0] >> 4
  end

  def ihl
    bytes[0] & 0xF
  end

  def dscp
    (bytes[1] & 0xFC) >> 2
  end

  def ecn
    bytes[1] & 0x3
  end

  def total_length
    word16(bytes[2], bytes[3])
  end
  
  def identification
    word16(bytes[4], bytes[5])
  end

  def flags
    bytes[6] >> 5
  end

  def fragment_offset
    word16((bytes[6] & 0x1F), bytes[7])
  end

   def time_to_live
    bytes[8]
  end

  def protocol
    bytes[9]
  end

  def header_checksum
    word16(bytes[10], bytes[11])
  end

  def source_ip_address
    bytes[12, 4].join('.')
  end

  def destination_ip_address
    bytes[16, 4].join('.')
  end

  def udp_datagram
    UDPDatagram.new(bytes.drop(header_bytes))
  end
  
  def header_bytes
    (ihl * 32) / 8
  end

  def word16(a, b)
    (a << 8) | b
  end
end