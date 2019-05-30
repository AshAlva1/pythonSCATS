import ScatsPythonPlugin

TEST_NAME = 0


def get_test_id(test_data):
    return test_data[TEST_NAME]


def move_the_clock(subsystem, delay, skip_cycles):
    current_clock_cycle_time = ScatsPythonPlugin.GetSSCurrentClockCycleTime(subsystem)
    while delay != current_clock_cycle_time or skip_cycles > 0:
        if delay == current_clock_cycle_time:
            skip_cycles -= 1
        ScatsPythonPlugin.TickTock()
        current_clock_cycle_time = ScatsPythonPlugin.GetSSCurrentClockCycleTime(subsystem)
