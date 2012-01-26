wget http://www.crummy.com/software/BeautifulSoup/download/3.x/BeautifulSoup-3.2.0.tar.gz
tar xzf BeautifulSoup-3.2.0.tar.gz
cd BeautifulSoup-3.2.0
python setup.py build
python setup.py install
cd ..
rm -Rf BeautifulSoup-3.2.0*

wget http://pypi.python.org/packages/source/s/simplejson/simplejson-2.3.2.tar.gz#md5=0863e016f682f06ead07dd9ef
tar xzf simplejson-2.3.2.tar.gz
cd simplejson-2.3.2
python setup.py build
python setup.py install
cd ..
rm -Rf simplejson-2.3.2*

wget http://httplib2.googlecode.com/files/httplib2-0.7.2.tar.gz
tar xzf httplib2-0.7.2.tar.gz
cd httplib2-0.7.2
python setup.py build
python setup.py install
cd ..
rm -Rf httplib2-0.7.2/*

wget http://pypi.python.org/packages/2.7/s/setuptools/setuptools-0.6c11-py2.7.egg#md5=fe1f997bc722265116870bc7919059ea
sh setuptools-0.6c11-py2.7.egg 

git clone https://github.com/simplegeo/python-oauth2.git
cd python-oauth2
python setup.py build
python setup.py install
cd ..
rm -Rf python-oauth2*

wget http://python-twitter.googlecode.com/files/python-twitter-0.8.2.tar.gz
tar xzf python-twitter-0.8.2.tar.gz
cd python-twitter-0.8.2
python setup.py build
python setup.py install
cd ..
rm -Rf python-twitter-0.8.2*
