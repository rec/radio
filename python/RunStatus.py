import os
import grp
import signal
import daemon
import lockfile

import Status

def runStatus():

  context = daemon.DaemonContext(
    working_directory='/home/radio/public_html/generated',
    umask=0o002,
    pidfile=lockfile.FileLock('/var/run/spam.pid'),
    )

  context.signal_map = {
    signal.SIGTERM: 'terminate',
    signal.SIGHUP: 'terminate',
    signal.SIGUSR1: reload_program_config,
    }
