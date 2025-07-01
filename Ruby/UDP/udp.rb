require 'socket'

BUFFER_SIZE = 1024

socket = UDPSocket.new
socket.bind('172.27.91.114', 4312)

loop do
  message, sender = socket.recvfrom(BUFFER_SIZE)

  port = sender[1]
  host = sender[2]

  socket.send(port.to_s, 0, host, port)
end