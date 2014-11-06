#!/bin/bash

string="7:3:respawn:/usr/sbin/sshd -D"

if grep -q "$string" /etc/inittab;then
   echo "sshd is in inittab."
else
   echo $string >> /etc/inittab
   echo "add sshd in inittab."
fi
