/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestFile.c,v 1.4 2004-08-02 14:08:46 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* lafrasse  23-Jun-2004  Forked from miscTestUtils.c
* lafrasse  21-Jul-2004  Added miscResolvePath, miscGetEnvVarValue, and
*                        miscYankLastPath tests
* lafrasse  02-Aug-2004  Changed local includes to use miscFile headers
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestFile.c,v 1.4 2004-08-02 14:08:46 lafrasse Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


/*
 * Local Headers 
 */
#include "mcs.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "miscFile.h"


/* 
 * Main
 */

int main (int argc, char *argv[])
{
    /* Give process name to mcs library */
    mcsInit(argv[0]);
    
    mcsSTRING256 fullFileName;
    char *tmp = NULL;
    tmp = calloc(sizeof(char), 256);

    /* Test of miscGetFileName() */
    printf("miscGetFileName() Function Test :\n\n");
    printf("   File Path                      | File Name\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (fullFileName, "fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "./fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "../fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/data/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetFileName(fullFileName));
    errDisplayStack();
    errCloseStack();
    printf("\n\n");

    /* Test of miscGetExtension() */
    printf("miscGetExtension() Function Test :\n\n");
    printf("   File Path                      | File Extension\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (fullFileName, "fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "./fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "./fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "../fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "../fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/data/fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/data/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/.data/fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/.data/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/../p/.data/fileName");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/../p/.data/fileName.txt");
    printf("   %-30s | %s\n", fullFileName, miscGetExtension(fullFileName));
    errDisplayStack();
    errCloseStack();
    printf("\n\n");

    /* Test of miscYankExtension() */
    printf("miscYankExtension() Function Test :\n\n");
    printf("   File Path                      | Without Extension\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (fullFileName, "fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "./fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "./fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "../fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "../fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/.data/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/.data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/../p/.data/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/../p/.data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankExtension(fullFileName, NULL);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    printf("\n\n");

    /* Test of miscGetEnvVarValue() */
    tmp[0] = '\0';
    printf("miscGetEnvVarValue() Function Test :\n\n");
    printf("   Environment Variable name      | Env. Var. value\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (fullFileName, "SHEL");
    printf("   %-30s | ", fullFileName);
    if (miscGetEnvVarValue(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", tmp);
    }
    strcpy (fullFileName, "HOME");
    printf("   %-30s | ", fullFileName);
    if (miscGetEnvVarValue(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", tmp);
    }
    strcpy (fullFileName, "INTROOT");
    printf("   %-30s | ", fullFileName);
    if (miscGetEnvVarValue(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", tmp);
    }
    strcpy (fullFileName, "MCSROOT");
    printf("   %-30s | ", fullFileName);
    if (miscGetEnvVarValue(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", tmp);
    }
    printf("\n\n");

    /* Test of miscYankLastPath() */
    printf("miscYankLastPath() Function Test :\n\n");
    printf("   File Path                      | Without Last Path\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (fullFileName, "fileName");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "./fileName");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "./fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "../fileName");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "../fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/.data/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/.data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/../p/.data/fileName");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    strcpy (fullFileName, "/tmp/../p/.data/fileName.txt");
    printf("   %-30s | ", fullFileName);
    miscYankLastPath(fullFileName);
    printf("%s\n", fullFileName);
    errDisplayStack();
    errCloseStack();
    printf("\n\n");

    /* Test of miscResolvePath() */
    printf("miscResolvePath() Function Test :\n\n");
    printf("   ---------------------------------------------------------\n");
    strcpy (fullFileName, "/tmp/../p/.data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", fullFileName);
    if (miscResolvePath(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (fullFileName, "~/../p/.data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", fullFileName);
    if (miscResolvePath(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (fullFileName, "$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", fullFileName);
    if (miscResolvePath(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (fullFileName, "/data/$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", fullFileName);
    if (miscResolvePath(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (fullFileName, "$MCSROOT/$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", fullFileName);
    if (miscResolvePath(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (fullFileName, "/data/$MCSROOT/$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", fullFileName);
    if (miscResolvePath(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (fullFileName, "~/data/$MCSROOT/$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", fullFileName);
    if (miscResolvePath(fullFileName, &tmp) == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    printf("\n\n");
    free(tmp);

    exit (EXIT_SUCCESS);
}

/*___oOo___*/
