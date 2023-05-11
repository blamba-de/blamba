
#define BUFFER_SIZE 8192

struct gif_header_t
{
  int version;
  /* unsigned short int width; */
  /* unsigned short int height; */
  unsigned int colors;
  unsigned int color_map[256];
  unsigned char resolution;
  unsigned char bgcolor;
  unsigned char aspect;
  unsigned char flags;
};

struct node_t
{
  int prev;
  unsigned char color;
};

struct compress_node_t
{
  int down;
  int right;
  unsigned char color;
};

int lzw_decompress(FILE *in, unsigned int *picture);
int lzw_compress(FILE *in, unsigned int *picture, int width, int height);



