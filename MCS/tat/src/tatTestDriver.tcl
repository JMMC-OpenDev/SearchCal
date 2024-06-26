#*******************************************************************************
# JMMC project ( http://www.jmmc.fr ) - Copyright (C) CNRS.
#*******************************************************************************
# REMARKS
#    Adapted from the VLT/ALMA program
#
#-------------------------------------------------------------------------------

#/**
# @file
# Script to run a sequence of tests and compare result
#
# @synopsis
#   tatTestDriver [-v] [-g] [-order] [-r num] [-w secs] [testNum...]
#
# @opt
#   @optname -g : generates reference files
#   @optname -order : takes into account the order in the sequence of
#               output lines
#   @optname -v : verbose, print on standard output progress messages
#   @optname -r : number of times the test must be repeated (def == 1)
#   @optname -w : number of seconds to wait at the end of a test before
#               killing the processes still alive.
#               Very usefull when running purecov, since it is necessary
#               to give the processes enought time to produce coverage
#               reports.
#
# @param testNum : execute only the selected tests
#
# @details
# tatTestDriver runs the test programs listed in the TestList file in the
# current directory. By default, all the tests are performed and the output of
# the test is compared with the reference output mentioned below.\n
# The output files are saved as \<testname\>.rep
#
# If the option -g is used tatTestDriver will generate reference output for each
# program listed in the test list. This is used as base for comparison. The
# reference output files are generated as \<testname\>.ref .
#
# If a list of numbers is given, all the tests identified by the numbers are
# executed. Look at the description of the TestList file for more details.
# The test number is the first field in a record of the TestList file. 
#
# Multiple entries can have the same number, that thus identify a class of
# tests.
#
# The following is a sample record, where the program "test" receive
# the parameters "-a sdf"
# @code
#        1     "test -a sdf"    test_slave
# @endcode 
#
# The output files are filtered to strip out the lines that can change from run
# to run. This is done by using the reference file (if exist) "TestList.grep".
# This file contains a list of matching strings. Egrep is run on the output
# files so that any line matching one of these strings is discarded. The file
# "TestList.sed" contains instead sed lines used to clean part of a line,
# using sed.
#
# After that, all the time stamps are deleted, where a time stamp is any string
# matching the following regular expression: {^[etLog].+}
#
# If differences are found they are listed in the file \<testname\>.diff .
#
# Processes to be executed are NOT SEARCHED using the $PATH environment
# variable, but only in .:../bin:$INTROOT/bin:$MCSROOT/bin
#
# @env
# @envvar PATH ../bin is added to the current PATH value
#
# @return
#   \li EXIT_SUCCESS (0)  If the test is succesfull
#   \li EXIT_FAILURE (1)  If the test could not be executed or if 
#                         differences have been found between the 
#                         report and reference files
#
# @usedfiles
# The program makes use of a number of files. All these files must be presend
# and/or are created in the module test directory, from where this program is
# executed.
#
# @filename TestList  Contains a list of tests to be performed
# This file has the following format:
# <ul>
#   <li> blank lines are allowed
#   <li> comment lines start with '#'
#   <li> each line correspont to a test and has the following structure:
#        <tt>\<testNum\> \<testName\> \<proc 1\> [\<proc 2\>..\<proc n\>]</tt>\n
#   <ul>
#       <li> <tt>\<testNum\></tt>\n
#           is the number assigned to the test. Usually every record has a
#           different number so that they can executed individually.
#       <li> <tt>\<testName\></tt>\n
#           is a symbolic name used to identify the test
#       <li> <tt>\<proc 1\>...</tt>\n
#           is a list of programs to be executed in order to perform the test.
#           It is required to have at least one program.  All programs in the
#           list are executed in background, except the last that is executed in
#           foreground.\n
#           To pass parameters to the programs it is enough to write the
#           complete command line for the program \<proc n\> in quotes.\n
#           Consider the following sample record:
#           @code
#  1  MyTest "test -a dfs" test_slave
#           @endcode
#
#           Instead of a program to be executed, it is possible to give the
#           special directive "\@SLEEP n" where n is a number of seconds to wait
#           before going on with the parsing of the line.
#
#           This can be used whenever it is necessary to wait for the proper
#           startup of a process before going on with the execution of the other
#           processes.
#
#           Typically this is useful when the first process in the list performs
#           some initialization (for example writing initial values in the
#           database to make deterministic the execution of the other components
#           of the test.
#
#           What follows is a sample record:
#           @code
#  1  MyTest initDb "\@SLEEP 5" "test -a dfs" test_slave
#           @endcode
#
#           This means that when initDb is executed, a sleep of 5 second is
#           simultaneously executed.
#   </ul>
# </ul>
#
# The name of the last process is used as the name for the test.
#
# @filename \<filename\>.ref 
#   Used as a reference for tests. It is created with option -g. There is one
#   per each program listed in TestList

# @filename \<filename\>.rep 
#   Report file created when executing test. It is compared with the reference
#   file. There is one per each program listed in TestList
#
# @filename \<filename\>.diff 
#   Report of differences. It the test is not PASSED because differences have
#   been found between report and reference files, these differences are
#   reported in this file There is one per each program listed in TestList
#
# @filename \<filename\>.grep 
#   This file contains a list of regular expressions to be used while filtering
#   the the output files in order to strip lines that change from run to run.
#   This applies only to the specific test.
#
# @filename TestList.grep 
#   This file contains a list of regular expressions to be used while filtering
#   the the output files in order to strip lines that change from run to run.
#   This file applies to all tests
#
# @filename \<filename\>.sed 
#   This file contains a list of sed expressions to be used while filtering  the
#   output files in order to cleanup lines that change from run to run. This
#   applies only to the specific test.
#
# @filename TestList.sed 
#   This file contains a list of sed expressions to be used while filtering the
#   the output files in order to cleanup lines that change from run to run. This
#   file applies to all tests
#
# @warning
# Up to now there are problems handling the sequence of events, so by default
# all the lines are sorted in alphabetical order. Use the -order option to
# preserve the order when comparing the files
#
# Processes to be executed are NOT SEARCHED using the $PATH environment
# variable, but only in .:../bin:$INTROOT/bin:$MCSROOT/bin
#
# The last process in every record of TestList is esecuted in foreground while
# the other processes are executed in background. The test is considered
# completed when the foreground process exit. At that time all the living
# background processes are killed sending them a SIGTERM
#
# It is not allowed to execute processes whose name starts with the "@"
# character.
# This is reserved for special directives like \@SLEEP that are interpreted
# internally by the application.
#
# @bug If the current directory has no write access, the program locks.
#
# */


# TCL package
package require Tclx

########################################
# Default values and global variables

# General variables

# Mode (run or generate)
set mode         "run"
set testAll      1
set toTest       {}

# File with list of tests
set testFiles    "TestList"

# Files with grep and sed patterns
set grepFile     "TestList.grep"
set sedFile      "TestList.sed"

# Used for verbose mode
set verbose      0

# Used to define if message order must be taken into account or ignored
set noorder      1

# Used to define if extended TESTLIST syntax must be used
set extended     1

# Used to define the number of times the test is repeated
set repeat      1

# Used to define the number of seconds to wait before killing
# processes at end of test
set waitAtEnd   0

#
#####   END DEFAULT AND GLOBAL VARIABLES   ############

#*******************************************************************
# proc printLog {str}         prints on standard output the given string
#                           properly formatted
# proc printLogVerbose {str}  like printLog, but prints only if verbose == 1
#
#*******************************************************************
proc printLog {str} {
    puts "$str"
}

proc printLogVerbose {str} {
    global verbose
    if {$verbose == 1} {
        puts "$str"
    }
}

###############################################################################
# runTest (fileName, gen)
#  runs the test programs as listed in in fileName
#  if gen is specified as "run" the test are run and compared 
#  with a reference output.
#  If gen is specified as "generate" a reference output file 
#  is generated
#
###############################################################################

proc runTest { testFiles mode } {

    global errorCode errorInfo
    global testAll   toTest
    global noorder   extended
    global waitAtEnd

    set lineCount 1
    set retFlag   0

    # Check if the requested file exist
    if {![file exists $testFiles]} {
        printLog "No tests to be done: $testFiles does not exists"
        exit 1
    }

    # Open the file for read
    set fd [open $testFiles r]

    # Loop over the lines in the file.
    # Each line correspond to a test
    while { [gets $fd line] >= 0} {

        # Skips comments and empty lines
        if {[regexp {^[ ]*#.+|^#$} $line] != 0} {
            incr lineCount
            continue
        }
        if {[llength $line] < 1} {
            incr lineCount
            continue
        }        

        # Check if the test must be executed. If not continue
        # First see if not all the tests must be executed, the
        # searches in the list of the tests that must be executed.
        # If this is not found, skips the test
        set thisTest [lindex $line 0]
        if { $testAll == 0 && [lsearch $toTest $thisTest] == -1 } {
            incr lineCount
            continue
        }

        if { $extended } {
            set nProc [expr [llength $line] - 2]
            set fProc 2
            set testProc [lindex $line 1]

        } else {
            set nProc [expr [llength $line] - 1]
            set fProc 1
            set testProc [lindex [lindex $line $nProc] 0]
        }

        printLogVerbose "TEST NAME: $testProc ($nProc processes to run)"

        # Prepares the file names for the otput
        # and execute the processes
        set userId [id user]
        set file /tmp/${userId}_test[pid]

        set fileList {}

        # Looks for all entries in sigle record
        # This can be real processes or special directives (@SLEEP)
        # then builds the list of real processes and stores the number 
        # in $actualProc
        set actualProc 0
        for {set i 0} { $i < $nProc } { incr i +1 } {
            
            set cmdLine [lindex $line [expr $i + $fProc]]
            
            if {[lindex $cmdLine 0] == "@SLEEP" } {
                set sleepTime [lindex $cmdLine 1]
                printLogVerbose "waiting $sleepTime secs."
                sleep $sleepTime
            } else {
                set fileArray($actualProc) /tmp/${userId}_test[pid].$actualProc
                lappend fileList $fileArray($actualProc)

                set bpid($actualProc) 0
        

                set outFile $fileArray($actualProc)

                if { $i != [expr $nProc -1] } {
                    printLogVerbose "executing bg: $cmdLine (out in $outFile)"
                    set bpid($actualProc) [eval exec tatTestSpawner [runProg $cmdLine] >&$outFile &]
                } else {
                    printLogVerbose "executing fg: $cmdLine (out in $outFile)"
                    sleep 2
                    catch {eval exec [runProg $cmdLine] >&$outFile }            
                }
                incr actualProc +1
            }
        }

        if { $waitAtEnd != 0 } {
                printLogVerbose "Sleeping $waitAtEnd secs"
                sleep $waitAtEnd
            }

        # Add a time stamp to all the lines of the output files
        # that do not have it.
        # Add also a mark to identify the process (master or slave)
        # Finally kill all the processes in the group
        # identified by the backgrund task
        for {set i 0} { $i < $actualProc } { incr i +1 } {
            addTimeStamp $fileArray($i) "[expr $i + 1] -"
            if { $noorder == 1 } {
                # Then sort all the lines according to the time stamp.
                eval exec cat $fileList | sort > $file
            } else {
                eval exec cat $fileList > $file
            }
            # Now I can safely kill all the processes in the group
            # identified by the backgrund task
            if {$bpid($i) > 0} { catch { kill -pgroup SIGTERM $bpid($i) } }

        }

        # Now the time stamp can be removed, since all lines are sorted
        rmTimeStamp $file

        # Filter the files for lines that can change
        egrepFilter $file $testProc
        sedFilter $file $testProc

        # Copy the file in the definitive place
        catch {file copy -force $file $testProc.rep}

        # Cleanup the files not used any more
        catch {eval file delete -force -- $fileList $file }

        # If Run, compare the output and the reference files,
        # writes a report and prints the test result
        if {$mode == "run"} {

            # Check if the reference file exist.
            # If not, no comparison can be done and exits
            if { ![file exists $testProc.ref] } {
                printLog "Reference file $testProc.ref does not exist"
                return 1
            }

            # Checks if the files must be compared taking into account
            if { $noorder == 1 } {

                # line position or not.
                set file1 /tmp/${userId}_test[pid].ref
                set file2 /tmp/${userId}_test[pid].rep

                # make sure the files does not exist already
                catch {file delete -force -- $file1 $file2}
            
                # sort the two files and compare them
                exec sort $testProc.ref > $file1
                exec sort $testProc.rep > $file2
                set err [catch {exec diff $file1 $file2} diffErr]

                # Delete the temporary .ref and .rep files after usage
                catch {file delete -force -- $file1 $file2}

            } else {

                set err [catch {exec diff $testProc.ref $testProc.rep} diffErr]

            }

            # Check if differences have been found and log proper messages
            if {$err > 0} {
                set dfd [open $testProc.diff w]
                puts $dfd $diffErr
                close $dfd

                printLogVerbose "Found differences: look in $testProc.diff"
                printLog "TEST$thisTest ($testProc) FAIL"
                set retFlag 1
            } else {
                printLog "TEST$thisTest ($testProc) PASS"
            }

        } 
        # end if run
      
        # if the option was "generate"
        if {$mode == "generate"} {
            printLog "Reference file $testProc.ref generated"
            file rename -force -- $testProc.rep $testProc.ref
        }
        
        incr lineCount
    }
    close $fd

    # Set the output file name depending on the mode
    if { $mode == "run" } { 
        if {$retFlag == 1} {
            printLog "FAIL"
            return 1
        } else {
            printLog "PASS"
            return 0
        }
    } else {
        printLog "Reference file/s generated!"
        return 0
    }
}

###############################################################################
#
# procedure egrepFilter (fileName testName) 
# Strips off from the file all the lines maching the reg. exps. in file
# $grepFile (if it exist)
############################################################################## 

proc egrepFilter {fileName testName} {

    global grepFile
    set userId [id user]
    set tmpGrep /tmp/${userId}_test[pid]

    if {[file exists $testName.grep] && [file exists $grepFile]} {
        printLogVerbose "Cleaning with $grepFile and $testName.grep: $fileName"
        catch {exec cat $grepFile $testName.grep > $tmpGrep$grepFile.tmp }
    } elseif {[file exists $testName.grep]} {
        printLogVerbose "Cleaning with $testName.grep: $fileName"
        catch {file copy -force $testName.grep $tmpGrep$grepFile.tmp}
    } elseif {[file exists $grepFile]} {
        printLogVerbose "Cleaning with $grepFile: $fileName"
        catch {file copy -force $grepFile $tmpGrep$grepFile.tmp}
    }

    # Clean the output file.
    if {[file exists $tmpGrep$grepFile.tmp]} {
        catch {exec egrep -v -f $tmpGrep$grepFile.tmp $fileName > $fileName.tmp}
        catch {file rename -force -- $fileName.tmp $fileName}
        catch {file delete -force -- $tmpGrep$grepFile.tmp}
    } else {
        printLogVerbose "Nothing to clean: grepFile does not exist"
    }

    return 0
}

###############################################################################
#
# procedure sedFilter (fileName testName) 
# Executes the sed commands stored in the file $sedFile (if it exist)
############################################################################## 

proc sedFilter {fileName testName} {

    global sedFile
    set userId [id user]
    set tmpSed /tmp/${userId}_test[pid]

    if {[file exists $testName.sed] && [file exists $sedFile]} {
        printLogVerbose "Cleaning with $sedFile and $testName.sed: $fileName"
        catch {exec cat $sedFile $testName.sed > $tmpSed$sedFile.tmp}
    } elseif {[file exists $testName.sed]} {
        printLogVerbose "Cleaning with $testName.sed: $fileName"
        catch {file copy -force $testName.sed $tmpSed$sedFile.tmp}
    } elseif {[file exists $sedFile]} {
        printLogVerbose "Cleaning with $sedFile: $fileName"
        catch {file copy -force $sedFile $tmpSed$sedFile.tmp}
    }

    # Clean the output file.
    if {[file exists $tmpSed$sedFile.tmp]} {
        catch {exec sed -f $tmpSed$sedFile.tmp $fileName > $fileName.tmp}
        catch {file rename -force -- $fileName.tmp $fileName}
        catch {file delete -force -- $tmpSed$sedFile.tmp}
    } else {
        printLogVerbose "Nothing to clean: sedFile does not exist"
    }

    return 0
}



###############################################################################
#
# procedure addTimeStamp (fileName)
#  strips off the time stamp (first) from the fileName
############################################################################## 

proc addTimeStamp {fileName pnum} {

    set stamp "0000-00-00"
    set time  "00:00:00."
    set msec  "000000"
    set count 0

    printLogVerbose "Adding time stamps to lines of file: $fileName"
    printLogVerbose "Adding time stamps to lines of file: $fileName"
    
    set fd [open $fileName r]
    set rfd [open $fileName.tmp w]
    while { [gets $fd line] >= 0} {
        if {[regexp {[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]} $line] != 0} {
            regexp {[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]} $line stamp
            regexp {[0-9][0-9]:[0-9][0-9]:[0-9][0-9].[0-9]* *} $line fulltime
            set  time  [string range $fulltime 0 8]
            set  msec  [string range $fulltime 9 end]
            set  count 0
            scan $msec "%d" msec
            if {$msec < 0 || $msec > 999999} { set msec 0 }
            set  nmsec [format "%6.6d" $msec]
            regsub {[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]T[0-9][0-9]:[0-9][0-9]:[0-9][0-9].[0-9][0-9][0-9][0-9][0-9][0-9] *} $line "" newLine

            puts $rfd "$stamp  $time$nmsec $pnum $newLine"
        } else {
            incr count
            set  nmsec [format "%6.6d" [expr $msec + $count]]
            puts $rfd "$stamp  $time$nmsec $pnum $line"
        }
    }
    close $fd 
    close $rfd
    
    # Copy back the new file over the original one
    catch {file rename -force -- $fileName.tmp $fileName}
}

###############################################################################
#
# procedure rmTimeStamp (fileName)
#  strips off the time stamp from the fileName
############################################################################## 
proc rmTimeStamp {fileName} {
    
    printLogVerbose "Removing time stamps from file: $fileName"
    
    set fd [open $fileName r]
    set rfd [open $fileName.tmp w]
    while { [gets $fd line] >= 0} {
        regsub {^[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9] *[0-9][0-9]:[0-9][0-9]:[0-9][0-9].[0-9][0-9][0-9][0-9][0-9][0-9] *} $line "" newLine
        puts $rfd $newLine
    }
    close $fd 
    close $rfd
    
    # Copy back the new file over the original one
    catch {file rename -force -- $fileName.tmp $fileName}
}

###############################################################################
#
# procedure runProg (name)
#
############################################################################## 
proc runProg {name} {
    global env

    if {[file exists [lindex $name 0]]} {
        return $name
    }
    if {[file exists ../bin/[lindex $name 0]]} {
        return "../bin/$name"
    }
    if {[info exists env(INTROOT)] && 
        [file exists $env(INTROOT)/bin/[lindex $name 0]]} {
        return "$env(INTROOT)/bin/$name"
    }
    if {[info exists env(MCSROOT)] && 
        [file exists $env(MCSROOT)/bin/[lindex $name 0]]} {
        return "$env(MCSROOT)/bin/$name"
    }
    printLog "Could not find program $name"
    exit 1
}

###############################################################################
#
# procedure getRefFile (line)
#
############################################################################## 
proc getRefFile {process} {
    set newLine  [split $process /]
    return "[lindex $newLine [expr [llength $newLine] - 1]].ref"
}

#*******************************************************************
# proc usage {}
#
# Procedure to print a short help for this program
#*******************************************************************

proc usage {} {
    global argv0
    printLog "argv0 \[-v\] \[-g\] \[-order\] \[-r num\] \[-w secs\] \[testNum...\]"
    exit 1
}

############################################
#
# main body of TestDriver
############################################
global argc argv0 argv

# I do not need the X window
# wm withdraw .

#
# Parse command line options
#
while {![lempty $argv]} {
        set arg [lvarpop argv]
        switch -regexp -- $arg {
        -v          { set verbose 1 }
        -g          { set mode generate }
        -order      { set noorder 0 }
        -r[0-9]+    { set repeat [string range $arg 2 end]}
        -r                { set repeat [lvarpop argv]}
        -w[0-9]+    { set waitAtEnd [string range $arg 2 end]}
        -w            { set waitAtEnd [lvarpop argv]}
        [0-9]+      { set testAll 0; lappend toTest $arg }
        default        {printLog "Invalid argument: [lindex $argv 0]"; 
                 usage; break}
    }
}


printLogVerbose "Module's Automatic Test - mode is: $mode (repeat $repeat times)"

# Add the current directory and ../bin to PATH
set PATH ../bin:$env(PATH)
set env(PATH) $PATH

printLogVerbose "Search path: $env(PATH)"

# execute the actual test
set retVal 0

# Loops on the test as many times as defined on the command line
for { set i $repeat } { $i > 0 } {incr i -1 } {
    set ret [runTest $testFiles $mode]
    if {$ret != 0} {
        set retVal  1
    }
}

# End of work
exit $retVal


####### END of script ############################################

















