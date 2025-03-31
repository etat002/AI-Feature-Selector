#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>

const std::vector<std::string> files = {"small-test-dataset.txt", "Large-test-dataset.txt", "CS170_Spring_2022_Large_data__67.txt", "CS170_Spring_2022_Small_data__67.txt"};
const std::vector<std::string> algorithms = {"Forward Selection", "Backward Elimination"};

void z_normalize(std::vector<std::vector<float>> &data);
void forward_selection(std::vector<std::vector<float>> &data);
void backward_elimination(std::vector<std::vector<float>> &data);
double leaveOneOutCrossValidation(std::vector<std::vector<float>> data, std::vector<int> features, int featureInQuestion);
void printCurrent(std::vector<int> current);
float distance(std::vector<float> pointA, std::vector<float> pointB);

int main() {
    int choice;
    std::cout << "Welcome to Eddy Tat's Feature Selection Algorithm." << std::endl;
    std::cout << "Which file would you like to test?" << std::endl;
    std::cout << "Type the number that corresponds with the desired file" << std::endl;
    for(int i=0; i<files.size(); i++){
        std::cout << i+1 << ". " << files[i] << std::endl;
    }
    do{
        std::cin >> choice;
    }
    while(choice < 1 || choice > files.size());

    srand(time(NULL));
    std::vector<std::vector<float>> data;                   //this is the data
    std::ifstream file(files[choice-1]);                 //this chunk of code reads in the file and puts the data into a 2D vector called data
    std::string line;
    while(std::getline(file, line)){
        std::vector<float> oneLine;
        std::stringstream stream(line);
        float number;
        while(stream >> number){
            oneLine.push_back(number);
        }
        data.push_back(oneLine);
    }
    file.close();
    z_normalize(data);                                  //this normalizes the data

    std::cout << "Type the number that corresponds with the algorithm you want to run" << std::endl;
    for(int i=0; i<algorithms.size(); i++){
        std::cout << i+1 << ". " << algorithms.at(i) << std::endl;
    }
    do{
        std::cin >> choice;
    }
    while(choice < 1 || choice > files.size());

    if(choice == 1){
        forward_selection(data);
    }
    else if(choice == 2) {
        backward_elimination(data);
    }
//    for(auto & i : data){
//        printf("%f\n", i.at(1));
//    }
    return 0;
}

void z_normalize(std::vector<std::vector<float>> &data){
    float mean, std_dev, sum;
    for(int i = 1; i < data.at(0).size(); i++){
        sum = 0;
        float numerator=0;
        for(int j=0; j<data.size(); j++){
            sum+= data.at(j).at(i);
        }
        mean = sum/(data.size());
        for(int j=0; j<data.size(); j++){
            numerator += pow((data.at(j).at(i)-mean), 2.00);
        }
        std_dev = pow(numerator/(data.size()-1), 0.500);
        for(int j=0; j<data.size(); j++){
            data.at(j).at(i) = (data.at(j).at(i)-mean)/std_dev;
        }
//    float mean, std_dev, sum;
//    for(int i = 0; i< data.size(); i++){
//        sum = 0;
//        float numerator = 0;
//        for(int j = 1; j<data.at(i).size(); j++){
//            sum += data.at(i).at(j);
//        }
//        mean = sum/(data.at(i).size()-1);
//        for(int j = 1; j<data.at(i).size(); j++){
//            numerator += pow((data.at(i).at(j)-mean), 2);
//        }
//        std_dev = pow(numerator/(data.at(i).size()-1-1), 0.5);
//
//        for(int j = 1; j<data.at(i).size(); j++){
//            data.at(i).at(j) = (data.at(i).at(j)-mean)/std_dev;
//        }
//        sum=0;
//        for(int j = 1; j<data.at(i).size(); j++) {
//            sum += data.at(i).at(j);
//        }
//        mean = sum/(data.at(i).size()-1);
//        std::cout << "this is normalized mean of " << i << ": " << mean << std::endl << std::endl;
    }
}

void forward_selection(std::vector<std::vector<float>> &data){
    int ones = 0, twos = 0;                         //first calculate the default rate with no features
    for(int i=0; i<data.size(); i++){
        if(data.at(i).at(0) == 1){
            ones++;
        }
        else{
            twos++;
        }
    }
    float greater;
    if(ones > twos){                                //default rate is based on the most common class
        greater = ones;
    }
    else{
        greater = twos;
    }
    std::cout << "Running nearest neighbor with no features (default rate), using \"leave one out\" evaluation, accuracy is " << ((greater/(ones+twos))*100.00) << "%\n" << std::endl;
    std::vector<int> currentFeatures;
    double bestAccAtm = 0;
    double tempAcc;
    bool accChange;
    int tempFeature;
    for(int i=1; i<data.at(0).size(); i++){
        accChange = false;
        tempAcc = 0;
//        if(i%100 > 10 && i%100 < 20){
//            std::cout << "On the " << i << "th level of the search tree" << std::endl;
//        }
//        else if(i%10 == 1) {
//            std::cout << "On the " << i << "st level of the search tree" << std::endl;
//        }
//        else if(i%10 == 2){
//            std::cout << "On the " << i << "nd level of the search tree" << std::endl;
//        }
//        else if(i%10 == 3){
//            std::cout << "On the " << i << "rd level of the search tree" << std::endl;
//        }
//        else{
//            std::cout << "On the " << i << "th level of the search tree" << std::endl;
//        }
        int addFeature;
        for(int j=1; j<data.at(0).size(); j++) {
            bool contains = false;
            for(int k=0; k<currentFeatures.size(); k++){
                if(currentFeatures.at(k) == j){
                    contains = true;
                    break;
                }
            }
            if(!contains){
                //std::cout << "  --Considering adding the " << j << " feature" << std::endl;
                double accuracy = leaveOneOutCrossValidation(data, currentFeatures, j);
                if(!currentFeatures.empty()) {
                    std::cout << "Using feature(s) {";
                    printCurrent(currentFeatures);
                    std::cout << ", " << j << "} accuracy is " << accuracy << "%" << std::endl;
                }
                else{
                    std::cout << "Using feature(s) {";
                    std::cout << j << "} accuracy is " << accuracy << "%" << std::endl;
                }
                if(accuracy >= tempAcc){
                    tempAcc = accuracy;
                    tempFeature = j;
                }
                if(accuracy >= bestAccAtm){
                    bestAccAtm = accuracy;
                    addFeature = j;
                    accChange = true;
                }
            }
        }
        if(accChange) {                                                         //if accuracy has improved
            currentFeatures.push_back(addFeature);
            //std::cout << "On level " << i << ", added feature " << addFeature << std::endl;
            std::cout << "\nFeature set {";
            printCurrent(currentFeatures);
            std::cout << "} was best, accuracy of " << bestAccAtm << "%" << std::endl;
            std::cout << "Adding: " << addFeature << std::endl << std::endl;
        }
        else{                                                                   //if accuracy hasn't improved
            std::cout << "\nFeature set {";
            printCurrent(currentFeatures);
            if(currentFeatures.empty()) {
                std::cout << tempFeature << "} was best, accuracy of " << tempAcc << "%" << std::endl;
            }
            else{
                std::cout << ", " << tempFeature << "} was best, accuracy of " << tempAcc << "%" << std::endl;
            }
            std::cout << "Accuracy dropped!" << std::endl << std::endl;
            std::cout << "Finished search! The best feature subset is {";
            printCurrent(currentFeatures);
            std::cout << "}, which has an accuracy of " << bestAccAtm << "%" << std::endl;
            break;
        }
    }
}

void backward_elimination(std::vector<std::vector<float>> &data){
    std::vector<int> allFeatures;
    for(int i=1; i<data.at(0).size(); i++){
        allFeatures.push_back(i);
    }
    float allAcc = leaveOneOutCrossValidation(data, allFeatures, -1);       //calculates accuracy by passing in a set with all features and not remove any features
    std::cout << "Running nearest neighbor with all features (default rate), using \"leave one out\" evaluation, accuracy is " << allAcc << "%\n" << std::endl;
    std::vector<int> currentFeatures;
    for(int i=1; i<data.at(0).size(); i++){
        currentFeatures.push_back(i);
    }
    double bestAccAtm = 0;
    double tempAcc;
    bool accChange;
    int tempFeature;
    for(int i=1; i<data.at(0).size(); i++){
        accChange = false;
        tempAcc = 0;
//        if(i%100 > 10 && i%100 < 20){
//            std::cout << "On the " << i << "th level of the search tree" << std::endl;
//        }
//        else if(i%10 == 1) {
//            std::cout << "On the " << i << "st level of the search tree" << std::endl;
//        }
//        else if(i%10 == 2){
//            std::cout << "On the " << i << "nd level of the search tree" << std::endl;
//        }
//        else if(i%10 == 3){
//            std::cout << "On the " << i << "rd level of the search tree" << std::endl;
//        }
//        else{
//            std::cout << "On the " << i << "th level of the search tree" << std::endl;
//        }
        int removeFeature;
        for(int j=1; j<data.at(0).size(); j++) {
            bool contains = false;
            for(int k=0; k<currentFeatures.size(); k++){            //checks if the current features has the feature to be removed
                if(currentFeatures.at(k) == j){
                    contains = true;
                    break;
                }
            }
            if(contains){
                //std::cout << "  --Considering adding the " << j << " feature" << std::endl;
                double accuracy = leaveOneOutCrossValidation(data, currentFeatures, j);
                if(!currentFeatures.empty()) {
                    std::vector<int> featuresTemp;
                    featuresTemp = currentFeatures;
                    for(int k=0; k<featuresTemp.size(); k++){
                        if(featuresTemp.at(k) == j){
                            featuresTemp.erase(featuresTemp.begin()+k);
                        }
                    }
                    std::cout << "Using feature(s) {";
                    printCurrent(featuresTemp);
                    std::cout << "} accuracy is " << accuracy << "%" << std::endl;
                }
                else{
                    std::cout << "Using feature(s) {";
                    std::cout << "} accuracy is " << accuracy << "%" << std::endl;
                }
                if(accuracy >= tempAcc){
                    tempAcc = accuracy;
                    tempFeature = j;
                }
                if(accuracy >= bestAccAtm){
                    bestAccAtm = accuracy;
                    removeFeature = j;
                    accChange = true;
                }
            }
        }
        if(accChange) {                                             //if the accuracy has improved
            //currentFeatures.push_back(addFeature);
            for(int j=0; j<currentFeatures.size(); j++){
                if(currentFeatures.at(j) == removeFeature){
                    currentFeatures.erase(currentFeatures.begin()+j);
                }
            }
            //std::cout << "On level " << i << ", added feature " << addFeature << std::endl;
            std::cout << "\nFeature set {";
            printCurrent(currentFeatures);
            std::cout << "} was best, accuracy of " << bestAccAtm << "%" << std::endl;
            std::cout << "Removing: " << removeFeature << std::endl << std::endl;
        }
        else{                                                       //if the accuracy has not improved
            std::vector<int> tempVec;
            tempVec = currentFeatures;
            for(int j=0; j<tempVec.size(); j++){
                if(tempVec.at(j) == tempFeature){
                    tempVec.erase(tempVec.begin()+j);
                }
            }
            std::cout << "\nFeature set {";
            printCurrent(tempVec);
            std::cout << "} was best, accuracy of " << tempAcc << "%" << std::endl;
            std::cout << "Accuracy dropped!" << std::endl << std::endl;
            std::cout << "Finished search! The best feature subset is {";
            printCurrent(currentFeatures);
            std::cout << "}, which has an accuracy of " << bestAccAtm << "%" << std::endl;
            break;
        }
    }
}

double leaveOneOutCrossValidation(std::vector<std::vector<float>> data, std::vector<int> features, int featureInQuestion){
    bool adding = true;                         //if not adding, then you're subtracting (removing feature from list)
    if(!features.empty()) {
        for (int i = 0; i < features.size(); i++) {
            if (features.at(i) == featureInQuestion) {
                adding = false;
            }
        }
    }
    if(adding && featureInQuestion != -1){
        features.push_back(featureInQuestion);
    }
    else{
        for(int i=0; i<features.size(); i++){
            if(features.at(i) == featureInQuestion) {
                features.erase(features.begin() + i);
            }
        }
    }
    double correct = 0;
    for(int i=0; i<data.size(); i++){                               //goes through the list of data and chooses row i as the testing data
        std::vector<float> test;
        for(int j=0; j<features.size(); j++){
            test.push_back(data.at(i).at(features.at(j)));
        }
        //test = data.at(i);
        float closestDistance = LONG_LONG_MAX;
        int nearestNeighbor;
        for(int j=0; j<data.size(); j++){                           //goes through the list of data and chooses row j as a training data
            if(j!=i){
                std::vector<float> train;
                for(int k=0; k<features.size(); k++){
                    train.push_back(data.at(j).at(features.at(k)));
                }
                if(distance(test, train) < closestDistance){
                    closestDistance = distance(test, train);
                    nearestNeighbor = j;
                }
            }
        }
        //std::cout << "test class " << i << ": " << data.at(i).at(0) << " and nn class " << nearestNeighbor << ": " << data.at(nearestNeighbor).at(0) << std::endl;
        if(data.at(i).at(0) == data.at(nearestNeighbor).at(0)){
            correct++;
        }
    }
    return (correct/data.size())*100.00;
    //return rand()%100;
}

void printCurrent(std::vector<int> current){    //prints out integers in vector in the form of "a, b,..., z"
    if(!current.empty()) {
        if(current.size() > 1) {
            for (int i = 0; i < current.size()-1; i++) {
                std::cout << current.at(i) << ", ";
            }
            std::cout << current.at(current.size()-1);
        }
        else{
            std::cout << current.at(0);
        }
    }
}

float distance(std::vector<float> pointA, std::vector<float> pointB){
    //std::cout << "pointA size: " << pointA.size() << ", pointB size: " << pointB.size() << std::endl;
    float total = 0;
    for(int i=0; i<pointA.size(); i++){
        total+= pow(pointB.at(i) - pointA.at(i), 2.000);        //euclidean distance formula
    }
    return pow(total, 0.500);
}