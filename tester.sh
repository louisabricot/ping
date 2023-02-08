#!/bin/bash

# Created by louisabricot with <3
# Usage: ./tester.sh [path to ft_ping]

# Test permission 
exec ./ft_ping
echo $UID
exec sudo ./ft_ping
# Test parsing of options


# Test basic commands
# --> test time and compare with ping
# --> test Ctrl-C

# Testcases
# Good Ipv6
# Good Ipv4
# Bad Ipv4
# Good hostname
# Bad hostname

# Test option -h
# Test option -v
# Test option -i
# Test option -t
# Test option -W
# Test option -w
