/*******************************************************************************
 * JMMC project
 *
 * "@(#) $Id: miscTestFile.c,v 1.19 2005-10-10 12:00:11 lafrasse Exp $"
 *
 * History
 * -------
 * $Log: not supported by cvs2svn $
 * Revision 1.18  2005/10/06 15:12:46  lafrasse
 * Added miscGetEnvVarIntValue function
 *
 * Revision 1.17  2005/05/26 16:05:11  lafrasse
 * Corrected a bug re-introduced during code review in miscResolvePath(), that was causing data loss upon call of the function on the result of another call of the same function
 *
 * Revision 1.16  2005/05/20 16:22:50  lafrasse
 * Code review : refined user and developper documentation, functions reordering, and rationnalized miscYankExtension()
 *
 * Revision 1.15  2005/02/15 09:44:37  gzins
 * Added CVS log as file modification history
 *
 * lafrasse  23-Jun-2004  Forked from miscTestUtils.c
 * lafrasse  21-Jul-2004  Added miscResolvePath, miscGetEnvVarValue, and
 *                        miscYankLastPath tests
 * lafrasse  02-Aug-2004  Changed local includes to use miscFile headers
 * lafrasse  03-Aug-2004  Changed miscResolvePath test to reveal a bug that was
 *                        causing an '\' append at the end of the computed path
 * lafrasse  23-Aug-2004  Changed miscGetEnvVarValue API
 * lafrasse  27-Sep-2004  Added miscFileExists test
 * lafrasse  28-Sep-2004  Added miscLocateFileInPath test and corrected a bug in
 *                        the miscResolvePath test
 * lafrasse  30-Sep-2004  Added miscLocateFile test
 * lafrasse  01-Oct-2004  Updated to reflect miscResolvePath API change
 * lafrasse  07-Oct-2004  Changed miscFileExists API
 *
 ******************************************************************************/

static char *rcsId="@(#) $Id: miscTestFile.c,v 1.19 2005-10-10 12:00:11 lafrasse Exp $"; 
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
    
    mcsSTRING256 string;
    mcsINT32     integer;
    char *tmp = NULL;

    /* Test of miscGetEnvVarValue() */
    printf("miscGetEnvVarValue() Function Test :\n\n");
    printf("   Environment Variable name      | Env. Var. value\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (string, "toto");
    printf("   %-30s | ", string);
    if (miscGetEnvVarValue(string, string, sizeof(mcsSTRING256))
        == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }
    strcpy (string, "~");
    printf("   %-30s | ", string);
    if (miscGetEnvVarValue(string, string, sizeof(mcsSTRING256))
        == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }
    strcpy (string, "~lafrasse");
    printf("   %-30s | ", string);
    if (miscGetEnvVarValue(string, string, sizeof(mcsSTRING256))
        == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }
    strcpy (string, "$HOME");
    printf("   %-30s | ", string);
    if (miscGetEnvVarValue(string, string, sizeof(mcsSTRING256))
        == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }
    strcpy (string, "MCSENV");
    printf("   %-30s | ", string);
    if (miscGetEnvVarValue(string, string, sizeof(mcsSTRING256))
        == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }
    strcpy (string, "$INTROOT");
    printf("   %-30s | ", string);
    if (miscGetEnvVarValue(string, string, sizeof(mcsSTRING256))
        == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }
    strcpy (string, "MCSROOT");
    printf("   %-30s | ", string);
    if (miscGetEnvVarValue(string, string, sizeof(mcsSTRING256))
        == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%s\n", string);
    }
    printf("\n\n");

    /* Test of miscGetEnvVarIntValue() */
    printf("miscGetEnvVarIntValue() Function Test :\n\n");
    printf("export MCS_ENV_TEST_NUMBER=110278\n\n");
    setenv("MCS_ENV_TEST_NUMBER", "110278", 0);
    printf("   Environment Variable name      | Env. Var. value\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (string, "toto");
    printf("   %-30s | ", string);
    if (miscGetEnvVarIntValue(string, &integer) == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%d\n", integer);
    }
    strcpy (string, "$HOME");
    printf("   %-30s | ", string);
    if (miscGetEnvVarIntValue(string, &integer) == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%d\n", integer);
    }
    strcpy (string, "MCS_ENV_TEST_NUMBER");
    printf("   %-30s | ", string);
    if (miscGetEnvVarIntValue(string, &integer) == mcsFAILURE)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("%d\n", integer);
    }
    printf("\n\n");

    /* Test of miscGetFileName() */
    printf("miscGetFileName() Function Test :\n\n");
    printf("   File Path                      | File Name\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (string, "fileName.txt");
    printf("   %-30s | %s\n", string, miscGetFileName(string));
    errCloseStack();
    strcpy (string, "./fileName.txt");
    printf("   %-30s | %s\n", string, miscGetFileName(string));
    errCloseStack();
    strcpy (string, "../fileName.txt");
    printf("   %-30s | %s\n", string, miscGetFileName(string));
    errCloseStack();
    strcpy (string, "/fileName.txt");
    printf("   %-30s | %s\n", string, miscGetFileName(string));
    errCloseStack();
    strcpy (string, "/tmp/data/fileName.txt");
    printf("   %-30s | %s\n", string, miscGetFileName(string));
    errCloseStack();
    printf("\n\n");

    /* Test of miscGetExtension() */
    printf("miscGetExtension() Function Test :\n\n");
    printf("   File Path                      | File Extension\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (string, "fileName");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "fileName.txt");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "./fileName");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "./fileName.txt");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "../fileName");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "../fileName.txt");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "/fileName");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "/fileName.txt");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "/tmp/data/fileName");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "/tmp/data/fileName.txt");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName.txt");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName.txt");
    printf("   %-30s | %s\n", string, miscGetExtension(string));
    errCloseStack();
    printf("\n\n");

    /* Test of miscYankExtension() */
    printf("miscYankExtension() Function Test :\n\n");
    printf(" File Path                      | Extension | Without Extension\n");
    printf("--------------------------------+-----------+------------------\n");
    tmp = NULL;
    strcpy(string, "fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    strcpy (string, "/.data/fileName.txtname");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/.data/fileName.txtname.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    printf("--------------------------------+-----------+------------------\n");
    tmp = "data";
    strcpy(string, "fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/.data/fileName.txtname");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/.data/fileName.txtname.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    printf("--------------------------------+-----------+------------------\n");
    tmp = "txt";
    strcpy(string, "fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/.data/fileName.txtname");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/.data/fileName.txtname.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    printf("--------------------------------+-----------+------------------\n");
    tmp = ".txt";
    strcpy(string, "fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/.data/fileName.txtname");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/.data/fileName.txtname.txt");
    printf(" %-30s | %-9s | ", string, tmp);
    miscYankExtension(string, tmp);
    printf("%s\n", string);
    errCloseStack();
    printf("\n\n");

    /* Test of miscYankLastPath() */
    printf("miscYankLastPath() Function Test :\n\n");
    printf("   File Path                      | Without Last Path\n");
    printf("   -------------------------------+-------------------------\n");
    strcpy (string, "fileName");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "fileName.txt");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "./fileName.txt");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "../fileName.txt");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/fileName.txt");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/data/fileName.txt");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/.data/fileName.txt");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    strcpy (string, "/tmp/../p/.data/fileName.txt");
    printf("   %-30s | ", string);
    miscYankLastPath(string);
    printf("%s\n", string);
    errCloseStack();
    printf("\n\n");

    /* Test of miscResolvePath() */
    printf("miscResolvePath() Function Test :\n\n");
    printf("   ---------------------------------------------------------\n");
    strcpy (string, "/tmp/../p/.data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "/tmp/../p/.data/");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "~/../p/.data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "~/../p/.data/");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "$INTROOT/data/");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "/data/$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "/data/$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "$MCSROOT/$INTROOT/data/");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "/data/$MCSROOT/$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "/data/$MCSROOT/$INTROOT/data/");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "~/data/$MCSROOT/$INTROOT/data/fileName.txt");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "~/data/$MCSROOT/$INTROOT/data/");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(string);
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path   = \"%s\"\n\n", tmp);
    }
    strcpy (string, "~/data/$MCSROOT/$INTROOT/data/");
    printf("Unresolved Path = \"%s\"\n", string);
    tmp = miscResolvePath(miscResolvePath(string));
    if (tmp == NULL)
    {
        printf("mcsFAILURE\n");
        errCloseStack();
    }
    else
    {
        printf("Resolved Path(Resolved Path())   = \"%s\"\n\n", tmp);
    }
    printf("\n\n");

    /* Test of miscFileExists() */
    printf("miscFileExists() Function Test (with error reporting OFF) :\n\n");
    printf("   ---------------------------------------------------------\n");
    printf("Tested File = \"%s\" ", "NULL");
    if (miscFileExists(NULL, mcsFALSE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "../miscErrors.xml");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsFALSE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "errors");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsFALSE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "errors/");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsFALSE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "../errors");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsFALSE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "../errors/");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsFALSE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "../errors/miscErrors.xml");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsFALSE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "$INTROOT/errors/miscErrors.xml");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsFALSE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "$MCSROOT/errors/miscErrors.xml");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsFALSE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    printf("\n");
    printf("miscFileExists() Function Test (with error reporting ON) :\n\n");
    printf("   ---------------------------------------------------------\n");
    printf("Tested File = \"%s\" ", "NULL");
    if (miscFileExists(NULL, mcsTRUE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "../miscErrors.xml");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsTRUE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "../errors/miscErrors.xml");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsTRUE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "$INTROOT/errors/miscErrors.xml");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsTRUE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    strcpy (string, "$MCSROOT/errors/miscErrors.xml");
    printf("Tested File = \"%s\" ", string);
    if (miscFileExists(string, mcsTRUE) == mcsFALSE)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST\n");
    }
    printf("\n\n");

    /* Test of miscLocateFileInPath() */
    printf("miscLocateFileInPath() Function Test :\n\n");
    printf("   ---------------------------------------------------------\n");
    tmp = NULL;
    printf("Tested Path = '%s' - '%s'\n", "NULL", "miscErrors.xml");
    tmp = miscLocateFileInPath(NULL, "miscErrors.xml");
    printf("Valid Path = '%s'\n", (tmp==NULL?"NONE":tmp));
    errCloseStack();
    tmp = NULL;
    strcpy(string, "../:$INTROOT/:$MCSROOT/");
    printf("Tested Path = '%s' - '%s'\n", string, "NULL");
    tmp = miscLocateFileInPath(string, NULL);
    printf("Valid Path = '%s'\n", (tmp==NULL?"NONE":tmp));
    errCloseStack();
    tmp = NULL;
    strcpy(string, "../:$INTROOT/:$MCSROOT/");
    printf("Tested Path = '%s' - '%s'\n", string, "err");
    tmp = miscLocateFileInPath(string, "err");
    printf("Valid Path = '%s'\n", (tmp==NULL?"NONE":tmp));
    errCloseStack();
    tmp = NULL;
    strcpy(string, "../:$INTROOT/:$MCSROOT/");
    printf("Tested Path = '%s' - '%s'\n", string, "errors");
    tmp = miscLocateFileInPath(string, "errors");
    printf("Valid Path = '%s'\n", (tmp==NULL?"NONE":tmp));
    errCloseStack();
    tmp = NULL;
    strcpy(string, "../:$INTROOT/:$MCSROOT/");
    printf("Tested Path = '%s' - '%s'\n", string, "errors/");
    tmp = miscLocateFileInPath(string, "errors/");
    printf("Valid Path = '%s'\n", (tmp==NULL?"NONE":tmp));
    errCloseStack();
    tmp = NULL;
    strcpy(string, "../:$INTROOT/:$MCSROOT/");
    printf("Tested Path = '%s' - '%s'\n", string, "miscErrors.xml");
    tmp = miscLocateFileInPath(string, "miscErrors.xml");
    printf("Valid Path = '%s'\n", (tmp==NULL?"NONE":tmp));
    errCloseStack();
    tmp = NULL;
    strcpy(string, "../:$INTROOT/errors/:$MCSROOT/errors/");
    printf("Tested Path = '%s' - '%s'\n", string, "miscErrors.xml");
    tmp = miscLocateFileInPath(string, "miscErrors.xml");
    printf("Valid Path = '%s'\n", (tmp==NULL?"NONE":tmp));
    errCloseStack();
    tmp = NULL;
    strcpy(string, "../:$MCSROOT/errors/:$INTROOT/errors/");
    printf("Tested Path = '%s' - '%s'\n", string, "miscErrors.xml");
    tmp = miscLocateFileInPath(string, "miscErrors.xml");
    printf("Valid Path = '%s'\n", (tmp==NULL?"NONE":tmp));
    errCloseStack();
    tmp = NULL;
    strcpy(string, "../:/home/$INTROOT/errors/:/home/$MCSROOT/errors/");
    printf("Tested Path = '%s' - '%s'\n", string, "miscErrors.xml");
    tmp = miscLocateFileInPath(string, "miscErrors.xml");
    printf("Valid Path = '%s'\n", (tmp==NULL?"NONE":tmp));
    errCloseStack();
    printf("\n\n");

    /* Test of miscLocateFile() */
    printf("miscLocateFile() Function Test :\n\n");
    printf("   ---------------------------------------------------------\n");
    printf("Tested File = \"%s\" ", "NULL");
    if ((tmp = miscLocateFile(NULL)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "miscErrors");
    printf("Tested File = \"%s\" ", string);
    if ((tmp = miscLocateFile(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "miscErrors.xml");
    printf("Tested File = \"%s\" ", string);
    if ((tmp = miscLocateFile(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "miscConfig.cfg");
    printf("Tested File = \"%s\" ", string);
    if ((tmp = miscLocateFile(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    printf("\n\n");

    /* Test of miscLocateDir() */
    printf("miscLocateDir() Function Test :\n\n");
    printf("   ---------------------------------------------------------\n");
    printf("Tested Dir = \"%s\" ", "NULL");
    if ((tmp = miscLocateDir(NULL)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "miscErrors");
    printf("Tested Dir = \"%s\" ", string);
    if ((tmp = miscLocateDir(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "err");
    printf("Tested Dir = \"%s\" ", string);
    if ((tmp = miscLocateDir(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "errors");
    printf("Tested Dir = \"%s\" ", string);
    if ((tmp = miscLocateDir(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "templates");
    printf("Tested Dir = \"%s\" ", string);
    if ((tmp = miscLocateDir(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "etc");
    printf("Tested Dir = \"%s\" ", string);
    if ((tmp = miscLocateDir(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }


    exit (EXIT_SUCCESS);
}

/*___oOo___*/
