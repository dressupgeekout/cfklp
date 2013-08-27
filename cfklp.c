/*
 * cfklp -- cfklp.c
 * Christian Koch <cfkoch@sdf.lonestar.org>
 */

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include <libps/pslib.h>

#include "cfklp.h"

struct cfklp*
cfklp_new(char* infile_name, char* outfile_name)
{
  struct cfklp* c;
  c = malloc(sizeof(struct cfklp));
  c->doc = PS_new();
  c->infile_name = infile_name;
  c->outfile_name = outfile_name;
  return c;
}


void
cfklp_free(struct cfklp* c)
{
  PS_delete(c->doc);
  free(c);
}


void
cfklp_read_infile(struct cfklp* c)
{
  FILE* infile;
  struct stat sb;

  infile = fopen(c->infile_name, "r");
  if (infile == NULL) err(EXIT_FAILURE, "%s", c->infile_name);
  stat(c->infile_name, &sb);
  c->infile_s = malloc(sb.st_size);
  fread(c->infile_s, sizeof(char), sb.st_size, infile);
  fclose(infile);
}


void
cfklp_write_outfile(struct cfklp* c)
{
  int font_id;
  int couldnt_write;

  PS_open_file(c->doc, c->outfile_name);
  font_id = PS_findfont(c->doc, c->font, NULL, 0);

  PS_begin_page(c->doc, c->page_width, c->page_height);
  PS_setfont(c->doc, font_id, c->font_size);

  couldnt_write = PS_show_boxed(
    c->doc, c->infile_s,
    c->margin, c->margin,
    (c->page_width - (2 * c->margin)),
    (c->page_height - (2 * c->margin)),
    c->justification, ""
  );

  PS_end_page(c->doc);
  PS_close(c->doc);

  if (couldnt_write > 0) {
    fprintf(
      stderr, "%s: note: couldn't fit %d characters on this page\n",
      getprogname(), couldnt_write
    );
  }
}


void
cfklp_set_font(struct cfklp* c, char* font)
{
  c->font = font;
}


void
cfklp_set_font_size(struct cfklp* c, float font_size)
{
  c->font_size = font_size;
}


void
cfklp_set_justification(struct cfklp* c, char* j)
{
  c->justification = j;
}


void
cfklp_set_page_size(struct cfklp* c, float w, float h, float m)
{
  c->page_width = w;
  c->page_height = h;
  c->margin = m;
}
