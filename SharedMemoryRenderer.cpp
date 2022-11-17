#include "SharedMemoryRenderer.h"
#include "Utils.h"
#include <regex>

// define tab length and new line string for formatted output
#define TAB "  "
#define NEW_LINE "\r\n"

SharedMemoryRenderer::SharedMemoryRenderer() {};
std::string tab;
std::string nl;	//new line

void renderBuildInfo(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"buildinfo\":{" << nl;
	ss << tab << tab << "\"mVersion\":" << sharedData->mVersion << "," << nl;
	ss << tab << tab << "\"mBuildVersionNumber\":" << sharedData->mBuildVersionNumber << nl << tab << "}";
}

void renderGameStates(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"gameStates\":{" << nl;
	ss << tab << tab << "\"mGameState\":" << sharedData->mGameState << "," << nl;
	ss << tab << tab << "\"mSessionState\":" << sharedData->mSessionState << "," << nl;
	ss << tab << tab << "\"mRaceState\":" << sharedData->mRaceState << nl << tab << "}";
}

void renderParticipant(std::stringstream& ss, const ParticipantInfo participantInfo) {
	ss << tab << tab << tab << "{" << nl << tab << tab << tab << tab << "\"mIsActive\":" << (participantInfo.mIsActive ? "true" : "false") << "," << nl;
	ss << tab << tab << tab << tab << "\"mName\":\"" << participantInfo.mName << "\"," << nl;
	ss << tab << tab << tab << tab << "\"mWorldPosition\":[" << participantInfo.mWorldPosition[0] << "," << participantInfo.mWorldPosition[1] << "," << participantInfo.mWorldPosition[2] << "]," << nl;
	ss << tab << tab << tab << tab << "\"mCurrentLapDistance\":" << participantInfo.mCurrentLapDistance << "," << nl;
	ss << tab << tab << tab << tab << "\"mRacePosition\":" << participantInfo.mRacePosition << "," << nl;
	ss << tab << tab << tab << tab << "\"mLapsCompleted\":" << participantInfo.mLapsCompleted << "," << nl;
	ss << tab << tab << tab << tab << "\"mCurrentLap\":" << participantInfo.mCurrentLap << "," << nl;
	ss << tab << tab << tab << tab << "\"mCurrentSector\":" << participantInfo.mCurrentSector << "," << nl;
}

void renderParticipants(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"participants\":{" << nl;
	ss << tab << tab << "\"mViewedParticipantIndex\":" << sharedData->mViewedParticipantIndex << "," << nl;
	ss << tab << tab << "\"mNumParticipants\":" << sharedData->mNumParticipants;

	if (sharedData->mNumParticipants > -1) {
		ss << "," << nl;
		ss << tab << tab << "\"mParticipantInfo\":[" << nl;

		for (int i = 0; i < sharedData->mNumParticipants; i++) {
			renderParticipant(ss, sharedData->mParticipantInfo[i]);
			// pcars2 additional participant data:
			ss << tab << tab << tab << tab << "\"mRaceStates\":" << sharedData->mRaceStates[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mPitModes\":" << sharedData->mPitModes[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mFastestLapTimes\":" << sharedData->mFastestLapTimes[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mLastLapTimes\":" << sharedData->mLastLapTimes[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mFastestSector1Times\":" << sharedData->mFastestSector1Times[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mFastestSector2Times\":" << sharedData->mFastestSector2Times[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mFastestSector3Times\":" << sharedData->mFastestSector3Times[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mCurrentSector1Times\":" << sharedData->mCurrentSector1Times[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mCurrentSector2Times\":" << sharedData->mCurrentSector2Times[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mCurrentSector3Times\":" << sharedData->mCurrentSector3Times[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mLapsInvalidated\":" << sharedData->mLapsInvalidated[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mOrientations\":[" << sharedData->mOrientations[i][0] << "," << sharedData->mOrientations[i][1] << "," << sharedData->mOrientations[i][2] << "]," << nl;
			ss << tab << tab << tab << tab << "\"mSpeeds\":" << sharedData->mSpeeds[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mCarNames\":\"" << sharedData->mCarNames[i] << "\"," << nl;
			ss << tab << tab << tab << tab << "\"mCarClassNames\":\"" << sharedData->mCarClassNames[i] << "\"," << nl;
			ss << tab << tab << tab << tab << "\"mPitSchedules\":" << sharedData->mPitSchedules[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mHighestFlagColours\":" << sharedData->mHighestFlagColours[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mHighestFlagReasons\":" << sharedData->mPitModes[i] << "," << nl;
			ss << tab << tab << tab << tab << "\"mNationalities\":" << sharedData->mPitModes[i] << nl << tab << tab << tab << "}";
			// End pcars2 participant data

			if (i < (sharedData->mNumParticipants - 1)) {
				ss << "," << nl;
			}
		}
		ss << nl << tab << tab << "]";
	}
	ss << nl << tab << "}";
}

void renderUnfilteredInput(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"unfilteredInput\":{" << nl;
	ss << tab << tab << "\"mUnfilteredThrottle\":" << sharedData->mUnfilteredThrottle << "," << nl;
	ss << tab << tab << "\"mUnfilteredBrake\":" << sharedData->mUnfilteredBrake << "," << nl;
	ss << tab << tab << "\"mUnfilteredSteering\":" << sharedData->mUnfilteredSteering << "," << nl;
	ss << tab << tab << "\"mUnfilteredClutch\":" << sharedData->mUnfilteredClutch << "," << nl;
	ss << tab << tab << "\"mJoyPad0\":" << sharedData->mJoyPad0 << "," << nl;	//AMS2 v 1.3.3.0 Shared Memory v10 additional data
	ss << tab << tab << "\"mDPad\":" << sharedData->mDPad << nl << tab << "}";	//AMS2 v 1.3.3.0 Shared Memory v10 additional data
}

void renderVehicleInformation(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"vehicleInformation\":{" << nl;
	ss << tab << tab << "\"mCarName\":\"" << sharedData->mCarName << "\"," << nl;
	ss << tab << tab << "\"mCarClassName\":\"" << sharedData->mCarClassName << "\"" << nl << tab << "}";
}

void renderEventInformation(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"eventInformation\":{" << nl;
	ss << tab << tab << "\"mLapsInEvent\":" << sharedData->mLapsInEvent << "," << nl;
	ss << tab << tab << "\"mSessionDuration\":" << sharedData->mSessionDuration << "," << nl;	//AMS2 v 1.3.3.0 Shared Memory v10 additional data
	ss << tab << tab << "\"mSessionAdditionalLaps\":" << sharedData->mSessionAdditionalLaps << "," << nl;	//AMS2 v 1.3.3.0 Shared Memory v10 additional data
	ss << tab << tab << "\"mTrackLocation\":\"" << sharedData->mTrackLocation << "\"," << nl;
	ss << tab << tab << "\"mTrackVariation\":\"" << sharedData->mTrackVariation << "\"," << nl;
	ss << tab << tab << "\"mTrackLength\":" << sharedData->mTrackLength << "," << nl;
	//pcars2 additional data
	ss << tab << tab << "\"mTranslatedTrackLocation\":\"" << sharedData->mTranslatedTrackLocation << "\"," << nl;
	ss << tab << tab << "\"mTranslatedTrackVariation\":\"" << sharedData->mTranslatedTrackVariation << "\"" << nl << tab << "}";
}

void renderTimings(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"timings\":{" << nl;
	ss << tab << tab << "\"mLapInvalidated\":" << (sharedData->mLapInvalidated ? "true" : "false") << "," << nl;
	ss << tab << tab << "\"mBestLapTime\":" << sharedData->mBestLapTime << "," << nl;
	ss << tab << tab << "\"mLastLapTime\":" << sharedData->mLastLapTime << "," << nl;
	ss << tab << tab << "\"mCurrentTime\":" << sharedData->mCurrentTime << "," << nl;
	ss << tab << tab << "\"mSplitTimeAhead\":" << sharedData->mSplitTimeAhead << "," << nl;
	ss << tab << tab << "\"mSplitTimeBehind\":" << sharedData->mSplitTimeBehind << "," << nl;
	ss << tab << tab << "\"mSplitTime\":" << sharedData->mSplitTime << "," << nl;
	ss << tab << tab << "\"mEventTimeRemaining\":" << sharedData->mEventTimeRemaining << "," << nl;
	ss << tab << tab << "\"mPersonalFastestLapTime\":" << sharedData->mPersonalFastestLapTime << "," << nl;
	ss << tab << tab << "\"mWorldFastestLapTime\":" << sharedData->mWorldFastestLapTime << "," << nl;
	ss << tab << tab << "\"mCurrentSector1Time\":" << sharedData->mCurrentSector1Time << "," << nl;
	ss << tab << tab << "\"mCurrentSector2Time\":" << sharedData->mCurrentSector2Time << "," << nl;
	ss << tab << tab << "\"mCurrentSector3Time\":" << sharedData->mCurrentSector3Time << "," << nl;
	ss << tab << tab << "\"mFastestSector1Time\":" << sharedData->mFastestSector1Time << "," << nl;
	ss << tab << tab << "\"mFastestSector2Time\":" << sharedData->mFastestSector2Time << "," << nl;
	ss << tab << tab << "\"mFastestSector3Time\":" << sharedData->mFastestSector3Time << "," << nl;
	ss << tab << tab << "\"mPersonalFastestSector1Time\":" << sharedData->mPersonalFastestSector1Time << "," << nl;
	ss << tab << tab << "\"mPersonalFastestSector2Time\":" << sharedData->mPersonalFastestSector2Time << "," << nl;
	ss << tab << tab << "\"mPersonalFastestSector3Time\":" << sharedData->mPersonalFastestSector3Time << "," << nl;
	ss << tab << tab << "\"mWorldFastestSector1Time\":" << sharedData->mWorldFastestSector1Time << "," << nl;
	ss << tab << tab << "\"mWorldFastestSector2Time\":" << sharedData->mWorldFastestSector2Time << "," << nl;
	ss << tab << tab << "\"mWorldFastestSector3Time\":" << sharedData->mWorldFastestSector3Time << nl << tab << "}";
}

void renderFlags(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"flags\":{" << nl;
	ss << tab << tab << "\"mHighestFlagColour\":" << sharedData->mHighestFlagColour << "," << nl;
	ss << tab << tab << "\"mHighestFlagReason\":" << sharedData->mHighestFlagReason << "," << nl;
	ss << tab << tab << "\"mYellowFlagState\":" << sharedData->mYellowFlagState << nl << tab << "}";
}

void renderPitInfo(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"pitInfo\":{" << nl;
	ss << tab << tab << "\"mPitMode\":" << sharedData->mPitMode << "," << nl;
	ss << tab << tab << "\"mPitSchedule\":" << sharedData->mPitSchedule << "," << nl;
	//pcars2 additional data
	ss << tab << tab << "\"mEnforcedPitStopLap\":" << sharedData->mEnforcedPitStopLap << nl << tab << "}";
}

void renderCarState(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"carState\":{" << nl;
	ss << tab << tab << "\"mCarFlags\":" << sharedData->mCarFlags << "," << nl;
	ss << tab << tab << "\"mOilTempCelsius\":" << sharedData->mOilTempCelsius << "," << nl;
	ss << tab << tab << "\"mWaterTempCelsius\":" << sharedData->mWaterTempCelsius << "," << nl;
	ss << tab << tab << "\"mWaterPressureKPa\":" << sharedData->mWaterPressureKPa << "," << nl;
	ss << tab << tab << "\"mFuelPressureKPa\":" << sharedData->mFuelPressureKPa << "," << nl;
	ss << tab << tab << "\"mFuelLevel\":" << sharedData->mFuelLevel << "," << nl;
	ss << tab << tab << "\"mFuelCapacity\":" << sharedData->mFuelCapacity << "," << nl;
	ss << tab << tab << "\"mSpeed\":" << sharedData->mSpeed << "," << nl;
	ss << tab << tab << "\"mRpm\":" << sharedData->mRpm << "," << nl;
	ss << tab << tab << "\"mMaxRPM\":" << sharedData->mMaxRPM << "," << nl;
	ss << tab << tab << "\"mBrake\":" << sharedData->mBrake << "," << nl;
	ss << tab << tab << "\"mThrottle\":" << sharedData->mThrottle << "," << nl;
	ss << tab << tab << "\"mClutch\":" << sharedData->mClutch << "," << nl;
	ss << tab << tab << "\"mSteering\":" << sharedData->mSteering << "," << nl;
	ss << tab << tab << "\"mGear\":" << sharedData->mGear << "," << nl;
	ss << tab << tab << "\"mNumGears\":" << sharedData->mNumGears << "," << nl;
	ss << tab << tab << "\"mOdometerKM\":" << sharedData->mOdometerKM << "," << nl;
	ss << tab << tab << "\"mAntiLockActive\":" << (sharedData->mAntiLockActive ? "true" : "false") << "," << nl;
	ss << tab << tab << "\"mLastOpponentCollisionIndex\":" << sharedData->mLastOpponentCollisionIndex << "," << nl;
	ss << tab << tab << "\"mLastOpponentCollisionMagnitude\":" << sharedData->mLastOpponentCollisionMagnitude << "," << nl;
	ss << tab << tab << "\"mBoostActive\":" << (sharedData->mBoostActive ? "true" : "false") << "," << nl;
	ss << tab << tab << "\"mBoostAmount\":" << sharedData->mBoostAmount << "," << nl;
	//pcars2 additional data
	ss << tab << tab << "\"mEngineSpeed\":" << sharedData->mEngineSpeed << "," << nl;
	ss << tab << tab << "\"mEngineTorque\":" << sharedData->mEngineTorque << "," << nl;
	ss << tab << tab << "\"mWings\":[" << sharedData->mWings[0] << "," << sharedData->mWings[1] << "]," << nl;
	ss << tab << tab << "\"mHandBrake\":" << sharedData->mHandBrake << "," << nl;
	ss << tab << tab << "\"mBrakeBias\":" << sharedData->mBrakeBias << "," << nl;
	ss << tab << tab << "\"mTurboBoostPressure\":" << sharedData->mTurboBoostPressure << "," << nl;
	ss << tab << tab << "\"mDrsState\":" << sharedData->mDrsState << "," << nl;	//AMS2 v 1.3.3.0 Shared Memory v10 additional data
	ss << tab << tab << "\"mAntiLockSetting\":" << sharedData->mAntiLockSetting << "," << nl;	//AMS2 v 1.3.4.0 Shared Memory v11 additional data
	ss << tab << tab << "\"mTractionControlSetting\":" << sharedData->mTractionControlSetting << "," << nl;	//AMS2 v 1.3.4.0 Shared Memory v11 additional data
	ss << tab << tab << "\"mErsDeploymentMode\":" << sharedData->mErsDeploymentMode << "," << nl;	//AMS2 v 1.3.5.2 Shared Memory v12 additional data
	ss << tab << tab << "\"mErsAutoModeEnabled\":" << sharedData->mErsAutoModeEnabled << "," << nl;	//AMS2 v 1.3.5.2 Shared Memory v12 additional data
	ss << tab << tab << "\"mClutchTemp\":" << sharedData->mClutchTemp << "," << nl;	//AMS2 v 1.3.5.2 Shared Memory v12 additional data
	ss << tab << tab << "\"mClutchWear\":" << sharedData->mClutchWear << "," << nl;	//AMS2 v 1.3.5.2 Shared Memory v12 additional data
	ss << tab << tab << "\"mClutchOverheated\":" << sharedData->mClutchOverheated << "," << nl;	//AMS2 v 1.3.5.2 Shared Memory v12 additional data
	ss << tab << tab << "\"mClutchSlipping\":" << sharedData->mClutchSlipping << nl << tab << "}";	//AMS2 v 1.3.5.2 Shared Memory v12 additional data
}

void renderMotionDeviceRelated(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"motionAndDeviceRelated\":{" << nl;
	ss << tab << tab << "\"mOrientation\":[" << sharedData->mOrientation[0] << "," << sharedData->mOrientation[1] << "," << sharedData->mOrientation[2] << "]," << nl;
	ss << tab << tab << "\"mLocalVelocity\":[" << sharedData->mLocalVelocity[0] << "," << sharedData->mLocalVelocity[1] << "," << sharedData->mLocalVelocity[2] << "]," << nl;
	ss << tab << tab << "\"mWorldVelocity\":[" << sharedData->mWorldVelocity[0] << "," << sharedData->mWorldVelocity[1] << "," << sharedData->mWorldVelocity[2] << "]," << nl;
	ss << tab << tab << "\"mAngularVelocity\":[" << sharedData->mAngularVelocity[0] << "," << sharedData->mAngularVelocity[1] << "," << sharedData->mAngularVelocity[2] << "]," << nl;
	ss << tab << tab << "\"mLocalAcceleration\":[" << sharedData->mLocalAcceleration[0] << "," << sharedData->mLocalAcceleration[1] << "," << sharedData->mLocalAcceleration[2] << "]," << nl;
	ss << tab << tab << "\"mWorldAcceleration\":[" << sharedData->mWorldAcceleration[0] << "," << sharedData->mWorldAcceleration[1] << "," << sharedData->mWorldAcceleration[2] << "]," << nl;
	ss << tab << tab << "\"mExtentsCentre\":[" << sharedData->mExtentsCentre[0] << "," << sharedData->mExtentsCentre[1] << "," << sharedData->mExtentsCentre[2] << "]" << nl << tab << "}";
}

void renderWheelsTyres(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"wheelsAndTyres\":{" << nl;
	ss << tab << tab << "\"mTyreFlags\":[" << sharedData->mTyreFlags[0] << "," << sharedData->mTyreFlags[1] << "," << sharedData->mTyreFlags[2] << "," << sharedData->mTyreFlags[3] << "]," << nl;
	ss << tab << tab << "\"mTerrain\":[" << sharedData->mTerrain[0] << "," << sharedData->mTerrain[1] << "," << sharedData->mTerrain[2] << "," << sharedData->mTerrain[3] << "]," << nl;
	ss << tab << tab << "\"mTyreY\":[" << sharedData->mTyreY[0] << "," << sharedData->mTyreY[1] << "," << sharedData->mTyreY[2] << "," << sharedData->mTyreY[3] << "]," << nl;
	ss << tab << tab << "\"mTyreRPS\":[" << sharedData->mTyreRPS[0] << "," << sharedData->mTyreRPS[1] << "," << sharedData->mTyreRPS[2] << "," << sharedData->mTyreRPS[3] << "]," << nl;
	ss << tab << tab << "\"mTyreSlipSpeed\":[" << sharedData->mTyreSlipSpeed[0] << "," << sharedData->mTyreSlipSpeed[1] << "," << sharedData->mTyreSlipSpeed[2] << "," << sharedData->mTyreSlipSpeed[3] << "]," << nl;
	ss << tab << tab << "\"mTyreTemp\":[" << sharedData->mTyreTemp[0] << "," << sharedData->mTyreTemp[1] << "," << sharedData->mTyreTemp[2] << "," << sharedData->mTyreTemp[3] << "]," << nl;
	ss << tab << tab << "\"mTyreTempLeft\":[" << sharedData->mTyreTempLeft[0] << "," << sharedData->mTyreTempLeft[1] << "," << sharedData->mTyreTempLeft[2] << "," << sharedData->mTyreTempLeft[3] << "]," << nl;	//AMS2 v 1.3.3.0 Shared Memory v10 additional data
	ss << tab << tab << "\"mTyreTempCenter\":[" << sharedData->mTyreTempCenter[0] << "," << sharedData->mTyreTempCenter[1] << "," << sharedData->mTyreTempCenter[2] << "," << sharedData->mTyreTempCenter[3] << "]," << nl;	//AMS2 v 1.3.3.0 Shared Memory v10 additional data
	ss << tab << tab << "\"mTyreTempRight\":[" << sharedData->mTyreTempRight[0] << "," << sharedData->mTyreTempRight[1] << "," << sharedData->mTyreTempRight[2] << "," << sharedData->mTyreTempRight[3] << "]," << nl;	//AMS2 v 1.3.3.0 Shared Memory v10 additional data
	ss << tab << tab << "\"mTyreGrip\":[" << sharedData->mTyreGrip[0] << "," << sharedData->mTyreGrip[1] << "," << sharedData->mTyreGrip[2] << "," << sharedData->mTyreGrip[3] << "]," << nl;
	ss << tab << tab << "\"mTyreHeightAboveGround\":[" << sharedData->mTyreHeightAboveGround[0] << "," << sharedData->mTyreHeightAboveGround[1] << "," << sharedData->mTyreHeightAboveGround[2] << "," << sharedData->mTyreHeightAboveGround[3] << "]," << nl;
	ss << tab << tab << "\"mTyreLateralStiffness\":[" << sharedData->mTyreLateralStiffness[0] << "," << sharedData->mTyreLateralStiffness[1] << "," << sharedData->mTyreLateralStiffness[2] << "," << sharedData->mTyreLateralStiffness[3] << "]," << nl;
	ss << tab << tab << "\"mTyreWear\":[" << sharedData->mTyreWear[0] << "," << sharedData->mTyreWear[1] << "," << sharedData->mTyreWear[2] << "," << sharedData->mTyreWear[3] << "]," << nl;
	ss << tab << tab << "\"mBrakeDamage\":[" << sharedData->mBrakeDamage[0] << "," << sharedData->mBrakeDamage[1] << "," << sharedData->mBrakeDamage[2] << "," << sharedData->mBrakeDamage[3] << "]," << nl;
	ss << tab << tab << "\"mSuspensionDamage\":[" << sharedData->mSuspensionDamage[0] << "," << sharedData->mSuspensionDamage[1] << "," << sharedData->mSuspensionDamage[2] << "," << sharedData->mSuspensionDamage[3] << "]," << nl;
	ss << tab << tab << "\"mBrakeTempCelsius\":[" << sharedData->mBrakeTempCelsius[0] << "," << sharedData->mBrakeTempCelsius[1] << "," << sharedData->mBrakeTempCelsius[2] << "," << sharedData->mBrakeTempCelsius[3] << "]," << nl;
	ss << tab << tab << "\"mTyreTreadTemp\":[" << sharedData->mTyreTreadTemp[0] << "," << sharedData->mTyreTreadTemp[1] << "," << sharedData->mTyreTreadTemp[2] << "," << sharedData->mTyreTreadTemp[3] << "]," << nl;
	ss << tab << tab << "\"mTyreLayerTemp\":[" << sharedData->mTyreLayerTemp[0] << "," << sharedData->mTyreLayerTemp[1] << "," << sharedData->mTyreLayerTemp[2] << "," << sharedData->mTyreLayerTemp[3] << "]," << nl;
	ss << tab << tab << "\"mTyreCarcassTemp\":[" << sharedData->mTyreCarcassTemp[0] << "," << sharedData->mTyreCarcassTemp[1] << "," << sharedData->mTyreCarcassTemp[2] << "," << sharedData->mTyreCarcassTemp[3] << "]," << nl;
	ss << tab << tab << "\"mTyreRimTemp\":[" << sharedData->mTyreRimTemp[0] << "," << sharedData->mTyreRimTemp[1] << "," << sharedData->mTyreRimTemp[2] << "," << sharedData->mTyreRimTemp[3] << "]," << nl;
	ss << tab << tab << "\"mTyreInternalAirTemp\":[" << sharedData->mTyreInternalAirTemp[0] << "," << sharedData->mTyreInternalAirTemp[1] << "," << sharedData->mTyreInternalAirTemp[2] << "," << sharedData->mTyreInternalAirTemp[3] << "]," << nl;
	//pcars2 additional data
	ss << tab << tab << "\"mWheelLocalPositionY\":[" << sharedData->mWheelLocalPositionY[0] << "," << sharedData->mWheelLocalPositionY[1] << "," << sharedData->mWheelLocalPositionY[2] << "," << sharedData->mWheelLocalPositionY[3] << "]," << nl;
	ss << tab << tab << "\"mSuspensionTravel\":[" << sharedData->mSuspensionTravel[0] << "," << sharedData->mSuspensionTravel[1] << "," << sharedData->mSuspensionTravel[2] << "," << sharedData->mSuspensionTravel[3] << "]," << nl;
	ss << tab << tab << "\"mSuspensionVelocity\":[" << sharedData->mSuspensionVelocity[0] << "," << sharedData->mSuspensionVelocity[1] << "," << sharedData->mSuspensionVelocity[2] << "," << sharedData->mSuspensionVelocity[3] << "]," << nl;
	ss << tab << tab << "\"mRideHeight\":[" << sharedData->mRideHeight[0] << "," << sharedData->mRideHeight[1] << "," << sharedData->mRideHeight[2] << "," << sharedData->mRideHeight[3] << "]," << nl;	//AMS2 v 1.3.3.0 Shared Memory v10 additional data
	ss << tab << tab << "\"mAirPressure\":[" << sharedData->mAirPressure[0] << "," << sharedData->mAirPressure[1] << "," << sharedData->mAirPressure[2] << "," << sharedData->mAirPressure[3] << "]," << nl;
	ss << tab << tab << "\"mTyreCompound\":[\"" << sharedData->mTyreCompound[0] << "\",\"" << sharedData->mTyreCompound[1] << "\",\"" << sharedData->mTyreCompound[2] << "\",\"" << sharedData->mTyreCompound[3] << "\"]" << nl << tab << "}";

}

void renderCarDamage(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"carDamage\":{" << nl;
	ss << tab << tab << "\"mCrashState\":" << sharedData->mCrashState << "," << nl;
	ss << tab << tab << "\"mAeroDamage\":" << sharedData->mAeroDamage << "," << nl;
	ss << tab << tab << "\"mEngineDamage\":" << sharedData->mEngineDamage << nl << tab << "}";
}

void renderWeather(std::stringstream& ss, const SharedMemory* sharedData) {
	ss << tab << "\"weather\":{" << nl;
	ss << tab << tab << "\"mAmbientTemperature\":" << sharedData->mAmbientTemperature << "," << nl;
	ss << tab << tab << "\"mTrackTemperature\":" << sharedData->mTrackTemperature << "," << nl;
	ss << tab << tab << "\"mRainDensity\":" << sharedData->mRainDensity << "," << nl;
	ss << tab << tab << "\"mWindSpeed\":" << sharedData->mWindSpeed << "," << nl;
	ss << tab << tab << "\"mWindDirectionX\":" << sharedData->mWindDirectionX << "," << nl;
	ss << tab << tab << "\"mWindDirectionY\":" << sharedData->mWindDirectionY << "," << nl;
	ss << tab << tab << "\"mCloudBrightness\":" << sharedData->mCloudBrightness << "," << nl;
	//pcars2 additional data
	ss << tab << tab << "\"mSnowDensity\":" << sharedData->mSnowDensity << nl << tab << "}";
}

// Adds a comma, unless skipped
void addSeparator(std::stringstream& ss, bool skip) {
	if (!skip) {
		ss << "," << nl;
	}
}

// Returns true if the given section should be rendered, based on the presence
// of the sections name parameter in the query string
bool shouldRender(std::string queryString, std::string sectionName) {
	std::stringstream ss;
	ss << sectionName << "=true";
	//render the section if the URL parameter list is empty or includes "[sectionName]=true"
	return queryString.empty() || Utils::contains(queryString, ss.str());
}

std::string SharedMemoryRenderer::render(const SharedMemory* sharedData, std::string queryString) {

	std::stringstream ss;

	std::regex target("formatted=true");
	std::string replacement = "";

	//Get current system UTC time for adding a timestamp to the data
	struct timespec ts;
	if (timespec_get(&ts, TIME_UTC) != TIME_UTC)
	{
		printf("timespec_get failed!\n");
	}
	uint64_t cur_time = 1000000000 * ts.tv_sec + ts.tv_nsec;
	uint64_t cur_time_ms = (uint64_t)round((double)cur_time / 1000000);	// cast to double first for correct rounding and cast to uint64_t back
	//printf("cur Time: %llu , cur Time ms: %llu\n", cur_time, cur_time_ms);

	// if the URL parameter includes "formatted=true" then generate a formatted browser output
	if (Utils::contains(queryString, "formatted=true")) {
		tab = TAB;
		nl = NEW_LINE;

		// Remove "formatted=true" from queryString, because the renderer generates all only if the the queryString is empty. And if "formatted=true" is the only parameter, it should render all and then must be empty.
		queryString = std::regex_replace(queryString, target, replacement);
	}
	else {
		// Tabulator and new line empty for unformatted output, which reduces the overhead in the data
		tab = "";
		nl = "";
	}

	ss << "{" << nl;
	bool skipSeparator = true;
	if (shouldRender(queryString, "buildInfo")) {
		renderBuildInfo(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "gameStates")) {
		addSeparator(ss, skipSeparator);
		renderGameStates(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "participants")) {
		addSeparator(ss, skipSeparator);
		renderParticipants(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "unfilteredInputs")) {
		addSeparator(ss, skipSeparator);
		renderUnfilteredInput(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "vehicleInformation")) {
		addSeparator(ss, skipSeparator);
		renderVehicleInformation(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "eventInformation")) {
		addSeparator(ss, skipSeparator);
		renderEventInformation(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "timings")) {
		addSeparator(ss, skipSeparator);
		renderTimings(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "flags")) {
		addSeparator(ss, skipSeparator);
		renderFlags(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "pitInfo")) {
		addSeparator(ss, skipSeparator);
		renderPitInfo(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "carState")) {
		addSeparator(ss, skipSeparator);
		renderCarState(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "motionDeviceRelated")) {
		addSeparator(ss, skipSeparator);
		renderMotionDeviceRelated(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "wheelsAndTyres")) {
		addSeparator(ss, skipSeparator);
		renderWheelsTyres(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "carDamage")) {
		addSeparator(ss, skipSeparator);
		renderCarDamage(ss, sharedData);
		skipSeparator = false;
	}
	if (shouldRender(queryString, "weather")) {
		addSeparator(ss, skipSeparator);
		renderWeather(ss, sharedData);
		skipSeparator = false;
	}
	//ss << "," << nl << tab << "\"timestamp\":" << cur_time / 100000; // curtime in milliseconds + 1 digit - for milliseconds divide it by 10 (division of integers truncates the value instead of rounding, in result 1 digit more for precision)
	ss << "," << nl << tab << "\"timestamp\":" << cur_time_ms;
	ss << nl << "}";

	return ss.str();
}