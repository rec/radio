/*	$Id: configfile.h 16319 2009-07-22 20:19:38Z moritz $	*/
/*
 *  ezstream - source client for Icecast with external en-/decoder support
 *  Copyright (C) 2003, 2004, 2005, 2006  Ed Zaleski <oddsock@oddsock.org>
 *  Copyright (C) 2007                    Moritz Grimm <mdgrimm@gmx.net>
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

#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

#include <libxml/parser.h>

#define MP3_FORMAT		"MP3"
#define VORBIS_FORMAT		"VORBIS"
#define THEORA_FORMAT		"THEORA"

#define MAX_FORMAT_ENCDEC	15

#define TRACK_PLACEHOLDER	"@T@"
#define METADATA_PLACEHOLDER	"@M@"
#define ARTIST_PLACEHOLDER	"@a@"
#define TITLE_PLACEHOLDER	"@t@"
#define STRING_PLACEHOLDER	"@s@"

typedef struct tag_FORMAT_ENCDEC {
	char	*format;
	char	*match;
	char	*encoder;
	char	*decoder;
} FORMAT_ENCDEC;

typedef struct tag_EZCONFIG {
	char		*URL;
	char		*username;
	char		*password;
	char		*format;
	char		*fileName;
	char		*metadataProgram;
	char		*metadataFormat;
	char		*serverName;
	char		*serverURL;
	char		*serverGenre;
	char		*serverDescription;
	char		*serverBitrate;
	char		*serverChannels;
	char		*serverSamplerate;
	char		*serverQuality;
	int		 serverPublic;
	int		 reencode;
	FORMAT_ENCDEC	*encoderDecoders[MAX_FORMAT_ENCDEC];
	int		 numEncoderDecoders;
	int		 shuffle;
	int		 fileNameIsProgram;
	int		 streamOnce;
	unsigned int	 reconnectAttempts;
} EZCONFIG;

EZCONFIG *	getEZConfig(void);
const char *	getFormatEncoder(const char *format);
const char *	getFormatDecoder(const char *match);
int		parseConfig(const char *fileName);
void		freeConfig(EZCONFIG *);

#endif /* __CONFIGFILE_H__ */
