"""
For the purposes of this file, a vector is defined as a numpy array with
his first element being his "key" and the elements after that are his
coordinates.
All vectors has number of dimensions that equal the variable vector_len
"""



import numpy as np
import math
import pandas as pd
import sys
import kmeanssp as kmeans
np.random.seed(0)





def distance(vec1, vec2):
    #Returns the square of the distance between the two vectors
    output = 0
    for i in range(1,len(vec1)):
        output += (vec1[i]-vec2[i])**2
    return math.sqrt(output)

def closest_centroid(vec):
    #Returns the distance between vec and the vector in the array
    #initial_centroids that is the closest to vec
    global initial_centroids
    output = distance(vec , initial_centroids[0])
    for i in range(1, len(initial_centroids)):
        distance_from_vec = distance(vec , initial_centroids[i])
        if(distance_from_vec < output):
            output = distance_from_vec
    return output

def add_to_initial_vectors():
    #Adds the next element that should be added to initial_vectors

    global initial_centroids
    global vectors_arr
    #Extreme case
    if(len(initial_centroids)==0):
        new_centroid_index = np.random.choice(range(len(vectors_arr)))
        new_centroid = vectors_arr[new_centroid_index]
        initial_centroids.append(new_centroid)
        return

    
    
    distances = [] #Distances of all vectors as culculated by closest_centroid
    for vec in vectors_arr:
        distances.append(closest_centroid(vec))
        
    sum_of_distances = 0 #Sum of all elements in distances
    for d in distances:
        sum_of_distances += d

    distribution = [] #The chance to picking each vector
    for d in distances:
        distribution.append(d/sum_of_distances)

    new_centroid_index = np.random.choice(range(len(vectors_arr)), p = distribution)
    new_centroid = vectors_arr[new_centroid_index]
    initial_centroids.append(new_centroid)

def culc_initial_centroids():
    #Culculates what vectors should go into initial_centroids and writes them
    #into initial_centroids
    global num_of_clusters
    for i in range(num_of_clusters):
        add_to_initial_vectors()

def read_vectors(input_data):
    file1 = open(input_data, 'r')
    data_points = []

    for line in file1:
        line_lst = line.split(",")
        data_points.append([float(x) for x in line_lst])
    file1.close()
    global vector_len
    vector_len = len(data_points[0])
    return data_points

'''def array_to_data_frame(arr):
    #Takes an array of vectors and convert it to a data_frame

    text_dict = dict()
    next_vector = None
    for i in range(len(arr)):
        #Every iteration adds a line from the iterator to text_dict
        next_vector = arr[i]
        text_dict["vector" + str(i)] = next_vector

    indexes = ["key"] + ["coordinate" + str(i) for i in range(1 ,len(next_vector))]
    #The indexes for the output
    return pd.DataFrame(data = text_dict, index = indexes)'''

def array_to_data_frame(arr):
    #Takes an array of vectors and converts it to a data_frame
    index_labels = ["vector" + str(i) for i in range(len(arr))]
    column_labels = ["key"] + ["coordinate" + str(i) for i in range(1 ,vector_len)]
    return pd.DataFrame(data = arr, index = index_labels, columns = column_labels)
        

def file_to_data_frame(file_name):
    #Returns the data_frame that a text file with a list of vectors represents
    file_array = np.array(read_vectors(file_name))
    return array_to_data_frame(file_array)

def sort_by_key(arr):
    #Sorts a list of vectors by their keys
    #The key of vector 'vec' is vec[0]
    if(len(arr)==1):
        return arr
    elif(len(arr)==0):
        return arr
    else:
        pivot = arr[0]
        arr1 = [vec for vec in arr if vec[0] < pivot[0]]
        arr2 = [vec for vec in arr if vec[0] > pivot[0]]
        return sort_by_key(arr1) + [pivot] + sort_by_key(arr2)
    
    

def culc_vectors_arr(file_name1, file_name2):
    #Culculates vectors_arr based on the inner join of file_name1 and file_name2
    df1 = file_to_data_frame(file_name1)
    df2 = file_to_data_frame(file_name2)
    vectors_df = pd.merge(df1, df2, on="key", how = "inner")
    global vectors_arr
    vectors_arr = sort_by_key(vectors_df.values.tolist())

def handle_c_output():
    #Handles the output of the c program
    culc_vectors_arr(file_name1, file_name2)
    culc_initial_centroids()

    print("Initial centroids:")
    print([c[0] for c in initial_centroids])
    print(kmeans.fit.__doc__)
    
    result_centroids = kmeans.fit(vectors_arr, initial_centroids, num_of_clusters, iter_num,
        len(vectors_arr), vector_len, epsilon)
    if result_centroids != None:
        print("Result centroids:")
        print([c[0] for c in result_centroids])
  

    
if __name__ == "__main__":
    args = sys.argv
    file_name1 = ""
    file_name2 = ""

    global vector_len #The number of dimensions of any vector. Initialized in read_vectors
    global vectors_arr #The array of all data_points
    global num_of_clusters #The number of clusters we desire to define
    global epsilon #We will run the kMeans algorithm until the distance between
    #the newly created list of centroids and the previous one is less than epsilon
    global iter_num #Maximal number of iterations of the kmeans algorithm that we do
    global initial_centroids

    initial_centroids = []
    result_centroids = []
    
    if len(args) == 5:
        num_of_clusters = int(args[1])
        epsilon = float(args[2])
        file_name1 = args[3]
        file_name2 = args[4]
        iter_num = 300
        handle_c_output()

        
    elif len(args) == 6:
        num_of_clusters = int(args[1])
        iter_num = int(args[2])
        epsilon = float(args[3])
        file_name1 = args[4]
        file_name2 = args[5]
        handle_c_output()

    else:
        print("An Error Has Occurred")
    
    
