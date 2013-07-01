/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

/**
 * @file
 * Function definition for angular diameter computation.
 *
 * @sa JMMC-MEM-2600-0009 document.
 */


/* 
 * System Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"
#include "err.h"
#include "misc.h"


/* 
 * Local Headers
 */
#include "alx.h"
#include "alxPrivate.h"
#include "alxErrors.h"


/* Minimal diameter error (1%) */
#define MIN_DIAM_ERROR      1.0

/* Invalid or maximum diameter error (400%) when the diameter error is negative */
#define MAX_DIAM_ERROR  400.0

#define ENABLE_POLYNOM_CHECK_LOGS 0

/* effective polynom domains */
/* disable in concurrent context (static vars) i.e. SOAP server */
static mcsDOUBLE effectiveDomainMin[alxNB_COLOR_INDEXES];
static mcsDOUBLE effectiveDomainMax[alxNB_COLOR_INDEXES];

#define alxNB_NORM_DIST 100

/** 2 sampled normal distributions (100 samples) (mean=0.0, sigma=1.0) sampled up to 3 sigma (99.7%) */
static mcsDOUBLE normDist1[alxNB_NORM_DIST] = {
                                               -0.405815326280233,
                                               0.7721437695835841,
                                               -0.37015641342571515,
                                               -1.8393849442476695,
                                               0.12489245523914759,
                                               0.45425961157940065,
                                               -0.9217267590822211,
                                               0.5206607650844187,
                                               1.5107480602026888,
                                               -0.025386649087379497,
                                               -0.6645651844199074,
                                               0.9842780472202869,
                                               0.6101581877256014,
                                               0.8065713105216289,
                                               -0.17824997184989788,
                                               -0.678971025250415,
                                               0.050922017757265285,
                                               1.2262650513797033,
                                               -1.0755859544234767,
                                               1.0506442478918137,
                                               -0.49440989713932365,
                                               1.1679338563424218,
                                               0.7228686621980924,
                                               -0.34071377479343645,
                                               -0.20835212632216304,
                                               0.37926272748358525,
                                               -1.44762602212604,
                                               0.3137253153103986,
                                               -1.3258961805235925,
                                               0.6770278787348959,
                                               -0.9051387258452307,
                                               -1.6456197720221348,
                                               -0.8471181511513595,
                                               0.03843163757009671,
                                               0.006528503488059608,
                                               -1.4931301295818673,
                                               -0.6741640074358313,
                                               1.043742717503957,
                                               -0.018029864050070267,
                                               -1.9694274905617575,
                                               1.6130857140144343,
                                               -0.035987669942561064,
                                               -1.4106757714152929,
                                               1.0689167795511714,
                                               -0.027616836990015388,
                                               1.295095133482102,
                                               1.4903238095950222,
                                               -1.9788990533960271,
                                               -0.1398415152099508,
                                               -0.24643126592158407,
                                               1.7113319658627602,
                                               -1.4015876155950004,
                                               1.9435413450071546,
                                               -0.005180967014450233,
                                               0.6390576868701946,
                                               -1.5064676393046021,
                                               -0.3974558217445514,
                                               1.7251294440266762,
                                               0.0033581592199167353,
                                               -0.056644858593569596,
                                               -1.0005686188071898,
                                               1.1530389902154319,
                                               -1.0768128257785903,
                                               -0.596583179627351,
                                               -0.9719658059970787,
                                               1.1825058424702757,
                                               -0.044207524769527336,
                                               1.6762786733588027,
                                               0.1707858622186104,
                                               -1.1676422655602903,
                                               1.0089192263277795,
                                               -0.4077465802979183,
                                               -0.21867770932775896,
                                               1.197446810788689,
                                               -0.9821298119190911,
                                               -0.15411562888547978,
                                               1.2924806723046096,
                                               -0.2517743889767277,
                                               -0.627189113385789,
                                               -0.3467362884379821,
                                               -0.11739668875662056,
                                               -0.37610136757242035,
                                               -1.103135861849059,
                                               1.7775647978446851,
                                               1.0132152149707343,
                                               0.6414343725144919,
                                               0.7114478692206261,
                                               -0.5613846064531317,
                                               1.7370607434875958,
                                               -0.3249912202838192,
                                               1.11079458172647,
                                               0.42719573157808893,
                                               -1.9681410391046306,
                                               1.3652947966987192,
                                               0.5881672715536158,
                                               0.9212964066240584,
                                               -0.8821742883454401,
                                               -1.3985771713635113,
                                               0.8247395433717678,
                                               -0.907625144879098
};
static mcsDOUBLE normDist2[alxNB_NORM_DIST] = {
                                               -0.815876621390182,
                                               -1.9304526376272324,
                                               -1.9999123146885922,
                                               -1.9002840973922752,
                                               1.14139355273984,
                                               -0.5349127271514056,
                                               1.4939212758301814,
                                               -1.904136483421475,
                                               1.0728570521001886,
                                               -0.2533179262598795,
                                               0.25034772013628515,
                                               1.1365098285956199,
                                               0.2938735767375494,
                                               -0.12951149264047981,
                                               -1.1448821400996576,
                                               -0.5079421174012917,
                                               -0.26532086742333627,
                                               -0.3920745522552183,
                                               -1.5684383597829843,
                                               1.9810482112231365,
                                               -0.3085477757910423,
                                               1.087444967239354,
                                               -0.6509370692158335,
                                               -0.8437781571200225,
                                               -0.43270718467684743,
                                               1.0259460166470287,
                                               -0.6073857428514298,
                                               0.4048403822661135,
                                               1.8498753237684848,
                                               1.5744600580237704,
                                               -0.3215374812303839,
                                               -0.7129452360898226,
                                               -0.4002922781889841,
                                               0.250500564875698,
                                               -0.29655227030476494,
                                               -0.5926311436910346,
                                               -1.492320836385822,
                                               -0.05598377315402614,
                                               1.4519866930265426,
                                               -0.6883852440389577,
                                               0.7030122105843625,
                                               0.39550989625036614,
                                               -0.06952581299314188,
                                               -0.8675946258017432,
                                               -0.5687284377105486,
                                               0.3722826386287184,
                                               -0.3980678393993771,
                                               1.646376929375372,
                                               1.195302341454566,
                                               -0.5688064488095321,
                                               1.348176216122298,
                                               0.673213594116966,
                                               -0.27432208764568283,
                                               -0.8876681913956069,
                                               -1.8183393908787486,
                                               0.39082242608748413,
                                               0.7405718508461351,
                                               0.18301524297037708,
                                               0.041376414205862876,
                                               0.2438813284547554,
                                               -0.906978008739808,
                                               -0.1297360428147141,
                                               -0.740624221759114,
                                               -0.2852370023849496,
                                               1.1144476346302095,
                                               1.5800571671625057,
                                               0.9185292541503605,
                                               0.9119170149549204,
                                               1.5124166422577443,
                                               0.15679741149465706,
                                               -1.0070341988381286,
                                               0.35435558598052513,
                                               -1.3964389442540066,
                                               0.9478379298119284,
                                               -1.0510351927882617,
                                               0.8383262273634826,
                                               0.7781212488877818,
                                               0.42852599386087425,
                                               -0.2207309253647106,
                                               0.683905929397226,
                                               1.45472684742991,
                                               -1.1926677270946826,
                                               -0.10392927764278716,
                                               -0.680494627953999,
                                               0.9304696491183547,
                                               -0.41969593478340056,
                                               0.30009748992265495,
                                               -0.23981546880195193,
                                               -1.0015320791743085,
                                               -1.5008332910695314,
                                               -0.3314543073113778,
                                               -1.5719970089069524,
                                               -0.25800012091091984,
                                               1.4957371337409384,
                                               -1.6449739238752332,
                                               -1.0952629820809243,
                                               0.2233546041836687,
                                               0.6308500135913273,
                                               1.9767700638944907,
                                               -1.3695510435558615
};


/** alx dev flag */
static mcsLOGICAL alxDevFlag = mcsFALSE;

mcsLOGICAL alxGetDevFlag(void)
{
    return alxDevFlag;
}

void alxSetDevFlag(mcsLOGICAL flag)
{
    alxDevFlag = flag;
    logInfo("alxDevFlag: %s", isTrue(alxDevFlag) ? "true" : "false");
}


/*
 * Local Functions declaration
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynomialForAngularDiameter(void);

const char* alxGetDiamLabel(const alxDIAM diam);

/* 
 * Local functions definition
 */

/**
 * Compute the polynomial value y = p(x) = coeffs[0] + x * coeffs[1] + x^2 * coeffs[2] ... + x^n * coeffs[n]
 * @param len coefficient array length
 * @param coeffs coefficient array
 * @param x input value
 * @return polynomial value
 */
static mcsDOUBLE alxComputePolynomial(mcsUINT32 len, mcsDOUBLE* coeffs, mcsDOUBLE x)
{
    mcsUINT32 i;
    mcsDOUBLE p  = 0.0;
    mcsDOUBLE xn = 1.0;

    /* iterative algorithm */
    for (i = 0; i < len; i++)
    {
        p += xn * coeffs[i];
        xn *= x;
    }
    return p;
}

/**
 * Return the polynomial coefficients for angular diameter computation 
 *
 * @return pointer onto the structure containing polynomial coefficients, or
 * NULL if an error occured.
 *
 * @usedfiles alxAngDiamPolynomial.cfg : file containing the polynomial
 * coefficients to compute the angular diameter for bright star. The polynomial
 * coefficients are given for (B-V), (V-R), (V-K), (I-J), (I-K), (J-H), (J-K), (H-K).
 */
static alxPOLYNOMIAL_ANGULAR_DIAMETER *alxGetPolynomialForAngularDiameter(void)
{
    /*
     * Check wether the polynomial structure in which polynomial coefficients
     * will be stored to compute angular diameter is loaded into memory or not,
     * and load it if necessary.
     */
    static alxPOLYNOMIAL_ANGULAR_DIAMETER polynomial = {mcsFALSE, "alxAngDiamPolynomial.cfg", "alxAngDiamErrorPolynomial.cfg"};
    if (isTrue(polynomial.loaded))
    {
        return &polynomial;
    }

    /* 
     * Build the dynamic buffer which will contain the coefficient file for angular diameter computation
     */
    /* Find the location of the file */
    char* fileName;
    fileName = miscLocateFile(polynomial.fileName);
    if (isNull(fileName))
    {
        return NULL;
    }

    /* Load file. Comment lines start with '#' */
    miscDYN_BUF dynBuf;
    miscDynBufInit(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    mcsINT32 lineNum = 0;
    const char* pos = NULL;
    mcsSTRING1024 line;
    mcsSTRING4 color;
    mcsINT32 c;

    while (isNotNull(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* If the current line is not empty */
        if (isFalse(miscIsSpaceStr(line)))
        {
            /* Check if there is too many lines in file */
            if (lineNum >= alxNB_COLOR_INDEXES)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* 
             * Read polynomial coefficients 
             * #Color	a0	a1	a2	a3	a4	a5	Error(skip)	DomainMin	DomainMax
             */
            if (sscanf(line, "%4s %lf %lf %lf %lf %lf %lf %*f %lf %lf",
                       color,
                       &polynomial.coeff[lineNum][0],
                       &polynomial.coeff[lineNum][1],
                       &polynomial.coeff[lineNum][2],
                       &polynomial.coeff[lineNum][3],
                       &polynomial.coeff[lineNum][4],
                       &polynomial.coeff[lineNum][5],
                       &polynomial.domainMin[lineNum],
                       &polynomial.domainMax[lineNum]) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER + 2))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Count non zero coefficients */
            polynomial.nbCoeff[lineNum] = alxNB_POLYNOMIAL_COEFF_DIAMETER;
            for (c = 0; c < alxNB_POLYNOMIAL_COEFF_DIAMETER; c++)
            {
                if (polynomial.coeff[lineNum][c] == 0.0)
                {
                    logDebug("Color '%s' nb coeff Diam=%d", alxGetDiamLabel(lineNum), c);
                    polynomial.nbCoeff[lineNum] = c;
                    break;
                }
            }

            if (strcmp(color, alxGetDiamLabel(lineNum)) != 0)
            {
                logError("Color index mismatch: '%s' (expected '%s')", color, alxGetDiamLabel(lineNum));
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    free(fileName);

    /* Check if there is missing line */
    if (lineNum != alxNB_COLOR_INDEXES)
    {
        miscDynBufDestroy(&dynBuf);
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_COLOR_INDEXES, fileName);
        return NULL;
    }

    /* 
     * Build the dynamic buffer which will contain the coefficient file for angular diameter error computation
     */
    /* Find the location of the file */
    fileName = miscLocateFile(polynomial.fileNameError);
    if (isNull(fileName))
    {
        miscDynBufDestroy(&dynBuf);
        return NULL;
    }

    /* Load file. Comment lines start with '#' */
    miscDynBufReset(&dynBuf);

    logInfo("Loading %s ...", fileName);

    NULL_DO(miscDynBufLoadFile(&dynBuf, fileName, "#"),
            miscDynBufDestroy(&dynBuf);
            free(fileName));

    /* For each line of the loaded file */
    lineNum = 0;
    pos = NULL;

    while (isNotNull(pos = miscDynBufGetNextLine(&dynBuf, pos, line, sizeof (line), mcsTRUE)))
    {
        logTrace("miscDynBufGetNextLine()='%s'", line);

        /* If the current line is not empty */
        if (isFalse(miscIsSpaceStr(line)))
        {
            /* Check if there is too many lines in file */
            if (lineNum >= alxNB_COLOR_INDEXES)
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_TOO_MANY_LINES, fileName);
                free(fileName);
                return NULL;
            }

            /* 
             * Read polynomial coefficients 
             * #Color	a0	a1	a2	a3	a4	a5
             */
            if (sscanf(line, "%4s %lf %lf %lf %lf %lf %lf",
                       color,
                       &polynomial.coeffError[lineNum][0],
                       &polynomial.coeffError[lineNum][1],
                       &polynomial.coeffError[lineNum][2],
                       &polynomial.coeffError[lineNum][3],
                       &polynomial.coeffError[lineNum][4],
                       &polynomial.coeffError[lineNum][5]) != (1 + alxNB_POLYNOMIAL_COEFF_DIAMETER))
            {
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Count non zero coefficients */
            polynomial.nbCoeffErr[lineNum] = 0;
            for (c = 0; c < alxNB_POLYNOMIAL_COEFF_DIAMETER; c++)
            {
                if (polynomial.coeffError[lineNum][c] == 0.0)
                {
                    logDebug("Color '%s' nb coeff Err =%d", alxGetDiamLabel(lineNum), c);
                    polynomial.nbCoeffErr[lineNum] = c;
                    break;
                }
            }

            if (strcmp(color, alxGetDiamLabel(lineNum)) != 0)
            {
                logError("Color index mismatch: '%s' (expected '%s')", color, alxGetDiamLabel(lineNum));
                miscDynBufDestroy(&dynBuf);
                errAdd(alxERR_WRONG_FILE_FORMAT, line, fileName);
                free(fileName);
                return NULL;
            }

            /* Next line */
            lineNum++;
        }
    }

    /* Destroy the dynamic buffer where is stored the file information */
    miscDynBufDestroy(&dynBuf);

    free(fileName);

    /* Check if there is missing line */
    if (lineNum != alxNB_COLOR_INDEXES)
    {
        errAdd(alxERR_MISSING_LINE, lineNum, alxNB_COLOR_INDEXES, fileName);
        return NULL;
    }

    /* Specify that the polynomial has been loaded */
    polynomial.loaded = mcsTRUE;

    return &polynomial;
}

/**
 * Compute am angular diameters for a given color-index based
 * on the coefficients from table. If a magnitude is not set,
 * the diameter is not computed.
 *
 * @param mA is the first input magnitude of the color index
 * @param mB is the second input magnitude of the color index
 * @param polynomial coeficients for angular diameter
 * @param band is the line corresponding to the color index A-B
 * @param diam is the structure to get back the computation 
 * @param checkDomain true to ensure mA-mB is within validity domain 
 *  
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeDiameter(const char* msg,
                                 alxDATA mA,
                                 alxDATA mB,
                                 alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                                 mcsINT32 band,
                                 alxDATA *diam,
                                 mcsLOGICAL checkDomain)
{
    mcsDOUBLE a_b;

    /* V-Kc is given in COUSIN while the coefficients for V-K are are expressed
       for JOHNSON, thus the conversion (JMMC-MEM-2600-0009 Sec 2.1) */
    if (band == alxV_K_DIAM)
    {
        a_b = (mA.value - mB.value - 0.03) / 0.992;
    }
    else if (band == alxB_V_DIAM)
    {
        /* in B-V, it is the V mag that should be used to compute apparent
           diameter with formula 10^-0.2magV, thus V is given as first mag (mA)
           while the coefficients are given in B-V */
        a_b = mB.value - mA.value;
    }
    else
    {
        a_b = mA.value - mB.value;
    }

    /* Check the polynom's domain */
    if (isTrue(checkDomain))
    {
        /* update effective polynom domains */
        if (alxIsDevFlag())
        {
            if (a_b < effectiveDomainMin[band])
            {
                effectiveDomainMin[band] = a_b;
            }
            if (a_b > effectiveDomainMax[band])
            {
                effectiveDomainMax[band] = a_b;
            }
        }

        SUCCESS_COND_DO((a_b < polynomial->domainMin[band]) || (a_b > polynomial->domainMax[band]),
                        logTest("%s Color %s out of validity domain: %lf < %lf < %lf", msg,
                                alxGetDiamLabel(band), polynomial->domainMin[band], a_b, polynomial->domainMax[band]);
                        alxDATAClear((*diam)));
    }

    diam->isSet = mcsTRUE;

    /* Compute the angular diameter */
    mcsDOUBLE p_a_b = alxComputePolynomial(polynomial->nbCoeff[band], polynomial->coeff[band], a_b);

    /* check if diameter is negative */
    if (p_a_b <= 0.0)
    {
        if (ENABLE_POLYNOM_CHECK_LOGS)
        {
            logWarning("%s Color %s diameter negative : %8.3lf (a-b=%.3lf)", msg, alxGetDiamLabel(band), p_a_b, a_b);
        }
        /* Set diameter and error to 0.0 to consider this diameter as incorrect */
        diam->value = 0.0;
        diam->error = 0.0;
    }
    else
    {
        /* Compute apparent diameter */
        /* Note: the polynom value may be negative or very high outside the polynom's domain */
        diam->value = 9.306 * pow(10.0, -0.2 * mA.value) * p_a_b;

        /* Compute error */
        mcsDOUBLE p_err = alxComputePolynomial(polynomial->nbCoeffErr[band], polynomial->coeffError[band], a_b);

        /* TODO: remove ASAP */
        if (ENABLE_POLYNOM_CHECK_LOGS && (p_err > 75.0))
        {
            /* warning when the error is very high */
            logInfo   ("%s Color %s error high     ? %8.3lf%% (a-b=%.3lf)", msg, alxGetDiamLabel(band), p_err, a_b);
        }

        /* check if error is negative */
        if (p_err < 0.0)
        {
            if (ENABLE_POLYNOM_CHECK_LOGS)
            {
                logWarning("%s Color %s error negative : %8.3lf%% (a-b=%.3lf)", msg, alxGetDiamLabel(band), p_err, a_b);
            }
            /* Set error to 0.0 to consider this diameter as incorrect */
            p_err = 0.0;
        }
        else if (p_err < MIN_DIAM_ERROR)
        {
            /* ensure error is not too small */
            if (ENABLE_POLYNOM_CHECK_LOGS)
            {
                logWarning("%s Color %s error too small: %8.3lf%% (a-b=%.3lf)", msg, alxGetDiamLabel(band), p_err, a_b);
            }
            p_err = MIN_DIAM_ERROR;
        }
        else if (p_err > MAX_DIAM_ERROR)
        {
            /* ensure error is not too high */
            if (ENABLE_POLYNOM_CHECK_LOGS)
            {
                logWarning("%s Color %s error too high : %8.3lf%% (a-b=%.3lf)", msg, alxGetDiamLabel(band), p_err, a_b);
            }
            /* Set error to 0.0 to consider this diameter as incorrect */
            p_err = 0.0;
        }

        diam->error = (p_err != 0) ? diam->value * p_err * 0.01 : 0.0;
    }

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeDiameterWithMagErr(alxDATA mA,
                                           alxDATA mB,
                                           alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial,
                                           mcsINT32 band,
                                           alxDATA *diam)
{
    /* If any magnitude is not available, then the diameter is not computed. */
    SUCCESS_COND_DO(alxIsNotSet(mA) || alxIsNotSet(mB),
                    alxDATAClear((*diam)));

    /** check domain and compute diameter and its error */
    alxComputeDiameter("|a-b|   ", mA, mB, polynomial, band, diam, mcsTRUE);

    /* Set confidence as the smallest confidence of the two magnitudes */
    diam->confIndex = (mA.confIndex <= mB.confIndex) ? mA.confIndex : mB.confIndex;

    /* If diameter is not computed (domain check), return */
    SUCCESS_COND(isFalse(diam->isSet));

    /* If any missing magnitude error (should not happen as error = 0.1 if missing error),
     * return diameter with low confidence index */
    SUCCESS_COND_DO((mA.error == 0.0) || (mB.error == 0.0),
                    diam->confIndex = alxCONFIDENCE_LOW);

    /*
     * Perform 'bootstrap / monte carlo' (brute-force algorithm):
     * - use one gaussian distribution per magnitude where mean = magnitude and sigma = magnitude error
     * - sample both diameter and its error for all combinations (2 magnitudes)
     * - finally compute diameter mean and error mean from all samples
     */

    alxDATA mAs, mBs, diamSample;

    /* copy original magnitudes */
    alxDATACopy(mA, mAs);
    alxDATACopy(mB, mBs);

    mcsUINT32 nA, nB, nSample = 0;
    mcsDOUBLE sumDiamSample = 0.0;
    mcsDOUBLE sumErrSample  = 0.0;

    /* mA */
    for (nA = 0; nA < alxNB_NORM_DIST; nA++)
    {
        /* sample mA using one gaussian distribution */
        mAs.value = mA.value + mA.error * normDist1[nA];

        /* mB */
        for (nB = 0; nB < alxNB_NORM_DIST; nB++)
        {
            /* sample mB using another gaussian distribution */
            mBs.value = mB.value + mB.error * normDist2[nB];

            /* do not check domain */
            alxComputeDiameter("|a-b|mc ", mAs, mBs, polynomial, band, &diamSample, mcsFALSE);

            /* Note: avoid storing diameters ie only keep sum(diameter) and sum(error) */

            /* check that diameter and error are defined (not dumb values) */
            if (diamSample.value != 0.0 && diamSample.error != 0.0)
            {
                nSample++;
                sumDiamSample += diamSample.value;
                sumErrSample  += diamSample.error;
            }
        }
    }

    diamSample.value = sumDiamSample / nSample;
    diamSample.error = sumErrSample  / nSample;

    logTest("Diameter %s diam=%.3lf(%.3lf %.1lf%%) diamSample=%.3lf(%.3lf %.1lf%%) [%d samples]",
            alxGetDiamLabel(band),
            diam->value, diam->error, alxDATARelError(*diam),
            diamSample.value, diamSample.error, alxDATARelError(diamSample), nSample
            );

    /* note: 9025 = 95^2 ie accept confidence down to 2 sigma (94.6%) */
    if (nSample < 9025)
    {
        /* too small samples, set low confidence index */
        diam->confIndex = alxCONFIDENCE_LOW;
    }

    /* if less samples or relative error difference > 10%, check values */
    if (nSample < alxNB_NORM_DIST * alxNB_NORM_DIST ||
        (fabs(diamSample.error - diam->error) / alxMin(diamSample.value, diam->value)) > 0.1)
    {
        logInfo("CheckDiameter %s diam=%.3lf(%.3lf %.1lf%%) diamSample=%.3lf(%.3lf %.1lf%%) [%d samples]"
                " from magA=%.3lf(%.3lf) magB=%.3lf(%.3lf)",
                alxGetDiamLabel(band),
                diam->value, diam->error, alxDATARelError(*diam),
                diamSample.value, diamSample.error, alxDATARelError(diamSample), nSample,
                mA.value, mA.error, mB.value, mB.error
                );
    }

    /* Update diameter and its error */
    diam->value = diamSample.value;
    diam->error = diamSample.error;

    return mcsSUCCESS;
}

/*
 * Public functions definition
 */

/**
 * Compute stellar angular diameters from its photometric properties.
 * 
 * @param magnitudes B V R Ic Jc Hc Kc L M (Johnson / Cousin CIT)
 * @param diameters the structure to give back all the computed diameters
 *  
 * @return mcsSUCCESS on successful completion. Otherwise mcsFAILURE is
 * returned.
 */
mcsCOMPL_STAT alxComputeAngularDiameters(const char* msg,
                                         alxMAGNITUDES magnitudes,
                                         alxDIAMETERS diameters)
{
    /* Get polynomial for diameter computation */
    alxPOLYNOMIAL_ANGULAR_DIAMETER *polynomial;
    polynomial = alxGetPolynomialForAngularDiameter();
    FAIL_NULL(polynomial);

    /* Compute diameters for B-V, V-R, V-K, I-J, I-K, J-H, J-K, H-K */

    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxB_BAND], polynomial, alxB_V_DIAM, &diameters[alxB_V_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxR_BAND], polynomial, alxV_R_DIAM, &diameters[alxV_R_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxV_BAND], magnitudes[alxK_BAND], polynomial, alxV_K_DIAM, &diameters[alxV_K_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxJ_BAND], polynomial, alxI_J_DIAM, &diameters[alxI_J_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxI_BAND], magnitudes[alxK_BAND], polynomial, alxI_K_DIAM, &diameters[alxI_K_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxJ_BAND], magnitudes[alxH_BAND], polynomial, alxJ_H_DIAM, &diameters[alxJ_H_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxJ_BAND], magnitudes[alxK_BAND], polynomial, alxJ_K_DIAM, &diameters[alxJ_K_DIAM]);
    alxComputeDiameterWithMagErr(magnitudes[alxH_BAND], magnitudes[alxK_BAND], polynomial, alxH_K_DIAM, &diameters[alxH_K_DIAM]);

    /* Display results */
    alxLogTestAngularDiameters(msg, diameters);

    return mcsSUCCESS;
}

mcsCOMPL_STAT alxComputeMeanAngularDiameter(alxDIAMETERS diameters,
                                            alxDATA     *meanDiam,
                                            alxDATA     *weightedMeanDiam,
                                            alxDATA     *weightedMeanStdDev,
                                            mcsUINT32   *nbDiameters,
                                            mcsUINT32    nbRequiredDiameters,
                                            miscDYN_BUF *diamInfo)
{
    /*
     * Note: Weighted arithmetic mean
     * from http://en.wikipedia.org/wiki/Weighted_arithmetic_mean#Dealing_with_variance
     */
    /*
     * Only use diameters with HIGH confidence 
     * ie computed from catalog magnitudes and not interpolated magnitudes.
     */
    mcsUINT32 band;
    alxDATA   diameter;
    mcsDOUBLE diam, diamError, weight;
    mcsDOUBLE minDiamError = FP_INFINITE;
    mcsUINT32 nDiameters = 0;
    mcsDOUBLE sumDiameters = 0.0;
    mcsDOUBLE weightedSumDiameters = 0.0;
    mcsDOUBLE weightSum = 0.0;
    mcsDOUBLE dist = 0.0;
    mcsLOGICAL inconsistent = mcsFALSE;
    mcsSTRING32 tmp;

    for (band = 0; band < alxNB_DIAMS; band++)
    {
        diameter = diameters[band];

        /* Note: high confidence means diameter computed from catalog magnitudes */
        if (alxIsSet(diameter) && (diameter.confIndex == alxCONFIDENCE_HIGH))
        {
            nDiameters++;
            diam = diameter.value;

            diamError = diameter.error;
            if (diamError < minDiamError)
            {
                minDiamError = diamError;
            }

            sumDiameters += diam;

            /* weight = inverse variance ie (diameter error)^2 */
            /* Note: diameter error should be > 0.0 ! */
            weight = 1.0 / (diamError * diamError);
            weightSum += weight;
            weightedSumDiameters += diam * weight;
        }
    }

    /* initialize structures */
    alxDATAClear((*meanDiam));
    alxDATAClear((*weightedMeanDiam));
    alxDATAClear((*weightedMeanStdDev));

    /* update diameter count */
    *nbDiameters = nDiameters;

    /* If less than nb required diameters, can not compute mean diameter... */
    if (nDiameters < nbRequiredDiameters)
    {
        logTest("Cannot compute mean diameter (%d < %d valid diameters)", nDiameters, nbRequiredDiameters);

        /* Set diameter flag information */
        sprintf(tmp, "REQUIRED_DIAMETERS (%1d): %1d", nbRequiredDiameters, nDiameters);
        miscDynBufAppendString(diamInfo, tmp);

        return mcsSUCCESS;
    }

    /* Compute mean diameter */
    meanDiam->isSet = mcsTRUE;
    meanDiam->value = sumDiameters / nDiameters;
    /* 
     Compute error on the mean: either 20% or the best 
     error on diameter if worst than 20%
     FIXME: the spec was 10% for the bright case 
     according to JMMC-MEM-2600-0009
     */
    meanDiam->error = 0.2 * meanDiam->value;
    /* Ensure error is larger than the mininimum diameter error */
    if (meanDiam->error < minDiamError)
    {
        meanDiam->error = minDiamError;
    }

    /* Compute weighted mean diameter  */
    weightedMeanDiam->isSet = mcsTRUE;
    weightedMeanDiam->value = weightedSumDiameters / weightSum;
    /* Note: intialize to high confidence as only high confidence diameters are used */
    weightedMeanDiam->confIndex = alxCONFIDENCE_HIGH;
    /* 
     Compute error on the weighted mean: either 20% or the best 
     error on diameter if worst than 20% 
     FIXME: the spec was 10% for the bright case 
     according to JMMC-MEM-2600-0009
     */
    weightedMeanDiam->error = 0.2 * weightedMeanDiam->value;

    /* 
     Check consistency between weighted mean diameter and individual
     diameters within 20%. If inconsistency is found, the
     weighted mean diameter has a LOW confidence.
     FIXME: the spec was 10% for the bright case 
     according to JMMC-MEM-2600-000
     */
    for (band = 0; band < alxNB_DIAMS; band++)
    {
        diameter = diameters[band];
        /* Note: high confidence means diameter computed from catalog magnitudes */
        if (alxIsSet(diameter) && (diameter.confIndex == alxCONFIDENCE_HIGH))
        {
            dist = fabs(diameter.value - weightedMeanDiam->value);

            if ((dist > weightedMeanDiam->error) && (dist > diameter.error))
            {
                if (isFalse(inconsistent))
                {
                    inconsistent = mcsTRUE;

                    /* Set confidence indexes to LOW */
                    weightedMeanDiam->confIndex = alxCONFIDENCE_LOW;

                    /* Set diameter flag information */
                    miscDynBufAppendString(diamInfo, "INCONSISTENT_DIAMETER ");
                }

                /* Append each color (distance) in diameter flag information */
                sprintf(tmp, "%s (%.3lf) ", alxGetDiamLabel(band), dist);
                miscDynBufAppendString(diamInfo, tmp);
            }
        }
    }

    /* Ensure error is larger than min diameter error */
    if (weightedMeanDiam->error < minDiamError)
    {
        weightedMeanDiam->error = minDiamError;
    }

    /* stddev = variance of the weighted mean = SQRT(N / sum(weight) ) if weight = 1 / variance */
    weightedMeanStdDev->isSet = mcsTRUE;
    weightedMeanStdDev->value = sqrt((1.0 / weightSum) * nDiameters);

    /* Propagate the weighted mean confidence to mean diameter and std dev */
    meanDiam->confIndex = weightedMeanDiam->confIndex;
    weightedMeanStdDev->confIndex = weightedMeanDiam->confIndex;

    logTest("Diameter mean=%.3lf(%.3lf %.1lf%%) weighted mean=%.3lf(%.3lf %.1lf%%) "
            "stddev=(%.3lf %.1lf%%) valid=%s [%s] from %d diameters",
            meanDiam->value, meanDiam->error, alxDATARelError(*meanDiam),
            weightedMeanDiam->value, weightedMeanDiam->error, alxDATARelError(*weightedMeanDiam),
            weightedMeanStdDev->value, 100.0 * weightedMeanStdDev->value / weightedMeanDiam->value,
            (weightedMeanDiam->confIndex == alxCONFIDENCE_HIGH) ? "true" : "false",
            alxGetConfidenceIndex(weightedMeanDiam->confIndex), nDiameters);

    return mcsSUCCESS;
}

/**
 * Return the string literal representing the confidence index 
 * @return string literal "NO", "LOW", "MEDIUM" or "HIGH"
 */
const char* alxGetConfidenceIndex(const alxCONFIDENCE_INDEX confIndex)
{
    switch (confIndex)
    {
        case alxCONFIDENCE_HIGH:
            return "HIGH";
        case alxCONFIDENCE_MEDIUM:
            return "MEDIUM";
        case alxCONFIDENCE_LOW:
            return "LOW";
        case alxNO_CONFIDENCE:
        default:
            return "NO";
    }
}

/**
 * Return the string literal representing the diam
 * @return string literal
 */
const char* alxGetDiamLabel(const alxDIAM diam)
{
    switch (diam)
    {
        case alxB_V_DIAM:
            return "B-V";
        case alxV_R_DIAM:
            return "V-R";
        case alxV_K_DIAM:
            return "V-K";
        case alxI_J_DIAM:
            return "I-J";
        case alxI_K_DIAM:
            return "I-K";
        case alxJ_H_DIAM:
            return "J-H";
        case alxJ_K_DIAM:
            return "J-K";
        case alxH_K_DIAM:
            return "H-K";
        default:
            return "";
    }
}

/**
 * Initialize this file
 */
void alxAngularDiameterInit(void)
{
    mcsUINT32 band;
    for (band = 0; band < alxNB_COLOR_INDEXES; band++)
    {
        effectiveDomainMin[band] = +100.0;
        effectiveDomainMax[band] = -100.0;
    }

    alxGetPolynomialForAngularDiameter();
}

/**
 * Log the effective diameter polynom domains
 */
void alxShowDiameterEffectiveDomains(void)
{
    if (alxIsDevFlag())
    {
        logInfo("Effective domains for diameter polynoms:");

        mcsUINT32 band;
        for (band = 0; band < alxNB_COLOR_INDEXES; band++)
        {
            logInfo("Color %s - validity domain: %lf < color < %lf",
                    alxGetDiamLabel(band), effectiveDomainMin[band], effectiveDomainMax[band]);
        }
    }
}
/*___oOo___*/
