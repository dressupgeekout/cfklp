/*
 * cfklp -- cfklp.h
 * Christian Koch <cfkoch@sdf.lonestar.org>
 */

#ifndef CFKLP_H
#define CFKLP_H

#include <libps/pslib.h>

struct cfklp {
  PSDoc* doc;
  char* infile_name;
  char* outfile_name;
  char* infile_s;
  char* font;
  char* justification;
  float font_size;
  float page_width;
  float page_height;
  float margin;
};

struct cfklp* cfklp_new(char* infile_name, char* outfile_name);
void cfklp_free(struct cfklp* c);

void cfklp_read_infile(struct cfklp* c);
void cfklp_write_outfile(struct cfklp* c);

void cfklp_set_font(struct cfklp* c, char* font);
void cfklp_set_font_size(struct cfklp* c, float font_size);
void cfklp_set_justification(struct cfklp* c, char* j);
void cfklp_set_page_size(struct cfklp* c, float w, float h, float m);

#endif /* CFKLP_H */
