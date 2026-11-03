#define KYOEI_IM_WRAP_TEST
#include "kyoei_im_wrap.c"

static int check(const char *got, const char *want, int code) {
    if (strcmp(got, want)) { printf("FAIL %d\n got: [%s]\nwant: [%s]\n", code, got, want); return code; }
    return 0;
}
int main(void) {
    float half[95]; for (int i = 0; i < 95; ++i) half[i] = .5f;
    kyoei_im_set_metrics(half);                     /* every ASCII letter = half a cell */
    char out[512]; int r;
    /* 6 cells = 12 half-width characters per line */
    kyoei_im_rewrap("Pouco depois das 18h de hoje", 6, 0, out, sizeof out);
    if ((r = check(out, "Pouco depois\ndas 18h de\nhoje", 1))) return r;
    kyoei_im_rewrap("Linha\nO Secretario-Chefe do", 6, 0, out, sizeof out);
    if ((r = check(out, "Linha\nO\nSecretario-\nChefe do", 2))) return r;
    kyoei_im_rewrap("um dois tres quatro cinco seis", 4, 2, out, sizeof out);   /* 2 lines max */
    if ((r = check(out, "um dois\ntres...", 3))) return r;
    /* accented letter = 0.6 cell */
    kyoei_im_rewrap("\xc3\xa1\xc3\xa1\xc3\xa1 \xc3\xa1\xc3\xa1\xc3\xa1", 4, 0, out, sizeof out);
    if ((r = check(out, "\xc3\xa1\xc3\xa1\xc3\xa1\n\xc3\xa1\xc3\xa1\xc3\xa1", 4))) return r;
    kyoei_im_shorten("Gigante", 10, out, sizeof out);
    if ((r = check(out, "Gigante", 5))) return r;
    kyoei_im_shorten("Gigante derrota monstro vermelho", 10, out, sizeof out);
    if ((r = check(out, "Gigante derrota...", 6))) return r;
    if (kyoei_im_shorten("Supercalifragilisticoexpialidocious", 5, out, sizeof out)) return 7;
    puts("IM WRAP PASS: width by cells, words whole, hyphen break, ellipsis on overflow");
    return 0;
}
