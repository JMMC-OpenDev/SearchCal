/*******************************************************************************
 * E.S.O. - VLT project
 *
 *  "@(#) $Id: ctooDoManPages.c,v 1.1 2004-06-24 17:32:22 gzins Exp $"
 * 
 * who        when       what
 * ---------  --------   ----------------------------------------------
 * gzins      24/06/2004 Created from VLT docDoManPages utility
 */

/************************************************************************
 *   NAME
 *   ctooDoManPages - format documentation from text files (C-source, 
 *   script, etc) into man/Xman (nroff)
 * 
 *   SYNOPSIS
 *   ctooDoManPages  <input_file>  <manSection>  [<lastChange>]
 * 
 *   DESCRIPTION
 *   This utility is used in the preparation of the Reference Manual on on-line
 *   man/xman format. 
 *
 *   The original information is extracted from either a C source file or a
 *   shell script and formatted into one or more nroff-files readable by man
 *   and xman utilities.
 *
 *
 *   --------INPUT FORMAT----------
 *
 *   The input file must be formatted as follows:
 *
 *             + - - - - - - - - - - - - - - - - - - - - - - - - 
 *             |  .
 *             |  .
 *     begin-->| -(+)-NAME
 *             |      routine1, routine2 - brief description
 *             |      
 *             |      SECTION
 *             |      .............
 *             |      .............
 *             |  
 *             |      SECTION
 *             |      .............
 *             |      .............
 *             |           . 
 *       end-->|      ----------------  or   ****************
 *             |  .
 *             |  .
 *             |  .
 *             + - - - - - - - - - - - - - - - - - - - - - - - - 
 *                  (+) the position of NAME is considered for the following 
 *                      lines as the left margin
 *                      
 *      In detail:
 *
 *      - the man page is started by a line containing ONLY the keyword NAME 
 *        and is terminated by a line of at least 40 "-" or "*".
 *        The column position of NAME is assumed as left margin for the 
 *        following line of text. Everything farther left is ignored!!!
 *        (<TAB>s are expanded into blanks, before being interpreted, 
 *         using a tabbing step of 8).
 *
 *
 *      - the text following NAME must be in the format:
 *         
 *              name1[, name2, name3, ...] - brief description
 *                   |<-- (optional) --->|
 *
 *        For long list of names, the text can be split over multiple lines.
 *
 *        According to the name list:
 *          - one man page and one text file for each word processor format 
 *            is created with the text in the source file; these files
 *            are named using the name1 keyword.
 *          - one man page and one text file for each word processor format 
 *            is created for each of the following "namex" of the list;
 *            these file are named using the namex keyword and refer
 *            to the first file (full text).
 *
 *        (See below for more details on output file names and content)
 *
 *      - a line of only UPPERCASE characters is interpreted as the start of a
 *        text section (blanks, tabs, "*"s, "#"s at the beginning are ignored).
 *
 *      - each following line that does not begin a section is stripped off 
 *        the left margin and transferred to the output; the original line 
 *        break and justification is kept.
 *
 *      NOTE1: blank lines between section title and text are ignored.
 *
 *      NOTE2: section titles without text are not transferred to output.
 *             (this allows you to leave the titles as a reminder of the
 *              original template, and fill them later). 
 *             A warning is issued for such sections.
 *
 * 
 *   --------OUTPUT FORMAT----------
 *
 *   "ctooDoManPages" assumes the existence of "man" and "doc" directories
 *   at the same level of your current working directory: 
 *   (The existence of the target directories is checked at the beginning.)
 *
 *
 *        .../modulezzz/
 *                     |-----xxxxxx/
 *                     |           /--- input_file.c   <-- file to be processed
 *                     |                                  
 *                     |-----kkkkkk/
 *                     |        .
 *                     |        .
 *                     |
 *                     |-------man/
 *                     |          |-----man1/
 *                     |          |         |-----aaaa.1    <-- manpages
 *                     |          |         |-----bbb1.1
 *                     |          |         |-----bbb2.1
 *                     |          |         .
 *                     |          |
 *                     |          |-----man2/
 *                     |          |  
 *                     |          |-----man3/
 *                     |
 *                     |
 *                     |-------ctoo/
 *                     |          |-----aaaa.inc          <-- wordprocessor
 *                     |          |-----aaaa.text             files (LaTeX)
 *                     |          |-----bbb1.inc          
 *                     |          |-----bbb2.inc
 *                     |          |-----bbb.text 
 *                     |          .
 *                     |          .
 *                     |          .
 *                     |          |-----aaaa.mif          <-- wordprocessor
 *                     |          |-----bbb1.mif              files (FrameMaker)
 *                     |          |-----bbb2.mif          
 *                     |          .
 *                     .
 *
 *
 *   ---Man Page files---
 *   The files are in nroff format and are created under the local ../man
 *   directory.
 *   According to the name list:
 *      - a man page ../man/man<manSection>.name1.<manSection> is created with 
 *        the text in the source file
 *        Some simple filtering is applied to the text for compatibility with
 *        nroff rules (e.g., \ is transformed into \\)
 *      - a man page ../man/man<manSection>.namex.<manSection> is created for 
 *        each "namex" of the list. These files refer to the first file
 *        (full text): 
 *                        ".so man<manSection>/name1.<manSection>"
 *
 *
 *   The command parameters are the following:
 *   <input_file>  the ASCII file containing the source to be analyzed.
 *                 The file is searched in the current directory.
 *                        ---- DO NOT SPECIFY THE PATH!!! ---- 
 *
 *   <manSection>  a number indicating the section to which the page belongs.
 *
 *   <lastChange>  (optional) a string that will appear at the bottom of each
 *                 manual page after the label "Last change".
 *                 You should indicate at least the module name and version.
 *                 If omitted, "development" is used.
 *
 *   COMMANDS
 *
 *   RETURN VALUES
 *
 *   EXIT_SUCCESS if there are no errors (header may be not found)
 *   EXIT_FAILURE for system or input format error
 *   
 *   Additional diagnostic messages are printed by the utility.
 *
 *
 *   CAUTION
 *   The templates described in the VLT Programming Standards 1.0 10/03/93
 *   are NOT fully compliant with the above mentioned rules.
 *
 *   Do not start a line of text with "." or "'"(single quote): nroff will 
 *   take this line as a command with umpredictable results.
 *
 *
 *   EXAMPLES
 *   The command:   ctooDoManPages  myfile.c  2  "CCS/1.1 - 5 september 93"
 *
 *         ./myfile.c
 *             +----------------------------------
 *             |    .
 *             | *
 *             | * NAME
 *             | * routine1, routine_2 - two simple routines
 *             | * 
 *             | * SYNOPSIS
 *             |    .
 *             |    .
 *             +----------------------------------
 *
 *   will produce the output files:
 *
 *          ../man/man2/routine1.2
 *             +----------------------------------
 *             |.\" ....prolog ...
 *             |.\"
 *             |.TH routine1 2 "CCS/1.1 - 5 september 93"
 *             |.SH NAME
 *             |routine1, routine_2 - two simple routines
 *             |.SH SYNOPSIS
 *             |.nf 
 *             |.LP 
 *             |.bf B
 *             |     .
 *             |     .
 *             +----------------------------------
 *             
 *          ../man/man2/routine_2.2
 *             +----------------------------------
 *             |.\" ....prolog ...
 *             |.\"
 *             |.so man2/routine1.2
 *             +----------------------------------
 *
 *   SEE ALSO
 *
 *   BUGS
 *   In order to keep the complexity of this formatter to a reasonable level,
 *   the parsing capability and the accepted level of freedom the input
 *   file are limited. So format your input file strictly according to rules. 
 *
 *   The following are known restrictions that now apply and might be
 *   removed in the next release.
 *
 *   1) The utility does not check the presence of the mandatory 
 *      sections:NAME, SYNOPSIS and DESCRIPTION.
 *
 *   3) the set of filters appl is now very limited. New filters will be added
 *      as the need arises.
 *
 *------------------------------------------------------------------------
 */


static char *rcsId="@(#) $Id: ctooDoManPages.c,v 1.1 2004-06-24 17:32:22 gzins Exp $";
static void *use_rcsId = ((void)&use_rcsId, (void *) &rcsId);

/* 
 * System Headers 
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>             /* required by malloc */
#include <sys/types.h>          /* required by stat */
#include <sys/stat.h>           /* required by stat */


/*
 * Local Headers 
 */
/* none */

#define IN
#define OUT


/*
 * Constant and type definitions
 */
#define MAXCHAR  10000              /*maximum number of characters per line  */
#define MAXNAME    100              /*maximum number of characters per name  */
/*MAXCHAR shall be greater than MAXNAME  */

#define MAXTITLES  100              /*maximum number of titles for one page  */
#define MAXSECTION 100              /*maximum number of sections in one page)*/
#define MAXLINES  1000              /*maximum number of lines per section    */
#define EOP_1 "----------------------------------------"    /*end-of-manpage */
#define EOP_2 "****************************************"    /*end-of-manpage */


typedef enum {
    START_PAGE = 1,
    START_SECTION,
    END_OF_COMMENT,
    TEXT
} LINE_ANALYSIS_RESULT;             /* result of analyzing an input line */

typedef enum {
    MANPAGE_NOT_FOUND = 1,
    MANPAGE_OPEN,
    MANPAGE_FOUND
} INPUT_ANALYSIS_RESULT;            /* result of searching for manual page */

typedef struct {
    char name[MAXNAME + 1];             /* name of the manual page section */
    char *text[MAXLINES];               /* text of the manual page section */
} SECTION;  


/* 
 * Macros
 */

void panic(char *filename, int line)
{
    fprintf(stderr, "\n?Panic in line %d of file %s\n", line, filename);
    perror("Unespected UNIX error");
    abort();
}

#define PANIC (panic(__FILE__, __LINE__))

/*
 *******************
 * Local functions *
 *******************
 */

/*
 *****************************************************************************
 */

int getLine ( FILE IN  *filein, 
             char OUT *line)
/*
 * get characters from filein until '\n' or EOF are encountered,
 * Read characters, excluding terminators, are stored into "line".
 * the value returned is
 *     EOF    if no more tokens (EOF encountered)
 *     <>EOF  if token is not the last one
 */
{
    int i=0;
    int j=0;
    int nextTab=0;
    int c;

    do
    {
        c=getc(filein);

        switch (c) 
        {
            case EOF : 
            case '\n':              /* end of line, close line */
                line[i]='\0';       
                i++;
                break; 

            case '\t':              /* expand <tab> into blanks */
                nextTab = ((((i / 8 ) + 1) * 8) - 1);
                for (j = i; j <= nextTab ; j++)   
                {
                    line[i]=' ';
                    i++;
                }
                break; 

            default:                /* insert the character into line */
                line[i]=c;
                i++;
                break;
        }

        if ( i >= MAXCHAR - 8 )     /* input line too long: */
        { 
            line[i]='\0';           /* close the line*/
            i++;
        }
    }
    while ( line[i - 1] != '\0' );

    /*
     * delete trailing blanks 
     */

    i = i - 2;

    while ( i >=0 )                
    {                         
        if ( line[i] == ' ' )
        {
            line[i] = '\0';
            i--;
        }
        else
        {
            break;
        }
    }

    return c;
}

/*
 *****************************************************************************
 */

LINE_ANALYSIS_RESULT isNAMEthere (
                                  char IN OUT  *line, 
                                  char OUT     *sectionName,
                                  int  OUT     *leftMargin)
/*
 * find out whether the line is the NAME keyword,
 * i.e., the beginning of the manpage (START_PAGE).
 * Return TEXT elsewhere.
 * Set the <leftMargin> position for further evaluation.
 */

/*
 * REMARK: This the routine has multiple exits!!!!
 */

{
    int i = 0;                      /* index & counter */

    /*
     * Is it the NAME directive?
     * -------------------------
     * find the first alphabetical char and check whether "NAME" is the only 
     * word in the line 
     */

    for (i = 0; i <= strlen(line); i++)    /* find the first alpahabetical char */
    {                         
        if (((line[i] >= 'a') && (line[i] <= 'z')) || 
            ((line[i] >= 'A') && (line[i] <= 'Z')))
        {
            if ((line[i]     == 'N') && 
                (line[i + 1] == 'A') &&
                (line[i + 2] == 'M') &&
                (line[i + 3] == 'E') &&
                (line[i + 4] == '\0'))     /* !trailing blanks have been skipped*/
            {
                *leftMargin = i;           /* NAME define the left margin */
                strcpy(sectionName, "NAME");
                return START_PAGE;
            }
            else
            {
                return TEXT;
            }
        }
    }

    return TEXT;
}

/*
 *****************************************************************************
 */

LINE_ANALYSIS_RESULT treatLine (
                                char IN OUT  *line, 
                                char OUT     *sectionName,
                                int  IN      *leftMargin)
/*
 * Once the beginning of the manpage has been found out, sort the line as:
 *   - a text line (TEXT)
 *   - the beginning of another section (START_SECTION)
 *   - the end of the manpage (END_OF_COMMENT)
 *
 * The first <leftMargin> positions are not considered.
 */

/*
 * REMARK: This the routine has multiple exits!!!!
 */

{
    int i = 0;                      /* index & counter */

    /*
     * stripping the first <leftMargin> characters. Terminate if shorter.
     */
    if (strlen(line) > *leftMargin)       
        for (i = *leftMargin; i <= strlen(line); i++)  
        {
            line[i - *leftMargin] = line[i];
        }
    else
    {
        line[0] = '\0';
        return TEXT;                /* empty lines are considered TEXT */
    }

    /*
     * Is it the end of the man page header (a separator of "-"s or "*"s)?
     * -------------------------------------------------------------------
     */
    if ((strstr(line, EOP_1) != NULL) ||
        (strstr(line, EOP_2) != NULL))
    {
        return END_OF_COMMENT;
    }

    /*
     * Does it start a new section?
     * ----------------------------
     * By convention, a section header is a line of UPPERCASE words (A-Z," ")!!!
     */

    i = 0;
    while (i < strlen(line))  
    {                         
        if ((line[i] == ' ') || 
            ((line[i] >= 'A') && (line[i] <= 'Z')) || (line[i] == '/'))
        {
            i++;
        }
        else
        {
            /*
             * If at least one character is not in the allowed set of characters, 
             * the line is a normal text line.
             */
            return TEXT; 
        }
    }
    strcpy(sectionName, line);
    return START_SECTION;
}

/*
 *****************************************************************************
 */
INPUT_ANALYSIS_RESULT readInputFile(
                                    char IN  *inputFile, 
                                    int  OUT *sectionc,
                                    SECTION OUT *section[])

/*
 * Load input file into section array. 
 *
 * <sectionc> is the number of section header that have been found
 * <section>  is the array containing the information
 *
 * section[0].name-->"NAME"      section[0].text[0]-->"routine1 - this is ...."
 *                               section[0].text[1]-->" blah blah blah ...."
 *                               section[0].text[2]-->NULL
 *   
 * section[1].name-->"SYNOPSIS"  section[1].text[0]-->""
 *                               section[1].text[1]-->" int routine1(...)"
 *                               section[1].text[2]-->NULL
 */
{
    FILE *filein;
    INPUT_ANALYSIS_RESULT result = MANPAGE_NOT_FOUND;
    char line[MAXCHAR + 1] = "";            /* working line buffer  */
    int leftMargin = 0;                     /* left margin of input text */
    int currentLine = 0;                    /* position of the next text line*/
    char sectionName[MAXNAME + 1] = "";     /* name of the encountered section */
    LINE_ANALYSIS_RESULT status = TEXT;     /* result of line analysis */


    /*
     * macros to deal with arrays
     */
#define ADD_SECTION(string)                                       \
    { (*sectionc)++;                                    \
        section[*sectionc - 1] = malloc(sizeof(SECTION)); \
            strcpy ( (char *)section[*sectionc - 1]->name, (string));    \
            currentLine = 0;                                  \
            section[*sectionc - 1]->text[currentLine] =  NULL; }

#define APPEND_TEXT_LINE(string)                                           \
        {section[*sectionc - 1]->text[currentLine] =                \
            (char *) malloc(strlen(string) + 1);  \
                strcpy(section[*sectionc - 1]->text[currentLine], string); \
                currentLine++;                                             \
                section[*sectionc - 1]->text[currentLine] = NULL;}


            /*
             * open input file
             */
            filein = fopen(inputFile, "r");
            if (filein == NULL)
            {
                printf("ERROR: cannot access source file: %s\n", inputFile);
                perror("       system error");
                exit(EXIT_FAILURE);
            }

            printf("Input file: %s\n", inputFile);

            /* 
             * process all lines in the input file
             */
            *sectionc = 0;
            while (getLine(filein, line) != EOF )
            {
                switch (result)
                {
                    case MANPAGE_NOT_FOUND:
                        status = isNAMEthere(line, sectionName, &leftMargin);
                        switch (status)
                        {
                            case START_PAGE:
                                ADD_SECTION("NAME"); 
                                result = MANPAGE_OPEN;
                                printf("       Begin of manual page found . . .");
                                break;

                            case TEXT:
                                break;              /* ignore input line */

                            default:
                                PANIC;
                                break;
                        }
                        break;
                        break;

                    case MANPAGE_OPEN:
                        status = treatLine(line, sectionName, &leftMargin);
                        switch (status)
                        {
                            case START_SECTION:
                                ADD_SECTION(sectionName);
                                break; 

                            case TEXT:
                                /*
                                 * output line to output file, empty lines are passed
                                 * to output only if they are not the first
                                 */
                                if ((strlen(line) > 0 ) || (currentLine != 0))
                                {
                                    APPEND_TEXT_LINE(&line[0]);
                                }
                                break;

                            case END_OF_COMMENT:
                                fclose(filein);
                                result = MANPAGE_FOUND;
                                printf(" end of page reached.\n");
                                return result;      /* succefull return */
                                break; 

                            default:
                                PANIC;
                                break;
                        }
                        break;

                    default:
                        PANIC;
                        break;
                }
            }

            /* 
             * EOF has been reached without finding the beginning or the end
             * of the manpage
             */

            fclose(filein);

            if (result == MANPAGE_OPEN)
            {
                /*
                 * produce warning on both terminal and output files 
                 */
                APPEND_TEXT_LINE("");
                APPEND_TEXT_LINE("**********************************************************");
                APPEND_TEXT_LINE("* WARNING: check input file. End-page terminator missing *");
                APPEND_TEXT_LINE("**********************************************************");
                printf("\n");
                printf("WARNING: input file does not contain the end-of-page terminator\n");
                printf("         i.e., %s\n", EOP_1);
                printf("            or %s\n", EOP_2); 
                result = MANPAGE_FOUND;
            }

            return result;                  /* abnormal  return */
}


/*
 *****************************************************************************
 */

void getTitle(
              char IN  *line, 
              int  OUT *titlec,
              char OUT *titlev[])
/*
 * analyze the line to extract the title(s) of the man page(s)
 * The line is expected to be in the format:
 *
 *               name1[, name2, name3, ...] - brief description
 *                    |<---optionally --->|
 *
 * titlec is the number of titles that have been found
 * titlev is the array containing the titles.
 *   In the exaple would be:
 *       titlec = 3      titlev[0] = "name1"
 *                       titlev[1] = "name2"
 *                       titlev[2] = "name3"
 */
{
    int i = 1;                      /* index & counter */
    int j = 0;                      /* index & counter */
    char title[MAXCHAR + 1] = "";   /* working line buffer  */
    typedef enum {
        OPEN_TITLE = 1,
        NO_OPEN_TITLE
    } TITLE_STATUS;
    TITLE_STATUS status = NO_OPEN_TITLE;

    /*
     * Algorithm:
     *   read a character from line:
     *    - the character belongs to the set of allowed chars [a-z] [A-Z],
     *      [0-9], ".", "_")
     *         OPEN_TITLE: append the char to current title
     *         NO_OPEN_TITLE: initialize a new title,
     *                        append the char and change status -->OPEN_TITLE
     *    - the character is the terminator "-":
     *                        close current title, if any, and return
     *    - the character is not one of the prvious ones:
     *         OPEN_TITLE: close current title
     *         NO_OPEN_TITLE: ignore it
     */

    *titlec = 0;
    while (i <= strlen(line))
    {
        if (((line[i] >= 'a' ) && (line[i] <= 'z' )) ||
            ((line[i] >= 'A' ) && (line[i] <= 'Z' )) || 
            ((line[i] >= '0' ) && (line[i] <= '9' )) ||
            (line[i] == '.') ||
            (line[i] == '_'))
        {                           
            switch (status)
            {                   /* open a new title */
                case NO_OPEN_TITLE:
                    {
                        j = 0;
                        title[j] = line[i];
                        j++;
                        status = OPEN_TITLE;
                        break;
                    }               
                case OPEN_TITLE:
                    {               /* add char to current title */
                        title[j] = line[i];
                        j++;
                        break;
                    }
                default:
                    PANIC;
                    break;
            }
        }
        else if (line[i] == '-')
        {                       /* close current title, if any */ 
            if (status == OPEN_TITLE)
            {
                title[j] = '\0';
                if (*titlec < MAXTITLES)
                {
                    (*titlec)++;
                }
                else 
                {
                    PANIC;
                }
                titlev[*titlec - 1] = malloc(strlen(title) + 1);
                strcpy(titlev[*titlec - 1], title);
            }               
            return;                 /* end of process */
        }

        else
        {
            switch (status)
            {
                case OPEN_TITLE:
                    {                   /* close current title */
                        (*titlec)++;
                        title[j] = '\0';
                        titlev[*titlec - 1] = malloc(strlen(title) + 1);
                        strcpy(titlev[*titlec - 1], title);
                        status = NO_OPEN_TITLE;
                        break;
                    }
                case NO_OPEN_TITLE:
                    {
                        break;              /* ignore the char */
                    }
                default:
                    PANIC;
                    break;
            }
        }
        i++;                        /* point to next char */
    }

    printf("ERROR: No terminator (-) found. Check input format:\n");
    printf("           NAME\n");
    printf("           name1[, name2, ...] - ...... \n ");
    printf("                               ^        \n ");
    printf("                               |        \n ");
    printf("                     may be this is missed\n");
    printf(" or there is a blank line between NAME and the brief description.\n");
    exit(EXIT_FAILURE);

}

/*
 *****************************************************************************
 */
FILE *openManFile(
                  char IN *outputName,
                  char IN *source,
                  char IN *formatterVersion)
/*
 * - open an output file named as <outputName>
 *
 * - initialize the file with the prolog : .\"....
 *                                         .\"....
 */
{
    FILE *manfile;

    manfile = fopen(outputName, "w");
    if (manfile == NULL)
    {
        printf("ERROR: cannot open output file: %s\n", outputName);
        perror("       system error");
        exit(EXIT_FAILURE);
    }

    /*
     * WARNING: the following sentence is used by vltMakefile to find which
     *          files have been written by docDoManPage when a "make clean" is
     *          done Do not change it without updating vltMakefile
     *          accordingly.
     */
    fprintf(manfile, ".\\\" This Man Page has been automatically produced.");
    fprintf(manfile, " DO NOT EDIT THIS FILE!!!!!!\n");
    fprintf(manfile, ".\\\" Input file:  %s\n", source);
    fprintf(manfile, ".\\\" Formatter :  %s\n", formatterVersion);
    fprintf(manfile, ".\\\" -----------------------------------------------\n");

    return manfile;
}

/*
 *****************************************************************************
 */

void outputNroffLine(
                     FILE *outputfile,
                     char *line)
/*
 * output line to output file in nroff format.
 * Simple filtering is applied to the text for compatibility with nroff
 * At present:   "\" is transformed into "\\"
 *               a line beginning with "." (like ./path) or "'" begins with a  
 *               blank (otherwise nroff ignores it).
 */
{
    int i = 0;                              /* index & counter */


    for (i = 0; i < strlen(line); i++)
    {
        if (line[i] == '\\')
        {
            fprintf(outputfile,"\\\\");
        }
        else if ( i == 0 && line[0] == '.')
        {
            fprintf(outputfile," .");
        }
        else if ( i == 0 && line[0] == '\'')
        {
            fprintf(outputfile," '");
        }
        else 
        {
            fprintf(outputfile,"%c", line[i]);
        }
    }

    fprintf(outputfile,"\n");

    return; 
}



/* 
 *****************************************************************************
 * Main
 *****************************************************************************
 */

int main (int argc, char *argv[])
{

    int i = 0;                           /* index & counter */
    int j = 0;                           /* index & counter */
    char line[MAXCHAR + 1] = "";         /* working line buffer  */

    char inputFile[MAXNAME + 1] = "";    /* input file name*/
    char manSection[MAXNAME + 1] = "";   /* name of the man subdirectory */
    char lastChange[MAXNAME + 1] = "";   /* change reason */

    FILE *manfile = NULL;                /* output file (man page) */ 

    int     sectionc = 0;                /* number of sections in the manpage*/
    SECTION *section[MAXSECTION];        /* sections found in the input file*/

    int   titlec = 0;                    /* number of titles found after NAME*/
    char *title[MAXTITLES];              /* titles found after NAME */


    char mandir[MAXNAME + 1] = "";       /* target man page(s) directory */
    struct stat dirStatus;               /* used to check man&doc directories*/
    char outputName[MAXNAME + 1] = "";   /* buffer for output file names */

    INPUT_ANALYSIS_RESULT result = MANPAGE_NOT_FOUND;
    /* result of input file analysis */

    /* 
     ***************************************************************************
     * Program Overview
     * 
     *  - preliminary checks: correct input parameter number, accessibility to 
     *                        output directories, etc.
     *  - look for the documentation section in the input file, if found
     *    the text is loaded into memory as an array of sections. Each section 
     *    has a title and a text.
     *  - clean-up of section with empty text (i.e, only the title)
     *  - extract from the NAME section the names of the manual page(s)
     *  - output of manual pages for man/Xman (nroff format)
     *  - output of manual pages for word processor (LaTeX format)
     *
     ***************************************************************************
     */

    /* 
     ***************************************************************************
     *  - Welcome
     ***************************************************************************
     */
    printf("----------------------------------------");
    printf("----------------------------------------\n");
    printf("%s \n\n", rcsId);

    /* 
     ***************************************************************************
     *  - preliminary checks
     ***************************************************************************
     */

    /* 
     * check input parameter number (it can be 3 or 4, depending whether 
     *                               the lastChange parameter has been given)
     */

    if ((argc <= 2) || (argc >= 5))
    {
        printf("Usage:  ctooDoManPages  <input_file>  <manSection>  [<lastChange>]\n");
        exit(EXIT_FAILURE);
    }

    /* 
     * argv[1] ---> <input_file> and check its existence
     */
    strcpy(inputFile, argv[1]);
    if (stat(inputFile, &dirStatus) != 0 )
    {
        printf("ERROR: cannot access input file: %s\n", inputFile);
        perror("       system error");
        exit(EXIT_FAILURE); 
    }

    /* 
     * argv[2] ---> man pages subdirectory
     *              build man directory name and check that it is existing 
     */
    strcpy(manSection, argv[2]);        
    strcpy(mandir, "../man/man");        
    strcat(mandir, manSection);                
    if (stat(mandir, &dirStatus) != 0 )
    {
        printf("ERROR: cannot access directory: %s\n", mandir);
        perror("       system error");
        printf("           Are you sure that >>%s<<\n", manSection);
        printf("           is a legal name for a man section?\n");
        exit(EXIT_FAILURE); 
    }
    if (S_ISDIR(dirStatus.st_mode) == 0)
    {
        printf("ERROR:  %s is not a directory.\n", mandir);
        exit(EXIT_FAILURE); 
    }


    /* 
     * argv[3] ---> change reason (if not specified, a default is used )
     */
    if (argc == 4)
    {                                  
        strcpy(lastChange, argv[3]);
    }
    else
    {                                       
        strcpy(lastChange, "development");
    }


    /* 
     ***************************************************************************
     *  - extract the documentation section from the input file
     ***************************************************************************
     */
    result = readInputFile(inputFile, &sectionc, section);

    if (result == MANPAGE_NOT_FOUND)
    {
        printf("WARNING: input file does not contain the keyword NAME or\n");
        printf("         it is in the wrong format.\n\n");
        exit(EXIT_SUCCESS);
    }

    printf("\nThe input manual page contains the following section headers:\n");
    for (i=0 ; i < sectionc ; i++)
    {
        if (section[i]->text[0] != NULL)
        {
            printf("             - %s \n", section[i]->name);
        }
        else
        {
            printf("             - (%s) -----> empty (no output)\n", 
                   section[i]->name);
        }
    }

    /* 
     ***************************************************************************
     *  - extract from the NAME section the names of the manual page(s)
     ***************************************************************************
     */
    /* 
     * concatenate all text line following NAME into one line only
     */

    /* 
     * 09/08/95 GFILIPPI: the following loop can be dangerous because
     * adding lines can override the MAXCHAR limit and corrupt other 
     * data in memory. For the time being I have increased the MAXCHAR
     * limit. A future version will have a better fix.
     */
    i = 0;
    strcpy(line, "");                        
    while (section[0]->text[j] != NULL)
    {
        strcat(line, section[i]->text[j]);
        j++;
    }

    /* 
     * extract the title(s) for this manual page
     */
    getTitle(line, &titlec, title);

    if (titlec <=0 )
    {
        printf("ERROR: No title found. Check input format:\n");
        printf("           NAME\n");
        printf("           name1[, name2, ...] - ...... \n\n");
        exit(EXIT_FAILURE);
    }

    /* 
     ***************************************************************************
     *  - output of manual pages for man/Xman (nroff format)
     ***************************************************************************
     */
    /* 
     * - for the main man output file (the first name):
     *     - open the file and initialize it with the  prolog 
     *     - write the title 
     *                 .TN <name> <manSection> <lastChange>
     *     - write the NAME section   
     *                 .SH NAME
     *                 ........  
     *                 ........  
     *     - for each following section with text:
     *        - write the section header     
     *                 .SH <sectionName> 
     *        - write nroff commands to prevent justification on the following
     *          text
     *                 .nf   = no fill
     *                 .LP   = left-Block paragraph (no paragraph indent)
     *        - write the text of the section
     *                 ........    ("\" are changed into "\\")
     *                 ........   
     *     - close the file.
     *
     * - for each additional name, if any:
     *     - open the file and initialize it with the  prolog 
     *     - write the reference file 
     *              .so man<manSection>/<title1>
     *     - close the file.
     */

    printf("\nOutput man/Xman files (nroff format): \n");

    /* 
     * do main man page (name1)
     */
    strcpy(outputName, mandir);           /* build output filename:          */
    strcat(outputName, "/");              /*    <mandir>/<name>.<manSection> */
    strcat(outputName, title[0]);                
    strcat(outputName, ".");                    
    strcat(outputName, manSection);

    printf("  --> man page (by text): %s\n", outputName);

    manfile = openManFile(outputName,       /* man file name */
                          inputFile,        /* source        */
                          rcsId);           /* formatter     */

    fprintf(manfile,".TH %s %s \"%s\"\n", title[0], manSection, lastChange);
    fprintf(manfile,".SH %s \n", section[0]->name);
    j = 0;
    while (section[0]->text[j] != NULL)
    {
        outputNroffLine(manfile, section[0]->text[j]);
        j++;
    }
    for (i=1 ; i < sectionc ; i++)
    {
        if (section[i]->text[0] != NULL)    /* do not output empty section */
        {
            fprintf(manfile,".SH %s \n", section[i]->name);
            fprintf(manfile,".nf \n");           
            fprintf(manfile,".LP \n");          
            j = 0;
            while (section[i]->text[j] != NULL)
            {
                outputNroffLine(manfile, section[i]->text[j]);
                j++;
            }
        }
    }
    fclose(manfile);

    /* 
     * if any, do referencing pages (name2, name3, ....)
     */
    if (titlec > 1 )
    { 
        for (i = 1; i < titlec; i++)
        {
            strcpy(outputName, mandir);   /* build output filename:          */
            strcat(outputName, "/");      /*    <mandir>/<name>.<manSection> */
            strcat(outputName, title[i]);                
            strcat(outputName, ".");                    
            strcat(outputName, manSection);
            printf("  --> man page (by ref.): %s\n", outputName);
            manfile = openManFile(outputName,       /* man file name */
                                  inputFile,        /*source    */
                                  rcsId);           /*formatter */
            fprintf(manfile,".so man%s//%s.%s\n", 
                    manSection, title[0], manSection);
            fprintf(manfile,".\\\" _____oOo_____\n");
            fclose(manfile);
        }
    } 
    /*
     ***************************************************************************
     * - end of process
     ***************************************************************************
     */
    printf("----------------------------------------");
    printf("----------------------------------------\n\n");
    exit(EXIT_SUCCESS);

}

/*___oOo___*/

