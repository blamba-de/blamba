
#define VERSION "3.00alpha5"
#define DATE_RELEASED "April 2, 2006"
#define VOLUME 7
#define SMS_SIZE 140
#define RINGSTACK_SIZE 8192
#define COPYRIGHT "Copyright 2001-2006 - Michael Kohn (mike@mikekohn.net)\n\n" \
                  "THIS MAY NOT BE USED IN COMMERCIAL ENVIRONMENTS WITHOUT\n" \
                  "PERMISSION OF THE AUTHOR.  PLEASE READ THE LICENSE.\n\n"

#define SONGNAME_LEN 64
#define MAX_WIDTH 640
#define MAX_HEIGHT 480
#define COLOR_THRESHOLD 50

#define RTT_TYPE_UNDEFINED 0
#define RTT_TYPE_RTTTL 1
#define RTT_TYPE_RTX 2
#define RTT_TYPE_EMELODY 3
#define RTT_TYPE_IMELODY 4
#define RTT_TYPE_MIDI 5
#define RTT_TYPE_KWS 6             /* Kyocera Keypress */
#define RTT_TYPE_SEO 7             /* Siemens */
#define RTT_TYPE_BMP 8 
#define RTT_TYPE_NOKIA3210 9       /* Nokia 3210 Keypress */
#define RTT_TYPE_SCKL 10
#define RTT_TYPE_MORSECODE 11 
#define RTT_TYPE_EMS 12 
#define RTT_TYPE_NOL 13            /* Nokia Operator Logo */ 
#define RTT_TYPE_NGG 14            /* Nokia Graphic */
#define RTT_TYPE_WBMP 15
#define RTT_TYPE_ICO 16            /* Windows ICO File */
#define RTT_TYPE_GIF 17
#define RTT_TYPE_SAMSUNG1 18
#define RTT_TYPE_SAMSUNG2 19
#define RTT_TYPE_PDB 20
#define RTT_TYPE_WAV 21
#define RTT_TYPE_MOT 22            /* Motorola Monophonic */
#define RTT_TYPE_MOT_KEYPRESS 23   /* Motorola Keypress */
#define RTT_TYPE_OTT 24
#define RTT_TYPE_RNG 24
#define RTT_TYPE_DSP 25
#define RTT_TYPE_TEXT 26

#define RTT_NO_ERROR 0
#define RTT_UNSUPPORTED_OUTTYPE -10
#define RTT_UNSUPPORTED_INTYPE -11

#define RTT_HEADERS_NONE 0
#define RTT_HEADERS_DEFAULT 1
#define RTT_HEADERS_LESS 2
#define RTT_HEADERS_MOST 3
#define RTT_HEADERS_STD 4
#define RTT_HEADERS_FULL 5

struct rtt_info_t
{
  char inname[1024];
  char outname[1024];
  char songname[SONGNAME_LEN];
  char channels;
  int sample_rate;
  unsigned int samples_per_beat;
  int bpm;
  int pause;
  int bytes;
  int scale;
  int prev_scale;
  int tone;
  int length;
  int modifier;
  int style;
  int transpose;
  int flats;
  int volume;
  int a440,ats;
  int width,height,mcc,mnc,bmp_flags;
  int note_count;
  int marker;
  int ems;
  int trans;
  unsigned char loop;
  int *picture;
  unsigned char *message;
  unsigned char *wav_buffer;
  short int ring_stack[RINGSTACK_SIZE];
  int stackptr;
  int bitptr;
  int headers;
  int full_sms_size;
  int out_type;
  int in_type;
};

extern int quiet;

/* API CALLS */

int rtt_info_alloc();
int rtt_init_info(struct rtt_info_t *rtt_info);
int rtt_info_free(struct rtt_info_t *rtt_info);

int rtt_set_filenames(struct rtt_info_t *rtt_info, char *input_filename, char *output_filename);
int rtt_add_extension_to_file(struct rtt_info_t *rtt_info);
int rtt_get_outtype(struct rtt_info_t *rtt_info, char *param);
int rtt_get_intype(struct rtt_info_t *rtt_info, char *param);

int rtt_set_logo_dims(struct rtt_info_t *rtt_info, int width, int height);
int rtt_set_types(struct rtt_info_t *rtt_info, int in_type, int out_type);
int rtt_set_tempo(struct rtt_info_t *rtt_info, int bpm);
int rtt_set_name(struct rtt_info_t *rtt_info, char *songname);
int rtt_set_pause(struct rtt_info_t *rtt_info, int pause);
int rtt_set_wav_options(struct rtt_info_t *rtt_info, int bits_per_sample, int sample_rate);
int rtt_set_mcc_mnc(struct rtt_info_t *rtt_info, int mcc, int mnc);
int rtt_transpose(struct rtt_info_t *rtt_info, int transpose);
int rtt_set_sms_size(struct rtt_info_t *rtt_info, int sms_size);
int rtt_set_volume(struct rtt_info_t *rtt_info, int volume);
int rtt_set_midi_options(struct rtt_info_t *rtt_info, int channel, int track);
int rtt_set_header_options(struct rtt_info_t *rtt_info, int header_option);
int rtt_convert(struct rtt_info_t *rtt_info);
int rtt_get_version(char *version_str);

/* OTHER CALLS */

int reverse_tempo(int l);
int get_tempo(int tempo);
int push(int data, int size, struct rtt_info_t *rtt_info);
/* int push_addr(int data, int size, int stackptr, int bitptr); */
int push_addr(int data, int size, struct rtt_info_t *rtt_info);
void print_hex(int t);
void write_codes(FILE *out, char *port, struct rtt_info_t *rtt_info);

void check_songname(FILE *in, char *songname);
int gettoken(FILE *fp, char *token, int flag);
int convcolor(char *s);

int color_distance(int c1, int c2);
int get_color(int col, int *palette, int palette_count);


