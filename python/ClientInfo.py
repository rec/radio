import urllib2
import BeautifulSoup

URL = 'http://ax.to:8000/admin/listclients.xsl?mount=/swirlymount'

def makeOpener():
  # create a password manager
  password_mgr = urllib2.HTTPPasswordMgrWithDefaultRealm()

  # Add the username and password.
  # If we knew the realm, we could use it instead of ``None``.
  top_level_url = "http://example.com/foo/"
  password_mgr.add_password(None, 'http://ax.to:8000/admin/', 'admin', 'wombatwombat')

  handler = urllib2.HTTPBasicAuthHandler(password_mgr)

  # create "opener" (OpenerDirector instance)
  return urllib2.build_opener(handler)

OPENER = makeOpener()

def getTableLine(c):
  def get(i): 
    return c.contents[i].contents[0].extract()
  return dict(ip=get(1), time=get(3), client=get(5))


def getClientInfo():
  soup = BeautifulSoup.BeautifulSoup(OPENER.open(URL).read())
  contents = soup.find(id='clientTable')

  first = True
  for c in contents:
    if hasattr(c, 'contents'):
      if first:
        first = False
      else:
        yield getTableLine(c)

