#include "./header_files/decision_tree_classifier.h"
#include "./header_files/quick_sort.h"
#include<iostream>
#include<vector>
#include<map>
class DecisionTreeClassifier
{
    private:
        // a split point is X <= V, use this structure to store a split point would be more clear
        typedef struct SplitPoint
        {
            int feature;
            float value;
            float score;
        }SplitPoint;

        // node in a tree
        typedef struct TreeNode
        {
            int isLeaf;
            int Label;
            SplitPoint splitPoint;
            struct TreeNode *rightChild;
            struct TreeNode *leftChild;
        }TreeNode;

        float **X_train, pi;
        int n, c, numOfFeatures, eta, classLoc;
        int *featuresList;
        std::map<int, float> featuresScore;
        // structure that used to evaluate the total score of each feature, in order to find the q-best features
        
        float calGini(int* partition1, int* partition2)
        {   
            int sizeOfP1 = 0, sizeOfP2 = 0;
            float giniOfP1 = 1.0, giniOfP2 = 1.0;

            // calculate the gini index of partition1
            for(int i = 0; i < c; i++)
            {
                sizeOfP1 += partition1[i];
            }
            for(int i = 0; i < c; i++)
            {
                giniOfP1 -= ((float)partition1[i]/sizeOfP1) * ((float)partition1[i]/sizeOfP1);
            }

            // calculate the gini index of partition2
            for(int i = 0; i < c; i++)
            {
                sizeOfP2 +=  partition2[i];
            }

            for(int i = 0; i < c; i++)
            {
                giniOfP2 -= ((float)partition2[i]/sizeOfP2) * ((float)partition2[i]/sizeOfP2);
            }

            // return weighted gini index of partition1 & partition2
            return((float)sizeOfP1 / (sizeOfP1 + sizeOfP2)) * giniOfP1 + ((float)sizeOfP2 / (sizeOfP1 + sizeOfP2)) * giniOfP2;
        }
        SplitPoint evaluateSplitPoint(std::vector<int> data, int feature_id)
        {
            int numOfData = data.size();
            int *sizeOfClassesInD_Y = (int*)malloc(c * sizeof(int));
            int *sizeOfClassesInD_N = (int*)malloc(c * sizeof(int));
            int classOfPoint = 0;
            float mid = 0.0, score = 0.0;
            SplitPoint best;
            best.score = 1; best.value = 0; best.feature = feature_id;

            // used for extract specific feature of each point
            float *featureOfEachPoint = (float*)malloc(numOfData * sizeof(float));
            for(int i = 0; i < numOfData; i++)
            {
                featureOfEachPoint[i] = X_train[data[i]][feature_id];
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
                    for(int j = 0; j < c; j++)
                    {
                        sizeOfClassesInD_Y[j] = 0;
                        sizeOfClassesInD_N[j] = 0;
                    }

                    //Analyze the distribution of two partitions
                    for(int j = 0; j < numOfData; j++)
                    {
                        classOfPoint = X_train[data[j]][classLoc];
                        if(X_train[data[j]][feature_id] <= mid)
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
        int decisionTree(TreeNode *node, std::vector<int> data_id)
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
            int *sizeOfClasses = (int*)calloc(c, sizeof(int));
            if(sizeOfClasses == NULL)
            {
                printf("dynamic memory allocation error\n");
                return 1;
            }
        
            // calculate the number of each class and find the maximum one
            for(int i = 0; i < numOfData; i++)
            {
                classOfThisPoint = X_train[data_id[i]][classLoc];
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
            if(numOfData <= eta || (float)majorClassSize >= pi * numOfData)
            {
                node->isLeaf = 1;
                node->Label = majorClass;
                std::cout << "is leaf with class " << node->Label <<std::endl;
                return 0;
            }
            
            // find the best split point
            for(int i = 0; i < numOfFeatures; i++)
            {
                std::cout<< i << std::endl;
                splitPoint = evaluateSplitPoint(data_id, featuresList[i]);
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
                std::cout << "update key " << best.feature << std::endl << std::endl;
            }
            else
            {
                featuresScore[best.feature] = best.score;
                std::cout << "insert key " << best.feature << std::endl << std::endl;
            }
            

            // partition data with best split point
            for(int i = 0; i < numOfData; i++)
            {
                if(X_train[data_id[i]][best.feature] <= best.value)
                {
                    D_Y.push_back(data_id[i]);
                }
                else
                {
                    D_N.push_back(data_id[i]);
                }
            }
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
        int rightChild(std::vector<std::pair<int, float>> *heap, int index)
        {
            if(index > heap->size() || (2 * index + 1) > heap->size())
            {
                return -1;
            }
            else
            {
                return 2 * index + 1;
            }
        }
        int leftChild(std::vector<std::pair<int, float>> *heap, int index)
        {
            if(index > heap->size() || (2 * index) > heap->size())
            {
                return -1;
            }
            else
            {
                return 2 * index;
            }
        }
        void maxHeapify(std::vector<std::pair<int, float>> *heap, int index)
        {
            int right, left;
            if (index > heap->size())
            {
                return;
            }
            right = rightChild(heap, index);
            left = leftChild(heap, index);
            if(left <= 0)
            {
                return;
            }
            if(right > 0)
            {
                if((*heap)[left - 1].second <= (*heap)[right - 1].second)
                {
                    if((*heap)[index - 1].second < (*heap)[right - 1].second)
                    {
                        std::swap((*heap)[index - 1], (*heap)[right - 1]);
                        maxHeapify(heap, right);
                    }
                }
                else{
                    if((*heap)[index - 1].second < (*heap)[left - 1].second)
                    {
                        std::swap((*heap)[index - 1], (*heap)[left - 1]);
                        maxHeapify(heap, left);
                    }
                }
            } 
        }
        void createMaxHeap(std::vector<std::pair<int, float>> *heap)
        {
            int sizeOfHeap = heap->size();
            for(int i = sizeOfHeap / 2; i > 0; i--)
            {
                //std::cout << (*heap)[i].first << ", " << (*heap)[i].second << std::endl;
                maxHeapify(heap, i);
            }
        }
        void updateMaxHeap(std::vector<std::pair<int, float>> *heap, std::pair<int, float> newElement)
        {
            if(newElement.second < (*heap)[0].second)
            {
                (*heap)[0] = newElement;
                maxHeapify(heap, 1);
            }
        }
    public:
        float **trainingSet;
        int sizeOfTrainingSet;
        int r;
        int *rfeaturesList;
        int classLocation;
        int numOfClasses;
        int smallestSizeOfANode;
        float purity;
        int fit(void)
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

            X_train = trainingSet;
            featuresList = rfeaturesList;
            n = sizeOfTrainingSet;
            c = numOfClasses;
            classLoc = classLocation;
            eta = smallestSizeOfANode;
            pi = purity;
            numOfFeatures = r;
            for(int i = 0; i < sizeOfTrainingSet; i++)
            {
                data_id.push_back(i);
            }

            if(decisionTree(root, data_id) == 1)
            {
                return 1;
            }
            else{
                return 0;
            }
        }
        std::vector<std::pair<int, float>> getQBestFeature(int q)
        {
            std::vector<std::pair<int, float>> heap;
            int counter = 0;
            if(q > featuresScore.size())
            {
                std::cout << "no enough features to extract" << std::endl;
                return heap;
            }
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
            for(int i = 0 ; i < heap.size(); i++)
            {
                std::cout << heap[i].first <<  " , " << heap[i].second << std::endl;
            }
            return heap;
        }
};
int main(void)
{
    DecisionTreeClassifier decisionTree;
    float trainingSet[6][6] = {   
        {37.8870,    1.5916,   15.8550,   39.7600,   37.7343,    1.0000},
        {37.1566,   13.8461,   47.5111,    9.3436,   13.8013,    2.0000},
        {19.6114,    2.3086,    1.7223,   24.4882,   33.9851,    1.0000},
        {32.7739,    4.8566,   21.9372,   22.2793,   32.7549,    0.0000},
        {8.5593,   41.1729,   19.0779,   32.3157,    8.1306,    1.0000},
        {35.3023,   34.7414,   38.2758,   35.4682,    5.9499,    2.0000}
    };
    float **training = (float **)malloc(6 * sizeof(float*));
    for(int i = 0; i < 6; i++)
    {
        training[i] = (float*)malloc(6 * sizeof(float));
    }
    for(int i = 0; i < 6; i++)
    {
        for(int j = 0; j < 6; j++)
        {
            training[i][j] = trainingSet[i][j];
        }
    }
    int rFeatureList[5] = {0,1,2,3,4};
    decisionTree.trainingSet = training;
    decisionTree.sizeOfTrainingSet = 6;
    decisionTree.r = 5;
    decisionTree.rfeaturesList = rFeatureList;
    decisionTree.classLocation = 5;
    decisionTree.numOfClasses = 3;
    decisionTree.smallestSizeOfANode = 1;
    decisionTree.purity = 0.8;
    decisionTree.fit();
    decisionTree.getQBestFeature(2);
}

