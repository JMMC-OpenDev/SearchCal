/*******************************************************************************
* JMMC project
*
* "@(#) $Id: miscTestFile.c,v 1.7 2004-09-27 14:59:47 lafrasse Exp $"
*
* who       when		 what
* --------  -----------	 -------------------------------------------------------
* lafrasse  23-Jun-2004  Forked from miscTestUtils.c
* lafrasse  21-Jul-2004  Added miscResolvePath, miscGetEnvVarValue, and
*                        miscYankLastPath tests
* lafrasse  02-Aug-2004  Changed local includes to use miscFile headers
* lafrasse  03-Aug-2004  Changed miscResolvePath test to reveal a bug that was
*                        causing an '\' append at the end of the computed path
* lafrasse  23-Aug-2004  Changed miscGetEnvVarValue API
* lafrasse  27-Sep-2004  Added miscFileExists test
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: miscTestFile.c,v 1.7 2004-09-27 14:59:47 lafrasse Exp $"; 
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
    printf("miscGetEnvVarValue() Function Test :\n\n");
    printf("   Environment Variable name      | Env. Var. value\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (fullFileName, "SHEL");
    printf("   %-30s | ", fullFileName);
    if (miscGetEnvVarValue(fullFileName, fullFileName, sizeof(mcsSTRING256))
        == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", fullFileName);
    }
    strcpy (fullFileName, "HOME");
    printf("   %-30s | ", fullFileName);
    if (miscGetEnvVarValue(fullFileName, fullFileName, sizeof(mcsSTRING256))
        == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", fullFileName);
    }
    strcpy (fullFileName, "INTROOT");
    printf("   %-30s | ", fullFileName);
    if (miscGetEnvVarValue(fullFileName, fullFileName, sizeof(mcsSTRING256))
        == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", fullFileName);
    }
    strcpy (fullFileName, "MCSROOT");
    printf("   %-30s | ", fullFileName);
    if (miscGetEnvVarValue(fullFileName, fullFileName, sizeof(mcsSTRING256))
        == FAILURE)
    {
        printf("FAILURE\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("%s\n", fullFileName);
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
    strcpy (fullFileName, "/tmp/../p/.data/");
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
    strcpy (fullFileName, "~/../p/.data/");
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
    strcpy (fullFileName, "$INTROOT/data/");
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
    strcpy (fullFileName, "$MCSROOT/$INTROOT/data/");
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
    strcpy (fullFileName, "/data/$MCSROOT/$INTROOT/data/");
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
    strcpy (fullFileName, "~/data/$MCSROOT/$INTROOT/data/");
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


    /* Test of miscFileExists() */
    printf("miscFileExists() Function Test :\n\n");
    printf("   ---------------------------------------------------------\n");
    strcpy (fullFileName, "miscErrors.xml");
    printf("Tested File = \"%s\" ", fullFileName);
    if (miscFileExists(fullFileName) == FAILURE)
    {
        printf("DOESN'T EXIST\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("        EXIST\n");
    }
    strcpy (fullFileName, "../miscErrors.xml");
    printf("Tested File = \"%s\" ", fullFileName);
    if (miscFileExists(fullFileName) == FAILURE)
    {
        printf("DOESN'T EXIST\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("        EXIST\n");
    }
    strcpy (fullFileName, "../errors/miscErrors.xml");
    printf("Tested File = \"%s\" ", fullFileName);
    if (miscFileExists(fullFileName) == FAILURE)
    {
        printf("DOESN'T EXIST\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("        EXIST\n");
    }
    strcpy (fullFileName, "$INTROOT/errors/miscErrors.xml");
    printf("Tested File = \"%s\" ", fullFileName);
    if (miscFileExists(fullFileName) == FAILURE)
    {
        printf("DOESN'T EXIST\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("        EXIST\n");
    }
    strcpy (fullFileName, "$MCSROOT/errors/miscErrors.xml");
    printf("Tested File = \"%s\" ", fullFileName);
    if (miscFileExists(fullFileName) == FAILURE)
    {
        printf("DOESN'T EXIST\n");
        errDisplayStack();
        errCloseStack();
    }
    else
    {
        printf("        EXIST\n");
    }
    exit (EXIT_SUCCESS);
}

/*___oOo___*/
