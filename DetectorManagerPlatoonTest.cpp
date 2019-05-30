#include "stdafx.h"
#include "CppUnitTest.h"

#include "TestUtil.h"

#include "..\src\DetectorManager\DetectorManagerTypes.h"
#include "..\src\DetectorManager\RichDetectorDataStream.h"
#include "..\src\DetectorManager\DetectorManager.h"
#include "..\src\DetectorManager\ExtDetector.h"
#include "..\src\DetectorManager\RichDetectorDataProtocol.h"
#include "..\src\DetectorManager\IRichdetectorDataStreamHandler.h"
#include "..\src\DetectorManager\PlatoonArrivalDepartureAndLength.h"
#include <string>
#include <iostream>
#include <memory>
#include <algorithm>
#include "PacketUtil.h"
#include "Packet.h"
#include "..\src\DetectorManager\MessageBytes.h"
#include<boost/tokenizer.hpp>
#include "..\src\DetectorManager\Logger.h"
#include <iostream> 
#include <string> 
#include <sstream> 

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DM_PlatoonTests
{
	using namespace DetectorManager;
	std::array<unsigned int, 3> events6[] = { { 30,5,5 },{ 31,5,5 },{ 430,5,5 },{ 530,5,5 },{ 830,5,5 },{ 930,5,5 } };
	std::array<unsigned int, 3> events10[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
	std::array<unsigned int, 3> events12[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 431,5,5 },{ 530,5,5 },{ 540,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };

	std::array<unsigned int, 3> eventsLate5[] = { { 90,5,5 },{ 135,5,5 },{ 136,5,5 },{ 140,5,5 },{ 150,5,5 },{ 160,5,5 },{ 170,5,5 },{ 190,5,5 },{ 195,5,5 },{ 200,5,5 },{ 205,5,5 },{ 210,5,5 } };
	std::array<unsigned int, 3> eventsEarly5[] = { { 490,5,5 },{ 535,5,5 },{ 536,5,5 },{ 540,5,5 },{ 550,5,5 },{ 560,5,5 },{ 570,5,5 },{ 590,5,5 },{ 595,5,5 },{ 600,5,5 },{ 695,5,5 },{ 700,5,5 } };
	std::array<unsigned int, 3> events2Early5[] = { { 670,5,5 },{ 675,5,5 },{ 686,5,5 },{ 690,5,5 },{ 700,5,5 },{ 710,5,5 },{ 720,5,5 },{ 730,5,5 },{ 735,5,5 },{ 740,5,5 },{ 745,5,5 },{ 750,5,5 } };
	std::array<unsigned int, 3> events3Early5[] = { { 670,10,5 },{ 675,10,5 },{ 686,10,5 },{ 690,10,5 },{ 700,10,5 },{ 710,10,5 },{ 720,10,5 },{ 730,10,5 },{ 735,0,10 },{ 740,10,5 },{ 745,10,5 },{ 750,10,5 } };

	std::array<unsigned int, 3> eventsMid[] = { { 330,5,5 },{ 340,5,5 } };

	std::array<unsigned int, 3> eventsVeryLate[] = { { 930,5,5 },{ 940,5,5 },{ 940,5,5 },{ 950,5,5 },{ 950,5,5 },{ 955,5,5 } };


	TEST_CLASS(DETECTOR_MANAGER_PLATOON_TEST)
	{
	public:
		TEST_CLASS_INITIALIZE(ClassInitialize)
		{
			//
			// so our test cases can be smaller
			//
			::DetectorManager::DetectorManager::sMinPlatoonLength = 3;
			::DetectorManager::RichDetectorDataStream::setMaxStreamSize(10);



//			pRegionCalibrationCandidates = new DetectorCalibration::RegionCalibrationCandidates();

			//
			// global from SCATSSATA.H
			//
//			pSched = new Sched();

			//
			//
			// setup  globals
			//
//			TestUtil::setupSCATS();

//			Sched::SchedStart();
			//
			// wait for everything to start
			//
	//		Logger::WriteMessage("Waiting 3 seconds for stuff to startup");
	//		::Sleep(3000);
		}


		TEST_CLASS_CLEANUP(ClassCleanup)
		{
	//		Logger::WriteMessage("Cleaning up VR89 Test");
	//		Sched::Shutdown();
	//		delete pSched;
	//		pSched = nullptr;

	//		delete pRegionCalibrationCandidates;

	//		TestUtil::teardownSCATS();
		}

		TEST_METHOD_INITIALIZE(TestInit)
		{

		}

		TEST_METHOD_CLEANUP(TestCleanup)
		{
			// nothing to do
		}

		void testArrivalDeparture( RDDEvents& events, PlatoonArrivalDepartureAndLength expected)
		{
			const time_t streamStart = 0;
			const auto streamDuration = 100LL;

			PlatoonArrivalDepartureAndLength plArrDepAndLen;
			RichDetectorDataStream rdds(streamStart, streamStart + streamDuration, std::move(events));
			rdds.platoonArrivalDeparture(plArrDepAndLen);

			Assert::AreEqual((int)expected.getArrivalDeparture(), (int)plArrDepAndLen.getArrivalDeparture(), L"arrival departure");
			Assert::AreEqual((int)expected.getLength(), (int)plArrDepAndLen.getLength(), L"platoon len");
		}

		RDDEvents toRDDEvents(std::array<unsigned int, 3> events[], int len, bool prevEventWasOn = false)
		{
			RDDEvents rdd;
			for (int i = 0; i < len; i++)
			{
				if (prevEventWasOn)
				{
					// its an off event
					rdd.push_back(RDDEvent(events[i][0], events[i][1], events[i][2]));
				}
				else
				{
					// its an on event
					rdd.push_back(RDDEvent(events[i][0]));
				}
				prevEventWasOn = !prevEventWasOn;
			}

			return rdd;
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test00)
		{
			PlatoonArrivalDepartureAndLength exp(0,2);
			std::array<unsigned int, 3> events[] = { { 0,0,0 },{ 12,0,0 },{ 15,0,0 },{ 25,0,0 }};

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test01)
		{
			PlatoonArrivalDepartureAndLength exp(10, 3);
			std::array<unsigned int, 3> events[] = { { 101,0,0 },{ 102,0,0 },{ 103,0,0 },{ 104,0,0 },{ 104,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test02)
		{
			PlatoonArrivalDepartureAndLength exp(0, 1);
			std::array<unsigned int, 3> events[] = { { 1,0,0 },{ 2,0,0 },{ 103,0,0 },{ 104,0,0 },{ 104,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test03)
		{
			PlatoonArrivalDepartureAndLength exp(10, 3);
			std::array<unsigned int, 3> events[] = { { 101,0,0 },{ 102,0,0 },{ 103,0,0 },{ 104,0,0 },{ 104,0,0 },{ 205,0,0 } };
			
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test04)
		{
			PlatoonArrivalDepartureAndLength exp(10, 4);
			std::array<unsigned int, 3> events[] = { { 101,0,0 },{ 102,0,0 },{ 103,0,0 },{ 104,0,0 },{ 104,0,0 },{ 105,0,0 },{ 106,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test05)
		{
			PlatoonArrivalDepartureAndLength exp(10, 4);
			std::array<unsigned int, 3> events[] = { { 101,0,0 },{ 102,0,0 },{ 103,0,0 },{ 104,0,0 },{ 104,0,0 },{ 105,0,0 },{ 106,0,0 },{ 207,0,0 },{ 208,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test06)
		{
			PlatoonArrivalDepartureAndLength exp(10, 4);
			std::array<unsigned int, 3> events[] = { { 101,0,0 },{ 102,0,0 },{ 103,0,0 },{ 104,0,0 },{ 104,0,0 },{ 105,0,0 },{ 106,0,0 },{ 107,0,0 },{ 208,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test07)
		{
			PlatoonArrivalDepartureAndLength exp(0, 1);
			std::array<unsigned int, 3> events[] = { { 1,0,0 },{ 5,0,0 },{ 106,0,0 },{ 107,0,0 },{ 107,0,0 },{ 108,0,0 },{ 109,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test08)
		{
			PlatoonArrivalDepartureAndLength exp(11, 3);
			std::array<unsigned int, 3> events[] = { { 1,0,0 },{ 5,0,0 },{ 106,0,0 },{ 107,0,0 },{ 107,0,0 },{ 108,0,0 },{ 109,0,0 },{ 110,0,0 },{ 111,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test09)
		{
			PlatoonArrivalDepartureAndLength exp;
			std::array<unsigned int, 3> events[] = { { 101,0,0 },{ 102,0,0 },{ 103,0,0 },{ 204,0,0 },{ 204,0,0 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test10)
		{
			PlatoonArrivalDepartureAndLength exp(0, 1);
			std::array<unsigned int, 3> events[] = { { 1,0,0 },{ 5,0,0 },{ 106,0,0 },{ 107,0,0 },{ 107,0,0 },{ 208,0,0 },{ 208,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}
		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test11)
		{
			PlatoonArrivalDepartureAndLength exp(127, 3);
			std::array<unsigned int, 3> events[] = { { 1274,0,0 },{ 1277,0,0 },{ 1278,0,0 },{ 1279,0,0 },{ 1279,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test12)
		{
			PlatoonArrivalDepartureAndLength exp;
			std::array<unsigned int, 3> events[] = { { 1275,0,0 },{ 1277,0,0 },{ 1278,0,0 },{ 1279,0,0 },{ 1279,0,0 } };

			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		TEST_METHOD(RDDS_PlatoonArrivalDeparture_Test13)
		{
			std::array<unsigned int, 3> events[] = { { 101,0,0 },{ 102,0,0 },{ 103,0,0 },{ 104,0,0 },{ 205,0,0 },{ 206,0,0 },{ 206,0,0 },{ 207,0,0 },{ 207,0,0 } };
			PlatoonArrivalDepartureAndLength exp;
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			testArrivalDeparture(ev, exp);
		}

		//
		// for convenience we declare this class to check the expected results
		//
		class ExpectedUpstreamDetectorPlatoonOffsetInfo : public UpstreamDetectorPlatoonOffsetInfo
		{
		public:
			ExpectedUpstreamDetectorPlatoonOffsetInfo(Metres distanceToStopLine, int plOffset, unsigned int	plLen, unsigned int	toStopLineDSecs,
				unsigned int numArrivals, int prevOffset, int curPlOffset, unsigned int	curPlLen,
				int latePlOffset, unsigned int latePlLen, int earlyPlOffset, unsigned int earlyPlLen) :
				UpstreamDetectorPlatoonOffsetInfo(distanceToStopLine)
			{
				mPlatoon.setOffset(plOffset);
				mPlatoon.setLength(plLen);
			
				mToStopLineDSecs = toStopLineDSecs;
				mNumArrivals = numArrivals;
			
				mPreviousPlatoon.setOffset(prevOffset);
				mCurrentPlatoon.setOffset(curPlOffset);
				mCurrentPlatoon.setLength(curPlLen);
				mLatePlatoon.setOffset(latePlOffset);
				mLatePlatoon.setLength(latePlLen);
				mEarlyPlatoon.setOffset(earlyPlOffset);
				mEarlyPlatoon.setLength(earlyPlLen);
			}

			void AreEqual(const UpstreamDetectorPlatoonOffsetInfo& platoonInfo) const
			{
				Assert::AreEqual(mPlatoon.getOffset(), platoonInfo.mPlatoon.getOffset(), L"Platoon Offset");
				Assert::AreEqual(mPlatoon.getLength(), platoonInfo.mPlatoon.getLength(), L"Platoon Length");
				Assert::AreEqual(mToStopLineDSecs, platoonInfo.getDecisecondsToStopLine(), L"Deciseconds to Stop line");
				Assert::AreEqual(mNumArrivals, platoonInfo.getNumArrivals(), L"Num arrivals");
				Assert::AreEqual(mPreviousPlatoon.getOffset(), platoonInfo.mPreviousPlatoon.getOffset(), L"previous platoon offset");
				Assert::AreEqual(mCurrentPlatoon.getLength(), platoonInfo.mCurrentPlatoon.getLength(), L"current platoon length");
				Assert::AreEqual(mCurrentPlatoon.getOffset(), platoonInfo.mCurrentPlatoon.getOffset(), L"current platoon offset");
				Assert::AreEqual(mLatePlatoon.getOffset(), platoonInfo.mLatePlatoon.getOffset(), L"late platoon offset");
				Assert::AreEqual(mLatePlatoon.getLength(), platoonInfo.mLatePlatoon.getLength(), L"late platoon length");
				Assert::AreEqual(mEarlyPlatoon.getOffset(), platoonInfo.mEarlyPlatoon.getOffset(), L"early platoon offset");
				Assert::AreEqual(mEarlyPlatoon.getLength(), platoonInfo.mEarlyPlatoon.getLength(), L"early platoon length");
			}
		};

		TEST_METHOD(Upstream01_All_Platoon)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 }, { 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));
			UpstreamDetectorPlatoonOffsetInfo platoonInfo(20);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 5, 40,
				0, -70, 30, 5,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream02_All_Platoon_Repeat)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));
			UpstreamDetectorPlatoonOffsetInfo platoonInfo(20);
			rdds.platoonUpstreamOffsets(platoonInfo);
			rdds.platoonUpstreamOffsets(platoonInfo);
			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 5, 40,
				0, -70, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream03_Idle)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));
			UpstreamDetectorPlatoonOffsetInfo platoonInfo(20);
			rdds.platoonUpstreamOffsets(platoonInfo);
			rdds.platoonUpstreamOffsets(platoonInfo);

			RDDEvents evEmpty;
			RichDetectorDataStream rddsEmpty(0, 100, std::move(evEmpty));
			rddsEmpty.platoonUpstreamOffsets(platoonInfo);
			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 0, 40,
				0, -101, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream04_AllPlatoonAgain)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 0, 40,
				0, -101, 0, 0,
				0, 0, 0, 0);;

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70, 5, 40,
				0, -70, 70, 5,
				70, 5, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream05_Platoon1_8)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70, 5, 40,
				0, -70, 70, 5,
				70, 5, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70, 9, 40,
				0, -270, 0, 0,
				70, 9, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream06_Platoon1_8_Repeat)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70, 9, 40,
				0, -270, 0, 0,
				70, 9, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70, 4, 40,
				0, -270, 0, 0,
				70, 4, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream07_AllPlatoonAgain)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70, 4, 40,
				0, -270, 0, 0,
				70, 4, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70, 5, 40,
				0, -70, 70, 5,
				70, 5, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream08_Platoon7_12)
		{
			std::array<unsigned int, 3> events[] = { { 530,5,5 },{ 540,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70, 5, 40,
				0, -70, 70, 5,
				70, 5, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430, 3, 40,
				0, -70, -430, 3,
				0, 0, -430, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream09_Platoon7_12_Repeat)
		{
			std::array<unsigned int, 3> events[] = { { 530,5,5 },{ 540,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430, 3, 40,
				0, -70, -430, 3,
				0, 0, -430, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430, 3, 40,
				0, -70, -430, 3,
				0, 0, -430, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream10_Platoon1_8_Again)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430, 3, 40,
				0, -70, -430, 3,
				0, 0, -430, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430, 7, 40,
				0, -270, 0, 0,
				0, 0, -430, 7);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream11_Platoon7_12_Again)
		{
			std::array<unsigned int, 3> events[] = { { 530,5,5 },{ 540,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430, 7, 40,
				0, -270, 0, 0,
				0, 0, -430, 7);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430, 3, 40,
				0, -70, -430, 3,
				0, 0, -430, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream12_No_Platoon)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 31,5,5 },{ 430,5,5 },{ 530,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430, 3, 40,
				0, -70, -430, 3,
				0, 0, -430, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430, 4, 40,
				0, -70, -130, 1,
				0, 0, -430, 4);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream13_No_Platoon_Repeat)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 31,5,5 },{ 430,5,5 },{ 530,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]));
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430, 4, 40,
				0, -70, -130, 1,
				0, 0, -430, 4);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 3, 40,
				0, -70, -130, 1,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream14_Late3Early5_both_discarded)
		{
				
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late3Early5[0],late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 3, 40,
				0, -70, -130, 1,
				0, 0, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 8, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream15_Late3Early5_both_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 8, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 8, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream16_Late2Early5_late_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late2Early5;
			late2Early5.insert(late2Early5.end(), eventsLate5 + 2, eventsLate5 + 6);
			late2Early5.insert(late2Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late2Early5[0], late2Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 8, 40,
					0, -400, 0, 0,
					0, 0, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470, 5, 40,
				0, -400, 0, 0,
				0, 0, -470, 5);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream17_Late2Early5_late_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late2Early5;
			late2Early5.insert(late2Early5.end(), eventsLate5 + 2, eventsLate5 + 6);
			late2Early5.insert(late2Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late2Early5[0], late2Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 8, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470, 5, 40,
				0, -400, 0, 0,
				0, 0, -470, 5);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream18_Late3Early6_late_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early6;
			late3Early6.insert(late3Early6.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early6.insert(late3Early6.end(), eventsEarly5, eventsEarly5 + 12);

			RDDEvents ev = toRDDEvents(&late3Early6[0], late3Early6.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470, 5, 40,
				0, -400, 0, 0,
				0, 0, -470, 5);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470, 3, 40,
				0, -300, 0, 0,
				0, 0, -470, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream19_Late3Early6_late_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early6;
			late3Early6.insert(late3Early6.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early6.insert(late3Early6.end(), eventsEarly5, eventsEarly5 + 12);

			RDDEvents ev = toRDDEvents(&late3Early6[0], late3Early6.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470, 3, 40,
				0, -300, 0, 0,
				0, 0, -470, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470, 3, 40,
				0, -300, 0, 0,
				0, 0, -470, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream20_Late5Early3_both_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6);
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470, 3, 40,
				0, -300, 0, 0,
				0, 0, -470, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 8, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream21_Late5Early3_both_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6);
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470, 3, 40,
				0, -300, 0, 0,
				0, 0, -470, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 8, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream22_Late5Early2_early_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 4);
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 8, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130, 5, 40,
				0, -460, 0, 0,
				130, 5, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream23_Late5Early3_early_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 4);
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 8, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130, 5, 40,
				0, -460, 0, 0,
				130, 5, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream24_Late6Early3_early_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 12);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				130, 5, 40,
				0, -460, 0, 0,
				130, 5, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130, 3, 40,
				0, -440, 0, 0,
				130, 3, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream25_Arr1Late3Early5_only_early)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // arr 1
			late3Early5.insert(late3Early5.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				130, 3, 40,
				0, -440, 0, 0,
				130, 3, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470, 5, 40,
				0, -400, 0, 0,
				0, 0, -470, 5);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream26_Arr1Late3Early5_both_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // arr 1
			late3Early5.insert(late3Early5.end(), eventsLate5 + 2, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470, 5, 40,
				0, -400, 0, 0,
				0, 0, -470, 5);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 10, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}
		TEST_METHOD(Upstream27_Late5Arr1Early3_both_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsMid, eventsMid + 2); // Arr1
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 10, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 9, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream28_Late5Arr1Early2_early_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsMid, eventsMid + 2); // Arr1
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 4);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 9, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130, 5, 40,
				0, -460, 0, 0,
				130, 5, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream28_Late5Arr1Early2F_early_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsMid, eventsMid + 2); // Arr1
			late3Early5.insert(late3Early5.end(), events3Early5, events3Early5 + 4);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				130, 5, 40,
				0, -460, 0, 0,
				130, 5, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130, 5, 33,
				0, -310, 0, 0,
				130, 5, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream29_Single)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsMid, eventsMid + 2); // Arr1

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				130, 5, 33,
				0, -310, 0, 0,
				130, 5, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 1, 40,
				0, -660, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream29_Late3Late4_Late4_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // Late1
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 4); // Late 2-3
			late3Early5.insert(late3Early5.end(), events12 + 4, events12 + 12); // Late 1-4

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 1, 40,
					0, -660, 0, 0,
					0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70, 3, 40,
				0, -70, 470, 4,
				70, 3, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream30_Late3Late4_Late4_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // Late1
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 4); // Late 2-3
			late3Early5.insert(late3Early5.end(), events12 + 4, events12 + 12); // Late 1-4

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70, 3, 40,
				0, -70, 470, 4,
				70, 3, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				470, 3 + 4, 40,
				0, -70, 470, 4,
				470, 3 + 4, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream31_Idle_prev_Late4)
		{
			RDDEvents evNone;
			RichDetectorDataStream rdds(0, 100, std::move(evNone));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				470, 3 + 4, 40,
				0, -70, 470, 4,
				470, 3 + 4, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 0, 40,
				0, -101, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream32_Late3Late7_Late3_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // Late1
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 4); // Late 2-3
			late3Early5.insert(late3Early5.end(), events12 + 4, events12 + 12); // Late 1-4
			late3Early5.insert(late3Early5.end(), eventsVeryLate, eventsVeryLate + 6); // Late 5-7
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 0, 40,
				0, -101, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				470, 4, 40,
				0, -45, 470, 7,
				470, 4, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream33_Late3Late7_Early3_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6); // Early 3
			late3Early5.insert(late3Early5.end(), events2Early5, events2Early5 + 8); // Early 4	
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size()); 
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				470, 4, 40,
				0, -45, 470, 7,
				470, 4, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-290, 4, 40,
				0, -270, 0, 0,
				0, 0, -290, 4);

			expInfo.AreEqual(platoonInfo);
		}


		TEST_METHOD(Upstream34_Late3Late7_Early3_discarded_faster)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6); // Early 3
			late3Early5.insert(late3Early5.end(), events3Early5, events3Early5 + 8); // Early 4			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-290, 4, 40,
				0, -270, 0, 0,
				0, 0, -290, 4);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-301, 4, 29,
				0, -270, 0, 0,
				0, 0, -301, 4);

			expInfo.AreEqual(platoonInfo);
		}

		//
		// repeat tests, but treat the first event as an off event
		//
		TEST_METHOD(Upstream_OnOff_01_AllPlatoon)
		{
			std::vector<std::array<unsigned int, 3>> ev;
			ev.insert(ev.end(), events10, events10 + 10);
			
			RDDEvents evRdd = toRDDEvents(&ev[0], ev.size(), true);
			RichDetectorDataStream rdds(0, 100, std::move(evRdd));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-301, 4, 29,
				0, -270, 0, 0,
				0, 0, -301, 4);
			platoonInfo.setDecisecondsToStopLine(0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 5, 40,
				0, -70, 30 + 100, 5,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_02_All_Platoon_Repeat)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 5, 40,
				0, -70, 30 + 100, 5,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);
			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 5, 40,
				0, -70, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_03_Idle)
		{
			RDDEvents evNone; 
			RichDetectorDataStream rdds(0, 100, std::move(evNone));
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 5, 40,
				0, -70, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 0, 40,
				0, -101, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_04_AllPlatoonAgain)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 0, 40,
				0, -101, 0, 0,
				0, 0, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70 + 100, 5, 40,
				0, -70, 70 + 100, 5,
				70 + 100, 5, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_05_Platoon1_8)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70 + 100, 5, 40,
				0, -70, 70 + 100, 5,
				70 + 100, 5, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70 + 100, 9, 40,
				0, -270, 0, 0,
				70 + 100, 9, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_06_Platoon1_8_Repeat)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70 + 100, 9, 40,
				0, -270, 0, 0,
				70 + 100, 9, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70 + 100, 4, 40,
				0, -270, 0, 0,
				70 + 100, 4, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_07_AllPlatoonAgain)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70 + 100, 4, 40,
				0, -270, 0, 0,
				70 + 100, 4, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70 + 100, 5, 40,
				0, -70, 70 + 100, 5,
				70 + 100, 5, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_08_Platoon7_12)
		{
			std::array<unsigned int, 3> events[] = { { 530,5,5 },{ 540,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70 + 100, 5, 40,
				0, -70, 70 + 100, 5,
				70 + 100, 5, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430 + 10, 3, 40,
				0, -70, -430 + 10, 3,
				0, 0, -430 + 10, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_09_Platoon7_12_Repeat)
		{
			std::array<unsigned int, 3> events[] = { { 530,5,5 },{ 540,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430 + 10, 3, 40,
				0, -70, -430 + 10, 3,
				0, 0, -430 + 10, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430 + 10, 3, 40,
					0, -70, -430 + 10, 3,
					0, 0, -430 + 10, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_10_Platoon1_8_Again)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 130,5,5 },{ 230,5,5 },{ 330,5,5 },{ 430,5,5 },{ 530,5,5 },{ 630,5,5 },{ 730,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430 + 10, 3, 40,
				0, -70, -430 + 10, 3,
				0, 0, -430 + 10, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430 + 10, 7, 40,
				0, -270, 0, 0,
				0, 0, -430 + 10, 7);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_11_Platoon7_12_Again)
		{
			std::array<unsigned int, 3> events[] = { { 530,5,5 },{ 540,5,5 },{ 630,5,5 },{ 730,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430 + 10, 7, 40,
				0, -270, 0, 0,
				0, 0, -430 + 10, 7);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430 + 10, 3, 40,
				0, -70, -430 + 10, 3,
				0, 0, -430 + 10, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_12_No_Platoon)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 31,5,5 },{ 430,5,5 },{ 530,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430 + 10, 3, 40,
				0, -70, -430 + 10, 3,
				0, 0, -430 + 10, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-430 + 10, 4, 40,
				0, -70, -130 + 100, 1,
				0, 0, -430 + 10, 4);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_13_No_Platoon_Repeat)
		{
			std::array<unsigned int, 3> events[] = { { 30,5,5 },{ 31,5,5 },{ 430,5,5 },{ 530,5,5 },{ 830,5,5 },{ 930,5,5 } };
			RDDEvents ev = toRDDEvents(events, sizeof(events) / sizeof(events[0]),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-430 + 10, 4, 40,
				0, -70, -130 + 100, 1,
				0, 0, -430 + 10, 4);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 3, 40,
				0, -70, -130 + 100, 1,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_14_Late3Early5_both_discarded)
		{

			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 3, 40,
				0, -70, -130 + 100, 1,
				0, 0, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 8, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_15_Late3Early5_both_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 8, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 8, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_16_Late2Early5_late_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late2Early5;
			late2Early5.insert(late2Early5.end(), eventsLate5 + 2, eventsLate5 + 6);
			late2Early5.insert(late2Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late2Early5[0], late2Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 8, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470 + 45, 5, 40,
				0, -400, 0, 0,
				0, 0, -470 + 45, 5);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_17_Late2Early5_late_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late2Early5;
			late2Early5.insert(late2Early5.end(), eventsLate5 + 2, eventsLate5 + 6);
			late2Early5.insert(late2Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late2Early5[0], late2Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470 + 45, 5, 40,
				0, -400, 0, 0,
				0, 0, -470 + 45, 5);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470 + 45, 5, 40,
				0, -400, 0, 0,
				0, 0, -470 + 45, 5);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_18_Late3Early6_late_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early6;
			late3Early6.insert(late3Early6.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early6.insert(late3Early6.end(), eventsEarly5, eventsEarly5 + 12);

			RDDEvents ev = toRDDEvents(&late3Early6[0], late3Early6.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470 + 45, 5, 40,
				0, -400, 0, 0,
				0, 0, -470 + 45, 5);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470 + 45, 3, 40,
				0, -300, 0, 0,
				0, 0, -470 + 45, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_19_Late3Early6_late_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early6;
			late3Early6.insert(late3Early6.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early6.insert(late3Early6.end(), eventsEarly5, eventsEarly5 + 12);

			RDDEvents ev = toRDDEvents(&late3Early6[0], late3Early6.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470 + 45, 3, 40,
				0, -300, 0, 0,
				0, 0, -470 + 45, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470 + 45, 3, 40,
				0, -300, 0, 0,
				0, 0, -470 + 45, 3);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_20_Late5Early3_both_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6);
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470 + 45, 3, 40,
				0, -300, 0, 0,
				0, 0, -470 + 45, 3);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 8, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_21_Late5Early3_both_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6);
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 8, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);

			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 8, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_22_Late5Early2_early_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 4);
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 8, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130 + 45, 5, 40,
				0, -460, 0, 0,
				130 + 45, 5, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_23_Late5Early3_early_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 4);
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				130 + 45, 5, 40,
				0, -460, 0, 0,
				130 + 45, 5, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130 + 45, 5, 40,
				0, -460, 0, 0,
				130 + 45, 5, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_24_Late6Early3_early_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 12);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				130 + 45, 5, 40,
				0, -460, 0, 0,
				130 + 45, 5, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130 + 45, 3, 40,
				0, -440, 0, 0,
				130 + 45, 3, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_25_Arr1Late3Early5_only_early)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // arr 1
			late3Early5.insert(late3Early5.end(), eventsLate5 + 2, eventsLate5 + 8);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(), true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				130 + 45, 3, 40,
				0, -440, 0, 0,
				130 + 45, 3, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-470 + 45, 5, 40,
				0, -400, 0, 0,
				0, 0, -470 + 45, 5);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_26_Arr1Late3Early5_both_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // arr 1
			late3Early5.insert(late3Early5.end(), eventsLate5 + 2, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 10);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-470 + 45, 5, 40,
				0, -400, 0, 0,
				0, 0, -470 + 45, 5);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 10, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}
		TEST_METHOD(Upstream_OnOff_27_Late5Arr1Early3_both_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsMid, eventsMid + 2); // Arr1
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 10, 40,
				0, -400, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 9, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_28_Late5Arr1Early2_early_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsMid, eventsMid + 2); // Arr1
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 4);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 9, 40,
				0, -440, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130 + 45, 5, 40,
				0, -460, 0, 0,
				130 + 45, 5, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_28_Late5Arr1Early2F_early_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 10);
			late3Early5.insert(late3Early5.end(), eventsMid, eventsMid + 2); // Arr1
			late3Early5.insert(late3Early5.end(), events3Early5, events3Early5 + 4);

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				130 + 45, 5, 40,
				0, -460, 0, 0,
				130 + 45, 5, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				130 + 45, 5, 33,
				0, -310, 0, 0,
				130 + 45, 5, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_29_Single)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsMid, eventsMid + 2); // Arr1

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				130 + 45, 5, 33,
				0, -310, 0, 0,
				130 + 45, 5, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 1, 40,
				0, -660, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_29_Late3Late4_Late4_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // Late1
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 4); // Late 2-3
			late3Early5.insert(late3Early5.end(), events12 + 4, events12 + 12); // Late 1-4

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 1, 40,
				0, -660, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				70 + 1, 3, 40,
				0, -70, 470 + 1, 4,
				70 + 1, 3, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_30_Late3Late4_Late4_discarded_repeat)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // Late1
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 4); // Late 2-3
			late3Early5.insert(late3Early5.end(), events12 + 4, events12 + 12); // Late 1-4

			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				70 + 1, 3, 40,
				0, -70, 470 + 1, 4,
				70 + 1, 3, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				470 + 1, 3 + 4, 40,
				0, -70, 470 + 1, 4,
				470 + 1, 3 + 4, 0, 0);
			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_31_Idle_prev_Late4)
		{
			RDDEvents evNone;
			RichDetectorDataStream rdds(0, 100, std::move(evNone));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				470 + 1, 3 + 4, 40,
				0, -70, 470 + 1, 4,
				470 + 1, 3 + 4, 0, 0);;
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				0, 0, 40,
				0, -101, 0, 0,
				0, 0, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_32_Late3Late7_Late3_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), events6, events6 + 2); // Late1
			late3Early5.insert(late3Early5.end(), eventsLate5, eventsLate5 + 4); // Late 2-3
			late3Early5.insert(late3Early5.end(), events12 + 4, events12 + 12); // Late 1-4
			late3Early5.insert(late3Early5.end(), eventsVeryLate, eventsVeryLate + 6); // Late 5-7
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(), true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				0, 0, 40,
				0, -101, 0, 0,
				0, 0, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				470 + 1, 4, 40,
					0, -45, 470 + 1, 7,
					470 + 1, 4, 0, 0);

			expInfo.AreEqual(platoonInfo);
		}

		TEST_METHOD(Upstream_OnOff_33_Late3Late7_Early3_discarded)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6); // Early 3
			late3Early5.insert(late3Early5.end(), events2Early5, events2Early5 + 8); // Early 4	
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				470 + 1, 4, 40,
				0, -45, 470 + 1, 7,
				470 + 1, 4, 0, 0);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-290 + 5, 4, 40,
				0, -270, 0, 0,
				0, 0, -290 + 5, 4);

			expInfo.AreEqual(platoonInfo);
		}


		TEST_METHOD(Upstream_OnOff_34_Late3Late7_Early3_discarded_faster)
		{
			std::vector<std::array<unsigned int, 3>> late3Early5;
			late3Early5.insert(late3Early5.end(), eventsEarly5, eventsEarly5 + 6); // Early 3
			late3Early5.insert(late3Early5.end(), events3Early5, events3Early5 + 8); // Early 4			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size());
			RDDEvents ev = toRDDEvents(&late3Early5[0], late3Early5.size(),true);
			RichDetectorDataStream rdds(0, 100, std::move(ev));

			//
			// setup precondition
			//
			UpstreamDetectorPlatoonOffsetInfo platoonInfo = ExpectedUpstreamDetectorPlatoonOffsetInfo(20,
				-290 + 5, 4, 40,
				0, -270, 0, 0,
				0, 0, -290 + 5, 4);
			rdds.platoonUpstreamOffsets(platoonInfo);

			ExpectedUpstreamDetectorPlatoonOffsetInfo expInfo(20,
				-301 + 5, 4, 29,
				0, -270, 0, 0,
				0, 0, -301 + 5, 4);
			expInfo.AreEqual(platoonInfo);
		}



	};
}