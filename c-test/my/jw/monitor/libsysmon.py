###############################################################################
# Author : liyunteng
# Email : li_yunteng@163.com
# Created Time : 2014-02-14 09:55
# Filename : libsysmon.py
# Description : 
###############################################################################
#!/usr/bin/env python
# -*- coding: utf-8 -*-

import time
import socket
import json
import string

SYSMON_ADDR = "/root/soket/unix/test.domain"

class ev:
	def __init__(self):
		self.module = ''
		self.event = ''
		self.param = ''
		self.msg = ''


def sysmon_event(module, event, param, msg):
	_ev = ev()
	_ev.module = module
	_ev.event = event
	_ev.param = param
	_ev.msg = msg

	ev_msg = json.dumps(_ev.__dict__)
	try:
		client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
		client.connect(SYSMON_ADDR)
		client.send(ev_msg)
		client.close()
	except:
		return False

	return True

if  __name__ == '__main__':
	i = '0'
	while sysmon_event('disk', 'online', i, 'disk online') :
		time.sleep(1)
		i = str(string.atoi(i) + 1)
