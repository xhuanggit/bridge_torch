/* bridge_torch_main.c */
/* Author: Xiaolong Huang */

#include <iostream>
#include <string>
#include "yaml-cpp/yaml.h"

using namespace std;

// calculate cross time on one bridge, speed inverse already sorted from small to large
double calc_time_one_bridge(vector<double>& speedInv, int speedLen, int bridgeLen) {
	double speedInvTot = 0;
	int k;

	// two ways for k, k+1 to cross and bring back torch: t0*2+tk+t(k+1) or t0+2t1+t(k+1) 
	// to compare: t0+tk vs 2t1
	for (k=speedLen-2; k>1; k=k-2) {
		if ((speedInv[0]+speedInv[k])>2*speedInv[1]) {
			speedInvTot += speedInv[0]+speedInv[1]*2+speedInv[k+1];
		} else {
			speedInvTot += speedInv[0]*2+speedInv[k]+speedInv[k+1];
		}
	}

	// hikers left
	if (k==0) {
		// 2 hikers left
		speedInvTot += speedInv[1];
	} else if (k==1) {
		// 3 hikers left
		speedInvTot += speedInv[0]+speedInv[1]+speedInv[2];
	}

	// 1 hikers in total
	if (speedLen==1) {
		speedInvTot = speedInv[0];
	}

	return bridgeLen*speedInvTot;
}

int main(int argc, char *argv[]) {

	// default input file name
	string inputFileName = "input/input.yaml";
	// can add suffix to input file, e.g. input/inputxhyb.yaml
	string inputFileSuffix;

	if (argc>1) {
		inputFileSuffix = argv[1];
		inputFileName.insert(11,inputFileSuffix);
	}
	// load input
	YAML::Node Input = YAML::LoadFile(inputFileName);

	// check number of hikers and bridges
	if (Input["hikers"].size()==0) {
		cout << "No hiker." << endl;
		return -1;
	}
	if (Input["bridgeLengths"].size()==0) {
		cout << "No bridge to cross." << endl;
		return -1;
	}

	// process input
	YAML::const_iterator hikerIter;
	int numBridges = Input["bridgeLengths"].size();
	vector<int> speedCountPerBridge(numBridges,0);


	for (hikerIter=Input["hikers"].begin(); hikerIter!=Input["hikers"].end(); hikerIter++) {
		// check input hikers
		try {
			int startBridge = (*hikerIter)["startBridge"].as<int>();
			if (startBridge<0) {
				throw "start bridge negative";
			} else if (startBridge>=numBridges){
				throw "start bridge too large out of range";
			} else {
				// accumulate speed count
				speedCountPerBridge[startBridge]++;
			}
			if ((*hikerIter)["speed"].as<double>()<0) {
				throw "speed negative";
			}
		} catch (const char *msg) {
			cout << "Input error! at hiker " << (*hikerIter)["name"].as<string>() << ": " << msg << endl;
			return -1;
		}
	}

	vector<double> bridgeLengths = Input["bridgeLengths"].as<vector<double>>();
	for (int i=0; i<numBridges; i++) {
		// check input bridge lengths
		try {
			if (bridgeLengths[i]<0) {
				throw "length negative";
			}
		} catch (const char *msg) {
			cout << "Input error! at bridge " << i << ": " << msg << endl;
			return -1;
		}
	}

	// save hiker speed in one vector
	vector<int> speedAccumCounts(numBridges,0);
	speedAccumCounts[0] = speedCountPerBridge[0];
	for (int i=1; i<numBridges; i++) {
		speedAccumCounts[i] = speedAccumCounts[i-1]+speedCountPerBridge[i];
	}

	int numHikers = Input["hikers"].size();
	vector<double> speedInv(numHikers,0);
	// use multiple iterators for hikers starting at different bridges
	vector<vector<double>::iterator> speedInvIters(numBridges,speedInv.begin());
	for (int i=1; i<numBridges; i++) {
		speedInvIters[i] += speedAccumCounts[i-1];
	}
	for (hikerIter=Input["hikers"].begin(); hikerIter!=Input["hikers"].end(); hikerIter++) {
		int startBridge = (*hikerIter)["startBridge"].as<int>();
		*speedInvIters[startBridge] = 1.0/(*hikerIter)["speed"].as<double>();
		speedInvIters[startBridge]++;
	}
	
	// calculate time
	double timeTot = 0;
	vector<double>::iterator speedInvIter=speedInv.begin();
	for (int i=0; i<numBridges; i++) {
		// sort hiker speeds at current bridge
		sort(speedInvIter, speedInvIter+speedCountPerBridge[i]);
		// merge with speeds at previous bridge
		inplace_merge(speedInv.begin(), speedInvIter, speedInv.begin()+speedAccumCounts[i]);
		// add to total time
		timeTot += calc_time_one_bridge(speedInv, speedAccumCounts[i], bridgeLengths[i]);
		speedInvIter += speedCountPerBridge[i];
	}
	
	cout << "Total time to cross all bridges: " << timeTot << " minutes." << endl;

	return 0;
}
