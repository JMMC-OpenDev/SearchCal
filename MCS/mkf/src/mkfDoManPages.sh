#! /bin/bash
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mkfDoManPages
#
# History
# -------
# $Log: not supported by cvs2svn $
# gzins     09-Jun-2004  created from ALMA doxygenize program
#
#************************************************************************
# NAME 
#   mkfDoManPages - front-end to doxygen utilities
# 
# SYNOPSIS  
#   mkfDoManPages <man section> <file1> [<file2> ...<fileN> 
# 
# DESCRIPTION
#   Calls doxygen for the files given as parameters, and generating man pages
#   in the given section.
#   This script is meant to be called by the make command when executing make
#   man
#
# FILES  
#   ${MCSROOT}/templates/forDoxygen/doxyfile
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
#------------------------------------------------------------------------
#
# signal trap (if any)
#
# ___oOo___
#
# error conditions:
# doxygen not found, configuration file not found
# cannot create directories
# current location is src

MAN_SECTION=$1
shift
INPUT_FILES=$*

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
HEADER=${T_DIR}/doxygen-header.html
FOOTER=${T_DIR}/doxygen-footer.html
IMAGE=${T_DIR}/eii.jpg
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
REVISION=`grep Makefile,v Makefile | awk '{print $5}'`
#
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

# Adding doxygen customizations 
if [ -f $BASECAMP/../config/doxyfile ]
then

    # Creation of a copy of doxyfile called doxyfile_clean
    cp $BASECAMP/../config/doxyfile /tmp/doxyfile_clean

    # Remotion of comments in the customer customized file
    egrep -v '^#|^$'  /tmp/doxyfile_clean > /tmp/doxyfile.old
    mv /tmp/doxyfile.old /tmp/doxyfile_clean

    # Cautional remotion
    if [ -f /tmp/doxyfile.old ]
    then
        rm /tmp/doxyfile.old
    fi

    # Remotion of all spaces before and after char "=" in the customized file
    cat /tmp/doxyfile_clean | sed s/\ .*=/=/g >> /tmp/doxyfile.old
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
# calling doxygen for API
#
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

        sed -e "s#^OUTPUT_DIRECTORY.*#OUTPUT_DIRECTORY  = $OUTPUT#g;s#^INPUT .*#INPUT = $INPUT#;s#^HTML_HEADER.*#HTML_HEADER = $HEADER#;s/^PROJECT_NAME.*/PROJECT_NAME = \"$MODULE_NAME API\"/;s/^PROJECT_NUMBER.*/PROJECT_NUMBER = $REVISION/;s#^HTML_FOOTER.*#HTML_FOOTER = $FOOTER#;s#^FILE_PATTERNS.*#FILE_PATTERNS = $INPUT_FILES#;s#^GENERATE_HTML.*#GENERATE_HTML = NO#;s#^GENERATE_MAN.*#GENERATE_MAN = YES#;s#^MAN_EXTENSION.*#MAN_EXTENSION = .$MAN_SECTION#;s#^MAN_LINKS.*#MAN_LINKS = YES#;s#^HAVE_DOT.*#HAVE_DOT = $HAVE_DOT#" $FILE | doxygen -

        for man in ${OUTPUT}/man/man${MAN_SECTION}/*
        do
            rm -f /tmp/mytmp3$$
            SynopsisFound=`grep Synopsis $man`
            mv $man /tmp/mytmp3$$ 
            FILTER="s/\.RS 4$/\n.RS 4/"
            if [ "$SynopsisFound" != "" ]
            then
                FILTER="$FILTER;/SYNOPSIS/d"
            fi
            sed -e "$FILTER" /tmp/mytmp3$$ > $man 
        done
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
if [ -f /tmp/mytmp3$$ ]
then
    rm -f /tmp/mytmp3$$
fi

cd $BASECAMP
# cleanup all the mess
rm -f $FILE
