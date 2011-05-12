#! /bin/bash
#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# NAME 
#   mkfDoxygen - front-end to doxygen utilities
# 
# SYNOPSIS  
#   mkfDoxygen [ clean ] 
# 
# DESCRIPTION
#   Calls doxygen onto a module, modifying a standard all-purpose
#   template for IDL and API, and generating both html and RTF documentation
#   This script is meant to be called by the make command when executing make
#   doc
#
# FILES  
#   ${MCSROOT}/templates/forDoxygen/doxyfile
#   ${MCSROOT}/templates/forDoxygen/doxygen-footer.html
#   ${MCSROOT}/templates/forDoxygen/doxygen-header-<institute>.html
#
# RETURN VALUES
#   0  for success
#   -1 for no template file
#
# CAUTIONS
#   It assumes to be launched in the src of a module
#
# EXAMPLES
#
# SEE ALSO
#
# BUGS     
#

#
# signal trap (if any)
#

# error conditions:
# doxygen not found, configuration file not found
# cannot create directories
# current location is src

# Convert the given argument into an all lower case string.
toLower() {
  echo $1 | tr "[:upper:]" "[:lower:]" 
} 

# Convert the given argument into an all upper case string.
toUpper() {
  echo $1 | tr "[:lower:]" "[:upper:]" 
} 

# Set institute (by default jmmc) 
file=../doc/moduleDescription.xml
if [ -f $file ]
then
    # get line containing the name of the module in moduledescription.xml file
    # =>     <institute>INSTITUTE</institute>
    lineContainingInstitute=`grep "<institute>.*</institute>" $file | grep -v Institute`
    # trim left the above extracted line => institute">
    rightSideOfInstitute=${lineContainingInstitute## *<institute>}

    # trim right the above extracted string to get module name => modulename
    institute=${rightSideOfInstitute%%</institute>}
fi

if [ "$institute" != "" ]
then
    institute=`toLower $institute`
else
    institute=jmmc
fi

# Directory for templates
if [ -d ../templates/forDoxygen ] 
then
    T_DIR=../templates/forDoxygen
elif [ -d ${INTROOT}/templates/forDoxygen ] 
then
    T_DIR=${INTROOT}/templates/forDoxygen
elif [ -d ${MCSROOT}/templates/forDoxygen ] 
then
    T_DIR=${MCSROOT}/templates/forDoxygen
else
    echo "Could not find template directory"
fi

BASELINE=${T_DIR}/doxyfile
HEADER=${T_DIR}/doxygen-header-${institute}.html
FOOTER=${T_DIR}/doxygen-footer.html
IMAGE=${T_DIR}/${institute}.jpg

#
BASECAMP=`\pwd`
#
FILE=/tmp/doxygenize$$
#
# determine the module name
#
MODULE_NAME=`\pwd`
MODULE_NAME=${MODULE_NAME%%/src}
if  [ "${MODULE_NAME##*/}" = "ws" -o  "${MODULE_NAME##*/}" = "lcu" ]
then
    prelim=${MODULE_NAME}
    prelim=${prelim%%/ws}
    prelim=${prelim%%/lcu}
    prelim=${prelim##*/}
    MODULE_NAME=$prelim
else
    MODULE_NAME=${MODULE_NAME##*/}
fi
#
# determine the revision
#
#REVISION=`cmmLast $MODULE_NAME`
#REVISION=${REVISION##*:}
#REVISION=${REVISION%%modified*}
REVISION=`grep 'Id: Makefile,v' Makefile | awk '{print $5}'`
#
if [ "${1}" = "pdf" ] 
then
    PDF=1
else
    PDF=0
fi
#

#
if [ ! -f $BASELINE ] 
then
    echo "Could not find template"
    exit -1
fi
#
# determine the dot tool is available
#
if [ "`which dot 2>/dev/null`" != "" ] 
then
    HAVE_DOT="YES"; 
else 
    HAVE_DOT="NO"; 
fi

###################################################

# Default API IDL INPUT OUTPUT directories
INPUT_IDL=" ../idl"
INPUT_API=". ../include"
OUTPUT_DIR=../doc/
OUTPUT_IDL=$OUTPUT_DIR/idl 
OUTPUT_API=$OUTPUT_DIR/api

# By default IDL and API document generation is enabled 
SKIP_IDL="NO"
SKIP_API="NO"

# Weed out comments and blank lines
egrep -v '^#|^$'  $BASELINE > $FILE
if [ $PDF = 1 ]
then
    sed -e "s/^GENERATE_LATEX.*/GENERATE_LATEX = YES/" $FILE > $FILE.tmp
    mv $FILE.tmp $FILE
fi

# Adding doxygen customizations 
if [ -f $BASECAMP/../config/doxyfile ]
then

    # Creation of a copy of doxyfile called doxyfile_clean
    cp $BASECAMP/../config/doxyfile /tmp/doxyfile_clean
    chmod 644 /tmp/doxyfile_clean

    # Remotion of comments in the customer customized file
    egrep -v '^#|^$'  /tmp/doxyfile_clean > /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean

    # Cautional remotion
    if [ -f /tmp/doxyfile.old ]
    then
        rm /tmp/doxyfile.old
    fi

    # Remotion of all spaces before and after char "=" in the customized file
    cat /tmp/doxyfile_clean | sed s/\ *=/=/g >> /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean
    cat /tmp/doxyfile_clean | sed s/=\ */=/g >> /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean

    # Trick: spaces converted in #
    cat /tmp/doxyfile_clean | sed s/\ /#/g >> /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean   

    # Storing eventually new INPUT OUTPUT settings for IDL and API
    for myline in $(cat  /tmp/doxyfile_clean) 
    do
        VARIABLE=$myline
        VALUE=$myline
        VARIABLE=${VARIABLE%%=*}
        VALUE=${VALUE##*=}

        case $VARIABLE in
            INPUT_IDL )
            echo $VALUE > /tmp/mytmp$$
            cat /tmp/mytmp$$ | sed s/#/\ /g > /tmp/mytmp2$$
            INPUT_IDL=$(< /tmp/mytmp2$$);;
            INPUT_API )
            echo $VALUE > /tmp/mytmp$$
            cat /tmp/mytmp$$ | sed s/#/\ /g > /tmp/mytmp2$$
            INPUT_API=$(< /tmp/mytmp2$$);;
            OUTPUT_IDL )
            echo $VALUE > /tmp/mytmp$$
            cat /tmp/mytmp$$ | sed s/#/\ /g > /tmp/mytmp2$$
            OUTPUT_IDL=$(< /tmp/mytmp2$$);;
            OUTPUT_API )
            echo $VALUE > /tmp/mytmp$$
            cat /tmp/mytmp$$ | sed s/#/\ /g > /tmp/mytmp2$$
            OUTPUT_API=$(< /tmp/mytmp2$$);;
            SKIP_IDL )
            echo $VALUE > /tmp/mytmp$$
            cat /tmp/mytmp$$ | sed s/#/\ /g > /tmp/mytmp2$$
            SKIP_IDL=$(< /tmp/mytmp2$$);;
            SKIP_API )
            echo $VALUE > /tmp/mytmp$$
            cat /tmp/mytmp$$ | sed s/#/\ /g > /tmp/mytmp2$$
            SKIP_API=$(< /tmp/mytmp2$$);;
        esac
    done

    # Remotion of each INPUT OUTPUT IDL API info
    sed -e "s/INPUT_IDL.*//" /tmp/doxyfile_clean > /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean
    sed -e "s/INPUT_API.*//" /tmp/doxyfile_clean > /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean
    sed -e "s/OUTPUT_IDL.*//" /tmp/doxyfile_clean > /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean
    sed -e "s/OUTPUT_API.*//" /tmp/doxyfile_clean > /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean
    sed -e "s/SKIP_IDL.*//" /tmp/doxyfile_clean > /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean
    sed -e "s/SKIP_API.*//" /tmp/doxyfile_clean > /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean

    # Update of the doxyfile file with the info in doxyfile_clean
    for myline in $(cat  /tmp/doxyfile_clean) 
    do

        VARIABLE=$myline
        VARIABLE=${VARIABLE%%=*}

        if grep $VARIABLE $FILE > /tmp/to_be_removed
        then
            # Substitution of the variable with the customized value
            echo $myline > /tmp/mytmp$$
            cat /tmp/mytmp$$ | sed s/#/\ /g > /tmp/mytmp2$$
            origin_line=$(< /tmp/mytmp2$$)
            sed -e "s!$VARIABLE.*!$origin_line !" $FILE > $FILE.tmp
            mv $FILE.tmp $FILE
        else
            # Addition of a new variable
            echo $myline > /tmp/mytmp$$
            cat /tmp/mytmp$$ | sed s/#/\ /g > /tmp/mytmp2$$
            origin_line=$(< /tmp/mytmp2$$)
            echo $myline >> $FILE
        fi
    done

    # Remotion of temporary files
    if [ -f /tmp/to_be_removed ]
    then
        rm /tmp/to_be_removed
    fi
    if [ -f /tmp/mytmp$$ ]
    then
        rm /tmp/mytmp$$
    fi
    if [ -f /tmp/mytmp2$$ ]
    then
        rm /tmp/mytmp2$$
    fi
    if [ -f /tmp/doxyfile_clean ]
    then
        rm /tmp/doxyfile_clean
    fi
    if [ -f /tmp/doxyfile.old ]
    then
        rm /tmp/doxyfile.old
    fi
fi

#
if [ "${1}" = "clean" ] 
then
    # Remotion of char "
    echo $OUTPUT_API > /tmp/mytmp$$
    cat /tmp/mytmp$$ | sed s/\"//g  > /tmp/mytmp2$$
    OUTPUT_API=$(< /tmp/mytmp2$$)

    # Remotion of char "
    echo $OUTPUT_IDL > /tmp/mytmp$$
    cat /tmp/mytmp$$ | sed s/\"//g  > /tmp/mytmp2$$
    OUTPUT_IDL=$(< /tmp/mytmp2$$)

    rm -fr $OUTPUT_IDL $OUTPUT_API $OUTPUT_DIR/index.html

    # Remotion of temporary files
    if [ -f /tmp/mytmp$$ ]
    then
        rm /tmp/mytmp$$
    fi
    if [ -f /tmp/mytmp2$$ ]
    then
        rm /tmp/mytmp2$$
    fi
    rm -f $FILE

    exit 0
fi
#

#
# calling doxygen for IDL
#
if [ "$SKIP_IDL" = "NO" ]
then

    OUTPUT=$OUTPUT_IDL
    INPUT=$INPUT_IDL

    PROCEED="YES"

    # Remotion of char "
    echo $INPUT > /tmp/mytmp$$
    cat /tmp/mytmp$$ | sed s/\"//g  > /tmp/mytmp2$$
    INPUT=$(< /tmp/mytmp2$$)

    # Remotion of char "
    echo $OUTPUT > /tmp/mytmp$$
    cat /tmp/mytmp$$ | sed s/\"//g  > /tmp/mytmp2$$
    OUTPUT=$(< /tmp/mytmp2$$)

    # checking about the list of input direcory/ies
    for dir_name in $(echo $INPUT)
    do
        if [ ! -d $dir_name ]
        then
            PROCEED="NO"
        fi
    done

    # Proceed if all input directories exist
    if [ "$PROCEED" = "YES" ] 
    then 

        # Creation of output directories
        for dir_name in $(echo $OUTPUT)
        do 
            if [ ! -d $dir_name ]
            then
                mkdir -p $dir_name
            fi
        done

        echo "....calling doxygen for IDL"
        sed -e "s#^OUTPUT_DIRECTORY.*#OUTPUT_DIRECTORY  = $OUTPUT#g;s#^INPUT .*#INPUT = $INPUT#;s#^HTML_HEADER.*#HTML_HEADER = $HEADER#;s/^PROJECT_NAME.*/PROJECT_NAME = \"$MODULE_NAME IDL\"/;s/^PROJECT_NUMBER.*/PROJECT_NUMBER = $REVISION/;s#^HTML_FOOTER.*#HTML_FOOTER = $FOOTER#;s#^HAVE_DOT.*#HAVE_DOT = $HAVE_DOT#" $FILE | doxygen -
        # copy the nice picture. To be re-thought!
        #
        # make the pdf file
        if [ $PDF = 1 ]
        then    
            cd $OUTPUT/pdf
            make pdf > /dev/null 2>&1
            if [ $? = 0 ]
            then
                mv refman.pdf .refman.pdf
                rm -f * 
                mv .refman.pdf refman.pdf
            else
                echo "Compilation of PDF file failed"
            fi
        fi
    fi
fi
cd $BASECAMP

#
# calling doxygen for API
#
#if [ `ls ../include | grep -c ".h"` != 0 ]
if [ "$SKIP_API" = "NO" ]
then

    OUTPUT=$OUTPUT_API
    INPUT=$INPUT_API

    PROCEED="YES"

    # Remotion of char "
    echo $INPUT > /tmp/mytmp$$
    cat /tmp/mytmp$$ | sed s/\"//g  > /tmp/mytmp2$$
    INPUT=$(< /tmp/mytmp2$$)

    # Remotion of char "
    echo $OUTPUT > /tmp/mytmp$$
    cat /tmp/mytmp$$ | sed s/\"//g  > /tmp/mytmp2$$
    OUTPUT=$(< /tmp/mytmp2$$)

    # checking about the list of input direcory/ies
    for dir_name in $(echo $INPUT)
    do
        if [ ! -d $dir_name ]
        then
            PROCEED="NO"
        fi
    done

    # Proceed if all input directories exist
    if [ "$PROCEED" = "YES" ] 
    then 

        # Creation of output directories
        for dir_name in $(echo $OUTPUT)
        do 
            if [ ! -d $dir_name ]
            then
                mkdir -p $dir_name
            fi
        done

        echo "....calling doxygen for API"
        sed -e "s#^OUTPUT_DIRECTORY.*#OUTPUT_DIRECTORY  = $OUTPUT#g;s#^INPUT .*#INPUT = $INPUT#;s#^HTML_HEADER.*#HTML_HEADER = $HEADER#;s/^PROJECT_NAME.*/PROJECT_NAME = \"$MODULE_NAME API\"/;s/^PROJECT_NUMBER.*/PROJECT_NUMBER = $REVISION/;s#^HTML_FOOTER.*#HTML_FOOTER = $FOOTER#;s#^HAVE_DOT.*#HAVE_DOT = $HAVE_DOT#" $FILE | doxygen -

        #
        # place main index.html under the doc directory
        echo "<!-- Main HTML index file for module documentation -->" > $OUTPUT_DIR/index.html
        echo "<!-- Created automatically by mkfDoxygen - `date '+%d.%m.%y %T'` -->" >> $OUTPUT_DIR/index.html
        echo "<!-- DO NOT EDIT THIS FILE -->" >> $OUTPUT_DIR/index.html
        echo "<meta HTTP-EQUIV=\"REFRESH\" content=\"0; url=./api/html/index.html\">" >> $OUTPUT_DIR/index.html
        
        #
        # copy the nice picture. To be re-thought!

        cp -f $IMAGE $OUTPUT/html
        #
        # make the pdf file
        if [ $PDF = 1 ]
        then    
            cd $OUTPUT/pdf
            make pdf > /dev/null 2>&1
            if [ $? = 0 ]
            then
                mv refman.pdf .refman.pdf
                rm -f * 
                mv .refman.pdf refman.pdf
            else
                echo "Compilation of PDF file failed"
            fi
        fi
    fi
fi 

# Remotion of temporary files
if [ -f /tmp/mytmp$$ ]
then
    rm /tmp/mytmp$$
fi
if [ -f /tmp/mytmp2$$ ]
then
    rm /tmp/mytmp2$$
fi

cd $BASECAMP
# cleanup all the mess
rm -f $FILE
