import pytest
import json
import ScatsPythonPlugin

SCATS_DLL = "C:\\Users\\costelda\\Documents\\VR89_Test_Suite\\ScatsPythonPlugin\\SCATS.dll"
LX_FILE = "C:\\Users\\costelda\\Documents\\VR89_Test_Suite\\ScatsPythonPlugin\\Sys_VR89_UnitTest.lx"
TEST_JSON = "C:\\Users\\costelda\\Documents\\VR89_Test_Suite\\tests\\offset_remedy\\test_data_jarek.json"

# 8 Slots & 3 SubSystems configured in the LX;
# Slot 1 is the master, from which offsets for the remaining 7 are inspected under certain conditions
# (as setup for each written test).
SLOT1 = 1  # Master
SLOT2 = 2
SLOT3 = 3
SLOT4 = 4
SLOT5 = 5
SLOT6 = 6
SLOT7 = 7
SLOT8 = 8
#
CG0 = 0
CG4 = CG0 + 4     # TickTock delay past CG0
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
FIRST_PHASE = 1
#
ACTIVE_CYCLE_TIME = 50
CYCLE_TIME = ACTIVE_CYCLE_TIME
#
MAX_DIFF = 1  # Maximum value of absolute difference between a test result and an expected value.
#
NO_SPIN = 0
SPIN_AROUND_ONCE = 1
SPIN_AROUND_TWICE = 2  # number of whole SCATS cycles (full active cycle time).

INT_17_DETECTORS = [1, 2, 3, 4, 5, 6, 15, 16, 17, 18, 19, 20, 21]

TEST_NAME = 0
LANE_DATA = 1
SI_NUM = 2
CG_STEP = 3
ROTATION_CYCLES = 4
EXPECTED_OFFSETS = 5


def get_lane_data(data):
    if data == 'None':
        return None
    else:
        return data


def get_si_num(data):
    if data == 'None':
        return None
    else:
        return data


def get_clock_step(data):
    return data


def get_skipped_cycles(data):
    return data


def get_slot_offsets(data):
    return data


def load_params_from_json(file_path):
    test_data = []
    with open(file_path) as f:
        data = json.loads(f.read())
        for test in data:
            lane_data = get_lane_data(data[test]["lane_data"])
            si_num = get_si_num(data[test]["si_num"])
            cg_step = get_clock_step(data[test]["rotation_time"])
            rotation_cycles = get_skipped_cycles(data[test]["spin_cycles"])
            expected_offsets = get_slot_offsets(data[test]["slot_offsets"])
            test_data.append([test, lane_data, si_num, cg_step, rotation_cycles, expected_offsets])
        return test_data


def get_test_id(test_data):
    return test_data[TEST_NAME]


@pytest.fixture(scope="module")
def run_scats():
    ScatsPythonPlugin.StartupScats(SCATS_DLL, 1, 5000, LX_FILE)
    set_up()
    yield run_scats
    ScatsPythonPlugin.ShutdownScats()


@pytest.mark.parametrize("test_data", load_params_from_json(TEST_JSON), ids=get_test_id)
def test_jarek(run_scats, test_data):
    if test_data[TEST_NAME] == "Test4":
        ScatsPythonPlugin.SetVR89PreviousSiteForSlot(SLOT3, 17)
    site = ScatsPythonPlugin.GetSiteForSL(SLOT1)
    subsystem = ScatsPythonPlugin.GetSSForSL(SLOT1)
    lane_data = test_data[LANE_DATA]
    set_data_available(site, INT_17_DETECTORS)
    if lane_data is not None:
        set_platoon_values_for_si(test_data[SI_NUM], lane_data)
    move_the_clock(subsystem, test_data[CG_STEP], test_data[ROTATION_CYCLES])
    get_offset_remedy_results(SLOT1, test_data[EXPECTED_OFFSETS])


def set_up():
    set_x_key(False, False, False)
    dim_si = ScatsPythonPlugin.GetMaxSICount()
    for si in range(1, dim_si):
        ScatsPythonPlugin.ClearSI(si)
    move_the_clock(SS1, CG4, NO_SPIN)
    subsystems = [SS1, SS2, SS3]
    set_subsystem_cycle_time(subsystems, CYCLE_TIME)
    set_subsystem_active_cycle_time(subsystems, ACTIVE_CYCLE_TIME)
    if not (ScatsPythonPlugin.IsSSConnected(SS2) and ScatsPythonPlugin.IsSSConeected(SS3)):
        return False


def set_x_key(rd_flag, vl_vk_flag, remedy_flag):
    ScatsPythonPlugin.SetRichDetectorFlag(rd_flag)
    ScatsPythonPlugin.SetVelVkFlag(vl_vk_flag)
    ScatsPythonPlugin.SetRemedyFlag(remedy_flag)
    return


def set_subsystem_cycle_time(subsystems, cycle_time):
    for subsystem in subsystems:
        ScatsPythonPlugin.SetSSCycleTime(subsystem, cycle_time)


def set_subsystem_active_cycle_time(subsystems, active_cycle_time):
    for subsystem in subsystems:
        ScatsPythonPlugin.SetSSActiveCycleTime(subsystem, active_cycle_time)


def set_data_available(site, detectors):
    for detector in detectors:
        ScatsPythonPlugin.SetDataAvailableForSiteAndDetector(0, site, detector, 1)


def get_offset_remedy_results(from_slot, expected_results):
    from_slot_time = ScatsPythonPlugin.PhaseStatus(from_slot, FIRST_PHASE, False).phaseTime
    from_active_cycle_length = ScatsPythonPlugin.GetSSActiveCycleTime(from_slot)
    regression_string = "Slot" + str(from_slot) + " phasetime = " + str(from_slot_time)
    for slot, expected_offset in expected_results:
        to_slot_time = ScatsPythonPlugin.PhaseStatus(slot, FIRST_PHASE, False).phaseTime
        regression_string += ", " + "Slot" + str(slot) + " phasetime = " + str(to_slot_time) + ", offset: " + str(expected_offset)
        # new_slot_times = get_slots_in_same_cycle((from_slot_time, to_slot_time), from_active_cycle_length)
        # check_offset(new_slot_times, expected_offset)
        assert check_offset_temp(from_slot_time, to_slot_time, expected_offset, from_active_cycle_length) is True


def check_offset(phase_times, expected_offset):
    actual_offset = phase_times[1] - phase_times[0]
    assert actual_offset == expected_offset


# TODO fix this later, for now go for cheap and crappy solution
def get_slots_in_same_cycle(slot_times, active_cycle_length):
        from_slot_time = slot_times[0]
        to_slot_time = slot_times[1]
        if from_slot_time >= active_cycle_length and to_slot_time >= active_cycle_length:
            return from_slot_time, to_slot_time
        elif from_slot_time >= active_cycle_length:
            return from_slot_time - active_cycle_length, to_slot_time
        elif to_slot_time >= active_cycle_length:
            return from_slot_time, to_slot_time - active_cycle_length
        else:
            return from_slot_time, to_slot_time


# This is a poor version of offset checking and needs to be replaced by something that doesn't allow a 3 second window
# of "valid" results. When applying an 8 second remedy, this window represents over 30% of the offset
def check_offset_temp(from_slot_time, to_slot_time, expected_offset, active_cycle_length):
    diff = to_slot_time - from_slot_time - expected_offset
    if abs(diff) <= 1:
        return True
    if diff < 0:
        diff += active_cycle_length
    else:
        diff -= active_cycle_length
    if abs(diff) <= 1:
        return True


def set_platoon_values_for_si(si, lane_data):
    this_lane = 0
    for arrival_dep, platoon_len, ds in lane_data:
        ScatsPythonPlugin.SetPlatoonArrivalDepartureForSILane(si, this_lane, arrival_dep)
        ScatsPythonPlugin.SetPlatoonLengthForSILane(si, this_lane, platoon_len)
        ScatsPythonPlugin.SetDSForSILane(si, this_lane, ds)
        this_lane += 1
