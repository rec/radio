/*	$Id: ezstream.c 16537 2009-08-30 21:55:24Z moritz $	*/
/*
 *  ezstream - source client for Icecast with external en-/decoder support
 *  Copyright (C) 2003, 2004, 2005, 2006  Ed Zaleski <oddsock@oddsock.org>
 *  Copyright (C) 2007, 2009              Moritz Grimm <mdgrimm@gmx.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "ezstream.h"

#ifdef HAVE_SIGNAL_H
# include <signal.h>
#endif

#include <shout/shout.h>

#include "configfile.h"
#include "metadata.h"
#include "playlist.h"
#include "strfctns.h"
#include "util.h"
#include "xalloc.h"

#define STREAM_DONE	0
#define STREAM_CONT	1
#define STREAM_SKIP	2
#define STREAM_SERVERR	3
#define STREAM_UPDMDATA 4

#ifdef HAVE___PROGNAME
extern char		*__progname;
#else
char			*__progname;
#endif /* HAVE___PROGNAME */

int			 nFlag;
int			 qFlag;
int			 sFlag;
int			 vFlag;
int			 metadataFromProgram;

EZCONFIG		*pezConfig = NULL;
playlist_t		*playlist = NULL;
int			 playlistMode = 0;
unsigned int		 resource_errors = 0;

#ifdef HAVE_SIGNALS
const int		 ezstream_signals[] = {
	SIGTERM, SIGINT, SIGHUP, SIGUSR1, SIGUSR2
};

volatile sig_atomic_t	 rereadPlaylist = 0;
volatile sig_atomic_t	 rereadPlaylist_notify = 0;
volatile sig_atomic_t	 skipTrack = 0;
volatile sig_atomic_t	 queryMetadata = 0;
volatile sig_atomic_t	 quit = 0;
#else
int			 rereadPlaylist = 0;
int			 rereadPlaylist_notify = 0;
int			 skipTrack = 0;
int			 queryMetadata = 0;
int			 quit = 0;
#endif /* HAVE_SIGNALS */

typedef struct tag_ID3Tag {
	char	tag[3];
	char	trackName[30];
	char	artistName[30];
	char	albumName[30];
	char	year[3];
	char	comment[30];
	char	genre;
} ID3Tag;

int		urlParse(const char *, char **, unsigned short *, char **);
void		replaceString(const char *, char *, size_t, const char *,
			      const char *);
char *		buildCommandString(const char *, const char *, metadata_t *);
char *		getMetadataString(const char *, metadata_t *);
metadata_t *	getMetadata(const char *);
int		setMetadata(shout_t *, metadata_t *, char **);
FILE *		openResource(shout_t *, const char *, int *, metadata_t **,
			     int *, long *);
int		reconnectServer(shout_t *, int);
const char *	getTimeString(long);
int		sendStream(shout_t *, FILE *, const char *, int, const char *,
			   struct timeval *);
int		streamFile(shout_t *, const char *);
int		streamPlaylist(shout_t *, const char *);
char *		getProgname(const char *);
void		usage(void);
void		usageHelp(void);
int		ez_shutdown(int);

#ifdef HAVE_SIGNALS
void		sig_handler(int);

# ifndef SIG_IGN
#  define SIG_IGN	 (void (*)(int))1
# endif /* !SIG_IGN */

void
sig_handler(int sig)
{
	switch (sig) {
	case SIGTERM:
	case SIGINT:
		quit = 1;
		break;
	case SIGHUP:
		rereadPlaylist = 1;
		rereadPlaylist_notify = 1;
		break;
	case SIGUSR1:
		skipTrack = 1;
		break;
	case SIGUSR2:
		queryMetadata = 1;
		break;
	default:
		break;
	}
}
#endif /* HAVE_SIGNALS */

int
urlParse(const char *url, char **hostname, unsigned short *port,
	 char **mountname)
{
	const char	*p1, *p2, *p3;
	char		 tmpPort[6] = "";
	size_t		 hostsiz, mountsiz;
	const char	*errstr;

	if (hostname == NULL || port == NULL || mountname == NULL) {
		printf("%s: urlParse(): Internal error: Bad arguments\n",
		       __progname);
		exit(1);
	}

	if (strncmp(url, "http://", strlen("http://")) != 0) {
		printf("%s: Error: Invalid <url>: Not an HTTP address\n",
		       __progname);
		return (0);
	}

	p1 = url + strlen("http://");
	p2 = strchr(p1, ':');
	if (p2 == NULL) {
		printf("%s: Error: Invalid <url>: Missing port\n",
		       __progname);
		return (0);
	}
	hostsiz = (p2 - p1) + 1;
	*hostname = xmalloc(hostsiz);
	strlcpy(*hostname, p1, hostsiz);

	p2++;
	p3 = strchr(p2, '/');
	if (p3 == NULL || p3 - p2 >= (int)sizeof(tmpPort)) {
		printf("%s: Error: Invalid <url>: Missing mountpoint or too long port number\n",
		       __progname);
		xfree(*hostname);
		return (0);
	}

	strlcpy(tmpPort, p2, (p3 - p2) + 1UL);
	*port = (unsigned short)strtonum(tmpPort, 1LL, (long long)USHRT_MAX, &errstr);
	if (errstr) {
		printf("%s: Error: Invalid <url>: Port '%s' is %s\n",
		       __progname, tmpPort, errstr);
		xfree(*hostname);
		return (0);
	}

	mountsiz = strlen(p3) + 1;
	*mountname = xmalloc(mountsiz);
	strlcpy(*mountname, p3, mountsiz);

	return (1);
}

void
replaceString(const char *source, char *dest, size_t size,
	      const char *from, const char *to)
{
	const char	*p1 = source;
	const char	*p2;

	p2 = strstr(p1, from);
	if (p2 != NULL) {
		if ((unsigned int)(p2 - p1) >= size) {
			printf("%s: replaceString(): Internal error: p2 - p1 >= size\n",
			       __progname);
			abort();
		}
		strncat(dest, p1, (size_t)(p2 - p1));
		strlcat(dest, to, size);
		p1 = p2 + strlen(from);
	}
	strlcat(dest, p1, size);
}

char *
buildCommandString(const char *extension, const char *fileName,
		   metadata_t *mdata)
{
	char	*commandString = NULL;
	size_t	 commandStringLen = 0;
	char	*encoder = NULL;
	char	*decoder = NULL;
	char	*newDecoder = NULL;
	size_t	 newDecoderLen = 0;
	char	*newEncoder = NULL;
	size_t	 newEncoderLen = 0;
	char	*localTitle = UTF8toCHAR(metadata_get_title(mdata),
					 ICONV_REPLACE);
	char	*localArtist = UTF8toCHAR(metadata_get_artist(mdata),
					  ICONV_REPLACE);
	char	*localMetaString = UTF8toCHAR(metadata_get_string(mdata),
					      ICONV_REPLACE);

	decoder = xstrdup(getFormatDecoder(extension));
	if (strlen(decoder) == 0) {
		printf("%s: Unknown extension '%s', cannot decode '%s'\n",
		       __progname, extension, fileName);
		xfree(localTitle);
		xfree(localArtist);
		xfree(localMetaString);
		xfree(decoder);
		return (NULL);
	}
	newDecoderLen = strlen(decoder) + strlen(fileName) + 1;
	newDecoder = xcalloc(newDecoderLen, sizeof(char));
	replaceString(decoder, newDecoder, newDecoderLen, TRACK_PLACEHOLDER,
		      fileName);
	if (strstr(decoder, ARTIST_PLACEHOLDER) != NULL) {
		size_t tmpLen = strlen(newDecoder) + strlen(localArtist) + 1;
		char *tmpStr = xcalloc(tmpLen, sizeof(char));
		replaceString(newDecoder, tmpStr, tmpLen, ARTIST_PLACEHOLDER,
			      localArtist);
		xfree(newDecoder);
		newDecoder = tmpStr;
	}
	if (strstr(decoder, TITLE_PLACEHOLDER) != NULL) {
		size_t tmpLen = strlen(newDecoder) + strlen(localTitle) + 1;
		char *tmpStr = xcalloc(tmpLen, sizeof(char));
		replaceString(newDecoder, tmpStr, tmpLen, TITLE_PLACEHOLDER,
			      localTitle);
		xfree(newDecoder);
		newDecoder = tmpStr;
	}
	/*
	 * if meta
	 *   if (prog && format)
	 *      metatoformat
	 *   else
	 *     if (!prog && title)
	 *       emptymeta
	 *     else
	 *       replacemeta
	 */
	if (strstr(decoder, METADATA_PLACEHOLDER) != NULL) {
		if (metadataFromProgram && pezConfig->metadataFormat != NULL) {
			char *mdataString = getMetadataString(pezConfig->metadataFormat, mdata);
			size_t tmpLen = strlen(newDecoder) + strlen(mdataString) + 1;
			char *tmpStr = xcalloc(tmpLen, sizeof(char));
			replaceString(newDecoder, tmpStr, tmpLen,
				      METADATA_PLACEHOLDER, mdataString);
			xfree(newDecoder);
			xfree(mdataString);
			newDecoder = tmpStr;
		} else {
			if (!metadataFromProgram && strstr(decoder, TITLE_PLACEHOLDER) != NULL) {
				size_t tmpLen = strlen(newDecoder) + 1;
				char *tmpStr = xcalloc(tmpLen, sizeof(char));
				replaceString(newDecoder, tmpStr, tmpLen,
					      METADATA_PLACEHOLDER, "");
				xfree(newDecoder);
				newDecoder = tmpStr;
			} else {
				size_t tmpLen = strlen(newDecoder) + strlen(localMetaString) + 1;
				char *tmpStr = xcalloc(tmpLen, sizeof(char));
				replaceString(newDecoder, tmpStr, tmpLen,
					      METADATA_PLACEHOLDER,
					      localMetaString);
				xfree(newDecoder);
				newDecoder = tmpStr;
			}
		}
	}

	encoder = xstrdup(getFormatEncoder(pezConfig->format));
	if (strlen(encoder) == 0) {
		if (vFlag)
			printf("%s: Passing through%s%s data from the decoder\n",
			       __progname,
			       (strcmp(pezConfig->format, THEORA_FORMAT) != 0) ? " (unsupported) " : " ",
			       pezConfig->format);
		commandStringLen = strlen(newDecoder) + 1;
		commandString = xcalloc(commandStringLen, sizeof(char));
		strlcpy(commandString, newDecoder, commandStringLen);
		xfree(localTitle);
		xfree(localArtist);
		xfree(localMetaString);
		xfree(decoder);
		xfree(encoder);
		xfree(newDecoder);
		return (commandString);
	}

	newEncoderLen = strlen(encoder) + strlen(localArtist) + 1;
	newEncoder = xcalloc(newEncoderLen, sizeof(char));
	replaceString(encoder, newEncoder, newEncoderLen, ARTIST_PLACEHOLDER,
		      localArtist);
	if (strstr(encoder, TITLE_PLACEHOLDER) != NULL) {
		size_t tmpLen = strlen(newEncoder) + strlen(localTitle) + 1;
		char *tmpStr = xcalloc(tmpLen, sizeof(char));
		replaceString(newEncoder, tmpStr, tmpLen, TITLE_PLACEHOLDER,
			      localTitle);
		xfree(newEncoder);
		newEncoder = tmpStr;
	}
	if (strstr(encoder, METADATA_PLACEHOLDER) != NULL) {
		if (metadataFromProgram && pezConfig->metadataFormat != NULL) {
			char *mdataString = getMetadataString(pezConfig->metadataFormat, mdata);
			size_t tmpLen = strlen(newEncoder) + strlen(mdataString) + 1;
			char *tmpStr = xcalloc(tmpLen, sizeof(char));
			replaceString(newEncoder, tmpStr, tmpLen,
				      METADATA_PLACEHOLDER, mdataString);
			xfree(newEncoder);
			xfree(mdataString);
			newEncoder = tmpStr;
		} else {
			if (!metadataFromProgram && strstr(encoder, TITLE_PLACEHOLDER) != NULL) {
				size_t tmpLen = strlen(newEncoder) + 1;
				char *tmpStr = xcalloc(tmpLen, sizeof(char));
				replaceString(newEncoder, tmpStr, tmpLen,
					      METADATA_PLACEHOLDER, "");
				xfree(newEncoder);
				newEncoder = tmpStr;
			} else {
				size_t tmpLen = strlen(newEncoder) + strlen(localMetaString) + 1;
				char *tmpStr = xcalloc(tmpLen, sizeof(char));
				replaceString(newEncoder, tmpStr, tmpLen,
					      METADATA_PLACEHOLDER,
					      localMetaString);
				xfree(newEncoder);
				newEncoder = tmpStr;
			}
		}
	}

	commandStringLen = strlen(newDecoder) + strlen(" | ") +
		strlen(newEncoder) + 1;
	commandString = xcalloc(commandStringLen, sizeof(char));
	snprintf(commandString, commandStringLen, "%s | %s", newDecoder,
		 newEncoder);

	xfree(localTitle);
	xfree(localArtist);
	xfree(localMetaString);
	xfree(decoder);
	xfree(encoder);
	xfree(newDecoder);
	xfree(newEncoder);

	return (commandString);
}

char *
getMetadataString(const char *format, metadata_t *mdata)
{
	char	*tmp, *str;
	size_t	 len;

	if (mdata == NULL) {
		printf("%s: getMetadataString(): Internal error: NULL metadata_t\n",
		       __progname);
		abort();
	}

	if (format == NULL)
		return (NULL);

	str = xstrdup(format);

	if (strstr(format, ARTIST_PLACEHOLDER) != NULL) {
		len = strlen(str) + strlen(metadata_get_artist(mdata)) + 1;
		tmp = xcalloc(len, sizeof(char));
		replaceString(str, tmp, len, ARTIST_PLACEHOLDER,
			      metadata_get_artist(mdata));
		xfree(str);
		str = tmp;
	}
	if (strstr(format, TITLE_PLACEHOLDER) != NULL) {
		len = strlen(str) + strlen(metadata_get_title(mdata)) + 1;
		tmp = xcalloc(len, sizeof(char));
		replaceString(str, tmp, len, TITLE_PLACEHOLDER,
			      metadata_get_title(mdata));
		xfree(str);
		str = tmp;
	}
	if (strstr(format, STRING_PLACEHOLDER) != NULL) {
		len = strlen(str) + strlen(metadata_get_string(mdata)) + 1;
		tmp = xcalloc(len, sizeof(char));
		replaceString(str, tmp, len, STRING_PLACEHOLDER,
			      metadata_get_string(mdata));
		xfree(str);
		str = tmp;
	}
	if (strstr(format, TRACK_PLACEHOLDER) != NULL) {
		len = strlen(str) + strlen(metadata_get_filename(mdata)) + 1;
		tmp = xcalloc(len, sizeof(char));
		replaceString(str, tmp, len, TRACK_PLACEHOLDER,
			      metadata_get_filename(mdata));
		xfree(str);
		str = tmp;
	}

	return (str);
}

metadata_t *
getMetadata(const char *fileName)
{
	metadata_t	*mdata;

	if (metadataFromProgram) {
		if ((mdata = metadata_program(fileName, nFlag)) == NULL)
			return (NULL);

		if (!metadata_program_update(mdata, METADATA_ALL)) {
			metadata_free(&mdata);
			return (NULL);
		}
	} else {
		if ((mdata = metadata_file(fileName, nFlag)) == NULL)
			return (NULL);

		if (!metadata_file_update(mdata)) {
			metadata_free(&mdata);
			return (NULL);
		}
	}

	return (mdata);
}

int
setMetadata(shout_t *shout, metadata_t *mdata, char **mdata_copy)
{
	shout_metadata_t	*shout_mdata = NULL;
	char			*songInfo;
	const char		*artist, *title;
	int			 ret = SHOUTERR_SUCCESS;

	if (shout == NULL) {
		printf("%s: setMetadata(): Internal error: NULL shout_t\n",
		       __progname);
		abort();
	}

	if (mdata == NULL)
		return 1;

	if ((shout_mdata = shout_metadata_new()) == NULL) {
		printf("%s: shout_metadata_new(): %s\n", __progname,
		       strerror(ENOMEM));
		exit(1);
	}

	artist = metadata_get_artist(mdata);
	title = metadata_get_title(mdata);

	/*
	 * We can do this, because we know how libshout works. This adds
	 * "charset=UTF-8" to the HTTP metadata update request and has the
	 * desired effect of letting newer-than-2.3.1 versions of Icecast know
	 * which encoding we're using.
	 */
	if (shout_metadata_add(shout_mdata, "charset", "UTF-8") != SHOUTERR_SUCCESS) {
		/* Assume SHOUTERR_MALLOC */
		printf("%s: shout_metadata_add(): %s\n", __progname,
		       strerror(ENOMEM));
		exit(1);
	}

	if ((songInfo = getMetadataString(pezConfig->metadataFormat, mdata)) == NULL) {
		if (artist[0] == '\0' && title[0] == '\0')
			songInfo = xstrdup(metadata_get_string(mdata));
		else
			songInfo = metadata_assemble_string(mdata);
		if (artist[0] != '\0' && title[0] != '\0') {
			if (shout_metadata_add(shout_mdata, "artist", artist) != SHOUTERR_SUCCESS) {
				printf("%s: shout_metadata_add(): %s\n", __progname,
				       strerror(ENOMEM));
				exit(1);
			}
			if (shout_metadata_add(shout_mdata, "title", title) != SHOUTERR_SUCCESS) {
				printf("%s: shout_metadata_add(): %s\n", __progname,
				       strerror(ENOMEM));
				exit(1);
			}
		} else {
			if (shout_metadata_add(shout_mdata, "song", songInfo) != SHOUTERR_SUCCESS) {
				printf("%s: shout_metadata_add(): %s\n", __progname,
				       strerror(ENOMEM));
				exit(1);
			}
		}
	} else if (shout_metadata_add(shout_mdata, "song", songInfo) != SHOUTERR_SUCCESS) {
		printf("%s: shout_metadata_add(): %s\n", __progname,
		       strerror(ENOMEM));
		exit(1);
	}

	if ((ret = shout_set_metadata(shout, shout_mdata)) != SHOUTERR_SUCCESS)
		printf("%s: shout_set_metadata(): %s\n",
		       __progname, shout_get_error(shout));

	shout_metadata_free(shout_mdata);

	if (ret == SHOUTERR_SUCCESS) {
		if (mdata_copy != NULL && *mdata_copy == NULL)
			*mdata_copy = xstrdup(songInfo);
	}

	xfree(songInfo);
	return (ret);
}

FILE *
openResource(shout_t *shout, const char *fileName, int *popenFlag,
	     metadata_t **mdata_p, int *isStdin, long *songLen)
{
	FILE		*filep = NULL;
	char		 extension[25];
	char		*p = NULL;
	char		*pCommandString = NULL;
	metadata_t	*mdata;

	if (mdata_p != NULL)
		*mdata_p = NULL;
	if (songLen != NULL)
		*songLen = 0;

	if (strcmp(fileName, "stdin") == 0) {
		if (metadataFromProgram) {
			if ((mdata = getMetadata(pezConfig->metadataProgram)) == NULL)
				return (NULL);
			if (setMetadata(shout, mdata, NULL) != SHOUTERR_SUCCESS) {
				metadata_free(&mdata);
				return (NULL);
			}
			if (mdata_p != NULL)
				*mdata_p = mdata;
			else
				metadata_free(&mdata);
		}

		if (isStdin != NULL)
			*isStdin = 1;
#ifdef WIN32
		_setmode(_fileno(stdin), _O_BINARY);
#endif
		filep = stdin;
		return (filep);
	}

	if (isStdin != NULL)
		*isStdin = 0;

	extension[0] = '\0';
	p = strrchr(fileName, '.');
	if (p != NULL)
		strlcpy(extension, p, sizeof(extension));
	for (p = extension; *p != '\0'; p++)
		*p = tolower((int)*p);

	if (strlen(extension) == 0) {
		printf("%s: Error: Cannot determine file type of '%s'\n",
		       __progname, fileName);
		return (filep);
	}

	if (metadataFromProgram) {
		if ((mdata = getMetadata(pezConfig->metadataProgram)) == NULL)
			return (NULL);
	} else {
		if ((mdata = getMetadata(fileName)) == NULL)
			return (NULL);
	}
	if (songLen != NULL)
		*songLen = metadata_get_length(mdata);

	*popenFlag = 0;
	if (pezConfig->reencode) {
		int	stderr_fd = -1;

		pCommandString = buildCommandString(extension, fileName, mdata);
		if (mdata_p != NULL)
			*mdata_p = mdata;
		else
			metadata_free(&mdata);
		if (vFlag > 1)
			printf("%s: Running command `%s`\n", __progname,
			       pCommandString);

		if (qFlag) {
			int fd;

			stderr_fd = dup(fileno(stderr));
			if ((fd = open(_PATH_DEVNULL, O_RDWR, 0)) == -1) {
				printf("%s: Cannot open %s for redirecting STDERR output: %s\n",
				       __progname, _PATH_DEVNULL, strerror(errno));
				exit(1);
			}

			dup2(fd, fileno(stderr));
			if (fd > 2)
				close(fd);
		}

		fflush(NULL);
		errno = 0;
		if ((filep = popen(pCommandString, "r")) == NULL) {
			printf("%s: popen(): Error while executing '%s'",
			       __progname, pCommandString);
			/* popen() does not set errno reliably ... */
			if (errno)
				printf(": %s\n", strerror(errno));
			else
				printf("\n");
		} else {
			*popenFlag = 1;
#ifdef WIN32
			_setmode(_fileno(filep), _O_BINARY );
#endif
		}
		xfree(pCommandString);

		if (qFlag)
			dup2(stderr_fd, fileno(stderr));

		if (stderr_fd > 2)
			close(stderr_fd);

		return (filep);
	}

	if (mdata_p != NULL)
		*mdata_p = mdata;
	else
		metadata_free(&mdata);

	if ((filep = fopen(fileName, "rb")) == NULL)
		printf("%s: %s: %s\n", __progname, fileName,
		       strerror(errno));

	return (filep);
}

int
reconnectServer(shout_t *shout, int closeConn)
{
	unsigned int	i;
	int		close_conn = closeConn;

	printf("%s: Connection to %s lost\n", __progname, pezConfig->URL);

	i = 0;
	while (++i) {
		printf("%s: Attempting reconnection #", __progname);
		if (pezConfig->reconnectAttempts > 0)
			printf("%u/%u: ", i,
			       pezConfig->reconnectAttempts);
		else
			printf("%u: ", i);

		if (close_conn == 0)
			close_conn = 1;
		else
			shout_close(shout);
		if (shout_open(shout) == SHOUTERR_SUCCESS) {
			printf("OK\n%s: Reconnect to %s successful\n",
			       __progname, pezConfig->URL);
			return (1);
		}

		printf("FAILED: %s\n", shout_get_error(shout));

		if (pezConfig->reconnectAttempts > 0 &&
		    i >= pezConfig->reconnectAttempts)
			break;

		printf("%s: Waiting 5s for %s to come back ...\n",
		       __progname, pezConfig->URL);
		if (quit)
			return (0);
		else
			sleep(5);
	};

	printf("%s: Giving up\n", __progname);
	return (0);
}

const char *
getTimeString(long seconds)
{
	static char	str[20];
	long		secs, mins, hours;

	if (seconds < 0)
		return (NULL);

	secs = seconds;
	hours = secs / 3600;
	secs %= 3600;
	mins = secs / 60;
	secs %= 60;

	snprintf(str, sizeof(str), "%ldh%02ldm%02lds", hours, mins, secs);
	return ((const char *)str);
}

int
sendStream(shout_t *shout, FILE *filepstream, const char *fileName,
	   int isStdin, const char *songLenStr, struct timeval *tv)
{
	unsigned char	 buff[4096];
	size_t		 bytes_read, total, oldTotal;
	int		 ret;
	double		 kbps = -1.0;
	struct timeval	 timeStamp, *startTime = tv;

	if (startTime == NULL) {
		printf("%s: sendStream(): Internal error: startTime is NULL\n",
		       __progname);
		abort();
	}

	timeStamp.tv_sec = startTime->tv_sec;
	timeStamp.tv_usec = startTime->tv_usec;

	total = oldTotal = 0;
	ret = STREAM_DONE;
	while ((bytes_read = fread(buff, 1UL, sizeof(buff), filepstream)) > 0) {
		if (shout_get_connected(shout) != SHOUTERR_CONNECTED &&
		    reconnectServer(shout, 0) == 0) {
			ret = STREAM_SERVERR;
			break;
		}

		shout_sync(shout);

		if (shout_send(shout, buff, bytes_read) != SHOUTERR_SUCCESS) {
			printf("%s: shout_send(): %s\n", __progname,
			       shout_get_error(shout));
			if (reconnectServer(shout, 1))
				break;
			else {
				ret = STREAM_SERVERR;
				break;
			}
		}

		if (quit)
			break;
		if (rereadPlaylist_notify) {
			rereadPlaylist_notify = 0;
			if (!pezConfig->fileNameIsProgram)
				printf("%s: SIGHUP signal received, will reread playlist after this file\n",
				       __progname);
		}
		if (skipTrack) {
			skipTrack = 0;
			ret = STREAM_SKIP;
			break;
		}
		if (queryMetadata) {
			queryMetadata = 0;
			if (metadataFromProgram) {
				ret = STREAM_UPDMDATA;
				break;
			}
		}

		total += bytes_read;
		if (qFlag && vFlag) {
			struct timeval	tval;
			double		oldTime, newTime;

			if (!isStdin && playlistMode) {
				if (pezConfig->fileNameIsProgram) {
					char *tmp = xstrdup(pezConfig->fileName);
					printf("  [%s]",
					       local_basename(tmp));
					xfree(tmp);
				} else
					printf("  [%4lu/%-4lu]",
					       playlist_get_position(playlist),
					       playlist_get_num_items(playlist));
			}

			oldTime = (double)timeStamp.tv_sec
				+ (double)timeStamp.tv_usec / 1000000.0;
			ez_gettimeofday((void *)&tval);
			newTime = (double)tval.tv_sec
				+ (double)tval.tv_usec / 1000000.0;
			if (songLenStr == NULL)
				printf("  [ %s]",
				       getTimeString(tval.tv_sec - startTime->tv_sec));
			else
				printf("  [ %s/%s]",
				       getTimeString(tval.tv_sec - startTime->tv_sec),
				       songLenStr);
			if (newTime - oldTime >= 1.0) {
				kbps = (((double)(total - oldTotal) / (newTime - oldTime)) * 8.0) / 1000.0;
				timeStamp.tv_sec = tval.tv_sec;
				timeStamp.tv_usec = tval.tv_usec;
				oldTotal = total;
			}
			if (kbps < 0)
				printf("                 ");
			else
				printf("  [%8.2f kbps]", kbps);

			printf("  \r");
			fflush(stdout);
		}
	}
	if (ferror(filepstream)) {
		if (errno == EINTR) {
			clearerr(filepstream);
			ret = STREAM_CONT;
		} else if (errno == EBADF && isStdin)
			printf("%s: No (more) data available on standard input\n",
			       __progname);
		else
			printf("%s: sendStream(): Error while reading '%s': %s\n",
			       __progname, fileName, strerror(errno));
	}

	return (ret);
}

int
streamFile(shout_t *shout, const char *fileName)
{
	FILE		*filepstream = NULL;
	int		 popenFlag = 0;
	char		*songLenStr = NULL;
	int		 isStdin = 0;
	int		 ret, retval = 0;
	long		 songLen;
	metadata_t	*mdata;
	struct timeval	 startTime;

	if ((filepstream = openResource(shout, fileName, &popenFlag,
					&mdata, &isStdin, &songLen))
	    == NULL) {
		if (++resource_errors > 100) {
			printf("%s: Too many errors -- giving up.\n", __progname);
			return (0);
		}
		/* Continue with next resource on failure: */
		return (1);
	}
	resource_errors = 0;

	if (mdata != NULL) {
		char	*tmp, *metaData;

		tmp = metadata_assemble_string(mdata);
		if ((metaData = UTF8toCHAR(tmp, ICONV_REPLACE)) == NULL)
			metaData = xstrdup("(unknown title)");
		xfree(tmp);
		printf("%s: Streaming ``%s''", __progname, metaData);
		if (vFlag)
			printf(" (file: %s)\n", fileName);
		else
			printf("\n");
		xfree(metaData);

		/* MP3 streams are special, so set the metadata explicitly: */
		if (strcmp(pezConfig->format, MP3_FORMAT) == 0)
			setMetadata(shout, mdata, NULL);

		metadata_free(&mdata);
	} else if (isStdin)
		printf("%s: Streaming from standard input\n", __progname);

	if (songLen > 0)
		songLenStr = xstrdup(getTimeString(songLen));
	ez_gettimeofday((void *)&startTime);
	do {
		ret = sendStream(shout, filepstream, fileName, isStdin,
				 songLenStr, &startTime);
		if (quit)
			break;
		if (ret != STREAM_DONE) {
			if ((skipTrack && rereadPlaylist) ||
			    (skipTrack && queryMetadata)) {
				skipTrack = 0;
				ret = STREAM_CONT;
			}
			if (queryMetadata && rereadPlaylist) {
				queryMetadata = 0;
				ret = STREAM_CONT;
			}
			if (ret == STREAM_SKIP || skipTrack) {
				skipTrack = 0;
				if (!isStdin && vFlag)
					printf("%s: SIGUSR1 signal received, skipping current track\n",
					       __progname);
				retval = 1;
				ret = STREAM_DONE;
			}
			if (ret == STREAM_UPDMDATA || queryMetadata) {
				queryMetadata = 0;
				if (metadataFromProgram) {
					char		*mdataStr = NULL;
					metadata_t	*prog_mdata;

					if (vFlag > 1)
						printf("%s: Querying '%s' for fresh metadata\n",
						       __progname, pezConfig->metadataProgram);
					if ((prog_mdata = getMetadata(pezConfig->metadataProgram)) == NULL) {
						retval = 0;
						ret = STREAM_DONE;
						continue;
					}
					if (setMetadata(shout, prog_mdata, &mdataStr) != SHOUTERR_SUCCESS) {
						retval = 0;
						ret = STREAM_DONE;
						continue;
					}
					metadata_free(&prog_mdata);
					printf("%s: New metadata: ``%s''\n",
					       __progname, mdataStr);
					xfree(mdataStr);
				}
			}
			if (ret == STREAM_SERVERR) {
				retval = 0;
				ret = STREAM_DONE;
			}
		} else
			retval = 1;
	} while (ret != STREAM_DONE);

	if (popenFlag)
		pclose(filepstream);
	else
		fclose(filepstream);

	if (songLenStr != NULL)
		xfree(songLenStr);

	return (retval);
}

int
streamPlaylist(shout_t *shout, const char *fileName)
{
	const char	*song;
	char		 lastSong[PATH_MAX];

	if (playlist == NULL) {
		if (pezConfig->fileNameIsProgram) {
			if ((playlist = playlist_program(fileName)) == NULL)
				return (0);
		} else {
			if ((playlist = playlist_read(fileName)) == NULL)
				return (0);
			if (vFlag && playlist_get_num_items(playlist) == 0)
				printf("%s: Warning: Playlist '%s' is empty\n",
				       __progname, fileName);
		}
	} else {
		/*
		 * XXX: This preserves traditional behavior, however,
		 *      rereading the playlist after each walkthrough seems a
		 *      bit more logical.
		 */
		playlist_rewind(playlist);
	}

	if (!pezConfig->fileNameIsProgram && pezConfig->shuffle)
		playlist_shuffle(playlist);

	while ((song = playlist_get_next(playlist)) != NULL) {
		strlcpy(lastSong, song, sizeof(lastSong));
		if (!streamFile(shout, song))
			return (0);
		if (quit)
			break;
		if (rereadPlaylist) {
			rereadPlaylist = rereadPlaylist_notify = 0;
			if (pezConfig->fileNameIsProgram)
				continue;
			printf("%s: Rereading playlist\n", __progname);
			if (!playlist_reread(&playlist))
				return (0);
			if (pezConfig->shuffle)
				playlist_shuffle(playlist);
			else {
				playlist_goto_entry(playlist, lastSong);
				playlist_skip_next(playlist);
			}
			continue;
		}
	}

	return (1);
}

/*
 * Borrowed from OpenNTPd-portable's compat-openbsd/bsd-misc.c.
 * Does not use xalloc on purpose, as the 9 bytes of memory that don't get
 * cleaned up in the end really don't matter.
 */
char *
getProgname(const char *argv0)
{
#ifdef HAVE___PROGNAME
	(void)argv0;
	return (strdup(__progname));
#else
	char	*p;

	if (argv0 == NULL)
		return ((char *)"ezstream");
	p = strrchr(argv0, path_separators[0]);
	if (p == NULL)
		p = (char *)argv0;
	else
		p++;

	return (strdup(p));
#endif /* HAVE___PROGNAME */
}

int
ez_shutdown(int exitval)
{
	shout_shutdown();
	playlist_shutdown();
	freeConfig(pezConfig);
	xalloc_shutdown();

	return (exitval);
}

void
usage(void)
{
	printf("usage: %s [-hnqVv] -c configfile\n", __progname);
	printf("       %s -s [playlist]\n", __progname);
}

void
usageHelp(void)
{
	printf("\n");
	printf("  -c configfile  use XML configuration in configfile (mandatory)\n");
	printf("  -h             display this additional help and exit\n");
	printf("  -n             normalize metadata strings\n");
	printf("  -q             suppress STDERR output from external en-/decoders\n");
	printf("  -s [playlist]  read lines from playlist (or STDIN), shuffle and print them to\n");
	printf("                 STDOUT, then exit\n");
	printf("  -V             print the version number and exit\n");
	printf("  -v             verbose output (use twice for more effect)\n");
	printf("\n");
	printf("See the ezstream(1) manual for detailed information.\n");
}

int
main(int argc, char *argv[])
{
	int		 c;
	char		*configFile = NULL;
	char		*host = NULL;
	unsigned short	 port = 0;
	char		*mount = NULL;
	shout_t 	*shout;
	extern char	*optarg;
	extern int	 optind;
#ifdef HAVE_SIGNALS
	struct sigaction act;
	unsigned int	 i;
#endif

#ifdef XALLOC_DEBUG
	xalloc_initialize_debug(2, NULL);
#else
	xalloc_initialize();
#endif /* XALLOC_DEBUG */

	playlist_init();
	shout_init();

	__progname = getProgname(argv[0]);
	pezConfig = getEZConfig();

	nFlag = 0;
	qFlag = 0;
	vFlag = 0;

	while ((c = local_getopt(argc, argv, "c:hnqsVv")) != -1) {
		switch (c) {
		case 'c':
			if (configFile != NULL) {
				printf("Error: multiple -c arguments given\n");
				usage();
				return (ez_shutdown(2));
			}
			configFile = xstrdup(optarg);
			break;
		case 'h':
			usage();
			usageHelp();
			return (ez_shutdown(0));
		case 'n':
			nFlag = 1;
			break;
		case 'q':
			qFlag = 1;
			break;
		case 's':
			sFlag = 1;
			break;
		case 'V':
			printf("%s\n", PACKAGE_STRING);
			return (ez_shutdown(0));
		case 'v':
			vFlag++;
			break;
		case '?':
			usage();
			return (ez_shutdown(2));
		default:
			break;
		}
	}
	argc -= optind;
	argv += optind;

	if (sFlag) {
		playlist_t	*pl;
		const char	*entry;

		switch (argc) {
		case 0:
			pl = playlist_read(NULL);
			if (pl == NULL)
				return (ez_shutdown(1));
			break;
		case 1:
			pl = playlist_read(argv[0]);
			if (pl == NULL)
				return (ez_shutdown(1));
			break;
		default:
			printf("Error: Too many arguments.\n");
			return (ez_shutdown(2));
		}

		playlist_shuffle(pl);
		while ((entry = playlist_get_next(pl)) != NULL)
			printf("%s\n", entry);

		playlist_free(&pl);

		return (ez_shutdown(0));
	}

	if (configFile == NULL) {
		printf("You must supply a config file with the -c argument.\n");
		usage();
		return (ez_shutdown(2));
	} else {
		/*
		 * Attempt to open configFile here for a more meaningful error
		 * message. Where possible, do it with stat() and check for
		 * safe config file permissions.
		 */
#ifdef HAVE_STAT
		struct stat	  st;

		if (stat(configFile, &st) == -1) {
			printf("%s: %s\n", configFile, strerror(errno));
			usage();
			return (ez_shutdown(2));
		}
		if (vFlag && (st.st_mode & (S_IRGRP | S_IROTH)))
			printf("%s: Warning: %s is group and/or world readable\n",
			       __progname, configFile);
		if (st.st_mode & (S_IWGRP | S_IWOTH)) {
			printf("%s: Error: %s is group and/or world writeable\n",
			       __progname, configFile);
			return (ez_shutdown(2));
		}
#else
		FILE		 *tmp;

		if ((tmp = fopen(configFile, "r")) == NULL) {
			printf("%s: %s\n", configFile, strerror(errno));
			usage();
			return (ez_shutdown(2));
		}
		fclose(tmp);
#endif /* HAVE_STAT */
	}

	if (!parseConfig(configFile))
		return (ez_shutdown(2));

	if (pezConfig->URL == NULL) {
		printf("%s: Error: Missing <url>\n", configFile);
		return (ez_shutdown(2));
	}
	if (!urlParse(pezConfig->URL, &host, &port, &mount)) {
		printf("Must be of the form ``http://server:port/mountpoint''\n");
		return (ez_shutdown(2));
	}
	if (strlen(host) == 0) {
		printf("%s: Error: Invalid <url>: Missing server:\n", configFile);
		printf("Must be of the form ``http://server:port/mountpoint''\n");
		return (ez_shutdown(2));
	}
	if (strlen(mount) == 0) {
		printf("%s: Error: Invalid <url>: Missing mountpoint:\n", configFile);
		printf("Must be of the form ``http://server:port/mountpoint''\n");
		return (ez_shutdown(2));
	}
	if (pezConfig->password == NULL) {
		printf("%s: Error: Missing <sourcepassword>\n", configFile);
		return (ez_shutdown(2));
	}
	if (pezConfig->fileName == NULL) {
		printf("%s: Error: Missing <filename>\n", configFile);
		return (ez_shutdown(2));
	}
	if (pezConfig->format == NULL) {
		printf("%s: Warning: Missing <format>:\n", configFile);
		printf("Specify a stream format of either MP3, VORBIS or THEORA\n");
	}

	xfree(configFile);

	if ((shout = stream_setup(host, port, mount)) == NULL)
		return (ez_shutdown(1));

	if (pezConfig->metadataProgram != NULL)
		metadataFromProgram = 1;
	else
		metadataFromProgram = 0;

#ifdef HAVE_SIGNALS
	memset(&act, 0, sizeof(act));
	act.sa_handler = sig_handler;
# ifdef SA_RESTART
	act.sa_flags = SA_RESTART;
# endif
	for (i = 0; i < sizeof(ezstream_signals) / sizeof(int); i++) {
		if (sigaction(ezstream_signals[i], &act, NULL) == -1) {
			printf("%s: sigaction(): %s\n",
			       __progname, strerror(errno));
			return (ez_shutdown(1));
		}
	}
	/*
	 * Ignore SIGPIPE, which has been seen to give a long-running ezstream
	 * process trouble. EOF and/or EPIPE are also easier to handle.
	 */
	act.sa_handler = SIG_IGN;
	if (sigaction(SIGPIPE, &act, NULL) == -1) {
		printf("%s: sigaction(): %s\n",
		       __progname, strerror(errno));
		return (ez_shutdown(1));
	}
#endif /* HAVE_SIGNALS */

	if (shout_open(shout) == SHOUTERR_SUCCESS) {
		int	ret;

		printf("%s: Connected to http://%s:%hu%s\n", __progname,
		       host, port, mount);

		if (pezConfig->fileNameIsProgram ||
		    strrcasecmp(pezConfig->fileName, ".m3u") == 0 ||
		    strrcasecmp(pezConfig->fileName, ".txt") == 0)
			playlistMode = 1;
		else
			playlistMode = 0;

		if (vFlag && pezConfig->fileNameIsProgram)
			printf("%s: Using program '%s' to get filenames for streaming\n",
			       __progname, pezConfig->fileName);

		ret = 1;
		do {
			if (playlistMode) {
				ret = streamPlaylist(shout,
						     pezConfig->fileName);
			} else {
				ret = streamFile(shout, pezConfig->fileName);
			}
			if (quit)
				break;
			if (pezConfig->streamOnce)
				break;
		} while (ret);

		shout_close(shout);
	} else
		printf("%s: Connection to http://%s:%hu%s failed: %s\n", __progname,
		       host, port, mount, shout_get_error(shout));

	if (quit)
		printf("\r%s: SIGINT or SIGTERM received\n", __progname);

	if (vFlag)
		printf("%s: Exiting ...\n", __progname);

	xfree(host);
	xfree(mount);
	playlist_free(&playlist);

	return (ez_shutdown(0));
}
