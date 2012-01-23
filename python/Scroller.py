import datetime

SCROLLER_FORMAT = (
  '<li style="' + 
  'font-size:13px; ' +
  'line-height: 20px; ' +
  'height: 20px; ' + 
  'overflow: hidden; ' + 
  'border-bottom: 1px dotted #CCC; ' + 
  'color: #333; ' + 
  '{alternate}' + 
  '"> <span style="color:#AF7B00;">{time}</span> ' + 
  '{title}' + 
  '</li>')

MAX_SCROLLER_LINES = 30
SEPARATOR = '\n'

ALTERNATES = ['background: #F0F0F0', 
              'background: #CCC']

def findAlternate(line, alternates):
  for i, a in enumerate(alternates):
    if a in line:
      return alternates[(i + 1) % len(alternates)]
  return alternates[0]


def scroll(previous, title):
  lines = []
  last = ''
  if previous:
    lines = previous.split(SEPARATOR)
    if lines:
      last = lines[0]

  s = SCROLLER_FORMAT.format(
    alternate=findAlternate(last, ALTERNATES),
    time=datetime.datetime.now().strftime('%H:%M'),
    title=title)

  lines.insert(0, s)
  if len(lines) > MAX_SCROLLER_LINES:
    lines = lines[0:MAX_SCROLLER_LINES]

  return SEPARATOR.join(lines)
