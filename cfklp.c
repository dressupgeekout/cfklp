/*
 * cfklp -- Christian Koch <cfkoch@sdf.lonestar.org>
 *
 * TODO:
 *    - supply individual margins
 *    - paper type shortcuts ("letter" == 8.5"x11", etc.)
 *    - write man page
 *    - automatic page breaks
 *    - force page break on form feed
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <libps/pslib.h>

#define PPI 72

#define DEFAULT_LINEBREAK      true
#define DEFAULT_PARBREAK       false
#define DEFAULT_FONT_SIZE      10
#define DEFAULT_LEADING        DEFAULT_FONT_SIZE
#define DEFAULT_FONT           "fonts/Courier"
#define DEFAULT_NUMINDENTLINES 1
#define DEFAULT_PARINDENT      0.0
#define DEFAULT_JUSTIFICATION  "left"
#define DEFAULT_PAGE_WIDTH     in(8.5)
#define DEFAULT_PAGE_HEIGHT    in(11)
#define DEFAULT_MARGIN         in(1)

static float
in(float x)
{
  return x * PPI;
}


static char*
bool_to_str(bool b)
{
  return b ? "true" : "false";
}

/* */

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
void cfklp_set_font(struct cfklp* c, char* font);
void cfklp_set_font_size(struct cfklp* c, float font_size);
void cfklp_set_justification(struct cfklp* c, char* j);
void cfklp_set_page_size(struct cfklp* c, float w, float h, float m);
void cfklp_read_infile(struct cfklp* c);
void cfklp_write_outfile(struct cfklp* c);

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
  size_t infile_sz;

  infile = fopen(c->infile_name, "r");
  fseek(infile, 0, SEEK_END);
  infile_sz = ftell(infile);
  rewind(infile);
  c->infile_s = malloc(infile_sz);
  fread(c->infile_s, sizeof(char), infile_sz, infile);
  fclose(infile);
}


/*
 * XXX Currently defaults to 1" all-around margins. Should be settable.
 */
void
cfklp_write_outfile(struct cfklp* c)
{
  int font_id;

  PS_open_file(c->doc, c->outfile_name);
  font_id = PS_findfont(c->doc, c->font, NULL, 0);

  PS_begin_page(c->doc, c->page_width, c->page_height);
  PS_setfont(c->doc, font_id, c->font_size);

  PS_show_boxed(
    c->doc, c->infile_s,
    c->margin, c->margin,
    (c->page_width - (2 * c->margin)),
    (c->page_height - (2 * c->margin)),
    c->justification, ""
  );

  PS_end_page(c->doc);

  PS_close(c->doc);
}

static void
cfklp_usage(void)
{
  printf(
    "usage: %s [-LP] [-H page-height] [-W page-width] [-f font]\n"
    "\t[-j justification] [-l leading] [-m margin] [-n numindentlines]\n"
    "\t[-p parindent] [-s font-size] infile outfile\n",
    getprogname()
  );
}

/* */

int
main(int argc, char* argv[])
{
  struct cfklp* cfklp;
  char *infile_name, *outfile_name;

  char *font, *justification;
  float leading, font_size, parindent; 
  float page_width, page_height, margin;
  int numindentlines;
  bool linebreak, parbreak; 

  int ch;

  /* */

  if (argc == 1) {
    cfklp_usage();
    return EXIT_SUCCESS;
  }

  linebreak      = DEFAULT_LINEBREAK;
  parbreak       = DEFAULT_PARBREAK;
  font_size      = DEFAULT_FONT_SIZE;
  leading        = DEFAULT_LEADING;
  font           = DEFAULT_FONT;
  numindentlines = DEFAULT_NUMINDENTLINES;
  parindent      = DEFAULT_PARINDENT;
  justification  = DEFAULT_JUSTIFICATION;
  page_width     = DEFAULT_PAGE_WIDTH;
  page_height    = DEFAULT_PAGE_HEIGHT;
  margin         = DEFAULT_MARGIN;

  while ((ch = getopt(argc, argv, "H:LPW:f:j:l:m:n:p:s:")) != -1) {
    switch (ch) {
    case 'H':
      page_height = in(atof(optarg));
      break;
    case 'L':
      linebreak = !linebreak;
      break;
    case 'P':
      parbreak = !parbreak;
      break;
    case 'W':
      page_width = in(atof(optarg));
      break;
    case 'f':
      font = optarg;
      break;
    case 'j':
      justification = optarg;
      break;
    case 'l':
      leading = atof(optarg);
      break;
    case 'm':
      margin = in(atof(optarg));
      break;
    case 'n':
      numindentlines = atoi(optarg);
      break;
    case 'p':
      parindent = atof(optarg);
      break;
    case 's':
      font_size = atof(optarg);
      break;
    case '?':
    default:
      cfklp_usage();
      return EXIT_FAILURE;
    }
  }
  argc -= optind;
  argv += optind;

  /* */

  if (argc != 2) {
    fprintf(stderr, "%s: need an infile and an outfile\n", getprogname());
    return EXIT_FAILURE;
  }

  infile_name = argv[0];
  outfile_name = argv[1];

  PS_boot();

  cfklp = cfklp_new(infile_name, outfile_name);

  cfklp_set_font(cfklp, font);
  cfklp_set_font_size(cfklp, font_size);
  cfklp_set_justification(cfklp, justification);
  cfklp_set_page_size(cfklp, page_width, page_height, margin);
  PS_set_value(cfklp->doc, "leading", leading);
  PS_set_value(cfklp->doc, "numindentlines", numindentlines);
  PS_set_value(cfklp->doc, "parindent", parindent);
  PS_set_parameter(cfklp->doc, "linebreak", bool_to_str(linebreak));
  PS_set_parameter(cfklp->doc, "parbreak", bool_to_str(parbreak));

  cfklp_read_infile(cfklp);
  cfklp_write_outfile(cfklp);
  cfklp_free(cfklp);

  PS_shutdown();

  return EXIT_SUCCESS;
}
