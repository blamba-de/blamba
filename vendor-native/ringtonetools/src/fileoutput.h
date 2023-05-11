
int write_long(FILE *out, int n);
int write_word(FILE *out, int n);
int read_long(FILE *in);
int read_word(FILE *in);
int write_long_b(FILE *out, int n);
int write_word_b(FILE *out, int n);
int read_long_b(FILE *in);
int read_word_b(FILE *in);
int read_chars(FILE *in, char *s, int count);
int write_chars(FILE *out, char *s);

void write_wav_header(FILE *out, struct rtt_info_t *rtt_info);
void write_wav_note(FILE *out, struct rtt_info_t *rtt_info);
void write_wav_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_dsp_header(FILE *out, struct rtt_info_t *rtt_info);
void write_dsp_note(FILE *out, struct rtt_info_t *rtt_info);
void write_dsp_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_kws_header(FILE *out, struct rtt_info_t *rtt_info);
void write_kws_note(FILE *out, struct rtt_info_t *rtt_info);
void write_kws_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_mot_header(FILE *out, struct rtt_info_t *rtt_info);
void write_mot_note(FILE *out, struct rtt_info_t *rtt_info);
void write_mot_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_rtttl_header(FILE *out, struct rtt_info_t *rtt_info);
void write_rtttl_note(FILE *out, struct rtt_info_t *rtt_info);
void write_rtttl_footer(FILE *out, struct rtt_info_t *rtt_info);
void write_rtx_bpm(FILE *out, struct rtt_info_t *rtt_info);

void write_samsung_header(FILE *out, struct rtt_info_t *rtt_info);
void write_samsung1_note(FILE *out, struct rtt_info_t *rtt_info);
void write_samsung2_note(FILE *out, struct rtt_info_t *rtt_info);
void write_samsung_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_siemens_header(FILE *out, struct rtt_info_t *rtt_info);
void write_siemens_note(FILE *out, struct rtt_info_t *rtt_info);
void write_siemens_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_mot_key_header(FILE *out, struct rtt_info_t *rtt_info);
void write_mot_key_note(FILE *out, struct rtt_info_t *rtt_info);
void write_mot_key_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_emelody_header(FILE *out, struct rtt_info_t *rtt_info);
void write_emelody_note(FILE *out, struct rtt_info_t *rtt_info);
void write_emelody_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_imelody_header(FILE *out, struct rtt_info_t *rtt_info);
void write_imelody_note(FILE *out, struct rtt_info_t *rtt_info);
void write_imelody_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_nokia_header(FILE *out, struct rtt_info_t *rtt_info);
void write_nokia_note(FILE *out, struct rtt_info_t *rtt_info);
void write_nokia_footer(FILE *out, struct rtt_info_t *rtt_info);
void write_nokia_bpm(FILE *out, struct rtt_info_t *rtt_info);
void write_nokia_volume(FILE *out, struct rtt_info_t *rtt_info);
void write_nokia_style(FILE *out, struct rtt_info_t *rtt_info);

void write_midi_header(FILE *out, struct rtt_info_t *rtt_info);
void write_midi_note(FILE *out, struct rtt_info_t *rtt_info);
void write_midi_footer(FILE *out, struct rtt_info_t *rtt_info);
void write_midi_bpm(FILE *out, struct rtt_info_t *rtt_info);

void write_3210_header(FILE *out, struct rtt_info_t *rtt_info);
void write_3210_note(FILE *out, struct rtt_info_t *rtt_info);
void write_3210_footer(FILE *out, struct rtt_info_t *rtt_info);

int header_route(FILE *out, struct rtt_info_t *rtt_info);
int note_route(FILE *out, struct rtt_info_t *rtt_info);
int footer_route(FILE *out, struct rtt_info_t *rtt_info);
int tempo_route(FILE *out, struct rtt_info_t *rtt_info);
int volume_route(FILE *out, struct rtt_info_t *rtt_info);

int logo_header_route(FILE *out, struct rtt_info_t *rtt_info);
int logo_footer_route(FILE *out, struct rtt_info_t *rtt_info);

void write_ems(struct rtt_info_t *rtt_info, char *s);
void write_ems_footer(FILE *out, struct rtt_info_t *rtt_info);
void write_ems_logo_header(FILE *out, struct rtt_info_t *rtt_info);
void write_ems_logo_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_nokia_logo_header(FILE *out, struct rtt_info_t *rtt_info);
void write_nokia_logo_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_ngg_logo_header(FILE *out, struct rtt_info_t *rtt_info);
void write_ngg_logo_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_bmp_header(FILE *out, struct rtt_info_t *rtt_info);
void write_bmp_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_wbmp_header(FILE *out, struct rtt_info_t *rtt_info);
void write_wbmp_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_icon_header(FILE *out, struct rtt_info_t *rtt_info);
void write_icon_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_gif_header(FILE *out, struct rtt_info_t *rtt_info);
void write_gif_footer(FILE *out, struct rtt_info_t *rtt_info);

void write_text_footer(FILE *out, struct rtt_info_t *rtt_info);

int write_seo(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int write_treo_pdb(FILE *in, FILE *out, struct rtt_info_t *rtt_info);

int parse_rtttl(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_imelody(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int wav2pdb(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_logo(FILE *in, FILE *out, struct rtt_info_t *rtt_info, char *message);
int parse_midi(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_kws(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_wav(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_siemens(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_emelody(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_3210(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_sckl(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_morsecode(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_ems(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_ott(FILE *in, FILE *out, struct rtt_info_t *rtt_info);

int parse_ngg(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_bmp(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_text(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_wbmp(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_icon(FILE *in, FILE *out, struct rtt_info_t *rtt_info);
int parse_gif(FILE *in, FILE *out, struct rtt_info_t *rtt_info);

#ifdef DEBUG
void debug_logo(struct rtt_info_t *rtt_info);
#endif



