# Ft_ping

Recoding some features of the ping utility.

# Installation

``` bash
git clone 
cd ping/
make
```

# Usage

This program creates a raw socket and thus must be run with sudo.

```bash
sudo ./ft_ping [options] <destination>
```

# Example

```bash
sudo ./ft_ping -h

Usage
ft_ping [options] <destination>

Options:
<destination>       dns name of ip address
-c <count>          stop after <count> replies
-h                  print help and exit
-i <interval>       seconds between sending each packet
-n                  no dns name resolution
-v                  verbose output
-w <deadline>       reply wait <deadline> in second
-W <timeout>        time to wait for response
```
