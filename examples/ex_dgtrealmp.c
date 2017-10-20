#include <sndfile.h>
#include "complex.h"
#include "../include/ltfat.h"

int
loadwavfile(const char* name, double** f, int* Ls, int* W )
{
    SF_INFO info;
    SNDFILE* sf = sf_open(name, SFM_READ, &info);
    if (!sf)
    {
        printf("Failed to load %s\n", name);
        exit(1);
    }

    *Ls = info.frames * info.channels;
    *W = info.channels;
    double* ftmp = ltfat_calloc_d(*Ls);
    *f = ltfat_calloc_d(*Ls / (*W));

    *Ls = sf_read_double(sf, ftmp, *Ls);
    *Ls /= *W;

    if (*W > 1)
    {
        for (int l = 0; l < *Ls; l++)
        {
            (*f)[l] = ftmp[(*W) * l];
        }
    }

    ltfat_safefree(ftmp);
    sf_close(sf);
    return 0;
}


int main(int argc, char* argv[])
{
    int Ls, L;
    int W;
    double* f;
    double complex* cout;
    int fs = 44100;
    loadwavfile("/home/susnak/Desktop/SQAM/57.wav", &f, &Ls, &W);
    Ls = 44100 * 4;
    printf("Ls=%d,W=%d\n", Ls, W);

    ltfat_int a = 16;
    ltfat_int M = 2048;
    ltfat_int gl = 2048;
    ltfat_int M2 = M/2 + 1;

    L = ltfat_dgtlength(Ls, a, M);
    f = ltfat_postpad_d(f, Ls, L);
    double* fout = ltfat_malloc_d(L);

    double* g = ltfat_malloc_d(gl);
    cout = ltfat_malloc_dc(M2*L/a);

    ltfat_firwin_d(LTFAT_BLACKMAN, gl, g);
    ltfat_normalize_d(g, gl, LTFAT_NORMALIZE_ENERGY, g);

    ltfat_dgtrealmp_state_d* plan;

    ltfat_dgtrealmp_params* params = ltfat_dgtrealmp_params_allocdef();
    ltfat_dgtrealmp_setpar_phaseconv(params,LTFAT_TIMEINV);
    ltfat_dgtrealmp_setpar_errtoldb(params,-40);
    ltfat_dgtrealmp_setpar_maxatoms(params,0.8*L);
    ltfat_dgtrealmp_setpar_iterstep(params,1);

    ltfat_dgtrealmp_init_d((const double**)&g, &gl, L, 1, &a, &M, params, &plan);

    ltfat_dgtrealmp_params_free(params);


    ltfat_dgtrealmp_execute_d(plan,f,&cout,fout);

    ltfat_dgtrealmp_done_d(&plan);

    double snr;
    ltfat_snr_d(f,fout,L,&snr);

    printf("SNR=%2.3f dB\n",snr);

    ltfat_safefree(g);
    ltfat_safefree(f);
    ltfat_safefree(cout);
    ltfat_safefree(fout);

    return 0;
}