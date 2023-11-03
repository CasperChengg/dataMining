#include "./header_files/decision_tree_classifier.h"
#include "./header_files/quick_sort.h"
#include<iostream>
#include<vector>
#include<map>
// private structure-----------------------------------------------------------------------------------------------------------
// a split point is X <= V, use this structure to store a split point would be more clear
// typedef struct SplitPoint
// {
//     int feature;
//     float value;
//     float score;
// }SplitPoint;

// node in a tree
// typedef struct TreeNode
// {
//     int isLeaf;
//     int Label;
//     SplitPoint splitPoint;
//     struct TreeNode *rightChild;
//     struct TreeNode *leftChild;
// }TreeNode;
// private parameter-----------------------------------------------------------------------------------------------------------
// structure that used to evaluate the total score of each feature, in order to find the q-best features
// std::map<int, float> featuresScore;
// private functions-----------------------------------------------------------------------------------------------------------
float DecisionTreeClassifier::calGini(int* partition1, int* partition2)
{   
    int sizeOfP1 = 0, sizeOfP2 = 0;
    float giniOfP1 = 1.0, giniOfP2 = 1.0;

    // calculate the gini index of partition1
    for(int i = 0; i < numOfClasses; i++)
    {
        sizeOfP1 += partition1[i];
    }
    for(int i = 0; i < numOfClasses; i++)
    {
        giniOfP1 -= ((float)partition1[i]/sizeOfP1) * ((float)partition1[i]/sizeOfP1);
    }

    // calculate the gini index of partition2
    for(int i = 0; i < numOfClasses; i++)
    {
        sizeOfP2 +=  partition2[i];
    }

    for(int i = 0; i < numOfClasses; i++)
    {
        giniOfP2 -= ((float)partition2[i]/sizeOfP2) * ((float)partition2[i]/sizeOfP2);
    }

    // return weighted gini index of partition1 & partition2
    return((float)sizeOfP1 / (sizeOfP1 + sizeOfP2)) * giniOfP1 + ((float)sizeOfP2 / (sizeOfP1 + sizeOfP2)) * giniOfP2;
}
SplitPoint DecisionTreeClassifier::evaluateSplitPoint(std::vector<int> data_id, int feature_id)
{
    int numOfData = data_id.size();
    int *sizeOfClassesInD_Y = (int*)malloc(numOfClasses * sizeof(int));
    int *sizeOfClassesInD_N = (int*)malloc(numOfClasses * sizeof(int));
    int classOfPoint = 0;
    float mid = 0.0, score = 0.0;
    SplitPoint best;
    best.score = 1.1; best.value = 0; best.feature = feature_id;

    // used for extract specific feature of each point
    float *featureOfEachPoint = (float*)malloc(numOfData * sizeof(float));
    for(int i = 0; i < numOfData; i++)
    {
        featureOfEachPoint[i] = trainingSet[data_id[i]][feature_id];
    }
    // sort the extracted feature array
    quickSort(featureOfEachPoint, 0, numOfData - 1);
    
    // try all the possible split point in specfic feature and find out the best one
    for(int i = 1; i < numOfData; i++)
    {
        if(featureOfEachPoint[i] != featureOfEachPoint[i-1])
        {
            // midpoint of any two successive points could represent all the split points between these two
            mid = (featureOfEachPoint[i] + featureOfEachPoint[i-1]) / 2;
            
            // initialize the sizeOfClasses array every iteration
            for(int j = 0; j < numOfClasses; j++)
            {
                sizeOfClassesInD_Y[j] = 0;
                sizeOfClassesInD_N[j] = 0;
            }

            //Analyze the distribution of two partitions
            for(int j = 0; j < numOfData; j++)
            {
                classOfPoint = trainingSet[data_id[j]][classLocation];
                if(trainingSet[data_id[j]][feature_id] <= mid)
                {
                    sizeOfClassesInD_Y[classOfPoint]++;
                }
                else{
                    sizeOfClassesInD_N[classOfPoint]++;
                }
            }

            // calculate the gini index for a split point
            score = calGini(sizeOfClassesInD_Y, sizeOfClassesInD_N);
            // store the best one
            if(score < best.score)
            {
                best.score = score;
                best.value = mid;
            }
        }
    }
    free(sizeOfClassesInD_N);
    free(sizeOfClassesInD_Y);
    free(featureOfEachPoint);
    return best;
}
int DecisionTreeClassifier::decisionTree(TreeNode *node, std::vector<int> data_id)
{   
    std::cout<<"new node"<< " ";
    for(int i = 0; i < data_id.size(); i++)
    {
        std::cout << data_id[i] << " ";
    }
    std::cout<<std::endl;
    // for calculating purity
    int majorClassSize = 0, majorClass = 0;
    int numOfData = data_id.size();

    int classOfThisPoint = 0;

    // initializa best split point
    SplitPoint best, splitPoint;
    best.feature = 0; best.value = 0; best.score = 1;
    splitPoint.feature = 0; splitPoint.value = 0; splitPoint.score = 0;

    // initializa data of leftchild and right child 
    std::vector<int> D_Y, D_N;
    int *sizeOfClasses = (int*)calloc(numOfClasses, sizeof(int));
    if(sizeOfClasses == NULL)
    {
        printf("dynamic memory allocation error\n");
        return 1;
    }

    // calculate the number of each class and find the maximum one
    for(int i = 0; i < numOfData; i++)
    {
        classOfThisPoint = trainingSet[data_id[i]][classLocation];
        sizeOfClasses[classOfThisPoint]++;
        if(sizeOfClasses[classOfThisPoint] > majorClassSize)
        {
            majorClassSize = sizeOfClasses[classOfThisPoint];
            majorClass = classOfThisPoint;
        }
    }
    free(sizeOfClasses);

    std::cout << "numOfData = " << numOfData << " purity = " << (float)majorClassSize / numOfData << std::endl;
    // check the stopping condition
    if(numOfData <= smallestSizeOfANode || (float)majorClassSize >= purity * numOfData)
    {
        node->isLeaf = 1;
        node->Label = majorClass;
        std::cout << "is leaf with class " << node->Label <<std::endl;
        std::cout << "--------------------------------------------------------------------------"<< std::endl;
        return 0;
    }
    
    // find the best split point
    for(int i = 0; i < numOfSeclectedFeatures; i++)
    {
        splitPoint = evaluateSplitPoint(data_id, seclectedFeatures[i]);
        if(splitPoint.score < best.score)
        {
            best.score = splitPoint.score;
            best.value = splitPoint.value;
            best.feature = splitPoint.feature;
        }
    }
    std::cout << "best split = " << best.feature << " <= " << best.value << " , " << best.score<< std::endl;

    //sum up the score of each internal node
    if(featuresScore.count(best.feature) > 0) 
    {
        featuresScore[best.feature] += best.score;
    }
    else
    {
        featuresScore[best.feature] = best.score;
    }
    

    // partition data with best split point
    for(int i = 0; i < numOfData; i++)
    {
        if(trainingSet[data_id[i]][best.feature] <= best.value)
        {
            D_Y.push_back(data_id[i]);
        }
        else
        {
            D_N.push_back(data_id[i]);
        }
    }
    std::cout << "--------------------------------------------------------------------------"<< std::endl;
    if(D_Y.size() > 0)
    {
        node->leftChild = (TreeNode*)malloc(sizeof(TreeNode));
        if(node->leftChild == NULL)
        {
            printf("dynamic memory allocation error\n");
            return 1;
        }
        node->leftChild->leftChild = NULL;
        node->leftChild->rightChild = NULL;
        if(decisionTree(node->leftChild, D_Y) == 1)
        {
            return 1;
        }
    }
    if(D_N.size() > 0)
    {
        node->rightChild = (TreeNode*)malloc(sizeof(TreeNode));
        if(node->rightChild == NULL)
        {
            printf("dynamic memory allocation error\n");
            return 1;
        }
        node->rightChild->leftChild = NULL;
        node->rightChild->rightChild = NULL;
        if(decisionTree(node->rightChild, D_N) == 1)
        {
            return 1;
        }
    }
    return 0;
}
int DecisionTreeClassifier::rightChild(std::vector<std::pair<int, float>> *heap, int index)
{
    if(index > heap->size() || (2 * index + 1) > heap->size())
    {
        return 0;
    }
    else
    {
        return 2 * index + 1;
    }
}
int DecisionTreeClassifier::leftChild(std::vector<std::pair<int, float>> *heap, int index)
{
    if(index > heap->size() || (2 * index) > heap->size())
    {
        return 0;
    }
    else
    {
        return 2 * index;
    }
}
void DecisionTreeClassifier::maxHeapify(std::vector<std::pair<int, float>> *heap, int index)
{
    int rChild, lChild;
    if (index > heap->size())
    {
        return;
    }
    rChild = rightChild(heap, index);
    lChild = leftChild(heap, index);
    if(lChild == 0)
    {
        return;
    }
    if(rChild > 0)
    {
        if((*heap)[lChild - 1].second <= (*heap)[rChild - 1].second)
        {
            if((*heap)[index - 1].second < (*heap)[rChild - 1].second)
            {
                std::swap((*heap)[index - 1], (*heap)[rChild - 1]);
                maxHeapify(heap, rChild);
            }
        }
        else{
            if((*heap)[index - 1].second < (*heap)[lChild- 1].second)
            {
                std::swap((*heap)[index - 1], (*heap)[lChild - 1]);
                maxHeapify(heap, lChild);
            }
        }
    }
    else{
        if((*heap)[index - 1].second < (*heap)[lChild- 1].second)
        {
            std::swap((*heap)[index - 1], (*heap)[lChild - 1]);
        }
    } 
}
void DecisionTreeClassifier::createMaxHeap(std::vector<std::pair<int, float>> *heap)
{
    int sizeOfHeap = heap->size();
    for(int i = sizeOfHeap / 2; i > 0; i--)
    {
        maxHeapify(heap, i);
    }
}
void DecisionTreeClassifier::updateMaxHeap(std::vector<std::pair<int, float>> *heap, std::pair<int, float> newElement)
        {
            if(newElement.second < (*heap)[0].second)
            {
                (*heap)[0] = newElement;
                maxHeapify(heap, 1);
            }
        }

// public parameters-----------------------------------------------------------------------------------------------------------
// float **trainingSet = NULL;
// int sizeOfTrainingSet = 0;
// int *seclectedFeatures = NULL;
// int numOfSeclectedFeatures = 0;
// int classLocation = -1;
// int numOfClasses = 0;
// int smallestSizeOfANode = -1;
// float purity = -1.0;

// public functions-----------------------------------------------------------------------------------------------------------
int DecisionTreeClassifier::fit(void)
{
    TreeNode *root = (TreeNode*)malloc(sizeof(TreeNode));
    std::vector<int> data_id;
    if(root == NULL)
    {
        std::cout << "dynamic allocation error" << std::endl;
        return 1;
    }
    root->leftChild = NULL;
    root->rightChild = NULL;

    // check if the user has completely set the parameters
    if(trainingSet == NULL)
    {
        std::cout << "Please set the pointer to trainingSet first" << std::endl;
        return 1;
    }
    if(sizeOfTrainingSet == 0)
    {
        std::cout << "Please set the sizeOfTrainingSet first" << std::endl;
        return 1;
    }
    if(seclectedFeatures == NULL)
    {
        std::cout << "Please set the pointer to seclectedFeatures first" << std::endl;
        return 1;
    }
    if(numOfSeclectedFeatures == 0)
    {
        std::cout << "Please set the numOfSeclectedFeatures first" << std::endl;
        return 1;
    }
    if(classLocation == -1)
    {
        std::cout << "Please set the classLocation first" << std::endl;
        return 1;
    }
    if(numOfClasses == 0)
    {
        std::cout << "Please set the numOfClasses first" << std::endl;
        return 1;
    }
    if(smallestSizeOfANode == -1)
    {
        std::cout << "Please set the smallestSizeOfANode first" << std::endl;
        return 1;
    }
    if(purity == -1.0)
    {
        std::cout << "Please set the purity first" << std::endl;
        return 1;
    }

    // initialize the data_id vector
    for(int i = 0; i < sizeOfTrainingSet; i++)
    {
        data_id.push_back(i);
    }

    // start
    return decisionTree(root, data_id);
}
void DecisionTreeClassifier::getQBestFeature(int q, int *qBestFeatures)
{
    std::vector<std::pair<int, float>> heap;
    int counter = 0;
    for(auto it = featuresScore.begin(); it != featuresScore.end(); it++)
    {
        if(counter < q)
        {
            heap.push_back(*it);
            counter++;
            if(counter == q)
            {
                createMaxHeap(&heap);
            }
        }
        else
        {
            updateMaxHeap(&heap, *it);
        }
    }
    for(counter = 0 ; counter < heap.size(); counter++)
    {
        qBestFeatures[counter] = heap[counter].first;
    }
    if(counter < q)
    {
        qBestFeatures[counter] = -1;
    }
}

