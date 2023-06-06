#ifndef __DATA_HPP
#define __DATA_HPP

#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <unordered_set>
#include <random>
#include <algorithm>
#include <cmath>

/*
Objects of class data are used to store images belonging to the MNIST dataset, as vectors (flattened images).
A data object corresponds to a 28x28 image (diamensions are (28x28)x1 due to flattening of the image).
Each vector component contains the value of a pixel of the image, as well as the label of the image (a digit between 0-9).
It also contains the field enumerated label which represents each digit between 0-9 with an integer between 0-9, in order to
represent the categorical class with an integer.

The entries are sets with the following form:
data point = {[P1 P2 ...Pn] , label, enum_label }, n = 28*28

The class contains  the following fields:   feature_vector: to store the flattened image
                                            label: to store the category of each image
                                            enum_label: to enumerate the label
                                            distance: to keep the distance between other data points

The class contains the following functions: set_feature_vector 
                                            set_label
                                            set_enumerated_label
                                            set_distance 
                                            append_to_feature_vector: adds an element to feature vector
                                            get_feature_vector
                                            get_feature_vector_size: returns the length of the feature vector
                                            get_label
                                            get_enumerated_label
                                            get_distance
                                            
This class is used in data_handler.hpp to store the images and their labels (see data_handler.hpp header file).
*/

class data
{
    std::vector<uint8_t> * feature_vector;  //Pointer to an std::vector object that stores uint8_t values.
    uint8_t label;                          //Class label as 8-bit unsigned integer.
    int enum_label;                        //Enumerated class label.
    double distance;
    

   
    public:   //Constructor & destructor.
    data(){ 
        feature_vector = new std::vector<uint8_t>;
    };  

    ~data(){}; 

    
    void set_feature_vector(std::vector<uint8_t> *vect){  //Set methods.
        feature_vector = vect;
    }; 

    void set_label(uint8_t val){ 
        label = val;
    };  

    void set_enumerated_label(int val){
        enum_label = val;
    };

    void set_distance(double val){
        distance = val;
    }

    void append_to_feature_vector(uint8_t val){
        feature_vector->push_back(val);
    };       

    std::vector<uint8_t> *get_feature_vector(){  //Get methods.
        return feature_vector;
    };

    int get_feature_vector_size(){
        return feature_vector->size();
    };

    uint8_t get_label(){
        return label;
    };

    
    uint8_t get_enumerated_label(){
        return enum_label;
    }; 

    double get_distance(){
        return distance;
    }
    

};

#endif