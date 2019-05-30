import ScatsPythonPlugin

# Parameter1 ==> Path to the SCATS target build
# Parameter2 ==> Version & configuration info for the ScatsPythonPlugin & the SCATS target build. Fopr now, always set this value to 1.
# Parameter3 ==> Time in milliseconds to wait for SCATS to reach a quiescent state after startup. I;m toying with getting rid of this. There probably is enough slack in SCATS startup already. For now, leave this value at 5000 (5 seconds).
# Parameter4 ==> Path to the SCATS LX configuration file.
ScatsPythonPlugin.StartupScats("C:\\PycharmProjects\\VR89_Test_Suite\\ScatsPythonPlugin\\SCATS.dll", 1,
                               5000,
                               "C:\\PycharmProjects\\VR89_Test_Suite\\ScatsPythonPlugin\\Sys_VR89_UnitTest.lx")

# Some global magic numbers that correlate to prior knowledge of the configuration of this test (including prior knowledge of what is inside the LX file).
#
# 8 Slots & 3 SubSystems configured in the LX;
# Slot 1 is the master, from which offsets for the remaining 7 are inspected under certain conditions (as setup for each written test).
SLOT1 = 1  # Master
SLOT2 = 2
SLOT3 = 3
SLOT4 = 4
SLOT5 = 5
SLOT6 = 6
SLOT7 = 7
SLOT8 = 8
#
NO_DELAY = -1
CG0 = 0
CG4 = CG0 + 4  # TickTock delay past CG0
CG5 = CG0 + 5  # TickTock delay past CG0
CG6 = CG0 + 6  # TickTock delay past CG0
CG7 = CG0 + 7  # TickTock delay past CG0
CG8 = CG0 + 8  # TickTock delay past CG0
CG9 = CG0 + 9  # TickTock delay past CG0
CG10 = CG0 + 10  # TickTock delay past CG0
CG15 = CG0 + 15  # TickTock delay past CG0
CG16 = CG0 + 16  # TickTock delay past CG0
CG26 = CG0 + 26  # TickTock delay past CG0
CG36 = CG0 + 36  # TickTock delay past CG0
CG46 = CG0 + 46  # TickTock delay past CG0
CG56 = CG0 + 56  # TickTock delay past CG0
#
# Detectors on actually on SLOT#1 SI#1
DET1 = 1  # Detector #1.
DET2 = 2
DET3 = 3
DET4 = 4
DET5 = 5
DET6 = 6
DET15 = 15
DET16 = 16
DET17 = 17
DET18 = 18
DET19 = 19
DET20 = 20
DET21 = 21
#
SI1 = 1
SI2 = 2
SI3 = 3
SI4 = 4
SI5 = 5
SI6 = 6
SI7 = 7
SI8 = 8
#
SS1 = 1  # SubSystem #1
SS2 = 2  # SubSystem #2
SS3 = 3  # SubSystem #3
#
FIRSTPHASE = 1
#
ACTIVE_CYCLE_TIME = 50
CYCLE_TIME = ACTIVE_CYCLE_TIME
#
MAXDIFF = 1  # Maximum value of absolute difference between a test result and an expected value.
#
SPIN_AROUND_ONCE = 1  # number of whole SCATS cycles (full active cycle time).
SPIN_AROUND_TWICE = 2  # number of whole SCATS cycles (full active cycle time).
NO_SPIN = 0


####################################################################################################
####################################################################################################
#
#   SetXkey(rdFlag, vlVkFlag, remedyFlag)
#
#   A helper function used during test initialisation. 
#   I just implemented it because that's how the C++ unit tests were setup.
#   
#   
####################################################################################################
####################################################################################################
def SetXkey(rdFlag, vlVkFlag, remedyFlag):
    ScatsPythonPlugin.SetRichDetectorFlag(rdFlag)
    ScatsPythonPlugin.SetVelVkFlag(vlVkFlag)
    ScatsPythonPlugin.SetRemedyFlag(remedyFlag)
    return


####################################################################################################
####################################################################################################
#
#   InitialiseTest()
#
#   A generic initialisation process before running a test.
#   
#   
####################################################################################################
####################################################################################################
def InitialiseTest():
    SetXkey(False, False, False)
    dimSI = ScatsPythonPlugin.GetMaxSICount()
    for si in range(1, dimSI + 1):
        ScatsPythonPlugin.ClearSI(si)
    #
    return


####################################################################################################
####################################################################################################
#
#   MoveTheClock(subSystem, delay, skipCycles):
#
#   
####################################################################################################
####################################################################################################
def MoveTheClock(subSystem, delay, skipCycles):
    if (NO_DELAY == delay):
        return
    #
    CurrentClockCycleTime = ScatsPythonPlugin.GetSSCurrentClockCycleTime(subSystem)
    while (delay != CurrentClockCycleTime or skipCycles > 0):
        if (
                delay == CurrentClockCycleTime):  # Indicates the start of a new cycle; we want to wait for "skipCycles" many cycles past delay.
            skipCycles -= 1;
        ScatsPythonPlugin.TickTock()
        CurrentClockCycleTime = ScatsPythonPlugin.GetSSCurrentClockCycleTime(subSystem)
    #
    return


####################################################################################################
####################################################################################################
#
#   OffsetRemedyTest1(testName, fromSlot, delay, skipCycles, expectedResults)
#
#   An implementation of the 1st C++ unit test:
#   Exercises SCATS and checks the applied offsets when VR89 is running but there are no platoons. 
#   The applied offsets from SCATS should be exactly as what you would expect from examination of the
#   LX file, but without the disturbance of any (live) data.
#
#   "expectedResults" is a list of pairs ==> toSlot, expectedValue.
####################################################################################################
####################################################################################################
def OffsetRemedyTest1(testName, fromSlot, delay, skipCycles, expectedResults):
    #   Initialisation part1:
    InitialiseTest()
    #
    #   Initialisation part2:
    #   Keep driving SCATS and wait here until we get to CG4.
    subSystem = ScatsPythonPlugin.GetSSForSL(fromSlot)
    MoveTheClock(subSystem, CG4, 0)
    #
    #   Now Starting the actual test.
    #
    ScatsPythonPlugin.SetSSCycleTime(subSystem, CYCLE_TIME)
    ScatsPythonPlugin.SetSSCycleTime(SS2, CYCLE_TIME)
    ScatsPythonPlugin.SetSSCycleTime(SS3, CYCLE_TIME)
    #
    ScatsPythonPlugin.SetSSActiveCycleTime(subSystem, ACTIVE_CYCLE_TIME)
    ScatsPythonPlugin.SetSSActiveCycleTime(SS2, ACTIVE_CYCLE_TIME)
    ScatsPythonPlugin.SetSSActiveCycleTime(SS3, ACTIVE_CYCLE_TIME)
    #
    #   Let's get married (as per the LX file configuration).
    if (not (ScatsPythonPlugin.AttemptSSConnect(SS2) or ScatsPythonPlugin.IsSSConnected(SS2))):
        print("SS2 couldn't get married as expected.")
        return False
    #
    #   Let's get married again (as per the LX file configuration).
    if (not (ScatsPythonPlugin.AttemptSSConnect(SS3) or ScatsPythonPlugin.IsSSConnected(SS3))):
        print("SS3 couldn't get married as expected.")
        return False
    #
    #   All setup went OK. Now run the the actual test scenario. We're going to poke each detector for SLOT1 (from knowledge of LX file)
    #   to get each one to report to SCATS on the next TickTock cycle that (platoon) data is available to be acted upon. But... we're not going
    #   to supply any platoon data!!! So SCATS should run without applying VR89 and produce a set of offsets exactly as per the LX file.
    #
    #   The parameters of interest to function SetDataAvailableForSiteAndDetector() - parameters 2 & 3 - come from the LX file for Slot1
    #   Parameter1 ==>  The RDDP communication channel between the DetectorManager & RDDP.
    #                   Since RDDP is not actually running in this test, this value should be given the nominal default value of 0.
    #   Parameter2 ==>  The SiteID (not the Slot#). Here fromSite corresponds to the site ID for Slot1 in the LX file (actually 17).
    #   Parameter3 ==>  The Detector ID for the site. Each of these detectors are configured against this particular site (LX file).
    #                   We hard code them here, but later we can call an API to provide a collection of detector IDs for each site.
    #   Parameter4 ==>  Time in milliseconds since epoch. This value should be given the nominal default value of 1. It has significance, but not here.
    #
    #   Important!  Each SI can have from 0~4 lanes configured. A lane is considered to be "configured" if it has a detector configured for it.
    #               The LX file makes no specific mention of lanes in the SI configuration, but they are implied by the sequential order of detectors configured against the SI.
    #               For instance, the LX files indicates the following detectors for SI2:
    #                   D#=3-6!
    #               This means detector #3 is configured for lane #0, detector #4 for lane #1, detector #5 for lane #2 and detctor #6 for lane #3.
    #               Each detector at a site belongs to one-and-only-one lane at the site. Each SI at a site can have from 0~4 lanes (i.e. 0~4 detectors) configured.
    #               The (unique) combination of SiteID and Detector ID OR Site ID and lane number identifies the corresponding SI to the DetectorManager.
    #
    #   Function SetDataAvailableForSiteAndDetector operates on the DetectorManager which normally takes input from RDDP.
    #   However, RDDP is not running, so here we poke the DetectorManager in a similar way that RDDP would,
    #   telling the DetectorManager to go & read some data DURING our call to TickTock() (see below).
    #   Of course, for this test, there is no data available because we haven't provided any. But we want to check
    #   that offsets are applied as per the LX file configuration. So here we setup the DetectorManager to think
    #   data is available, after which point (when we call TickTock()) it will notify SCATS which will then apply the
    #   LX file offsets directly. When all the dust has settled (see use of delay & skipCycles below), we will then go & look at the
    #   offsets calculated by SCATS & see if they are what we expect.
    #
    #
    fromSite = ScatsPythonPlugin.GetSiteForSL(fromSlot)
    #
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET1, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET2, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET3, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET4, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET5, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET6, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET15, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET16, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET17, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET18, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET19, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET20, 1)
    ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, fromSite, DET21, 1)
    #
    #   Now keep driving the clock in SCATS until it has digested the above ("bogus") notifications of data.
    #   Once digested, it is safe to go & retrieve the results.
    #   Here we wait for the clock to circumnavigate past delay with skipCycles revolutions.
    MoveTheClock(subSystem, delay, skipCycles)
    #
    #   Now were ready to read the test results
    GetOffsetRemedyTestResults(testName, fromSlot, expectedResults)
    return True


####################################################################################################
####################################################################################################
#
#   GetOffsetRemedyTestResults(testName, fromSlot, expectedResults)
#   Get test results for test testName.
#
#   "expectedResults" is a list of pairs ==> toSlot, expectedValue.
####################################################################################################
####################################################################################################
def GetOffsetRemedyTestResults(testName, fromSlot, expectedResults):
    #
    #   Calculate the new phase time (1st phase) for the fromSlot. Ignore Parameter3 for now (set False).
    fromPhaseTime = ScatsPythonPlugin.PhaseStatus(fromSlot, FIRSTPHASE, False).phaseTime
    fromActiveCycleLength = ScatsPythonPlugin.GetSSActiveCycleTime(fromSlot)
    #
    regressionString = "Slot" + str(fromSlot) + " phasetime = " + str(fromPhaseTime)
    for toSlot, expectedValue in expectedResults:
        toSlotPhaseTime = ScatsPythonPlugin.PhaseStatus(toSlot, FIRSTPHASE, False).phaseTime
        regressionString += ", Slot" + str(toSlot) + " phasetime = " + str(toSlotPhaseTime)
        #
        if (not (CheckOffset(fromPhaseTime, toSlotPhaseTime, expectedValue, fromActiveCycleLength))):
            print("Test " + testName + " failed for slot" + str(toSlot))
            return False
    #
    #   For regression test analysis we can output the actual measured values (say to a file) and then compare with a previous run of this script.
    print(regressionString)
    #
    print("Test " + testName + " passed")
    #
    return True


####################################################################################################
####################################################################################################
#
#   CheckOffset
#   Helper function used during offset remedy result checking.
#   Compare the difference between toPhaseTime and fromPhaseTime with the expectedOffset & determines
#   whether or not they within a MAXDIFF tolerance between each other.
#
####################################################################################################
####################################################################################################
def CheckOffset(fromPhaseTime, toPhaseTime, expectedOffset, fromActiveCycleTime):
    diff = toPhaseTime - fromPhaseTime - expectedOffset
    if (abs(diff) <= MAXDIFF):
        return True
    #
    if (diff < 0):
        diff += fromActiveCycleTime
    else:
        diff -= fromActiveCycleTime
    #
    if (abs(diff) <= MAXDIFF):
        return True
    return False


####################################################################################################
####################################################################################################
#
#   OffsetRemedyTestX(testName, fromSlot, delay, skipCycles)
#
#   An implementation of the 2nd-6th C++ unit tests:
#   Exercises SCATS and checks the applied offsets when VR89 is running but there are no platoons. 
#   The applied offsets from SCATS should be exactly as what you would expect from examination of the
#   LX file, but without the disturbance of any (live) data.
#
#   "expectedResults" is a list of pairs ==> toSlot, expectedValue.
####################################################################################################
####################################################################################################
def OffsetRemedyTestX(testName, fromSlot, delay, skipCycles, expectedResults):
    #
    #   There is no setup preamble to this test. The test is just to move the clock a given number of times
    #   and then check that nothing has changed. Here, we sit & wait until the clock makes skipCycles
    #   revolutions past delay.
    subSystem = ScatsPythonPlugin.GetSSForSL(fromSlot)
    MoveTheClock(subSystem, delay, skipCycles)
    #
    #   Now were ready to read the test results. No changes are "expected"
    return GetOffsetRemedyTestResults(testName, fromSlot, expectedResults)


####################################################################################################
####################################################################################################
#
#   OffsetRemedyPlatoonTest(testNumber, si, laneData, delay, skipCycles)
#   Exercise SCATS, but this time with platoon data (laneData) for the given si.
#
#   "expectedResults" is a list of pairs ==> toSlot, expectedValue.
####################################################################################################
####################################################################################################
def OffsetRemedyPlatoonTest(testName, si, laneData, delay, skipCycles, expectedResults):
    #   Test preamble/setup.
    if (not SetPlatoonValuesForSI(si, laneData)):
        print("Test " + testName + " FAILED")
        return false
    #
    fromSlot = ScatsPythonPlugin.GetSLForSI(si)
    subSystem = ScatsPythonPlugin.GetSSForSL(fromSlot)
    #   Wait for the clock to circumnavigate past delay with skipCycles revolutions.
    MoveTheClock(subSystem, delay, skipCycles)
    #   Now were ready to read the test results.
    return GetOffsetRemedyTestResults(testName, fromSlot, expectedResults)


####################################################################################################
####################################################################################################
#
#   SetPlatoonValuesForSI(si, laneData)
#   Set platoon data (per lane) for the given si.
#
#   "laneData" is a container of triplet values. There may be anywhere between 0 to 4 triplets in the
#   container. Each triplet respresents platoon data for a single lane. The concept of a lane corresponds
#   directly with a detector (although it is possible that a lane is configured with no detector).
#   An SI may have between 0 and 4 lanes configured for it in the LX file. A properly configured lane 
#   in an SI will have an associated detector. When referencing a lane for a given SI, keep in mind 
#   that the lanes may (only) be numbered from 0~3.
####################################################################################################
####################################################################################################
def SetPlatoonValuesForSI(si, laneData):
    if (not ScatsPythonPlugin.IsRddSI(si)):
        print("SI" + str(si) + " is not an RDD SI")
        return false
    #
    #   Iterate all given triplets (i.e. lanes/detectors) in container laneData and set the given platoon
    #   arrivalDep time, platoonLen and ds in each triplet/lane for the given SI.
    thisLane = 0  # 1st lane
    for arrivalDep, platoonLen, ds in laneData:
        print("Setting arrivalDep = " + str(arrivalDep) + ", platoonLen = " + str(platoonLen) + ", ds = " + str(
            ds) + " for SI = " +
              str(si) + ", lane = " + str(thisLane))
        ScatsPythonPlugin.SetPlatoonArrivalDepartureForSILane(si, thisLane, arrivalDep)
        ScatsPythonPlugin.SetPlatoonLengthForSILane(si, thisLane, platoonLen)
        ScatsPythonPlugin.SetDSForSILane(si, thisLane, ds)
        thisLane += 1
    return True


####################################################################################################
####################################################################################################
####################################################################################################
#
#   main()
#
#   Main entry point for the suite of Offset Remedy Tests
#
####################################################################################################
####################################################################################################
####################################################################################################
#
# First Test round: Exercise SCATS with no platoons present.
ExpectedResultsForNoData = [(2, 5), (3, 11), (4, -20 - 8 + 11), (5, -20 + 11), (6, -20 - 8 + 11 + 15),
                            (7, -20 - 8 + 11 + 15 + 10), (8, -20 - 8 + 11 + 15 - 10)]
OffsetRemedyTest1("Test001", SLOT1, CG6, SPIN_AROUND_TWICE, ExpectedResultsForNoData)
OffsetRemedyTestX("Test002", SLOT1, CG16, NO_SPIN, ExpectedResultsForNoData)
OffsetRemedyTestX("Test003", SLOT1, CG26, NO_SPIN, ExpectedResultsForNoData)
OffsetRemedyTestX("Test004", SLOT1, CG36, NO_SPIN, ExpectedResultsForNoData)
OffsetRemedyTestX("Test005", SLOT1, CG46, NO_SPIN, ExpectedResultsForNoData)
OffsetRemedyTestX("Test006", SLOT1, CG0, NO_SPIN, ExpectedResultsForNoData)
#
# Second Test round: Exercise SCATS with platoons present - nowExercise SCATS with specific platoon data input
# Test 7 & 8
NoLaneData = []
LaneDataTest7 = [(19, 5, 70)]
ExpectedResultsForLaneDataTest7And8 = [(2, 5), (3, 11), (4, -(20 + 8) - 8 + 11), (5, -(20 + 8) + 11),
                                       (6, -(20 + 8) - 8 + 11 + 15), (7, -(20 + 8) - 8 + 11 + 15 + 10),
                                       (8, -(20 + 8) - 8 + 11 + 15 - 10)]
OffsetRemedyPlatoonTest("Test007", SI4, LaneDataTest7, CG7, SPIN_AROUND_ONCE, ExpectedResultsForLaneDataTest7And8)
OffsetRemedyPlatoonTest("Test008", SI4, NoLaneData, NO_DELAY, NO_SPIN, ExpectedResultsForLaneDataTest7And8)

LaneDataTest9 = [(-4, 5, 50), (-4, 5, 60), (-4, 5, 70), (0, 15, 80)]
ExpectedResultsForLaneDataTest9And10 = [(2, 5), (3, (11 - 4 / 2)), (4, -(20 + 8) - 8 + (11 - 4 / 2)),
                                        (5, -(20 + 8) + (11 - 4 / 2)), (6, -(20 + 8) - 8 + (11 - 4 / 2) + 15),
                                        (7, -(20 + 8) - 8 + (11 - 4 / 2) + 15 + 10),
                                        (8, -(20 + 8) - 8 + (11 - 4 / 2) + 15 - 10)]
OffsetRemedyPlatoonTest("Test009", SI2, LaneDataTest7, CG7, SPIN_AROUND_ONCE, ExpectedResultsForLaneDataTest9And10)
OffsetRemedyPlatoonTest("Test0010", SI2, NoLaneData, NO_DELAY, NO_SPIN, ExpectedResultsForLaneDataTest9And10)
