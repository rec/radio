/*	$Id: util.c 16319 2009-07-22 20:19:38Z moritz $	*/
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

/*
 * This file contains utility functions, as well as a few other unexciting
 * but verbose functions outsourced from ezstream.c to make it more readable.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "ezstream.h"

#ifdef HAVE_LANGINFO_H
# include <langinfo.h>
#endif
#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

#ifdef HAVE_ICONV
# include <iconv.h>
#endif
#include <shout/shout.h>

#include "util.h"
#include "configfile.h"
#include "xalloc.h"

#ifndef BUFSIZ
# define BUFSIZ 1024
#endif

extern EZCONFIG *pezConfig;
extern char	*__progname;

char *	iconvert(const char *, const char *, const char *, int);

int
strrcmp(const char *s, const char *sub)
{
	size_t	slen = strlen(s);
	size_t	sublen = strlen(sub);

	if (sublen > slen)
		return (1);

	return (memcmp(s + slen - sublen, sub, sublen));
}

int
strrcasecmp(const char *s, const char *sub)
{
	char	*s_cpy = xstrdup(s);
	char	*sub_cpy = xstrdup(sub);
	char	*p;
	int	 ret;

	for (p = s_cpy; *p != '\0'; p++)
		*p = tolower((int)*p);

	for (p = sub_cpy; *p != '\0'; p++)
		*p = tolower((int)*p);

	ret = strrcmp(s_cpy, sub_cpy);

	xfree(s_cpy);
	xfree(sub_cpy);

	return (ret);
}

shout_t *
stream_setup(const char *host, unsigned short port, const char *mount)
{
	shout_t *shout = NULL;

	if ((shout = shout_new()) == NULL) {
		printf("%s: shout_new(): %s", __progname, strerror(ENOMEM));
		return (NULL);
	}

	if (shout_set_host(shout, host) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_host(): %s\n", __progname,
		       shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (shout_set_protocol(shout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_protocol(): %s\n", __progname,
			shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (shout_set_port(shout, port) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_port: %s\n", __progname,
			shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (shout_set_password(shout, pezConfig->password) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_password(): %s\n", __progname,
			shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (shout_set_mount(shout, mount) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_mount(): %s\n", __progname,
			shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (shout_set_user(shout, "source") != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_user(): %s\n", __progname,
			shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}

	if (!strcmp(pezConfig->format, MP3_FORMAT) &&
	    shout_set_format(shout, SHOUT_FORMAT_MP3) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_format(MP3): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if ((!strcmp(pezConfig->format, VORBIS_FORMAT) ||
	     !strcmp(pezConfig->format, THEORA_FORMAT)) &&
	    shout_set_format(shout, SHOUT_FORMAT_OGG) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_format(OGG): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}

	if (pezConfig->username &&
	    shout_set_user(shout, pezConfig->username) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_user(): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (pezConfig->serverName &&
	    shout_set_name(shout, pezConfig->serverName) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_name(): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (pezConfig->serverURL &&
	    shout_set_url(shout, pezConfig->serverURL) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_url(): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (pezConfig->serverGenre &&
	    shout_set_genre(shout, pezConfig->serverGenre) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_genre(): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (pezConfig->serverDescription &&
	    shout_set_description(shout, pezConfig->serverDescription) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_description(): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (pezConfig->serverBitrate &&
	    shout_set_audio_info(shout, SHOUT_AI_BITRATE, pezConfig->serverBitrate) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_audio_info(AI_BITRATE): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (pezConfig->serverChannels &&
	    shout_set_audio_info(shout, SHOUT_AI_CHANNELS, pezConfig->serverChannels) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_audio_info(AI_CHANNELS): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (pezConfig->serverSamplerate &&
	    shout_set_audio_info(shout, SHOUT_AI_SAMPLERATE, pezConfig->serverSamplerate) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_audio_info(AI_SAMPLERATE): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}
	if (pezConfig->serverQuality &&
	    shout_set_audio_info(shout, SHOUT_AI_QUALITY, pezConfig->serverQuality) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_audio_info(AI_QUALITY): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}

	if (shout_set_public(shout, (unsigned int)pezConfig->serverPublic) != SHOUTERR_SUCCESS) {
		printf("%s: shout_set_public(): %s\n",
		       __progname, shout_get_error(shout));
		shout_free(shout);
		return (NULL);
	}

	return (shout);
}

char *
CHARtoUTF8(const char *in_str, int mode)
{
#ifndef WIN32
	char	*codeset;

# if defined(HAVE_NL_LANGINFO) && defined(HAVE_SETLOCALE) && defined(CODESET)
	setlocale(LC_CTYPE, "");
	codeset = nl_langinfo((nl_item)CODESET);
	setlocale(LC_CTYPE, "C");
# else
	codeset = (char *)"";
# endif /* HAVE_NL_LANGINFO && HAVE_SETLOCALE && CODESET */
#else
	char	 codeset[24];

	snprintf(codeset, sizeof(codeset), "CP%u", GetACP());
#endif /* !WIN32 */

	return (iconvert(in_str, codeset, "UTF-8", mode));
}

char *
UTF8toCHAR(const char *in_str, int mode)
{
#ifndef WIN32
	char	*codeset;

# if defined(HAVE_NL_LANGINFO) && defined(HAVE_SETLOCALE) && defined(CODESET)
	setlocale(LC_CTYPE, "");
	codeset = nl_langinfo((nl_item)CODESET);
	setlocale(LC_CTYPE, "C");
# else
	codeset = (char *)"";
# endif /* HAVE_NL_LANGINFO && HAVE_SETLOCALE && CODESET */
#else
	char	 codeset[24];

	snprintf(codeset, sizeof(codeset), "CP%u", GetACP());
#endif /* !WIN32 */

	return (iconvert(in_str, "UTF-8", codeset, mode));
}

char *
iconvert(const char *in_str, const char *from, const char *to, int mode)
{
#ifdef HAVE_ICONV
	iconv_t 		 cd;
	ICONV_CONST char	*input, *ip;
	size_t			 input_len;
	char			*output;
	size_t			 output_size;
	char			 buf[BUFSIZ], *bp;
	size_t			 bufavail;
	size_t			 out_pos;
	char			*tocode;

	if (NULL == in_str)
		return (xstrdup(""));

	switch (mode) {
		size_t	siz;

	case ICONV_TRANSLIT:
		siz = strlen(to) + strlen("//TRANSLIT") + 1;
		tocode = xcalloc(siz, sizeof(char));
		snprintf(tocode, siz, "%s//TRANSLIT", to);
		break;
	case ICONV_IGNORE:
		siz = strlen(to) + strlen("//IGNORE") + 1;
		tocode = xcalloc(siz, sizeof(char));
		snprintf(tocode, siz, "%s//IGNORE", to);
		break;
	case ICONV_REPLACE:
		/* FALLTHROUGH */
	default:
		tocode = xstrdup(to);
		break;
	}

	if ((cd = iconv_open(tocode, from)) == (iconv_t)-1 &&
	    (cd = iconv_open("", from)) == (iconv_t)-1 &&
	    (cd = iconv_open(tocode, "")) == (iconv_t)-1) {
		xfree(tocode);
		printf("%s: iconv_open(): %s\n", __progname, strerror(errno));
		return (xstrdup(in_str));
	}

	ip = input = (ICONV_CONST char *)in_str;
	input_len = strlen(input);
	output_size = 1;
	output = xcalloc(output_size, sizeof(char));
	out_pos = 0;
	output[out_pos] = '\0';
	while (input_len > 0) {
		char	*op;
		size_t	 count;

		buf[0] = '\0';
		bp = buf;
		bufavail = sizeof(buf) - 1;

		if (iconv(cd, &ip, &input_len, &bp, &bufavail) == (size_t)-1 &&
		    errno != E2BIG) {
			*bp++ = '?';
			ip++;
			input_len--;
			bufavail--;
		}
		*bp = '\0';

		count = sizeof(buf) - bufavail - 1;

		output_size += count;
		op = output = xrealloc(output, output_size, sizeof(char));
		op += out_pos;
		memcpy(op, buf, count);
		out_pos += count;
		op += count;
		*op = '\0';
	}

	if (iconv_close(cd) == -1) {
		printf("%s: iconv_close(): %s\n", __progname, strerror(errno));
		xfree(output);
		xfree(tocode);
		return (xstrdup(in_str));
	}

	xfree(tocode);
	return (output);
#else
	(void)from;
	(void)to;
	(void)mode;

	if (NULL == in_str)
		return (xstrdup(""));

	return (xstrdup(in_str));
#endif /* HAVE_ICONV */
}

int
ez_gettimeofday(void *tp_arg)
{
	struct timeval	*tp = (struct timeval *)tp_arg;
	int		 ret = -1;

#ifdef HAVE_GETTIMEOFDAY
	ret = gettimeofday(tp, NULL);
#else /* HAVE_GETTIMEOFDAY */
# ifdef WIN32
	/*
	 * Idea for this way of implementing gettimeofday()-like functionality
	 * on Windows taken from cURL, (C) 1998 - 2007 Daniel Steinberg, et al.
	 * http://curl.haxx.se/docs/copyright.html
	 */
	SYSTEMTIME	 st;
	struct tm	 tm;

	GetLocalTime(&st);
	tm.tm_sec = st.wSecond;
	tm.tm_min = st.wMinute;
	tm.tm_hour = st.wHour;
	tm.tm_mday = st.wDay;
	tm.tm_mon = st.wMonth - 1;
	tm.tm_year = st.wYear - 1900;
	tm.tm_isdst = -1;
	tp->tv_sec = (long)mktime(&tm);
	tp->tv_usec = st.wMilliseconds * 1000;
	ret = 0;
# else /* WIN32 */
	/* Fallback to time(): */
	tp->tv_sec = (long)time(NULL);
	tp->tv_usec = 0;
	ret = 0;
# endif /* WIN32 */
#endif /* HAVE_GETTIMEOFDAY */

	return (ret);
}
