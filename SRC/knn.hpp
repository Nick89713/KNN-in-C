#ifndef __KNN_HPP
#define __KNN_HPP

#include "data_handler.hpp"
#include <iterator> 
#include <string.h>
#include <tuple>

/*
Class that creates a knn classifier.
The class contains the following fields:    Κ:                      KNN's number of neighbors
                                            neighbors:              Vector which stores the nearest data points
                                            distance_formula:       Used to select method for distance calculation
                                            distance_class_map:     Associative container which stores pairs of (distance, enumerated label).
                                            training_data
                                            testing_data
                                            validation_data

The class contains the following functions: tune:                           Set KNN's hyperparameters
                                            get_K
                                            get_number_of_classes
                                            get_distance_formula
                                            set_training_data
                                            set_validation_data
                                            set_testing_data
                                            calculate_euclidean_distance
                                            calculate_manhattan_distance
                                            find_neighbors:                 Find K nearest neighbors of a data point
                                            predict_class:                  Predict the enumerated class label of a data point
                                            fit_predict:                    Perform classification on testing data, using the training data
                                            accuracy:                       Calculate accuracy given the predictions made on the testing data

For further details regarding the implementation of the functions see below.
*/




class knn{
    int K;
    std::string distance_formula;
    int number_of_classes;
    std::vector<data *> *neighbors;
    
    std::vector<data  *> *training_data;
    std::vector<data  *> *testing_data;
    std::vector<data  *> *validation_data;

    std::map<double,  int> distance_class_map;

    public:
        knn(){};
        ~knn();
        
        
        void tune(int k, int num_classes, std::string formula){
            K = k;
            number_of_classes = num_classes;
            distance_formula = formula;
        }

        
        int get_K(){
            return K;
        }

        int get_number_of_classes(){
            return number_of_classes;
        };
       
        std::string get_distance_formula(){
            return distance_formula;
        };

        //train,test,val sets for knn
        void set_training_data(std::vector <data *> *training_set){
            training_data = training_set;
        };

        void set_validation_data(std::vector <data *> *validation_set){
            validation_data = validation_set;
        };

        void set_testing_data(std::vector <data *> *testing_set){
            testing_data = testing_set;
        };

        double calculate_euclidean_distance(data *vec1, data *vec2){
            if(vec1->get_feature_vector_size() != vec2->get_feature_vector_size()){
                std::cout << "Unable to valculate distance between vectors of different spaces" << std::endl;
                exit(1);
            }
            double sum = 0; 
            for(int i = 0; i < vec1->get_feature_vector_size(); i++){
                sum = sum + pow(vec1->get_feature_vector()->at(i) - vec2->get_feature_vector()->at(i), 2);
            }
            return sqrt(sum);
        };

        double calculate_manhattan_distance(data *vec1, data *vec2){
             if(vec1->get_feature_vector_size() != vec2->get_feature_vector_size()){
                std::cout << "Unable to valculate distance between vectors of different spaces" << std::endl;
                exit(1);
            }
            double sum = 0;
            for(int i = 0; i < vec1->get_feature_vector_size(); i++){
                sum = sum + abs( vec1->get_feature_vector()->at(i) - vec2->get_feature_vector()->at(i));
            }
            return sum;
        };
     

       
        void find_neighbors(data *new_point){
            neighbors = new std::vector<data *>();
            double min = std::numeric_limits<double>::max();        //min is initially equal to the max of the doubles
            double previous_min;
            int index = 0;
            double dist;

            for(int i = 0; i < K; i++){
                if(i == 0){                                         //First scan:calculate distances from new point and find minimum distance
                    for(int j = 0; j <(int) training_data->size(); j++){
                        if(strcmp(distance_formula.c_str(), "Euclidean") == 0 ){ 
                            dist = calculate_euclidean_distance(new_point,training_data->at(j));
                        }
                        else if(strcmp(distance_formula.c_str(), "Manhattan") == 0 ){
                            dist = calculate_manhattan_distance(new_point,training_data->at(j));
                        }
                        training_data->at(j)->set_distance(dist);

                        if(dist < min){ 
                            min = dist;
                            index = j;
                        }
                    }

                    neighbors->push_back(training_data->at(index));
                    previous_min = min;
                    min = std::numeric_limits<double>::max();
                }
                else{                                           //For the other scans: get (k-1) training data points with smaller distances
                    for(int j = 0; j <(int) training_data->size(); j++){
                        if(training_data->at(j)->get_distance() > previous_min && training_data->at(j)->get_distance() < min){
                            min = training_data->at(j)->get_distance();
                            index = j;
                        }
                    }

                    neighbors->push_back(training_data->at(index));
                    previous_min = min;
                    min = std::numeric_limits<double>::max();
                }
               
            }
        };

        int predict_class(){
            int max_index;
            int *frequency{ new int[number_of_classes]{0} };            //Initialize all frequencies to 0
            
            for(int i = 0; i < number_of_classes; i++){                 //Find frequency of K nearest  classes
                for(int n = 0; n < (int) neighbors->size(); n++){
                    if((int) neighbors->at(n)->get_enumerated_label() == i){
                        frequency[i]++;
                    }
                }
            }
           
            for(int i = 0; i < number_of_classes; i++){                 //Find class with max frequency
                if(frequency[i] > frequency[i-1]){
                    max_index = i;
                }
            }
            
            if(max_index < 0 ){ 
                for(int i = 0; i < number_of_classes; i++){
                    std::cout << frequency[i] << std::endl;
                }
                std::cout << " Error" << std::endl;
                exit(1);}
            return max_index;
        }
        
       std::vector<int> *fit_predict(){
            std::vector<int> *predictions = new  std::vector<int>();

            for(int i = 0; i < (int) testing_data->size(); i++){
                find_neighbors(testing_data->at(i));
                predictions->push_back(predict_class());
                delete neighbors;
                std::cout << "Testing data point - " << i << " examined" << std::endl;
            }

           return predictions;
        };
        
        double accuracy(std::vector<int> *predictions){                     //Compute accuracy = correct_predictions/total number of samples
            int correct_predictions = 0;
            for(int i = 0; i < (int) testing_data->size(); i++){
                if(predictions->at(i) == ((int)testing_data->at(i)->get_enumerated_label())){
                    correct_predictions++;
                }
            }

            return correct_predictions/((int) testing_data->size());
        }
        
}; 

#endif