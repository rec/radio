/*	$Id: metadata.c 15797 2009-03-18 20:15:24Z moritz $	*/
/*
 * Copyright (c) 2007, 2009 Moritz Grimm <mdgrimm@gmx.net>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "ezstream.h"

#ifdef HAVE_TAGLIB
# include <taglib/tag_c.h>
#endif /* HAVE_TAGLIB */
#ifdef HAVE_VORBISFILE
# include <vorbis/vorbisfile.h>
#endif /* HAVE_VORBISFILE */
#include <shout/shout.h>

#include "metadata.h"
#include "strfctns.h"
#include "util.h"
#include "xalloc.h"

/* Usually defined in <sys/stat.h>. */
#ifndef S_IEXEC
# define S_IEXEC	S_IXUSR
#endif /* !S_IEXEC */

extern char		*__progname;
extern int		 vFlag;

static const char	*blankString = "";

struct metadata {
	char	*filename;
	char	*string;
	char	*artist;
	char	*title;
	int	 songLen;
	int	 normalize;
	int	 program;
};

struct ID3Tag {
	char tag[3];
	char trackName[30];
	char artistName[30];
	char albumName[30];
	char year[4];
	char comment[30];
	char genre;
};

metadata_t *	metadata_create(const char *);
void		metadata_use_taglib(metadata_t *, FILE **);
void		metadata_use_self(metadata_t *, FILE **);
void    	metadata_clean_md(metadata_t *);
void		metadata_get_extension(char *, size_t, const char *);
char *		metadata_get_name(const char *);
void		metadata_process_md(metadata_t *);
void		metadata_normalize_string(char **);

metadata_t *
metadata_create(const char *filename)
{
	metadata_t	*md;

	md = xcalloc(1UL, sizeof(metadata_t));
	md->filename = xstrdup(filename);
	md->songLen = -1;

	return (md);
}

void
metadata_use_taglib(metadata_t *md, FILE **filep)
#ifdef HAVE_TAGLIB
{
	TagLib_File			*tf;
	TagLib_Tag			*tt;
	const TagLib_AudioProperties	*ta;
	char				*str;
  char        *str2;
  int len;

	if (md == NULL || md->filename == NULL) {
		printf("%s: metadata_use_taglib(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	if (filep != NULL)
		fclose(*filep);

	metadata_clean_md(md);
	taglib_set_string_management_enabled(0);
#ifdef HAVE_ICONV
	taglib_set_strings_unicode(1);
#else
	taglib_set_strings_unicode(0);
#endif /* HAVE_ICONV */

	if (md->string != NULL) {
		xfree(md->string);
		md->string = NULL;
	}

	if ((tf = taglib_file_new(md->filename)) == NULL) {
		md->string = metadata_get_name(md->filename);
		return;
	}

	tt = taglib_file_tag(tf);
	ta = taglib_file_audioproperties(tf);

	str = taglib_tag_artist(tt);

  printf("artist: %s, %s\n", str, str2);
	if (str != NULL) {
    str2 = taglib_tag_album(tt);
    if (str2 != NULL) {
      const char* p;
      len = strlen(str) + strlen(str2) + 4;
      md->artist = xmalloc(len);
      snprintf(md->artist, len, "%s - %s\n", str, str2);
      free(str2);
    } else {
      if (strlen(str) > 0)
        md->artist = xstrdup(str);
    }
		free(str);
	}

	str = taglib_tag_title(tt);
  printf("title: %s\n", str);
	if (str != NULL) {
		if (strlen(str) > 0)
			md->title = xstrdup(str);
		free(str);
	}

	md->songLen = taglib_audioproperties_length(ta);

	taglib_file_free(tf);
}
#else
{
	(void)md;
	(void)filep;

	printf("%s: Internal error: metadata_use_taglib() called without TagLib support\n",
	       __progname);
	abort();
}
#endif /* HAVE_TAGLIB */

void
metadata_use_self(metadata_t *md, FILE **filep)
#ifdef HAVE_TAGLIB
{
	(void)md;
	(void)filep;

	printf("%s: Internal error: metadata_use_self() called with TagLib support\n",
	       __progname);
	abort();
}
#else
{
	char		extension[25];
	struct ID3Tag	id3tag;

	if (md == NULL || filep == NULL || *filep == NULL ||
	    md->filename == NULL) {
		printf("%s: metadata_use_self(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	metadata_clean_md(md);
	metadata_get_extension(extension, sizeof(extension), md->filename);

	if (strcmp(extension, ".mp3") == 0) {
		memset(&id3tag, 0, sizeof(id3tag));
		fseek(*filep, -128L, SEEK_END);
		fread(&id3tag, 1UL, sizeof(struct ID3Tag), *filep);
		if (memcmp(id3tag.tag, "TAG", 3UL) == 0) {
			if (strlen(id3tag.artistName) > 0)
				md->artist = CHARtoUTF8(id3tag.artistName, ICONV_REPLACE);
			if (strlen(id3tag.trackName) > 0)
				md->title = CHARtoUTF8(id3tag.trackName, ICONV_REPLACE);
		}
#ifdef HAVE_VORBISFILE
	} else if (strcmp(extension, ".ogg") == 0) {
		OggVorbis_File	vf;
		int		ret;

		if ((ret = ov_open(*filep, &vf, NULL, 0L)) != 0) {
			switch (ret) {
			case OV_EREAD:
				printf("%s: ov_open(): %s: Media read error\n",
				       __progname, md->filename);
				break;
			case OV_ENOTVORBIS:
				printf("%s: ov_open(): %s: Invalid Vorbis bitstream\n",
				       __progname, md->filename);
				break;
			case OV_EVERSION:
				printf("%s: ov_open(): %s: Vorbis version mismatch\n",
				       __progname, md->filename);
				break;
			case OV_EBADHEADER:
				printf("%s: ov_open(): %s: Invalid Vorbis bitstream header\n",
				       __progname, md->filename);
				break;
			case OV_EFAULT:
				printf("%s: Fatal: Internal libvorbisfile fault\n",
				       __progname);
				abort();
			default:
				printf("%s: ov_open(): %s: ov_read() returned unknown error\n",
				       __progname, md->filename);
				break;
			}
		} else {
			char	**ptr;

			for (ptr = ov_comment(&vf, -1)->user_comments; *ptr != NULL; ptr++) {
				if (md->artist == NULL &&
				    strncasecmp(*ptr, "ARTIST", strlen("ARTIST")) == 0) {
					if (strlen(*ptr + strlen("ARTIST=")) > 0)
						md->artist = xstrdup(*ptr + strlen("ARTIST="));
				}
				if (md->title == NULL &&
				    strncasecmp(*ptr, "TITLE", strlen("TITLE")) == 0) {
					if (strlen(*ptr + strlen("TITLE=")) > 0)
						md->title = xstrdup(*ptr + strlen("TITLE="));
				}
			}

			ov_clear(&vf);
			*filep = NULL;
		}
#endif /* HAVE_VORBISFILE */
	}

	if (*filep != NULL)
		fclose(*filep);

	if (md->artist == NULL && md->title == NULL)
		md->string = metadata_get_name(md->filename);
}
#endif /* HAVE_TAGLIB */

void
metadata_clean_md(metadata_t *md)
{
	if (md == NULL) {
		printf("%s: Internal error: metadata_clean_md(): NULL argument\n",
		       __progname);
		abort();
	}

	if (md->string != NULL) {
		xfree(md->string);
		md->string = NULL;
	}
	if (md->artist != NULL) {
		xfree(md->artist);
		md->artist = NULL;
	}
	if (md->title != NULL) {
		xfree(md->title);
		md->title = NULL;
	}
}

void
metadata_get_extension(char *buf, size_t siz, const char *filename)
{
	char	 *p;

	if (buf == NULL || siz == 0 || filename == NULL) {
		printf("%s: metadata_get_extension(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	if ((p = strrchr(filename, '.')) != NULL)
		strlcpy(buf, p, siz);
	else
		buf[0] = '\0';
	for (p = buf; *p != '\0'; p++)
		*p = tolower((int)*p);
}

char *
metadata_get_name(const char *file)
{
	char	*filename = xstrdup(file);
	char	*p1, *p2, *name;

	if (file == NULL) {
		printf("%s: metadata_get_name(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	if ((p1 = local_basename(filename)) == NULL) {
		printf("%s: Internal error: basename() failed with '%s'\n",
		       __progname, filename);
		exit(1);
	}

	if ((p2 = strrchr(p1, '.')) != NULL)
		*p2 = '\0';

	if (strlen(p1) == 0)
		name = xstrdup("[unknown]");
	else
		name = CHARtoUTF8(p1, ICONV_REPLACE);

	xfree(filename);
	return (name);
}

void
metadata_process_md(metadata_t *md)
{
	if (md == NULL) {
		printf("%s: metadata_process_md(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	if (md->string == NULL)
		md->string = metadata_assemble_string(md);

	if (md->normalize) {
		metadata_normalize_string(&md->string);
		metadata_normalize_string(&md->artist);
		metadata_normalize_string(&md->title);
	}
}

void
metadata_normalize_string(char **s)
{
	char	*str, *cp, *tmpstr, *tp;
	int	 is_space;

	if (s == NULL || (str = *s) == NULL || strlen(str) == 0)
		return;

	tmpstr = xcalloc(strlen(str) + 1, sizeof(char));

	tp = tmpstr;
	is_space = 1;
	for (cp = str; *cp != '\0'; cp++) {
		if (*cp == ' ') {
			if (!is_space && strlen(tmpstr) > 0 &&
			    tmpstr[strlen(tmpstr) - 1] != ' ')
				*tp++ = ' ';
			is_space = 1;
		} else {
			*tp++ = *cp;
			is_space = 0;
		}
	}
	if (strlen(tmpstr) > 0 && tmpstr[strlen(tmpstr) - 1] == ' ')
		tmpstr[strlen(tmpstr) - 1] = '\0';

	xfree(str);
	*s = xrealloc(tmpstr, strlen(tmpstr) + 1, sizeof (char));
}

metadata_t *
metadata_file(const char *filename, int normalize)
{
	metadata_t	*md;

	if (filename == NULL || strlen(filename) == 0) {
		printf("%s: metadata_file(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	md = metadata_create(filename);
	if (!metadata_file_update(md)) {
		metadata_free(&md);
		return (NULL);
	}

	md->normalize = normalize;

	return (md);
}

metadata_t *
metadata_program(const char *program, int normalize)
{
	metadata_t	*md;
#ifdef HAVE_STAT
	struct stat	 st;
#else
	FILE		*filep;
#endif

	if (program == NULL || strlen(program) == 0) {
		printf("%s: metadata_program(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	md = metadata_create(program);
	md->program = 1;
	md->string = xstrdup("");

#ifdef HAVE_STAT
	if (stat(program, &st) == -1) {
		printf("%s: %s: %s\n", __progname, program, strerror(errno));
		metadata_free(&md);
		return (NULL);
	}
	if (st.st_mode & (S_IWGRP | S_IWOTH)) {
		printf("%s: Error: %s is group and/or world writeable\n",
		       __progname, program);
		metadata_free(&md);
		return (NULL);
	}
	if (!(st.st_mode & (S_IEXEC | S_IXGRP | S_IXOTH))) {
		printf("%s: %s: Not an executable program\n", __progname, program);
		metadata_free(&md);
		return (NULL);
	}
#else
	if ((filep = fopen(program, "r")) == NULL) {
		printf("%s: %s: %s\n", __progname, program, strerror(errno));
		metadata_free(&md);
		return (NULL);
	}
	fclose(filep);
#endif /* HAVE_STAT */

	md->normalize = normalize;

	return (md);
}

void
metadata_free(metadata_t **md_p)
{
	metadata_t	*md;

	if (md_p == NULL || (md = *md_p) == NULL)
		return;

	if (md->filename != NULL) {
		xfree(md->filename);
		md->filename = NULL;
	}
	metadata_clean_md(md);
	xfree(*md_p);
	*md_p = NULL;
}


int
metadata_file_update(metadata_t *md)
{
  printf("metadata_file_update\n");
	FILE	*filep;

	if (md == NULL) {
		printf("%s: metadata_file_update(): Internal error: NULL argument\n",
		       __progname);
		abort();
	}

	if (md->program) {
		printf("%s: metadata_file_update(): Internal error: Called with program handle\n",
		       __progname);
		abort();
	}

	if ((filep = fopen(md->filename, "rb")) == NULL) {
		printf("%s: %s: %s\n", __progname, md->filename, strerror(errno));
		return (0);
	}

#ifdef HAVE_TAGLIB
	metadata_use_taglib(md, &filep);
#else
	metadata_use_self(md, &filep);
#endif /* HAVE_TAGLIB */

	metadata_process_md(md);

	return (1);
}

int
metadata_program_update(metadata_t *md, enum metadata_request md_req)
{
	FILE	*filep;
	char	 buf[METADATA_MAX + 1];
	char	 command[PATH_MAX + sizeof(" artist")];

	if (md == NULL) {
		printf("%s: metadata_program_update(): Internal error: NULL argument\n",
		       __progname);
		abort();
	}

	if (!md->program) {
		printf("%s: metadata_program_update(): Internal error: Received file handle\n",
		       __progname);
		abort();
	}

	switch (md_req) {
	case METADATA_ALL:
		metadata_clean_md(md);
		if (!metadata_program_update(md, METADATA_STRING) ||
		    !metadata_program_update(md, METADATA_ARTIST) ||
		    !metadata_program_update(md, METADATA_TITLE))
			return (0);
		else
			return (1);
	case METADATA_STRING:
		strlcpy(command, md->filename, sizeof(command));
		if (md->string != NULL) {
			xfree(md->string);
			md->string = NULL;
		}
		break;
	case METADATA_ARTIST:
		snprintf(command, sizeof(command), "%s artist", md->filename);
		if (md->artist != NULL) {
			xfree(md->artist);
			md->artist = NULL;
		}
		break;
	case METADATA_TITLE:
		snprintf(command, sizeof(command), "%s title", md->filename);
		if (md->title != NULL) {
			xfree(md->title);
			md->title = NULL;
		}
		break;
	default:
		printf("%s: metadata_program_update(): Internal error: Unknown md_req\n",
		       __progname);
		abort();
	}

	fflush(NULL);
	errno = 0;
	if (vFlag > 1)
		printf("%s: Running command `%s`\n", __progname,
		       command);
	if ((filep = popen(command, "r")) == NULL) {
		printf("%s: playlist_run_program(): Error while executing '%s'",
		       __progname, command);
		/* popen() does not set errno reliably ... */
		if (errno)
			printf(": %s\n", strerror(errno));
		else
			printf("\n");
		return (0);
	}

	if (fgets(buf, (int)sizeof(buf), filep) == NULL) {
		if (ferror(filep))
			printf("%s: Error while reading output from program '%s': %s\n",
			       __progname, md->filename, strerror(errno));
		pclose(filep);
		printf("%s: FATAL: External program '%s' not (or no longer) usable.\n",
		       __progname, md->filename);
		exit(1);
	}

	pclose(filep);

	if (strlen(buf) == sizeof(buf) - 1)
		printf("%s: Warning: Metadata string received via '%s' is too long and has been truncated\n",
		       __progname, command);

	buf[strcspn(buf, "\n")] = '\0';
	buf[strcspn(buf, "\r")] = '\0';

	switch (md_req) {
	case METADATA_STRING:
		if (strlen(buf) == 0) {
			printf("%s: Warning: Empty metadata string received from '%s'\n",
			       __progname, md->filename);
			md->string = xstrdup("");
		} else
			md->string = xstrdup(buf);
		break;
	case METADATA_ARTIST:
		if (strlen(buf) > 0)
			md->artist = xstrdup(buf);
		break;
	case METADATA_TITLE:
		if (strlen(buf) > 0)
			md->title = xstrdup(buf);
		break;
	case METADATA_ALL:
	default:
		printf("%s: metadata_program_update(): Internal error: METADATA_ALL in code unreachable by METADATA_ALL\n",
		       __progname);
		abort();
	}

	if (md->normalize) {
		metadata_normalize_string(&md->string);
		metadata_normalize_string(&md->artist);
		metadata_normalize_string(&md->title);
	}

	return (1);
}

const char *
metadata_get_string(metadata_t *md)
{
	if (md == NULL) {
		printf("%s: metadata_get_string(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	if (md->string == NULL) {
		printf("%s: metadata_get_string(): Internal error: md->string cannot be NULL\n",
		       __progname);
		abort();
	}

	return (md->string);
}

const char *
metadata_get_artist(metadata_t *md)
{
	if (md == NULL) {
		printf("%s: metadata_get_artist(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	if (md->artist == NULL)
		return (blankString);
	else
		return (md->artist);
}

const char *
metadata_get_title(metadata_t *md)
{
	if (md == NULL) {
		printf("%s: metadata_get_title(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	if (md->title == NULL)
		return (blankString);
	else
		return (md->title);
}

const char *
metadata_get_filename(metadata_t *md)
{
	if (md == NULL) {
		printf("%s: metadata_get_filename(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	if (md->filename == NULL)
		/* Should never happen: */
		return (blankString);
	else
		return (md->filename);
}

int
metadata_get_length(metadata_t *md)
{
	if (md == NULL) {
		printf("%s: metadata_get_length(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	return (md->songLen);
}

char *
metadata_assemble_string(metadata_t *md)
{
	size_t	  len;
	char	 *str;

	if (md == NULL) {
		printf("%s: metadata_assemble_string(): Internal error: Bad arguments\n",
		       __progname);
		abort();
	}

	if (md->artist == NULL && md->title == NULL && md->program == 0)
		return (metadata_get_name(md->filename));

	len = 0;
	if (md->artist != NULL)
		len += strlen(md->artist);
	if (md->title != NULL) {
		if (len > 0)
			len += strlen(" - ");
		len += strlen(md->title);
	}
	len++;
	str = xcalloc(len, sizeof(char));

	if (md->artist != NULL)
		strlcpy(str, md->artist, len);
	if (md->title != NULL) {
		if (md->artist != NULL)
			strlcat(str, " - ", len);
		strlcat(str, md->title, len);
	}

	return (str);
}
