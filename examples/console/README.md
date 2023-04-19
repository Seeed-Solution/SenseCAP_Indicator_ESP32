# console

It's a console program, nothing is displayed.

## How to use example

The command list is as follows:

```
version 
  Get version of chip and SDK

restart 
  Software reset of the chip

log_level  <tag|*> <none|error|warn|debug|verbose>
  Set log level for all tags or a specific tag.
       <tag|*>  Log tag to set the level for, or * to set for all tags
  <none|error|warn|debug|verbose>  Log level to set. Abbreviated words are accepted.

restart_rp2040 
  Reset of the rp2040 chip

read_mac 
  Read mac address

read_bmp3xx 
  Read bmp3xx sensor data

lora_tx  [-f <f>] [-s <6~12>] [-b <0|1|2>] [-c <1|2|3|4>] [-p ] [--crc=<0|1>] [--iq=<0|1>] [--net=<0|1>] [-i <t>] [-d <d>] [-n <n>]
  lora tx data
  -f, --freq=<f>  Set the radio frequency in Hz, range: 415000000 ~ 940000000 Hz, default: 868000000
  -s, --sf=<6~12>  Set Lora SF, range: 6 ~ 12, default: 7
  -b, --bw=<0|1|2>  Set Lora Bandwidth, 0:125KHz  1:250KHz  2:500KHz, default: 0
  -c, --cr=<1|2|3|4>  Set Lora CodingRate, 1:CR_4_5  2:CR_4_6  3:CR_4_7  4:CR_4_8 , default: 1
  -p, --power=  Set the radio power, LPA range: -17 ~ +14 dB, HPA range: -9 ~ +22 dB, default: 10
   --crc=<0|1>  Set Lora CRC, 0:DISABLE  1:ENABLE, default: 1
    --iq=<0|1>  Set Lora IQ mode, 0:STANDARD  1:INVERTED, default: 0
   --net=<0|1>  Set Public Network, 0: Private Network, 1: Public Network, default: 0
  -i, --interval=<t>  Set the tx interval ms, default: 0
  -d, --txt=<d>  Set the text data to send, default: hello
  -n, --num=<n>  Set Number of packets sent, 0: Keep sending, default: 1

lora_rx  [-f <f>] [-s <6~12>] [-b <0|1|2>] [-c <1|2|3|4>] [--crc=<0|1>] [--iq=<0|1>] [--net=<0|1>] [--boosted=<0|1>]
  lora rx data
  -f, --freq=<f>  Set the radio frequency in Hz, range: 415000000 ~ 940000000 Hz, default: 868000000
  -s, --sf=<6~12>  Set Lora SF, range: 6 ~ 12, default: 7
  -b, --bw=<0|1|2>  Set Lora Bandwidth, 0:125KHz  1:250KHz  2:500KHz, default: 0
  -c, --cr=<1|2|3|4>  Set Lora CodingRate, 1:CR_4_5  2:CR_4_6  3:CR_4_7  4:CR_4_8 , default: 1
   --crc=<0|1>  Set Lora CRC, 0:DISABLE  1:ENABLE, default: 1
    --iq=<0|1>  Set Lora IQ mode, 0:STANDARD  1:INVERTED, default: 0
   --net=<0|1>  Set Public Network, 0: Private Network, 1: Public Network, default: 0
  --boosted=<0|1>  1: Boosted RX, 0: Normal RX, default: 0

lora_cw  [-f <f>] [-p ]
  Tx Continuous Wave
  -f, --freq=<f>  Set the radio frequency in Hz, range: 415000000 ~ 940000000 Hz, default: 868000000
  -p, --power=  Set the radio power, LPA range: -17 ~ +14 dB, HPA range: -9 ~ +22 dB, default: 10

lora_fcc_fhss  [-m <0|1>] [-s <6~12>] [-c <1|2|3|4>] [-p ] [--crc=<0|1>] [--iq=<0|1>] [--net=<0|1>] [-i <t>] [-d <d>]
  lora fcc fhss tx test
  -m, --mode=<0|1>  Set the fhss mode, 0: FHSS_125K_MODE, 1: FHSS_500K_MODE, default: 0
  -s, --sf=<6~12>  Set Lora SF, range: 6 ~ 12, default: 10
  -c, --cr=<1|2|3|4>  Set Lora CodingRate, 1:CR_4_5  2:CR_4_6  3:CR_4_7  4:CR_4_8 , default: 1
  -p, --power=  Set the radio power, LPA range: -17 ~ +14 dB, HPA range: -9 ~ +22 dB, default: 14
   --crc=<0|1>  Set Lora CRC, 0:DISABLE  1:ENABLE, default: 1
    --iq=<0|1>  Set Lora IQ mode, 0:STANDARD  1:INVERTED, default: 0
   --net=<0|1>  Set Public Network, 0: Private Network, 1: Public Network, default: 0
  -i, --interval=<t>  Set the tx interval ms, default: 0
  -d, --txt=<d>  Set the text data to send, default: hello seeed! 1234567

sta  <ssid> [<pass>]
  WiFi is station mode, join specified soft-AP
        <ssid>  SSID of AP
        <pass>  password of AP

scan  [<ssid>]
  WiFi is station mode, start scan ap
        <ssid>  SSID of AP want to be scanned

ap  <ssid> [<pass>]
  AP mode, configure ssid and password
        <ssid>  SSID of AP
        <pass>  password of AP

query 
  query WiFi info

iperf  [-suVa] [-c <ip>] [-p <port>] [-l <length>] [-i <interval>] [-t <time>] [-b <bandwidth>]
  iperf command
  -c, --client=<ip>  run in client mode, connecting to <host>
  -s, --server  run in server mode
     -u, --udp  use UDP rather than TCP
  -V, --ipv6_domain  use IPV6 address rather than IPV4
  -p, --port=<port>  server port to listen on/connect to
  -l, --len=<length>  Set read/write buffer size
  -i, --interval=<interval>  seconds between periodic bandwidth reports
  -t, --time=<time>  time in seconds to transmit for (default 10 secs)
  -b, --bandwidth=<bandwidth>  bandwidth to send at in Mbits/sec
   -a, --abort  abort running iperf

ping  [-W <t>] [-i <t>] [-s <n>] [-c <n>] [-Q <n>] [-T <n>] <host>
  send ICMP ECHO_REQUEST to network hosts
  -W, --timeout=<t>  Time to wait for a response, in seconds
  -i, --interval=<t>  Wait interval seconds between sending each packet
  -s, --size=<n>  Specify the number of data bytes to be sent
  -c, --count=<n>  Stop after sending count packets
  -Q, --tos=<n>  Set Type of Service related bits in IP datagrams
  -T, --ttl=<n>  Set Time to Live related bits in IP datagrams
        <host>  Host address

help 
  Print the list of registered commands

```


### Build and Flash

Run `idf.py -p PORT build flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.
