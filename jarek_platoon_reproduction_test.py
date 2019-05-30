import pytest
import json
import ScatsPythonPlugin
import common_methods

TEST_NAME = 0
EVENTS = 1
EXPECTED_RESULTS = 2

SCATS_DLL = "C:\\PycharmProjects\\VR89_Test_Suite\\ScatsPythonPlugin\\SCATS.dll"
LX_FILE = "C:\\PycharmProjects\\VR89_Test_Suite\\ScatsPythonPlugin\\Sys_VR89_UnitTest.lx"
TEST_JSON = "C:\\PycharmProjects\\VR89_Test_Suite\\tests\\platoon_detection\\test_data_jarek.json"


def get_expected_results(data):
    return data


def get_vehicle_events(data):
    return data


def load_params_from_json(file_path):
    test_data = []
    with open(file_path) as f:
        data = json.loads(f.read())
        for test in data:
            expected_results = get_expected_results(data[test]["expected_results"])
            events = get_vehicle_events(data[test]["vehicle_events"])
            test_data.append([test, events, expected_results])
        return test_data


@pytest.fixture(scope="module")
def run_scats():
    ScatsPythonPlugin.StartupScats(SCATS_DLL, 1, 5000, LX_FILE)
    set_up(3, 10)
    yield run_scats
    ScatsPythonPlugin.ShutdownScats()


def set_up(platoon_length, max_rdds_stream_size):
    ScatsPythonPlugin.SetMinPlatoonLength(platoon_length)
    ScatsPythonPlugin.SetMaxRDDStreamSize(max_rdds_stream_size)
    return True


@pytest.mark.parametrize("test_data", load_params_from_json(TEST_JSON), ids=common_methods.get_test_id)
def test_jarek_platoon_detection(run_scats, test_data):
    events = test_data[EVENTS]
    expected_results = test_data[EXPECTED_RESULTS]
    rdds = ScatsPythonPlugin.RichDetectorDataStream(events, True)
    for expected_arrival_dep, expected_platoon_length in expected_results:
        assert expected_arrival_dep == rdds.platoonArrivalDepartureTime
        assert expected_platoon_length == rdds.platoonLength
