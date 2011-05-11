/*******************************************************************************
 * JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
 ******************************************************************************/

package jmmc.mcs.err;

import java.io.*;
import java.util.StringTokenizer;

import org.apache.xerces.parsers.*;
import org.w3c.dom.*;
import org.xml.sax.*;

public class ErrXMLGenerator {
    ErrTableGenerator tableGen;
    String modName;
    String schemaPath;
    private ErrMenu parent;

    /**
     * This constructor constructs an instance of ErrXMLGenerator for the
     * specified ErrTablegenerator and module name
     * @param app ErrMenu application reference
     * @param tablegen ErrTableGenerator : an ErrTableGenerator Object
     * @param modname String : the module name
     */
    public ErrXMLGenerator(ErrMenu app, ErrTableGenerator tablegen, String modname) {
        parent = app;
        tableGen = tablegen;
        modName = modname;
        //Path of the schema
        schemaPath = new String(app.MCSROOT+"/config/errXmlToH.xsd");
    }

    /**
     * This function generates the XML code into a relative pathname.
     * @param errorDirPath error directory 
     * @return int : 1 if no problems else 0
     * The generated filename is errorDirPath/dummyModErrors.xml for dummyMod module.
     * A message is displayed in case of error.
     */
    public int generateXML(String errorDirPath) {
        try {
            PrintWriter file = new PrintWriter(new FileOutputStream(errorDirPath+"/"+modName +
                        "Errors.xml"));
            file.println("<?xml version=\"1.0\"?>");
            //We delete the path from the modname
            String name = getLastToken(modName, "/");
            file.println("<errorList moduleName=\"" + name + "\">");
            for (int i = 0; i < tableGen.getRowCount(); i++) {
                printError(file, i);
            }
            file.println("</errorList>");
            file.close();

            return 1;
        }
        catch (Exception e) {
            // e.printStackTrace();
            parent.dispErrStatus("Problem trying to write into "+errorDirPath+"/"+modName +
                    "Errors.xml");
            return 0;
        }
    }

    /**
     * This function reads the XML file and adds in the table the list
     * of the errors
     * @param errorDirPath error directory 
     * @return int : 1 if no problems, else 0
     * A message is displayed in case of error.
     */
    public int readXML(String errorDirPath) {
        Document doc = validateXML(errorDirPath);
        if (doc != null) {
            NodeList no = doc.getElementsByTagName("errorList");
            //First of all there is one node called "errorList"
            Node errorList = no.item(0);

            //The children of errorList are node called "error"
            NodeList errors = errorList.getChildNodes();
            for (int i = 0; i < errors.getLength(); i++) {
                if (errors.item(i).getNodeType() == Node.ELEMENT_NODE) {
                    readXMLError(errors.item(i));
                }
            }
            return 1;
        }
        else {
            return 0;
        }
    }

    /**
     * This function takes all error informations from the node and adds them to
     * the table
     * @param error Node : the error XML node
     */
    private void readXMLError(Node error) {
        //Id
        NamedNodeMap attr = error.getAttributes();
        Node idNode = attr.getNamedItem("id");
        Integer id = new Integer(idNode.getNodeValue());

        NodeList errs = error.getChildNodes();
        //errname
        Node errNameNode = errs.item(1);
        errNameNode = errNameNode.getFirstChild();
        String errName = errNameNode.getNodeValue();
        //errSeverity
        Node errSeverityNode = errs.item(3);
        errSeverityNode = errSeverityNode.getFirstChild();
        String errSeverity = errSeverityNode.getNodeValue();
        //errname
        Node errFormatNode = errs.item(5);
        errFormatNode = errFormatNode.getFirstChild();
        String errFormat = errFormatNode.getNodeValue();

        //We add these errorinformations in the table
        //You don"t forget to add the modName
        String name = getLastToken(modName, "/");
        tableGen.addError(name + "ERR_" + errName, errSeverity, errFormat,
                id.intValue());

    }

    /**
     * This function validates a XML file according to the schema specification
     * @param errorDirPath error directory 
     * @return Document : a XML document or null if an error occured
     */
    private Document validateXML(String errorDirPath) {
        Document doc = null;
        //Parsing
        DOMParser parser = new DOMParser();
        parser.setErrorHandler(new TestErrorHandler());

        try {
            parser.setFeature("http://apache.org/xml/features/validation/schema", true);
            parser.setFeature("http://xml.org/sax/features/validation", true);
            parser.setProperty(
                    "http://apache.org/xml/properties/schema/external-noNamespaceSchemaLocation",
                    schemaPath);
            parser.parse(errorDirPath+"/"+modName + "Errors" + ".xml");
            doc = parser.getDocument();
        }
        catch (SAXException se) {
            parent.dispErrStatus("Validation error : " + se.getMessage());
        }
        catch (IOException ioe) {
            parent.dispErrStatus("Can't access file : "+errorDirPath+"/"+modName + "Errors" + ".xml");
        }
        return doc;
    }

    /**
     * This function prints in the xml file the description of the error according
     * to the schema specification
     * @param file PrintWriter : the file where the errors must be print
     * @param row int : the row number
     */
    private void printError(PrintWriter file, int row) {
        String errName = tableGen.getErrName(row);
        String errFormat = tableGen.getErrFormat(row);
        String errSeverity = tableGen.getErrSeverity(row);
        Integer id = tableGen.getId(row);
        file.println("   <error id=\"" + id.toString() + "\">");
        file.println("      <errName>" + errName.toUpperCase() + "</errName>");
        file.println("      <errSeverity>" + errSeverity + "</errSeverity>");
        file.println("      <errFormat><![CDATA[" + errFormat + "]]></errFormat>");
        file.println("   </error>");
    }


    /**
     * This method returns the first substring of str delimiting by del
     * @param str String : the string to analyse
     * @param del String : the delimiter
     * @return String : the result string
     */
    private String getLastToken(String str, String del) {
        String lastToken = null;
        StringTokenizer token = new StringTokenizer(str, del);
        while (token.hasMoreElements())
        {
            lastToken = token.nextToken(); //The substring before del
        }

        return lastToken;
    }
}

/**
  Simple error handler class that prints error or warning location and raise exception.
 */
class TestErrorHandler
    implements ErrorHandler {
        public void warning(SAXParseException exception) throws SAXException {
            System.out.println("Warning : line " + exception.getLineNumber() +
                    ", column " + exception.getColumnNumber());
            throw exception;
        }

        public void error(SAXParseException exception) throws SAXException {
            System.out.println("Error : line " + exception.getLineNumber() +
                    ", column " + exception.getColumnNumber());
            throw exception;
        }

        public void fatalError(SAXParseException exception) throws SAXException {
            System.out.println("Fatal Error : ligne " + exception.getLineNumber() +
                    ", column " + exception.getColumnNumber());
            throw exception;
        }

}
