import urllib2
import BeautifulSoup

import Config
import File

def makeOpener():
  # create a password manager
  password_mgr = urllib2.HTTPPasswordMgrWithDefaultRealm()
  password_mgr.add_password(None, 
                            Config.ADMIN_URL, 
                            Config.ADMIN_USER,
                            Config.ADMIN_PASSWORD)

  handler = urllib2.HTTPBasicAuthHandler(password_mgr)

  # create "opener" (OpenerDirector instance)
  return urllib2.build_opener(handler)

OPENER = makeOpener()

def getTableLine(c):
  def get(i): 
    return c.contents[i].contents[0].extract()
  return dict(ip=get(1), time=get(3), client=get(5))


def getClientInfo():
  try:
    soup = BeautifulSoup.BeautifulSoup(OPENER.open(Config.CLIENT_URL).read())
  except:
    print 'No client for ', Config.CLIENT_URL
    return
    
  contents = soup.find(id='clientTable')

  first = True
  for c in contents:
    if hasattr(c, 'contents'):
      if first:
        first = False
      else:
        yield getTableLine(c)


def getClients():
  File.replaceAtomic(Config.CLIENT_FILE, json.dumps(list(getClientInfo())))

