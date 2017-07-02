#!/usr/bin/env python
import msgflo
import gevent

class OpenToolLock(msgflo.Participant):
  def __init__(self, role):
    d = {
      'component': 'OpenToolLock',
      'label': 'Repeat input data without change',
      'icon': 'star-o',
      'inports': [
        { 'id': 'in', 'type': 'any' },
      ],
      'outports': [
        { 'id': 'out', 'type': 'boolean' },
      ],
    }
    self.state = False
    msgflo.Participant.__init__(self, d, role)

  def process(self, inport, msg):
    #gevent.Greenlet.spawn(self.worker)
    if msg.data == '1':
      self.state = True
    else:
      self.state = False
    self.send('out', self.state)
    #self.ack(msg)
    
  #unused
  def worker(self):
    while (True):
      self.send('out', True)
      print("true sent")
      gevent.sleep(1)
      self.send('out', False)
      print("false sent")
      gevent.sleep(1)

if __name__ == '__main__':
  msgflo.main(OpenToolLock)
