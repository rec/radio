import BeautifulSoup

import Config
import File

def getTableLine(c):
  def get(i):
    return c.contents[i].contents[0].extract()

  return dict(ip=get(1), time=get(3), client=get(5))

def getListeners(data):
  listeners = []
  contents = BeautifulSoup.BeautifulSoup(urlData).find(id='listenerTable')
  for c in contents:
    if hasattr(c, 'contents'):
      listeners.append(getTableLine(c))
    listeners.pop(0)
  return listeners

class ListenerJob(object):
  def __init__(self):
    self.interval = Config.LISTENER_INTERVAL

  def callback(self):
    listeners = []
    try:
      listeners = getListeners(File.readUrl(Config.LISTENER_URL))
    except:
      Config.log('Got an error for ' + Config.LISTENER_URL)
      pass
    File.replaceAtomic(Config.LISTENER_FILE, json.dumps(listeners))


