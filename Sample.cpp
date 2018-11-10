#include "Sample.h"
#include <ctime>
#include <iostream>
#include "FeatureFactory.h"

Sample::Sample(Eigen::MatrixXf *dataset, Eigen::VectorXi *labels, 
			   Eigen::MatrixXi *indexMat, Eigen::MatrixXf *distMat, int numClass, int numFeature):
	_dataset(dataset),
	_labels(labels),
	_indexMat(indexMat),
	_distMat(distMat),
	_numClass(numClass),
	_numFeature(numFeature)
{ 
}


Sample::Sample(Sample* sample):
	_dataset(sample->_dataset),
	_labels(sample->_labels),
	_indexMat(sample->_indexMat),
	_distMat(sample->_distMat),
	_numClass(sample->_numClass),
	_numFeature(sample->_numFeature),
	_numSelectedSamples(sample->getNumSelectedSamples()),
	_selectedSamplesId(sample->getSelectedSamplesId())
{ 
}

Sample::Sample(const Sample* sample, Eigen::VectorXi &samplesId) :
	_dataset(sample->_dataset),
	_labels(sample->_labels),
	_indexMat(sample->_indexMat),
	_distMat(sample->_distMat),
	_numClass(sample->_numClass),
	_numFeature(sample->_numFeature),
	_selectedSamplesId(samplesId),
	_numSelectedSamples(samplesId.rows())
{
}

void Sample::randomSampleDataset(Eigen::VectorXi &selectedSamplesId, int numSelectedSamples)
{
	int numTotalSamples = _dataset->rows();
	_numSelectedSamples = numSelectedSamples;

	// to generate a uniform distribution and sample with replacement
	std::default_random_engine generator;
	generator.seed(time(NULL));
	std::uniform_int_distribution<int> distribution(0, numTotalSamples - 1);

	for (int i = 0; i < _numSelectedSamples; ++i)
	{
		selectedSamplesId[i] = distribution(generator);
		//selectedSamplesId[i] = rand() % numTotalSamples;
	}

	_selectedSamplesId = selectedSamplesId;
}

void Sample::randomSampleFeatures()
{
	// total population
	int numPoints = _indexMat->cols();
	for (int i = 0; i < _numFeature; ++i)
	{
		Features feature;
		// number of voxels in this feature
		feature._numVoxels = randomFrom124();
		Random rd(numPoints, feature._numVoxels);
		feature._pointId = rd.sampleWithoutReplacement();
		// for each voxel, randomly choose the number of points in each voxel which is
		// in the range (0.1*numOfPointsInNeighborhood, 0.5*numOfPointsInNeighborhood)
		for (int i = 0; i < feature._numVoxels; ++i)
		{
			Random rd(feature.maxSamplesPerVoxel - feature.minSamplesPerVoxel, 1);
			int tmp = rd.sampleWithoutReplacement().at(0) + feature.minSamplesPerVoxel;
			feature._voxelSize.push_back(tmp);
		}
		// randomly select a projection type from all possible projections
		Random rd2(FeatureFactory::numOfPossibleProjections, 1);
		feature._featType = rd2.sampleWithoutReplacement().at(0);
		_features.push_back(feature);
	}
}

int Sample::randomFrom124()
{
	int arr[] = { 1, 2, 4 };
	Random rd(3, 1);
	int inx = rd.sampleWithoutReplacement().at(0);
	return arr[inx];
}

Eigen::MatrixXf Sample::buildNeighborhood(int pointId) const
{
	// number of points in the neighborhood
	int k = _indexMat->cols();
	// datapoint dimension
	int d = _dataset->cols();
	/*std::cout << "pointID\n";
	std::cout << pointId << std::endl;*/
	Eigen::MatrixXf neighborhood(k, d);
	Eigen::VectorXi candidatePointIndices;
	candidatePointIndices = _indexMat->row(pointId);
	// std::cout << "candidates\n";
	// std::cout << candidatePointIndices << std::endl;
	//neighborhood.row(0) = _dataset.row(pointId);
	for (int i = 0; i < k; ++i) {
		neighborhood.row(i) = _dataset->row(candidatePointIndices[i]);
	}
	return neighborhood;
}
