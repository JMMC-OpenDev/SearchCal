/*******************************************************************************
* JMMC project
*
* "@(#) $Id: vobsPARSER.C,v 1.1 2004-07-13 13:41:09 scetre Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* scetre    06-Jul-2004  Created
*
*
*******************************************************************************/

static char *rcsId="@(#) $Id: vobsPARSER.C,v 1.1 2004-07-13 13:41:09 scetre Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);

/* 
 * System Headers 
 */
#include <iostream>
using namespace std;
#define MODULE_ID "vobs"

/*
 * MCS Headers 
 */
#include "mcs.h"
#include "log.h"


/*
 * Local Headers 
 */

#include"vobsPARSER.h"

/*
 * Local Variables
 */


/*
 * Local Functions
 */
// Class constructor 
vobsPARSER::vobsPARSER()
{
    nbLineToJump=0;
}

// Class destructor 
vobsPARSER::~vobsPARSER()
{
}

/**
 * main methode of vobsPARSER which return 0 when the treatment is over
 * \return 0 or 1
 */
int vobsPARSER::MainParser(char *request)
{
    logExtDbg("vobsPARSER::MainParser()");	
    GdomeDOMImplementation *domimpl;
    GdomeDocument *doc;
    GdomeElement *root;
    GdomeNodeList *childs;
    GdomeException exc;
    /* First I get a DOMImplementation reference */
    domimpl = gdome_di_mkref ();

    /* I load a new document from the URL*/
    doc = gdome_di_createDocFromURI(domimpl, request, GDOME_LOAD_PARSING, &exc);
    if (doc == NULL) {
        fprintf (stderr, "DOMImplementation.createDocFromURI: failed\n\tException #%d\n", exc);
        return 1;
    }

    /* I get reference to the root element of the document */
    root = gdome_doc_documentElement (doc, &exc);
    if (root == NULL) {
        fprintf (stderr, "Document.documentElement: NULL\n\tException #%d\n", exc);
        return 1;
    }

    /* I get the reference to the childrens NodeList of the root element */
    childs = gdome_el_childNodes (root, &exc);
    if (childs == NULL) {
        fprintf (stderr, "Element.childNodes: NULL\n\tException #%d\n", exc);
        return 1;
    }

    // I begin the recursif look of the tree
    XMLParser(childs, doc);

    logTest("there is %d column in the CDATA\n",tab.size());
    std::vector<char *>::iterator P=tab.begin();
    for (unsigned int i=0;i<tab.size();i++)
    {
        logTest("name of column %d : %s\n",i+1,*P);
        P++;
    }

    // I print the number of line we need to jump to parse CDATA
    logTest("The number of lines we need to jump to parse CDATA is %d",nbLineToJump);

    // I print CDATA
    //logTest("%s\n",CDATA);
    
    // I Parse CDATA
    if (nbLineToJump!=0)
        CDATAParser();

    // here I just see if the list of ucd is ok
    std::vector<char *>::iterator iterateur=ucd.begin();
    while (iterateur !=ucd.end())
    {
        logTest("%s \n",*iterateur);
        ++iterateur;
    }
    
    // I print listOfStar
    /*std::list<vobsCALIBRATOR_STAR>::iterator Q=listOfStar.begin();
    while (Q!=listOfStar.end())
    {
        (*Q).View();
        ++Q;
    }
    */
    /* I free the document structure and the DOMImplementation */
    gdome_di_freeDoc (domimpl, doc, &exc);
    gdome_di_unref (domimpl, &exc);
    return 0;
}

/**
 * XML Parser which return 1 when parsing succeed
 * \return 1 or 0
 */
int vobsPARSER::XMLParser(GdomeNodeList *childs, GdomeDocument *doc)
{
    logExtDbg("vobsPARSER:XMLParser()");
    /* I get all tag name for each children of the root element */
    GdomeNode *el;
    GdomeException exc;
    GdomeDOMString *name;
    GdomeNamedNodeMap *attributes;
    GdomeNode *attribute;
    unsigned long i, nchilds, j;
    GdomeNodeList *childs2;

    // I get the number of child
    nchilds = gdome_nl_length (childs, &exc);
    for (i = 0; i < nchilds; i++)
    {
        el = gdome_nl_item (childs, i, &exc);
        if (el == NULL) {
            fprintf (stderr, "NodeList.item(%d): NULL\n\tException #%d\n", (int)i, exc);
            return 1;
        }
        // I make a test to know if i am in the CDATASection
        if (gdome_n_nodeType (el, &exc) == GDOME_CDATA_SECTION_NODE) 
        {
            /* I get CDATA */
            CDATA=gdome_cds_data(GDOME_CDS(el),&exc)->str;
            return 1;
        }
        // if I'm not in CDATAsection i try to get information
        if (gdome_n_nodeType (el, &exc) == GDOME_ELEMENT_NODE) 
        {
            /* I get tag name */
            name = gdome_n_nodeName(el, &exc);
            if (exc)
            {
                fprintf (stderr, "Element.setAttribute: failed\n\tException #%d\n", exc);
                return 1;
            }
            else
            {
                /* List attributes */
                attributes = gdome_n_attributes(el, &exc);
                if (exc) 
                {
                    fprintf (stderr, "gdome_el_attributes: failed\n\tException #%d\n", exc);
                    return 1;
                }
                else
                {
                    for (j=0;j<gdome_nnm_length(attributes,&exc);j++)
                    {
                        attribute = gdome_nnm_item(attributes,j,&exc);
                        if (exc) 
                        {
                            fprintf (stderr, "gdome_nnm_item: failed\n\tException #%d\n", exc);
                            return 1;
                        }
                        else
                        {
                            // we look if the node is FIELD and if there is a name and we put the name in the stl vector of name
                            if ((strcmp(gdome_n_nodeName(attribute,&exc)->str,"name")==0)&&(strcmp(name->str,"FIELD")==0))
                            {
                                tab.push_back(gdome_n_nodeValue(attribute,&exc)->str); 
                            }
                            if ((strcmp(gdome_n_nodeName(attribute,&exc)->str,"UCD")==0)&&(strcmp(name->str,"FIELD")==0))
                            {
                                ucd.push_back(gdome_n_nodeValue(attribute,&exc)->str); 
                            }

                            if ((strcmp(gdome_n_nodeName(attribute,&exc)->str,"headlines")==0)&&(strcmp(name->str,"CSV")==0))
                                if (atoi(gdome_n_nodeValue(attribute,&exc)->str))
                                    nbLineToJump=atoi(gdome_n_nodeValue(attribute,&exc)->str);
                        }
                    }
                }
                childs2 = gdome_n_childNodes (el, &exc);
                if (childs2 == NULL) 
                {
                    fprintf (stderr, "Element.childNodes: NULL\n\tException #%d\n", exc);
                    return 1;
                }
                XMLParser(childs2,doc);
            }
        }
        gdome_n_unref (el, &exc);
        
    }
    return 1;

}

/**
 * CDATA Parser extract information by token according to the delimiter "\t".
 * It return 1 if succeed
 * \return 0 or 1
 */
int vobsPARSER::CDATAParser()
{
    logExtDbg("vobsPARSER::CDATAParser()");
    char *line=NULL;
    char *delimiters = "\t";
    char *token;
    //nbLineToJump=nbLineToJump+1;
    char *in;
    
    if (nbLineToJump==1)
        return 0;
    else
    {
        in=CDATA;

        // Read each line one at a time 
        line=strtok(in,"\n");
        for (int k=0;k<nbLineToJump-1;k++)
        {
            line=strtok(NULL,"\n");
        }
        std::vector<char *>::iterator P;
        char *nbBytes=NULL;
        nbBytes=strtok(NULL,"\n"); 
        while(nbBytes != NULL)
        {
            // Get one line 
            // Break the line up according to our delimiters
            char *tmp=nbBytes;
            char *saveptr;
            token = strtok_r(tmp, delimiters, &saveptr);
            tmp=saveptr;
            vobsCALIBRATOR_STAR calibStar;
            P=ucd.begin();
            while ((token != NULL)&&(P!=ucd.end()))
            {            
                // for each line we create a calibrator star
                // and we add the elemets in the calibrator star              
                if (strcmp(token,"      ")==0) strcpy(token,"99.99");
                calibStar.AddElement(*P,token);
                ++P; 
                logTest("%s  ",token);
                // Get the next word 
                token = strtok_r(tmp, delimiters, &saveptr);
                tmp=saveptr;
            }
            logTest("\n");
            // we put now the calibrator star in a calibrator star list
            listOfStar.push_back(calibStar);
            nbBytes=strtok(NULL,"\n");
        }
        return 0;
    }
}

// Method to return a list
std::list<vobsCALIBRATOR_STAR> vobsPARSER::GetList()
{
    return listOfStar;
}

/*___oOo___*/
