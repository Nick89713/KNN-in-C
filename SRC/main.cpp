//#include "data.hpp"
//#include "data_handler.hpp"
#include "knn.hpp"
#include <string.h>

int main(){

 
  std::string training_data_path = "C:/Users/Hp/Desktop/ml_cpp/dataset/train-images.idx3-ubyte";
  std::string training_labels_path = "C:/Users/Hp/Desktop/ml_cpp/dataset/train-labels.idx1-ubyte";
  
  //create a data handler
  data_handler * dh = new data_handler();

  //load data
  dh->read_feature_vector(training_data_path);
  dh->read_feature_labels(training_labels_path);

  


  //split
  dh->split_data();

  //count features
  dh->count_features();
  
   //count classes
  dh->count_classes();

  //enumerate classes
  dh->enumerate_classes();

  
  //create a knn object
  knn *knn_classifier = new knn();

  //set hyperparameters
  knn_classifier->tune(4, dh->get_number_of_classes(), "Euclidean"  );
  std::cout << "KNN tuned" << std::endl;


  //train,test, val data parsed
  knn_classifier->set_training_data(dh->get_training_data());
  knn_classifier->set_testing_data(dh->get_testing_data());
  knn_classifier->set_validation_data(dh->get_validation_data());
  std::cout << "Training, testing and  validation datasets defined" << std::endl;

  
  //apply knn
  std::vector<int> *predictions = new  std::vector<int>();
  predictions = knn_classifier->fit_predict();
  std::cout << "Classification done" << std::endl;

  //find accuracy
  double accuracy = knn_classifier->accuracy(predictions);
  std::cout << "Accuracy = " << accuracy << std::endl;

}

