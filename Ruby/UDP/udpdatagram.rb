class UDPDatagram
  attr_reader :bytes
  def initialize(bytes)
    @bytes = bytes
  end

  def source_port
    word16(bytes[0], bytes[1])
  end

  def destination_port
    word16(bytes[2], bytes[3])
  end

  def length
    word16(bytes[4], bytes[5])
  end

  def checksum
    word16(bytes[6], bytes[7])
  end

  def body
    bytes[8, (length - 8)].pack('C*')
  end

  def word16(a, b)
    (a << 8) | b
  end
end

# def udp_datagram:
#   UDPDatagram.new(bytes.drop(20))
# end
