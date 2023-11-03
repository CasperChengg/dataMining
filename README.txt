To use the decision_tree_classifier, you need to first include the following line:

	#include"header_files/decision_tree_classifier.h"

After that, declare an object as follow

	class DecisionTreeClassifier [myDecisionTreeClassifier]

Before training, set the following parameters.

	myDecisionTreeClassifier.trainingSet = ...;
	myDecisionTreeClassifier.sizeOfTrainingSet = ...;
	myDecisionTreeClassifier.seclectedFeatures = ...;
	myDecisionTreeClassifier.numOfSeclectedFeatures = ...;
	myDecisionTreeClassifier.classLocation = ...;
	myDecisionTreeClassifier.numOfClasses = ...;
	myDecisionTreeClassifier.smallestSizeOfANode = ...;
	myDecisionTreeClassifier.purity = ...;

Then you can start training by calling
	
	myDecisionTreeClassifier.fit();

To get the best q features, call the following function:
	
	myDecisionTreeClassifier.getQBestFeatures(int q, int *qBestFeatures);

Note that if the number of features is not greater than q, the function will return all the features and end with -1.