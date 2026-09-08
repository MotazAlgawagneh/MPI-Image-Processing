#ifndef PGM_H
#define PGM_H

/* Data */
typedef struct {
    int width, height, maxval;
    int *data; // المصفوفة الخطية (1D Array) التي ستحمل قيم البكسلات
} PGMImage;

/* Functions */
PGMImage* pgm_create(int w, int h, int maxval);
PGMImage* pgm_read(const char *filename);
int pgm_write(const char *filename, const PGMImage *img);
void pgm_free(PGMImage *img);

void pgm_write_histogram(const int *bins, int nbins, 
                         int width, int height, 
                         const char *filename);

#endif