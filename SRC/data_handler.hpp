#ifndef __DATA_HANDLER_HPP
#define __DATA__HANDLER_HPP

#include "data.hpp"

/*
Class used to sore and handle data objects.
A data_handler ovject contains fields to store  data objects
as well as important variables which store information avout the data.
In order to have a good understanding of the code below, read the PDF:
MNIST+handwritten+digit+database,+Yann+LeCun,+Corinna+Cortes+and+Chris+Burges
located in the DATASET folder.

The class contains the following fields:    data_array: All of the data
(which are vectors where each element       training_data
corresponds (pointer) to a data object)     testing_data
                                            validation_data                                    

It also contains the following fields:  num_classes:            To store the number of categories
                                        feature_vector_size:    Length of feature vectors (number of variables)
                                        TRAIN_SET_PERCENT:      80% of data
                                        TEST_SET_PERCENT:       10% οf data
                                        VALIDATION_SET_PERCENT: 10% of data                                        

It also contains the field class_map: associative container which stores pairs of (label, enumerated label).
                                    
The class contains the following functions: convert_to_little_endian:   Convert const unigned char bytes to little endian. used to read the headers of data files.
                                            read_feature_vector:        Load data into the data array
                                            read_feature_labels:        Load labels
                                            split_data:                 Train-test-val split
                                            count_classes          
                                            count_features
                                            get_data_array  
                                            get_training_data
                                            get_validation_data
                                            get_testing_data
                                            get_number_of_classes
                                            enumerate_classes:          1-1 correspondence between the labels and the numbers 0-9. 
                                                                        Notice that a label x may not correspond to the number x, for example 
                                                                        an image with class label 9 may have as enumerated label the number 1, and no other class
                                                                        label corresponds to 1.
                                            print_map:                  Print hte labels with the corresponding enumerated labels.
                                            
For further details regarding the implementation of the functions see below.
*/

class data_handler
{
   
    std::vector<data *> *data_array;            
    std::vector<data *> *training_data;         
    std::vector<data *> *test_data;
    std::vector<data *> *validation_data;
   
    int num_classes;                    
    int feature_vector_size;            
    const double TRAIN_SET_PERCENT = 0.80;  
    const double TEST_SET_PERCENT = 0.10;
    const double VALIDATION_SET_PERCENT = 0.10;

    std::map<uint8_t, int> class_map;           //Object of the std::map class template from the C++ Standard Library. 
                                                //It represents an associative container that stores key-value pairs, 
                                                //where the keys are of type uint8_t and the values are of type int.
                                                //Something like SQL table, where the field uint8_t is the index and int is the entry.

    
    public:
    data_handler(){                             
        data_array = new std::vector<data *>;  
        training_data = new std::vector<data *>;
        test_data = new std::vector<data *>;
        validation_data = new std::vector<data *>;
    };
    ~data_handler(){};

    uint32_t convert_to_little_endian(const unsigned char * bytes){  //Convert const unigned char bytes to little endian.
        return (uint32_t) ( ( bytes[0] << 24) |
                            ( bytes[1] << 16) | 
                            ( bytes[2] << 8)  |
                            ( bytes[3]));
        
    }; 

    void read_feature_vector(std::string path){     //Load data.
        std::ifstream infile;
        infile.open(path.c_str(), std::ios::binary | std::ios::in);

        uint32_t header[4];                         // Read : Magic Number, Number of  Images, Row size  and Column size of each image.
        char bytes[4];
        unsigned char unbytes[4];

        uint8_t element[1];
        char elementchar[1];

        int image_size;
        int loopNum;
       
        for (int i = 0; i < 4; ++i){                //Read 4 chars (1 byte), convert them to unsigned chars and then convert to little endian.
            if (infile.read(bytes, sizeof(bytes))){ 
                for (int j = 0; j < 4; ++j){
                    unbytes[j] = bytes[j];
                }
                header[i] = convert_to_little_endian(unbytes);
            }
            else{
                std::cout << "\nError Reading the header from image file" << std::endl;
                exit(1);
            }
        }       

        image_size = header[2] * header[3];         //Compute image size and number of images.
        loopNum = header[1];
  
        for (int i = 0; i < loopNum; ++i){          //Iterate thbrough images and parse each image as a data point.
            data* d = new data();

            for (int j = 0; j < image_size; ++j){ 
                if (infile.read(elementchar, sizeof(elementchar))){

                    element[0] = (uint8_t)atoi(elementchar);
                    d->append_to_feature_vector(element[0]);
                }
                else{
                    std::cout << "\nError Reading from image file" << std::endl;
                    exit(1);
                }

            }

            data_array->push_back(d);
        }

        std::cout << "\nSuccessfuly read and stored " << data_array->size() << " feature vectors." << std::endl;
        infile.close();
}
    
    void read_feature_labels(std::string path){    //Read labels of each image and store it.
        uint32_t header[2];
        char bytes[4];
        unsigned char unbytes[4];

        std::ifstream infile;
        infile.open(path.c_str(), std::ios::binary | std::ios::in);

   
        for (int i = 0; i < 2; ++i){ 
            if (infile.read(bytes, sizeof(bytes))){
                for (int j = 0; j < 4; ++j){
                    unbytes[j] = bytes[j];
                }
                header[i] = convert_to_little_endian(unbytes);
            }
            else{
                std::cout << "\nError Reading the header from image file" << std::endl;
                exit(1);
            }
        } 

        int number_of_items = header[1];
        uint8_t element[1];
        char elementchar[1];

        
        int labels = 0;
        for(int i = 0; i < number_of_items; i++){
            if (infile.read(elementchar, sizeof(elementchar))){
                element[0] = *elementchar;
                data_array->at(i)->set_label(element[0]);
                labels++;
            }
            else{
                std::cout << "\nError while reading from label file" << std::endl;
                exit(1);
            }
        }
        std::cout << "\nSuccessfuly read and stored " << labels << " labes." << std::endl;
        infile.close();
    };

    void split_data(){                                                      //Create train,test and val data
        int train_size = data_array->size() * TRAIN_SET_PERCENT;
        int test_size = data_array->size() * TEST_SET_PERCENT;
        int validation_size = data_array->size() * VALIDATION_SET_PERCENT;
       
        data * entry = new data();
        std::unordered_set<int> used_indexes;   //To keep track of the indexes, when splitting the data.
        std::vector<int> values;                //To store indexes in random order.
        int random_value;                       //To store a single random index.
        int counter;

        std::random_device rd;                  //Used for seeding the random number generator.
        std::mt19937 generator(rd());           //Random number generator.

        for(int i = 0; i < (int) data_array->size(); i++){ //Store indexes and shuffle them
            values.push_back(i);
        }
        std::shuffle(values.begin(), values.end(), generator);
        

        counter = 0;                                                        //Creating train dataset
        while(counter < train_size){
            random_value = values[counter];                                 //Choose  a random index.
            if(used_indexes.find(random_value) == used_indexes.end()){      //If the corresponding entry at that index has not been used, it will be used and
                used_indexes.insert(random_value);                          //the index is no longer available to use.
                entry = data_array->at(random_value);
                training_data->push_back(entry); 
                counter = counter + 1;
            }
        }

        counter = train_size;                                         //Similarly create validation and testing set
        while(counter < test_size + train_size){
            random_value = values[counter]; 
            if(used_indexes.find(random_value) == used_indexes.end()){ 
                used_indexes.insert(random_value);  
                entry = data_array->at(random_value);
                test_data->push_back(entry); 
                counter = counter + 1;
            }
        }

        counter =  test_size + train_size;
        while(counter < test_size + train_size + validation_size){
            random_value = values[counter]; 
            if(used_indexes.find(random_value) == used_indexes.end()){ 
                used_indexes.insert(random_value);  
                entry = data_array->at(random_value);
                validation_data->push_back(entry); 
                counter = counter + 1;
            }
        }
        std::cout << "\nData split into training, testing and validation sets" << std::endl;
        std::cout << "--------Training set size = " << train_size << std::endl;
        std::cout << "--------Testing set size = " << test_size << std::endl;
        std::cout << "--------Validation set size = " << validation_size << std::endl;
    }

    void count_classes(){   //Iterate over data array, do the mapping between class labels and integers and count the classes.
        int counter = 0;
      
        for(int i = 0; i < (int) data_array->size(); i++){
            if(class_map.find(data_array->at(i)->get_label()) == class_map.end()){
                class_map[data_array->at(i)->get_label()] = counter;
                counter = counter + 1;
            }
        }
        num_classes = counter;
        std::cout << "\nNumber of Classes = " << num_classes << std::endl;
    };

    void count_features( ){ //Count the number of features/data entry.
        int features_per_entry;
        int features_of_prev_entry;
        
        
        int i = 0;
        do{ features_per_entry = data_array->at(i)->get_feature_vector()->size();
            if(i > 0){
                features_of_prev_entry = data_array->at(i-1)->get_feature_vector()->size();
                if( features_of_prev_entry != features_per_entry ){
                    std::cout << "\nEntries with different features" << std::endl;
                }
            }
            i= i+1;
        }
        while(i < (int) data_array->size());
        feature_vector_size = features_per_entry;
        std::cout << "\nNumber of features = " << feature_vector_size << std::endl;
    };

    //Get methods
    std::vector<data *> *get_data_array(){
        return data_array;
    };

    std::vector<data *> *get_training_data(){
        return training_data;
    }; 

    std::vector<data *> *get_testing_data(){
        return test_data;
    };
    std::vector<data *> *get_validation_data(){
        return validation_data;
    };

    int get_number_of_classes(){
        return num_classes;
    }

    void print_map(){  //note that some class actual labels are not printable
        std::map<uint8_t,  int >::iterator it ;
        
        
        for(it = class_map.begin() ; it != class_map.end(); it++){
            std::cout << it->second  <<  std::endl;
        } 
    }

    void enumerate_classes(){
        
        std::map<uint8_t,  int >::iterator it ;

        for(int i = 0; i < (int) data_array->size(); i++){
            
            for(it = class_map.begin() ; it != class_map.end(); it++){
                if(data_array->at(i)->get_label() == it->first){
                    data_array->at(i)->set_enumerated_label(it->second);
                    break;
                }
            }
        }
    };
};

#endif