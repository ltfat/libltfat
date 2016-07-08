#include "ltfat.h"
#include "ltfat/types.h"
#include "ltfat/macros.h"

struct LTFAT_NAME(idgt_long_plan)
{
    ltfatInt a;
    ltfatInt M;
    ltfatInt L;
    ltfatInt W;
    ltfatInt c;
    ltfatInt h_a;
    dgt_phasetype ptype;
    LTFAT_REAL scalconst;
    LTFAT_COMPLEX* f;
    const LTFAT_COMPLEX* cin;
    LTFAT_COMPLEX* gf, *ff, *cf, *cwork, *cbuf;
    LTFAT_FFTW(plan) p_before, p_after, p_veryend;
};

LTFAT_EXTERN int
LTFAT_NAME(idgt_long)(const LTFAT_COMPLEX* cin, const LTFAT_TYPE* g,
                      const ltfatInt L, const ltfatInt W,
                      const ltfatInt a, const ltfatInt M,
                      const dgt_phasetype ptype,
                      LTFAT_COMPLEX* f)
{
    LTFAT_NAME(idgt_long_plan)* plan = NULL;
    int status = LTFATERR_SUCCESS;

    CHECKSTATUS(
        LTFAT_NAME(idgt_long_init)(cin, g, L, W, a, M, f, ptype, FFTW_ESTIMATE,
                                   &plan),
        "Init failed");

    LTFAT_NAME(idgt_long_execute)(plan);

    LTFAT_NAME(idgt_long_done)(&plan);

error:
    return status;
}

LTFAT_EXTERN int
LTFAT_NAME(idgt_long_init)(const LTFAT_COMPLEX* cin, const LTFAT_TYPE* g,
                           const ltfatInt L, const ltfatInt W,
                           const ltfatInt a, const ltfatInt M, LTFAT_COMPLEX* f,
                           const dgt_phasetype ptype, unsigned flags,
                           LTFAT_NAME(idgt_long_plan)** pout)
{
    LTFAT_NAME(idgt_long_plan)* plan = NULL;
    int status = LTFATERR_SUCCESS;
    CHECKNULL(cin); CHECKNULL(g); CHECKNULL(pout); CHECKNULL(f);
    CHECK(LTFATERR_NOTPOSARG, W > 0, "W (passed %d) must be positive.", W);
    CHECK(LTFATERR_NOTPOSARG, a > 0, "a (passed %d) must be positive.", a);
    CHECK(LTFATERR_NOTPOSARG, M > 0, "M (passed %d) must be positive.", M);

    ltfatInt minL = ltfat_lcm(a, M);
    CHECK(LTFATERR_BADARG,
          L > 0  && !(L % minL),
          "L (passed %d) must be positive and divisible by lcm(a,M)=%d.",
          L, minL);

    CHECKMEM(plan = ltfat_calloc(1, sizeof * plan));

    ltfatInt h_m;

    plan->a = a; plan->L = L; plan->M = M; plan->W = W; plan->ptype = ptype;
    /*  ----------- calculation of parameters and plans -------- */

    const ltfatInt b = L / M;
    const ltfatInt N = L / a;

    plan->c = ltfat_gcd(a, M, &plan->h_a, &h_m);
    const ltfatInt p = a / plan->c;
    const ltfatInt q = M / plan->c;
    const ltfatInt d = b / p;


    CHECKMEM( plan->gf    = ltfat_malloc(L * sizeof * plan->gf));
    CHECKMEM( plan->ff    = ltfat_malloc(d * p * q * W * sizeof * plan->ff));
    CHECKMEM( plan->cf    = ltfat_malloc(d * q * q * W * sizeof * plan->cf));
    CHECKMEM( plan->cwork = ltfat_malloc(M * N * W * sizeof * plan->cwork));
    CHECKMEM( plan->cbuf  = ltfat_malloc(d * sizeof * plan->cbuf));
    plan->cin = cin;
    plan->f = f;
    LTFAT_NAME(wfac)(g, L, 1, a, M, plan->gf);

    /* Scaling constant needed because of FFTWs normalization. */
    plan->scalconst = 1.0 / ((LTFAT_REAL)d * sqrt((LTFAT_REAL)M));

    /* Create plans. In-place. */

    plan->p_after  = LTFAT_FFTW(plan_dft_1d)(d, plan->cbuf, plan->cbuf,
                     FFTW_FORWARD, flags);

    CHECKINIT(plan->p_after, "FFTW plan failed.");

    plan->p_before = LTFAT_FFTW(plan_dft_1d)(d, plan->cbuf, plan->cbuf,
                     FFTW_BACKWARD, flags);
    CHECKINIT(plan->p_before, "FFTW plan failed.");

    /* Create plan. Copy data so we do not overwrite input. Therefore
       it is ok to cast away the constness of cin.*/

    // Downcast to int
    int Mint = (int) M;

    plan->p_veryend = LTFAT_FFTW(plan_many_dft)(1, &Mint, N * W,
                      (LTFAT_COMPLEX*)cin, NULL,
                      1, Mint, plan->cwork, NULL,
                      1, Mint, FFTW_BACKWARD, flags);

    CHECKINIT(plan->p_veryend, "FFTW plan failed.");

    *pout = plan;
    return status;
error:
    if (plan)
    {
        if (plan->p_before) LTFAT_FFTW(destroy_plan)(plan->p_before);
        if (plan->p_after) LTFAT_FFTW(destroy_plan)(plan->p_after);
        if (plan->p_veryend) LTFAT_FFTW(destroy_plan)(plan->p_veryend);
        LTFAT_SAFEFREEALL(plan->gf, plan->ff, plan->cf, plan->cwork, plan->cbuf);
        ltfat_free(plan);
    }
    return status;
}

LTFAT_EXTERN int
LTFAT_NAME(idgt_long_execute)(LTFAT_NAME(idgt_long_plan)* p)
{
    int status = LTFATERR_SUCCESS;
    CHECKNULL(p);

    LTFAT_FFTW(execute)(p->p_veryend);

    if (p->ptype)
        LTFAT_NAME_COMPLEX(dgtphaseunlockhelper)(p->cwork, p->L, p->W, p->a,
                p->M, p->cwork);

    LTFAT_NAME(idgt_walnut_execute)(p);
error:
    return status;

}

LTFAT_EXTERN int
LTFAT_NAME(idgt_long_done)(LTFAT_NAME(idgt_long_plan)** plan)
{
    int status = LTFATERR_SUCCESS;
    CHECKNULL(plan); CHECKNULL(*plan);
    LTFAT_NAME(idgt_long_plan)* p = *plan;

    LTFAT_FFTW(destroy_plan)(p->p_before);
    LTFAT_FFTW(destroy_plan)(p->p_after);
    LTFAT_FFTW(destroy_plan)(p->p_veryend);
    LTFAT_SAFEFREEALL(p->gf, p->ff, p->cf, p->cwork, p->cbuf);

    ltfat_free(p);
    p = NULL;
error:
    return status;
}

LTFAT_EXTERN void
LTFAT_NAME(idgt_walnut_execute)(LTFAT_NAME(idgt_long_plan)* p)
{
    const ltfatInt b = p->L / p->M;
    const ltfatInt N = p->L / p->a;

    const ltfatInt c = p->c;
    const ltfatInt pp = p->a / c;
    const ltfatInt q = p->M / c;
    const ltfatInt d = b / pp;

    const ltfatInt L = p->L;
    const ltfatInt W = p->W;
    const ltfatInt a = p->a;
    const ltfatInt M = p->M;

    ltfatInt h_a = -p->h_a;

    /* Scaling constant needed because of FFTWs normalization. */
    const LTFAT_REAL scalconst = p->scalconst;

    /* -------- Main loop ----------------------------------- */

    const ltfatInt ld4c = M * N;

    /* Leading dimensions of cf */
    const ltfatInt ld3b = q * q * W;

    /* Leading dimensions of the 4dim array. */
    const ltfatInt ld2ff = pp * q * W;

    for (ltfatInt r = 0; r < c; r++)
    {
        LTFAT_COMPLEX* cfp = p->cf;

        for (ltfatInt w = 0; w < W; w++)
        {
            /* Complete inverse fac of coefficients */
            for (ltfatInt l = 0; l < q; l++)
            {
                for (ltfatInt u = 0; u < q; u++)
                {
                    for (ltfatInt s = 0; s < d; s++)
                    {
                        const ltfatInt rem = r + l * c +
                                             positiverem(u + s * q - l * h_a, N)
                                             * M + w * ld4c;
                        p->cbuf[s] = p->cwork[rem];
                    }

                    /* Do inverse fft of length d */
                    LTFAT_FFTW(execute)(p->p_after);

                    for (ltfatInt s = 0; s < d; s++)
                    {
                        cfp[s * ld3b] =  p->cbuf[s];
                    }
                    /* Advance the cf pointer. This is only done in this
                    * one place, because the loops are placed such that
                    * this pointer will advance linearly through
                    * memory. Reordering the loops will break this. */
                    cfp++;
                }
            }
        }



        /* -------- compute matrix multiplication ---------- */


        /* Do the matmul  */
        for (ltfatInt s = 0; s < d; s++)
        {

            const LTFAT_COMPLEX* gbase = p->gf + (r + s * c) * pp * q;
            LTFAT_COMPLEX*       fbase = p->ff + s * pp * q * W;
            const LTFAT_COMPLEX* cbase = (const LTFAT_COMPLEX*)p->cf + s * q * q * W;

            for (ltfatInt nm = 0; nm < q * W; nm++)
            {
                for (ltfatInt km = 0; km < pp; km++)
                {

                    fbase[km + nm * pp] = 0.0;
                    for (ltfatInt mm = 0; mm < q; mm++)
                    {
                        fbase[km + nm * pp] += gbase[km + mm * pp] * cbase[mm + nm * q];
                    }
                    /* Scale because of FFTWs normalization. */
                    fbase[km + nm * pp] = fbase[km + nm * pp] * scalconst;
                }
            }
        }




        /* ----------- compute inverse signal factorization ---------- */


        LTFAT_COMPLEX* ffp = p->ff;
        LTFAT_COMPLEX* fp  = p->f + r;

        for (ltfatInt w = 0; w < W; w++)
        {
            for (ltfatInt l = 0; l < q; l++)
            {
                for (ltfatInt k = 0; k < pp; k++)
                {
                    for (ltfatInt s = 0; s < d; s++)
                    {
                        p->cbuf[s] = ffp[s * ld2ff];
                    }

                    LTFAT_FFTW(execute)(p->p_before);

                    for (ltfatInt s = 0; s < d; s++)
                    {
                        const ltfatInt rem = positiverem(k * M + s * pp * M -
                                                         l * h_a * a, L);
                        fp[rem] = p->cbuf[s];
                    }

                    /* Advance the ff pointer. This is only done in this
                    * one place, because the loops are placed such that
                    * this pointer will advance linearly through
                    * memory. Reordering the loops will break this. */
                    ffp++;
                }
            }
            fp += L;
        }
        fp -= L * W;

        /* ----- Main loop ends here ------------- */
    }



}

LTFAT_EXTERN void
LTFAT_NAME(idgt_fac)(const LTFAT_COMPLEX* cin, const LTFAT_COMPLEX* gf,
                     const ltfatInt L, const ltfatInt W,
                     const ltfatInt a, const ltfatInt M,
                     const dgt_phasetype ptype,
                     LTFAT_COMPLEX* f)
{

    /*  --------- initial declarations -------------- */

    ltfatInt h_a, h_m;

    LTFAT_FFTW(plan) p_before, p_after, p_veryend;
    LTFAT_COMPLEX* ff, *cf, *cwork, *cbuf;

    /*  ----------- calculation of parameters and plans -------- */

    const ltfatInt b = L / M;
    const ltfatInt N = L / a;

    const ltfatInt c = ltfat_gcd(a, M, &h_a, &h_m);
    const ltfatInt p = a / c;
    const ltfatInt q = M / c;
    const ltfatInt d = b / p;

    h_a = -h_a;

    ff    = ltfat_malloc(d * p * q * W * sizeof * ff);
    cf    = ltfat_malloc(d * q * q * W * sizeof * cf);
    cwork = ltfat_malloc(M * N * W * sizeof * cwork);
    cbuf  = ltfat_malloc(d * sizeof * cbuf);

    /* Scaling constant needed because of FFTWs normalization. */
    const LTFAT_REAL scalconst = 1.0 / ((LTFAT_REAL)d * sqrt((LTFAT_REAL)M));

    /* Create plans. In-place. */

    p_after  = LTFAT_FFTW(plan_dft_1d)(d, cbuf, cbuf,
                                       FFTW_FORWARD, FFTW_ESTIMATE);

    p_before = LTFAT_FFTW(plan_dft_1d)(d, cbuf, cbuf,
                                       FFTW_BACKWARD, FFTW_ESTIMATE);

    /* Create plan. Copy data so we do not overwrite input. Therefore
       it is ok to cast away the constness of cin.*/

    // Downcast to int
    int Mint = (int) M;

    p_veryend = LTFAT_FFTW(plan_many_dft)(1, &Mint, N * W,
                                          (LTFAT_COMPLEX*)cin, NULL,
                                          1, Mint,
                                          cwork, NULL,
                                          1, Mint,
                                          FFTW_BACKWARD, FFTW_ESTIMATE);

    /* -------- Execute initial IFFT ------------------------ */
    LTFAT_FFTW(execute)(p_veryend);


    if (ptype)
        LTFAT_NAME_COMPLEX(dgtphaseunlockhelper)(cwork, L, W, a, M, cwork);

    /* -------- Main loop ----------------------------------- */

    const ltfatInt ld4c = M * N;

    /* Leading dimensions of cf */
    const ltfatInt ld3b = q * q * W;

    /* Leading dimensions of the 4dim array. */
    const ltfatInt ld2ff = p * q * W;

    for (ltfatInt r = 0; r < c; r++)
    {
        LTFAT_COMPLEX* cfp = cf;

        for (ltfatInt w = 0; w < W; w++)
        {
            /* Complete inverse fac of coefficients */
            for (ltfatInt l = 0; l < q; l++)
            {
                for (ltfatInt u = 0; u < q; u++)
                {
                    for (ltfatInt s = 0; s < d; s++)
                    {
                        const ltfatInt rem = r + l * c +
                                             positiverem(u + s * q - l * h_a, N)
                                             * M + w * ld4c;
                        cbuf[s] = cwork[rem];
                    }

                    /* Do inverse fft of length d */
                    LTFAT_FFTW(execute)(p_after);

                    for (ltfatInt s = 0; s < d; s++)
                    {
                        cfp[s * ld3b] =  cbuf[s];
                    }
                    /* Advance the cf pointer. This is only done in this
                    * one place, because the loops are placed such that
                    * this pointer will advance linearly through
                    * memory. Reordering the loops will break this. */
                    cfp++;
                }
            }
        }



        /* -------- compute matrix multiplication ---------- */


        /* Do the matmul  */
        for (ltfatInt s = 0; s < d; s++)
        {

            const LTFAT_COMPLEX* gbase = gf + (r + s * c) * p * q;
            LTFAT_COMPLEX*       fbase = ff + s * p * q * W;
            const LTFAT_COMPLEX* cbase = (const LTFAT_COMPLEX*)cf + s * q * q * W;

            for (ltfatInt nm = 0; nm < q * W; nm++)
            {
                for (ltfatInt km = 0; km < p; km++)
                {

                    fbase[km + nm * p] = 0.0;
                    for (ltfatInt mm = 0; mm < q; mm++)
                    {
                        fbase[km + nm * p] += gbase[km + mm * p] * cbase[mm + nm * q];
                    }
                    /* Scale because of FFTWs normalization. */
                    fbase[km + nm * p] = fbase[km + nm * p] * scalconst;
                }
            }
        }




        /* ----------- compute inverse signal factorization ---------- */


        LTFAT_COMPLEX* ffp = ff;
        LTFAT_COMPLEX* fp  = f + r;

        for (ltfatInt w = 0; w < W; w++)
        {
            for (ltfatInt l = 0; l < q; l++)
            {
                for (ltfatInt k = 0; k < p; k++)
                {
                    for (ltfatInt s = 0; s < d; s++)
                    {
                        cbuf[s] = ffp[s * ld2ff];
                    }

                    LTFAT_FFTW(execute)(p_before);

                    for (ltfatInt s = 0; s < d; s++)
                    {
                        const ltfatInt rem = positiverem(k * M + s * p * M -
                                                         l * h_a * a, L);
                        fp[rem] = cbuf[s];
                    }

                    /* Advance the ff pointer. This is only done in this
                    * one place, because the loops are placed such that
                    * this pointer will advance linearly through
                    * memory. Reordering the loops will break this. */
                    ffp++;
                }
            }
            fp += L;
        }
        fp -= L * W;

        /* ----- Main loop ends here ------------- */
    }

    /* -----------  Clean up ----------------- */

    LTFAT_FFTW(destroy_plan)(p_veryend);
    LTFAT_FFTW(destroy_plan)(p_after);
    LTFAT_FFTW(destroy_plan)(p_before);

    LTFAT_SAFEFREEALL(cwork, ff, cf, cbuf);
}