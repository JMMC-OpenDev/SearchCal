#! /bin/sh
#*******************************************************************************
# JMMC project
#
# "@(#) $Id: mkfMakeTclScript.sh,v 1.1 2004-09-10 13:40:57 gzins Exp $" 

# who       when         what
# --------  --------     ----------------------------------------------
# gzins     26-Aug-2004  Adapted from VLT

#************************************************************************
#   NAME
#   mkfMakeTclScript - create an executable Tcl/Tk procedure
# 
#   SYNOPSIS
#
#   mkfMakeTclScript <tclChecker> <defTclShell> <tclShell>  
#                             <exeName> <objectList> <tclLibList>
# 
#   DESCRIPTION
#   Utility used by mkfMakefile to create an executable Tcl/Tk procedure.
#   It is not intended to be used as a standalone command.
#
#   An Tcl/Tk procedure is obtained starting from one or more tcl/tk 
#   script files in the src/ directory.
#
#   (1) see also Tcl/Tk manual, 13.7 Autoloading, pag.138.
#
#
#   <tclChecker>  the program to be used as syntax checek for tcl files
#
#   <defTclShell> The full name of the default tcl shell
#
#   <tclShell>    The full name of the tcl shell for this one
#
#   <exeName>     The name of the  executable. The output is named 
#                 ../bin/<exeName>
#
#   <objectList>  The list of the script files in the src/ directory.
#                 (Without neither directory nor .tcl suffix)
#
#   <tclLibList>  the list of tcl libraries needed to link.
#                 the library directory name is created as
#                 lib<listMember>.tcl
#
#   FILES
#   $MCSROOT/include/mkfMakefile   
#
#   ENVIRONMENT
#
#   RETURN VALUES
#
#   SEE ALSO 
#   mkfMakefile, Makefile, (GNU) make
#
#   BUGS    
#
#----------------------------------------------------------------------

if [ $# -ne 6 ]
then
    echo "" >&2
    echo "mkfMakeTclScript <tclChecker> <defTclShell> <tclShell>  <exeName> <objectList> <tclLibList>" >&2
    echo "" >&2
    exit 1
fi

#
# set up more readable variables:
tclChecker=$1
defTclShell=$2
tclShell=$3
exeName=$4
objectList=$5
libList=$6

OUTPUT=../bin/${exeName}

#
#  The first line of a script can be used to specify the shell to be used 
#  to execute the script. Such a line shall have the following format:
#          #! <shellFilename> [<parameter> ... ]
#  The <shellFilename> MUST be completed with the path and the total 
#  lenght cannot exceed 32 chars.
#
#  This impose a serious limitation to the flexibility of finding a shell
#  executable according to the precedence established by the current PATH 
#  
#  Since the introduction of tcl/Tk and Sequencer, many commands are build 
#  as scripts. To allow the usual flexibility given by the mod/int/mcs-root
#  architecture, there is the need to use the following trick:
#  
#      - all scripts are executed by /bin/sh (first line)
#      - the second line ends with "\"
#      - the third line is an "exec" of the actual shell we want to use
#        (such a command is searched according to the current PATH)
#
#  Thanks to a different way in handling the "\" at the end of a comment line
#  (such line is considered a comment line for Tcl, i.e. when you "source"
#  the file, but not for /bin/sh ) this allows to be able both to source 
#  the script and to execute. In both cases the same shell (the one given
#  by "which shell") is used.
#  
#  Therefore the header of the output file shall be:
#  
#     #!/bin/sh
#     #\
#     exec <tclShell> "$0" ${1+"$@"}
#  

if [ "${tclShell}" = "" ]
then
    tclShell=$defTclShell
fi

echo "#!/bin/sh" >$OUTPUT
echo "#\\" >>$OUTPUT
echo "exec ${tclShell} \"\$0\" \${1+\"\$@\"}" >>$OUTPUT


#
# create header
#
echo "#*******************************************************************************" >>$OUTPUT
echo "# "                                                             >>$OUTPUT
echo "# ${exeName}"                                                   >>$OUTPUT
echo "# "                                                             >>$OUTPUT
echo "# Created by mkfMakeTclScript: `whoami` `date '+%d.%m.%y %T'` " >>$OUTPUT
echo "# "                                                             >>$OUTPUT
echo "# "                                                             >>$OUTPUT
echo "#        !!!!!!!!!!!!!!  DO NOT MANUALLY EDIT THIS FILE  !!!!!!!!!!!!!!" >>$OUTPUT
echo "# "                                                             >>$OUTPUT
echo "#-------------------------------------------------------------------------------">>$OUTPUT
echo ""                                                               >>$OUTPUT

    
# 
# define standard variables: MCSROOT, INTROOT, MODROOT, HOME
# (only if not using seqSh/seqWish, as these shells take care of that)
if [ "${tclShell}" != "seqSh" ] && [ "${tclShell}" != "seqWish" ]
then
    echo "#" >>$OUTPUT
    echo "#  Get directory root of MCS, integration, current module."        >>$OUTPUT
    echo "if {[catch {set MCSROOT \$env(MCSROOT)}]} {set MCSROOT undefined}" >>$OUTPUT
    echo "if {[catch {set INTROOT \$env(INTROOT)}]} {set INTROOT undefined}" >>$OUTPUT
    echo "if {[catch {set MODROOT \$env(MODROOT)}]} {set MODROOT undefined}" >>$OUTPUT
    echo "if {[catch {set HOME    \$env(HOME)}]}    {set HOME    undefined}" >>$OUTPUT
    echo "" >>$OUTPUT
fi

#
# if the list of libraries is not empty, insert each lib into the autopath
if [ "${libList}" != "" ]
then
    echo "#"                                           >>$OUTPUT
    echo "#  Adjust auto_path for each library"        >>$OUTPUT
    echo "#  (this permits to find the resp. tclIndex files)" >>$OUTPUT
    for member in ${libList}
    do
	echo ""                                        >>$OUTPUT
	echo "# "                                      >>$OUTPUT
	echo "# >>>${member}<<<"                       >>$OUTPUT

	if [ "${tclShell}" = "seqSh" ] || [ "${tclShell}" = "seqWish" ]
	then 
	    echo "if {[catch {seq_findFile lib/lib${member}.tcl} dir] || ![file isdirectory \$dir]} {"  >>$OUTPUT
	    echo "    puts stderr  \"${exeName} - ERROR:\"; "              >>$OUTPUT
	    echo "    puts stderr  \"   Tcl library lib${member}.tcl not found. Unable to continue.\"; " >>$OUTPUT
	    echo "    puts stderr  \"\"; "                                 >>$OUTPUT
	    echo "    exit 1;"                                             >>$OUTPUT
	    echo "}"                                                       >>$OUTPUT
	else 
	    echo "if {[file exists ../lib/lib${member}.tcl] && [file isdirectory ../lib/lib${member}.tcl]} {" >>$OUTPUT
	    echo "    # the current relative path can be affected by any change of working"   >>$OUTPUT
	    echo "    # directory within the application. "                >>$OUTPUT
	    echo "    # To prevent possible mess, the RELATIVE path is changed into absolute" >>$OUTPUT
	    echo "    set keepWd [pwd]"                                    >>$OUTPUT
	    echo "    cd ../lib/lib${member}.tcl"                          >>$OUTPUT
	    echo "    set dir [pwd]"                                       >>$OUTPUT
	    echo "    cd \$keepWd"                                         >>$OUTPUT
	    echo "    unset keepWd"                                        >>$OUTPUT
	    echo "} elseif {[file exists \$MODROOT/lib/lib${member}.tcl] && [file isdirectory \$MODROOT/lib/lib${member}.tcl]} {"  >>$OUTPUT
	    echo "    set dir \$MODROOT/lib/lib${member}.tcl"              >>$OUTPUT
	    echo "} elseif {[file exist \$INTROOT/lib/lib${member}.tcl] && [file isdirectory \$INTROOT/lib/lib${member}.tcl]} {"                  >>$OUTPUT
	    echo "    set dir \$INTROOT/lib/lib${member}.tcl"              >>$OUTPUT
	    echo "} elseif {[file exists \$MCSROOT/lib/lib${member}.tcl] && [file isdirectory \$MCSROOT/lib/lib${member}.tcl]} {"                  >>$OUTPUT
	    echo "    set dir \$MCSROOT/lib/lib${member}.tcl"              >>$OUTPUT
	    echo "} else {"                                                >>$OUTPUT
	    echo "    puts stderr  \"${exeName} - ERROR:\"; "              >>$OUTPUT
	    echo "    puts stderr  \"   Tcl library lib${member}.tcl not found. Unable to continue.\"; " >>$OUTPUT
	    echo "    puts stderr  \"\"; "                                 >>$OUTPUT
	    echo "    exit 1;"                                             >>$OUTPUT
	    echo "}"                                                       >>$OUTPUT
	fi
	echo "if {[lsearch -exact \$auto_path \$dir] == -1} {"             >>$OUTPUT
	echo "    set auto_path [linsert \$auto_path 0 \$dir]"             >>$OUTPUT
	echo "}"                                                           >>$OUTPUT
    done
    echo "catch {unset dir}"                                               >>$OUTPUT
else
    echo "#"                                                               >>$OUTPUT
    echo "# No libraries defined for this script."                         >>$OUTPUT
    echo "# " >>$OUTPUT
fi

echo "###########################################################" >>$OUTPUT
echo "#                                                         #" >>$OUTPUT
echo "# if the current shell is including TK, i.e. option       #" >>$OUTPUT
echo "# command is known, then set-up X resources               #" >>$OUTPUT
echo "#                                                         #" >>$OUTPUT
echo "###########################################################" >>$OUTPUT

echo ""                                          >>$OUTPUT
echo "if {[info command option] != \"\"} {"      >>$OUTPUT

echo "    "                                      >>$OUTPUT
echo "    #"                                     >>$OUTPUT
echo "    # Load resources:"                     >>$OUTPUT
echo "    # merge possible Xresource files in the following order:"  >>$OUTPUT
echo "    #           - $MCSROOT/app-defaults  "                     >>$OUTPUT
echo "    #           - $INTROOT/app-defaults  "                     >>$OUTPUT
echo "    #           - $MODROOT/app-defaults  "                     >>$OUTPUT
echo "    #           - $HOME/                 "                     >>$OUTPUT
echo "    #           - ../app-defaults        "                     >>$OUTPUT
echo "    #"                                                         >>$OUTPUT
echo "    if {[file exists \$MCSROOT/app-defaults/X${exeName}]} {"   >>$OUTPUT
echo "        option readfile \$MCSROOT/app-defaults/X${exeName} "   >>$OUTPUT
echo "    }"                                                         >>$OUTPUT

echo "    if {[file exists \$INTROOT/app-defaults/X${exeName}]} {"   >>$OUTPUT
echo "        option readfile \$INTROOT/app-defaults/X${exeName} "   >>$OUTPUT
echo "    }"                                                         >>$OUTPUT

echo "    if {[file exists \$MODROOT/app-defaults/X${exeName}]} {"   >>$OUTPUT
echo "        option readfile \$MODROOT/app-defaults/X${exeName} "   >>$OUTPUT
echo "    }"                                                         >>$OUTPUT

echo "    if {[file exists \$HOME/X${exeName}]} {"                   >>$OUTPUT
echo "        option readfile \$HOME/X${exeName} "                   >>$OUTPUT
echo "    }"                                                         >>$OUTPUT

echo "    if {[file exists ../app-defaults/X${exeName}]} {"          >>$OUTPUT
echo "        option readfile ../app-defaults/X${exeName} "          >>$OUTPUT
echo "    }"                                                         >>$OUTPUT

echo "}"                                                             >>$OUTPUT
   
echo "#-------------------------------------------------------------------" >>$OUTPUT
echo "#                    END OF STANDARD PROLOGUE "                       >>$OUTPUT
echo "#-------------------------------------------------------------------" >>$OUTPUT

#
# append each source file to the output file
if [ "${objectList}" = "" ]
then
    echo "WARNING: mkfMakeTclScript: ${exeName}_OBJECTS is not defined." >&2
    exit 1
fi

for member in ${objectList}
do
    #
    # run the tcl checker on each tcl-file
    $tclChecker ${member}.tcl 1>&2
    
    echo "" >>$OUTPUT
    echo "#vvvvvvvvvvvvvvvvv  src/${member}.tcl  vvvvvvvvvvvvvvvvvvvvvv" >>$OUTPUT
    cat ${member}.tcl                                                    >>$OUTPUT
    echo "#^^^^^^^^^^^^^^^^^  src/${member}.tcl  ^^^^^^^^^^^^^^^^^^^^^^" >>$OUTPUT
done

#
# terminate output file
echo ""                                                                >>$OUTPUT
echo "#--------------------------------------------------------------" >>$OUTPUT
echo "#                       End of ${exeName}"                       >>$OUTPUT
echo "#--------------------------------------------------------------" >>$OUTPUT
echo "# ___oOo___ " >>$OUTPUT

#
# make output file executable
chmod +x $OUTPUT

#
# notify user that all has been done
echo "                      $OUTPUT  created"
#
# ___oOo___
