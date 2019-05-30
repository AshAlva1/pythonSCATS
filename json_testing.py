import json
import ScatsPythonPlugin

LANE_DATA = 0
SI_NUM = 1
CG_STEP = 2
ROTATION_CYCLES = 3
EXPECTED_OFFSETS = 4


ScatsPythonPlugin.StartupScats("C:\\Users\\costelda\\Documents\\VR89_Test_Suite\\ScatsPythonPlugin\\SCATS.dll", 1,
                               5000,
                                   "C:\\Users\\costelda\\Documents\\VR89_Test_Suite\\ScatsPythonPlugin\\Sys_VR89_UnitTest.lx")


def load_params_from_json(file_path):
    test_data = []
    with open(file_path) as f:
        data = json.load(f)
        for test in data:
            lane_data = get_lane_data(data[test]["lane_data"])
            si_num = get_si_num(data[test]["si_num"])
            cg_step = get_clock_step(data[test]["rotation_time"])
            rotation_cycles = get_skipped_cycles(data[test]["spin_cycles"])
            expected_offsets = get_slot_offsets(data[test]["slot_offsets"])
            test_data.append([lane_data, si_num, cg_step, rotation_cycles, expected_offsets])
        return test_data


def load_params_from_json2(file_path):
    test_data = []
    with open(file_path) as f:
        data = json.loads(f.read())
        for test in data:
            expected_results = get_expected_results(data[test]["expected_results"])
            events = get_vehicle_events(data[test]["vehicle_events"])
            test_data.append([test, events, expected_results])
        return test_data


def get_expected_results(data):
    return data


def get_vehicle_events(data):
    return data


def get_lane_data(data):
    if data == 'None':
        return None
    else:
        return data


def get_si_num(data):
    if data is not None:
        return data
    else:
        return None


def get_clock_step(data):
    return data


def get_skipped_cycles(data):
    return data


def get_slot_offsets(data):
    return data


def move_the_clock(subsystem, delay, skip_cycles):
    current_clock_cycle_time = ScatsPythonPlugin.GetSSCurrentClockCycleTime(subsystem)
    while delay != current_clock_cycle_time or skip_cycles > 0:
        if delay == current_clock_cycle_time:
            skip_cycles -= 1
        ScatsPythonPlugin.TickTock()
        current_clock_cycle_time = ScatsPythonPlugin.GetSSCurrentClockCycleTime(subsystem)


def set_platoon_values_for_si(si, lane_data):
    this_lane = 0
    for arrival_dep, platoon_len, ds in lane_data:
        a = si
        b = arrival_dep
        c = platoon_len
        d = ds
        # ScatsPythonPlugin.SetPlatoonArrivalDepartureForSILane(si, this_lane, arrival_dep)
        # ScatsPythonPlugin.SetPlatoonLengthForSILane(si, this_lane, platoon_len)
        # ScatsPythonPlugin.SetDSForSILane(si, this_lane, ds)
        this_lane += 1


if __name__ == '__main__':
    i = 0
    test_data = load_params_from_json2("C:\\Users\\costelda\\Documents\\VR89_Test_Suite\\tests\\platoon_detection\\test_data_jarek.json")
    set_platoon_values_for_si(test_data[SI_NUM], test_data[LANE_DATA])

