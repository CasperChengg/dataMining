#include<iostream>
#include<vector>
#include<map>
// defined structures
typedef struct SplitPoint
{
    int feature;
    float value;
    float score;
}SplitPoint;
typedef struct TreeNode
{
    int isLeaf;
    int Label;
    SplitPoint splitPoint;
    struct TreeNode *rightChild;
    struct TreeNode *leftChild;
}TreeNode;   
#pragma once
class DecisionTreeClassifier
{
    private: 
        // defined functions
        float calGini(int* partition1, int* partition2);
        SplitPoint evaluateSplitPoint(std::vector<int> data_id, int feature_id);
        int decisionTree(TreeNode *node, std::vector<int> data_id);
        int rightChild(std::vector<std::pair<int, float>> *heap, int index);
        int leftChild(std::vector<std::pair<int, float>> *heap, int index);
        void maxHeapify(std::vector<std::pair<int, float>> *heap, int index);
        void createMaxHeap(std::vector<std::pair<int, float>> *heap);
        void updateMaxHeap(std::vector<std::pair<int, float>> *heap, std::pair<int, float> newElement);
        
        // defined private variable
        std::map<int, float> featuresScore;
    public:
        // defined public variables
        float **trainingSet;
        int sizeOfTrainingSet;
        int *seclectedFeatures;
        int numOfSeclectedFeatures;
        int classLocation;
        int numOfClasses;
        int smallestSizeOfANode;
        float purity;

        //defined public functions
        int fit(void);
        void getQBestFeature(int q, int *qBestFeatures);
};