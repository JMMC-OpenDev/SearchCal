import pyAlx
from string import split, center


#
# Stars corresponding to interpolated table lines
#
interpolatedStarsTuple = '62434', '20889', '93864', '18543', '60965'

#
# Test sets signification
#
# 1 -> B and V are known
# 2 -> B, V and K are known
# 3 -> B, V, I and K are known
testSetsSignificationDict = {'1' : 'B and V are known', 
                             '2' : 'B, V and K are known', 
                             '3' : 'B, V, I and K are known'}

#
# Build expected results sets
#

# Open file
file = open('alxTestComputeMissingMagnitudeForBrightObj.res','r')

# initialize line index
firstLine = 1

# initialize tests set list
resultsDico = {}

# For each file line
for line in file:
    if firstLine == 1:
        # It's the first line
        # Get column items list
        resultsColItemList = split(line)
        firstLine = 0
    else:
        # It's not the first line
        # Get test set value list
        resultsValuesList = split(line)
        # set test set key tuple
        key = resultsValuesList[0]
        # Set information values list
        infoValuesTuple = resultsValuesList[1], resultsValuesList[2], \
                          resultsValuesList[3]
        # Set parameters values list
        paramValueList = resultsValuesList[4:]
        # Append a test set (dictionary) to tests sets list
        resultsDico[key] = (infoValuesTuple, paramValueList)

#
# Build tests sets
#

# Open file
file = open('alxTestComputeMissingMagnitudeForBrightObj.input','r')

# initialize line index
firstLine = 1

# initialize tests set list
testsSetList = []

# For each file line
for line in file:
    if firstLine == 1:
        # It's the first line
        # Get column items list
        testSetsColItemList = split(line)
        firstLine = 0
    else:
        # It's not the first line
        # Get test set value list
        testsSetValuesList = split(line)
        # set test set key tuple
        keyTuple = testsSetValuesList[0], testsSetValuesList[1]
        # Set parameters values list
        paramValueList = testsSetValuesList[2:]
        # Append a test set (dictionary) to tests sets list
        testsSetList.append({keyTuple : paramValueList})

#
# Execute tests set
#
for testSet in testsSetList:
    testSetNb = testSet.keys()[0][0]
    testSetSignificance = testSetsSignificationDict[str(testSetNb)]
    TestedStarName = testSet.keys()[0][1]
    print '\n*****************************************************************'
    print 'Jeu de test', testSetNb, ': ', testSetSignificance, \
          '----> star name = ', TestedStarName, '\n'
    
    print '=> Infos:'
    print '   ------'
    spectralType = resultsDico[TestedStarName][0][0]
    lineNb = resultsDico[TestedStarName][0][1]
    starType = resultsDico[TestedStarName][0][2]
    print 'Spectral type: ', spectralType
    print 'Numero of line:', lineNb
    print 'Star type:', starType
    
    print '\n=> Input data:'
    print '   -----------'

    # Assign string parameter
    spType = pyAlx.mcsSTRING32_lire(testSet.values()[0][0])
    print 'spType = ', spType

    # Assign float parameters
    magB = pyAlx.mcsFLOAT_lire(float(testSet.values()[0][1]))
    print 'magB = ', magB
    magV = pyAlx.mcsFLOAT_lire(float(testSet.values()[0][2]))
    print 'magV = ', magV
        
    # Assign float pointer parameters
    magR = pyAlx.new_mcsFLOATp()
    pyAlx.mcsFLOATp_assign(magR, float(testSet.values()[0][3]))
    print 'magR = ', pyAlx.mcsFLOATp_value(magR)
    magI = pyAlx.new_mcsFLOATp()
    pyAlx.mcsFLOATp_assign(magI, float(testSet.values()[0][4]))
    print 'magI = ', pyAlx.mcsFLOATp_value(magI)
    magJ = pyAlx.new_mcsFLOATp()
    pyAlx.mcsFLOATp_assign(magJ, float(testSet.values()[0][5]))
    print 'magJ = ', pyAlx.mcsFLOATp_value(magJ)
    magH = pyAlx.new_mcsFLOATp()
    pyAlx.mcsFLOATp_assign(magH, float(testSet.values()[0][6]))
    print 'magH = ', pyAlx.mcsFLOATp_value(magH)
    magK = pyAlx.new_mcsFLOATp()
    pyAlx.mcsFLOATp_assign(magK, float(testSet.values()[0][7]))
    print 'magK = ', pyAlx.mcsFLOATp_value(magK)
    magL = pyAlx.new_mcsFLOATp()
    pyAlx.mcsFLOATp_assign(magL, float(testSet.values()[0][8]))
    print 'magL = ', pyAlx.mcsFLOATp_value(magL)
    magM = pyAlx.new_mcsFLOATp()
    pyAlx.mcsFLOATp_assign(magM, float(testSet.values()[0][9]))
    print 'magM = ', pyAlx.mcsFLOATp_value(magM)
    confIdx = pyAlx.new_alxCONFIDENCE_INDEXp()

    # alxComputeMagnitudesForBrightStar function call
    res = pyAlx.alxComputeMagnitudesForBrightStar(spType, magB, magV, magR, 
                                                  magI, magJ, magH, magK, 
                                                  magL, magM, confIdx)
    
    # Check if star is accepted or rejected
    if res == 1:
        result = 'star REJECTED'
    else:
        result = 'star ACCEPTED'

    # Test if it is an interpolated star
    if TestedStarName in interpolatedStarsTuple:
        interpolated = '<-- interpolation'
    else:
        interpolated = ''
        
    # Observed results
    magRobs = pyAlx.mcsFLOATp_value(magR)
    magIobs = pyAlx.mcsFLOATp_value(magI)
    magJobs = pyAlx.mcsFLOATp_value(magJ)
    magHobs = pyAlx.mcsFLOATp_value(magH)
    magKobs = pyAlx.mcsFLOATp_value(magK)
    magLobs = pyAlx.mcsFLOATp_value(magL)
    magMobs = pyAlx.mcsFLOATp_value(magM)

    # Expected results
    magRexp = float(resultsDico[TestedStarName][1][2])
    magIexp = float(resultsDico[TestedStarName][1][3])
    magJexp = float(resultsDico[TestedStarName][1][4])
    magHexp = float(resultsDico[TestedStarName][1][5])
    magKexp = float(resultsDico[TestedStarName][1][6])
    magLexp = float(resultsDico[TestedStarName][1][7])
    magMexp = float(resultsDico[TestedStarName][1][8])
   
    # Difference between observed and expected
    diffR = magRobs - magRexp
    diffI = magIobs - magIexp
    diffJ = magJobs - magJexp
    diffH = magHobs - magHexp
    diffK = magKobs - magKexp
    diffL = magLobs - magLexp
    diffM = magMobs - magMexp
    
    # Print results
    print '\n=> Results: ', interpolated, ' ==> ', result
    print '   --------'

    print center('Band', 15), center('Observed(O)', 15), \
          center('Expected (E)', 15), center('Difference O - E', 15)
    print center(resultsColItemList[6], 15), center(str(magRobs), 15), \
          center(str(magRexp), 15), center(str(diffR), 15)
    print center(resultsColItemList[7], 15), center(str(magIobs), 15), \
          center(str(magIexp), 15), center(str(diffI), 15)
    print center(resultsColItemList[8], 15), center(str(magJobs), 15), \
          center(str(magJexp), 15), center(str(diffJ), 15)
    print center(resultsColItemList[9], 15), center(str(magHobs), 15), \
          center(str(magHexp), 15), center(str(diffH), 15)
    print center(resultsColItemList[10], 15), center(str(magKobs), 15), \
          center(str(magKexp), 15), center(str(diffK), 15)
    print center(resultsColItemList[11], 15), center(str(magLobs), 15), \
          center(str(magLexp), 15), center(str(diffL), 15)
    print center(resultsColItemList[12], 15), center(str(magMobs), 15), \
          center(str(magMexp), 15), center(str(diffM), 15)

