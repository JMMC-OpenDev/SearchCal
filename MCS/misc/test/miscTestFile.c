/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

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

    /* Test of miscLocateExe() */
    printf("miscLocateExe() Function Test :\n\n");
    printf("   ---------------------------------------------------------\n");
    printf("Tested Exe = \"%s\" ", "NULL");
    if ((tmp = miscLocateExe(NULL)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "miscLocateDir");
    printf("Tested Exe = \"%s\" ", string);
    if ((tmp = miscLocateExe(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "sclsvrServer");
    printf("Tested Exe = \"%s\" ", string);
    if ((tmp = miscLocateExe(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "logManager");
    printf("Tested Exe = \"%s\" ", string);
    if ((tmp = miscLocateExe(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "cvs");
    printf("Tested Exe = \"%s\" ", string);
    if ((tmp = miscLocateExe(string)) == NULL)
    {
        printf(" -> DOESN'T EXIST\n");
        errCloseStack();
    }
    else
    {
        printf(" -> EXIST at : %s\n", tmp);
    }
    strcpy (string, "error");
    printf("Tested Exe = \"%s\" ", string);
    if ((tmp = miscLocateExe(string)) == NULL)
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
