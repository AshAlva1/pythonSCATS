import ScatsPythonPlugin

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test00 = [(0, 2)]
EventsForRDDS_PlatoonArrivalDeparture_Test00 = [(0, 0, 0), (12, 0, 0), (15, 0, 0), (25, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test01 = [(10, 3)]
EventsForRDDS_PlatoonArrivalDeparture_Test01 = [(101, 0, 0), (102, 0, 0), (103, 0, 0), (104, 0, 0), (104, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test02 = [(0, 1)]
EventsForRDDS_PlatoonArrivalDeparture_Test02 = [(1, 0, 0), (2, 0, 0), (103, 0, 0), (104, 0, 0), (104, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test03 = [(10, 3)]
EventsForRDDS_PlatoonArrivalDeparture_Test03 = [(101, 0, 0), (102, 0, 0), (103, 0, 0), (104, 0, 0), (104, 0, 0),
                                                (205, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test04 = [(10, 4)]
EventsForRDDS_PlatoonArrivalDeparture_Test04 = [(101, 0, 0), (102, 0, 0), (103, 0, 0), (104, 0, 0), (104, 0, 0),
                                                (105, 0, 0), (106, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test05 = [(10, 4)]
EventsForRDDS_PlatoonArrivalDeparture_Test05 = [(101, 0, 0), (102, 0, 0), (103, 0, 0), (104, 0, 0), (104, 0, 0),
                                                (105, 0, 0), (106, 0, 0), (207, 0, 0), (208, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test06 = [(10, 4)]
EventsForRDDS_PlatoonArrivalDeparture_Test06 = [(101, 0, 0), (102, 0, 0), (103, 0, 0), (104, 0, 0), (104, 0, 0),
                                                (105, 0, 0), (106, 0, 0), (107, 0, 0), (208, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test07 = [(0, 1)]
EventsForRDDS_PlatoonArrivalDeparture_Test07 = [(1, 0, 0), (5, 0, 0), (106, 0, 0), (107, 0, 0), (107, 0, 0),
                                                (108, 0, 0), (109, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test08 = [(11, 3)]
EventsForRDDS_PlatoonArrivalDeparture_Test08 = [(1, 0, 0), (5, 0, 0), (106, 0, 0), (107, 0, 0), (107, 0, 0),
                                                (108, 0, 0), (109, 0, 0), (110, 0, 0), (111, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test09 = [(0, 0)]
EventsForRDDS_PlatoonArrivalDeparture_Test09 = [(101, 0, 0), (102, 0, 0), (103, 0, 0), (204, 0, 0), (204, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test10 = [(0, 1)]
EventsForRDDS_PlatoonArrivalDeparture_Test10 = [(1, 0, 0), (5, 0, 0), (106, 0, 0), (107, 0, 0), (107, 0, 0),
                                                (208, 0, 0), (208, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test11 = [(127, 3)]
EventsForRDDS_PlatoonArrivalDeparture_Test11 = [(1274, 0, 0), (1277, 0, 0), (1278, 0, 0), (1279, 0, 0), (1279, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test12 = [(0, 0)]
EventsForRDDS_PlatoonArrivalDeparture_Test12 = [(1275, 0, 0), (1277, 0, 0), (1278, 0, 0), (1279, 0, 0), (1279, 0, 0)]

ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test13 = [(0, 0)]
EventsForRDDS_PlatoonArrivalDeparture_Test13 = [(101, 0, 0), (102, 0, 0), (103, 0, 0), (104, 0, 0), (205, 0, 0),
                                                (206, 0, 0), (206, 0, 0), (207, 0, 0), (207, 0, 0)]

# Parameter1 ==> Path to the SCATS target build
# Parameter2 ==> Version & configuration info for the ScatsPythonPlugin & the SCATS target build. Fopr now, always set this value to 1.
# Parameter3 ==> Time in milliseconds to wait for SCATS to reach a quiescent state after startup. I have found that I can't get rid this wait-time: otherwise SCATS misbehaves. For now, leave this value at 5000 (5 seconds).
# Parameter4 ==> Path to the SCATS LX configuration file.
ScatsPythonPlugin.StartupScats("C:\\PycharmProjects\\VR89_Test_Suite\\ScatsPythonPlugin\\SCATS.dll", 1,
                               5000,
                               "C:\\PycharmProjects\\VR89_Test_Suite\\ScatsPythonPlugin\\Sys_VR89_UnitTest.lx")


####################################################################################################
####################################################################################################
#
#   Basic_RDDS_PlatoonArrivalDeparture_TestXXSetup()
#   Preamble for Basic_RDDS_PlatoonArrivalDeparture_TestXX.
#
####################################################################################################
####################################################################################################
def Basic_RDDS_PlatoonArrivalDeparture_TestXXSetup():
    ScatsPythonPlugin.SetMinPlatoonLength(3)
    ScatsPythonPlugin.SetMaxRDDStreamSize(10)
    return True


####################################################################################################
####################################################################################################
#
#   Basic_RDDS_PlatoonArrivalDeparture_TestXX(events, expectedResults)
#   Create a SCATS RichDataStream object based on a given set of "events" and exercise SCATS platoon 
#   "calculation" to generate values for platoon length and arrivalDeparture time.
#
####################################################################################################
####################################################################################################
def Basic_RDDS_PlatoonArrivalDeparture_TestXX(events, expectedResults):
    rdds = ScatsPythonPlugin.RichDetectorDataStream(events,
                                                    True)  # boolean indicates if the 1st event in the list of "events" is an "On" event
    for expectedArrivalDep, expectedPlatoonLen in expectedResults:
        assert (expectedArrivalDep == rdds.platoonArrivalDepartureTime)
        assert (expectedPlatoonLen == rdds.platoonLength)

    return True


####################################################################################################
####################################################################################################
####################################################################################################
#
#   main()
#
#   Main entry point for the suite of Platoon Tests
#
####################################################################################################
####################################################################################################
####################################################################################################
#


Basic_RDDS_PlatoonArrivalDeparture_TestXXSetup()
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test00,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test00)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test01,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test01)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test02,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test02)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test03,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test03)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test04,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test04)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test05,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test05)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test06,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test06)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test07,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test07)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test08,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test08)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test09,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test09)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test10,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test10)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test11,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test11)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test12,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test12)
Basic_RDDS_PlatoonArrivalDeparture_TestXX(EventsForRDDS_PlatoonArrivalDeparture_Test13,
                                          ExpectedResultsForRDDS_PlatoonArrivalDeparture_Test13)
