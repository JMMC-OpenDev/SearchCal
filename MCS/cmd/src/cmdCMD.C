/*******************************************************************************
* JMMC project
*
* "@(#) $Id: cmdCMD.C,v 1.1 2004-11-19 16:29:40 mella Exp $"
*
* who       when         what
* --------  -----------  -------------------------------------------------------
* mella     15-Nov-2004  Created
*
*
*******************************************************************************/
/**
 * \file
 * cmdCMD class definition.
 * \todo get Default value from cdf
 * \todo perform better check for argument parsing
 */

static char *rcsId="@(#) $Id: cmdCMD.C,v 1.1 2004-11-19 16:29:40 mella Exp $"; 
static void *use_rcsId = ((void)&use_rcsId,(void *) &rcsId);


/* 
 * System Headers 
 */
#include <iostream>
using namespace std;


/*
 * MCS Headers 
 */
#include "mcs.h"
#include "misc.h"
#include "log.h"
#include "err.h"


/*
 * Local Headers 
 */
#include "cmd.h"
#include "cmdCMD.h"
#include "cmdPrivate.h"

/*
 * Class constructor
 */

/** 
 *  Constructs a new command.
 *
 * \param name  the name of the command (mnemonic).
 * \param params  the arguments of the command.
 *
 */
cmdCMD::cmdCMD(string name, string params)
{
    logExtDbg("cmdCMD::cmdCMD()");
    _name = name;
    _params = params;
    _hasNotBeenYetParsed = mcsTRUE;
}


/*
 * Class destructor
 */

/** 
 *  Destructor.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
cmdCMD::~cmdCMD()
{
    logExtDbg("cmdCMD::~cmdCMD()");
    // \todo delete _children ...
}


/*
 * Public methods
 */


/** 
 *  Set the description of the command.
 *
 * \param desc  the description string.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::setDesc(string desc)
{
    logExtDbg ("cmdCMD::setDescription()");
    _desc=desc;
    return SUCCESS;
}

/** 
 *  Add a new parameter to the command.
 *
 * \param param the parameter to add.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::addParam(cmdPARAM *param)
{
    logExtDbg ("cmdCMD::addParam()");
    _children.insert( make_pair(param->getName(), param) );
    return SUCCESS;
}

/** 
 *  Get the parameter associated to paramName.
 *
 * \param paramName  the name of the requested parameter.
 * \param param  a pointer where to store the parameter pointer
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 *  param should be considered valid only on SUCCESS case.
 */
mcsCOMPL_STAT cmdCMD::getParam(string paramName, cmdPARAM **param)
{
    logExtDbg ("cmdCMD::getParam()");
    STRING2PARAM::iterator iter = _children.find(paramName);
  
    if(iter!= _children.end())
    {
        *param = iter->second;
        return SUCCESS;
    }
    else
    {
        // \todo errAdd
        return FAILURE;
    }
}

/** 
 *  Return the help of the command.
 *
 *  \returns the help string.
 */
string cmdCMD::getHelp()
{
    logExtDbg ("cmdCMD::getHelp()");

    string s;
    
    if( _hasNotBeenYetParsed )
    {
        if( parse() == FAILURE )
        {
            s.append("Sorry help can't be generated because an error occured during parsing\n");        
            return s;
        }
    }
    
    s.append("Help for ");
    s.append(_name);
    s.append(":\n");
   
    // append description of command
    if (_desc.empty())
    {
        s.append("Sorry, no description found.");
    }
    else
    {
        s.append("Description:\n");
        s.append(_desc);
    }
    s.append("\n");
    
    // append help for each parameter if any
    if (_children.size()>0)
    {
        s.append("Help on parameters:\n");
        STRING2PARAM::iterator i = _children.begin();
        while(i != _children.end())
        {
            cmdPARAM * child = i->second;
            string childHelp = child->getHelp();
            if ( ! childHelp.empty() )
            {
                s.append(childHelp);
                s.append("\n");
            }
            i++;
        }
    }
    else
    {
        s.append("This command takes no parameter\n");
    }

    return s;
}

/*
 * Protected methods
 */



/*
 * Private methods
 */

/** 
 *  This method should be called before any real action on any parameter.
 *  It calls  parseCdf and  parseParams.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::parse()
{
    logExtDbg ("cmdCMD::parse()");
    
    // find the correcsponding cdf file
    string filename = _name;
    filename.append(".cdf");
    char * cdfFilename = miscLocateFile(filename.data());
    // check if the cdf file has been found   
    if( cdfFilename == NULL)
    {
        return FAILURE;
    }
        
    if(parseCdf(cdfFilename)==FAILURE)
    {
        return FAILURE;
    }
    
    if(parseParams()==FAILURE)
    {
        return FAILURE;
    }

    if (checkParams() == FAILURE)
    {
        return FAILURE;
    }
        
    // and flag a right performed parsing only after this point
    _hasNotBeenYetParsed = mcsFALSE;
    return SUCCESS;
}

/** 
 *  This method should be called before any real action on any parameter.
 *  It parses the cdf file and the parameters given to the constructor.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::parseParams()
{
    logExtDbg ("cmdCMD::parseParams()");

    cout << "working on " << _params << endl;
   
    string::iterator i = _params.begin();
    int posA=0;
    int posB=0;

    // we start walking out of a parameter value.
    mcsLOGICAL valueZone=mcsFALSE;

    while(i != _params.end())
    {
        if(*i=='-')
        {
            if(! valueZone){
                cout << "posA=" << posA << " posB=" <<  posB <<endl;
                if (posA>0)
                {
                    if(parseTupleParam(_params.substr(posB, posA-posB))==FAILURE)
                    {
                        return FAILURE;
                    }
                }
                posB=posA;
            }
        }

        // if double quotes are encountered it opens or closes a valueZone
        if(*i=='"')
        {
            valueZone = ! valueZone;
        }
        
        i++;
        posA++;
    }
    // parse last tuple
    if(parseTupleParam(_params.substr(posB, posA-posB))==FAILURE)
    {
        return FAILURE;
    }

    return SUCCESS;
}

/** 
 *  Parse a tuple (-<paramName> <paramValue>). Tuples are extracted from the line of parameter by the
 *  parseParams method.
 *
 *  \param tuple  the name value tuple for the a parameter.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::parseTupleParam(string tuple)
{
    logExtDbg ("cmdCMD::parseTupleParam()");
    
    int dashPos = tuple.find_first_of("-");
    int endPos = tuple.find_last_not_of(" ");
    string str = tuple.substr(dashPos, endPos+1);

    int spacePos = str.find_first_of(" ");
    /* start from 1 to remove '-' and remove the last space*/
    /* \todo enhance code to accept more than one space between name and value */
    string paramName = str.substr(1,spacePos-1);
    string paramValue = str.substr(spacePos+1);
    
    cout << "tuple: ["<< str << "]: " << paramName<<"," << paramValue <<endl;
   
    cmdPARAM *p;
    /* If parameter does'nt exist in the cdf */
    if(getParam(paramName,&p) == FAILURE)
    {
        return FAILURE;
    }
    
    cout<<    p->getHelp()<<endl;
    p->setUserValue(paramValue);
    
    return SUCCESS;
}

/** 
 *  Parse a cdf file and build new parameters for the command.
 *  After this step the parameters can be parsed.
 *
 * \param cdfFilename  the cdf file name.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::parseCdf(string cdfFilename)
{
    logExtDbg ("cmdCMD::parseCdf()");

    const char *xmlFilename = cdfFilename.data();
    GdomeDOMImplementation *domimpl;
    GdomeDocument *doc;
    GdomeElement *root=NULL;
    GdomeException exc;

    logDebug("Using cdf file %s",xmlFilename);
    
    /* Get a DOMImplementation reference */
    domimpl = gdome_di_mkref ();

    /* create a new Document from the cdf file */
    doc = gdome_di_createDocFromURI(domimpl, xmlFilename, GDOME_LOAD_PARSING,
                                    &exc);
    if (doc == NULL)
    {
        logWarning ("Illegal format encountered for cdf file "
                    "'%.100s'. DOMImplementation.createDocFromURI() failed "
                    "with exception #%d", xmlFilename, exc);
        goto errCond;
    }

    /* Get reference to the root element of the document */
    root = gdome_doc_documentElement (doc, &exc);
    if (root == NULL) {
        logWarning ("Illegal format encountered for cdf file "
                    "'%.100s'. Document.documentElement() failed "
                    "with exception #%d", xmlFilename, exc);
        goto errCond;
    }

    /* Parse for Description */
    if(parseCdfForDesc(root)==FAILURE){
        goto errCond;
    }
    
    /* Parse for Parameters */
    if(parseCdfForParameters(root)==FAILURE){
        goto errCond;
    }

//endCond:
    /* Free the document structure and the DOMImplementation */
    gdome_el_unref(root, &exc);
    gdome_doc_unref (doc, &exc);
    gdome_di_unref (domimpl, &exc);
    xmlCleanupParser();
    return SUCCESS;

errCond:
    /* Free the document structure and the DOMImplementation */
    gdome_el_unref(root, &exc);
    gdome_doc_unref (doc, &exc);
    gdome_di_unref (domimpl, &exc);
    xmlCleanupParser();
    return FAILURE;
}

/** 
 *  Parse the cdf document to extract description.
 *
 * \param root  the root node of the cdf document.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::parseCdfForDesc(GdomeElement *root)
{
    logExtDbg ("cmdCMD::parseCdfForDesc()");

    GdomeElement *el,*el2;
    GdomeNodeList *nl;
    GdomeException exc;
    GdomeDOMString *name, *value;
    int nbChildren;
    
    name = gdome_str_mkref ("desc");
    /* Get the reference to the childrens NodeList of the root element */
    nl = gdome_el_getElementsByTagName (root, name, &exc);
    gdome_str_unref(name);
    if (nl == NULL)
    {
        logWarning ("Illegal format encountered for cdf file "
                    ". Element.childNodes() failed "
                    "with exception #%d", exc);
        gdome_nl_unref(nl, &exc);
        return FAILURE;
    }
 
    nbChildren = gdome_nl_length (nl, &exc);

    /* if a desc does exist */
    if(nbChildren > 0)
    {
        el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
        if (el == NULL)
        {
            logWarning ("Illegal format encountered for cdf file "
                        ". NodeList.item(%d) failed "
                        "with exception #%d", 0, exc);
            return FAILURE;
        }
        el2=(GdomeElement *)gdome_el_firstChild(el, &exc);
        value=gdome_el_nodeValue(el2,&exc);
        setDesc(value->str);
        gdome_str_unref(value);
        gdome_el_unref(el2, &exc);
        gdome_el_unref(el, &exc);
    }
    
    gdome_nl_unref(nl, &exc);
    
    return SUCCESS;
}

/** 
 *  Parse the cdf document to extract the parameters.
 *
 * \param root  the root node of the cdf document.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::parseCdfForParameters(GdomeElement *root)
{
    logExtDbg ("cmdCMD::parseCdfForParameters()");
    
    GdomeElement *el;
    GdomeNodeList *nl;
    GdomeException exc;
    GdomeDOMString *name;
    int nbChildren, i;
    
    name = gdome_str_mkref ("params");
    /* Get the reference to the childrens NodeList of the root element */
    nl = gdome_el_getElementsByTagName (root, name, &exc);
    gdome_str_unref(name);
    if (nl == NULL)
    {
        logWarning ("Illegal format encountered for cdf file "
                    ". Element.childNodes() failed "
                    "with exception #%d", exc);
        gdome_nl_unref(nl, &exc);
        return FAILURE;
    }
 
    nbChildren = gdome_nl_length (nl, &exc);
    /* if params does exist */
    if(nbChildren > 0)
    {
        el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
        if (el == NULL)
        {
            logWarning ("Illegal format encountered for cdf file "
                        ". NodeList.item(%d) failed "
                        "with exception #%d", 0, exc);
            return FAILURE;
        }
        gdome_nl_unref(nl, &exc);

        /* Get the reference to the list of param elements */
        name = gdome_str_mkref ("param");
        nl = gdome_el_getElementsByTagName (el, name, &exc);
        gdome_str_unref(name);
        if (nl == NULL)
        {
            logWarning ("Illegal format encountered for cdf file "
                        ". Element.childNodes() failed "
                        "with exception #%d", exc);
            gdome_el_unref(el, &exc);
            gdome_nl_unref(nl, &exc);
            return FAILURE;
        }

        nbChildren = gdome_nl_length (nl, &exc);
        /* if param does exist */
        if(nbChildren > 0)
        {
            for (i=0;i<nbChildren;i++){
                el = (GdomeElement *)gdome_nl_item (nl, i, &exc);
                if (el == NULL)
                {
                    logWarning ("Illegal format encountered for cdf file "
                                ". NodeList.item(%d) failed "
                                "with exception #%d", i, exc);
                    return FAILURE;
                }

                cout << "parameter " <<i <<endl;
                if(parseCdfForParam(el)==FAILURE)
                {
                    gdome_el_unref(el, &exc);
                    return FAILURE;
                }
            }
        }
    }
    return SUCCESS;
}
/** 
 * Get the content of the first child node using tagName. If tagName is empty
 * the first child is used without any care of the child's tag.
 *
 * \param parentNode the parent node. 
 * \param tagName  the tag of the child or empty.
 * \param content  the storage string.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::cmdGetNodeContent(GdomeElement *parentNode, string tagName, string &content)
{
    logExtDbg("cmdCMD::cmdGetNodeContent()");
    logDebug("searching content for '%s' element\n",tagName.data());
    
    GdomeElement *el, *el2;
    GdomeNodeList *nl;
    GdomeException exc;
    GdomeDOMString *name, *value;
    int nbChildren;
    
   if(tagName.empty())
   {
        nl = gdome_el_childNodes(parentNode, &exc);
        cout<<"empty"<<endl;
   }else{
       name = gdome_str_mkref (tagName.data());
       /* Get the reference to the childrens NodeList of the root element */
       nl = gdome_el_getElementsByTagName (parentNode, name, &exc);
       gdome_str_unref(name);
   }
    
   if (nl == NULL)
    {
        logWarning ("Illegal format encountered for cdf file "
                    ". Element.childNodes() failed "
                    "with exception #%d", exc);
        gdome_nl_unref(nl, &exc);
        return FAILURE;
    }
    nbChildren = gdome_nl_length (nl, &exc);
    /* if a desc does exist */
    if(nbChildren > 0)
    {
        el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
        if (el == NULL)
        {
            logWarning ("Illegal format encountered for cdf file "
                        ". NodeList.item(%d) failed "
                        "with exception #%d", 0, exc);
            return FAILURE;
        }
        el2=(GdomeElement *)gdome_el_firstChild(el, &exc);

        if(el2 == NULL)
        {
            // \todo errAdd
            return FAILURE;
        }
        
        value=gdome_el_nodeValue(el2,&exc);
        content.append(value->str);
        
        gdome_str_unref(value);
        gdome_el_unref(el2, &exc);
        gdome_el_unref(el, &exc);
    }
    else
    {
        // \todo add error
        return FAILURE;
    }
    
    gdome_nl_unref(nl, &exc);
   
    return SUCCESS;
}

/** 
 *  Parse the cdf document to extract the parameters.
 *
 *  \param param  one param node of the cdf document.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::parseCdfForParam(GdomeElement *param)
{
    logExtDbg ("cmdCMD::parseCdfForParam()");
    string name;
    string desc;
    string type;
    string defaultValue;
    string unit;
    
    /* get mandatory name */
    if( cmdGetNodeContent(param, "name", name)==FAILURE )
    {
        // \todo add error
        return FAILURE;   
    }
    /* get mandatory type */
    if( cmdGetNodeContent(param, "type", type)==FAILURE )
    {
        // \todo add error
        return FAILURE;   
    }
    
    /* get optional description */
     cmdGetNodeContent(param, "desc", desc);
    /* get optional unit */
    cmdGetNodeContent(param, "unit", unit);
   
    /* get optional defaultValue */
    { 
        GdomeNodeList *nl;
        GdomeElement *el;
        GdomeException exc;
        GdomeDOMString *name;
        int nbChildren;

        name = gdome_str_mkref ("defaultValue");
        /* Get the reference to the defaultValue elements  */
        nl = gdome_el_getElementsByTagName (param, name, &exc);
        gdome_str_unref(name);
        if (nl == NULL)
        {
            logWarning ("Illegal format encountered for cdf file "
                        ". Element.childNodes() failed "
                        "with exception #%d", exc);
            gdome_nl_unref(nl, &exc);
            return FAILURE;
        }
        nbChildren = gdome_nl_length (nl, &exc);
        /* if a defaultValue does exist */
        if(nbChildren > 0)
        {
            /* get defaultValue element */
            el = (GdomeElement *)gdome_nl_item (nl, 0, &exc);
            gdome_nl_unref(nl, &exc);
            if (el == NULL)
            {
                logWarning ("Illegal format encountered for cdf file "
                            ". NodeList.item(%d) failed "
                            "with exception #%d", 0, exc);
                gdome_el_unref(el, &exc);
                return FAILURE;
            }
            if( cmdGetNodeContent(el, type, defaultValue) == FAILURE )
            {
                // \todo add error
                gdome_el_unref(el, &exc);
                return FAILURE;   
            }
            gdome_el_unref(el, &exc);
        }
        else
        {
            // there should not be any defaultValue
            cout << "no defaultValue found " <<endl;
        }


    }

    cmdPARAM *p = new cmdPARAM(name, desc, unit, mcsFALSE);
    
    addParam(p);
    
    return SUCCESS;
}


/** 
 *  Check if all mandatory parameters have a user value.
 *
 *  \returns an MCS completion status code (SUCCESS or FAILURE)
 */
mcsCOMPL_STAT cmdCMD::checkParams(){
    logExtDbg("cmdCMD::checkParams()");

    return SUCCESS;
}


/*___oOo___*/
