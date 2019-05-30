import ScatsPythonPlugin

TEST_NAME = 0

FIRST_PHASE = 1


def get_test_id(test_data):
    return test_data[TEST_NAME]


def move_the_clock(subsystem, delay, skip_cycles):
    current_clock_cycle_time = ScatsPythonPlugin.GetSSCurrentClockCycleTime(subsystem)
    while delay != current_clock_cycle_time or skip_cycles > 0:
        if delay == current_clock_cycle_time:
            skip_cycles -= 1
        ScatsPythonPlugin.TickTock()
        current_clock_cycle_time = ScatsPythonPlugin.GetSSCurrentClockCycleTime(subsystem)


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


